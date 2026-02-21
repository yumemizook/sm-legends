#pragma once
// ============================================================================
// GameWindow.h 窶・SDL2 test GUI for the VSRG timing engine
//
// Two screens:
//   SONG_SELECT 窶・Browse and select songs from a directory
//   GAMEPLAY    窶・Play/visualize a selected chart with audio and input
//
// Song Select Controls:
//   Up / Down    窶・Navigate songs
//   Left / Right 窶・Navigate charts within a song
//   Enter        窶・Play selected chart
//   Escape       窶・Quit
//
// Gameplay Controls:
//   Lane keys    窶・Play notes (DFJK for 4-key, arrows also work)
//   Space        窶・Play / Pause (when no lane key conflicts)
//   R            窶・Reset to beginning
//   F2           窶・Adjust scroll speed up
//   F3           窶・Adjust scroll speed down
//   Tab          窶・Toggle X-Mod / C-Mod
//   F4           窶・Toggle downscroll
//   F1           窶・Toggle debug overlay
//   Escape       窶・Back to song select
// ============================================================================

#if HAS_SDL2

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstring>

#include "timing/Conductor.h"
#include "parsing/Simfile.h"
#include "parsing/NoteChart.h"
#include "rendering/NoteRenderer.h"
#include "parsing/SongScanner.h"
#include "audio/AudioEngine.h"
#include "input/InputMapper.h"
#include "video/VideoDecoder.h" // Video Support
#include "gameplay/LifeMeter.h"
#include "gui/FontManager.h"
#include "core/Profile.h"
#include "timing/AttackMods.h"

#include "gui/Color.h"

namespace sml {

// Shatter particle for RISKY gauge destruction
struct ShatterParticle {
    float x, y;           // position
    float vx, vy;         // velocity
    float rot, rot_v;     // rotation angle and angular velocity
    float w, h;           // shard size
    Color c;              // color
    float alpha = 255.0f; // fade
};

enum class ShatterState { NONE, CRACKING, SHATTERING };

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
    ATTRACTION,
    PROFILE_LOAD,
    PROFILE_MANAGE,
    SONG_SELECT,
    DECIDE,
    GAMEPLAY,
    RESULTS,
    OPTIONS,
    CALIBRATION,
};

enum class ComboDisplayMode {
    None, Combo, AdditiveScore, SubtractiveScore,
    DiffS, DiffS_Plus, DiffSS, DiffSS_Plus, DiffSSS, DiffSSS_Plus,
    AdditiveEX, SubtractiveEX,
    DiffEX_1Star, DiffEX_2Star, DiffEX_3Star, DiffEX_4Star, DiffEX_5Star, DiffEX_6Star,
    HitOffset
};

enum class BGABrightness { LIGHTER, NORMAL, DARK, DARKER, DARKEST };

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

/// Visual indicator at the end of a Hold or Roll
struct HoldIndicator {
    int lane;
    int grade;   ///< 0=Good, 1=NG, 2=Bad
    double timer; ///< Total lifetime ~0.6s
};

/// Active state of a Hold or Roll note
struct ActiveHold {
    size_t row_index;       ///< Index in NoteChart::note_rows
    int col_index;          ///< Lane index
    NoteType type;          ///< HoldHead or RollHead
    
    double start_time;      ///< Time the head was hit
    double end_time;        ///< Time the tail ends
    double total_duration;  ///< end_time - start_time
    
    // Hold Specifics
    double time_held;            ///< Accumulated time the button was held
    double current_release_time; ///< How long it has been released *currently*
    bool broken_continuity;      ///< If release time ever exceeded 250ms
    
    // Roll Specifics
    int required_ticks;             ///< Total ticks required (one per beat)
    int ticks_hit;                  ///< Count of fresh taps registered during roll
    
    // Autoplay / Logic
    double start_beat;              ///< Beat where the hold started
    double last_autoplay_tick_beat; ///< Last beat where autoplay registered a roll hit
};

/// Per-player state for 2P support
struct PlayerState {
    bool   joined     = false;  ///< Whether this player slot is active
    int    player_num = 0;      ///< 0 = P1, 1 = P2

    // Input
    InputMapper input;

    // Profile
    Profile profile;

    // Scoring
    int    combo      = 0;
    int    max_combo  = 0;
    int    total_hits = 0;
    int    total_miss = 0;
    int    normal_judge_counts[9] = {};
    int    ex_judge_counts[9]     = {};
    double normal_score = 0.0;
    double ex_score     = 0.0;
    int    hold_judgement_counts[3] = {}; // 0=Good, 1=NG, 2=Bad
    LifeMeter life_meter;
    ClearType clear_type = ClearType::NONE;
    Judgement lowest_judgement_in_combo = Judgement::NONE;
    int    total_hittable_notes = 0;

    // Floating judgement display
    Judgement last_judgement      = Judgement::NONE;
    double    judgement_timer     = 0.0;
    double    last_timing_error   = 0.0;
    std::string last_combo_text;
    double    score_anim_timer    = 0.0;

    // Hit detection
    size_t next_hittable_note      = 0;
    std::vector<uint32_t> note_hit_masks;
    std::vector<double>   row_best_error;
    std::vector<ActiveHold> active_holds; // Track active holds/rolls

    // Hit flash silhouettes
    std::vector<HitFlash> hit_flashes;
    std::vector<HoldIndicator> hold_indicators;    // End-of-hold visual status

    // Records
    std::vector<HitRecord> hit_history;

    // Fail sequence
    bool   failed_sequence      = false;
    double fail_animation_timer = 0.0;
    
    double clear_animation_timer = 0.0;
    
    // Independent chart state
    const NoteChart* current_chart = nullptr;
    NoteChart runtime_chart;
    double chart_end_time = 0.0;
    bool chart_finished = false;
    bool ready = false;
    double results_delay = 0.0;
    double last_up_press_time = 0.0;
    double last_down_press_time = 0.0;

    // Visual polish
    double combo_pop_timer       = 0.0;
    int    last_grade_milestone  = 0;
    double grade_popup_timer     = 0.0;
    const char* grade_popup_str  = "";

    // Life gauge animations
    float  displayed_life        = 0.8f;
    int    last_active_segments  = 16;
    double segment_flash_timers[20] = {0};
    int    last_battery_lives    = 0;
    double battery_flash_timers[4] = {0};
    double flare_flash_timer     = 0.0;
    float  last_flare_life       = 1.0f;

    // RISKY shatter animation
    ShatterState shatter_state   = ShatterState::NONE;
    double shatter_timer         = 0.0;
    std::vector<ShatterParticle> shatter_particles;

    // NoteFieldConfig per-player (for split screen)
    NoteFieldConfig field_config;
    int field_x_offset = 0; ///< Horizontal pixel offset for this player's notefield

    // Modifier menu state (per-player)
    bool   showing_modifier_menu = false;
    int    modifier_menu_cursor  = 0;
    float  sudden_plus_val       = 0.0f;
    float  hidden_plus_val       = 0.0f;
    int    effect_mode           = 0; // 0: None, 1: Mirror, 2: Random
    int    noteskin_index        = 0;
    bool   ex_mode               = false;
    ComboDisplayMode combo_display_mode = ComboDisplayMode::Combo;
    BGABrightness bga_brightness = BGABrightness::NORMAL;
    ActiveMods   smoothed_mods; // For seamless transitions

    // Animation state
    double modifier_menu_anim = 0.0; // 0.0 to 1.0 (open/close)

    /// Reset all gameplay state for a new chart
    void ResetGameplay() {
        combo = 0;
        max_combo = 0;
        total_hits = 0;
        total_miss = 0;
        normal_score = 0.0;
        ex_score = 0.0;
        last_judgement = Judgement::NONE;
        judgement_timer = 0.0;
        last_timing_error = 0.0;
        last_grade_milestone = 0;
        grade_popup_timer = 0.0;
        failed_sequence = false;
        fail_animation_timer = 0.0;
        clear_animation_timer = 0.0;
        clear_type = ClearType::ALL_PERFECT_EXTRAORDINARY;
        lowest_judgement_in_combo = Judgement::NONE;
        combo_pop_timer = 0.0;
        next_hittable_note = 0;
        total_hittable_notes = 0;
        current_chart = nullptr;
        chart_finished = false;
        results_delay = 1.5;
        note_hit_masks.clear();
        row_best_error.clear();
        active_holds.clear(); // Clear active holds
        hit_flashes.clear();
        hold_indicators.clear();
        hit_history.clear();
        std::memset(normal_judge_counts, 0, sizeof(normal_judge_counts));
        std::memset(ex_judge_counts, 0, sizeof(ex_judge_counts));
        std::memset(hold_judgement_counts, 0, sizeof(hold_judgement_counts));
        input.Reset();
        modifier_menu_anim = 0.0;
        showing_modifier_menu = false;
        displayed_life = 0.0f;
        last_active_segments = 0;
        std::memset(segment_flash_timers, 0, sizeof(segment_flash_timers));
        last_battery_lives = 0;
        std::memset(battery_flash_timers, 0, sizeof(battery_flash_timers));
        flare_flash_timer = 0.0;
        last_flare_life = 1.0f;
        shatter_state = ShatterState::NONE;
        shatter_timer = 0.0;
        shatter_particles.clear();
    }
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
    void HandleKeyDown_Attraction(SDL_Keycode key);
    void HandleKeyDown_ProfileLoad(SDL_Keycode key);
    void HandleKeyDown_SongSelect(SDL_Keycode key);
    void HandleKeyDown_Gameplay(SDL_Keycode key);
    void HandleKeyUp_Gameplay(SDL_Keycode key);
    void HandleKeyDown_Results(SDL_Keycode key);
    void HandleKeyDown_Options(SDL_Keycode key);
    void HandleKeyDown_Calibration(SDL_Keycode key);
    void HandleKeyDown_ModifierMenu(int p, SDL_Keycode key);
    void HandleModifierMenuInput(int p, SDL_Keycode key);

    void HandleControllerButtonDown(SDL_GameControllerButton button);
    void HandleControllerButtonUp(SDL_GameControllerButton button);

    // --- Animation & Transition state ---
    double screen_transition_timer_ = 1.0; 
    double global_anim_timer_      = 0.0;
    ScreenState target_screen_     = ScreenState::ATTRACTION;
    bool is_transitioning_         = false;

    // --- Update ---
    void Update(double dt);
    void ChangeScreen(ScreenState next);
    
    void UpdateScores(double abs_error, int num_notes, int player_idx = 0);
    void UpdateClearType(Judgement j_norm, Judgement j_ex, int player_idx = 0);

    // --- Hit detection ---
    void ProcessLaneHit(int lane, double forced_time = -1.0, int player_idx = 0, bool is_release = false);

    // --- Rendering: dispatch ---
    void Render();

    // --- Rendering: Attraction / Profile Load / Profile Manage ---
    void RenderAttraction();
    void RenderProfileLoad();
    void RenderProfileManage();
    void UpdateAttraction(double dt);
    void UpdateProfileLoad(double dt);
    void HandleKeyDown_ProfileManage(SDL_Keycode key);

    // --- Profile helpers ---
    void ScanLocalProfiles();
    void SaveActiveProfile(int player_idx);
    void ApplyProfileMods(int player_idx);
    
    struct RatingStyle {
        Color main_color;
        Color outline_color;
        bool  is_rainbow = false;
        int   rainbow_type = 0; // 0=none, 1=rainbow, 2=rainbow2
    };
    RatingStyle GetRatingStyle(double rating);
    void RenderRating(int x, int y, double rating, TextAlign align, bool is_p2);
    static std::string FormatMeter(double d);

    // --- Rendering: Song Select ---
    void RenderSongSelect();
    void OnEnterSongSelect();
    void RenderSongList();
    void RenderChartPanel(int p);
    void RenderSongSelectHUD();
    void RenderModifierMenu(int p);
    std::string GetComboDisplayText(int p);

    // --- Rendering: Gameplay ---
    void RenderDecide();
    void RenderGameplay();
    void RenderBackground();
    void RenderLanes();
    void RenderReceptors(const NoteFieldConfig& cfg, const ActiveMods& mods);
    void RenderHoldIndicators(int p, const NoteFieldConfig& cfg, const ActiveMods& mods);
    void RenderNotes(const NoteFieldConfig& cfg, const ActiveMods& mods);
    void RenderMeasureLines(const NoteFieldConfig& cfg, const ActiveMods& mods);
    void RenderMasks();
    void RenderHUD();
    void RenderPlayerTopHUD(int p);
    void RenderPlayerLifeBar(int p);
    void RenderProgressBar();
    void RenderBeatFlash();
    void RenderJudgement();
    void RenderHitFlashes(const NoteFieldConfig& cfg, const ActiveMods& mods);
    void RenderFailOverlay();
    void RenderModifierMenu();
    void RenderOptions();
    void RenderCalibration();

    // --- Rendering: Results ---
    void RenderResults();
    void RenderResultsPanel(int p, double t, int x, int y, int w, int h);
    void RenderOffsetGraph(int x, int y, int w, int h, const std::vector<HitRecord>& hits);

    // --- Transitions ---
    void StartGameplay(size_t song_index, size_t chart_index);
    void StartGameplayDirect();
    void HandleKeyDown_ModifierMenu(SDL_Keycode key);
    void HandleModifierMenuInput(SDL_Keycode key);
    void ReturnToSongSelect();
    void ShowResults();
    void JoinPlayer(int player_idx);
    void UnjoinPlayer(int player_idx);
    void ApplyInputBindings();
    void SetupPlayerFieldLayout();

    // --- Audio helpers ---
    bool TryLoadSongAudio();

    // --- Drawing helpers ---
    Color GetNoteColor(double beat) const;
    int GetQuantizationRow(double beat) const;
    void DrawNote(int lane, double y, double beat, NoteType type,
                  double x_offset = 0.0, double scale = 1.0, double extra_angle = 0.0,
                  double alpha = 1.0);
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
    static std::string GetChartModeName(const std::string& type);

    // --- Color helpers ---
    static Color GetDifficultyColor(const std::string& diff_name);
    static Color GetJudgementColor(Judgement j, bool ex_mode = false);
    static Color GetClearTypeColor(ClearType ct);

    // --- State ---
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool          fullscreen_ = true;
    bool          running_  = false;
    ScreenState   screen_   = ScreenState::ATTRACTION;

    // --- 2P Player State ---
    static constexpr int MAX_PLAYERS = 2;
    PlayerState players_[MAX_PLAYERS];
    int num_active_players_ = 0; ///< How many players have joined (0-2)
    int active_player_idx_  = 0; ///< Currently active player for single-player compat

    /// Helper: get the "current" player (P1 by default)
    PlayerState& P1() { return players_[0]; }
    PlayerState& P2() { return players_[1]; }
    const PlayerState& P1() const { return players_[0]; }
    const PlayerState& P2() const { return players_[1]; }

    // Subsystems
    SongScanner  scanner_;
    AudioEngine  audio_;
    InputMapper  input_;  ///< Legacy: still used for calibration/options. Players use players_[i].input
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

    SDL_Texture* hold_good_texture_ = nullptr;
    SDL_Texture* hold_ng_texture_   = nullptr;
    SDL_Texture* hold_bad_texture_  = nullptr;
    int hold_judge_w_ = 0, hold_judge_h_ = 0;

    SDL_Texture* bg_texture_ = nullptr;

    std::map<std::string, SDL_Texture*> jacket_cache_;
    std::map<std::string, SDL_Texture*> bga_textures_;

    // --- Current BGA State ---
    std::map<std::string, SDL_Texture*> diff_textures_;
    std::map<std::string, SDL_Texture*> playtype_textures_;

    SDL_Texture* current_bga_tex_ = nullptr;
    std::string current_bga_file_;
    std::string auto_bga_file_; // Discovered video file when #BGCHANGES is missing
    double last_bga_beat_ = -1.0;

    SDL_Texture* GetJacketTexture(const std::string& path);
    SDL_Texture* GetBGATexture(const std::string& path);

    SDL_Texture* LoadTexture(const std::string& path, int* w, int* h, bool make_white = false);

    // Song select state
    int  selected_song_  = 0;
    std::vector<int> filtered_songs_; // Maps wheel index -> scanner index
    void UpdateFilteredSongs();
    int  selected_chart_[MAX_PLAYERS] = {0, 0};
    int  scroll_offset_  = 0;
    int  visible_songs_  = 12;
    int  preferred_mode_  = 0;  ///< 0 = Single, 1 = Double (toggled by Tab in Song Select)
    
    // Legacy/Shared mod state (to be replaced by per-player)
    bool   showing_modifier_menu_ = false;
    int    modifier_menu_cursor_  = 0;
    float  sudden_plus_val_       = 0.0f;
    float  hidden_plus_val_       = 0.0f;
    int    effect_mode_           = 0; 
    std::vector<std::string> available_noteskins_;
    int    noteskin_index_        = 0;

    // Options/Calibration state
    struct BindInfo {
        enum Type { NONE, KEY, BUTTON } type = NONE;
        int id = 0; // SDL_Keycode or SDL_GameControllerButton
    };

    int    options_lane_cursor_   = 0;
    int    options_slot_cursor_   = 0;
    int    options_player_cursor_ = 0; // 0=P1, 1=P2
    int    calibration_cursor_    = 0;
    double audio_offset_          = 0.0; // Global audio offset in seconds
    bool   is_rebinding_          = false;
    BindInfo custom_binds_[2][10][3] = {}; // [Player][Action][Slot]
    
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
    uint64_t last_tick_  = 0;
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
            if (IsTap(nt))
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
            if (IsTap(cols[c]))
                mask |= (1u << c);
        }
        return mask;
    }

    /// Check if all hittable notes in a row have been hit
    bool IsRowFullyHit(size_t row_idx, int p) const {
        if (p < 0 || p >= MAX_PLAYERS) return false;
        uint32_t required = GetRowRequiredMask(row_idx);
        if (required == 0) return false;
        return (players_[p].note_hit_masks[row_idx] & required) == required;
    }

    // Combo/score 窶・LEGACY: These are kept for backward compat during refactoring.
    // New code should use players_[i].combo, players_[i].normal_score, etc.
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

    // Decide screen state
    double      decide_timer_ = 0.0;

    // Polish Effects

    double combo_pop_timer_     = 0.0;
    
    int    last_grade_milestone_ = 0; // 0=None, 1=S, 2=SS, 3=SSS
    double grade_popup_timer_    = 0.0;
    const char* grade_popup_str_ = "";

    double ready_animation_timer_ = 0.0;
    double clear_animation_timer_ = 0.0;
    double time_scale_            = 1.0; // For gradual halt on total failure

    struct ScoreRecord {
        double percentage = 0.0;
        double ex_score = 0.0;
        int stars = 0;
        std::string grade;
        int max_combo = 0;
    };
    std::map<std::string, ScoreRecord> high_scores_; // key: filepath + "|" + chart_index

    // --- Attraction / Profile Load state ---
    double attraction_timer_     = 0.0;  ///< Animation timer for title screen
    double usb_poll_timer_       = 0.0;  ///< Periodic USB check timer
    static constexpr double USB_POLL_INTERVAL = 1.0; ///< Check USB every 1 second

    /// USB drive letter mapping: P1 = drive_map_[0], P2 = drive_map_[1]
    char drive_map_[2] = { 'E', 'F' };

    /// Profile load screen state per player
    struct ProfileLoadState {
        bool   ready      = false;  ///< Player has confirmed profile
        int    cursor     = 0;      ///< Menu cursor position
        bool   usb_found  = false;  ///< USB profile detected
        std::string usb_name;       ///< Name from USB profile
        std::vector<std::string> local_profiles; ///< Scanned local profiles
    };
    ProfileLoadState profile_load_state_[MAX_PLAYERS];

    // --- Profile management state ---
    std::vector<std::string> local_profile_names_;  ///< Scanned local profile names
    int profile_manage_cursor_ = 0;   ///< Cursor in profile management list
    int profile_manage_player_ = 0;   ///< Which player is managing their profile
    bool profile_manage_creating_ = false; ///< In name entry mode
    std::string profile_manage_new_name_;  ///< Name being typed for new profile

    // Window dimensions
    int width_  = 900;
    int height_ = 700;

    // Layout constants
    static constexpr int LANE_PADDING = 2;
    static constexpr int RECEPTOR_HEIGHT = 16; // SMX-style bracket receptors need more room
    static constexpr int NOTE_HEIGHT = 14;
    static constexpr int FIELD_MARGIN = 200;

    // Per-player rendering offset (set before each player's render pass)
    int render_x_offset_ = 0;  ///< Horizontal offset applied to notefield for current player
    int render_player_idx_ = 0; ///< Which player index is currently being rendered

    bool center_1p_ = false;   ///< If true, 1P notefield is centered instead of side-aligned

    // --- Video Decoder ---
    VideoDecoder video_decoder_;
    std::string video_path_;
};

/// Generate a built-in test chart.
} // namespace sml

#endif // HAS_SDL2
