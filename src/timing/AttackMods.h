// ============================================================================
// AttackMods.h — Runtime modifier state from #ATTACKS segments
// ============================================================================

#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cctype>

#include "timing/TimingData.h"

namespace sml {

/// Accumulated visual modifier state for the current frame.
struct ActiveMods {
    double drunk    = 0.0;   ///< Horizontal wave amplitude
    double dizzy    = 0.0;   ///< Spin rate
    double mini     = 0.0;   ///< Size reduction
    double beat     = 0.0;   ///< Bounce amplitude
    double reverse  = 0.0;   ///< Reverse scroll
    double blink    = 0.0;   ///< Blink rate
    
    // New Path Modifiers
    double tornado  = 0.0;   ///< Tornado horizontal expansion
    double tipsy    = 0.0;   ///< Field-wide horizontal wobble
    double bumpy    = 0.0;   ///< Vertical sine wave offset
    double confusion = 0.0;  ///< Static note rotation

    // New Speed/Scroll Modifiers
    double boost    = 0.0;   ///< Acceleration towards receptor
    double brake    = 0.0;   ///< Deceleration towards receptor
    double wave     = 0.0;   ///< Speed oscillation

    // New Visibility Modifiers
    double hidden   = 0.0;   ///< Fade out near receptor
    double sudden   = 0.0;   ///< Fade in near receptor
    double stealth  = 0.0;   ///< Complete invisibility

    [[nodiscard]] bool HasAnyEffect() const {
        return drunk != 0.0 || dizzy != 0.0 || mini != 0.0 ||
               beat != 0.0 || reverse != 0.0 || blink != 0.0 ||
               tornado != 0.0 || tipsy != 0.0 || bumpy != 0.0 ||
               confusion != 0.0 || boost != 0.0 || brake != 0.0 ||
               wave != 0.0 || hidden != 0.0 || sudden != 0.0 ||
               stealth != 0.0;
    }
};

/// Parse a single modifier string (e.g. "*1000 Dizzy", "100 Reverse", "-50 Mini")
/// and apply it to the given ActiveMods.
inline void ApplyModString(const std::string& mods_str, ActiveMods& out) {
    // Tokenize by spaces
    std::istringstream iss(mods_str);
    std::string token;
    std::vector<std::string> tokens;
    while (iss >> token) tokens.push_back(token);

    if (tokens.empty()) return;

    // Extract strength prefix if present
    double strength = 100.0;  // Default strength
    size_t mod_start = 0;

    if (!tokens.empty()) {
        const std::string& first = tokens[0];
        if (first[0] == '*' && first.size() > 1) {
            strength = 100.0;
            mod_start = 1;
        } else {
            bool is_number = true;
            for (size_t i = (first[0] == '-' ? 1 : 0); i < first.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(first[i])) && first[i] != '.') {
                    is_number = false;
                    break;
                }
            }
            if (is_number && (tokens.size() > 1 || first.find_first_of("0123456789") != std::string::npos)) {
                strength = std::stod(first);
                mod_start = 1;
            }
        }
    }

    std::string mod_name;
    for (size_t i = mod_start; i < tokens.size(); ++i) {
        if (i > mod_start) mod_name += ' ';
        mod_name += tokens[i];
    }

    std::string lower = mod_name;
    for (auto& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    bool is_disable = false;
    if (lower.size() > 3 && lower.substr(0, 3) == "no ") {
        is_disable = true;
        lower = lower.substr(3);
    }

    double value = is_disable ? 0.0 : strength;

    if (lower == "drunk")           out.drunk   = value;
    else if (lower == "dizzy")      out.dizzy   = value;
    else if (lower == "mini")       out.mini    = value;
    else if (lower == "beat")       out.beat    = value;
    else if (lower == "reverse")    out.reverse = value;
    else if (lower == "blink")      out.blink   = value;
    else if (lower == "tornado")    out.tornado = value;
    else if (lower == "tipsy")      out.tipsy   = value;
    else if (lower == "bumpy")      out.bumpy   = value;
    else if (lower == "confusion")  out.confusion = value;
    else if (lower == "boost")      out.boost   = value;
    else if (lower == "brake")      out.brake   = value;
    else if (lower == "wave")       out.wave    = value;
    else if (lower == "hidden")     out.hidden  = value;
    else if (lower == "sudden")     out.sudden  = value;
    else if (lower == "stealth")    out.stealth = value;
}

/// Evaluate all attacks at the given time, returning the accumulated modifier state.
inline ActiveMods EvaluateAttacks(const std::vector<Attack>& attacks, double play_time) {
    ActiveMods mods;

    for (const auto& atk : attacks) {
        if (atk.IsActive(play_time)) {
            ApplyModString(atk.mods, mods);
        }
    }

    return mods;
}

/// Smoothly interpolate between two ActiveMods states.
inline ActiveMods LerpActiveMods(const ActiveMods& from, const ActiveMods& to, double alpha) {
    if (alpha <= 0.0) return from;
    if (alpha >= 1.0) return to;
    
    ActiveMods out;
    auto lerp = [&](double a, double b) { return a + (b - a) * alpha; };
    
    out.drunk = lerp(from.drunk, to.drunk);
    out.dizzy = lerp(from.dizzy, to.dizzy);
    out.mini = lerp(from.mini, to.mini);
    out.beat = lerp(from.beat, to.beat);
    out.reverse = lerp(from.reverse, to.reverse);
    out.blink = lerp(from.blink, to.blink);
    out.tornado = lerp(from.tornado, to.tornado);
    out.tipsy = lerp(from.tipsy, to.tipsy);
    out.bumpy = lerp(from.bumpy, to.bumpy);
    out.confusion = lerp(from.confusion, to.confusion);
    out.boost = lerp(from.boost, to.boost);
    out.brake = lerp(from.brake, to.brake);
    out.wave = lerp(from.wave, to.wave);
    out.hidden = lerp(from.hidden, to.hidden);
    out.sudden = lerp(from.sudden, to.sudden);
    out.stealth = lerp(from.stealth, to.stealth);
    
    return out;
}

// ============================================================================
// Per-note effect calculations
// ============================================================================

/// Drunk: horizontal sine wave offset.
inline double CalcDrunkOffset(double drunk_pct, double beat, int col,
                               double play_time, double amplitude_px = 40.0) {
    if (drunk_pct == 0.0) return 0.0;
    double strength = drunk_pct / 100.0;
    double phase = beat * 1.8 + col * 1.5 + play_time * 2.0;
    return strength * amplitude_px * std::sin(phase);
}

/// Tipsy: all columns wobble horizontally in sync.
inline double CalcTipsyOffset(double tipsy_pct, double play_time, int col, double amplitude_px = 40.0) {
    if (tipsy_pct == 0.0) return 0.0;
    double strength = tipsy_pct / 100.0;
    // Tipsy is usually a slower, global wobble
    double phase = play_time * 1.5 + col * 0.4;
    return strength * amplitude_px * std::sin(phase);
}

/// Tornado: expand and contract horizontally as notes approach receptor.
inline double CalcTornadoOffset(double tornado_pct, double note_y, double receptor_y, int col, int num_cols, double amplitude_px = 64.0) {
    if (tornado_pct == 0.0) return 0.0;
    double strength = tornado_pct / 100.0;
    double dist = std::fabs(note_y - receptor_y);
    
    // Phase scales with distance from receptor
    double phase = (dist / 100.0) * M_PI;
    double offset_factor = std::sin(phase);
    
    // Each column expands differently based on its position relative to center
    double center = (num_cols - 1) / 2.0;
    double col_dist = col - center;
    
    return strength * amplitude_px * offset_factor * col_dist;
}

/// Dizzy: rotation angle based on distance from receptor.
inline double CalcDizzyAngle(double dizzy_pct, double note_y, double receptor_y) {
    if (dizzy_pct == 0.0) return 0.0;
    double strength = dizzy_pct / 100.0;
    double distance = note_y - receptor_y;
    return strength * distance * 1.5;
}

/// Confusion: static rotation offset.
inline double CalcConfusionAngle(double confusion_pct) {
    // Confusion is usually just a fixed rotation in degrees
    return confusion_pct;
}

/// Mini: scale factor for note size.
inline double CalcMiniScale(double mini_pct) {
    if (mini_pct == 0.0) return 1.0;
    return 1.0 - (mini_pct / 200.0);
}

/// Beat: vertical bounce offset.
inline double CalcBeatOffset(double beat_pct, double note_beat,
                              double current_beat, double amplitude_px = 30.0) {
    if (beat_pct == 0.0) return 0.0;
    double strength = beat_pct / 100.0;
    double frac = std::fmod(note_beat, 1.0);
    double bounce = std::fabs(std::sin(frac * M_PI));
    return strength * amplitude_px * bounce;
}

/// Bumpy: vertical sine wave offset.
inline double CalcBumpyOffset(double bumpy_pct, double note_beat, double amplitude_px = 40.0) {
    if (bumpy_pct == 0.0) return 0.0;
    double strength = bumpy_pct / 100.0;
    // Bumpy adds a vertical wobble based on beat
    double phase = note_beat * 2.0 * M_PI;
    return strength * amplitude_px * std::sin(phase);
}

/// Visibility (Hidden/Sudden/Stealth): returns alpha multiplier (0.0 to 1.0).
inline double CalcVisibilityAlpha(const ActiveMods& mods, double note_y, double receptor_y, double screen_h, bool downscroll) {
    if (mods.stealth != 0.0) return 0.0;
    
    double alpha = 1.0;
    double dist = std::fabs(note_y - receptor_y);
    
    // Hidden: Fades out near receptor
    if (mods.hidden != 0.0) {
        double hidden_strength = mods.hidden / 100.0;
        // Fade out in the middle of the screen
        double fade_dist = screen_h * 0.4;
        if (dist < fade_dist) {
            double factor = dist / fade_dist;
            alpha = std::min(alpha, 1.0 - hidden_strength * (1.0 - factor));
        }
    }
    
    // Sudden: Fades in near receptor
    if (mods.sudden != 0.0) {
        double sudden_strength = mods.sudden / 100.0;
        // Fade in from middle
        double fade_dist = screen_h * 0.6;
        if (dist > fade_dist) {
            double factor = (dist - fade_dist) / (screen_h - fade_dist);
            alpha = std::min(alpha, 1.0 - sudden_strength * factor);
        }
    }
    
    return std::clamp(alpha, 0.0, 1.0);
}

/// Blink: whether to hide the note this frame.
inline bool CalcBlinkHidden(double blink_pct, double play_time) {
    if (blink_pct < 1.0) return false;
    double cycle = std::fmod(play_time * 8.0, 1.0);
    return cycle < 0.5;
}

/// Smooth Speed/Scroll Curves (Boost/Brake/Wave)
inline double ApplyScrollCurves(const ActiveMods& mods, double dist, double screen_h) {
    if (mods.boost == 0.0 && mods.brake == 0.0 && mods.wave == 0.0) return dist;
    
    double new_dist = dist;
    double t = std::clamp(dist / screen_h, 0.0, 1.0);

    // Boost: Accelerates towards receptor (starts slow, ends fast)
    // Formula: dist = original_dist * (dist/screen_h) 
    if (mods.boost != 0.0) {
        double strength = mods.boost / 100.0;
        double boost_val = dist * t;
        new_dist = (1.0 - strength) * new_dist + strength * boost_val;
    }

    // Brake: Decelerates towards receptor (starts fast, ends slow)
    // Formula: dist = original_dist * (2 - dist/screen_h) -- simplified
    if (mods.brake != 0.0) {
        double strength = mods.brake / 100.0;
        double brake_val = dist * (2.0 - t);
        new_dist = (1.0 - strength) * new_dist + strength * brake_val;
    }

    // Wave: oscillating speed
    if (mods.wave != 0.0) {
        double strength = mods.wave / 100.0;
        double wave_val = dist + 64.0 * std::sin((dist / 64.0) * M_PI);
        new_dist = (1.0 - strength) * new_dist + strength * wave_val;
    }

    return new_dist;
}

/// Reverse: sign multiplier for scroll direction.
inline double CalcReverseSign(double reverse_pct) {
    return (reverse_pct >= 50.0) ? -1.0 : 1.0;
}

} // namespace sml
