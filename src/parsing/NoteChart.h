#pragma once
// ============================================================================
// NoteChart.h — Per-chart note data with optional split timing (SSC)
//
// Each NoteChart represents one difficulty/stepstype within a simfile.
// In .ssc files, charts can override the global timing with their own
// #BPMS, #STOPS, #SCROLLS, and #OFFSET tags.
// ============================================================================

#include <string>
#include <vector>
#include <cstdint>
#include "timing/TimingData.h"

namespace sml {
struct RadarValues {
    double stream = 0.0;
    double voltage = 0.0;
    double air = 0.0;
    double freeze = 0.0;
    double chaos = 0.0;
};

enum class ChartVariant {
    Normal,
    BeginnerPlus,
    EasyPlus,
    MediumPlus,
    HardPlus,
    ChallengePlus,
    Wild
};


// ============================================================================
// NoteRow — a single row of notes at a specific beat
// ============================================================================

/// A single row of notes. Each column corresponds to a lane/receptor.
/// For dance-single (4-panel), columns has 4 entries.
/// For dance-double (8-panel), columns has 8 entries. Etc.
struct NoteRow {
    double beat = 0.0;                  ///< The beat at which this row occurs
    std::vector<NoteType> columns;      ///< Note type per column/lane

    /// Check if this row has any actual notes (not just empty columns).
    [[nodiscard]] bool HasNotes() const {
        for (auto col : columns) {
            if (col != NoteType::None) return true;
        }
        return false;
    }

    /// Get the number of columns/lanes.
    [[nodiscard]] size_t NumColumns() const { return columns.size(); }

    /// Get the number of notes in this row (taps, hold heads, roll heads, lifts).
    [[nodiscard]]    int NumNotes() const {
        int count = 0;
        for (auto col : columns) {
            if (IsTap(col)) {
                count++;
            }
        }
        return count;
    }
};

// ============================================================================
// NoteChart — one chart (difficulty) within a simfile
// ============================================================================

/// A single chart within a simfile, containing note data and optional
/// per-chart timing overrides for SSC split timing.
struct NoteChart {
    // ========================================================================
    // Chart identity
    // ========================================================================
    std::string chart_type;         ///< e.g. "dance-single", "dance-double"
    std::string description;        ///< Author/description string
    std::string difficulty_name;    ///< e.g. "Beginner", "Easy", "Medium", "Hard", "Challenge", "Edit"
    int         difficulty_meter = 0; ///< Numeric difficulty rating
    std::string credit;             ///< Step artist / Author

    RadarValues radar;
    double custom_difficulty = 1.0;      ///< 1.0-30.0 scale (1 decimal)
    ChartVariant variant = ChartVariant::Normal;

    // ========================================================================
    // Note data
    // ========================================================================

    /// All note rows sorted by beat. Rows with no notes are excluded.
    std::vector<NoteRow> note_rows;

    /// Number of columns/lanes for this chart type.
    /// Derived from chart_type: dance-single=4, dance-double=8, etc.
    int num_columns = 4;

    // ========================================================================
    // Per-chart timing overrides (SSC split timing)
    // ========================================================================
    // These are ONLY populated for .ssc files when the chart defines its own
    // timing tags. If empty, the global Simfile timing is used instead.

    double offset = 0.0;                    ///< Chart-specific offset (SSC)
    std::vector<TimingSegment> bpms;        ///< Chart-specific BPM segments
    std::vector<TimingSegment> stops;       ///< Chart-specific stop segments
    std::vector<TimingSegment> scrolls;     ///< Chart-specific scroll segments
    std::vector<TimingSegment> speeds;      ///< Chart-specific speed segments

    bool has_own_timing = false;            ///< True if any chart-level timing was parsed

    // ========================================================================
    // Helpers
    // ========================================================================

    /// Returns true if this chart has its own timing data (SSC split timing).
    [[nodiscard]] bool HasOwnTimingData() const {
        return has_own_timing;
    }

    /// Determine num_columns from chart_type string.
    static int ColumnsForChartType(const std::string& type) {
        if (type == "dance-single")     return 4;
        if (type == "dance-double")     return 8;
        if (type == "dance-couple")     return 8;
        if (type == "dance-routine")    return 8;
        if (type == "dance-solo")       return 6;
        if (type == "dance-threepanel") return 3;
        if (type == "pump-single")      return 5;
        if (type == "pump-double")      return 10;
        if (type == "pump-halfdouble")  return 6;
        if (type == "kb7-single")       return 7;
        // Default to 4
        return 4;
    }

    /// Get the last beat in the chart (beat of the last note row).
    [[nodiscard]] double GetLastBeat() const {
        if (note_rows.empty()) return 0.0;
        return note_rows.back().beat;
    }

    /// Get the total number of hittable rows (rows containing Taps, HoldHeads, or RollHeads).
    [[nodiscard]]    int GetTotalTaps() const {
        int count = 0;
        for (const auto& row : note_rows) {
            for (auto col : row.columns) {
                if (IsTap(col)) {
                    ++count;
                    break; // Count the row once
                }
            }
        }
        return count;
    }
};

} // namespace sml
