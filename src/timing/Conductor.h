#pragma once
// ============================================================================
// Conductor.h 窶・The timing heart of the VSRG engine
//
// Pre-computes piecewise-linear lookup tables from timing events and provides
// O(log n) conversion between audio time, song beat, and visual position.
// Does NOT use the naive "beat = time * bpm" formula.
// ============================================================================

#include <vector>
#include "timing/TimingData.h"

namespace sml {

// Forward declaration
struct Simfile;
struct NoteChart;

/// The Conductor manages all time <-> beat <-> visual position conversions.
///
/// Usage:
///   Conductor conductor;
///   conductor.Initialize(simfile, chart);
///   // In your game loop:
///   conductor.Update(audioEngine.GetDSPTime());
///   double currentBeat = conductor.GetCurrentBeat();
///   double yPos = conductor.GetYPosForBeat(noteBeat, scrollMod, scrollSpeed);
class Conductor {
public:
    Conductor() = default;

    // ========================================================================
    // Initialization
    // ========================================================================

    /// Initialize the conductor with timing data from a simfile and chart.
    /// This pre-computes all lookup tables for O(log n) runtime queries.
    /// Respects SSC split timing: uses chart-level timing if available.
    void Initialize(const Simfile& simfile, const NoteChart& chart);

    /// Initialize with raw timing data directly (for testing).
    void Initialize(
        const std::vector<TimingSegment>& bpms,
        const std::vector<TimingSegment>& stops,
        const std::vector<TimingSegment>& scrolls,
        double offset
    );

    // ========================================================================
    // Runtime update
    // ========================================================================

    /// Update the conductor with the current audio playback time (seconds).
    /// This should be called every frame with precise DSP time, NOT frame time.
    /// Computes current_beat_ and current_visual_pos_ for use by the renderer.
    void Update(double audio_time);

    // ========================================================================
    // Time / Beat / Visual conversion queries
    // ========================================================================

    /// Convert an audio time (seconds) to a song beat. O(log n).
    [[nodiscard]] double TimeToBeat(double time) const;

    /// Convert a song beat to audio time (seconds). O(log n).
    [[nodiscard]] double BeatToTime(double beat) const;

    /// Convert a song beat to a visual scroll position. O(log n).
    /// Visual position accounts for #SCROLLS segments.
    [[nodiscard]] double BeatToVisualPosition(double beat) const;

    /// Get the Y-coordinate for a note at the given beat.
    ///
    /// @param target_beat  The beat of the note to position
    /// @param mod_type     XMod (BPM-relative) or CMod (constant speed)
    /// @param speed_mod    Speed multiplier (e.g., 2.0x for X-Mod, or pixels/sec for C-Mod)
    /// @param receptor_y   Y-coordinate of the receptor/hit line (pixels)
    /// @param pixels_per_beat Base pixels per beat for X-Mod (typically ~64)
    /// @return Y-coordinate in screen space (lower Y = higher on screen)
    [[nodiscard]] double GetYPosForBeat(
        double target_beat,
        ScrollModType mod_type,
        double speed_mod,
        double receptor_y,
        double pixels_per_beat = 64.0
    ) const;

    // ========================================================================
    // Accessors
    // ========================================================================

    /// Current audio time as set by the last Update() call.
    [[nodiscard]] double GetCurrentTime() const { return current_time_; }

    /// Current song beat (interpolated from current audio time).
    [[nodiscard]] double GetCurrentBeat() const { return current_beat_; }

    /// Current visual position (accounts for scroll segments).
    [[nodiscard]] double GetCurrentVisualPosition() const { return current_visual_pos_; }

    /// Current BPM at the current time.
    [[nodiscard]] double GetCurrentBPM() const { return current_bpm_; }

    /// Whether the conductor is initialized and ready.
    [[nodiscard]] bool IsInitialized() const { return initialized_; }

    /// Whether the song is currently in a stop/freeze.
    [[nodiscard]] bool IsInStop() const { return in_stop_; }

    /// Access the pre-computed time<->beat lookup table (for debugging/testing).
    [[nodiscard]] const std::vector<TimeBeatSegment>& GetTimeBeatTable() const {
        return time_beat_table_;
    }

    /// Access the pre-computed visual position table (for debugging/testing).
    [[nodiscard]] const std::vector<VisualSegment>& GetVisualTable() const {
        return visual_table_;
    }

private:
    // ========================================================================
    // Lookup table construction
    // ========================================================================

    /// Build the time <-> beat piecewise-linear lookup table from BPM changes
    /// and stops. This is the core of the timing system.
    void BuildTimeBeatTable(
        const std::vector<TimingSegment>& bpms,
        const std::vector<TimingSegment>& stops,
        double offset
    );

    /// Build the beat -> visual position lookup table from scroll segments.
    void BuildVisualTable(const std::vector<TimingSegment>& scrolls);

    // ========================================================================
    // Binary search helpers
    // ========================================================================

    /// Find the index of the segment containing the given audio time.
    [[nodiscard]] size_t FindSegmentByTime(double time) const;

    /// Find the index of the segment containing the given beat.
    [[nodiscard]] size_t FindSegmentByBeat(double beat) const;

    /// Find the index of the visual segment containing the given beat.
    [[nodiscard]] size_t FindVisualSegmentByBeat(double beat) const;

    // ========================================================================
    // State
    // ========================================================================

    /// Pre-computed piecewise-linear time <-> beat lookup table.
    std::vector<TimeBeatSegment> time_beat_table_;

    /// Pre-computed beat -> visual position lookup table.
    std::vector<VisualSegment> visual_table_;

    /// Current state (updated each frame by Update()).
    double current_time_       = 0.0;
    double current_beat_       = 0.0;
    double current_visual_pos_ = 0.0;
    double current_bpm_        = 0.0;
    bool   in_stop_            = false;
    bool   initialized_        = false;
};

} // namespace sml
