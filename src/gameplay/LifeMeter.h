#pragma once

#include "input/InputMapper.h"
#include <algorithm>

namespace sml {

enum class LifeType {
    STANDARD,
    LIFE4,
    RISKY,
    FLARE
};

class LifeMeter {
public:
    struct FlareDeduction {
        float perfect;
        float great;
        float good;
        float miss;
    };

    LifeMeter() = default;

    void Init(LifeType type, int flare_level = 1) {
        type_ = type;
        flare_level_ = std::clamp(flare_level, 1, 5);
        
        switch (type_) {
            case LifeType::STANDARD:
                life_val_ = 0.5f;
                battery_lives_ = 0;
                break;
            case LifeType::LIFE4:
                life_val_ = 1.0f;
                battery_lives_ = 4;
                break;
            case LifeType::RISKY:
                life_val_ = 1.0f;
                battery_lives_ = 1;
                break;
            case LifeType::FLARE:
                life_val_ = 1.0f;
                battery_lives_ = 0;
                break;
        }
        failed_ = false;
    }

    void OnJudgement(Judgement j) {
        if (failed_) return;

        switch (type_) {
            case LifeType::STANDARD:
                HandleStandard(j);
                break;
            case LifeType::LIFE4:
            case LifeType::RISKY:
                HandleBattery(j);
                break;
            case LifeType::FLARE:
                HandleFlare(j);
                break;
        }

        if (life_val_ <= 0.0f && type_ != LifeType::LIFE4 && type_ != LifeType::RISKY) {
            life_val_ = 0.0f;
            failed_ = true;
        }
        if (battery_lives_ <= 0 && (type_ == LifeType::LIFE4 || type_ == LifeType::RISKY)) {
            failed_ = true;
        }
    }

    void OnMineHit() {
        if (failed_) return;
        if (type_ == LifeType::STANDARD) {
            life_val_ -= 0.05f;
        } else if (type_ == LifeType::LIFE4 || type_ == LifeType::RISKY) {
            battery_lives_--;
        } else if (type_ == LifeType::FLARE) {
            // Mines in Flare usually deduct a lot or instant fail? 
            // Following DDR Flare rules: Mines are typically NG/Miss equivalent.
            float deduction = GetFlareDeductions().miss;
            life_val_ -= deduction;
        }
        
        if (life_val_ <= 0.0f) { life_val_ = 0.0f; failed_ = true; }
        if (battery_lives_ <= 0 && (type_ == LifeType::LIFE4 || type_ == LifeType::RISKY)) failed_ = true;
    }

    bool IsFailed() const { return failed_; }
    float GetLife() const { return life_val_; }
    int GetBatteryLives() const { return battery_lives_; }
    LifeType GetType() const { return type_; }
    int GetFlareLevel() const { return flare_level_; }

private:
    void HandleStandard(Judgement j) {
        switch (j) {
            case Judgement::PEXTRA:
            case Judgement::PCRIT:
            case Judgement::PERFECT:
            case Judgement::PERFECT_LOW: life_val_ += 0.005f; break;
            case Judgement::GREAT_HIGH:
            case Judgement::GREAT:
            case Judgement::GREAT_LOW:   life_val_ += 0.002f; break;
            case Judgement::GOOD:        life_val_ -= 0.01f; break;
            case Judgement::MISS:        life_val_ -= 0.05f; break;
            default: break;
        }
        life_val_ = std::min(1.0f, life_val_);
    }

    void HandleBattery(Judgement j) {
        if (j == Judgement::MISS) {
            battery_lives_--;
        }
    }

    void HandleFlare(Judgement j) {
        FlareDeduction d = GetFlareDeductions();
        switch (j) {
            case Judgement::PEXTRA:
            case Judgement::PCRIT:      break; // Safe
            case Judgement::PERFECT:    
            case Judgement::PERFECT_LOW: life_val_ -= d.perfect; break;
            case Judgement::GREAT_HIGH:
            case Judgement::GREAT:
            case Judgement::GREAT_LOW:   life_val_ -= d.great; break;
            case Judgement::GOOD:        life_val_ -= d.good; break;
            case Judgement::MISS:        life_val_ -= d.miss; break;
            default: break;
        }
    }

    FlareDeduction GetFlareDeductions() const {
        switch (flare_level_) {
            case 1: return {0.0000f, 0.0020f, 0.0010f, 0.1000f}; // Flare I
            case 2: return {0.0000f, 0.0056f, 0.0280f, 0.1400f}; // Flare IV
            case 3: return {0.0000f, 0.0128f, 0.0640f, 0.2200f}; // Flare VII
            case 4: return {0.0000f, 0.0200f, 0.1000f, 0.3000f}; // Flare IX
            case 5: return {0.0150f, 0.0300f, 0.1200f, 0.3600f}; // Flare NEO
            default: return {0.0f, 0.0f, 0.0f, 0.1f};
        }
    }

    LifeType type_ = LifeType::STANDARD;
    float life_val_ = 0.5f;
    int battery_lives_ = 0;
    int flare_level_ = 1;
    bool failed_ = false;
};

} // namespace sml
