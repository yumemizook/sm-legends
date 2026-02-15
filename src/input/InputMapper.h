#pragma once
// ============================================================================
// InputMapper.h — Keyboard-to-lane mapping for different chart types
//
// Maps SDL keycodes to column indices. Default bindings follow SM conventions.
// ============================================================================

#if HAS_SDL2

#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace sml {

/// Timing judgement windows (in seconds, one-sided)
/// Custom grading system — see grade.md for reference
namespace JudgeWindows {
    static constexpr double PCRIT       = 0.01667; // 16.67ms P-Critical
    static constexpr double PERFECT     = 0.03333; // 33.33ms Perfect
    static constexpr double PERFECT_LOW = 0.050;   // 50ms    Perfect(Low)
    static constexpr double GREAT_HIGH  = 0.06667; // 66.67ms Great(High)
    static constexpr double GREAT       = 0.080;   // 80ms    Great
    static constexpr double GREAT_LOW   = 0.100;   // 100ms   Great(Low)
    static constexpr double GOOD        = 0.166;   // 166ms   Good
    static constexpr double MISS        = 0.200;   // 200ms   (auto-miss window)
}

/// EX mode timing windows (per EX.md)
namespace JudgeWindowsEX {
    static constexpr double PEXTRA      = 0.010;   // 10ms    P-Extraordinary
    static constexpr double PCRIT       = 0.01667; // 16.67ms P-Critical
    static constexpr double PERFECT     = 0.03333; // 33.33ms Perfect
    static constexpr double PERFECT_LOW = 0.050;   // 50ms    Perfect(Low)
    static constexpr double OKAY_HIGH   = 0.06667; // 66.67ms Okay(High)
    static constexpr double OKAY        = 0.080;   // 80ms    Okay
    static constexpr double OKAY_LOW    = 0.100;   // 100ms   Okay(Low)
    static constexpr double MISS        = 0.200;   // 200ms
}

/// Judge score weights (percentage points per note, out of 101 max)
namespace JudgeWeights {
    static constexpr double PCRIT       = 101.0;
    static constexpr double PERFECT     = 101.0; // Per grade.md: P-Critical and Perfect are both 101%
    static constexpr double PERFECT_LOW = 100.0;
    static constexpr double GREAT_HIGH  =  85.0;
    static constexpr double GREAT       =  80.0;
    static constexpr double GREAT_LOW   =  75.0;
    static constexpr double GOOD        =  50.0;
    static constexpr double MISS        =   0.0;
}

/// EX mode score weights (per EX.md)
namespace JudgeWeightsEX {
    static constexpr double PEXTRA      = 100.0;
    static constexpr double PCRIT       =  90.0;
    static constexpr double PERFECT     =  75.0;
    static constexpr double PERFECT_LOW =  70.0;
    static constexpr double OKAY_HIGH   =  40.0;
    static constexpr double OKAY        =  20.0;
    static constexpr double OKAY_LOW    =   5.0;
    static constexpr double MISS        =   0.0;
}

/// Judgement result for a hit
enum class Judgement {
    NONE,
    PEXTRA,         // P-Extraordinary (EX only)
    PCRIT,          // P-Critical — white flash ("PERFECT!!")
    PERFECT,        // Perfect — yellow
    PERFECT_LOW,    // Perfect(Low) — light yellow
    GREAT_HIGH,     // Great(High) / Okay(High)
    GREAT,          // Great / Okay
    GREAT_LOW,      // Great(Low) / Okay(Low)
    GOOD,           // Good — blue (Normal only)
    MISS            // Miss — red
};

/// Get a display name for a judgement (defaulting to Normal mode names)
inline const char* JudgementName(Judgement j, bool ex_mode = false) {
    switch (j) {
        case Judgement::PEXTRA:      return "EXTRA!!";
        case Judgement::PCRIT:       return "PERFECT!!";
        case Judgement::PERFECT:     return "PERFECT";
        case Judgement::PERFECT_LOW: return "PERFECT";
        case Judgement::GREAT_HIGH:  return ex_mode ? "OKAY" : "GREAT";
        case Judgement::GREAT:       return ex_mode ? "OKAY" : "GREAT";
        case Judgement::GREAT_LOW:   return ex_mode ? "OKAY" : "GREAT";
        case Judgement::GOOD:        return "GOOD";
        case Judgement::MISS:        return "MISS";
        default:                     return "";
    }
}

/// Get the score weight for a judgement
inline double JudgeWeight(Judgement j, bool ex_mode = false) {
    if (ex_mode) {
        switch (j) {
            case Judgement::PEXTRA:      return JudgeWeightsEX::PEXTRA;
            case Judgement::PCRIT:       return JudgeWeightsEX::PCRIT;
            case Judgement::PERFECT:     return JudgeWeightsEX::PERFECT;
            case Judgement::PERFECT_LOW: return JudgeWeightsEX::PERFECT_LOW;
            case Judgement::GREAT_HIGH:  return JudgeWeightsEX::OKAY_HIGH;
            case Judgement::GREAT:       return JudgeWeightsEX::OKAY;
            case Judgement::GREAT_LOW:   return JudgeWeightsEX::OKAY_LOW;
            default:                     return 0.0;
        }
    } else {
        switch (j) {
            case Judgement::PCRIT:       return JudgeWeights::PCRIT;
            case Judgement::PERFECT:     return JudgeWeights::PERFECT;
            case Judgement::PERFECT_LOW: return JudgeWeights::PERFECT_LOW;
            case Judgement::GREAT_HIGH:  return JudgeWeights::GREAT_HIGH;
            case Judgement::GREAT:       return JudgeWeights::GREAT;
            case Judgement::GREAT_LOW:   return JudgeWeights::GREAT_LOW;
            case Judgement::GOOD:        return JudgeWeights::GOOD;
            case Judgement::MISS:        return JudgeWeights::MISS;
            default:                     return 0.0;
        }
    }
}

/// Classify a timing error (seconds) into a judgement
inline Judgement ClassifyHit(double abs_error, bool ex_mode = false) {
    if (ex_mode) {
        if (abs_error <= JudgeWindowsEX::PEXTRA)      return Judgement::PEXTRA;
        if (abs_error <= JudgeWindowsEX::PCRIT)       return Judgement::PCRIT;
        if (abs_error <= JudgeWindowsEX::PERFECT)     return Judgement::PERFECT;
        if (abs_error <= JudgeWindowsEX::PERFECT_LOW) return Judgement::PERFECT_LOW;
        if (abs_error <= JudgeWindowsEX::OKAY_HIGH)   return Judgement::GREAT_HIGH;
        if (abs_error <= JudgeWindowsEX::OKAY)        return Judgement::GREAT;
        if (abs_error <= JudgeWindowsEX::OKAY_LOW)    return Judgement::GREAT_LOW;
        return Judgement::MISS;
    } else {
        if (abs_error <= JudgeWindows::PCRIT)       return Judgement::PCRIT;
        if (abs_error <= JudgeWindows::PERFECT)     return Judgement::PERFECT;
        if (abs_error <= JudgeWindows::PERFECT_LOW) return Judgement::PERFECT_LOW;
        if (abs_error <= JudgeWindows::GREAT_HIGH)  return Judgement::GREAT_HIGH;
        if (abs_error <= JudgeWindows::GREAT)       return Judgement::GREAT;
        if (abs_error <= JudgeWindows::GREAT_LOW)   return Judgement::GREAT_LOW;
        if (abs_error <= JudgeWindows::GOOD)        return Judgement::GOOD;
        return Judgement::MISS;
    }
}

/// Per-lane state for input and visual feedback
struct LaneState {
    bool     pressed = false;       ///< Key is currently held down
    double   press_flash = 0.0;     ///< Flash intensity (decays each frame)

    // Last hit info (for judgement display)
    Judgement last_judgement = Judgement::NONE;
    double    judgement_timer = 0.0; ///< Time remaining to display judgement
    double    last_error = 0.0;     ///< Timing error of last hit (signed, seconds)
};

/// Key binding for a single column
struct KeyBinding {
    SDL_Keycode key;
    int column;
};

/// Input mapper: keyboard keys → lane indices.
/// Also tracks per-lane state (pressed, flash) and hit detection.
class InputMapper {
public:
    InputMapper() = default;

    /// Configure key bindings for a given chart type and column count.
    /// Uses sensible defaults based on SM conventions.
    void Configure(const std::string& chart_type, int num_columns);

    /// Set custom key bindings (overrides defaults).
    void SetBindings(const std::vector<KeyBinding>& bindings);

    /// Set custom gamepad button bindings.
    void SetButtonBindings(const std::unordered_map<SDL_GameControllerButton, int>& bindings);

    /// Get current key bindings for a column (first found).
    SDL_Keycode GetKeyBinding(int column) const;

    /// Get current button binding for a column (first found).
    SDL_GameControllerButton GetButtonBinding(int column) const;

    /// Process a key-down event. Returns the lane index hit (-1 if unmapped).
    int OnKeyDown(SDL_Keycode key);

    /// Process a key-up event. Returns the lane index released (-1 if unmapped).
    int OnKeyUp(SDL_Keycode key);

    /// Process a gamepad button-down event.
    int OnButtonDown(SDL_GameControllerButton button);

    /// Process a gamepad button-up event.
    int OnButtonUp(SDL_GameControllerButton button);

    /// Update timers (call each frame with delta time).
    void Update(double dt);

    /// Reset all lane states.
    void Reset();

    /// Get the lane state for a column.
    [[nodiscard]] const LaneState& GetLaneState(int column) const;

    /// Record a hit judgement for a lane (sets flash and judgement display).
    void RecordHit(int column, Judgement judgement, double error);

    /// Get the number of mapped columns.
    [[nodiscard]] int GetNumColumns() const { return num_columns_; }

    /// Check if a keycode is mapped to a lane.
    [[nodiscard]] bool IsLaneKey(SDL_Keycode key) const;

private:
    std::unordered_map<SDL_Keycode, int> key_to_column_;
    std::unordered_map<SDL_GameControllerButton, int> button_to_column_;
    std::vector<LaneState> lanes_;
    int num_columns_ = 0;

    static constexpr double FLASH_DURATION    = 0.08;   ///< Receptor flash duration
    static constexpr double JUDGEMENT_DISPLAY  = 0.6;    ///< Judgement text display time
};

// ============================================================================
// Implementation (header-only for simplicity)
// ============================================================================

inline void InputMapper::Configure(const std::string& chart_type, int num_columns) {
    key_to_column_.clear();
    num_columns_ = num_columns;
    lanes_.resize(static_cast<size_t>(num_columns));
    Reset();

    // Default key bindings based on chart type and column count
    std::vector<SDL_Keycode> keys;

    if (num_columns == 4) {
        // dance-single: D F J K (most common SM binding)
        keys = { SDLK_d, SDLK_f, SDLK_j, SDLK_k };
    } else if (num_columns == 5) {
        // pump-single or 5-key: D F SPACE J K
        keys = { SDLK_d, SDLK_f, SDLK_SPACE, SDLK_j, SDLK_k };
    } else if (num_columns == 6) {
        // dance-solo: S D F J K L
        keys = { SDLK_s, SDLK_d, SDLK_f, SDLK_j, SDLK_k, SDLK_l };
    } else if (num_columns == 7) {
        // 7-key (BMS style): S D F SPACE J K L
        keys = { SDLK_s, SDLK_d, SDLK_f, SDLK_SPACE, SDLK_j, SDLK_k, SDLK_l };
    } else if (num_columns == 8) {
        // dance-double: A S D F J K L SEMICOLON
        keys = { SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON };
    } else if (num_columns == 9) {
        // 9-key: A S D F SPACE J K L SEMICOLON
        keys = { SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_SPACE, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON };
    } else if (num_columns == 10) {
        // 10-key: A S D F G H J K L SEMICOLON
        keys = { SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON };
    } else {
        // Generic fallback: 1 2 3 4 5 6 7 8 9 0
        for (int i = 0; i < num_columns && i < 10; ++i) {
            keys.push_back(static_cast<SDL_Keycode>(SDLK_1 + i));
        }
    }

    // Map keys to columns
    for (size_t i = 0; i < keys.size() && static_cast<int>(i) < num_columns; ++i) {
        key_to_column_[keys[i]] = static_cast<int>(i);
    }

    // Also support alternative keys for 4-key
    if (num_columns == 4) {
        // Arrow keys: Left Down Up Right (DDR convention)
        key_to_column_[SDLK_LEFT]  = 0;
        key_to_column_[SDLK_DOWN]  = 1;
        key_to_column_[SDLK_UP]    = 2;
        key_to_column_[SDLK_RIGHT] = 3;

        // ZX./ keys
        key_to_column_[SDLK_z]      = 0;
        key_to_column_[SDLK_x]      = 1;
        key_to_column_[SDLK_PERIOD] = 2;
        key_to_column_[SDLK_SLASH]  = 3;
    }
}

inline void InputMapper::SetBindings(const std::vector<KeyBinding>& bindings) {
    key_to_column_.clear();
    for (const auto& b : bindings) {
        key_to_column_[b.key] = b.column;
    }
}

inline void InputMapper::SetButtonBindings(const std::unordered_map<SDL_GameControllerButton, int>& bindings) {
    button_to_column_ = bindings;
}

inline SDL_Keycode InputMapper::GetKeyBinding(int column) const {
    for (const auto& pair : key_to_column_) {
        if (pair.second == column) return pair.first;
    }
    return SDLK_UNKNOWN;
}

inline SDL_GameControllerButton InputMapper::GetButtonBinding(int column) const {
    for (const auto& pair : button_to_column_) {
        if (pair.second == column) return pair.first;
    }
    return SDL_CONTROLLER_BUTTON_INVALID;
}

inline int InputMapper::OnKeyDown(SDL_Keycode key) {
    auto it = key_to_column_.find(key);
    if (it == key_to_column_.end()) return -1;

    int col = it->second;
    if (col >= 0 && col < num_columns_) {
        lanes_[static_cast<size_t>(col)].pressed = true;
        lanes_[static_cast<size_t>(col)].press_flash = 1.0;
    }
    return col;
}

inline int InputMapper::OnKeyUp(SDL_Keycode key) {
    auto it = key_to_column_.find(key);
    if (it == key_to_column_.end()) return -1;

    int col = it->second;
    if (col >= 0 && col < num_columns_) {
        lanes_[static_cast<size_t>(col)].pressed = false;
    }
    return col;
}

inline int InputMapper::OnButtonDown(SDL_GameControllerButton button) {
    auto it = button_to_column_.find(button);
    if (it == button_to_column_.end()) return -1;

    int col = it->second;
    if (col >= 0 && col < num_columns_) {
        lanes_[static_cast<size_t>(col)].pressed = true;
        lanes_[static_cast<size_t>(col)].press_flash = 1.0;
    }
    return col;
}

inline int InputMapper::OnButtonUp(SDL_GameControllerButton button) {
    auto it = button_to_column_.find(button);
    if (it == button_to_column_.end()) return -1;

    int col = it->second;
    if (col >= 0 && col < num_columns_) {
        lanes_[static_cast<size_t>(col)].pressed = false;
    }
    return col;
}

inline void InputMapper::Update(double dt) {
    for (auto& lane : lanes_) {
        // Decay flash
        if (lane.press_flash > 0.0) {
            lane.press_flash -= dt / FLASH_DURATION;
            if (lane.press_flash < 0.0) lane.press_flash = 0.0;
        }
        // Decay judgement display
        if (lane.judgement_timer > 0.0) {
            lane.judgement_timer -= dt;
            if (lane.judgement_timer < 0.0) lane.judgement_timer = 0.0;
        }
    }
}

inline void InputMapper::Reset() {
    for (auto& lane : lanes_) {
        lane = LaneState{};
    }
}

inline const LaneState& InputMapper::GetLaneState(int column) const {
    static const LaneState empty;
    if (column < 0 || column >= num_columns_) return empty;
    return lanes_[static_cast<size_t>(column)];
}

inline void InputMapper::RecordHit(int column, Judgement judgement, double error) {
    if (column < 0 || column >= num_columns_) return;
    auto& lane = lanes_[static_cast<size_t>(column)];
    lane.last_judgement = judgement;
    lane.judgement_timer = JUDGEMENT_DISPLAY;
    lane.last_error = error;
    lane.press_flash = 1.0; // Extra flash on hit
}

inline bool InputMapper::IsLaneKey(SDL_Keycode key) const {
    return key_to_column_.find(key) != key_to_column_.end();
}

} // namespace sml

#endif // HAS_SDL2
