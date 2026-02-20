#pragma once
// ============================================================================
// NoteRenderer.h 窶・Note Y-position calculation for VSRG rendering
//
// Translates beat positions to screen coordinates using the Conductor's
// timing data. Supports X-Mod (BPM-relative) and C-Mod (constant speed).
// ============================================================================

#include "timing/Conductor.h"
#include "parsing/NoteChart.h"

namespace sml {

/// Configuration for the note field renderer.
struct NoteFieldConfig {
    double receptor_y      = 500.0;   ///< Y-coordinate of the receptor/hit line
    double pixels_per_beat  = 64.0;    ///< Base pixels per beat (X-Mod)
    double speed_mod       = 1.0;     ///< Speed multiplier (X-Mod) or pixels/sec (C-Mod)
    ScrollModType mod_type = ScrollModType::XMod;
    bool   downscroll      = false;   ///< True = notes fall downward (receptor at bottom)
    double reverse_pct     = 0.0;     ///< Smooth interpolation for Reverse attack (0.0 to 1.0)
    double screen_height   = 480.0;   ///< Reference height for speed scaling
};

/// Handles calculation of note screen positions.
///
/// This is intentionally a stateless utility class 窶・all state lives in the
/// Conductor. The renderer simply queries the Conductor each frame to compute
/// note Y-coordinates.
class NoteRenderer {
public:
    NoteRenderer() = default;

    /// Calculate the Y-coordinate for a note at the given beat.
    ///
    /// @param target_beat  Beat of the note
    /// @param conductor    Current conductor state (must be updated this frame)
    /// @param config       Note field rendering configuration
    /// @return Y-coordinate in screen pixels
    [[nodiscard]] static double GetYPosForBeat(
        double target_beat,
        const Conductor& conductor,
        const NoteFieldConfig& config
    );

    /// Calculate Y-coordinate using pre-calculated row timing data.
    [[nodiscard]] static double GetYPosForRow(
        const NoteRow& row,
        const Conductor& conductor,
        const NoteFieldConfig& config
    );

    /// Determine which notes are currently visible on screen.
    /// Returns the index range [first, last) of note_rows that are on screen.
    ///
    /// @param chart       The note chart
    /// @param conductor   Current conductor state
    /// @param config      Note field config
    /// @param screen_height Total screen height in pixels
    /// @param margin      Extra margin in beats to draw off-screen (for smoothness)
    /// @return Pair of (first_visible_index, past_last_visible_index)
    [[nodiscard]] static std::pair<size_t, size_t> GetVisibleNoteRange(
        const NoteChart& chart,
        const Conductor& conductor,
        const NoteFieldConfig& config,
        double screen_height,
        double margin_beats = 2.0
    );
};

} // namespace sml
