// ============================================================================
// Conductor.cpp 窶・The timing heart: piecewise-linear time/beat/visual mapping
// ============================================================================

#include "timing/Conductor.h"
#include "parsing/Simfile.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace sml {

// ============================================================================
// Initialization
// ============================================================================

void Conductor::Initialize(const Simfile& simfile, const NoteChart& chart) {
    // Resolve SSC split timing: use chart-level if available, else global
    const auto& bpms    = simfile.GetEffectiveBPMs(chart);
    const auto& stops   = simfile.GetEffectiveStops(chart);
    const auto& scrolls = simfile.GetEffectiveScrolls(chart);
    const auto& speeds  = simfile.GetEffectiveSpeeds(chart);
    double offset       = simfile.GetEffectiveOffset(chart);

    Initialize(bpms, stops, scrolls, speeds, offset);
}

void Conductor::Initialize(
    const std::vector<TimingSegment>& bpms,
    const std::vector<TimingSegment>& stops,
    const std::vector<TimingSegment>& scrolls,
    const std::vector<TimingSegment>& speeds,
    double offset
) {
    time_beat_table_.clear();
    visual_table_.clear();
    speed_segments_ = speeds;

    BuildTimeBeatTable(bpms, stops, offset);
    BuildVisualTable(scrolls, speeds);

    current_time_       = 0.0;
    current_beat_       = 0.0;
    current_visual_pos_ = 0.0;
    current_bpm_        = 0.0;
    in_stop_            = false;
    initialized_        = true;
}

void Conductor::PopulateChartTiming(NoteChart& chart) const {
    if (!initialized_) return;
    int num_rows = static_cast<int>(chart.note_rows.size());
    for (int i = 0; i < num_rows; ++i) {
        auto& row = chart.note_rows[i];
        row.time = BeatToTime(row.beat);
        row.visual_pos = BeatToVisualPosition(row.beat);
        
        row.tail_row_indices.assign(row.columns.size(), -1);
        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
            NoteType nt = row.columns[col];
            if (nt == NoteType::HoldHead || nt == NoteType::RollHead) {
                // Find tail
                for (int j = i + 1; j < num_rows; ++j) {
                    if (col < static_cast<int>(chart.note_rows[j].columns.size()) &&
                        chart.note_rows[j].columns[col] == NoteType::HoldTail) {
                        row.tail_row_indices[col] = j;
                        break;
                    }
                }
            }
        }
    }
}

// ============================================================================
// Runtime update 窶・called each frame with DSP audio time
// ============================================================================

void Conductor::Update(double audio_time) {
    if (!initialized_ || time_beat_table_.empty()) return;

    current_time_ = audio_time;

    // Find the segment containing this audio time via binary search
    size_t idx = FindSegmentByTime(audio_time);
    const auto& seg = time_beat_table_[idx];

    // Determine if we're in a stop
    in_stop_ = (seg.bpm <= 0.0);

    if (in_stop_) {
        // During a stop: beat is frozen, BPM is the BPM from the previous
        // non-stop segment (for display purposes)
        current_beat_ = seg.start_beat;
        // Look backwards for the last real BPM
        current_bpm_ = 0.0;
        for (size_t i = idx; i > 0; --i) {
            if (time_beat_table_[i - 1].bpm > 0.0) {
                current_bpm_ = time_beat_table_[i - 1].bpm;
                break;
            }
        }
    } else {
        // Normal segment: interpolate beat from time
        current_beat_ = seg.TimeToBeat(audio_time);
        current_bpm_  = seg.bpm;
    }

    // Update visual position
    current_visual_pos_ = BeatToVisualPosition(current_beat_);
}

// ============================================================================
// Time <-> Beat conversion
// ============================================================================

double Conductor::TimeToBeat(double time) const {
    if (time_beat_table_.empty()) return 0.0;

    size_t idx = FindSegmentByTime(time);
    return time_beat_table_[idx].TimeToBeat(time);
}

double Conductor::BeatToTime(double beat) const {
    if (time_beat_table_.empty()) return 0.0;

    size_t idx = FindSegmentByBeat(beat);
    return time_beat_table_[idx].BeatToTime(beat);
}

// ============================================================================
// Beat -> Visual Position conversion
// ============================================================================

double Conductor::BeatToVisualPosition(double beat) const {
    if (visual_table_.empty()) return beat; // Fallback: 1:1 mapping

    size_t idx = FindVisualSegmentByBeat(beat);
    return visual_table_[idx].BeatToVisualPos(beat);
}

// ============================================================================
// GetYPosForBeat 窶・the renderer's core function
// ============================================================================

double Conductor::GetYPosForBeat(
    double target_beat,
    ScrollModType mod_type,
    double speed_mod,
    double receptor_y,
    double pixels_per_beat
) const {
    switch (mod_type) {
        case ScrollModType::XMod: {
            // X-Mod: position is based on visual beat distance
            // Notes speed up/slow down with BPM changes and scroll segments.
            double target_vpos  = BeatToVisualPosition(target_beat);
            double current_vpos = current_visual_pos_;
            double delta = target_vpos - current_vpos;

            // Y = receptor - delta * pixelsPerBeat * speedMod
            // Negative delta (note above receptor) = lower Y = higher on screen
            // Positive delta (note below receptor) = higher Y = lower on screen
            return receptor_y - delta * pixels_per_beat * speed_mod;
        }

        case ScrollModType::CMod: {
            // C-Mod: position is based on time distance, constant speed
            // All notes scroll at the same pixels-per-second regardless of BPM.
            // speed_mod is in pixels-per-second for C-Mod.
            double target_time  = BeatToTime(target_beat);
            double delta_time   = target_time - current_time_;

            return receptor_y - delta_time * speed_mod;
        }
    }

    return receptor_y; // Unreachable
}

double Conductor::GetYPosForVisualPos(
    double target_vpos,
    double speed_mod,
    double receptor_y,
    double pixels_per_beat
) const {
    double delta = target_vpos - current_visual_pos_;
    return receptor_y - delta * pixels_per_beat * speed_mod;
}

double Conductor::GetYPosForTime(
    double target_time,
    double speed_mod,
    double receptor_y
) const {
    double delta_time = target_time - current_time_;
    return receptor_y - delta_time * speed_mod;
}

// ============================================================================
// Lookup table construction: Time <-> Beat
// ============================================================================

void Conductor::BuildTimeBeatTable(
    const std::vector<TimingSegment>& bpms,
    const std::vector<TimingSegment>& stops,
    double offset
) {
    if (bpms.empty()) return;

    // ---- Step 1: Merge BPM changes and stops into a single sorted timeline ----
    struct TimingEvent {
        double beat;
        double value;
        bool   is_stop;  // true = stop, false = BPM change

        bool operator<(const TimingEvent& other) const {
            if (beat != other.beat) return beat < other.beat;
            // Process stops before BPM changes at the same beat
            return is_stop && !other.is_stop;
        }
    };

    std::vector<TimingEvent> events;
    events.reserve(bpms.size() + stops.size());

    for (const auto& seg : bpms) {
        events.push_back({seg.start_beat, seg.value, false});
    }
    for (const auto& seg : stops) {
        events.push_back({seg.start_beat, seg.value, true});
    }

    std::sort(events.begin(), events.end());

    // ---- Step 2: Walk through events computing cumulative time ----

    // Walk through events computing cumulative time
    // StepMania convention: Time = BeatTime - Offset
    // Beat 0 occurs at Time = -Offset.
    // So if we want Beat 0 to happen at time T, we start current_time at -T.
    // BUT SM Offset is "Time in seconds at which beat 0 occurs".
    // If Offset is positive (e.g. 1.0), beat 0 is at 1.0s.
    // If Offset is negative (e.g. -1.0), beat 0 is at -1.0s?
    // Wait, the SM Wiki says: "Negative values cause the steps to start AFTER the music has begun."
    // This implies Negative Offset -> Positive Start Time.
    // So Beat 0 Time = -Offset.
    // e.g. Offset -7.530 -> Beat 0 at +7.530.
    double current_time = -offset;
    double current_beat = 0.0;
    double current_bpm  = 120.0; // Default BPM if first event isn't at beat 0

    // Check if the first event is a BPM change at beat 0
    for (const auto& ev : events) {
        if (!ev.is_stop && ev.beat <= 0.0) {
            current_bpm = ev.value;
            break;
        }
    }

    for (const auto& ev : events) {
        // Close the segment from current_beat to ev.beat
        if (ev.beat > current_beat && current_bpm > 0.0) {
            double dt = (ev.beat - current_beat) * 60.0 / current_bpm;

            TimeBeatSegment seg;
            seg.start_time = current_time;
            seg.start_beat = current_beat;
            seg.bpm        = current_bpm;
            seg.end_time   = current_time + dt;
            seg.end_beat   = ev.beat;
            time_beat_table_.push_back(seg);

            current_time += dt;
            current_beat = ev.beat;
        }

        if (ev.is_stop) {
            // Stop segment: time advances, beat stays frozen
            double stop_duration = ev.value;
            if (stop_duration > 0.0) {
                TimeBeatSegment seg;
                seg.start_time = current_time;
                seg.start_beat = current_beat;
                seg.bpm        = 0.0; // Marker: this is a stop
                seg.end_time   = current_time + stop_duration;
                seg.end_beat   = current_beat; // Beat doesn't change
                time_beat_table_.push_back(seg);

                current_time += stop_duration;
            }
        } else {
            // BPM change
            current_bpm = ev.value;
        }
    }

    // ---- Step 3: Final open-ended segment extending to "infinity" ----
    {
        TimeBeatSegment seg;
        seg.start_time = current_time;
        seg.start_beat = current_beat;
        seg.bpm        = current_bpm;
        seg.end_time   = TimeBeatSegment::INF;
        seg.end_beat   = TimeBeatSegment::INF;
        time_beat_table_.push_back(seg);
    }
}

// ============================================================================
// Lookup table construction: Visual Position
// ============================================================================

void Conductor::BuildVisualTable(
    const std::vector<TimingSegment>& scrolls,
    const std::vector<TimingSegment>& speeds
) {
    // Merge scroll and speed change points into a unified visual table.
    // Effective visual rate = scroll_rate * speed_ratio
    // SCROLLS are beat-based multipliers, SPEEDS are beat-based with transitions.
    // For simplicity, we treat SPEEDS as instant changes (ignoring transition for now).

    // Collect all change-point beats
    struct ChangePoint {
        double beat;
        enum Type { SCROLL, SPEED } type;
        double value;
    };
    std::vector<ChangePoint> changes;

    for (const auto& s : scrolls) {
        changes.push_back({s.start_beat, ChangePoint::SCROLL, s.value});
    }
    for (const auto& s : speeds) {
        changes.push_back({s.start_beat, ChangePoint::SPEED, s.value});
    }

    // Sort by beat
    std::sort(changes.begin(), changes.end(),
        [](const ChangePoint& a, const ChangePoint& b) { return a.beat < b.beat; });

    if (changes.empty()) {
        // No scroll or speed segments: 1:1 mapping
        VisualSegment seg;
        seg.start_beat       = 0.0;
        seg.start_visual_pos = 0.0;
        seg.scroll_rate      = 1.0;
        visual_table_.push_back(seg);
        return;
    }

    double current_beat = 0.0;
    double current_pos  = 0.0;
    double current_scroll = 1.0;
    double current_speed  = 1.0;

    // Handle changes that start after beat 0
    if (changes.front().beat > 0.0) {
        VisualSegment seg;
        seg.start_beat       = 0.0;
        seg.start_visual_pos = 0.0;
        seg.scroll_rate      = current_scroll * current_speed;
        visual_table_.push_back(seg);

        current_pos  = changes.front().beat * current_scroll * current_speed;
        current_beat = changes.front().beat;
    }

    for (const auto& cp : changes) {
        if (cp.beat > current_beat) {
            current_pos += (cp.beat - current_beat) * current_scroll * current_speed;
            current_beat = cp.beat;
        }

        if (cp.type == ChangePoint::SCROLL) {
            current_scroll = cp.value;
        } else {
            current_speed = cp.value;
        }

        VisualSegment seg;
        seg.start_beat       = cp.beat;
        seg.start_visual_pos = current_pos;
        seg.scroll_rate      = current_scroll * current_speed;
        visual_table_.push_back(seg);

        current_beat = cp.beat;
    }
}

// ============================================================================
// Binary search helpers
// ============================================================================

size_t Conductor::FindSegmentByTime(double time) const {
    assert(!time_beat_table_.empty());

    // Binary search for the last segment where start_time <= time
    size_t lo = 0;
    size_t hi = time_beat_table_.size();

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (time_beat_table_[mid].start_time <= time) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    // lo now points one past the desired segment
    return (lo > 0) ? lo - 1 : 0;
}

size_t Conductor::FindSegmentByBeat(double beat) const {
    assert(!time_beat_table_.empty());

    // For beat-based search, we need to find the last segment where
    // start_beat <= beat. Stop segments have start_beat == end_beat,
    // so we skip them when the beat is exactly on a stop boundary
    // (the non-stop segment after the stop should be used).
    size_t lo = 0;
    size_t hi = time_beat_table_.size();

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (time_beat_table_[mid].start_beat <= beat) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    size_t idx = (lo > 0) ? lo - 1 : 0;

    // If we landed on a stop segment and the beat matches exactly,
    // prefer the non-stop segment after it (for BeatToTime queries)
    if (time_beat_table_[idx].bpm <= 0.0 &&
        idx + 1 < time_beat_table_.size() &&
        time_beat_table_[idx + 1].start_beat <= beat) {
        idx++;
    }

    return idx;
}

size_t Conductor::FindVisualSegmentByBeat(double beat) const {
    assert(!visual_table_.empty());

    size_t lo = 0;
    size_t hi = visual_table_.size();

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (visual_table_[mid].start_beat <= beat) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return (lo > 0) ? lo - 1 : 0;
}

} // namespace sml
