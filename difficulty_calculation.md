# Difficulty Rating Calculation

The difficulty rating of a chart is calculated based on five key metrics (Stream, Voltage, Air, Freeze, Chaos). These metrics are derived from the chart's structure and density, and then combined into a final difficulty rating.

This document details the algorithm found in `src/parsing/ChartAnalyzer.cpp`.

## Core Metrics (Radar Values)

The system calculates 5 "Radar" values. Each metric is individually clamped to a minimum of **0.0** to handle cases where negative offsets would result in negative values for very easy charts.

### 1. Stream (Density)
Measures the consistent density of notes over time.
- **Calculation**:
    - The song is analyzed in 1-second overlapping windows (stepped by 0.5s).
    - For each window, the Notes Per Second (NPS) is calculated.
    - "Stream" is the average NPS of all "busy" windows.
        - A window is considered "busy" if its NPS > 50% of the peak NPS OR if its NPS > 4.0.
- **Formula**: `Stream Rating = Average Stream NPS * 3.8 - 6.0`

### 2. Voltage (Peak Density)
Measures the highest peak of note density.
- **Calculation**:
    - Uses the same windowed NPS analysis as Stream.
    - Identifies the maximum NPS achieved in any window.
- **Formula**: `Voltage Rating = Peak NPS * 1.8 - 4.0`

### 3. Air (Jumps)
Measures the frequency of jumps (2 or more notes at the same time).
- **Calculation**:
    - Jumps Per Second (JPS) is calculated: `Total Jumps / Song Duration`.
- **Formula**: `Air Rating = JPS * 15.0 - 5.0`

### 4. Freeze (Holds/Rolls)
Measures the frequency of hold and roll notes.
- **Calculation**:
    - Holds Per Second (HPS) is calculated: `Total Holds / Song Duration`.
- **Formula**: `Freeze Rating = HPS * 30.0 - 5.0`

### 5. Chaos (Irregular Rhythms)
Measures the complexity of the rhythms, specifically notes that do not fall on standard 4th, 8th, or 16th beats.
- **Calculation**:
    - The algorithm checks every note's beat position.
    - A note is considered "Simple" if it falls on a 4th, 8th, or 16th beat (quantization).
    - Any note falling on other beat fractions (12ths, 24ths, 32nds, etc.) is considered "Complex".
    - `Chaos Ratio` = `Total Complex Notes / Total Notes`.
- **Formula**: `Chaos Rating = Chaos Ratio * 75.0 - 10.0`

## Final Difficulty Rating

The final difficulty rating is a weighted aggregation of the five radar values. It heavily favors the single hardest aspect of the chart.

- **Inputs**: The 5 calculated radar values (Stream, Voltage, Air, Freeze, Chaos).
- **Aggregation**:
    - **Max Value**: The highest of the 5 radar values.
    - **Average Value**: The arithmetic mean of all 5 radar values.
- **Formula**: `Rating = (Max Value * 0.95) + (Average Value * 0.05)`
- **Clamping**: The result is clamped between 1.0 and 30.4.
- **Rounding**: The result is rounded to 1 decimal place.

### Summary of Scaling Factors
| Metric | Source Unit | Formula |
| :--- | :--- | :--- |
| **Stream** | Avg NPS (Busy Windows) | `Value * 3.8 - 6.0` |
| **Voltage** | Peak NPS | `Value * 1.8 - 4.0` |
| **Air** | Jumps Per Second | `Value * 15.0 - 5.0` |
| **Freeze** | Holds Per Second | `Value * 30.0 - 5.0` |
| **Chaos** | % of Off-Beat Notes | `Value * 75.0 - 10.0` |
