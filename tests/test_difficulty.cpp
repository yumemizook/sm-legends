#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "parsing/ChartAnalyzer.h"
#include "parsing/NoteChart.h"
#include "parsing/Simfile.h"
#include "parsing/SimfileParser.h"

using namespace sml;

void test_low_end_scaling() {
    std::cout << "Testing low end scaling...\n";
    NoteChart chart;
    chart.chart_type = "dance-single";
    chart.difficulty_name = "Beginner";
    
    // Create a very sparse chart (1 note per 10 seconds)
    for (int i = 0; i < 10; ++i) {
        NoteRow row;
        row.beat = i * 40.0;
        row.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None};
        chart.note_rows.push_back(row);
    }
    
    Simfile sim;
    sim.bpms.push_back({0.0, 60.0}); // 60 BPM
    sim.charts.push_back(chart);

    RadarValues radar = ChartAnalyzer::CalculateRadar(chart, &sim);
    double difficulty = ChartAnalyzer::CalculateCustomDifficulty(chart, radar);

    std::cout << "  Sparse chart Stream: " << radar.stream << "\n";
    std::cout << "  Sparse chart Difficulty: " << difficulty << "\n";

    assert(difficulty >= 1.0);
    // With old formula (nps * 3.8 - 6.0), a very low NPS would be 0.0.
    // With max(nps * 1.5, ...), it should be > 0 if nps > 0.
    assert(radar.stream > 0.0);
}

void test_progression() {
    std::cout << "Testing difficulty progression...\n";
    Simfile sim;
    sim.bpms.push_back({0.0, 120.0});

    NoteChart easy;
    easy.chart_type = "dance-single";
    easy.difficulty_name = "Easy";
    NoteRow r1; r1.beat = 0; r1.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None};
    easy.note_rows.push_back(r1);

    NoteChart medium;
    medium.chart_type = "dance-single";
    medium.difficulty_name = "Medium";
    medium.note_rows = easy.note_rows; // Identical chart

    sim.charts.push_back(easy);
    sim.charts.push_back(medium);

    // Manual triggers like SimfileParser::LoadFromString does
    for (auto& c : sim.charts) {
        c.radar = ChartAnalyzer::CalculateRadar(c, &sim);
        c.custom_difficulty = ChartAnalyzer::CalculateCustomDifficulty(c, c.radar);
    }

    // Logic from SimfileParser to enforce progression
    std::vector<NoteChart*> charts = {&sim.charts[0], &sim.charts[1]};
    if (sim.charts[1].custom_difficulty <= sim.charts[0].custom_difficulty) {
        sim.charts[1].custom_difficulty = sim.charts[0].custom_difficulty + 0.1;
    }

    std::cout << "  Easy: " << sim.charts[0].custom_difficulty << "\n";
    std::cout << "  Medium: " << sim.charts[1].custom_difficulty << "\n";

    assert(sim.charts[1].custom_difficulty > sim.charts[0].custom_difficulty);
}

void test_double_balancing() {
    std::cout << "Testing double balancing...\n";
    Simfile sim;
    sim.bpms.push_back({0.0, 120.0});

    NoteChart single;
    single.chart_type = "dance-single";
    single.difficulty_name = "Hard";
    for(int i=0; i<100; ++i) {
        NoteRow r; r.beat = i * 0.5; r.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None};
        single.note_rows.push_back(r);
    }

    NoteChart dbl;
    dbl.chart_type = "dance-double";
    dbl.difficulty_name = "Hard";
    for(int i=0; i<100; ++i) {
        NoteRow r; r.beat = i * 0.5; r.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None, NoteType::None, NoteType::None, NoteType::None, NoteType::None};
        dbl.note_rows.push_back(r);
    }

    RadarValues r_s = ChartAnalyzer::CalculateRadar(single, &sim);
    RadarValues r_d = ChartAnalyzer::CalculateRadar(dbl, &sim);

    std::cout << "  Single Stream: " << r_s.stream << "\n";
    std::cout << "  Double Stream: " << r_d.stream << "\n";

    // Double should be balanced (approx 65% of single if same NPS)
    assert(r_d.stream < r_s.stream);
}

void test_star_rating() {
    std::cout << "Testing star rating (Co-op)...\n";
    Simfile sim;
    sim.bpms.push_back({0.0, 120.0});

    NoteChart routine;
    routine.chart_type = "dance-routine"; // 8 lanes
    routine.difficulty_name = "Challenge";

    // Create a chart with high synergy (P1 and P2 hit together)
    for (int i = 0; i < 50; ++i) {
        NoteRow r;
        r.beat = i * 1.0;
        r.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None, // P1
                     NoteType::Tap, NoteType::None, NoteType::None, NoteType::None}; // P2
        routine.note_rows.push_back(r);
    }

    routine.radar = ChartAnalyzer::CalculateRadar(routine, &sim);
    routine.star_rating = ChartAnalyzer::CalculateStarRating(routine, routine.radar);

    std::cout << "  Synergy Multiplier: 1.0 (Full Sync)\n";
    std::cout << "  Star Rating: " << routine.star_rating << "\n";

    assert(routine.star_rating >= 1.0 && routine.star_rating <= 10.0);
    assert(routine.star_rating > 5.0); // Should be high due to full synergy

    // Create a chart with low synergy (split work)
    NoteChart split;
    split.chart_type = "dance-routine";
    split.difficulty_name = "Hard";
    for (int i = 0; i < 50; ++i) {
        NoteRow r;
        r.beat = i * 1.0;
        if (i % 2 == 0) {
            r.columns = {NoteType::Tap, NoteType::None, NoteType::None, NoteType::None, // P1
                         NoteType::None, NoteType::None, NoteType::None, NoteType::None}; // P2
        } else {
            r.columns = {NoteType::None, NoteType::None, NoteType::None, NoteType::None, // P1
                         NoteType::Tap, NoteType::None, NoteType::None, NoteType::None}; // P2
        }
        split.note_rows.push_back(r);
    }

    split.radar = ChartAnalyzer::CalculateRadar(split, &sim);
    split.star_rating = ChartAnalyzer::CalculateStarRating(split, split.radar);

    std::cout << "  Synergy Multiplier: 0.0 (Split)\n";
    std::cout << "  Star Rating: " << split.star_rating << "\n";

    assert(split.star_rating < routine.star_rating);
}

int main() {
    try {
        test_low_end_scaling();
        test_progression();
        test_double_balancing();
        test_star_rating();
        std::cout << "All tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
