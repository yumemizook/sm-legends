#pragma once
// ============================================================================
// TimingData.h — Core timing data structures for the VSRG engine
//
// These structs form the backbone of the timing system. Raw parsed segments
// are stored as TimingSegment; pre-computed lookup tables use TimeBeatSegment
// (for time<->beat mapping) and VisualSegment (for beat->visual position).
// ============================================================================

#include <cmath>
#include <limits>

namespace sml {

// ============================================================================
// Enums
// ============================================================================

/// Type of a raw parsed timing segment from a simfile.
enum class TimingSegmentType {
    BPM,        ///< Beats-per-minute change at a given beat
    Stop,       ///< Freeze/stop: time advances, beat stays fixed (value = seconds)
    Scroll,     ///< Visual scroll speed multiplier (SSC #SCROLLS)
    Speed,      ///< Visual speed change with transition (SSC #SPEEDS)
};

/// Scroll modifier type used during rendering.
enum class ScrollModType {
    XMod,   ///< Multiplier — speed scales with BPM and scroll segments
    CMod,   ///< Constant  — fixed pixels-per-second regardless of BPM
};

// ============================================================================
// Raw parsed segments (directly from simfile tags)
// ============================================================================

/// A single timing event parsed from #BPMS, #STOPS, #SCROLLS, or #SPEEDS.
struct TimingSegment {
    double start_beat = 0.0;    ///< Beat at which this segment takes effect
    double value     = 0.0;     ///< BPM value, stop duration (s), or scroll multiplier
    TimingSegmentType type = TimingSegmentType::BPM;

    // For #SPEEDS: additional parameters
    double duration  = 0.0;     ///< Transition duration (beats or seconds)
    int    unit      = 0;       ///< 0 = duration is in beats, 1 = duration is in seconds

    bool operator<(const TimingSegment& other) const {
        if (start_beat != other.start_beat) return start_beat < other.start_beat;
        // Stops should be processed before BPM changes at the same beat
        if (type == TimingSegmentType::Stop && other.type == TimingSegmentType::BPM) return true;
        if (type == TimingSegmentType::BPM && other.type == TimingSegmentType::Stop) return false;
        return false;
    }
};

// ============================================================================
// Pre-computed lookup table entries
// ============================================================================

/// A single segment in the time <-> beat piecewise-linear lookup table.
///
/// Within a segment, time and beat are related linearly:
///   beat = start_beat + (time - start_time) * (bpm / 60.0)
///
/// For stop segments, bpm == 0, and the beat is frozen at start_beat
/// while time advances from start_time to end_time.
struct TimeBeatSegment {
    double start_time = 0.0;    ///< Audio time (seconds) at segment start
    double start_beat = 0.0;    ///< Beat number at segment start
    double bpm        = 0.0;    ///< BPM during this segment (0 = stop/freeze)
    double end_time   = 0.0;    ///< Audio time (seconds) at segment end
    double end_beat   = 0.0;    ///< Beat number at segment end

    static constexpr double INF = std::numeric_limits<double>::infinity();

    /// Check if a given audio time falls within this segment.
    [[nodiscard]] bool ContainsTime(double t) const {
        return t >= start_time && t < end_time;
    }

    /// Check if a given beat falls within this segment.
    [[nodiscard]] bool ContainsBeat(double b) const {
        return b >= start_beat && b < end_beat;
    }

    /// Interpolate beat from audio time within this segment.
    [[nodiscard]] double TimeToBeat(double t) const {
        if (bpm <= 0.0) return start_beat; // Stop: beat is frozen
        return start_beat + (t - start_time) * (bpm / 60.0);
    }

    /// Interpolate audio time from beat within this segment.
    [[nodiscard]] double BeatToTime(double b) const {
        if (bpm <= 0.0) return start_time; // Stop: return segment start
        return start_time + (b - start_beat) * (60.0 / bpm);
    }
};

/// A single segment in the beat -> visual position lookup table.
///
/// Visual position accounts for #SCROLLS segments so notes can speed up or
/// slow down visually independent of musical tempo.
///
///   visual_pos = start_visual_pos + (beat - start_beat) * scroll_rate
struct VisualSegment {
    double start_beat       = 0.0;  ///< Beat at which this visual segment starts
    double start_visual_pos = 0.0;  ///< Cumulative visual position at start_beat
    double scroll_rate      = 1.0;  ///< Visual speed multiplier (1.0 = normal)

    /// Interpolate visual position from beat within this segment.
    [[nodiscard]] double BeatToVisualPos(double beat) const {
        return start_visual_pos + (beat - start_beat) * scroll_rate;
    }
};

// ============================================================================
// Note types
// ============================================================================

/// The type of note in a single column of a note row.
enum class NoteType : uint8_t {
    None      = '0',    ///< No note
    Tap       = '1',    ///< Standard tap note
    HoldHead  = '2',    ///< Beginning of a hold (long note)
    HoldTail  = '3',    ///< End of a hold / roll
    RollHead  = '4',    ///< Beginning of a roll
    Mine      = 'M',    ///< Mine (avoid hitting)
    Lift      = 'L',    ///< Lift note (release)
    Fake      = 'F',    ///< Fake note (cosmetic only, not scored)
};

} // namespace sml
