#ifndef SML_GUI_COLOR_H
#define SML_GUI_COLOR_H

#include <cstdint>
#include <cmath>
#include <algorithm>

namespace sml {

struct Color {
    uint8_t r, g, b, a;

    static Color FromHex(uint32_t hex, uint8_t alpha = 255) {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            alpha
        };
    }

    static Color FromHSV(float h, float s, float v) {
        // h: 0-360, s: 0-1, v: 0-1
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2.0f) - 1));
        float m = v - c;
        
        float r=0, g=0, b=0;
        if (h < 60) { r=c; g=x; b=0; }
        else if (h < 120) { r=x; g=c; b=0; }
        else if (h < 180) { r=0; g=c; b=x; }
        else if (h < 240) { r=0; g=x; b=c; }
        else if (h < 300) { r=x; g=0; b=c; }
        else { r=c; g=0; b=x; }
        
        return {
            static_cast<uint8_t>((r + m) * 255),
            static_cast<uint8_t>((g + m) * 255),
            static_cast<uint8_t>((b + m) * 255),
            255
        };
    }

    static Color Rainbow(double time, float saturation = 0.5f, float value = 0.5f) {
         // CycleHue
         float h = std::fmod(time * 50.0, 360.0);
         return FromHSV(h, saturation, value);
    }
};

} // namespace sml

#endif // SML_GUI_COLOR_H
