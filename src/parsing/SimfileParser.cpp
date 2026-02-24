// ============================================================================
// SimfileParser.cpp 窶・Full .sm and .ssc parser implementation
// ============================================================================

#include "parsing/SimfileParser.h"
#include "parsing/ChartAnalyzer.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

namespace sml {

// ============================================================================
// Public API
// ============================================================================

std::unique_ptr<Simfile> SimfileParser::LoadFromFile(const std::string& filepath) {
    // Read entire file into string
    std::ifstream file(filepath);
    if (!file.is_open()) {
        last_error_ = "Failed to open file: " + filepath;
        return nullptr;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string data = ss.str();

    SimfileFormat format = DetectFormat(filepath);
    if (format == SimfileFormat::Unknown) {
        last_error_ = "Unknown file format: " + filepath;
        return nullptr;
    }

    std::unique_ptr<Simfile> sim = LoadFromString(data, format);
    if (sim) {
        sim->directory = fs::path(filepath).parent_path().string();
    }
    return sim;
}

std::unique_ptr<Simfile> SimfileParser::LoadFromString(
    const std::string& data,
    SimfileFormat format
) {
    auto simfile = std::make_unique<Simfile>();

    // Parse global header tags (metadata + timing)
    ParseGlobalTags(data, *simfile, format);

    // Parse note data (format-specific)
    if (format == SimfileFormat::SM) {
        ParseSMNotes(data, *simfile);
    } else {
        ParseSSCNoteData(data, *simfile);
    }

    // Sort timing segments by beat
    auto sort_by_beat = [](std::vector<TimingSegment>& segs) {
        std::sort(segs.begin(), segs.end());
    };
    sort_by_beat(simfile->bpms);
    sort_by_beat(simfile->stops);
    sort_by_beat(simfile->scrolls);
    sort_by_beat(simfile->speeds);

    // Sort chart timing too
    for (auto& chart : simfile->charts) {
        sort_by_beat(chart.bpms);
        sort_by_beat(chart.stops);
        sort_by_beat(chart.scrolls);
        sort_by_beat(chart.speeds);
    }

    // Analyze charts (Groove Radar, Custom Difficulty, Variants)
    for (auto& chart : simfile->charts) {
        chart.radar = ChartAnalyzer::CalculateRadar(chart, simfile.get());
        chart.custom_difficulty = ChartAnalyzer::CalculateCustomDifficulty(chart, chart.radar);
        
        // --- STAR RATING (CO-OP) ---
        if (chart.chart_type == "dance-couple" || chart.chart_type == "dance-routine") {
            chart.star_rating = ChartAnalyzer::CalculateStarRating(chart, chart.radar);
            // Optionally override difficulty display if we want Stars to be primary
            // chart.custom_difficulty = 0.0; 
        }

        chart.variant = ChartAnalyzer::DetectChartVariant(chart);
    }

    // --- ENFORCE PROGRESSION ---
    // Group by chart type (dance-single, dance-double, etc)
    std::map<std::string, std::vector<NoteChart*>> type_groups;
    for (auto& chart : simfile->charts) {
        type_groups[chart.chart_type].push_back(&chart);
    }

    auto get_diff_rank = [](const std::string& name) {
        std::string n = name;
        for (auto& c : n) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (n == "beginner") return 0;
        if (n == "easy") return 1;
        if (n == "medium") return 2;
        if (n == "hard") return 3;
        if (n == "challenge") return 4;
        return 5; // Edit or other
    };

    for (auto& pair : type_groups) {
        auto& group_charts = pair.second;
        // Sort by difficulty rank
        std::sort(group_charts.begin(), group_charts.end(), [&](NoteChart* a, NoteChart* b) {
            return get_diff_rank(a->difficulty_name) < get_diff_rank(b->difficulty_name);
        });

        // Enforce difficulty[i] >= difficulty[i-1] + 0.1
        for (size_t i = 1; i < group_charts.size(); ++i) {
            int rank_i = get_diff_rank(group_charts[i]->difficulty_name);
            int rank_prev = get_diff_rank(group_charts[i-1]->difficulty_name);
            
            // Only enforce between standard categories if they are in order
            if (rank_i > rank_prev && rank_i < 5) {
                if (group_charts[i]->custom_difficulty < group_charts[i-1]->custom_difficulty + 0.1) {
                    group_charts[i]->custom_difficulty = group_charts[i-1]->custom_difficulty + 0.1;
                }
            }
        }
    }

    return simfile;
}

// ============================================================================
// Format detection
// ============================================================================

SimfileFormat SimfileParser::DetectFormat(const std::string& filepath) {
    size_t dot = filepath.rfind('.');
    if (dot == std::string::npos) return SimfileFormat::Unknown;

    std::string ext = filepath.substr(dot);
    // Lowercase the extension
    for (auto& c : ext) c = static_cast<char>(std::tolower(c));

    if (ext == ".sm")  return SimfileFormat::SM;
    if (ext == ".ssc") return SimfileFormat::SSC;
    return SimfileFormat::Unknown;
}

// ============================================================================
// Global tag parsing
// ============================================================================

void SimfileParser::ParseGlobalTags(
    const std::string& data,
    Simfile& simfile,
    SimfileFormat format
) {
    // --- Metadata ---
    simfile.title              = Trim(ExtractTagValue(data, "TITLE"));
    simfile.subtitle           = Trim(ExtractTagValue(data, "SUBTITLE"));
    simfile.artist             = Trim(ExtractTagValue(data, "ARTIST"));
    simfile.title_translit     = Trim(ExtractTagValue(data, "TITLETRANSLIT"));
    simfile.subtitle_translit  = Trim(ExtractTagValue(data, "SUBTITLETRANSLIT"));
    simfile.artist_translit    = Trim(ExtractTagValue(data, "ARTISTTRANSLIT"));
    simfile.genre              = Trim(ExtractTagValue(data, "GENRE"));
    simfile.credit             = Trim(ExtractTagValue(data, "CREDIT"));
    simfile.banner_path        = Trim(ExtractTagValue(data, "BANNER"));
    simfile.background_path    = Trim(ExtractTagValue(data, "BACKGROUND"));
    simfile.jacket_path        = Trim(ExtractTagValue(data, "JACKET"));
    simfile.cover_path         = Trim(ExtractTagValue(data, "COVER"));
    simfile.cd_title_path      = Trim(ExtractTagValue(data, "CDTITLE"));
    simfile.music_path         = Trim(ExtractTagValue(data, "MUSIC"));

    // --- Audio preview ---
    std::string sample_start = Trim(ExtractTagValue(data, "SAMPLESTART"));
    std::string sample_len   = Trim(ExtractTagValue(data, "SAMPLELENGTH"));
    if (!sample_start.empty()) simfile.sample_start  = std::stod(sample_start);
    if (!sample_len.empty())   simfile.sample_length = std::stod(sample_len);

    // --- Offset ---
    std::string offset_str = Trim(ExtractTagValue(data, "OFFSET"));
    if (!offset_str.empty()) simfile.offset = std::stod(offset_str);

    // --- Global timing segments ---
    std::string bpms_str    = ExtractTagValue(data, "BPMS");
    std::string stops_str   = ExtractTagValue(data, "STOPS");
    std::string scrolls_str = ExtractTagValue(data, "SCROLLS");
    std::string speeds_str  = ExtractTagValue(data, "SPEEDS");
    std::string fakes_str   = ExtractTagValue(data, "FAKES");
    std::string attacks_str = ExtractTagValue(data, "ATTACKS");

    if (!bpms_str.empty())
        simfile.bpms = ParseTimingTag(bpms_str, TimingSegmentType::BPM);
    if (!stops_str.empty())
        simfile.stops = ParseTimingTag(stops_str, TimingSegmentType::Stop);
    if (!scrolls_str.empty())
        simfile.scrolls = ParseTimingTag(scrolls_str, TimingSegmentType::Scroll);
    if (!speeds_str.empty())
        simfile.speeds = ParseSpeedsTag(speeds_str);
    if (!fakes_str.empty())
        simfile.fakes = ParseFakesTag(fakes_str);
    if (!attacks_str.empty())
        simfile.attacks = ParseAttacksTag(attacks_str);

    // --- BG Changes ---
    std::string bgc1 = ExtractTagValue(data, "BGCHANGES");
    std::string bgc2 = ExtractTagValue(data, "BGCHANGES1"); // Some files use this
    if (!bgc1.empty()) {
        auto events = ParseBGChangesTag(bgc1);
        simfile.bg_changes.insert(simfile.bg_changes.end(), events.begin(), events.end());
    }
    if (!bgc2.empty()) {
        auto events = ParseBGChangesTag(bgc2);
        simfile.bg_changes.insert(simfile.bg_changes.end(), events.begin(), events.end());
    }
    std::sort(simfile.bg_changes.begin(), simfile.bg_changes.end());
}

// ============================================================================
// SM note parsing (#NOTES:type:desc:diff:meter:groove:notedata;)
// ============================================================================

void SimfileParser::ParseSMNotes(const std::string& data, Simfile& simfile) {
    // In .sm format, #NOTES blocks have this structure:
    //   #NOTES:
    //        <chart_type>:
    //        <description>:
    //        <difficulty>:
    //        <meter>:
    //        <groove_radar>:
    //        <note_data>
    //   ;

    const std::string tag = "#NOTES:";
    size_t search_pos = 0;

    while (true) {
        size_t pos = data.find(tag, search_pos);
        if (pos == std::string::npos) break;

        pos += tag.length();

        // Find the closing semicolon for this NOTES block
        size_t end_pos = data.find(';', pos);
        if (end_pos == std::string::npos) break;

        std::string block = data.substr(pos, end_pos - pos);
        search_pos = end_pos + 1;

        // Split the block by colons into 6 fields
        std::vector<std::string> fields;
        size_t field_start = 0;
        for (int i = 0; i < 5; ++i) {
            size_t colon = block.find(':', field_start);
            if (colon == std::string::npos) break;
            fields.push_back(Trim(block.substr(field_start, colon - field_start)));
            field_start = colon + 1;
        }
        // The rest is note data
        if (fields.size() < 5) continue;
        fields.push_back(block.substr(field_start));

        NoteChart chart;
        chart.chart_type       = fields[0];
        chart.description      = fields[1];
        chart.difficulty_name  = fields[2];
        chart.difficulty_meter = std::atoi(fields[3].c_str());
        // fields[4] = groove radar (we skip it)
        
        // Extract Credit from Description if usually in format "Desc [Credit]" or similar conventions?
        // Or just usage description is fine?
        // Let's check for brackets []
        size_t b_start = chart.description.find('[');
        size_t b_end = chart.description.find(']');
        if (b_start != std::string::npos && b_end != std::string::npos && b_end > b_start) {
             chart.credit = chart.description.substr(b_start + 1, b_end - b_start - 1);
        } else {
             // Fallback: entire description might be the credit for edits
             chart.credit = chart.description;
        }

        chart.num_columns      = NoteChart::ColumnsForChartType(chart.chart_type);
        chart.note_rows        = ParseNoteData(fields[5], chart.num_columns);

        // SM format: no per-chart timing, always uses global
        chart.has_own_timing = false;

        simfile.charts.push_back(std::move(chart));
    }
}

// ============================================================================
// SSC note data parsing (#NOTEDATA: ... #NOTEDATA: ...)
// ============================================================================

void SimfileParser::ParseSSCNoteData(const std::string& data, Simfile& simfile) {
    // In .ssc format, each chart is a #NOTEDATA: block ending at the next
    // #NOTEDATA: or end-of-file. Tags within the block are per-chart.

    const std::string notedata_tag = "#NOTEDATA:";
    std::vector<size_t> block_starts;

    size_t search_pos = 0;
    while (true) {
        size_t pos = data.find(notedata_tag, search_pos);
        if (pos == std::string::npos) break;
        block_starts.push_back(pos + notedata_tag.length());
        search_pos = pos + notedata_tag.length();
    }

    for (size_t i = 0; i < block_starts.size(); ++i) {
        size_t block_start = block_starts[i];
        size_t block_end = (i + 1 < block_starts.size())
            ? block_starts[i + 1] - notedata_tag.length()
            : data.length();

        std::string block = data.substr(block_start, block_end - block_start);

        NoteChart chart;

        // Parse chart identity tags
        chart.chart_type      = Trim(ExtractTagValue(block, "STEPSTYPE"));
        chart.description     = Trim(ExtractTagValue(block, "DESCRIPTION"));
        chart.difficulty_name = Trim(ExtractTagValue(block, "DIFFICULTY"));
        chart.credit          = Trim(ExtractTagValue(block, "CREDIT"));
        std::string meter_str = Trim(ExtractTagValue(block, "METER"));
        if (!meter_str.empty()) chart.difficulty_meter = std::atoi(meter_str.c_str());
        chart.num_columns = NoteChart::ColumnsForChartType(chart.chart_type);

        // Parse per-chart timing overrides (SSC split timing)
        std::string chart_offset  = Trim(ExtractTagValue(block, "OFFSET"));
        std::string chart_bpms    = ExtractTagValue(block, "BPMS");
        std::string chart_stops   = ExtractTagValue(block, "STOPS");
        std::string chart_scrolls = ExtractTagValue(block, "SCROLLS");
        std::string chart_speeds  = ExtractTagValue(block, "SPEEDS");
        std::string chart_fakes   = ExtractTagValue(block, "FAKES");
        std::string chart_attacks = ExtractTagValue(block, "ATTACKS");

        if (!chart_offset.empty()) {
            chart.offset = std::stod(chart_offset);
            chart.has_own_timing = true;
        }
        if (!chart_bpms.empty()) {
            chart.bpms = ParseTimingTag(chart_bpms, TimingSegmentType::BPM);
            chart.has_own_timing = true;
        }
        if (!chart_stops.empty()) {
            chart.stops = ParseTimingTag(chart_stops, TimingSegmentType::Stop);
            chart.has_own_timing = true;
        }
        if (!chart_scrolls.empty()) {
            chart.scrolls = ParseTimingTag(chart_scrolls, TimingSegmentType::Scroll);
            chart.has_own_timing = true;
        }
        if (!chart_speeds.empty()) {
            chart.speeds = ParseSpeedsTag(chart_speeds);
            chart.has_own_timing = true;
        }
        if (!chart_fakes.empty()) {
            chart.fakes = ParseFakesTag(chart_fakes);
            chart.has_own_timing = true;
        }
        if (!chart_attacks.empty()) {
            chart.attacks = ParseAttacksTag(chart_attacks);
            chart.has_own_timing = true;
        }

        // Parse note data
        std::string notes_str = ExtractTagValue(block, "NOTES");
        if (!notes_str.empty()) {
            chart.note_rows = ParseNoteData(notes_str, chart.num_columns);
        }

        simfile.charts.push_back(std::move(chart));
    }
}

// ============================================================================
// Timing tag parsing
// ============================================================================

std::vector<TimingSegment> SimfileParser::ParseTimingTag(
    const std::string& value,
    TimingSegmentType type
) {
    // Format: "beat=value,beat=value,..."
    std::vector<TimingSegment> segments;
    std::string trimmed = Trim(value);
    if (trimmed.empty()) return segments;

    std::istringstream stream(trimmed);
    std::string pair;

    while (std::getline(stream, pair, ',')) {
        pair = Trim(pair);
        if (pair.empty()) continue;

        size_t eq = pair.find('=');
        if (eq == std::string::npos) continue;

        TimingSegment seg;
        seg.type = type;
        seg.start_beat = std::stod(pair.substr(0, eq));
        seg.value      = std::stod(pair.substr(eq + 1));
        segments.push_back(seg);
    }

    return segments;
}

std::vector<TimingSegment> SimfileParser::ParseSpeedsTag(const std::string& value) {
    // Format: "beat=ratio=duration=unit,..."
    // unit: 0 = beats, 1 = seconds
    std::vector<TimingSegment> segments;
    std::string trimmed = Trim(value);
    if (trimmed.empty()) return segments;

    std::istringstream stream(trimmed);
    std::string entry;

    while (std::getline(stream, entry, ',')) {
        entry = Trim(entry);
        if (entry.empty()) continue;

        // Split by '='
        std::vector<std::string> parts;
        std::istringstream es(entry);
        std::string part;
        while (std::getline(es, part, '=')) {
            parts.push_back(Trim(part));
        }

        if (parts.size() < 4) continue;

        TimingSegment seg;
        seg.type       = TimingSegmentType::Speed;
        seg.start_beat = std::stod(parts[0]);
        seg.value      = std::stod(parts[1]);   // ratio
        seg.duration   = std::stod(parts[2]);    // transition length
        seg.unit       = std::stoi(parts[3]);    // 0=beats, 1=seconds
        segments.push_back(seg);
    }

    return segments;
}

std::vector<FakeSegment> SimfileParser::ParseFakesTag(const std::string& value) {
    // Format: "beat=length,beat=length,..."
    std::vector<FakeSegment> segments;
    std::string trimmed = Trim(value);
    if (trimmed.empty()) return segments;

    std::istringstream stream(trimmed);
    std::string entry;

    while (std::getline(stream, entry, ',')) {
        entry = Trim(entry);
        if (entry.empty()) continue;

        // Split by '='
        size_t eq = entry.find('=');
        if (eq == std::string::npos) continue;

        FakeSegment seg;
        seg.start_beat   = std::stod(entry.substr(0, eq));
        seg.length_beats = std::stod(entry.substr(eq + 1));
        segments.push_back(seg);
    }

    return segments;
}

std::vector<Attack> SimfileParser::ParseAttacksTag(const std::string& value) {
    // SM5 Format: entries separated by ':'
    // Each entry has key=value pairs separated by ':'
    // e.g. "TIME=1.5:LEN=2.0:MODS=*0.5 xmod,TIME=5.0:LEN=3.0:MODS=drunk"
    // Or colon-delimited groups: "TIME=1.5:LEN=2.0:MODS=*0.5 xmod:END"
    std::vector<Attack> attacks;
    std::string trimmed = Trim(value);
    if (trimmed.empty()) return attacks;

    // Split by ':'
    std::vector<std::string> tokens;
    {
        std::istringstream stream(trimmed);
        std::string token;
        while (std::getline(stream, token, ':')) {
            tokens.push_back(Trim(token));
        }
    }

    // Walk tokens, building attacks from TIME/LEN/MODS groups
    Attack current;
    bool has_time = false;

    for (const auto& token : tokens) {
        if (token.empty() || token == "END") {
            if (has_time) {
                attacks.push_back(current);
                current = Attack{};
                has_time = false;
            }
            continue;
        }

        size_t eq = token.find('=');
        if (eq == std::string::npos) continue;

        std::string key = token.substr(0, eq);
        std::string val = token.substr(eq + 1);

        // Uppercase key for comparison
        for (auto& c : key) c = static_cast<char>(std::toupper(c));

        if (key == "TIME") {
            // If we had a previous incomplete attack, push it
            if (has_time) {
                attacks.push_back(current);
                current = Attack{};
            }
            current.start_time = std::stod(val);
            has_time = true;
        } else if (key == "LEN" || key == "LENGTH") {
            current.length = std::stod(val);
        } else if (key == "MODS" || key == "MOD") {
            current.mods = val;
        }
    }

    // Push the last attack if we have one
    if (has_time) {
        attacks.push_back(current);
    }

    return attacks;
}

std::vector<BGEvent> SimfileParser::ParseBGChangesTag(const std::string& value) {
    // Format: "beat=file=rate=transition=...,beat=file=rate=transition=..."
    std::vector<BGEvent> events;
    std::string trimmed = Trim(value);
    if (trimmed.empty()) return events;

    std::istringstream stream(trimmed);
    std::string entry;

    while (std::getline(stream, entry, ',')) {
        entry = Trim(entry);
        if (entry.empty()) continue;

        // Split by '='
        std::vector<std::string> parts;
        {
            std::istringstream es(entry);
            std::string part;
            while (std::getline(es, part, '=')) {
                parts.push_back(Trim(part));
            }
        }

        if (parts.size() < 2) continue;

        BGEvent ev;
        try {
            ev.beat = std::stod(parts[0]);
            ev.file = parts[1];
            if (parts.size() >= 3 && !parts[2].empty()) ev.rate = std::stod(parts[2]);
            if (parts.size() >= 4 && !parts[3].empty()) ev.transition = std::stoi(parts[3]);
            events.push_back(ev);
        } catch (...) {
            // Skip malformed entries
        }
    }

    return events;
}

// ============================================================================
// Note data parsing
// ============================================================================

std::vector<NoteRow> SimfileParser::ParseNoteData(
    const std::string& note_text,
    int num_columns
) {
    std::vector<NoteRow> rows;

    // Split into measures by comma
    std::vector<std::string> measures;
    {
        std::istringstream stream(note_text);
        std::string measure;
        while (std::getline(stream, measure, ',')) {
            measures.push_back(measure);
        }
    }

    for (size_t measure_idx = 0; measure_idx < measures.size(); ++measure_idx) {
        // Split measure into lines, filtering out empty/comment lines
        std::vector<std::string> lines;
        {
            std::istringstream stream(measures[measure_idx]);
            std::string line;
            while (std::getline(stream, line)) {
                line = Trim(line);
                // Skip empty lines and comment lines
                if (line.empty() || line[0] == '/' || line[0] == '-') continue;
                // Only keep lines that look like note data (start with 0-9 or M/L/F)
                if (line.length() >= static_cast<size_t>(num_columns)) {
                    bool is_note_line = true;
                    for (int c = 0; c < num_columns && is_note_line; ++c) {
                        char ch = line[static_cast<size_t>(c)];
                        if (ch != '0' && ch != '1' && ch != '2' && ch != '3' &&
                            ch != '4' && ch != 'M' && ch != 'L' && ch != 'F') {
                            is_note_line = false;
                        }
                    }
                    if (is_note_line) lines.push_back(line);
                }
            }
        }

        if (lines.empty()) continue;

        // Each line in the measure represents an equal subdivision.
        // A measure with N lines means each line = 1/N of a measure = 4/N beats.
        double beats_per_line = 4.0 / static_cast<double>(lines.size());

        for (size_t line_idx = 0; line_idx < lines.size(); ++line_idx) {
            const std::string& line = lines[line_idx];

            NoteRow row;
            row.beat = static_cast<double>(measure_idx) * 4.0
                     + static_cast<double>(line_idx) * beats_per_line;

            row.columns.resize(static_cast<size_t>(num_columns), NoteType::None);

            for (int c = 0; c < num_columns && c < static_cast<int>(line.length()); ++c) {
                char ch = line[static_cast<size_t>(c)];
                switch (ch) {
                    case '0': row.columns[static_cast<size_t>(c)] = NoteType::None;     break;
                    case '1': row.columns[static_cast<size_t>(c)] = NoteType::Tap;      break;
                    case '2': row.columns[static_cast<size_t>(c)] = NoteType::HoldHead; break;
                    case '3': row.columns[static_cast<size_t>(c)] = NoteType::HoldTail; break;
                    case '4': row.columns[static_cast<size_t>(c)] = NoteType::RollHead; break;
                    case 'M': row.columns[static_cast<size_t>(c)] = NoteType::Mine;     break;
                    case 'L': row.columns[static_cast<size_t>(c)] = NoteType::Lift;     break;
                    case 'F': row.columns[static_cast<size_t>(c)] = NoteType::Fake;     break;
                    default:  row.columns[static_cast<size_t>(c)] = NoteType::None;     break;
                }
            }

            // Only store rows that contain at least one note
            if (row.HasNotes()) {
                rows.push_back(std::move(row));
            }
        }
    }

    return rows;
}

// ============================================================================
// Tag extraction utility
// ============================================================================

std::string SimfileParser::ExtractTagValue(
    const std::string& data,
    const std::string& tag,
    size_t search_start
) {
    // Look for #TAG: pattern
    std::string needle = "#" + tag + ":";
    size_t pos = data.find(needle, search_start);
    if (pos == std::string::npos) {
        // Try case-insensitive search
        std::string data_upper = data;
        std::string needle_upper = needle;
        for (auto& c : data_upper) c = static_cast<char>(std::toupper(c));
        for (auto& c : needle_upper) c = static_cast<char>(std::toupper(c));
        pos = data_upper.find(needle_upper, search_start);
        if (pos == std::string::npos) return "";
    }

    pos += needle.length();

    // Find the closing semicolon
    size_t end = data.find(';', pos);
    if (end == std::string::npos) {
        // No semicolon found - take to end of string (lenient parsing)
        return data.substr(pos);
    }

    return data.substr(pos, end - pos);
}

// ============================================================================
// String utilities
// ============================================================================

std::string SimfileParser::Trim(const std::string& s) {
    size_t start = 0;
    size_t end = s.length();

    while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;

    return s.substr(start, end - start);
}

std::string SimfileParser::FindJacket(const std::string& directory, const std::vector<std::string>& metadata_hints) {
    if (directory.empty()) return "";

    // 1. Check strong metadata hints (#JACKET, #COVER)
    // We assume hints[0] and hints[1] are jacket and cover respectively
    for (size_t i = 0; i < std::min<size_t>(2, metadata_hints.size()); ++i) {
        if (metadata_hints[i].empty()) continue;
        fs::path p = fs::path(directory) / metadata_hints[i];
        if (fs::exists(p) && fs::is_regular_file(p)) return p.string();
    }

    // 2. Look for any file containing "-jacket" (Highly Specific Pattern)
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string lower_filename = filename;
                for (auto& c : lower_filename) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                
                if (lower_filename.find("-jacket") != std::string::npos) {
                    std::string ext = entry.path().extension().string();
                    for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif") 
                        return entry.path().string();
                }
            }
        }
    } catch (...) {}

    // 3. Look for standard "jacket" or "cover" filenames
    static const std::vector<std::string> high_priorities = {
        "jacket.png", "jacket.jpg", "jacket.jpeg", "jacket.bmp", "jacket.gif",
        "cover.png",  "cover.jpg",  "cover.jpeg",  "cover.bmp",  "cover.gif"
    };
    for (const auto& name : high_priorities) {
        fs::path p = fs::path(directory) / name;
        if (fs::exists(p) && fs::is_regular_file(p)) return p.string();
    }

    // 4. Fallback to generic metadata hints (#BANNER)
    if (metadata_hints.size() > 2 && !metadata_hints[2].empty()) {
        fs::path p = fs::path(directory) / metadata_hints[2];
        if (fs::exists(p) && fs::is_regular_file(p)) return p.string();
    }

    // 5. Look for generic filenames (banner, bg)
    static const std::vector<std::string> fallbacks = {
        "banner.png", "banner.jpg", "banner.jpeg", "banner.bmp", "banner.gif",
        "bg.png",     "bg.jpg",     "bg.jpeg",     "bg.bmp"
    };
    for (const auto& name : fallbacks) {
        fs::path p = fs::path(directory) / name;
        if (fs::exists(p) && fs::is_regular_file(p)) return p.string();
    }

    // 6. Last resort: first image file in directory
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") return entry.path().string();
            }
        }
    } catch (...) {}

    return "";
}

} // namespace sml
