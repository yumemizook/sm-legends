// ============================================================================
// test_timing.cpp — Automated tests for the Conductor timing engine
//
// Uses assert() for validation. Tests cover:
//   1. Constant BPM time<->beat conversion
//   2. BPM changes at mid-song
//   3. Stops (freezes)
//   4. Scroll segments (visual position)
//   5. Round-trip consistency (BeatToTime(TimeToBeat(t)) ≈ t)
//   6. GetYPosForBeat for X-Mod and C-Mod
//   7. Combined BPM change + stop scenario
// ============================================================================

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "timing/TimingData.h"
#include "timing/Conductor.h"

using namespace sml;

// Floating-point comparison with tolerance
static bool ApproxEqual(double a, double b, double epsilon = 0.0001) {
    return std::fabs(a - b) < epsilon;
}

#define ASSERT_APPROX(a, b) \
    do { \
        if (!ApproxEqual((a), (b))) { \
            std::printf("  FAIL: %s ≈ %s => %.8f ≈ %.8f (diff: %.8f)\n", \
                #a, #b, (double)(a), (double)(b), std::fabs((double)(a) - (double)(b))); \
            assert(false); \
        } \
    } while (0)

// ============================================================================
// Test 1: Constant BPM
// ============================================================================
static void TestConstantBPM() {
    std::printf("Test 1: Constant BPM (120)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {{0.0, 120.0, TimingSegmentType::BPM}};
    std::vector<TimingSegment> stops;
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // At 120 BPM: 2 beats per second
    // Beat 0 = Time 0
    // Beat 2 = Time 1.0
    // Beat 4 = Time 2.0
    ASSERT_APPROX(conductor.TimeToBeat(0.0), 0.0);
    ASSERT_APPROX(conductor.TimeToBeat(1.0), 2.0);
    ASSERT_APPROX(conductor.TimeToBeat(2.0), 4.0);
    ASSERT_APPROX(conductor.TimeToBeat(0.5), 1.0);

    // Reverse
    ASSERT_APPROX(conductor.BeatToTime(0.0), 0.0);
    ASSERT_APPROX(conductor.BeatToTime(2.0), 1.0);
    ASSERT_APPROX(conductor.BeatToTime(4.0), 2.0);

    // Update test
    conductor.Update(1.5);
    ASSERT_APPROX(conductor.GetCurrentBeat(), 3.0);
    ASSERT_APPROX(conductor.GetCurrentBPM(), 120.0);
    assert(!conductor.IsInStop());

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 2: BPM Change
// ============================================================================
static void TestBPMChange() {
    std::printf("Test 2: BPM Change (120 -> 240 at beat 4)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
        {4.0, 240.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops;
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // Phase 1: 120 BPM (2 beats/sec)
    // Beat 4 = Time 2.0
    ASSERT_APPROX(conductor.TimeToBeat(0.0), 0.0);
    ASSERT_APPROX(conductor.TimeToBeat(2.0), 4.0);
    ASSERT_APPROX(conductor.BeatToTime(4.0), 2.0);

    // Phase 2: 240 BPM (4 beats/sec) starting at beat 4 / time 2.0
    // Beat 6 = time 2.0 + (6-4)/4 = 2.0 + 0.5 = 2.5
    ASSERT_APPROX(conductor.TimeToBeat(2.5), 6.0);
    ASSERT_APPROX(conductor.BeatToTime(6.0), 2.5);

    // Beat 8 = time 2.0 + (8-4)/4 = 2.0 + 1.0 = 3.0
    ASSERT_APPROX(conductor.TimeToBeat(3.0), 8.0);
    ASSERT_APPROX(conductor.BeatToTime(8.0), 3.0);

    // Update at the transition point
    conductor.Update(2.0);
    ASSERT_APPROX(conductor.GetCurrentBeat(), 4.0);
    ASSERT_APPROX(conductor.GetCurrentBPM(), 240.0);

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 3: Stop (Freeze)
// ============================================================================
static void TestStop() {
    std::printf("Test 3: Stop (1.0s freeze at beat 4, 120 BPM)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops = {
        {4.0, 1.0, TimingSegmentType::Stop},  // 1 second stop at beat 4
    };
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // Before stop: normal 120 BPM
    ASSERT_APPROX(conductor.TimeToBeat(1.0), 2.0);
    ASSERT_APPROX(conductor.TimeToBeat(2.0), 4.0);

    // During stop (time 2.0 to 3.0): beat stays at 4.0
    conductor.Update(2.5);
    ASSERT_APPROX(conductor.GetCurrentBeat(), 4.0);
    assert(conductor.IsInStop());

    ASSERT_APPROX(conductor.TimeToBeat(2.0), 4.0);
    ASSERT_APPROX(conductor.TimeToBeat(2.5), 4.0);
    ASSERT_APPROX(conductor.TimeToBeat(3.0), 4.0);

    // After stop: BPM resumes at 120, but time is shifted by 1.0s
    // Beat 5 = time 3.0 + (5-4)*(60/120) = 3.0 + 0.5 = 3.5
    ASSERT_APPROX(conductor.TimeToBeat(3.5), 5.0);
    ASSERT_APPROX(conductor.BeatToTime(5.0), 3.5);

    // Beat 6 = time 3.0 + (6-4)*(60/120) = 3.0 + 1.0 = 4.0
    ASSERT_APPROX(conductor.TimeToBeat(4.0), 6.0);

    conductor.Update(3.5);
    ASSERT_APPROX(conductor.GetCurrentBeat(), 5.0);
    assert(!conductor.IsInStop());

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 4: Scroll Segments (Visual Position)
// ============================================================================
static void TestScrollSegments() {
    std::printf("Test 4: Scroll Segments (2.0x at beat 4)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops;
    std::vector<TimingSegment> scrolls = {
        {4.0, 2.0, TimingSegmentType::Scroll},  // 2x scroll at beat 4
    };

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // Before scroll change: 1:1 visual position
    ASSERT_APPROX(conductor.BeatToVisualPosition(0.0), 0.0);
    ASSERT_APPROX(conductor.BeatToVisualPosition(2.0), 2.0);
    ASSERT_APPROX(conductor.BeatToVisualPosition(4.0), 4.0);

    // After scroll change: 2x visual speed
    // Visual pos at beat 6 = 4.0 + (6-4)*2.0 = 4.0 + 4.0 = 8.0
    ASSERT_APPROX(conductor.BeatToVisualPosition(6.0), 8.0);

    // Visual pos at beat 5 = 4.0 + (5-4)*2.0 = 4.0 + 2.0 = 6.0
    ASSERT_APPROX(conductor.BeatToVisualPosition(5.0), 6.0);

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 5: Round-trip Consistency
// ============================================================================
static void TestRoundTrip() {
    std::printf("Test 5: Round-trip BeatToTime(TimeToBeat(t)) ≈ t ...\n");

    Conductor conductor;

    // Complex scenario: multiple BPM changes and a stop
    std::vector<TimingSegment> bpms = {
        {0.0,  120.0, TimingSegmentType::BPM},
        {8.0,  180.0, TimingSegmentType::BPM},
        {16.0, 90.0,  TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops = {
        {4.0, 0.5, TimingSegmentType::Stop},
        {12.0, 1.0, TimingSegmentType::Stop},
    };
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // Test round-trip at various time points
    double test_times[] = {0.0, 0.5, 1.0, 1.5, 3.0, 5.0, 7.0, 10.0, 15.0};
    for (double t : test_times) {
        double beat = conductor.TimeToBeat(t);
        double t_roundtrip = conductor.BeatToTime(beat);
        // During stops, TimeToBeat returns the frozen beat, and BeatToTime
        // returns the time the beat was first reached (before the stop).
        // So round-trip might not be exact during stops; it should return
        // a time <= original time.
        if (t_roundtrip > t + 0.001) {
            std::printf("  FAIL at t=%.4f: beat=%.4f, round-trip=%.4f\n",
                t, beat, t_roundtrip);
            assert(false);
        }
    }

    // Forward test: BeatToTime -> TimeToBeat should be exact
    double test_beats[] = {0.0, 1.0, 2.0, 3.0, 5.0, 7.0, 10.0, 14.0, 20.0};
    for (double b : test_beats) {
        double t = conductor.BeatToTime(b);
        double b_roundtrip = conductor.TimeToBeat(t);
        ASSERT_APPROX(b_roundtrip, b);
    }

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 6: GetYPosForBeat (X-Mod and C-Mod)
// ============================================================================
static void TestGetYPosForBeat() {
    std::printf("Test 6: GetYPosForBeat (X-Mod and C-Mod)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops;
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);
    conductor.Update(0.0); // Current beat = 0

    double receptor_y = 500.0;
    double ppb = 64.0;
    double speed = 2.0;

    // X-Mod: note at beat 4, current beat 0
    // Visual pos at beat 4 = 4.0 (no scrolls, 1:1)
    // Visual pos at beat 0 = 0.0
    // Y = 500 - (4.0 - 0.0) * 64 * 2.0 = 500 - 512 = -12
    double y_xmod = conductor.GetYPosForBeat(4.0, ScrollModType::XMod, speed, receptor_y, ppb);
    ASSERT_APPROX(y_xmod, -12.0);

    // Note at beat 0 (at receptor) should be at receptor_y
    double y_at_receptor = conductor.GetYPosForBeat(0.0, ScrollModType::XMod, speed, receptor_y, ppb);
    ASSERT_APPROX(y_at_receptor, receptor_y);

    // C-Mod: note at beat 4, current time 0, 120 BPM
    // Time at beat 4 = 2.0 seconds
    // speed_mod = 400 px/sec
    // Y = 500 - (2.0 - 0.0) * 400 = 500 - 800 = -300
    double y_cmod = conductor.GetYPosForBeat(4.0, ScrollModType::CMod, 400.0, receptor_y, ppb);
    ASSERT_APPROX(y_cmod, -300.0);

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 7: Combined BPM Change + Stop
// ============================================================================
static void TestCombinedBPMAndStop() {
    std::printf("Test 7: Combined BPM Change + Stop...\n");

    Conductor conductor;

    // 120 BPM -> stop at beat 4 (0.5s) -> 240 BPM at beat 4
    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
        {4.0, 240.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops = {
        {4.0, 0.5, TimingSegmentType::Stop},  // 0.5s stop right at the BPM change
    };
    std::vector<TimingSegment> scrolls;

    conductor.Initialize(bpms, stops, scrolls, 0.0);

    // Beat 4 at time 2.0 (120 BPM for 4 beats)
    ASSERT_APPROX(conductor.TimeToBeat(2.0), 4.0);

    // During stop (time 2.0 to 2.5): beat = 4.0
    ASSERT_APPROX(conductor.TimeToBeat(2.25), 4.0);

    // After stop, at 240 BPM:
    // Beat 5 = time 2.5 + (5-4)*(60/240) = 2.5 + 0.25 = 2.75
    ASSERT_APPROX(conductor.TimeToBeat(2.75), 5.0);
    ASSERT_APPROX(conductor.BeatToTime(5.0), 2.75);

    // Beat 8 = time 2.5 + (8-4)*(60/240) = 2.5 + 1.0 = 3.5
    ASSERT_APPROX(conductor.TimeToBeat(3.5), 8.0);

    std::printf("  PASS\n\n");
}

// ============================================================================
// Test 8: Offset
// ============================================================================
static void TestOffset() {
    std::printf("Test 8: Offset (0.1s positive offset)...\n");

    Conductor conductor;

    std::vector<TimingSegment> bpms = {
        {0.0, 120.0, TimingSegmentType::BPM},
    };
    std::vector<TimingSegment> stops;
    std::vector<TimingSegment> scrolls;

    // Positive offset = music starts 0.1s before beat 0
    // So beat 0 is at audio time 0.1s (since start_time = -offset = -0.1)
    // Actually: start_time = -offset => if offset = 0.1, start_time = -0.1
    // So at time 0.0, we're 0.1s before the first beat (beat 0)
    // beat = 0.0 + (0.0 - (-(-0.1))) ... Wait.
    // start_time = -offset. If offset = 0.1, start_time = -0.1.
    // TimeToBeat(t) = start_beat + (t - start_time) * (BPM/60)
    // At t=0.0: beat = 0.0 + (0.0 - (-0.1)) * (120/60) = 0.2? 
    // Wait, the SM convention is: Time = Beat_Time - Offset.
    // So Beat_Time = Time + Offset.
    // At Time 0, Beat_Time = Offset.
    // If BPM is 120 (2 beats/sec), and Beat_Time is 0.1s, then beat = 0.2.
    // My previous thought was: Offset    // Positive offset = music starts 0.1s before beat 0 (No).
    // SM Convention: Beat 0 Time = -Offset.
    // Initialize(..., 0.1). Offset = 0.1. Beat 0 at -0.1s.
    // At Time 0.0, we are 0.1s past Beat 0.
    // 0.1s at 120BPM (0.5s/beat) = 0.2 beats.
    conductor.Initialize(bpms, stops, scrolls, 0.1);

    ASSERT_APPROX(conductor.TimeToBeat(0.0), 0.2);
    ASSERT_APPROX(conductor.TimeToBeat(0.1), 0.4);
    ASSERT_APPROX(conductor.BeatToTime(0.0), -0.1);

    std::printf("  PASS\n\n");
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::printf("SM-Legends Timing Engine Tests\n");
    std::printf("==============================\n\n");

    TestConstantBPM();
    TestBPMChange();
    TestStop();
    TestScrollSegments();
    TestRoundTrip();
    TestGetYPosForBeat();
    TestCombinedBPMAndStop();
    TestOffset();

    std::printf("==============================\n");
    std::printf("All tests passed!\n");
    return 0;
}
