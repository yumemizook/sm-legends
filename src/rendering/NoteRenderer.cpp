// ============================================================================
// NoteRenderer.cpp 窶・Note position calculation
// ============================================================================

#include "rendering/NoteRenderer.h"

#include <algorithm>
#include <cmath>

namespace sml {

double NoteRenderer::GetYPosForBeat(
    double target_beat,
    const Conductor& conductor,
    const NoteFieldConfig& config
) {
    // Scaling factor based on 480px reference height
    double scale = config.screen_height / 480.0;

    double y = conductor.GetYPosForBeat(
        target_beat,
        config.mod_type,
        config.speed_mod * scale, // Scale speed mod for consistent visual density
        config.receptor_y,
        config.pixels_per_beat
    );

    double delta = config.receptor_y - y;
    double scroll_dir = config.downscroll ? 1.0 : -1.0;
    
    // Reverse attack interpolates the scroll direction
    // If downscroll: 1.0 (reverse=0) -> -1.0 (reverse=1)
    // If upscroll: -1.0 (reverse=0) -> 1.0 (reverse=1)
    if (config.downscroll) {
        scroll_dir = 1.0 - 2.0 * config.reverse_pct;
    } else {
        scroll_dir = -1.0 + 2.0 * config.reverse_pct;
    }

    return config.receptor_y - delta * scroll_dir;
}

double NoteRenderer::GetYPosForRow(
    const NoteRow& row,
    const Conductor& conductor,
    const NoteFieldConfig& config
) {
    double scale = config.screen_height / 480.0;
    double y;

    if (config.mod_type == ScrollModType::XMod) {
        y = conductor.GetYPosForVisualPos(row.visual_pos, config.speed_mod * scale, config.receptor_y, config.pixels_per_beat);
    } else {
        y = conductor.GetYPosForTime(row.time, config.speed_mod, config.receptor_y);
    }

    double delta = config.receptor_y - y;
    double scroll_dir = config.downscroll ? 1.0 : -1.0;
    
    if (config.downscroll) {
        scroll_dir = 1.0 - 2.0 * config.reverse_pct;
    } else {
        scroll_dir = -1.0 + 2.0 * config.reverse_pct;
    }

    return config.receptor_y - delta * scroll_dir;
}

std::pair<size_t, size_t> NoteRenderer::GetVisibleNoteRange(
    const NoteChart& chart,
    const Conductor& conductor,
    const NoteFieldConfig& config,
    double screen_height,
    double margin_beats
) {
    if (chart.note_rows.empty()) return {0, 0};

    double current_beat = conductor.GetCurrentBeat();

    // Estimate the visible beat range based on scroll speed.
    // For X-Mod: visible_range 竕・screen_height / (pixels_per_beat * speed_mod)
    // For C-Mod: we need to estimate based on BPM, but we use a generous range
    double visible_beats;
    if (config.mod_type == ScrollModType::XMod) {
        double effective_ppb = config.pixels_per_beat * config.speed_mod;
        if (effective_ppb <= 0.0) effective_ppb = 1.0;
        visible_beats = screen_height / effective_ppb;
    } else {
        // C-Mod: estimate visible time window, convert to beats generously
        double bpm = conductor.GetCurrentBPM();
        if (bpm <= 0.0) bpm = 120.0;
        double visible_seconds = screen_height / config.speed_mod;
        visible_beats = visible_seconds * (bpm / 60.0);
    }

    double min_beat = current_beat - margin_beats;
    double max_beat = current_beat + visible_beats + margin_beats;

    // Binary search for first note >= min_beat
    auto first_it = std::lower_bound(
        chart.note_rows.begin(),
        chart.note_rows.end(),
        min_beat,
        [](const NoteRow& row, double beat) { return row.beat < beat; }
    );

    // Binary search for first note > max_beat
    auto last_it = std::upper_bound(
        chart.note_rows.begin(),
        chart.note_rows.end(),
        max_beat,
        [](double beat, const NoteRow& row) { return beat < row.beat; }
    );

    size_t first = static_cast<size_t>(first_it - chart.note_rows.begin());
    size_t last  = static_cast<size_t>(last_it - chart.note_rows.begin());

    return {first, last};
}

} // namespace sml
