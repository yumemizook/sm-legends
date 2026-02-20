#pragma once
// ============================================================================
// Simfile.h 窶・Top-level song/simfile data container
//
// Represents a fully parsed .sm or .ssc file. Contains song metadata,
// global timing data, and a collection of NoteCharts (one per difficulty).
// ============================================================================

#include <string>
#include <vector>
#include "timing/TimingData.h"
#include "parsing/NoteChart.h"

namespace sml {

/// A single background change event (#BGCHANGES)
struct BGEvent {
    double beat = 0.0;
    std::string file;
    double rate = 1.0;
    int transition = 0; // 0=Crossfade, etc.

    bool operator<(const BGEvent& other) const {
        return beat < other.beat;
    }
};

/// A fully parsed simfile (.sm or .ssc) with all its charts.
struct Simfile {
    // ========================================================================
    // Song metadata
    // ========================================================================
    std::string title;
    std::string subtitle;
    std::string artist;
    std::string title_translit;
    std::string subtitle_translit;
    std::string artist_translit;
    std::string genre;
    std::string credit;
    std::string banner_path;
    std::string background_path;
    std::string jacket_path;
    std::string cover_path;
    std::string cd_title_path;
    std::string music_path;
    std::string directory;          ///< Original directory of the simfile

    // ========================================================================
    // Audio / preview
    // ========================================================================
    double sample_start  = 0.0;     ///< Preview start time (seconds)
    double sample_length = 0.0;     ///< Preview duration (seconds)

    // ========================================================================
    // Global timing data
    // ========================================================================
    double offset = 0.0;            ///< Global audio offset (seconds).
                                    ///< Positive = music starts before beat 0.

    /// Global BPM segments (shared by all charts unless overridden in SSC).
    std::vector<TimingSegment> bpms;

    /// Global stop/freeze segments.
    std::vector<TimingSegment> stops;

    /// Global scroll speed segments (SSC #SCROLLS).
    std::vector<TimingSegment> scrolls;

    /// Global speed segments (SSC #SPEEDS).
    std::vector<TimingSegment> speeds;

    /// Global fake segments (SSC #FAKES).
    std::vector<FakeSegment> fakes;

    /// Global attacks (SSC #ATTACKS).
    std::vector<Attack> attacks;

    /// Background animation changes (#BGCHANGES).
    std::vector<BGEvent> bg_changes;

    // ========================================================================
    // Charts
    // ========================================================================

    /// All note charts contained in this simfile (one per difficulty/stepstype).
    std::vector<NoteChart> charts;

    // ========================================================================
    // Helpers
    // ========================================================================

    /// Find a chart by type and difficulty name.
    /// Returns nullptr if not found.
    [[nodiscard]] const NoteChart* FindChart(
        const std::string& chart_type,
        const std::string& difficulty_name
    ) const {
        for (const auto& chart : charts) {
            if (chart.chart_type == chart_type &&
                chart.difficulty_name == difficulty_name) {
                return &chart;
            }
        }
        return nullptr;
    }

    /// Get the effective BPMs for a given chart (chart-level or global fallback).
    [[nodiscard]] const std::vector<TimingSegment>& GetEffectiveBPMs(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.bpms.empty()
            ? chart.bpms : bpms;
    }

    /// Get the effective stops for a given chart.
    [[nodiscard]] const std::vector<TimingSegment>& GetEffectiveStops(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.stops.empty()
            ? chart.stops : stops;
    }

    /// Get the effective scrolls for a given chart.
    [[nodiscard]] const std::vector<TimingSegment>& GetEffectiveScrolls(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.scrolls.empty()
            ? chart.scrolls : scrolls;
    }

    /// Get the effective speeds for a given chart.
    [[nodiscard]] const std::vector<TimingSegment>& GetEffectiveSpeeds(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.speeds.empty()
            ? chart.speeds : speeds;
    }

    /// Get the effective fakes for a given chart.
    [[nodiscard]] const std::vector<FakeSegment>& GetEffectiveFakes(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.fakes.empty()
            ? chart.fakes : fakes;
    }

    /// Get the effective attacks for a given chart.
    [[nodiscard]] const std::vector<Attack>& GetEffectiveAttacks(
        const NoteChart& chart
    ) const {
        return chart.HasOwnTimingData() && !chart.attacks.empty()
            ? chart.attacks : attacks;
    }

    /// Get the effective offset for a given chart.
    [[nodiscard]] double GetEffectiveOffset(const NoteChart& chart) const {
        return chart.HasOwnTimingData() ? chart.offset : offset;
    }
};

} // namespace sml
