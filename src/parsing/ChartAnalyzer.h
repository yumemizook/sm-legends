#pragma once

#include "parsing/NoteChart.h"
#include "parsing/Simfile.h"

namespace sml {


class ChartAnalyzer {
public:
    static RadarValues CalculateRadar(const NoteChart& chart, const Simfile* simfile);
    static double CalculateCustomDifficulty(const NoteChart& chart, const RadarValues& radar);
    static ChartVariant DetectChartVariant(const NoteChart& chart);
};

} // namespace sml
