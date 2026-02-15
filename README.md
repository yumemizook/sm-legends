# SM-Legends

A VSRG (Vertical Scrolling Rhythm Game) engine featuring a dual scoring system, advanced clear type tracking, and precision timing.
The engine is a PoC to be built into an arcade-like StepMania clone.

## Features

### Dual Scoring System
The engine tracks two independent scores simultaneously for every chart played:
1. **Normal Score**: Traditional percentage-based scoring with standard timing windows.
2. **EX Score**: A stricter, accuracy-focused scoring system with tighter timing windows.

Both scores are calculated in real-time and displayed on the results screen.

### Clear Types
The game tracks your best performance on a chart using a hierarchy of Clear Types. A higher Clear Type overwrites a lower one.

From highest to lowest:
1. **ALL PERFECT EXTRAORDINARY**: 100% P-Extraordinary judgments.
2. **ALL PERFECT CRITICAL**: 100% P-Critical judgments (or higher in EX mode).
3. **ALL PERFECT+**: All judgments are Perfect or higher.
4. **ALL PERFECT**: All judgments are Perfect (Low) or higher.
5. **FULL COMBO+**: All judgments are Great (Low) or higher (Normal Mode) / Okay (Low) or higher (EX Mode).
6. **FULL COMBO**: All judgments are Good or higher.
7. **CLEAR**: Passed the chart but broke combo (Missed notes).
8. **FAIL**: Lifebar reached zero.

## Scoring Systems

### Normal Mode
The standard scoring mode used for main grades.

**Timing Windows:**
- P-Critical: +/- 16.67ms
- Perfect: +/- 33.33ms
- Perfect (Low): +/- 50ms
- Great (High): +/- 66.67ms
- Great: +/- 80ms
- Great (Low): +/- 100ms
- Good: +/- 166ms
- Miss: > 166ms

**Grades:**
- SSS+: >= 100.9%
- SSS: >= 100.75%
- SS+: >= 100.5%
- SS: >= 100.0%
- S+: >= 99.0%
- S: >= 97.5%
- AAA: >= 95.0%
- AA: >= 92.5%
- A: >= 90.0%
- BBB: >= 80.0%
- BB: >= 70.0%
- B: >= 60.0%
- C: >= 50.0%
- D: < 50.0%

### EX Mode
A precision mode with strict timing and "Okay" judgments instead of "Great".

**Timing Windows:**
- P-Extraordinary: +/- 10ms
- P-Critical: +/- 16.67ms
- Perfect: +/- 33.33ms
- Perfect (Low): +/- 50ms
- Okay (High): +/- 66.67ms
- Okay: +/- 80ms
- Okay (Low): +/- 100ms
- Miss: > 100ms

**EX Grades (Stars):**
- 6 STARS: 100%
- 5 STARS: 95%
- 4 STARS: 88.5%
- 3 STARS: 82%
- 2 STARS: 73%
- 1 STAR: 60%
- NO STAR: < 60%

## Controls

### Song Select
- **Up / Down**: Navigate songs
- **Left / Right**: Select chart difficulty
- **Enter**: Play selected chart
- **T**: Quick load test chart
- **Escape**: Quit game

### Gameplay
- **Lane Keys**:
  - 4-Key: D F J K (or Arrow Keys, or Z X . /)
  - 5-Key: D F Space J K
  - 6-Key: S D F J K L
  - 7-Key: S D F Space J K L
- **Space**: Pause / Resume
- **R**: Retry chart from beginning
- **F1**: Toggle debug overlay
- **F2 / F3**: Adjust scroll speed
- **F4**: Toggle Downscroll / Upscroll
- **F5**: Toggle Active Mode HUD (Normal / EX)
- **Tab**: Toggle Scroll Mod (X-Mod / C-Mod)
- **Escape**: Exit to Song Select
