// ============================================================================
// main.cpp 窶・Demo / integration test for the SM-Legends timing engine
//
// Demonstrates the full pipeline: parse a simfile, initialize the conductor,
// and simulate a playback loop printing timing data.
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <string>

#include "parsing/SimfileParser.h"
#include "parsing/Simfile.h"
#include "timing/Conductor.h"
#include "rendering/NoteRenderer.h"

using namespace sml;

static void PrintUsage(const char* program) {
    std::printf("Usage: %s <simfile.sm|simfile.ssc> [chart_index]\n", program);
    std::printf("  Parses the simfile, initializes the conductor, and simulates playback.\n");
}

static void PrintSimfileInfo(const Simfile& simfile) {
    std::printf("=== Simfile Info ===\n");
    std::printf("  Title:    %s\n", simfile.title.c_str());
    std::printf("  Artist:   %s\n", simfile.artist.c_str());
    std::printf("  Music:    %s\n", simfile.music_path.c_str());
    std::printf("  Offset:   %.4f s\n", simfile.offset);
    std::printf("  BPMs:     %zu segment(s)\n", simfile.bpms.size());
    std::printf("  Stops:    %zu segment(s)\n", simfile.stops.size());
    std::printf("  Scrolls:  %zu segment(s)\n", simfile.scrolls.size());
    std::printf("  Charts:   %zu\n", simfile.charts.size());
    std::printf("\n");

    for (size_t i = 0; i < simfile.charts.size(); ++i) {
        const auto& chart = simfile.charts[i];
        std::printf("  [%zu] %s / %s (Meter: %d) - %d notes, %d columns",
            i,
            chart.chart_type.c_str(),
            chart.difficulty_name.c_str(),
            chart.difficulty_meter,
            chart.GetTotalTaps(),
            chart.num_columns
        );
        if (chart.HasOwnTimingData()) {
            std::printf(" [SPLIT TIMING]");
        }
        std::printf("\n");
    }
    std::printf("\n");
}

static void SimulatePlayback(const Simfile& simfile, size_t chart_idx) {
    if (chart_idx >= simfile.charts.size()) {
        std::printf("Error: chart index %zu out of range (0..%zu)\n",
            chart_idx, simfile.charts.size() - 1);
        return;
    }

    const auto& chart = simfile.charts[chart_idx];

    // Initialize conductor with timing data
    Conductor conductor;
    conductor.Initialize(simfile, chart);

    std::printf("=== Conductor Initialized ===\n");
    std::printf("  Time-Beat segments: %zu\n", conductor.GetTimeBeatTable().size());
    std::printf("  Visual segments:    %zu\n", conductor.GetVisualTable().size());
    std::printf("\n");

    // Print the time-beat table
    std::printf("=== Time-Beat Lookup Table ===\n");
    std::printf("  %-12s %-12s %-10s %-12s %-12s\n",
        "StartTime", "StartBeat", "BPM", "EndTime", "EndBeat");
    for (const auto& seg : conductor.GetTimeBeatTable()) {
        std::printf("  %-12.4f %-12.4f %-10.2f %-12.4f %-12.4f\n",
            seg.start_time, seg.start_beat, seg.bpm,
            seg.end_time, seg.end_beat);
    }
    std::printf("\n");

    // Print the visual table
    std::printf("=== Visual Position Table ===\n");
    std::printf("  %-12s %-14s %-12s\n", "Beat", "VisualPos", "ScrollRate");
    for (const auto& seg : conductor.GetVisualTable()) {
        std::printf("  %-12.4f %-14.4f %-12.4f\n",
            seg.start_beat, seg.start_visual_pos, seg.scroll_rate);
    }
    std::printf("\n");

    // Simulate playback at 60 FPS for the first 10 seconds
    std::printf("=== Simulated Playback (60 FPS, 10s) ===\n");
    std::printf("  %-10s %-10s %-10s %-10s %-8s\n",
        "Time", "Beat", "VisPos", "BPM", "InStop");

    NoteFieldConfig config;
    config.receptor_y     = 500.0;
    config.pixels_per_beat = 64.0;
    config.speed_mod      = 2.0;
    config.mod_type       = ScrollModType::XMod;

    constexpr double dt = 1.0 / 60.0;
    for (double t = 0.0; t <= 10.0; t += dt) {
        conductor.Update(t);

        // Print every ~0.5 seconds
        int frame = static_cast<int>(t / dt);
        if (frame % 30 == 0) {
            std::printf("  %-10.4f %-10.4f %-10.4f %-10.2f %-8s\n",
                conductor.GetCurrentTime(),
                conductor.GetCurrentBeat(),
                conductor.GetCurrentVisualPosition(),
                conductor.GetCurrentBPM(),
                conductor.IsInStop() ? "YES" : "no");
        }
    }
    std::printf("\n");

    // Print Y-positions for the first 20 notes
    conductor.Update(0.0); // Reset to start
    size_t note_count = std::min(chart.note_rows.size(), size_t(20));
    if (note_count > 0) {
        std::printf("=== Note Y-Positions (first %zu notes, X-Mod 2.0x) ===\n", note_count);
        std::printf("  %-10s %-12s %-12s\n", "Beat", "Y (XMod)", "Y (CMod)");
        for (size_t i = 0; i < note_count; ++i) {
            double beat = chart.note_rows[i].beat;
            double y_xmod = NoteRenderer::GetYPosForBeat(beat, conductor, config);

            NoteFieldConfig cmod_config = config;
            cmod_config.mod_type  = ScrollModType::CMod;
            cmod_config.speed_mod = 400.0; // 400 pixels/sec
            double y_cmod = NoteRenderer::GetYPosForBeat(beat, conductor, cmod_config);

            std::printf("  %-10.4f %-12.2f %-12.2f\n", beat, y_xmod, y_cmod);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string filepath = argv[1];
    size_t chart_idx = (argc >= 3) ? static_cast<size_t>(std::atoi(argv[2])) : 0;

    // Parse the simfile
    SimfileParser parser;
    auto simfile = parser.LoadFromFile(filepath);

    if (!simfile) {
        std::printf("Error: %s\n", parser.GetLastError().c_str());
        return 1;
    }

    PrintSimfileInfo(*simfile);

    if (!simfile->charts.empty()) {
        SimulatePlayback(*simfile, chart_idx);
    } else {
        std::printf("No charts found in simfile.\n");
    }

    return 0;
}
