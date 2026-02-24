#include "parsing/ChartAnalyzer.h"
#include "timing/Conductor.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <iostream>
#include <numeric>
#include <vector>

namespace sml {

// Helper to determine if a character is a Kanji (CJK Unified Ideograph)
// Basic range: 0x4E00 - 0x9FFF
static bool IsKanji(char32_t c) {
    return (c >= 0x4E00 && c <= 0x9FFF);
}

static std::string ExtractFirstKanji(const std::string& str) {
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 0x80) {
            i++;
        } else if ((c & 0xE0) == 0xC0) { // 2 bytes
            if (i + 1 >= str.length()) break;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 bytes (most CJK)
            if (i + 2 >= str.length()) break;
            uint32_t cp = ((c & 0x0F) << 12) | 
                          ((str[i+1] & 0x3F) << 6) | 
                          (str[i+2] & 0x3F);
            if (IsKanji(cp)) return str.substr(i, 3);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 bytes
            if (i + 3 >= str.length()) break;
            i += 4;
        } else {
            i++; // Invalid or other
        }
    }
    return "";
}

static bool ContainsKanji(const std::string& str) {
    return !ExtractFirstKanji(str).empty();
}

RadarValues ChartAnalyzer::CalculateRadar(const NoteChart& chart, const Simfile* simfile) {
    RadarValues radar;
    if (chart.note_rows.empty() || !simfile) return radar;

    // Use Conductor to get accurate timing
    Conductor conductor;
    conductor.Initialize(*simfile, chart);

    double first_beat = chart.note_rows.front().beat;
    double last_beat = chart.GetLastBeat();
    double song_duration = conductor.BeatToTime(last_beat) - conductor.BeatToTime(first_beat);
    if (song_duration < 1.0) song_duration = 1.0;

    // 1. Analyze Intervals for Stream/Voltage
    // We'll calculate localized NPS (Notes Per Second) in 1-second windows
    std::vector<double> window_nps;
    double window_size = 1.0; // seconds
    double max_nps = 0.0;
    
    // Iterate time from start to end
    double start_time = conductor.BeatToTime(first_beat);
    double end_time = conductor.BeatToTime(last_beat);
    
    // Create a time-sorted map of notes for easier windowing?
    // Or just iterate rows.
    // Let's iterate chunks.
    
    // Optimization: flattened note times
    std::vector<double> note_times;
    note_times.reserve(chart.GetTotalTaps());
    
    std::vector<double> jump_times;

    int total_holds = 0;
    double total_hold_seconds = 0.0;
    int notes_not_4th_8th = 0;

    for (const auto& row : chart.note_rows) {
        double t = conductor.BeatToTime(row.beat);
        int taps_on_row = 0;
        bool has_hold = false;
        
        for (auto col : row.columns) {
            if (IsTap(col)) {
                note_times.push_back(t);
                taps_on_row++;
            }
            if (col == NoteType::HoldHead || col == NoteType::RollHead) {
                has_hold = true;
                total_holds++;
                // Approximate hold length? 
                // We'd need to find the tail. 
                // For simplified analyzer, we can skip precise hold length or scan ahead.
                // Scan ahead is expensive O(N^2) if naive. 
                // Let's just count hold heads for now or try to find tails if easy.
                // Assuming well-formed chart, finding tail in same column is O(N).
            }
        }
        
        if (taps_on_row >= 2) {
            jump_times.push_back(t);
        }

        // Chaos Check (quantization)
        // 4th: 0.0, 1.0... 
        // 8th: 0.5...
        // 12th: 0.33, 0.66
        // 16th: 0.25, 0.75
        double b = row.beat;
        double frac = b - std::floor(b);
        const double EPS = 0.001;
        
        // Check 4th, 8th, 12th, 16th
        // 12th = frac * 3 is integer
        bool is_simple = false;
        if (std::abs(frac * 4 - std::round(frac * 4)) < EPS) is_simple = true;
        else if (std::abs(frac * 3 - std::round(frac * 3)) < EPS) is_simple = true;
        
        if (!is_simple) notes_not_4th_8th++;
    }
    
    // Calculate Windowed NPS
    for (double t = start_time; t < end_time; t += 0.5) { // 0.5s step
        double w_start = t;
        double w_end = t + 1.0;
        int count = 0;
        for (double nt : note_times) {
            if (nt >= w_start && nt < w_end) count++;
        }
        double nps = count; // count / 1.0s
        window_nps.push_back(nps);
        if (nps > max_nps) max_nps = nps;
    }
    
    // --- STREAM ---
    // Avg NPS of "active" windows (where NPS > 20% of peak or > 2?)
    double stream_sum = 0;
    int stream_count = 0;
    for (double nps : window_nps) {
        if (nps > max_nps * 0.5 || nps > 4.0) { // Only count busy sections
            stream_sum += nps;
            stream_count++;
        }
    }
    double avg_stream_nps = stream_count > 0 ? (stream_sum / stream_count) : 0.0;
    
    // Double Balancing Factor
    double double_factor = chart.Is8Lane() ? 0.65 : 1.0;

    // --- STREAM ---
    radar.stream = std::max(avg_stream_nps * 1.5, avg_stream_nps * 3.8 - 6.0) * double_factor;

    // --- VOLTAGE ---
    radar.voltage = std::max(max_nps * 1.0, max_nps * 1.8 - 4.0) * double_factor;

    // --- AIR ---
    double jps = jump_times.size() / song_duration;
    radar.air = std::max(jps * 5.0, jps * 15.0 - 5.0) * double_factor;

    // --- FREEZE ---
    double hps = (double)total_holds / song_duration;
    radar.freeze = std::max(hps * 10.0, hps * 30.0 - 5.0) * (chart.Is8Lane() ? 0.8 : 1.0); // Holds are easier on double? slightly less reduction

    // --- CHAOS ---
    double chaos_ratio = 0.0;
    if (!note_times.empty()) chaos_ratio = (double)notes_not_4th_8th / note_times.size();
    radar.chaos = std::max(chaos_ratio * 25.0, chaos_ratio * 75.0 - 10.0); // No double factor for rhythm complexity

    // Clamp all to 0-30 (+epsilon visually)
    // Actually user wants 1-30 integers.
    // We store as double, but Logic will clamp later?
    // Let's clamp here to be safe, max 30.
    // But keep doubles for "over 30" possibilities for boss songs?
    // Just clamp 0-30 for now.
    
    return radar; // Values are now roughly 0..30
}

double ChartAnalyzer::CalculateCustomDifficulty(const NoteChart& chart, const RadarValues& radar) {
    // Weighted aggregation
    // Difficulty is dominated by the hardest aspect usually.
    
    double max_val = std::max({radar.stream, radar.voltage, radar.air, radar.freeze, radar.chaos});
    double avg_val = (radar.stream + radar.voltage + radar.air + radar.freeze + radar.chaos) / 5.0;
    
    // --- HIGH END BONUSES ---
    // 1. Technicality Multiplier
    double tech_bonus = 0.0;
    if (radar.chaos > 15.0) tech_bonus += (radar.chaos - 15.0) * 0.1; // Extra 0.1 per point above 15
    if (radar.air > 20.0) tech_bonus += (radar.air - 20.0) * 0.05;   // Extra 0.05 per point above 20
    
    // 2. Endurance Bonus
    if (radar.stream > 20.0 && radar.voltage > 15.0) {
        tech_bonus += 0.5; // Flat bonus for heavy stream + peaks
    }

    // Formula: 90% Max + 10% Avg + bonus
    double rating = max_val * 0.90 + avg_val * 0.10 + tech_bonus;
    
    // Round to 1 decimal place
    double r = std::round(rating * 10.0) / 10.0;
    
    if (r < 1.0) r = 1.0;
    if (r > 30.4) r = 30.4;
    return r;
}

double ChartAnalyzer::CalculateStarRating(const NoteChart& chart, const RadarValues& radar) {
    if (!chart.Is8Lane()) return 0.0;

    // 1. Base Density (Normalized NPS for 2 players)
    // radar.stream and radar.voltage already have a 0.65 factor for double.
    // Let's use their raw contribution to stars.
    double density_stars = (radar.stream * 0.2) + (radar.voltage * 0.1); // Max ~6 stars from pure density if metrics are 20-30
    
    // 2. Synergy Factor (Sync Hits)
    // How many rows have notes for BOTH players?
    int sync_rows = 0;
    int total_rows = (int)chart.note_rows.size();
    
    for (const auto& row : chart.note_rows) {
        bool p1 = false;
        bool p2 = false;
        // P1: lanes 0-3, P2: lanes 4-7
        for (int i = 0; i < 4 && i < (int)row.columns.size(); ++i) {
            if (row.columns[i] != NoteType::None) p1 = true;
        }
        for (int i = 4; i < 8 && i < (int)row.columns.size(); ++i) {
            if (row.columns[i] != NoteType::None) p2 = true;
        }
        if (p1 && p2) sync_rows++;
    }
    
    double synergy_factor = total_rows > 0 ? (double)sync_rows / total_rows : 0.0;
    double synergy_stars = synergy_factor * 5.0; // Up to 5 stars for full synchronization
    
    // 3. Chaos contribution
    double chaos_stars = radar.chaos * 0.1; // Max ~2-3 stars
    
    // Final Aggregation
    double total = density_stars + synergy_stars + chaos_stars;
    
    // Normalization and Clamping
    // We want 1.0 to 10.0 range.
    double r = std::round(total * 2.0) / 2.0; // Round to 0.5
    
    if (r < 1.0) r = 1.0;
    if (r > 10.0) r = 10.0;
    
    return r;
}

ChartVariant ChartAnalyzer::DetectChartVariant(NoteChart& chart) {
    std::string desc = chart.description;
    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
    std::string diff = chart.difficulty_name;
    std::transform(diff.begin(), diff.end(), diff.begin(), ::tolower);
    std::string cred = chart.credit;
    // Don't lower credit initially to preserve Kanji

    // 1. Wild
    std::string kanji = ExtractFirstKanji(chart.description);
    if (kanji.empty()) kanji = ExtractFirstKanji(chart.credit);

    if (!kanji.empty() || desc.find("wild") != std::string::npos || diff.find("wild") != std::string::npos) {
        chart.variant_kanji = kanji;
        return ChartVariant::Wild;
    }

    // 2. Beginner+ / Challenge+ / etc.
    if (desc.find("beginner+") != std::string::npos || diff.find("beginner+") != std::string::npos) return ChartVariant::BeginnerPlus;
    if (desc.find("easy+") != std::string::npos || diff.find("easy+") != std::string::npos) return ChartVariant::EasyPlus;
    if (desc.find("medium+") != std::string::npos || diff.find("medium+") != std::string::npos) return ChartVariant::MediumPlus;
    if (desc.find("hard+") != std::string::npos || diff.find("hard+") != std::string::npos) return ChartVariant::HardPlus;
    if (desc.find("challenge+") != std::string::npos || diff.find("challenge+") != std::string::npos) return ChartVariant::ChallengePlus;

    return ChartVariant::Normal;
}

} // namespace sml
