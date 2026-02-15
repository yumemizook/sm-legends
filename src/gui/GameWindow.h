#pragma once
// ============================================================================
// GameWindow.h — SDL2 test GUI for the VSRG timing engine
//
// Two screens:
//   SONG_SELECT — Browse and select songs from a directory
//   GAMEPLAY    — Play/visualize a selected chart with audio and input
//
// Song Select Controls:
//   Up / Down    — Navigate songs
//   Left / Right — Navigate charts within a song
//   Enter        — Play selected chart
//   Escape       — Quit
//
// Gameplay Controls:
//   Lane keys    — Play notes (DFJK for 4-key, arrows also work)
//   Space        — Play / Pause (when no lane key conflicts)
//   R            — Reset to beginning
//   F2           — Adjust scroll speed up
//   F3           — Adjust scroll speed down
//   Tab          — Toggle X-Mod / C-Mod
//   F4           — Toggle downscroll
//   F1           — Toggle debug overlay
//   Escape       — Back to song select
// ============================================================================

#if HAS_SDL2

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "timing/Conductor.h"
#include "parsing/Simfile.h"
#include "parsing/NoteChart.h"
#include "rendering/NoteRenderer.h"
#include "parsing/SongScanner.h"
#include "audio/AudioEngine.h"
#include "input/InputMapper.h"
#include "gameplay/LifeMeter.h"
#include "gui/FontManager.h"

#include "gui/Color.h"

namespace sml {

/// Lane colors matching StepMania's DDR-style coloring by beat subdivision
namespace LaneColors {
    static constexpr Color QUARTER   = {0xFF, 0x40, 0x40, 0xFF};
    static constexpr Color EIGHTH    = {0x40, 0x80, 0xFF, 0xFF};
    static constexpr Color TWELFTH   = {0xC0, 0x40, 0xFF, 0xFF};
    static constexpr Color SIXTEENTH = {0x40, 0xFF, 0x40, 0xFF};
    static constexpr Color TWENTYFOURTH = {0xFF, 0x80, 0xC0, 0xFF};
    static constexpr Color THIRTYSECOND = {0xFF, 0xA0, 0x20, 0xFF};
    static constexpr Color OTHER     = {0x80, 0x80, 0x80, 0xFF};
    static constexpr Color MINE      = {0xFF, 0x00, 0x00, 0xFF};
    static constexpr Color HOLD      = {0xFF, 0xC0, 0x00, 0xFF};
    static constexpr Color ROLL      = {0xC0, 0x80, 0xFF, 0xFF};
}

/// Current screen state
enum class ScreenState {
    SONG_SELECT,
    GAMEPLAY,
    RESULTS,
    OPTIONS,
    CALIBRATION,
};

/// Clear types (per cleartypes.md)
enum class ClearType {
    NONE,
    FAIL,
    CLEAR,
    FULL_COMBO,
    FULL_COMBO_PLUS,
    ALL_PERFECT,
    ALL_PERFECT_PLUS,
    ALL_PERFECT_CRITICAL,
    ALL_PERFECT_EXTRAORDINARY
};

inline const char* ClearTypeName(ClearType ct) {
    switch (ct) {
        case ClearType::ALL_PERFECT_EXTRAORDINARY: return "ALL PERFECT EXTRAORDINARY";
        case ClearType::ALL_PERFECT_CRITICAL:    return "ALL PERFECT CRITICAL";
        case ClearType::ALL_PERFECT_PLUS:        return "ALL PERFECT+";
        case ClearType::ALL_PERFECT:             return "ALL PERFECT";
        case ClearType::FULL_COMBO_PLUS:         return "FULL COMBO+";
        case ClearType::FULL_COMBO:              return "FULL COMBO";
        case ClearType::CLEAR:                   return "CLEAR";
        case ClearType::FAIL:                    return "FAIL";
        default:                                 return "";
    }
}

/// Data for a fading hit flash silhouette
struct HitFlash {
    int lane;        ///< Which lane the note was in
    double beat;     ///< Beat of the note (for quantization row)
    double timer;    ///< Time remaining (starts at 0.3, counts down to 0)
    double hit_y;    ///< Screen Y position where the note was when hit
    Judgement judge; ///< Judgement of the hit (for coloring)
};

/// Record of a single hit for stats/graphs
struct HitRecord {
    double time;       ///< Time of hit relative to song start
    double error;      ///< Timing error (seconds), negative=early, positive=late
    Judgement judge;   ///< The judgement received
    int lane;          ///< The lane index
    bool is_ex_grade;  ///< Whether this was graded using EX standards (stored for reference)
};

/// Simple SDL2 test window for the VSRG engine.
class GameWindow {
public:
    GameWindow() = default;
    ~GameWindow();

    bool Init(int width = 900, int height = 700, const std::string& title = "SM-Legends");
    int ScanSongs(const std::string& songs_path);
    bool LoadSimfile(const std::string& filepath, int chart_index = 0);
    void LoadTestChart();
    void Run();
    void Shutdown();

private:
    // --- Event handling ---
    void HandleEvents();
    void HandleKeyDown(SDL_Keycode key);
    void HandleKeyUp(SDL_Keycode key);
    void HandleKeyDown_SongSelect(SDL_Keycode key);
    void HandleKeyDown_Gameplay(SDL_Keycode key);
    void HandleKeyUp_Gameplay(SDL_Keycode key);
    void HandleKeyDown_Results(SDL_Keycode key);
    void HandleKeyDown_Options(SDL_Keycode key);
    void HandleKeyDown_Calibration(SDL_Keycode key);

    void HandleControllerButtonDown(SDL_GameControllerButton button);
    void HandleControllerButtonUp(SDL_GameControllerButton button);

    // --- Update ---
    void Update(double dt);
    void ChangeScreen(ScreenState next);
    
    void UpdateScores(double abs_error, int num_notes);
    void UpdateClearType(Judgement j_norm, Judgement j_ex);

    // --- Hit detection ---
    void ProcessLaneHit(int lane, double forced_time = -1.0);

    // --- Rendering: dispatch ---
    void Render();

    // --- Rendering: Song Select ---
    void RenderSongSelect();
    void OnEnterSongSelect();
    void RenderSongList();
    void RenderChartPanel();
    void RenderSongSelectHUD();

    // --- Rendering: Gameplay ---
    void RenderGameplay();
    void RenderBackground();
    void RenderLanes();
    void RenderReceptors();
    void RenderNotes();
    void RenderMeasureLines();
    void RenderMasks();
    void RenderHUD();
    void RenderTopBar();
    void RenderBottomBar();
    void RenderLifeBar();
    void RenderProgressBar();
    void RenderBeatFlash();
    void RenderJudgement();
    void RenderHitFlashes();
    void RenderFailOverlay();
    void RenderModifierMenu();
    void RenderOptions();
    void RenderCalibration();

    // --- Rendering: Results ---
    void RenderResults();
    void RenderResultsPanel(int x, int y, int w, int h, const std::vector<HitRecord>& hits);
    void RenderOffsetGraph(int x, int y, int w, int h, const std::vector<HitRecord>& hits);

    // --- Transitions ---
    void StartGameplay(size_t song_index, size_t chart_index);
    void StartGameplayDirect();
    void HandleKeyDown_ModifierMenu(SDL_Keycode key);
    void ReturnToSongSelect();
    void ShowResults();

    // --- Audio helpers ---
    bool TryLoadSongAudio();

    // --- Drawing helpers ---
    Color GetNoteColor(double beat) const;
    int GetQuantizationRow(double beat) const;
    void DrawNote(int lane, double y, double beat, NoteType type);
    void DrawRect(int x, int y, int w, int h, Color color);
    void DrawRectOutline(int x, int y, int w, int h, Color color);
    void DrawText(int x, int y, const std::string& text, Color color, int scale = 2);
    int GetTextWidth(const std::string& text, int scale) const;
    void DrawDigit(int x, int y, int digit, int scale, Color color);
    void DrawChar(int x, int y, char ch, int scale, Color color);

    void SaveSettings();
    void LoadSettings();

    void ScanNoteskins();
    bool LoadNoteskin(const std::string& name = "Default");

    // --- Layout ---
    int GetLaneX(int lane) const;
    int GetLaneWidth() const;
    int GetFieldLeft() const;
    int GetFieldRight() const;

    // --- Color helpers ---
    static Color GetDifficultyColor(const std::string& diff_name);
    static Color GetJudgementColor(Judgement j, bool ex_mode = false);
    static Color GetClearTypeColor(ClearType ct);

    // --- State ---
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool          fullscreen_ = true;
    bool          running_  = false;
    ScreenState   screen_   = ScreenState::SONG_SELECT;

    // Subsystems
    SongScanner  scanner_;
    AudioEngine  audio_;
    InputMapper  input_;
    FontManager  font_;
    SDL_GameController* controller_ = nullptr;

    // Assets
    SDL_Texture* note_texture_ = nullptr;
    SDL_Texture* note_silhouette_texture_ = nullptr;
    int note_tex_w_ = 0, note_tex_h_ = 0;
    SDL_Texture* flash_texture_ = nullptr;
    int flash_tex_w_ = 0, flash_tex_h_ = 0;

    SDL_Texture* mine_texture_ = nullptr;
    int mine_tex_w_ = 0, mine_tex_h_ = 0;

    SDL_Texture* hold_body_texture_ = nullptr; // Active
    SDL_Texture* hold_cap_texture_  = nullptr; // Active
    SDL_Texture* roll_body_texture_ = nullptr;
    SDL_Texture* roll_cap_texture_  = nullptr;
    SDL_Texture* lift_texture_      = nullptr;
    int lift_tex_w_ = 0, lift_tex_h_ = 0;

    SDL_Texture* judge_normal_texture_ = nullptr;
    int judge_normal_w_ = 0, judge_normal_h_ = 0;
    SDL_Texture* judge_ex_texture_     = nullptr;
    int judge_ex_w_ = 0, judge_ex_h_ = 0;

    std::map<std::string, SDL_Texture*> normal_grade_textures_;
    std::map<int, SDL_Texture*> ex_grade_textures_;

    SDL_Texture* bg_texture_ = nullptr;

    std::map<std::string, SDL_Texture*> jacket_cache_;
    SDL_Texture* GetJacketTexture(const std::string& path);

    SDL_Texture* LoadTexture(const std::string& path, int* w, int* h, bool make_white = false);

    // Song select state
    int  selected_song_  = 0;
    int  selected_chart_ = 0;
    int  scroll_offset_  = 0;
    int  visible_songs_  = 12;
    double last_up_press_time_   = 0.0;
    double last_down_press_time_ = 0.0;
    
    // Modifier menu state
    bool   showing_modifier_menu_ = false;
    int    modifier_menu_cursor_  = 0;
    float  sudden_plus_val_       = 0.0f;
    float  hidden_plus_val_       = 0.0f;
    int    effect_mode_           = 0; // 0: None, 1: Mirror, 2: Random
    std::vector<std::string> available_noteskins_;
    int    noteskin_index_        = 0;

    // Options/Calibration state
    struct BindInfo {
        enum Type { NONE, KEY, BUTTON } type = NONE;
        int id = 0; // SDL_Keycode or SDL_GameControllerButton
    };

    int    options_lane_cursor_   = 0;
    int    options_slot_cursor_   = 0;
    int    calibration_cursor_    = 0;
    double audio_offset_          = 0.0; // Global audio offset in seconds
    bool   is_rebinding_          = false;
    BindInfo custom_binds_4k_[4][3] = {};
    
    // Calibration Sampling
    bool   is_calibrating_        = false;
    std::vector<double> cal_errors_;
    double suggested_offset_      = 0.0;
    double calibration_stdev_     = 0.0;


    // Timing engine (gameplay)
    Conductor conductor_;
    std::unique_ptr<Simfile> loaded_simfile_;
    const Simfile* active_simfile_ = nullptr;
    const NoteChart* current_chart_ = nullptr;
    NoteChart runtime_chart_; // For effects like Random
    NoteFieldConfig field_config_;

    // Playback state
    bool   playing_      = false;
    bool   audio_loaded_  = false;
    double play_time_    = 0.0;
    double last_tick_    = 0.0;
    double song_duration_ = 0.0;
    bool   show_debug_   = true;
    bool   ex_mode_      = false;
    bool   autoplay_     = false;
    double beat_flash_   = 0.0;

    // Hit detection state
    size_t next_hittable_note_ = 0;  ///< Index of the next note that can be hit
    int    total_hittable_notes_ = 0; ///< Total hittable notes in current chart
    std::vector<uint32_t> note_hit_masks_; ///< Tracking which notes in each row were hit
    std::vector<double>   row_best_error_; ///< Timing error of the last hit in each row (for chord cohesion)

    /// Check if a specific note in a row was already hit
    bool IsNoteHit(size_t row_idx, int col) const {
        if (row_idx >= note_hit_masks_.size()) return false;
        return (note_hit_masks_[row_idx] & (1 << col)) != 0;
    }

    /// Count hittable notes (Tap/HoldHead/RollHead) in a row
    int GetRowHittableCount(size_t row_idx) const {
        if (!current_chart_ || row_idx >= current_chart_->note_rows.size()) return 0;
        int count = 0;
        for (auto nt : current_chart_->note_rows[row_idx].columns) {
            if (nt == NoteType::Tap || nt == NoteType::HoldHead || nt == NoteType::RollHead)
                count++;
        }
        return count;
    }

    /// Get a bitmask of all hittable columns in a row
    uint32_t GetRowRequiredMask(size_t row_idx) const {
        if (!current_chart_ || row_idx >= current_chart_->note_rows.size()) return 0;
        uint32_t mask = 0;
        const auto& cols = current_chart_->note_rows[row_idx].columns;
        for (size_t c = 0; c < cols.size(); ++c) {
            if (cols[c] == NoteType::Tap || cols[c] == NoteType::HoldHead || cols[c] == NoteType::RollHead)
                mask |= (1u << c);
        }
        return mask;
    }

    /// Check if all hittable notes in a row have been hit
    bool IsRowFullyHit(size_t row_idx) const {
        uint32_t required = GetRowRequiredMask(row_idx);
        if (required == 0) return false;
        return (note_hit_masks_[row_idx] & required) == required;
    }

    // Combo/score
    int    combo_      = 0;
    int    max_combo_  = 0;
    int    total_hits_ = 0;
    int    total_miss_ = 0;
    int    normal_judge_counts_[9] = {};
    int    ex_judge_counts_[9] = {};
    
    double normal_score_ = 0.0;
    double ex_score_     = 0.0;
    LifeMeter life_meter_;
    ClearType clear_type_ = ClearType::NONE;

    Judgement lowest_judgement_in_combo_ = Judgement::NONE;

    // Floating judgement display (centered, most recent hit)
    Judgement last_judgement_       = Judgement::NONE;
    double    judgement_timer_     = 0.0;
    double    last_timing_error_  = 0.0;

    // Hit flash silhouettes
    std::vector<HitFlash> hit_flashes_;

    // End-of-chart detection
    bool   chart_finished_   = false;
    double chart_end_time_   = 0.0;  ///< Time of last note + buffer
    double results_delay_    = 0.0;  ///< Countdown before auto-showing results

    // Fail sequence state
    bool   failed_sequence_      = false;
    double fail_animation_timer_ = 0.0;

    // Results screen state
    std::string results_title_;
    std::string results_chart_info_;
    double      results_reveal_timer_ = 0.0;
    bool        results_ex_mode_ = false;
    std::vector<HitRecord> hit_history_;

    // Polish Effects
    double transition_timer_    = 0.0;
    double transition_duration_ = 0.4;
    ScreenState next_screen_    = ScreenState::SONG_SELECT;
    bool is_transitioning_      = false;

    double combo_pop_timer_     = 0.0;
    
    int    last_grade_milestone_ = 0; // 0=None, 1=S, 2=SS, 3=SSS
    double grade_popup_timer_    = 0.0;
    const char* grade_popup_str_ = "";

    double ready_animation_timer_ = 0.0;
    double clear_animation_timer_ = 0.0;

    struct ScoreRecord {
        double percentage = 0.0;
        double ex_score = 0.0;
        int stars = 0;
        std::string grade;
        int max_combo = 0;
    };
    std::map<std::string, ScoreRecord> high_scores_; // key: filepath + "|" + chart_index

    // Window dimensions
    int width_  = 900;
    int height_ = 700;

    // Layout constants
    static constexpr int LANE_PADDING = 2;
    static constexpr int RECEPTOR_HEIGHT = 16; // SMX-style bracket receptors need more room
    static constexpr int NOTE_HEIGHT = 14;
    static constexpr int FIELD_MARGIN = 200;
};

/// Generate a built-in test chart.
} // namespace sml

#endif // HAS_SDL2
