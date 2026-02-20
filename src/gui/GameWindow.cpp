// ============================================================================
// GameWindow.cpp 窶・SDL2 test GUI with Song Select + Gameplay screens
// ============================================================================

#if HAS_SDL2

#include "gui/GameWindow.h"
#include "parsing/SimfileParser.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <random>
namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace sml {

// ============================================================================
// 3x5 bitmap font (same as before)
// ============================================================================

void GameWindow::DrawDigit(int x, int y, int digit, int scale, Color color) {
    if (digit < 0 || digit > 9) return;
    static const bool bitmaps[10][5][3] = {
        {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}},
        {{0,1,0},{1,1,0},{0,1,0},{0,1,0},{1,1,1}},
        {{1,1,1},{0,0,1},{1,1,1},{1,0,0},{1,1,1}},
        {{1,1,1},{0,0,1},{1,1,1},{0,0,1},{1,1,1}},
        {{1,0,1},{1,0,1},{1,1,1},{0,0,1},{0,0,1}},
        {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}},
        {{1,1,1},{1,0,0},{1,1,1},{1,0,1},{1,1,1}},
        {{1,1,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},
        {{1,1,1},{1,0,1},{1,1,1},{1,0,1},{1,1,1}},
        {{1,1,1},{1,0,1},{1,1,1},{0,0,1},{1,1,1}},
    };
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (bitmaps[digit][row][col]) {
                SDL_Rect r = { x + col * scale, y + row * scale, scale, scale };
                SDL_RenderFillRect(renderer_, &r);
            }
        }
    }
}

void GameWindow::DrawChar(int x, int y, char ch, int scale, Color color) {
    if (ch >= '0' && ch <= '9') { DrawDigit(x, y, ch - '0', scale, color); return; }

    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

    static const bool DOT[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,1,0}};
    static const bool COLON[5][3] = {{0,0,0},{0,1,0},{0,0,0},{0,1,0},{0,0,0}};
    static const bool MINUS[5][3] = {{0,0,0},{0,0,0},{1,1,1},{0,0,0},{0,0,0}};
    static const bool SPACE[5][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
    static const bool B[5][3] = {{1,1,0},{1,0,1},{1,1,0},{1,0,1},{1,1,0}};
    static const bool P[5][3] = {{1,1,0},{1,0,1},{1,1,0},{1,0,0},{1,0,0}};
    static const bool M[5][3] = {{1,0,1},{1,1,1},{1,1,1},{1,0,1},{1,0,1}};
    static const bool X[5][3] = {{1,0,1},{0,1,0},{0,1,0},{0,1,0},{1,0,1}};
    static const bool CC[5][3] = {{1,1,1},{1,0,0},{1,0,0},{1,0,0},{1,1,1}};
    static const bool T[5][3] = {{1,1,1},{0,1,0},{0,1,0},{0,1,0},{0,1,0}};
    static const bool S[5][3] = {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}};
    static const bool O[5][3] = {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}};
    static const bool F[5][3] = {{1,1,1},{1,0,0},{1,1,0},{1,0,0},{1,0,0}};
    static const bool D[5][3] = {{1,1,0},{1,0,1},{1,0,1},{1,0,1},{1,1,0}};
    static const bool E[5][3] = {{1,1,1},{1,0,0},{1,1,0},{1,0,0},{1,1,1}};
    static const bool R[5][3] = {{1,1,0},{1,0,1},{1,1,0},{1,0,1},{1,0,1}};
    static const bool A[5][3] = {{0,1,0},{1,0,1},{1,1,1},{1,0,1},{1,0,1}};
    static const bool I[5][3] = {{1,1,1},{0,1,0},{0,1,0},{0,1,0},{1,1,1}};
    static const bool U[5][3] = {{1,0,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}};
    static const bool N[5][3] = {{1,0,1},{1,1,1},{1,1,1},{1,0,1},{1,0,1}};
    static const bool L[5][3] = {{1,0,0},{1,0,0},{1,0,0},{1,0,0},{1,1,1}};
    static const bool Y[5][3] = {{1,0,1},{1,0,1},{0,1,0},{0,1,0},{0,1,0}};
    static const bool W[5][3] = {{1,0,1},{1,0,1},{1,1,1},{1,1,1},{1,0,1}};
    static const bool V[5][3] = {{1,0,1},{1,0,1},{1,0,1},{0,1,0},{0,1,0}};
    static const bool G[5][3] = {{1,1,1},{1,0,0},{1,0,1},{1,0,1},{1,1,1}};
    static const bool H[5][3] = {{1,0,1},{1,0,1},{1,1,1},{1,0,1},{1,0,1}};
    static const bool K[5][3] = {{1,0,1},{1,1,0},{1,0,0},{1,1,0},{1,0,1}};
    static const bool J[5][3] = {{0,0,1},{0,0,1},{0,0,1},{1,0,1},{1,1,1}};
    static const bool Q[5][3] = {{1,1,1},{1,0,1},{1,0,1},{1,1,0},{0,0,1}};
    static const bool Z[5][3] = {{1,1,1},{0,0,1},{0,1,0},{1,0,0},{1,1,1}};
    static const bool SLASH[5][3] = {{0,0,1},{0,0,1},{0,1,0},{1,0,0},{1,0,0}};
    static const bool EQUALS[5][3] = {{0,0,0},{1,1,1},{0,0,0},{1,1,1},{0,0,0}};
    static const bool LBRACK[5][3] = {{1,1,0},{1,0,0},{1,0,0},{1,0,0},{1,1,0}};
    static const bool RBRACK[5][3] = {{0,1,1},{0,0,1},{0,0,1},{0,0,1},{0,1,1}};
    static const bool LPAREN[5][3] = {{0,1,0},{1,0,0},{1,0,0},{1,0,0},{0,1,0}};
    static const bool RPAREN[5][3] = {{0,1,0},{0,0,1},{0,0,1},{0,0,1},{0,1,0}};
    static const bool PIPE[5][3] = {{0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,1,0}};
    static const bool HASH[5][3] = {{1,0,1},{1,1,1},{1,0,1},{1,1,1},{1,0,1}};
    static const bool PLUS[5][3] = {{0,0,0},{0,1,0},{1,1,1},{0,1,0},{0,0,0}};

    const bool (*bitmap)[3] = nullptr;
    char upper = (ch >= 'a' && ch <= 'z') ? (ch - 32) : ch;
    switch (upper) {
        case '.': bitmap = DOT; break;  case ':': bitmap = COLON; break;
        case '-': bitmap = MINUS; break; case ' ': bitmap = SPACE; break;
        case '+': bitmap = PLUS; break;
        case 'B': bitmap = B; break;    case 'P': bitmap = P; break;
        case 'M': bitmap = M; break;    case 'X': bitmap = X; break;
        case 'C': bitmap = CC; break;   case 'T': bitmap = T; break;
        case 'S': bitmap = S; break;    case 'O': bitmap = O; break;
        case 'F': bitmap = F; break;    case 'D': bitmap = D; break;
        case 'E': bitmap = E; break;    case 'R': bitmap = R; break;
        case 'A': bitmap = A; break;    case 'I': bitmap = I; break;
        case 'U': bitmap = U; break;    case 'N': bitmap = N; break;
        case 'L': bitmap = L; break;    case 'Y': bitmap = Y; break;
        case 'W': bitmap = W; break;    case 'V': bitmap = V; break;
        case 'G': bitmap = G; break;    case 'H': bitmap = H; break;
        case 'K': bitmap = K; break;    case 'J': bitmap = J; break;
        case 'Q': bitmap = Q; break;    case 'Z': bitmap = Z; break;
        case '/': bitmap = SLASH; break; case '=': bitmap = EQUALS; break;
        case '[': bitmap = LBRACK; break; case ']': bitmap = RBRACK; break;
        case '(': bitmap = LPAREN; break; case ')': bitmap = RPAREN; break;
        case '|': bitmap = PIPE; break;  case '#': bitmap = HASH; break;
        default:  bitmap = SPACE; break;
    }
    if (bitmap) {
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (bitmap[row][col]) {
                    SDL_Rect r = { x + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRect(renderer_, &r);
                }
            }
        }
    }
}

void GameWindow::DrawText(int x, int y, const std::string& text, Color color, int scale) {
    int char_width = 3 * scale + scale; // 3px for char, 1px for spacing
    int cx = x;
    for (char ch : text) {
        DrawChar(cx, y, ch, scale, color);
        cx += char_width;
    }
}

int GameWindow::GetTextWidth(const std::string& text, int scale) const {
    if (text.empty()) return 0;
    int char_width = 3 * scale + scale;
    return static_cast<int>(text.length()) * char_width - scale;
}

// ============================================================================
// Lifecycle
// ============================================================================

GameWindow::~GameWindow() { Shutdown(); }

bool GameWindow::Init(int width, int height, const std::string& title) {
    width_ = width;
    height_ = height;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (fullscreen_) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window_ = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        flags);
    if (!window_) { std::printf("SDL_CreateWindow failed: %s\n", SDL_GetError()); return false; }

    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) { std::printf("SDL_CreateRenderer failed: %s\n", SDL_GetError()); return false; }

    // Open first available game controller
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller_ = SDL_GameControllerOpen(i);
            if (controller_) {
                std::printf("Connected Controller: %s\n", SDL_GameControllerName(controller_));
                break;
            }
        }
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    // Initialize audio engine
    if (!audio_.Init()) {
        std::printf("Warning: Audio init failed, continuing without audio\n");
    }

    field_config_.receptor_y      = height_ * 0.15; // Upscroll by default (Top)
    field_config_.pixels_per_beat  = 80.0;
    field_config_.speed_mod       = 2.0;
    field_config_.mod_type        = ScrollModType::XMod;
    field_config_.downscroll      = false;
    field_config_.screen_height   = height_;

    // Initialize player states
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        players_[i].player_num = i;
        players_[i].field_config = field_config_;
    }

    visible_songs_ = (height_ - 120) / 40;

    ScanNoteskins();
    LoadNoteskin("Default");

    // Initialize fonts
    if (!font_.Init("assets/fonts/Main.otf")) {
        std::printf("Warning: Font init failed, using legacy bitmap font\n");
    }
    font_.LoadFont("score", "assets/fonts/Score.otf");

    // Load grade sprites
    // Normal: D, C, B, BB, BBB, A, AA, AAA, S, Splus, SS, SSplus, SSS, SSSplus
    // Use std::vector for stable ordering if needed, or just map.
    const std::vector<std::string> normal_grades = {
        "D", "C", "B", "BB", "BBB", "A", "AA", "AAA", "S", "Splus", "SS", "SSplus", "SSS", "SSSplus"
    };

    for (const auto& g : normal_grades) {
        int w, h;
        std::string path = "sprites/grades/Normal/" + g + ".png";
        SDL_Texture* tex = LoadTexture(path, &w, &h);
        if (tex) {
            normal_grade_textures_[g] = tex;
        } else {
             std::printf("Failed to load grade texture: %s\n", path.c_str());
        }
    }

    // EX grades (Stars)
    // 1star.png, 2stars.png, ..., 6stars.png
    for (int i = 1; i <= 6; ++i) {
        int w, h;
        std::string filename = std::to_string(i) + ((i == 1) ? "star.png" : "stars.png");
        std::string path = "sprites/grades/EX/" + filename;
        SDL_Texture* tex = LoadTexture(path, &w, &h);
        if (tex) {
            ex_grade_textures_[i] = tex;
        } else {
            std::printf("Failed to load star texture: %s\n", path.c_str());
        }
    }

    // Hold Judgement Icons
    hold_good_texture_ = LoadTexture("sprites/holds/good.png", &hold_judge_w_, &hold_judge_h_);
    hold_ng_texture_   = LoadTexture("sprites/holds/ng.png",   &hold_judge_w_, &hold_judge_h_);
    hold_bad_texture_  = LoadTexture("sprites/holds/bad.png",  &hold_judge_w_, &hold_judge_h_);

    // Load Settings (Keybinds, etc)
    LoadSettings();
    ApplyInputBindings();

    return true;
}

int GameWindow::ScanSongs(const std::string& songs_path) {
    int count = scanner_.ScanDirectory(songs_path);
    if (count > 0) {
        // Don't force SONG_SELECT here 窶・let the Attraction screen handle transitions
        selected_song_ = 0;
        for (int p = 0; p < MAX_PLAYERS; ++p) selected_chart_[p] = 0;
    }
    return count;
}

bool GameWindow::LoadSimfile(const std::string& filepath, int chart_index) {
    SimfileParser parser;
    auto simfile = parser.LoadFromFile(filepath);
    if (!simfile) {
        std::printf("Failed: %s\n", parser.GetLastError().c_str());
        return false;
    }
    if (chart_index < 0 || chart_index >= static_cast<int>(simfile->charts.size())) {
        std::printf("Chart index %d out of range\n", chart_index);
        return false;
    }
    current_chart_ = &simfile->charts[static_cast<size_t>(chart_index)];
    conductor_.Initialize(*simfile, *current_chart_);
    active_simfile_ = simfile.get();
    loaded_simfile_ = std::move(simfile);
    StartGameplayDirect();
    std::printf("Loaded: %s - %s\n", active_simfile_->artist.c_str(), active_simfile_->title.c_str());
    return true;
}

void GameWindow::LoadTestChart() {
    SimfileParser parser;
    auto simfile = parser.LoadFromFile("testdata/timing_test.sm");
    if (!simfile) {
        std::printf("Failed to load test chart: %s\n", parser.GetLastError().c_str());
        return;
    }
    
    if (simfile->charts.empty()) {
        std::printf("Error: No charts found in test simfile.\n");
        return;
    }

    loaded_simfile_ = std::move(simfile);
    active_simfile_ = loaded_simfile_.get();
    current_chart_ = &active_simfile_->charts[0];
    conductor_.Initialize(*active_simfile_, *current_chart_);
    StartGameplayDirect();
}

void GameWindow::Shutdown() {
    audio_.Shutdown();
    font_.Shutdown();
    if (note_texture_)  { SDL_DestroyTexture(note_texture_); note_texture_ = nullptr; }
    if (note_silhouette_texture_) { SDL_DestroyTexture(note_silhouette_texture_); note_silhouette_texture_ = nullptr; }
    if (flash_texture_) { SDL_DestroyTexture(flash_texture_); flash_texture_ = nullptr; }
    if (mine_texture_)  { SDL_DestroyTexture(mine_texture_);  mine_texture_  = nullptr; }
    if (hold_body_texture_) { SDL_DestroyTexture(hold_body_texture_); hold_body_texture_ = nullptr; }
    if (hold_cap_texture_)  { SDL_DestroyTexture(hold_cap_texture_);  hold_cap_texture_  = nullptr; }
    if (roll_body_texture_) { SDL_DestroyTexture(roll_body_texture_); roll_body_texture_ = nullptr; }
    if (roll_cap_texture_)  { SDL_DestroyTexture(roll_cap_texture_);  roll_cap_texture_  = nullptr; }
    if (lift_texture_)      { SDL_DestroyTexture(lift_texture_);      lift_texture_      = nullptr; }
    if (judge_normal_texture_) { SDL_DestroyTexture(judge_normal_texture_); judge_normal_texture_ = nullptr; }
    if (judge_ex_texture_)     { SDL_DestroyTexture(judge_ex_texture_); judge_ex_texture_ = nullptr; }
    if (bg_texture_)           { SDL_DestroyTexture(bg_texture_);           bg_texture_           = nullptr; }
    if (hold_good_texture_)    { SDL_DestroyTexture(hold_good_texture_);    hold_good_texture_    = nullptr; }
    if (hold_ng_texture_)      { SDL_DestroyTexture(hold_ng_texture_);      hold_ng_texture_      = nullptr; }
    if (hold_bad_texture_)     { SDL_DestroyTexture(hold_bad_texture_);     hold_bad_texture_     = nullptr; }

    for (auto& pair : jacket_cache_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    jacket_cache_.clear();

    for (auto& pair : bga_textures_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    bga_textures_.clear();

    for (auto& pair : normal_grade_textures_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    normal_grade_textures_.clear();

    for (auto& pair : ex_grade_textures_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    ex_grade_textures_.clear();

    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_)   { SDL_DestroyWindow(window_); window_ = nullptr; }
    if (controller_) { SDL_GameControllerClose(controller_); controller_ = nullptr; }
    SDL_Quit();
}

// ============================================================================
// Main loop
// ============================================================================

void GameWindow::Run() {
    running_ = true;
    last_tick_ = SDL_GetPerformanceCounter();
    while (running_) {
        uint64_t now = SDL_GetPerformanceCounter();
        double dt = static_cast<double>(now - last_tick_) / static_cast<double>(SDL_GetPerformanceFrequency());
        last_tick_ = now;
        HandleEvents();
        Update(dt);
        Render();
    }
}

// ============================================================================
// Events
// ============================================================================

void GameWindow::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: running_ = false; break;
            case SDL_KEYDOWN:
                if (!event.key.repeat) // Ignore key repeats for gameplay
                    HandleKeyDown(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                HandleKeyUp(event.key.keysym.sym);
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                HandleControllerButtonDown(static_cast<SDL_GameControllerButton>(event.cbutton.button));
                break;
            case SDL_CONTROLLERBUTTONUP:
                HandleControllerButtonUp(static_cast<SDL_GameControllerButton>(event.cbutton.button));
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    width_  = event.window.data1;
                    height_ = event.window.data2;
                    field_config_.screen_height = height_;
                    if (field_config_.downscroll)
                        field_config_.receptor_y = height_ * 0.85;
                    else
                        field_config_.receptor_y = height_ * 0.15;
                    visible_songs_ = (height_ - 120) / 40;
                }
                break;
        }
    }
}

void GameWindow::HandleKeyDown(SDL_Keycode key) {
    // Check for COL_SELECT toggle for each player
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        int action = players_[p].input.OnKeyDown(key);
        if (action == InputMapper::COL_SELECT) {
            if (screen_ == ScreenState::SONG_SELECT || screen_ == ScreenState::GAMEPLAY) {
                players_[p].showing_modifier_menu = !players_[p].showing_modifier_menu;
                if (players_[p].showing_modifier_menu) {
                    players_[p].modifier_menu_cursor = 0;
                } else {
                    SaveSettings();
                }
                return;
            }
        }
    }

    if (key == SDLK_F12) {
        if (screen_ == ScreenState::SONG_SELECT || screen_ == ScreenState::RESULTS || screen_ == ScreenState::OPTIONS) {
            if (screen_ == ScreenState::OPTIONS) {
                SaveSettings();
                ChangeScreen(ScreenState::SONG_SELECT);
            } else {
                options_lane_cursor_ = 0;
                options_slot_cursor_ = 0;
                is_rebinding_ = false;
                ChangeScreen(ScreenState::OPTIONS);
            }
            return;
        }
    }

    if (key == SDLK_F11) {
        if (screen_ == ScreenState::SONG_SELECT || screen_ == ScreenState::RESULTS || screen_ == ScreenState::CALIBRATION || screen_ == ScreenState::GAMEPLAY) {
            if (screen_ == ScreenState::CALIBRATION || (screen_ == ScreenState::GAMEPLAY && is_calibrating_)) {
                if (screen_ == ScreenState::GAMEPLAY && is_calibrating_ && suggested_offset_ != 0.0) {
                    audio_offset_ = suggested_offset_;
                }
                SaveSettings();
                if (screen_ == ScreenState::GAMEPLAY) {
                    audio_.Stop();
                    // is_calibrating_ reset happens in transition or ReturnToSongSelect
                    ChangeScreen(ScreenState::CALIBRATION);
                } else {
                    ChangeScreen(ScreenState::SONG_SELECT);
                }
            } else {
                calibration_cursor_ = 0;
                ChangeScreen(ScreenState::CALIBRATION);
            }
            return;
        }
    }

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].showing_modifier_menu) {
            HandleKeyDown_ModifierMenu(p, key);
            return;
        }
    }

    switch (screen_) {
        case ScreenState::ATTRACTION:      HandleKeyDown_Attraction(key); break;
        case ScreenState::PROFILE_LOAD:    HandleKeyDown_ProfileLoad(key); break;
        case ScreenState::PROFILE_MANAGE:  HandleKeyDown_ProfileManage(key); break;
        case ScreenState::SONG_SELECT:     HandleKeyDown_SongSelect(key); break;
        case ScreenState::GAMEPLAY:        HandleKeyDown_Gameplay(key); break;
        case ScreenState::RESULTS:         HandleKeyDown_Results(key); break;
        case ScreenState::OPTIONS:         HandleKeyDown_Options(key); break;
        case ScreenState::CALIBRATION:     HandleKeyDown_Calibration(key); break;
        default: break;
    }
}

void GameWindow::HandleKeyUp(SDL_Keycode key) {
    if (screen_ == ScreenState::GAMEPLAY) HandleKeyUp_Gameplay(key);
}

void GameWindow::HandleKeyDown_SongSelect(SDL_Keycode key) {
    int song_count = static_cast<int>(scanner_.GetSongCount());
    if (song_count == 0 && key == SDLK_ESCAPE) { running_ = false; return; }
    if (song_count == 0) return;

    // --- Per-player input routing ---
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        int lane = players_[p].input.OnKeyDown(key);
        
        if (lane == 0 || lane == 3) { // Left or Right -> Change Song
            if (lane == 0) selected_song_ = (selected_song_ - 1 + song_count) % song_count;
            else          selected_song_ = (selected_song_ + 1) % song_count;
            
            // Reset state for new song
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                selected_chart_[i] = 0;
                players_[i].ready = false;
            }
            return;
        }
        else if (lane == 1 || lane == 2) { // Up or Down -> Double Tap for Chart
            double now = SDL_GetTicks() / 1000.0;
            auto& ps = players_[p];
            bool double_tap = false;
            
            if (lane == 1) { // Down
                if (now - ps.last_down_press_time < 0.35) double_tap = true;
                ps.last_down_press_time = now;
            } else { // Up
                if (now - ps.last_up_press_time < 0.35) double_tap = true;
                ps.last_up_press_time = now;
            }

            if (double_tap) {
                const auto& song = scanner_.GetSongs()[static_cast<size_t>(selected_song_)];
                int chart_count = static_cast<int>(song.charts.size());
                if (chart_count > 0) {
                    // Filter logic: if 2P joined, skip 8-lane charts
                    int step = (lane == 2) ? -1 : 1;
                    int next = (selected_chart_[p] + step + chart_count) % chart_count;
                    
                    if (num_active_players_ >= 2) {
                        for (int i = 0; i < chart_count; ++i) {
                            if (!song.charts[next].Is8Lane()) break;
                            next = (next + step + chart_count) % chart_count;
                        }
                    }
                    selected_chart_[p] = next;

                    ps.last_up_press_time = 0.0;
                    ps.last_down_press_time = 0.0;
                }
            }
            return;
        }
        else if (lane == InputMapper::COL_START) {
            // Toggle ready status
            players_[p].ready = !players_[p].ready;
            
            // Check if all joined players are ready
            bool all_ready = true;
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if (players_[i].joined && !players_[i].ready) {
                    all_ready = false;
                    break;
                }
            }
            
            if (all_ready) {
                // If all ready, proceed to transition
                StartGameplay(
                    static_cast<size_t>(selected_song_),
                    static_cast<size_t>(selected_chart_[0]));
            }
            return;
        }
    }

    switch (key) {
        case SDLK_ESCAPE:
            running_ = false;
            break;

        case SDLK_t:
            LoadTestChart();
            break;
    }
}

void GameWindow::HandleKeyDown_Gameplay(SDL_Keycode key) {
    // --- Per-player lane key routing ---
    // Check each joined player's InputMapper to route the key to the correct player
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        if (players_[p].input.IsLaneKey(key)) {
            int lane = players_[p].input.OnKeyDown(key);
            if (lane >= 0 && lane < 10 && playing_ && !autoplay_) { // Block lane input during autoplay
                ProcessLaneHit(lane, -1.0, p);
            } else if (lane == InputMapper::COL_START) {
                // Future: Handle Start button (e.g. ready up)
            }
            return;
        }
    }

    // Legacy single-player fallback (when no players are joined or no match)
    if (input_.IsLaneKey(key)) {
        int lane = input_.OnKeyDown(key);
        if (lane >= 0 && playing_ && !autoplay_) { // Block lane input during autoplay
            ProcessLaneHit(lane);
        }
        return;
    }

    // Non-lane keys
    switch (key) {
        case SDLK_ESCAPE:
            if (is_calibrating_) {
                if (suggested_offset_ != 0.0) audio_offset_ = suggested_offset_;
                SaveSettings();
                audio_.Stop();
                is_calibrating_ = false; // Reset here
                ChangeScreen(ScreenState::CALIBRATION);
            } else {
                ChangeScreen(ScreenState::SONG_SELECT);
            }
            break;
        case SDLK_SPACE:
            if (playing_) {
                playing_ = false;
                audio_.Pause();
            } else {
                playing_ = true;
                if (audio_loaded_) {
                    if (audio_.IsPaused())
                        audio_.Resume();
                    else
                        audio_.Play();
                }
            }
            break;
        case SDLK_r:
            // Full reset: re-initialize conductor and all gameplay state
            if (current_chart_ && active_simfile_) {
                conductor_.Initialize(*active_simfile_, *current_chart_);
                StartGameplayDirect();
            }
            break;
        case SDLK_F2:
            if (field_config_.mod_type == ScrollModType::CMod)
                field_config_.speed_mod += 50.0;
            else
                field_config_.speed_mod += 0.25;
            players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
            SaveSettings();
            break;
        case SDLK_F3:
            if (field_config_.mod_type == ScrollModType::CMod)
                field_config_.speed_mod = std::max(50.0, field_config_.speed_mod - 50.0);
            else
                field_config_.speed_mod = std::max(0.25, field_config_.speed_mod - 0.25);
            players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
            SaveSettings();
            break;
        case SDLK_F4:
            field_config_.downscroll = !field_config_.downscroll;
            field_config_.receptor_y = field_config_.downscroll ? height_ * 0.85 : height_ * 0.15;
            players_[active_player_idx_].field_config.downscroll = field_config_.downscroll;
            players_[active_player_idx_].field_config.receptor_y = field_config_.receptor_y;
            SaveSettings();
            break;
        case SDLK_TAB:
            if (field_config_.mod_type == ScrollModType::XMod) {
                field_config_.mod_type = ScrollModType::CMod;
                field_config_.speed_mod = 400.0;
            } else {
                field_config_.mod_type = ScrollModType::XMod;
                field_config_.speed_mod = 2.0;
            }
            players_[active_player_idx_].field_config.mod_type = field_config_.mod_type;
            players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
            SaveSettings();
            break;
        case SDLK_F1:
            show_debug_ = !show_debug_;
            break;
        case SDLK_F5:
            ex_mode_ = !ex_mode_;
            break;
        case SDLK_F7:
            center_1p_ = !center_1p_;
            SetupPlayerFieldLayout();
            SaveSettings();
            break;
        case SDLK_F8:
            autoplay_ = !autoplay_;
            break;
        case SDLK_F6:
            // Cycle Life Modes: Standard -> Life4 -> Risky -> Flare 1-5 
            if (life_meter_.GetType() == LifeType::STANDARD) {
                life_meter_.Init(LifeType::LIFE4);
            } else if (life_meter_.GetType() == LifeType::LIFE4) {
                life_meter_.Init(LifeType::RISKY);
            } else if (life_meter_.GetType() == LifeType::RISKY) {
                life_meter_.Init(LifeType::FLARE, 1);
            } else if (life_meter_.GetType() == LifeType::FLARE) {
                int level = life_meter_.GetFlareLevel();
                if (level < 5) {
                    life_meter_.Init(LifeType::FLARE, level + 1);
                } else {
                    life_meter_.Init(LifeType::STANDARD);
                }
            }
            break;
    }
}

void GameWindow::HandleKeyUp_Gameplay(SDL_Keycode key) {
    // Release key on all joined players' input mappers
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) {
            int lane = players_[p].input.OnKeyUp(key);
            if (lane >= 0 && lane < 10 && playing_ && !autoplay_) {
                ProcessLaneHit(lane, -1.0, p, true); // true = is_release
            }
        }
    }

    // Legacy fallback
    int leg_lane = input_.OnKeyUp(key);
    if (leg_lane >= 0 && playing_ && !autoplay_) {
        ProcessLaneHit(leg_lane, -1.0, active_player_idx_, true);
    }
}

void GameWindow::HandleKeyDown_Results(SDL_Keycode key) {
    // Non-lane keys
    switch (key) {
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_ESCAPE:
            ChangeScreen(ScreenState::SONG_SELECT);
            break;
        case SDLK_r:
            // Retry: replay the same chart
            if (current_chart_ && active_simfile_) {
                conductor_.Initialize(*active_simfile_, *current_chart_);
                StartGameplayDirect();
                ChangeScreen(ScreenState::GAMEPLAY);
            }
            break;
        case SDLK_F5:
            results_ex_mode_ = !results_ex_mode_;
            break;
    }
}
void GameWindow::HandleKeyDown_ModifierMenu(int p, SDL_Keycode key) {
    HandleModifierMenuInput(p, key);
}

void GameWindow::HandleModifierMenuInput(int p, SDL_Keycode key) {
    auto& ps = players_[p];
    if (!ps.showing_modifier_menu) return;
    int num_items = 12; // Speed, Type, Scroll, Sudden, Hidden, Skin, Effect, Life, Score, Center, Combo, BGA
    
    switch (key) {
        case SDLK_UP:
            ps.modifier_menu_cursor = (ps.modifier_menu_cursor - 1 + num_items) % num_items;
            break;
        case SDLK_DOWN:
            ps.modifier_menu_cursor = (ps.modifier_menu_cursor + 1) % num_items;
            break;
        case SDLK_LEFT: {
            switch (ps.modifier_menu_cursor) {
                case 0: { // Speed
                    if (ps.field_config.mod_type == ScrollModType::CMod)
                        ps.field_config.speed_mod = std::max(50.0, ps.field_config.speed_mod - 50.0);
                    else
                        ps.field_config.speed_mod = std::max(0.25, ps.field_config.speed_mod - 0.25);
                } break;
                case 1: // Mod Type
                    ps.field_config.mod_type = (ps.field_config.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod;
                    ps.field_config.speed_mod = (ps.field_config.mod_type == ScrollModType::XMod) ? 2.0 : 400.0;
                    break;
                case 2: // Scroll
                    ps.field_config.downscroll = !ps.field_config.downscroll;
                    ps.field_config.receptor_y = ps.field_config.downscroll ? height_ * 0.85 : height_ * 0.15;
                    break;
                case 3: // Sudden+
                    ps.sudden_plus_val = std::max(0.0f, ps.sudden_plus_val - 0.05f);
                    break;
                case 4: // Hidden+
                    ps.hidden_plus_val = std::max(0.0f, ps.hidden_plus_val - 0.05f);
                    break;
                case 5: // Noteskin
                    if (!available_noteskins_.empty()) {
                        ps.noteskin_index = (ps.noteskin_index - 1 + (int)available_noteskins_.size()) % (int)available_noteskins_.size();
                        if (p == active_player_idx_) LoadNoteskin(available_noteskins_[ps.noteskin_index]);
                    }
                    break;
                case 6: // Effects
                    ps.effect_mode = (ps.effect_mode - 1 + 3) % 3;
                    break;
                case 7: { // Life Mode
                    int flare = ps.life_meter.GetFlareLevel();
                    if (ps.life_meter.GetType() == LifeType::STANDARD) ps.life_meter.Init(LifeType::FLARE, 5);
                    else if (ps.life_meter.GetType() == LifeType::LIFE4) ps.life_meter.Init(LifeType::STANDARD);
                    else if (ps.life_meter.GetType() == LifeType::RISKY) ps.life_meter.Init(LifeType::LIFE4);
                    else if (ps.life_meter.GetType() == LifeType::FLARE) {
                        if (flare > 1) ps.life_meter.Init(LifeType::FLARE, flare - 1);
                        else ps.life_meter.Init(LifeType::RISKY);
                    }
                } break;
                case 8: // Scoring
                    ps.ex_mode = !ps.ex_mode;
                    break;
                case 9: // Center 1P
                    if (p == 0) {
                        center_1p_ = !center_1p_;
                        SetupPlayerFieldLayout();
                    }
                    break;
                case 10: // Combo Display
                    ps.combo_display_mode = static_cast<ComboDisplayMode>((static_cast<int>(ps.combo_display_mode) - 1 + 19) % 19);
                    break;
                case 11: // BGA Brightness
                    ps.bga_brightness = static_cast<BGABrightness>((static_cast<int>(ps.bga_brightness) - 1 + 5) % 5);
                    break;
                default: break;
            }
            break;
        }
        case SDLK_RIGHT:
        case SDLK_RETURN:
        case SDLK_KP_ENTER: {
            switch (ps.modifier_menu_cursor) {
                case 0: { // Speed
                    if (ps.field_config.mod_type == ScrollModType::CMod)
                        ps.field_config.speed_mod += 50.0;
                    else
                        ps.field_config.speed_mod += 0.25;
                } break;
                case 1: // Mod Type
                    ps.field_config.mod_type = (ps.field_config.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod;
                    ps.field_config.speed_mod = (ps.field_config.mod_type == ScrollModType::XMod) ? 2.0 : 400.0;
                    break;
                case 2: // Scroll
                    ps.field_config.downscroll = !ps.field_config.downscroll;
                    ps.field_config.receptor_y = ps.field_config.downscroll ? height_ * 0.85 : height_ * 0.15;
                    break;
                case 3: // Sudden+
                    ps.sudden_plus_val = std::min(0.8f, ps.sudden_plus_val + 0.05f);
                    break;
                case 4: // Hidden+
                    ps.hidden_plus_val = std::min(0.8f, ps.hidden_plus_val + 0.05f);
                    break;
                case 5: // Noteskin
                    if (!available_noteskins_.empty()) {
                        ps.noteskin_index = (ps.noteskin_index + 1) % (int)available_noteskins_.size();
                        if (p == active_player_idx_) LoadNoteskin(available_noteskins_[ps.noteskin_index]);
                    }
                    break;
                case 6: // Effects
                    ps.effect_mode = (ps.effect_mode + 1) % 3;
                    break;
                case 7: { // Life Mode
                    int flare = ps.life_meter.GetFlareLevel();
                    if (ps.life_meter.GetType() == LifeType::STANDARD) ps.life_meter.Init(LifeType::LIFE4);
                    else if (ps.life_meter.GetType() == LifeType::LIFE4) ps.life_meter.Init(LifeType::RISKY);
                    else if (ps.life_meter.GetType() == LifeType::RISKY) ps.life_meter.Init(LifeType::FLARE, 1);
                    else if (ps.life_meter.GetType() == LifeType::FLARE) {
                        if (flare < 5) ps.life_meter.Init(LifeType::FLARE, flare + 1);
                        else ps.life_meter.Init(LifeType::STANDARD);
                    }
                } break;
                case 8: // Scoring
                    ps.ex_mode = !ps.ex_mode;
                    break;
                case 9: // Center 1P
                    if (p == 0) {
                        center_1p_ = !center_1p_;
                        SetupPlayerFieldLayout();
                    }
                    break;
                case 10: // Combo Display
                    ps.combo_display_mode = static_cast<ComboDisplayMode>((static_cast<int>(ps.combo_display_mode) + 1) % 19);
                    break;
                case 11: // BGA Brightness
                    ps.bga_brightness = static_cast<BGABrightness>((static_cast<int>(ps.bga_brightness) + 1) % 5);
                    break;
                default: break;
            }
            break;
        }
        case SDLK_ESCAPE:
            ps.showing_modifier_menu = false;
            SaveActiveProfile(p);
            SaveSettings();
            break;
    }
    SaveSettings(); // Save settings after any change in the menu
    SaveActiveProfile(p);
}

void GameWindow::HandleKeyDown_Options(SDL_Keycode key) {
    if (is_rebinding_) {
        if (key == SDLK_ESCAPE) {
            is_rebinding_ = false;
            return;
        }
        // Bind key to the selected slot
        custom_binds_[options_player_cursor_][options_lane_cursor_][options_slot_cursor_] = { BindInfo::KEY, (int)key };
        is_rebinding_ = false;
        return;
    }

    int num_actions = 6; // L, D, U, R, Start, Select
    int num_slots = 3;
    int num_players = 2;

    switch (key) {
        case SDLK_UP:
            options_lane_cursor_ = (options_lane_cursor_ - 1 + num_actions) % num_actions;
            break;
        case SDLK_DOWN:
            options_lane_cursor_ = (options_lane_cursor_ + 1) % num_actions;
            break;
        case SDLK_LEFT:
            if (options_slot_cursor_ == 0) {
                if (options_player_cursor_ == 1) {
                    options_player_cursor_ = 0;
                    options_slot_cursor_ = num_slots - 1;
                } else {
                    // Wrap to the end of P2
                    options_player_cursor_ = 1;
                    options_slot_cursor_ = num_slots - 1;
                }
            } else {
                options_slot_cursor_--;
            }
            break;
        case SDLK_RIGHT:
            if (options_slot_cursor_ == num_slots - 1) {
                if (options_player_cursor_ == 0) {
                    options_player_cursor_ = 1;
                    options_slot_cursor_ = 0;
                } else {
                    // Wrap to the beginning of P1
                    options_player_cursor_ = 0;
                    options_slot_cursor_ = 0;
                }
            } else {
                options_slot_cursor_++;
            }
            break;
        case SDLK_TAB:
            options_player_cursor_ = (options_player_cursor_ + 1) % num_players;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            is_rebinding_ = true;
            break;
        case SDLK_BACKSPACE:
        case SDLK_DELETE:
            custom_binds_[options_player_cursor_][options_lane_cursor_][options_slot_cursor_] = { BindInfo::NONE, 0 };
            break;
        case SDLK_ESCAPE:
        case SDLK_F12:
            SaveSettings();
            ChangeScreen(ScreenState::SONG_SELECT);
            break;
    }
}

void GameWindow::HandleKeyDown_Calibration(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
            calibration_cursor_ = (calibration_cursor_ - 1 + 2) % 2;
            break;
        case SDLK_DOWN:
            calibration_cursor_ = (calibration_cursor_ + 1) % 2;
            break;
        case SDLK_LEFT:
            if (calibration_cursor_ == 0) audio_offset_ -= 0.001; // -1ms
            break;
        case SDLK_RIGHT:
            if (calibration_cursor_ == 0) audio_offset_ += 0.001; // +1ms
            break;
        case SDLK_RETURN:
            if (calibration_cursor_ == 0) {
                if (suggested_offset_ != 0.0) {
                    audio_offset_ = suggested_offset_;
                    suggested_offset_ = 0.0; // Apply once
                    
                    // Re-init conductor if we are currently "in a chart" (even if just calibration)
                    if (current_chart_ && active_simfile_) {
                        double total_offset = active_simfile_->GetEffectiveOffset(*current_chart_) + audio_offset_;
                        conductor_.Initialize(
                            active_simfile_->GetEffectiveBPMs(*current_chart_),
                            active_simfile_->GetEffectiveStops(*current_chart_),
                            active_simfile_->GetEffectiveScrolls(*current_chart_),
                            active_simfile_->GetEffectiveSpeeds(*current_chart_),
                            total_offset
                        );
                    }
                } else {
                    audio_offset_ = 0.0;
                }
            } else if (calibration_cursor_ == 1) {
                // Launch calibration chart
                std::string cal_path = "sync/sync.sm";
                if (std::filesystem::exists(cal_path)) {
                    SimfileParser parser;
                    loaded_simfile_ = parser.LoadFromFile(cal_path);
                    if (loaded_simfile_ && !loaded_simfile_->charts.empty()) {
                        active_simfile_ = loaded_simfile_.get();
                        current_chart_ = &active_simfile_->charts[0];
                        conductor_.Initialize(*active_simfile_, *current_chart_);
                        
                        // Start calibration mode
                        is_calibrating_ = true;
                        cal_errors_.clear();
                        suggested_offset_ = 0.0;
                        calibration_stdev_ = 0.0;

                        cal_errors_.clear();
                        suggested_offset_ = 0.0;
                        calibration_stdev_ = 0.0;
                        
                        StartGameplayDirect();
                        audio_.Play(0.0);
                        playing_ = true;
                        ChangeScreen(ScreenState::GAMEPLAY);
                    }
                }
            }
            break;
        case SDLK_ESCAPE:
        case SDLK_F11:
            SaveSettings();
            ChangeScreen(ScreenState::SONG_SELECT);
            break;
    }
    // Clamp offset to reasonable range [-0.5, 0.5]
    double old_off = audio_offset_;
    audio_offset_ = std::clamp(audio_offset_, -0.5, 0.5);
    
    // If offset changed via Left/Right arrows in calibration screen, hot-reload conductor if playing
    if (old_off != audio_offset_ && current_chart_ && active_simfile_) {
         double total_offset = active_simfile_->GetEffectiveOffset(*current_chart_) + audio_offset_;
         conductor_.Initialize(
             active_simfile_->GetEffectiveBPMs(*current_chart_),
             active_simfile_->GetEffectiveStops(*current_chart_),
             active_simfile_->GetEffectiveScrolls(*current_chart_),
             active_simfile_->GetEffectiveSpeeds(*current_chart_),
             total_offset
         );
    }
}

void GameWindow::HandleControllerButtonDown(SDL_GameControllerButton button) {
    if (is_rebinding_) {
        // Bind button to the selected slot
        custom_binds_[options_player_cursor_][options_lane_cursor_][options_slot_cursor_] = { BindInfo::BUTTON, (int)button };
        is_rebinding_ = false;
        return;
    }
    
    int lane = input_.OnButtonDown(button);
    if (lane >= 0 && playing_ && screen_ == ScreenState::GAMEPLAY && !autoplay_) { // Block lane input during autoplay
        ProcessLaneHit(lane);
    }
}

void GameWindow::HandleControllerButtonUp(SDL_GameControllerButton button) {
    input_.OnButtonUp(button);
}

// ============================================================================
// Transitions
// ============================================================================



void GameWindow::StartGameplay(size_t song_index, size_t chart_index) {
    if (!scanner_.EnsureLoaded(song_index)) {
        std::printf("Failed to load song: %s\n", scanner_.GetLastError().c_str());
        return;
    }
    active_simfile_ = scanner_.GetSongs()[song_index].simfile.get();
    if (!active_simfile_ || active_simfile_->charts.empty()) return;
    
    selected_song_ = static_cast<int>(song_index);
    
    // Primary chart for conductor timing (use P1 or first joined player)
    int master_p = -1;
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) { 
            master_p = p; 
            break; 
        }
    }
    if (master_p == -1) master_p = 0;

    const NoteChart* master_chart = &active_simfile_->charts[selected_chart_[master_p] < (int)active_simfile_->charts.size() ? selected_chart_[master_p] : 0];

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        
        int c_idx = selected_chart_[p];
        if (c_idx >= (int)active_simfile_->charts.size()) c_idx = 0;
        
        auto& ps = players_[p];
        ps.ResetGameplay();
        ps.current_chart = &active_simfile_->charts[c_idx];
        
        // --- Routine/Pseudo-Routine Handling ---
        bool is_routine = (ps.current_chart->chart_type == "dance-routine" || ps.current_chart->chart_type == "dance-couple");
        std::string desc_lower = ps.current_chart->description;
        std::string cred_lower = ps.current_chart->credit;
        std::transform(desc_lower.begin(), desc_lower.end(), desc_lower.begin(), ::tolower);
        std::transform(cred_lower.begin(), cred_lower.end(), cred_lower.begin(), ::tolower);

        bool has_routine_keyword = (desc_lower.find("routine") != std::string::npos || 
                                    desc_lower.find("couple") != std::string::npos ||
                                    desc_lower.find("gloves") != std::string::npos ||
                                    cred_lower.find("routine") != std::string::npos);

        bool uses_pseudo_types = false;
        if (ps.current_chart->chart_type == "dance-double") {
            int count_2_4 = 0, count_1 = 0;
            for (const auto& row : ps.current_chart->note_rows) {
                for (auto nt : row.columns) {
                    if (nt == NoteType::Tap) count_1++;
                    else if (nt == NoteType::HoldHead || nt == NoteType::RollHead) count_2_4++;
                }
            }
            if (count_2_4 > 0 && count_1 == 0) uses_pseudo_types = true;
        }

        ps.runtime_chart = *ps.current_chart;
        if (is_routine || (ps.current_chart->chart_type == "dance-double" && (has_routine_keyword || uses_pseudo_types))) {
            if (ps.runtime_chart.chart_type == "dance-double") ps.runtime_chart.chart_type = "dance-routine";
            for (auto& row : ps.runtime_chart.note_rows) {
                for (size_t c = 0; c < row.columns.size(); ++c) {
                    if (row.columns[c] == NoteType::Tap) row.columns[c] = (c < 4) ? NoteType::TapP1 : NoteType::TapP2;
                }
            }
        }
        ps.current_chart = &ps.runtime_chart;

        // Initialize per-player chart state
        size_t num_rows = ps.current_chart->note_rows.size();
        ps.note_hit_masks.assign(num_rows, 0);
        ps.row_best_error.assign(num_rows, 999.0f);
        
        ps.total_hittable_notes = 0;
        ps.combo_display_mode = static_cast<ComboDisplayMode>(ps.profile.combo_display_mode);
        for (const auto& row : ps.current_chart->note_rows) {
            for (auto nt : row.columns) if (IsTap(nt)) ps.total_hittable_notes++;
        }
    }

    // Initialize conductor with master chart timing
    double total_offset = active_simfile_->GetEffectiveOffset(*master_chart) + audio_offset_;
    conductor_.Initialize(
        active_simfile_->GetEffectiveBPMs(*master_chart),
        active_simfile_->GetEffectiveStops(*master_chart),
        active_simfile_->GetEffectiveScrolls(*master_chart),
        active_simfile_->GetEffectiveSpeeds(*master_chart),
        total_offset
    );

    // Legacy sync
    current_chart_ = master_chart; 
    
    loaded_simfile_.reset();
    auto_bga_file_ = ""; // Reset auto-discovered BGA before DECIDE screen
    decide_timer_ = 7.0; 
    ChangeScreen(ScreenState::DECIDE);

    std::printf("Playing Song Select charts...\n");
    
    // Transition to DECIDE first with 7s timer
    decide_timer_ = 7.0; 
    ChangeScreen(ScreenState::DECIDE);

    std::printf("Playing: %s - %s [%s %s %s]\n",
        active_simfile_->artist.c_str(),
        active_simfile_->title.c_str(),
        current_chart_->chart_type.c_str(),
        current_chart_->difficulty_name.c_str(),
        FormatMeter(current_chart_->custom_difficulty).c_str());
}

void GameWindow::StartGameplayDirect() {
    // 1. Reset Internal State
    next_hittable_note_ = 0;
    total_hits_ = 0;
    total_miss_ = 0;
    combo_ = 0;
    max_combo_ = 0;
    normal_score_ = 0.0;
    ex_score_ = 0.0;
    last_judgement_ = Judgement::NONE;
    judgement_timer_ = 0.0;
    last_grade_milestone_ = 0;
    grade_popup_timer_ = 0.0;
    chart_finished_ = false;
    results_delay_ = 0.0;
    failed_sequence_ = false;
    fail_animation_timer_ = 0.0;
    clear_type_ = ClearType::ALL_PERFECT_EXTRAORDINARY;
    clear_animation_timer_ = 0.0;
    time_scale_ = 1.0;
    hit_flashes_.clear();
    hit_history_.clear();
    results_ex_mode_ = false;
    // autoplay_ persists across chart starts — toggled by F8
    std::memset(normal_judge_counts_, 0, sizeof(normal_judge_counts_));
    std::memset(ex_judge_counts_, 0, sizeof(ex_judge_counts_));

    // 2. Load Assets
    audio_loaded_ = TryLoadSongAudio();
    
    if (bg_texture_) {
        SDL_DestroyTexture(bg_texture_);
        bg_texture_ = nullptr;
    }
    for (auto& pair : bga_textures_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    bga_textures_.clear();
    current_bga_tex_ = nullptr;
    current_bga_file_ = "";
    last_bga_beat_ = -1.0;
    video_path_ = "";
    if (active_simfile_ && !active_simfile_->background_path.empty()) {
        fs::path sim_dir(active_simfile_->directory);
        fs::path bg_rel(active_simfile_->background_path);
        fs::path full_bg = sim_dir / bg_rel;
        if (fs::exists(full_bg)) {
            int bw, bh;
            bg_texture_ = LoadTexture(full_bg.string(), &bw, &bh);
        }
    }

    // 3. Find first/last hittable notes and count totals
    total_hittable_notes_ = 0;
    double first_note_beat = -1.0;
    double last_note_beat = 0.0;

    if (current_chart_) {
        // --- BGA Auto-Discovery ---
        auto_bga_file_ = "";
        if (active_simfile_ && active_simfile_->bg_changes.empty()) {
            fs::path sim_dir(active_simfile_->directory);
            if (fs::exists(sim_dir)) {
                for (const auto& entry : fs::directory_iterator(sim_dir)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                        if (ext == ".avi" || ext == ".mpg" || ext == ".mpeg" || ext == ".mp4" || ext == ".flv" || ext == ".mkv" || ext == ".webm") {
                            auto_bga_file_ = entry.path().filename().string();
                            std::printf("Auto-discovered BGA: %s\n", auto_bga_file_.c_str());
                            break;
                        }
                    }
                }
            }
        }

        // Find timings
        for (const auto& row : current_chart_->note_rows) {
            bool has_hittable = false;
            bool has_visible = false;
            for (auto nt : row.columns) {
                if (IsTap(nt)) {
                    total_hittable_notes_++;
                    has_hittable = true;
                }
                if (IsVisibleNote(nt)) {
                    has_visible = true;
                }
            }
            if (has_visible) {
                if (first_note_beat < 0.0) first_note_beat = row.beat;
                if (row.beat > last_note_beat) last_note_beat = row.beat;
            }
        }

        double total_offset = active_simfile_->GetEffectiveOffset(*current_chart_) + audio_offset_;
        conductor_.Initialize(
            active_simfile_->GetEffectiveBPMs(*current_chart_),
            active_simfile_->GetEffectiveStops(*current_chart_),
            active_simfile_->GetEffectiveScrolls(*current_chart_),
            active_simfile_->GetEffectiveSpeeds(*current_chart_),
            total_offset
        );
        conductor_.PopulateChartTiming(const_cast<NoteChart&>(*current_chart_));

        double first_note_time = (first_note_beat >= 0.0) ? conductor_.BeatToTime(first_note_beat) : 0.0;
        chart_end_time_ = conductor_.BeatToTime(last_note_beat) + 2.0;
        song_duration_ = chart_end_time_;

        // 4. lead-in Logic
        // We always want a 2.0s "Get Ready" period before any note movement.
        ready_animation_timer_ = 2.0;
        play_time_ = std::min(0.0, first_note_time - 2.0);
        
        if (play_time_ >= 0.0) {
            playing_ = true;
            if (audio_loaded_) {
                audio_.Play(play_time_);
            }
        } else {
            playing_ = false;
        }


        // 5. Initialize hit masks and per-row error tracking
        note_hit_masks_.assign(current_chart_->note_rows.size(), 0);
        row_best_error_.assign(current_chart_->note_rows.size(), 999.0);

        // 6. Initialize Life Meter
        life_meter_.Init(life_meter_.GetType(), life_meter_.GetFlareLevel());

        // 7. Apply Effects
        if (effect_mode_ != 0) {
            runtime_chart_ = *current_chart_;
            current_chart_ = &runtime_chart_;

            if (effect_mode_ == 2) { // Random
                int num_cols = current_chart_->num_columns;
                std::vector<int> lane_map(num_cols);
                for(int i=0; i<num_cols; ++i) lane_map[i] = i;
                
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(lane_map.begin(), lane_map.end(), g);

                for (auto& row : runtime_chart_.note_rows) {
                    std::vector<NoteType> new_cols(num_cols, NoteType::None);
                    for (int i = 0; i < num_cols; ++i) {
                        new_cols[lane_map[i]] = row.columns[i];
                    }
                    row.columns = std::move(new_cols);
                }
                std::printf("Random effect applied.\n");
            }
        }

        // 8. Configure input mapper for the chart
        ApplyInputBindings();

        // 9. Configure per-player Gameplay State for 2P mode
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (!players_[p].joined) continue;
            auto& ps = players_[p];
            ps.ResetGameplay();
            
            ps.current_chart = current_chart_;
            ps.note_hit_masks.assign(current_chart_->note_rows.size(), 0);
            ps.row_best_error.assign(current_chart_->note_rows.size(), 999.0);
            ps.total_hittable_notes = total_hittable_notes_;
            ps.chart_end_time = chart_end_time_;
            ps.life_meter.Init(ps.life_meter.GetType(), ps.life_meter.GetFlareLevel());
        }
    }

    last_timing_error_ = 0.0;
}

bool GameWindow::TryLoadSongAudio() {
    if (!active_simfile_ || active_simfile_->music_path.empty()) return false;

    namespace fs = std::filesystem;
    fs::path music_file(active_simfile_->music_path);
    fs::path sim_dir(active_simfile_->directory);

    // 1. Try relative to simfile's own directory (handles manual loads like sync/sync.sm)
    if (!sim_dir.empty()) {
        fs::path full = sim_dir / music_file;
        if (fs::exists(full)) {
            return audio_.LoadMusic(full.string());
        }
    }

    // 2. Fallback: Search scanner results
    for (size_t i = 0; i < scanner_.GetSongCount(); ++i) {
        const auto& song = scanner_.GetSongs()[i];
        if (song.simfile.get() == active_simfile_) {
            fs::path full = fs::path(song.directory) / music_file;
            if (fs::exists(full)) {
                return audio_.LoadMusic(full.string());
            }
        }
    }

    // 3. Last resort: Try path as-is
    if (fs::exists(music_file)) {
        return audio_.LoadMusic(music_file.string());
    }

    std::printf("Audio file not found: %s\n", music_file.string().c_str());
    return false;
}

void GameWindow::ReturnToSongSelect() {
    if (scanner_.GetSongCount() == 0) {
        running_ = false;
        return;
    }
    audio_.Stop();
    audio_.UnloadMusic();
    audio_loaded_ = false;
    screen_ = ScreenState::SONG_SELECT;
    playing_ = false;
    is_calibrating_ = false;
    current_chart_ = nullptr;
    active_simfile_ = nullptr;
    auto_bga_file_ = ""; // Reset auto-discovered BGA
    loaded_simfile_.reset();
    input_.Reset();

    // Redundant RecalculateRating/SaveActiveProfile removed here. 
    // They should only happen when a score is actually earned or during profile unjoin/save explicitly.
}

void GameWindow::OnEnterSongSelect() {
    if (scanner_.GetSongs().empty()) return;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        players_[i].ready = false;
    }
}

void GameWindow::ShowResults() {
    audio_.Stop();
    playing_ = false;
    screen_ = ScreenState::RESULTS;

    // Cache results info for display
    if (active_simfile_) {
        results_title_ = active_simfile_->title;
    }
    if (current_chart_) {
        std::string mode_name = GetChartModeName(current_chart_->chart_type);
        std::string diff_name = current_chart_->difficulty_name;
        if (current_chart_->variant == ChartVariant::Wild) diff_name = "WILD";
        
        std::string meter_str = FormatMeter(current_chart_->custom_difficulty);
        results_chart_info_ = mode_name + " " + diff_name + " " + meter_str;

        // --- NEW: Calculate and Save High Score ---
        double normal_acc = (total_hittable_notes_ > 0) ? (normal_score_ / total_hittable_notes_) : 0.0;
        double ex_acc     = (total_hittable_notes_ > 0) ? (ex_score_ / total_hittable_notes_) : 0.0;
        
        // Use logic similar to RenderResults to determine grade
        std::string grade = "D";
        if (clear_type_ == ClearType::FAIL) grade = "E";
        else {
            if (normal_acc >= 100.9)       grade = "SSS+";
            else if (normal_acc >= 100.75) grade = "SSS";
            else if (normal_acc >= 100.5)  grade = "SS+";
            else if (normal_acc >= 100.0)  grade = "SS";
            else if (normal_acc >= 99.0)   grade = "S+";
            else if (normal_acc >= 97.5)   grade = "S";
            else if (normal_acc >= 95.0)   grade = "AAA";
            else if (normal_acc >= 92.5)   grade = "AA";
            else if (normal_acc >= 90.0)   grade = "A";
            else if (normal_acc >= 80.0)   grade = "BBB";
            else if (normal_acc >= 70.0)   grade = "BB";
            else if (normal_acc >= 60.0)   grade = "B";
            else if (normal_acc >= 50.0)   grade = "C";
        }

        int stars = 0;
        if (ex_acc >= 100.0)      stars = 6;
        else if (ex_acc >= 95.0)  stars = 5;
        else if (ex_acc >= 88.5)  stars = 4;
        else if (ex_acc >= 82.0)  stars = 3;
        else if (ex_acc >= 73.0)  stars = 2;
        else if (ex_acc >= 60.0)  stars = 1;

        std::string score_key = scanner_.GetSongs()[static_cast<size_t>(selected_song_)].filepath + "|" + std::to_string(selected_chart_[0]);
        auto it = high_scores_.find(score_key);
        if (it == high_scores_.end() || normal_acc > it->second.percentage) {
            high_scores_[score_key] = { normal_acc, ex_acc, stars, grade, max_combo_ };
        }

        // --- Save to active player's profile ---
        // --- Save to active player's profile ---
        bool is_coop = (current_chart_->chart_type == "dance-routine" || 
                        current_chart_->chart_type == "dance-couple");
        
        // No rating for coop charts
        double chart_rating = is_coop ? 0.0 : Profile::CalculateChartRating(
            current_chart_->custom_difficulty, normal_acc);

        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (!players_[p].joined) continue;
            auto& prof = players_[p].profile;
            auto pit = prof.high_scores.find(score_key);
            
            // Update if better score or new score
            if (pit == prof.high_scores.end() || normal_acc > pit->second.percentage) {
                // Normalize "dance-single" -> "single", etc.
                std::string normalized_mode = current_chart_->chart_type;
                if (normalized_mode == "dance-single") normalized_mode = "single";
                else if (normalized_mode == "dance-double") normalized_mode = "double";
                else if (normalized_mode == "dance-couple") normalized_mode = "couple";
                else if (normalized_mode == "dance-routine") normalized_mode = "routine";

                prof.high_scores[score_key] = {
                    normal_acc, ex_acc, stars, grade, max_combo_,
                    chart_rating,
                    normalized_mode
                };
            }
            prof.RecalculateRating();
            SaveActiveProfile(p);
        }
    }
}

void GameWindow::RenderResults() {
    // 1. Background
    RenderBackground(); 

    // 2. Dim overlay for results
    DrawRect(0, 0, width_, height_, {10, 10, 20, 180});

    // --- Header ---
    if (results_reveal_timer_ > 0.0) {
        double header_t = results_reveal_timer_;
        double header_alpha = std::min(1.0, header_t / 0.5);
        uint8_t a = static_cast<uint8_t>(255 * header_alpha);
        
        // Slide in from left
        int x_off = static_cast<int>(-50 * (1.0 - header_alpha));

        font_.DrawText(renderer_, 40 + x_off, 25, "RESULTS", {255, 255, 255, a}, FontSize::TITLE, TextAlign::LEFT);
        
        int y_title = 25;
        font_.DrawText(renderer_, width_ - 40 - x_off, y_title, results_title_, {200, 200, 220, a}, FontSize::MEDIUM, TextAlign::RIGHT, 1.0, "score");
        font_.DrawText(renderer_, width_ - 40 - x_off, y_title + 30, results_chart_info_, {150, 150, 170, a}, FontSize::SMALL, TextAlign::RIGHT);
    }

    // --- Panel Layout ---
    int py = 110;
    int ph = height_ - 170;

    if (num_active_players_ >= 2) {
        // Player 1 Column (Left Half, expanded)
        RenderResultsPanel(0, results_reveal_timer_, 15, py, width_ / 2 - 25, ph);
        
        // Player 2 Column (Right Half, expanded)
        RenderResultsPanel(1, results_reveal_timer_, width_ / 2 + 10, py, width_ / 2 - 25, ph);
    } else {
        // Calculate which player is active
        int active_p = players_[1].joined ? 1 : 0;
        
        // Use consistent half-screen panel on their side
        int pw = width_ / 2 - 25;
        // Strictly align to the side of the player
        int px = (active_p == 0) ? 15 : width_ / 2 + 10;

        RenderResultsPanel(active_p, results_reveal_timer_, px, py, pw, ph);
    }

    // --- Footer ---
    if (results_reveal_timer_ > 4.5) {
        double footer_alpha = std::min(1.0, (results_reveal_timer_ - 4.5) / 0.5);
        uint8_t a = static_cast<uint8_t>(255 * footer_alpha);
        font_.DrawText(renderer_, width_ / 2, height_ - 30, "PRESS [ENTER] OR [ESC] TO CONTINUE", {150, 150, 170, a}, FontSize::SMALL, TextAlign::CENTER);
    }
}

void GameWindow::RenderResultsPanel(int p, double t_total, int x, int y, int w, int h) {
    if (p < 0 || p >= MAX_PLAYERS) return;
    const auto& ps = players_[p];
    if (!ps.joined) return;
    
    // 1. Calculations
    int total_notes = ps.total_hittable_notes;
    double normal_acc = (total_notes > 0) ? (ps.normal_score / total_notes) : 0.0;
    double ex_acc     = (total_notes > 0) ? (ps.ex_score / total_notes) : 0.0;
    
    // Timing stats
    double mean = 0.0, stddev = 0.0;
    int fast = 0, slow = 0, hit_count = 0;
    for (const auto& hr : ps.hit_history) {
        if (hr.judge != Judgement::MISS && hr.judge != Judgement::NONE) {
            mean += hr.error;
            hit_count++;
            if (hr.error < -0.0001) fast++;
            else if (hr.error > 0.0001) slow++;
        }
    }
    if (hit_count > 0) {
        mean /= hit_count;
        double variance = 0.0;
        for (const auto& hr : ps.hit_history) {
            if (hr.judge != Judgement::MISS && hr.judge != Judgement::NONE) {
                variance += (hr.error - mean) * (hr.error - mean);
            }
        }
        stddev = std::sqrt(variance / hit_count);
    }

    // Panel reveal delay (staggered for P2)
    double t = t_total - (p * 0.2) - 0.2; 
    if (t < 0) return;
    
    // Panel Background Fade In
    uint8_t bg_a = static_cast<uint8_t>(220 * std::min(1.0, t / 0.3));
    DrawRect(x, y, w, h, {15, 15, 25, bg_a});
    DrawRectOutline(x, y, w, h, {80, 80, 110, static_cast<uint8_t>(bg_a * 0.8)});
    
    // Accent Side Bar
    Color p_col = (p == 0) ? Color{100, 150, 255, 255} : Color{255, 100, 150, 255};
    DrawRect(x, y, 4, h, {p_col.r, p_col.g, p_col.b, bg_a});

    int cx = x + w / 2;
    int cur_y = y + 20;

    // 1. Player Name
    font_.DrawText(renderer_, cx, cur_y, ps.profile.name, {255, 255, 255, bg_a}, FontSize::MEDIUM, TextAlign::CENTER);
    cur_y += 40;

    // 2. Grade & EX Star Reveal (Animated @ 0.5s)
    double grade_t = t - 0.5;
    if (grade_t > 0) {
        double grade_alpha = std::min(1.0, grade_t / 0.5);
        double grade_pop = 1.0 + 0.15 * std::pow(1.0 - grade_alpha, 3.0);
        uint8_t ga = static_cast<uint8_t>(255 * grade_alpha);

        if (ps.failed_sequence) {
             font_.DrawText(renderer_, cx, cur_y + 30, "FAILED", {255, 60, 60, ga}, FontSize::GIANT, TextAlign::CENTER, grade_pop);
             cur_y += 100;
        } else {
            // Normal Grade Lookup
            auto getGrade = [&](double acc) -> std::string {
                if (acc >= 100.9) return "SSSplus";
                if (acc >= 100.75) return "SSS";
                if (acc >= 100.5) return "SSplus";
                if (acc >= 100.0) return "SS";
                if (acc >= 99.0) return "Splus";
                if (acc >= 97.5) return "S";
                if (acc >= 95.0) return "AAA";
                if (acc >= 92.5) return "AA";
                if (acc >= 90.0) return "A";
                if (acc >= 80.0) return "BBB";
                if (acc >= 70.0) return "BB";
                if (acc >= 60.0) return "B";
                if (acc >= 50.0) return "C";
                return "D";
            };
            std::string grade_key = getGrade(normal_acc);
            
            // Draw Normal Grade Sprite
            SDL_Texture* gn_tex = normal_grade_textures_.count(grade_key) ? normal_grade_textures_[grade_key] : nullptr;
            if (gn_tex) {
                int tw, th; SDL_QueryTexture(gn_tex, nullptr, nullptr, &tw, &th);
                double s = 110.0 / th * grade_pop;
                SDL_Rect r = { cx - (int)(tw*s)/2 - 40, cur_y, (int)(tw*s), (int)(th*s) };
                SDL_SetTextureAlphaMod(gn_tex, ga);
                SDL_RenderCopy(renderer_, gn_tex, nullptr, &r);
                SDL_SetTextureAlphaMod(gn_tex, 255);
            } else {
                font_.DrawText(renderer_, cx - 40, cur_y + 40, grade_key, {255, 255, 255, ga}, FontSize::GIANT, TextAlign::CENTER, grade_pop);
            }

            // EX Star Grade (per EX.md)
            int stars = 0;
            if (ex_acc >= 100.0) stars = 6;
            else if (ex_acc >= 95.0) stars = 5;
            else if (ex_acc >= 88.5) stars = 4;
            else if (ex_acc >= 82.0) stars = 3;
            else if (ex_acc >= 73.0) stars = 2;
            else if (ex_acc >= 60.0) stars = 1;

            if (stars > 0) {
                SDL_Texture* ex_tex = ex_grade_textures_.count(stars) ? ex_grade_textures_[stars] : nullptr;
                if (ex_tex) {
                    int tw, th; SDL_QueryTexture(ex_tex, nullptr, nullptr, &tw, &th);
                    double s = 60.0 / th * grade_pop;
                    SDL_Rect r = { cx + 60, cur_y + 25, (int)(tw*s), (int)(th*s) };
                    SDL_SetTextureAlphaMod(ex_tex, ga);
                    SDL_RenderCopy(renderer_, ex_tex, nullptr, &r);
                    SDL_SetTextureAlphaMod(ex_tex, 255);
                }
            }
            cur_y += 110;
        }
    } else {
        cur_y += 110;
    }

    // 3. Accuracy & EX Score (Reveal @ 1.2s)
    double score_t = t - 1.2;
    if (score_t > 0) {
        double score_progress = std::min(1.0, score_t / 1.5);
        double eased_p = 1.0 - std::pow(1.0 - score_progress, 3.0);
        uint8_t sa = static_cast<uint8_t>(255 * std::min(1.0, score_t / 0.3));

        // Normal Accuracy
        font_.DrawText(renderer_, cx - 100, cur_y, "ACCURACY (NORMAL)", {180, 180, 200, sa}, FontSize::SMALL, TextAlign::CENTER);
        font_.DrawAccuracy(renderer_, cx - 100, cur_y + 25, normal_acc * eased_p, {255, 255, 255, sa}, TextAlign::CENTER, 0.7, 4, true, true, {0,0,0,0}, FontSize::HUGE);

        // EX Accuracy
        font_.DrawText(renderer_, cx + 110, cur_y, "ACCURACY (EX)", {220, 200, 180, sa}, FontSize::SMALL, TextAlign::CENTER);
        font_.DrawAccuracy(renderer_, cx + 110, cur_y + 25, ex_acc * eased_p, {255, 220, 150, sa}, TextAlign::CENTER, 0.7, 2, true, true, {0,0,0,0}, FontSize::HUGE);
    }
    cur_y += 85;

    // 4. Rating (Reveal @ 2.0s)
    double rating_t = t - 2.0;
    if (rating_t > 0) {
        double r_alpha = std::min(1.0, rating_t / 0.5);
        uint8_t ra = static_cast<uint8_t>(255 * r_alpha);
        
        // Use authoritative formula from Profile (pass normal_acc as percentage 0-101)
        double rating = Profile::CalculateChartRating(ps.current_chart ? ps.current_chart->custom_difficulty : 0.0, normal_acc);
        
        font_.DrawText(renderer_, cx, cur_y, "EARNED RATING", {150, 200, 220, ra}, FontSize::SMALL, TextAlign::CENTER);
        RenderRating(cx, cur_y + 22, rating * std::min(1.0, rating_t / 1.0), TextAlign::CENTER, p == 1);
    }
    cur_y += 75;

    // 5. Judge Counters & Stats (Reveal @ 2.5s)
    double stats_t = t - 2.5;
    if (stats_t > 0) {
        uint8_t row_a = static_cast<uint8_t>(255 * std::min(1.0, stats_t / 0.5));
        
        // Judgement List (Two columns inside panel)
        const char* j_names[] = {"P-EXTRA", "P-CRIT", "PERFECT", "GREAT", "GOOD", "MISS"};
        int j_indices[] = {static_cast<int>(Judgement::PEXTRA)-1, static_cast<int>(Judgement::PCRIT)-1, static_cast<int>(Judgement::PERFECT)-1, 
                           static_cast<int>(Judgement::GREAT)-1, static_cast<int>(Judgement::GOOD)-1, static_cast<int>(Judgement::MISS)-1};
        
        int jy_start = cur_y;
        for (int i = 0; i < 6; ++i) {
            int ry = jy_start + i * 22;
            int count = (j_indices[i] >= 0) ? ps.normal_judge_counts[j_indices[i]] : 0;
            
            Color jc = GetJudgementColor(static_cast<Judgement>(j_indices[i]+1));
            jc.a = row_a;
            
            font_.DrawText(renderer_, x + 60, ry, j_names[i], jc, FontSize::SMALL, TextAlign::LEFT);
            font_.DrawText(renderer_, cx - 20, ry, std::to_string(count), {255, 255, 255, row_a}, FontSize::SMALL, TextAlign::RIGHT);
        }

        // Hold/Roll Judgements (Reveal @ 2.8s)
        int hy_start = jy_start + 140;
        const char* h_names[] = {"HOLD GOOD", "HOLD NG", "HOLD BAD"};
        Color h_colors[] = {{100, 255, 100, row_a}, {255, 255, 100, row_a}, {255, 100, 100, row_a}};
        for (int i = 0; i < 3; ++i) {
            int ry = hy_start + i * 22;
            int count = ps.hold_judgement_counts[i];
            font_.DrawText(renderer_, x + 60, ry, h_names[i], h_colors[i], FontSize::SMALL, TextAlign::LEFT);
            font_.DrawText(renderer_, cx - 20, ry, std::to_string(count), {255, 255, 255, row_a}, FontSize::SMALL, TextAlign::RIGHT);
        }

        // Stats Column (Mean, StDev, Fast, Slow)
        int sx = cx + 60;
        int sy = jy_start;
        char buf[64];
        
        auto drawStat = [&](int row, const char* label, const char* value, Color col = {200,200,220,255}) {
            int ry = sy + row * 22;
            font_.DrawText(renderer_, sx, ry, label, col, FontSize::SMALL, TextAlign::LEFT);
            font_.DrawText(renderer_, x + w - 60, ry, value, {255,255,255,row_a}, FontSize::SMALL, TextAlign::RIGHT);
        };

        std::snprintf(buf, sizeof(buf), "%.2f ms", mean * 1000.0);
        drawStat(0, "Mean Error", buf);
        std::snprintf(buf, sizeof(buf), "%.2f ms", stddev * 1000.0);
        drawStat(1, "Std Dev", buf);
        
        std::snprintf(buf, sizeof(buf), "%d", fast);
        drawStat(3, "Fast", buf, {100, 200, 255, row_a});
        std::snprintf(buf, sizeof(buf), "%d", slow);
        drawStat(4, "Slow", buf, {255, 100, 100, row_a});

        // Max Combo
        std::snprintf(buf, sizeof(buf), "%d", ps.max_combo);
        drawStat(5, "Max Combo", buf, {255, 255, 150, row_a});
    }

    // 6. Detailed Graph (Reveal @ 4.5s)
    if (t > 4.5) {
        RenderOffsetGraph(x + 30, y + h - 110, w - 60, 90, ps.hit_history);
    }
}

void GameWindow::RenderOffsetGraph(int x, int y, int w, int h, const std::vector<HitRecord>& hits) {
    DrawRect(x, y, w, h, {0, 0, 0, 120});
    DrawRectOutline(x, y, w, h, {100, 100, 120, 150});
    
    int cy = y + h / 2;
    DrawRect(x, cy, w, 1, {255, 255, 255, 60}); // Center line
    
    double range = 0.100; // +/- 100ms
    double duration = (hits.empty()) ? 1.0 : hits.back().time + 1.0;

    for (const auto& hit : hits) {
        if (hit.judge == Judgement::MISS) continue;
        int dx = x + static_cast<int>((hit.time / duration) * w);
        int dy = cy + static_cast<int>((hit.error / range) * (h / 2));
        dy = std::clamp(dy, y + 2, y + h - 2);
        
        Color c = GetJudgementColor(hit.judge, false);
        DrawRect(dx - 1, dy - 1, 3, 3, c);
    }
}

// ===================================

// ============================================================================
// Hit Detection
// ============================================================================

void GameWindow::ProcessLaneHit(int lane, double forced_time, int p, bool is_release) {
    if (p < 0 || p >= MAX_PLAYERS || !players_[p].joined) return;
    auto& ps = players_[p];
    if (ps.failed_sequence) return; // Block hits after fail
    const NoteChart* chart = ps.current_chart;
    if (!chart || !playing_) return;

    int actual_lane = lane;
    if (effect_mode_ == 1) { // Mirror
        actual_lane = chart->num_columns - 1 - lane;
    }

    bool is_autoplay = (forced_time >= 0.0);
    double current_time = is_autoplay ? forced_time : play_time_;
    double best_error = 999.0;
    size_t best_idx = SIZE_MAX;

    // Search around the current hittable note index
    size_t search_start = (ps.next_hittable_note > 5) ? ps.next_hittable_note - 5 : 0;
    size_t search_end = std::min(chart->note_rows.size(), ps.next_hittable_note + 20);

    for (size_t i = search_start; i < search_end; ++i) {
        const auto& row = chart->note_rows[i];
        if (lane >= static_cast<int>(row.columns.size())) continue;
        
        // Use player-specific hit mask
        if (ps.note_hit_masks[i] & (1 << actual_lane)) continue;

        NoteType nt = row.columns[static_cast<size_t>(lane)];
        if (!IsTap(nt)) continue;

        // Lift Check: nt == Lift requires is_release, other Taps require !is_release
        bool nt_is_lift = (nt == NoteType::Lift);
        if (is_release != nt_is_lift) continue;

        double note_time = conductor_.BeatToTime(row.beat);
        double error = current_time - note_time;
        double abs_error = std::fabs(error);
        double max_window = ex_mode_ ? JudgeWindowsEX::MISS : JudgeWindows::GOOD;

        if (abs_error <= max_window && abs_error < std::fabs(best_error)) {
            best_error = error;
            best_idx = i;
        }
    }

    if (best_idx == SIZE_MAX) return;

    ps.note_hit_masks[best_idx] |= (1 << actual_lane);
    
    double abs_error_cur = std::fabs(best_error);
    // Chord Cohesion: worst hit determines the row's error
    if (abs_error_cur > std::fabs(ps.row_best_error[best_idx]) || ps.row_best_error[best_idx] == 999.0f) {
        ps.row_best_error[best_idx] = static_cast<float>(best_error);
    }

    Judgement j_visual = sml::ClassifyHit(abs_error_cur, ex_mode_);
    ps.input.RecordHit(actual_lane, j_visual, best_error);
    ps.hit_history.push_back({current_time, best_error, j_visual, actual_lane, ex_mode_});

    // Check for Hold/Roll head to start tracking
    const auto& row = chart->note_rows[best_idx];
    NoteType nt = row.columns[static_cast<size_t>(lane)];
    if (nt == NoteType::HoldHead || nt == NoteType::RollHead) {
        int tail_idx = row.tail_row_indices[actual_lane];
        if (tail_idx >= 0) {
            const auto& tail_row = chart->note_rows[tail_idx];
            double start_t = conductor_.BeatToTime(row.beat);
            double end_t = conductor_.BeatToTime(tail_row.beat);
            
            ActiveHold ah;
            ah.row_index = best_idx;
            ah.col_index = actual_lane;
            ah.type = nt;
            ah.start_time = start_t;
            ah.end_time = end_t;
            ah.total_duration = end_t - start_t;
            ah.time_held = 0.0;
            ah.current_release_time = 0.0;
            ah.broken_continuity = false;
            
            if (nt == NoteType::RollHead) {
                // One tick per beat correctly rounded
                ah.required_ticks = static_cast<int>(std::ceil(tail_row.beat - row.beat));
                if (ah.required_ticks < 1) ah.required_ticks = 1;
                ah.ticks_hit = 0;
            } else {
                ah.required_ticks = 0;
                ah.ticks_hit = 0;
            }
            
            ah.start_beat = row.beat;
            ah.last_autoplay_tick_beat = row.beat - 1.0; // Ensure first tick happens immediately if needed

            ps.active_holds.push_back(ah);
        }
    }

    // Spawn hit flash
    double note_beat = chart->note_rows[best_idx].beat;
    double note_y = NoteRenderer::GetYPosForBeat(note_beat, conductor_, ps.field_config);
    ps.hit_flashes.push_back({actual_lane, note_beat, 0.3, note_y, j_visual});

    // Scoring
    bool is_coop = (chart->chart_type == "dance-routine" || chart->chart_type == "dance-couple");
    
    if (is_coop) {
        // Routine/Couple: Individual note scoring
        UpdateScores(abs_error_cur, 1, p);
        Judgement j_target = ex_mode_ ? ClassifyHit(abs_error_cur, true) : ClassifyHit(abs_error_cur, false);
        ps.last_judgement = j_target;
        ps.last_timing_error = best_error;
        ps.judgement_timer = 0.6;

        if (j_target != Judgement::MISS) {
            ps.combo++;
            if (ps.combo > ps.max_combo) ps.max_combo = ps.combo;
            ps.combo_pop_timer = 0.15;
            if (ps.combo > 0 && ps.combo % 100 == 0) ps.grade_popup_timer = 0.2;
        } else {
            ps.combo = 0;
        }
    } else {
        // Standard: Chord cohesion check
        bool fully_hit = true;
        int hittable_count = 0;
        for (size_t c = 0; c < chart->note_rows[best_idx].columns.size(); ++c) {
            if (IsTap(chart->note_rows[best_idx].columns[c])) {
                hittable_count++;
                if (!(ps.note_hit_masks[best_idx] & (1 << c))) fully_hit = false;
            }
        }

        if (fully_hit) {
            double final_abs_error = std::fabs(ps.row_best_error[best_idx]);
            UpdateScores(final_abs_error, hittable_count, p);
            
            Judgement j_target = ex_mode_ ? ClassifyHit(final_abs_error, true) : ClassifyHit(final_abs_error, false);
            ps.last_judgement = j_target;
            ps.last_timing_error = ps.row_best_error[best_idx];
            ps.judgement_timer = 0.6;

            if (j_target != Judgement::MISS) {
                ps.combo++;
                if (ps.combo > ps.max_combo) ps.max_combo = ps.combo;
                ps.combo_pop_timer = 0.15;
                if (ps.combo > 0 && ps.combo % 100 == 0) ps.grade_popup_timer = 0.2;
            } else {
                ps.combo = 0;
            }

            // Sync legacy for active player
            if (p == active_player_idx_) {
                combo_ = ps.combo;
                last_judgement_ = ps.last_judgement;
                judgement_timer_ = 0.6;
            }
        }
    }

    // Advance ps.next_hittable_note past fully-hit or non-hittable rows
    while (ps.next_hittable_note < chart->note_rows.size() &&
           (GetRowRequiredMask(ps.next_hittable_note) == 0 || IsRowFullyHit(ps.next_hittable_note, p))) {
        ps.next_hittable_note++;
    }
}

// Update
// ============================================================================

void GameWindow::ChangeScreen(ScreenState next) {
    if (screen_ == next || (is_transitioning_ && target_screen_ == next)) return;
    
    target_screen_ = next;
    is_transitioning_ = true;
    screen_transition_timer_ = 0.0;
}

void GameWindow::Update(double dt) {
    global_anim_timer_ += dt;

    // --- Screen Transition Update ---
    if (is_transitioning_) {
        double prev_timer = screen_transition_timer_;
        screen_transition_timer_ += dt;
        double duration = 0.8; // Total transition time
        double half_duration = duration / 2.0;

        if (prev_timer < half_duration && screen_transition_timer_ >= half_duration) {
            ScreenState old_screen = screen_;
            screen_ = target_screen_;
            
            if (screen_ == ScreenState::RESULTS) {
                // Initialize results strings once
                if (active_simfile_) results_title_ = active_simfile_->title;
                if (current_chart_) {
                    std::string mode_name = GetChartModeName(current_chart_->chart_type);
                    results_chart_info_ = mode_name + " " + current_chart_->difficulty_name;
                }
                results_reveal_timer_ = 0.0;
            }
            
            // Fix: Stop music when leaving gameplay
            if (old_screen == ScreenState::GAMEPLAY && screen_ == ScreenState::SONG_SELECT) {
                audio_.Stop();
                audio_.UnloadMusic();
                audio_loaded_ = false;
                playing_ = false;
                video_path_ = "";
            }
        }

        if (screen_transition_timer_ >= duration) {
            is_transitioning_ = false;
            screen_transition_timer_ = 0.0;
        }
    }

    // --- Attraction & Profile Load Updates ---
    if (screen_ == ScreenState::ATTRACTION) {
        UpdateAttraction(dt);
        return;
    }
    if (screen_ == ScreenState::PROFILE_LOAD) {
        UpdateProfileLoad(dt);
        return;
    }

    if (screen_ == ScreenState::DECIDE) {
        if (!is_transitioning_) {
            decide_timer_ -= dt;
            if (decide_timer_ <= 0.0) {
                decide_timer_ = 0.0;
                StartGameplayDirect(); // Delayed start!
                ChangeScreen(ScreenState::GAMEPLAY);
                ready_animation_timer_ = 2.0;
            }
        }
        return; 
    }

    if (screen_ == ScreenState::GAMEPLAY) {
        // --- Get Ready Animation Delay ---
        if (!is_transitioning_ && ready_animation_timer_ > 0.0) {
            ready_animation_timer_ -= dt;
            
            if (ready_animation_timer_ <= 0.0) {
                ready_animation_timer_ = 0.0;
                // Music start logic is now handled in the centralized block below
            }
        }

        // --- Clear Type Animation Timer ---
        if (clear_animation_timer_ > 0.0) {
            clear_animation_timer_ -= dt;
            if (clear_animation_timer_ < 0.0) clear_animation_timer_ = 0.0;
        }

        // Get time from audio if available, otherwise accumulate dt
        if (playing_) {
            if (audio_loaded_ && audio_.IsPlaying()) {
                double audio_time = audio_.GetPlaybackTime();
                // Monotonicity guard: never jump backwards during the transition or due to jitter
                if (audio_time >= play_time_) {
                    play_time_ = audio_time;
                } else {
                    // Audio hasn't caught up to our lead-in accumulation yet, or jitter occurred
                    play_time_ += dt;
                }
            } else {
                play_time_ += dt * time_scale_;
            }
        } else {
            // Even when not playing music (lead-in), we must advance time for the notefield
            play_time_ += dt * time_scale_;

            // Trigger music start if we hit 0.0
            if (play_time_ >= 0.0 && !playing_) {
                 if (audio_loaded_ && !is_calibrating_) {
                     // Start at play_time_ to account for any slight overshoot
                     audio_.Play(play_time_);
                 }
                 playing_ = true;
            }
        }
        
        conductor_.Update(play_time_);

        // Calculate relative video playback time
        double bga_time = 0.0;
        double current_beat = conductor_.GetCurrentBeat();
        
        if (active_simfile_ && !active_simfile_->bg_changes.empty()) {
            const BGEvent* best_ev = nullptr;
            for (const auto& ev : active_simfile_->bg_changes) {
                if (ev.beat <= current_beat) {
                    best_ev = &ev;
                } else {
                    break;
                }
            }
            if (best_ev) {
                double ev_start_time = conductor_.BeatToTime(best_ev->beat);
                bga_time = play_time_ - ev_start_time;
            }
        } else if (!auto_bga_file_.empty() && current_beat >= 0.0) {
            // Auto-discovered BGAs default to 0.0s of the audio track
            bga_time = play_time_ - audio_offset_;
        }
        
        video_decoder_.Update(bga_time);
        input_.Update(dt);
        // Update per-player InputMappers (flash decay)
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (players_[p].joined) {
                players_[p].input.Update(dt);
            }
        }

        // Beat flash
        double beat = conductor_.GetCurrentBeat();
        double beat_frac = beat - std::floor(beat);
        beat_flash_ = (beat_frac < 0.1) ? 1.0 - (beat_frac / 0.1) : 0.0;

        if (judgement_timer_ > 0.0) {
            judgement_timer_ -= dt;
            if (judgement_timer_ < 0.0) judgement_timer_ = 0.0;
        }

        // Combo pop timer
        if (combo_pop_timer_ > 0.0) {
            combo_pop_timer_ -= dt;
            if (combo_pop_timer_ < 0.0) combo_pop_timer_ = 0.0;
        }

        // Grade popup timer
        if (grade_popup_timer_ > 0.0) {
            grade_popup_timer_ -= dt;
            if (grade_popup_timer_ < 0.0) grade_popup_timer_ = 0.0;
        }

        // Update per-player timers and hit flashes
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (players_[p].joined) {
                auto& ps = players_[p];
                if (ps.judgement_timer > 0.0) ps.judgement_timer = std::max(0.0, ps.judgement_timer - dt);
                if (ps.combo_pop_timer > 0.0) ps.combo_pop_timer = std::max(0.0, ps.combo_pop_timer - dt);
                if (ps.grade_popup_timer > 0.0) ps.grade_popup_timer = std::max(0.0, ps.grade_popup_timer - dt);
                if (ps.score_anim_timer > 0.0) ps.score_anim_timer = std::max(0.0, ps.score_anim_timer - dt);

                // Update hit flash timers
                for (auto& hf : ps.hit_flashes) {
                    hf.timer -= dt;
                }
                ps.hit_flashes.erase(
                    std::remove_if(ps.hit_flashes.begin(), ps.hit_flashes.end(),
                        [](const HitFlash& hf) { return hf.timer <= 0.0; }),
                    ps.hit_flashes.end());

                // 3. Update active holds/rolls
                double current_beat = conductor_.GetCurrentBeat();

                for (auto it = ps.active_holds.begin(); it != ps.active_holds.end(); ) {
                    auto& ah = *it;
                    bool is_held = ps.input.GetLaneState(ah.col_index).pressed;
                    
                    // Autoplay Override
                    if (autoplay_) {
                        if (ah.type == NoteType::HoldHead) {
                            is_held = true;
                        } else if (ah.type == NoteType::RollHead) {
                            // Automatically tick rolls based on beat progression
                            // Tick every integer beat relative to start
                            // Simple logic: if we crossed a beat boundary since last tick
                            if (current_beat >= ah.last_autoplay_tick_beat + 1.0) {
                                ah.ticks_hit++;
                                ah.last_autoplay_tick_beat += 1.0;
                                // Catch up (if low FPS or big jump)
                                while (current_beat >= ah.last_autoplay_tick_beat + 1.0) {
                                     ah.ticks_hit++;
                                     ah.last_autoplay_tick_beat += 1.0;
                                }
                            }
                        }
                    }

                    if (ah.type == NoteType::HoldHead) {
                        if (is_held) {
                            ah.time_held += dt;
                            ah.current_release_time = 0.0;
                        } else {
                            ah.current_release_time += dt;
                            if (ah.current_release_time > 0.250) ah.broken_continuity = true;
                        }
                    } else if (ah.type == NoteType::RollHead) {
                        if (!autoplay_ && ps.input.GetLaneState(ah.col_index).just_pressed) {
                            ah.ticks_hit++;
                        }
                    }

                    // Check for end of hold/roll
                    if (play_time_ >= ah.end_time) {
                        int grade = 2; // Default Bad
                        if (ah.type == NoteType::HoldHead) {
                            double held_pct = ah.time_held / ah.total_duration;
                            if (held_pct > 0.8 && !ah.broken_continuity) grade = 0; // Good
                            else if (held_pct > 0.4) grade = 1; // NG
                        } else {
                            double roll_pct = static_cast<double>(ah.ticks_hit) / ah.required_ticks;
                            if (roll_pct >= 1.0) grade = 0; // Good
                            else if (roll_pct > 0.5) grade = 1; // NG
                        }
                        
                        ps.hold_judgement_counts[grade]++;
                        ps.life_meter.OnHoldResult(grade);

                        // Spawn visual indicator
                        ps.hold_indicators.push_back({ah.col_index, grade, 0.6});

                        // Update Clear Type (cap it)
                        if (grade == 2) { // Bad
                            if (ps.clear_type > ClearType::CLEAR) ps.clear_type = ClearType::CLEAR;
                        } else if (grade == 1) { // NG
                            if (ps.clear_type > ClearType::FULL_COMBO) ps.clear_type = ClearType::FULL_COMBO;
                        }

                        it = ps.active_holds.erase(it);
                    } else {
                        ++it;
                    }
                }

                // 4. Update hold indicators
                for (auto it = ps.hold_indicators.begin(); it != ps.hold_indicators.end(); ) {
                    it->timer -= dt;
                    if (it->timer <= 0.0) {
                        it = ps.hold_indicators.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        // Auto-miss & Per-Player Gameplay Update
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (!players_[p].joined) continue;
            auto& ps = players_[p];
            const NoteChart* chart = ps.current_chart;
            if (!chart) continue;

            // 1. Autoplay Logic
            if (autoplay_ && playing_ && !ps.failed_sequence) {
                while (ps.next_hittable_note < chart->note_rows.size()) {
                    size_t idx = ps.next_hittable_note;
                    const auto& row = chart->note_rows[idx];
                    double note_time = conductor_.BeatToTime(row.beat);
                    if (play_time_ >= note_time) {
                        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                            NoteType nt = row.columns[col];
                            if (IsTap(nt) && !(ps.note_hit_masks[idx] & (1 << col))) {
                                ProcessLaneHit(col, note_time, p, (nt == NoteType::Lift));
                            }
                        }
                        // If ProcessLaneHit didn't advance past this row
                        // (e.g. row had only mines/holds), advance manually
                        if (ps.next_hittable_note == idx) {
                            ps.next_hittable_note++;
                        }
                    } else break;
                }
            }

            // 2. Miss Detection
            if (!ps.failed_sequence) {
                while (ps.next_hittable_note < chart->note_rows.size()) {
                    // Skip rows with no tappable notes (mines, hold tails, etc.)
                    if (GetRowRequiredMask(ps.next_hittable_note) == 0) {
                        ps.next_hittable_note++;
                        continue;
                    }

                    const auto& row = chart->note_rows[ps.next_hittable_note];
                    double note_time = conductor_.BeatToTime(row.beat);
                    double error = play_time_ - note_time;
                    double max_window = ex_mode_ ? JudgeWindowsEX::MISS : JudgeWindows::GOOD;

                    if (error > max_window) {
                        bool fully_hit = IsRowFullyHit(ps.next_hittable_note, p);
                        if (!fully_hit) {
                            int hittable_count = GetRowHittableCount(ps.next_hittable_note);
                            ps.combo = 0;
                            UpdateScores(999.0, hittable_count, p);
                            ps.hit_history.push_back({note_time, 999.0, Judgement::MISS, -1, ex_mode_});
                            ps.last_judgement = Judgement::MISS;
                            ps.judgement_timer = 0.6;

                            // Spawn "Bad" indicator for missed hold/roll heads
                            for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                                NoteType nt = row.columns[col];
                                if (nt == NoteType::HoldHead || nt == NoteType::RollHead) {
                                    ps.hold_indicators.push_back({col, 2, 0.6}); // 2 = Bad
                                }
                            }

                            if (ps.clear_type > ClearType::CLEAR) ps.clear_type = ClearType::CLEAR;
                        }
                        ps.next_hittable_note++;
                    } else break;
                }
            }

            // 3. Mine Detection (Rising-edge only: tap within 75ms window)
            double current_beat = conductor_.GetCurrentBeat();
            constexpr double MINE_WINDOW = 0.075; // 75ms one-sided
            size_t m_start = (ps.next_hittable_note > 20) ? ps.next_hittable_note - 20 : 0;
            size_t m_end = std::min(chart->note_rows.size(), ps.next_hittable_note + 50);

            for (size_t i = m_start; i < m_end; ++i) {
                const auto& row = chart->note_rows[i];
                double mine_time = conductor_.BeatToTime(row.beat);
                double time_diff = play_time_ - mine_time;
                if (time_diff > MINE_WINDOW) continue;   // Mine already passed window
                if (time_diff < -MINE_WINDOW) break;      // Mine not yet in window

                for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                    if (row.columns[col] == NoteType::Mine && !(ps.note_hit_masks[i] & (1 << col)) && !ps.failed_sequence) {
                        if (ps.input.GetLaneState(col).just_pressed) {
                            ps.note_hit_masks[i] |= (1 << col);
                            ps.life_meter.OnMineHit();
                            if (p == active_player_idx_) life_meter_.OnMineHit();
                        }
                    }
                }
            }

            // 4. End-of-Chart Detection
            if (!ps.chart_finished && !ps.failed_sequence && ps.chart_end_time > 0.0 &&
                ps.next_hittable_note >= chart->note_rows.size() &&
                play_time_ > ps.chart_end_time) {
                ps.chart_finished = true;
                ps.results_delay = (ps.clear_type >= ClearType::FULL_COMBO) ? 4.5 : 1.5;
            }

            if (ps.failed_sequence) {
                ps.fail_animation_timer += dt;
            }

            // --- Life Gauge Animations ---
            LifeType type = ps.life_meter.GetType();
            
            double intro_progress = 1.0;
            if (ready_animation_timer_ > 0.0) {
                intro_progress = std::clamp((2.0 - ready_animation_timer_) / 1.0, 0.0, 1.0);
            }
            float target_life = ps.life_meter.GetLife() * static_cast<float>(intro_progress);

            if (ps.displayed_life > target_life) {
                ps.displayed_life = std::max(target_life, static_cast<float>(ps.displayed_life - dt * 0.5));
            } else {
                ps.displayed_life = target_life;
            }

            if (type == LifeType::STANDARD) {
                int active_segs = static_cast<int>(std::ceil(target_life * 20));
                if (active_segs > ps.last_active_segments) {
                    for (int i = ps.last_active_segments; i < active_segs && i < 20; ++i) ps.segment_flash_timers[i] = 1.0;
                } else if (active_segs < ps.last_active_segments) {
                    for (int i = active_segs; i < ps.last_active_segments && i < 20; ++i) ps.segment_flash_timers[i] = -1.0;
                }
                ps.last_active_segments = active_segs;
                for (int i = 0; i < 20; ++i) {
                    if (ps.segment_flash_timers[i] > 0.0) ps.segment_flash_timers[i] = std::max(0.0, ps.segment_flash_timers[i] - dt * 3.0);
                    else if (ps.segment_flash_timers[i] < 0.0) ps.segment_flash_timers[i] = std::min(0.0, ps.segment_flash_timers[i] + dt * 3.0);
                }
            } else if (type == LifeType::LIFE4 || type == LifeType::RISKY) {
                int total_batt = (type == LifeType::LIFE4) ? 4 : 1;
                int active_batt = static_cast<int>(std::ceil((target_life / std::max(0.01f, ps.life_meter.GetLife())) * ps.life_meter.GetBatteryLives()));
                if (intro_progress >= 1.0) active_batt = ps.life_meter.GetBatteryLives(); // ensure it syncs exactly after intro
                
                if (active_batt < ps.last_battery_lives) {
                    for (int i = active_batt; i < ps.last_battery_lives && i < 4; ++i) ps.battery_flash_timers[i] = 1.0;
                } else if (active_batt > ps.last_battery_lives) {
                    for (int i = ps.last_battery_lives; i < active_batt && i < 4; ++i) ps.battery_flash_timers[i] = -1.0;
                }
                ps.last_battery_lives = active_batt;
                for (int i = 0; i < 4; ++i) {
                    if (ps.battery_flash_timers[i] > 0.0) ps.battery_flash_timers[i] = std::max(0.0, ps.battery_flash_timers[i] - dt * 2.0);
                    else if (ps.battery_flash_timers[i] < 0.0) ps.battery_flash_timers[i] = std::min(0.0, ps.battery_flash_timers[i] + dt * 2.0);
                }
            } else if (type == LifeType::FLARE) {
                if (target_life < ps.last_flare_life && intro_progress >= 1.0) {
                    ps.flare_flash_timer = 1.0;
                }
                ps.last_flare_life = target_life;
                if (ps.flare_flash_timer > 0.0) ps.flare_flash_timer = std::max(0.0, ps.flare_flash_timer - dt * 4.0);
            }

            // --- RISKY Shatter State Machine ---
            if (type == LifeType::RISKY && ps.shatter_state != ShatterState::NONE) {
                ps.shatter_timer += dt;

                if (ps.shatter_state == ShatterState::CRACKING) {
                    // Check if we should transition to SHATTERING
                    bool all_players_failed = true;
                    for (int j = 0; j < MAX_PLAYERS; ++j) {
                        if (players_[j].joined && !players_[j].failed_sequence) {
                            all_players_failed = false;
                            break;
                        }
                    }

                    bool should_shatter = false;
                    if (all_players_failed) {
                        // All dead: shatter after 1.5s of cracking
                        if (ps.shatter_timer >= 1.5) should_shatter = true;
                    } else {
                        // Other player alive: check if they also use RISKY and are alive
                        bool other_risky_alive = false;
                        for (int j = 0; j < MAX_PLAYERS; ++j) {
                            if (j == p || !players_[j].joined) continue;
                            if (players_[j].life_meter.GetType() == LifeType::RISKY && !players_[j].failed_sequence) {
                                other_risky_alive = true;
                            }
                        }
                        if (other_risky_alive) {
                            // Wait indefinitely (other RISKY player still alive)
                        } else {
                            // Other player is non-RISKY or non-existent: shatter independently after 1.5s
                            if (ps.shatter_timer >= 1.5) should_shatter = true;
                        }
                    }

                    if (should_shatter) {
                        ps.shatter_state = ShatterState::SHATTERING;
                        ps.shatter_timer = 0.0;

                        // Spawn particles from the gauge slot position
                        int bar_w_est = 22;
                        int bar_x_est = (p == 0) ? 4 : (width_ - bar_w_est - 4);
                        int bar_y_est = 80;
                        int bar_h_est = height_ - 160;
                        for (int k = 0; k < 18; ++k) {
                            ShatterParticle sp;
                            sp.x = static_cast<float>(bar_x_est + (std::rand() % bar_w_est));
                            sp.y = static_cast<float>(bar_y_est + (std::rand() % bar_h_est));
                            sp.vx = static_cast<float>((std::rand() % 200) - 100);
                            sp.vy = static_cast<float>((std::rand() % 150) - 120);
                            sp.rot = 0.0f;
                            sp.rot_v = static_cast<float>((std::rand() % 600) - 300) * 0.01f;
                            sp.w = static_cast<float>(4 + (std::rand() % 8));
                            sp.h = static_cast<float>(3 + (std::rand() % 6));
                            double pulse = 0.5 + 0.5 * std::sin(play_time_ * 6.0);
                            uint8_t wb = static_cast<uint8_t>(180 + 75 * pulse);
                            sp.c = {wb, wb, wb, 255};
                            sp.alpha = 255.0f;
                            ps.shatter_particles.push_back(sp);
                        }

                        // Stop audio if all players are now failed
                        if (all_players_failed) {
                            audio_.Stop();
                        }
                    }
                }

                // Update shatter particles physics
                if (ps.shatter_state == ShatterState::SHATTERING) {
                    for (auto& sp : ps.shatter_particles) {
                        sp.vy += static_cast<float>(dt * 400.0); // gravity
                        sp.x += sp.vx * static_cast<float>(dt);
                        sp.y += sp.vy * static_cast<float>(dt);
                        sp.rot += sp.rot_v * static_cast<float>(dt);
                        sp.alpha -= static_cast<float>(dt * 180.0); // fade over ~1.4s
                    }
                }
            }
        }

        // Unified End-of-Game Detection
        bool all_done = true;
        bool any_alive = false;
        int active_count = 0;
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (!players_[i].joined) continue;
            active_count++;
            
            bool player_done = false;
            if (players_[i].chart_finished && players_[i].results_delay <= 0.0) player_done = true;
            // RISKY players need longer for the shatter sequence
            double fail_threshold = 2.0;
            if (players_[i].life_meter.GetType() == LifeType::RISKY) fail_threshold = 4.0;
            if (players_[i].failed_sequence && players_[i].fail_animation_timer >= fail_threshold) player_done = true;
            
            if (!player_done) all_done = false;
            if (!players_[i].failed_sequence) any_alive = true;

            // Staggered results delay update
            if (players_[i].chart_finished && !players_[i].failed_sequence) {
                players_[i].results_delay -= dt;
            }
        }

        // Gradual time_scale_ deceleration when all players have failed
        if (!any_alive && time_scale_ > 0.0) {
            time_scale_ = std::max(0.0, time_scale_ - dt * 2.0); // halt over ~0.5s
        }

        if (active_count > 0 && all_done) {
            if (!any_alive) {
                audio_.Stop(); // Everyone failed
            }
            ChangeScreen(ScreenState::RESULTS);
        }
    } else if (screen_ == ScreenState::RESULTS) {
        results_reveal_timer_ += dt;
    }
}

// ============================================================================
// Rendering dispatch
// ============================================================================

void GameWindow::Render() {
    switch (screen_) {
        case ScreenState::ATTRACTION:      RenderAttraction(); break;
        case ScreenState::PROFILE_LOAD:    RenderProfileLoad(); break;
        case ScreenState::PROFILE_MANAGE:  RenderProfileManage(); break;
        case ScreenState::SONG_SELECT:     RenderSongSelect(); break;
        case ScreenState::DECIDE:          RenderDecide(); break;
        default: break;
    }
    if (screen_ == ScreenState::GAMEPLAY)    RenderGameplay();
    if (screen_ == ScreenState::RESULTS)     RenderResults();
    if (screen_ == ScreenState::OPTIONS)     RenderOptions();
    if (screen_ == ScreenState::CALIBRATION) RenderCalibration();

    // Polish transitions

    // --- Player-specific Mod Menus (Topmost global layer) ---
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) RenderModifierMenu(p);
    }

    // --- Screen Transition Overlay ---
    if (is_transitioning_) {
        double duration = 0.8;
        double half_duration = duration / 2.0;
        double alpha_factor = 0.0;

        if (screen_transition_timer_ < half_duration) {
            alpha_factor = screen_transition_timer_ / half_duration;
        } else {
            alpha_factor = 1.0 - ((screen_transition_timer_ - half_duration) / half_duration);
        }

        int alpha = static_cast<int>(255 * std::min(1.0, alpha_factor));
        DrawRect(0, 0, width_, height_, {0, 0, 0, static_cast<uint8_t>(alpha)});
    }

    SDL_RenderPresent(renderer_);
}

// ============================================================================
// Attraction Screen & Profile Loading
// ============================================================================

void GameWindow::HandleKeyDown_Attraction(SDL_Keycode key) {
    // Escape quits from attraction screen
    if (key == SDLK_ESCAPE) {
        running_ = false;
        return;
    }

    // P1 Start
    if (players_[0].input.OnKeyDown(key) == InputMapper::COL_START) {
        JoinPlayer(0);
        ChangeScreen(ScreenState::PROFILE_LOAD);
        return;
    }

    // P2 Start
    if (players_[1].input.OnKeyDown(key) == InputMapper::COL_START) {
        JoinPlayer(1);
        ChangeScreen(ScreenState::PROFILE_LOAD);
        return;
    }
}

void GameWindow::HandleKeyDown_ProfileLoad(SDL_Keycode key) {
    if (key == SDLK_ESCAPE) {
        // Go back to attraction screen, unjoin all players
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            UnjoinPlayer(i);
        }
        ChangeScreen(ScreenState::ATTRACTION);
        return;
    }

    // P1 Start - join P1 if not joined, or confirm if joined
    if (players_[0].input.OnKeyDown(key) == InputMapper::COL_START) {
        if (!players_[0].joined) {
            JoinPlayer(0);
        } else {
            profile_load_state_[0].ready = true;
        }
    }

    // P2 Start - join P2 if not joined, or confirm if joined
    if (players_[1].input.OnKeyDown(key) == InputMapper::COL_START) {
        if (!players_[1].joined) {
            JoinPlayer(1);
        } else {
            profile_load_state_[1].ready = true;
        }
    }

    // L/R to manage profiles for P1/P2
    if (key == SDLK_LEFT) {
        profile_manage_player_ = 0;
        profile_manage_cursor_ = 0;
        profile_manage_creating_ = false;
        ScanLocalProfiles();
        ChangeScreen(ScreenState::PROFILE_MANAGE);
        return;
    }
    if (key == SDLK_RIGHT) {
        profile_manage_player_ = 1;
        profile_manage_cursor_ = 0;
        profile_manage_creating_ = false;
        ScanLocalProfiles();
        ChangeScreen(ScreenState::PROFILE_MANAGE);
        return;
    }

    // Check if all joined players are ready -> transition to Song Select
    bool any_joined = false;
    bool all_ready = true;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players_[i].joined) {
            any_joined = true;
            if (!profile_load_state_[i].ready) {
                all_ready = false;
            }
        }
    }
    if (any_joined && all_ready) {
        SetupPlayerFieldLayout();
        ChangeScreen(ScreenState::SONG_SELECT);
    }
}

void GameWindow::UpdateAttraction(double dt) {
    attraction_timer_ += dt;

    // Periodic USB polling
    usb_poll_timer_ += dt;
    if (usb_poll_timer_ >= USB_POLL_INTERVAL) {
        usb_poll_timer_ = 0.0;
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (!players_[i].joined && Profile::CheckForProfile(drive_map_[i])) {
                // Auto-join this player from USB
                JoinPlayer(i);
                players_[i].profile.is_usb = true;
                players_[i].profile.Load(Profile::GetUSBProfilePath(drive_map_[i]));
                ChangeScreen(ScreenState::PROFILE_LOAD);
                return;
            }
        }
    }
}

void GameWindow::UpdateProfileLoad(double dt) {
    attraction_timer_ += dt;

    // Periodic USB polling for unjoint players
    usb_poll_timer_ += dt;
    if (usb_poll_timer_ >= USB_POLL_INTERVAL) {
        usb_poll_timer_ = 0.0;
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (!players_[i].joined && Profile::CheckForProfile(drive_map_[i])) {
                JoinPlayer(i);
                players_[i].profile.is_usb = true;
                players_[i].profile.Load(Profile::GetUSBProfilePath(drive_map_[i]));
                profile_load_state_[i].ready = true;
            }
        }
    }
}

void GameWindow::RenderAttraction() {
    // 1. Sliding Background / Logo
    double entrance = std::min(1.0, global_anim_timer_ / 1.5);
    double slide = 1.0 - std::pow(1.0 - entrance, 3.0);
    int logo_y = static_cast<int>(height_ / 2 - 100 + (1.0 - slide) * 50);

    // Vignette background
    DrawRect(0, 0, width_, height_, {10, 10, 20, 255});
    
    // Pulsing "Press Start"
    double pulse = 0.5 + 0.5 * std::sin(global_anim_timer_ * 4.0);
    uint8_t alpha = static_cast<uint8_t>(150 + 105 * pulse);
    
    font_.DrawText(renderer_, width_ / 2, logo_y, "SM LEGENDS", {255, 255, 255, static_cast<uint8_t>(255 * entrance)}, FontSize::TITLE, TextAlign::CENTER);
    font_.DrawText(renderer_, width_ / 2, height_ / 2 + 100, "PRESS START / [ENTER] TO PLAY", {200, 200, 255, alpha}, FontSize::MEDIUM, TextAlign::CENTER);
    
    // Bottom info
    font_.DrawText(renderer_, width_ / 2, height_ - 40, "© 2026 ANTIGRAVITY - ADVANCED AGENTIC CODING", {100, 100, 120, 255}, FontSize::SMALL, TextAlign::CENTER);
}

void GameWindow::RenderProfileLoad() {
    // Dark background
    DrawRect(0, 0, width_, height_, {10, 10, 25, 255});

    int center_x = width_ / 2;

    // Header
    font_.DrawText(renderer_, center_x, 30, "PLAYER SELECT",
        {255, 255, 255, 255}, FontSize::TITLE, TextAlign::CENTER);

    // Split screen: P1 on left half, P2 on right half
    int half_w = width_ / 2;
    int panel_margin = 30;
    int panel_w = half_w - panel_margin * 2;
    int panel_y = 100;
    int panel_h = height_ - 150;

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        int panel_x = (i == 0) ? panel_margin : half_w + panel_margin;
        
        // Panel background
        Color panel_bg = players_[i].joined 
            ? Color{30, 40, 60, 220} 
            : Color{20, 20, 30, 200};
        DrawRect(panel_x, panel_y, panel_w, panel_h, panel_bg);
        
        // Panel border
        Color border_color = players_[i].joined 
            ? Color{80, 120, 255, 200} 
            : Color{60, 60, 80, 150};
        DrawRectOutline(panel_x, panel_y, panel_w, panel_h, border_color);

        int px = panel_x + panel_w / 2;
        int py = panel_y + 30;

        // Player label
        const char* label = (i == 0) ? "PLAYER 1" : "PLAYER 2";
        Color label_color = players_[i].joined 
            ? Color{100, 180, 255, 255} 
            : Color{100, 100, 120, 200};
        font_.DrawText(renderer_, px, py, label, label_color, FontSize::LARGE, TextAlign::CENTER);
        py += 60;

        if (players_[i].joined) {
            // Show profile name
            font_.DrawText(renderer_, px, py, players_[i].profile.name,
                {255, 255, 255, 255}, FontSize::LARGE, TextAlign::CENTER);
            py += 45;

            if (players_[i].profile.is_usb) {
                font_.DrawText(renderer_, px, py, "[USB]",
                    {100, 255, 100, 200}, FontSize::SMALL, TextAlign::CENTER);
                py += 30;
            }

            py += 30;
            if (profile_load_state_[i].ready) {
                font_.DrawText(renderer_, px, py, "READY!",
                    {100, 255, 100, 255}, FontSize::LARGE, TextAlign::CENTER);
            } else {
                double blink = 0.5 + 0.5 * std::sin(attraction_timer_ * 3.0);
                uint8_t a = static_cast<uint8_t>(150 + 105 * blink);
                font_.DrawText(renderer_, px, py, "Press Start to confirm",
                    {255, 220, 100, a}, FontSize::MEDIUM, TextAlign::CENTER);
            }
        } else {
            // Not joined 窶・prompt
            int cy = panel_y + panel_h / 2 - 20;
            double blink = 0.5 + 0.5 * std::sin(attraction_timer_ * 2.0);
            uint8_t a = static_cast<uint8_t>(120 + 135 * blink);
            
            font_.DrawText(renderer_, px, cy, "Press Start",
                {255, 220, 100, a}, FontSize::LARGE, TextAlign::CENTER);
            font_.DrawText(renderer_, px, cy + 40, "or insert USB",
                {120, 120, 140, 180}, FontSize::SMALL, TextAlign::CENTER);
        }
    }

    // Divider line
    DrawRect(center_x - 1, panel_y, 2, panel_h, {60, 60, 80, 150});

    // Profile manage hint
    font_.DrawText(renderer_, center_x, panel_y + panel_h + 20,
        "LEFT / RIGHT to manage profiles", {120, 120, 150, 180}, FontSize::SMALL, TextAlign::CENTER);
}

void GameWindow::JoinPlayer(int player_idx) {
    if (player_idx < 0 || player_idx >= MAX_PLAYERS) return;
    if (players_[player_idx].joined) return;

    players_[player_idx].joined = true;
    players_[player_idx].profile.Reset();
    ApplyProfileMods(player_idx); // Apply default mods to the player state
    profile_load_state_[player_idx] = ProfileLoadState{};

    num_active_players_ = 0;
    active_player_idx_ = -1;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players_[i].joined) {
            num_active_players_++;
            if (active_player_idx_ == -1) active_player_idx_ = i;
        }
    }
    if (active_player_idx_ == -1) active_player_idx_ = 0;

    std::printf("Player %d joined! (%d active)\n", player_idx + 1, num_active_players_);
}

void GameWindow::UnjoinPlayer(int player_idx) {
    if (player_idx < 0 || player_idx >= MAX_PLAYERS) return;
    
    // Save before clearing
    if (players_[player_idx].joined) {
        // Redundant RecalculateRating removed here; SaveActiveProfile is enough 
        // as ratings are already calculated at chart end.
        SaveActiveProfile(player_idx);
    }

    players_[player_idx].joined = false;
    players_[player_idx].profile.Reset();
    profile_load_state_[player_idx] = ProfileLoadState{};

    num_active_players_ = 0;
    active_player_idx_ = -1;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players_[i].joined) {
            num_active_players_++;
            if (active_player_idx_ == -1) active_player_idx_ = i;
        }
    }
    if (active_player_idx_ == -1) active_player_idx_ = 0;
}

void GameWindow::SetupPlayerFieldLayout() {
    // 4-lane single mode positioning logic
    int num_cols = current_chart_ ? current_chart_->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;
    int lane_w = GetLaneWidth();
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;

    // Natural center of field (at offset=0) 
    int natural_center = width_ / 2;
    // P1 target center: left quarter  
    int p1_center_side = width_ / 4;
    // P2 target center: right quarter
    int p2_center_side = 3 * width_ / 4;

    if (num_active_players_ <= 1) {
        // Single player: align based on side choice + centering option
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            players_[i].field_config = field_config_;
            if (players_[i].joined) {
                if (center_1p_ || num_cols > 4) {
                    // Center it for 1P centered mode or for 8-lane doubles/couple/routine
                    players_[i].field_x_offset = 0;
                } else {
                    // Side-align by default for 4-lane single
                    int target_center = (i == 0) ? p1_center_side : p2_center_side;
                    players_[i].field_x_offset = target_center - natural_center;
                }
            } else {
                players_[i].field_x_offset = 0;
            }
        }
    } else {
        // Two players
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            players_[i].field_config = field_config_;
            if (num_cols > 4) {
                // For 8-lane modes (Couple/Routine), both players share/center the field
                players_[i].field_x_offset = 0;
            } else {
                // Two players on 4-lane: center each player's notefield in their half
                int target_center = (i == 0) ? p1_center_side : p2_center_side;
                players_[i].field_x_offset = target_center - natural_center;
            }
        }
    }
}

// ============================================================================
// Profile Management
// ============================================================================

void GameWindow::ScanLocalProfiles() {
    local_profile_names_ = Profile::ScanLocalProfiles("profiles");
}

void GameWindow::SaveActiveProfile(int p) {
    if (p < 0 || p >= MAX_PLAYERS) return;
    auto& ps = players_[p];
    auto& prof = ps.profile;
    if (prof.is_guest || prof.source_path.empty()) return;

    // Sync current per-player mods into profile before saving
    prof.speed_mod    = ps.field_config.speed_mod;
    prof.mod_type     = static_cast<int>(ps.field_config.mod_type);
    prof.downscroll   = ps.field_config.downscroll;
    
    // Global sync for legacy (if player is primary)
    if (p == active_player_idx_) {
        prof.audio_offset = audio_offset_;
    }

    prof.noteskin     = (ps.noteskin_index < static_cast<int>(available_noteskins_.size()))
                          ? available_noteskins_[ps.noteskin_index] : "Default";
    prof.effect_mode  = ps.effect_mode;
    
    // New fields
    prof.sudden_plus  = ps.sudden_plus_val;
    prof.hidden_plus  = ps.hidden_plus_val;
    prof.combo_display_mode = static_cast<int>(ps.combo_display_mode);
    prof.bga_brightness = static_cast<int>(ps.bga_brightness);
    prof.ex_mode      = ps.ex_mode;
    
    // Sync life mode/flare
    prof.life_type    = static_cast<int>(ps.life_meter.GetType());
    prof.flare_level  = ps.life_meter.GetFlareLevel();

    prof.Save(prof.source_path);
}

void GameWindow::ApplyProfileMods(int p) {
    if (p < 0 || p >= MAX_PLAYERS) return;
    auto& ps = players_[p];
    auto& prof = ps.profile;
    
    // 1. Restore per-player field config
    ps.field_config.speed_mod = prof.speed_mod;
    ps.field_config.mod_type  = static_cast<ScrollModType>(prof.mod_type);
    ps.field_config.downscroll = prof.downscroll;
    ps.field_config.receptor_y = ps.field_config.downscroll ? height_ * 0.85 : height_ * 0.15;

    // 2. Restore other modifiers
    ps.sudden_plus_val = prof.sudden_plus;
    ps.hidden_plus_val = prof.hidden_plus;
    ps.effect_mode     = prof.effect_mode;
    ps.ex_mode         = prof.ex_mode;
    ps.combo_display_mode = static_cast<ComboDisplayMode>(prof.combo_display_mode);
    ps.bga_brightness  = static_cast<BGABrightness>(prof.bga_brightness);
    
    // Life Mode
    ps.life_meter.Init(static_cast<LifeType>(prof.life_type), prof.flare_level);

    // Noteskin (scan for index)
    if (!prof.noteskin.empty()) {
        for (int i = 0; i < (int)available_noteskins_.size(); ++i) {
            if (available_noteskins_[i] == prof.noteskin) {
                ps.noteskin_index = i;
                break;
            }
        }
    }

    // 3. Sync global variables ONLY if this is the active player (for menu display/legacy)
    if (p == active_player_idx_) {
        field_config_ = ps.field_config;
        audio_offset_ = prof.audio_offset;
        effect_mode_  = prof.effect_mode;
        ex_mode_      = prof.ex_mode;
        if (!prof.noteskin.empty()) {
            LoadNoteskin(prof.noteskin);
        }
    }
}

void GameWindow::HandleKeyDown_ProfileManage(SDL_Keycode key) {
    if (profile_manage_creating_) {
        // --- Text input mode for new profile name ---
        if (key == SDLK_ESCAPE) {
            profile_manage_creating_ = false;
            profile_manage_new_name_.clear();
            return;
        }
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            if (!profile_manage_new_name_.empty()) {
                Profile::CreateLocalProfile(profile_manage_new_name_);
                ScanLocalProfiles();
                // Select the newly created profile
                for (int i = 0; i < static_cast<int>(local_profile_names_.size()); ++i) {
                    if (local_profile_names_[i] == profile_manage_new_name_) {
                        profile_manage_cursor_ = i;
                        break;
                    }
                }
                profile_manage_new_name_.clear();
            }
            profile_manage_creating_ = false;
            return;
        }
        if (key == SDLK_BACKSPACE) {
            if (!profile_manage_new_name_.empty()) {
                profile_manage_new_name_.pop_back();
            }
            return;
        }
        // Accept printable ASCII characters
        if (key >= 32 && key < 127 && profile_manage_new_name_.size() < 20) {
            // Apply shift for uppercase
            char ch = static_cast<char>(key);
            const uint8_t* keys = SDL_GetKeyboardState(nullptr);
            bool shift = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
            if (shift && ch >= 'a' && ch <= 'z') ch -= 32;
            profile_manage_new_name_ += ch;
        }
        return;
    }

    // --- Normal navigation mode ---
    int total_items = static_cast<int>(local_profile_names_.size()) + 1; // +1 for "Create New"

    if (key == SDLK_UP) {
        profile_manage_cursor_ = (profile_manage_cursor_ - 1 + total_items) % total_items;
    } else if (key == SDLK_DOWN) {
        profile_manage_cursor_ = (profile_manage_cursor_ + 1) % total_items;
    } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
        if (profile_manage_cursor_ == static_cast<int>(local_profile_names_.size())) {
            // "Create New" selected
            profile_manage_creating_ = true;
            profile_manage_new_name_.clear();
        } else {
            // Select existing profile 窶・load it
            int idx = profile_manage_cursor_;
            std::string dir = Profile::GetLocalProfilePath(local_profile_names_[idx]);
            auto& prof = players_[profile_manage_player_].profile;
            prof.Reset();
            prof.Load(dir);
            prof.is_usb = false;
            ApplyProfileMods(profile_manage_player_);

            // Mark as ready and return to profile load
            profile_load_state_[profile_manage_player_].ready = false;
            ChangeScreen(ScreenState::PROFILE_LOAD);
        }
    } else if (key == SDLK_DELETE) {
        // Delete selected profile (except "Create New")
        if (profile_manage_cursor_ < static_cast<int>(local_profile_names_.size())) {
            std::string dir = Profile::GetLocalProfilePath(local_profile_names_[profile_manage_cursor_]);
            std::filesystem::remove_all(dir);
            ScanLocalProfiles();
            if (profile_manage_cursor_ >= static_cast<int>(local_profile_names_.size())) {
                profile_manage_cursor_ = std::max(0, static_cast<int>(local_profile_names_.size()) - 1);
            }
        }
    } else if (key == SDLK_ESCAPE) {
        ChangeScreen(ScreenState::PROFILE_LOAD);
    }
}

void GameWindow::RenderProfileManage() {
    // Dark gradient background
    for (int y = 0; y < height_; ++y) {
        uint8_t r = static_cast<uint8_t>(15 + (y * 8 / height_));
        uint8_t g = static_cast<uint8_t>(15 + (y * 8 / height_));
        uint8_t b = static_cast<uint8_t>(35 + (y * 15 / height_));
        SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
        SDL_RenderDrawLine(renderer_, 0, y, width_, y);
    }

    int cx = width_ / 2;

    // Title
    font_.DrawText(renderer_, cx, 40, "PROFILE MANAGEMENT", {255, 215, 0, 255}, FontSize::LARGE, TextAlign::CENTER);
    std::string subtitle = "Player " + std::to_string(profile_manage_player_ + 1);
    font_.DrawText(renderer_, cx, 80, subtitle, {180, 180, 200, 200}, FontSize::MEDIUM, TextAlign::CENTER);

    // Panel
    int panel_w = 400;
    int panel_h = 380;
    int panel_x = cx - panel_w / 2;
    int panel_y = 120;
    DrawRect(panel_x, panel_y, panel_w, panel_h, {0, 0, 0, 150});
    DrawRectOutline(panel_x, panel_y, panel_w, panel_h, {80, 80, 120, 200});

    // Profile list
    int item_y = panel_y + 15;
    int item_h = 35;
    int total_items = static_cast<int>(local_profile_names_.size()) + 1;

    for (int i = 0; i < total_items; ++i) {
        bool selected = (i == profile_manage_cursor_);
        if (selected) {
            // Highlight bar
            DrawRect(panel_x + 5, item_y - 2, panel_w - 10, item_h, {60, 60, 120, 180});
        }

        Color text_col = selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 180};
        
        if (i < static_cast<int>(local_profile_names_.size())) {
            // Existing profile
            const auto& pname = local_profile_names_[i];
            
            // Load the profile briefly to get rating
            Profile temp;
            std::string dir = Profile::GetLocalProfilePath(pname);
            temp.Load(dir);
            
            font_.DrawText(renderer_, panel_x + 20, item_y + 5, pname, text_col, FontSize::MEDIUM);
            
            // Show rating (Single)
            char rate_buf[32];
            std::snprintf(rate_buf, sizeof(rate_buf), "%.2f", temp.rating_single);
            Color rate_col = selected ? Color{80, 255, 255, 255} : Color{100, 200, 200, 180};
            font_.DrawText(renderer_, panel_x + panel_w - 20, item_y + 5, rate_buf, rate_col, FontSize::MEDIUM, TextAlign::RIGHT);
        } else {
            // "Create New" option
            Color create_col = selected ? Color{100, 255, 100, 255} : Color{100, 200, 100, 180};
            font_.DrawText(renderer_, panel_x + 20, item_y + 5, "+ CREATE NEW PROFILE", create_col, FontSize::MEDIUM);
        }

        item_y += item_h;
        if (item_y > panel_y + panel_h - item_h) break;
    }

    // Name entry overlay
    if (profile_manage_creating_) {
        int overlay_w = 350;
        int overlay_h = 120;
        int ox = cx - overlay_w / 2;
        int oy = height_ / 2 - overlay_h / 2;
        DrawRect(ox, oy, overlay_w, overlay_h, {20, 20, 40, 240});
        DrawRectOutline(ox, oy, overlay_w, overlay_h, {100, 100, 200, 255});

        font_.DrawText(renderer_, cx, oy + 15, "ENTER PROFILE NAME", {255, 215, 0, 255}, FontSize::MEDIUM, TextAlign::CENTER);

        // Name input field
        std::string display_name = profile_manage_new_name_;
        // Blinking cursor
        double blink = std::fmod(attraction_timer_ * 2.0, 2.0);
        if (blink < 1.0) display_name += "_";

        DrawRect(ox + 20, oy + 50, overlay_w - 40, 30, {0, 0, 0, 200});
        DrawRectOutline(ox + 20, oy + 50, overlay_w - 40, 30, {80, 80, 150, 200});
        font_.DrawText(renderer_, ox + 30, oy + 55, display_name, {255, 255, 255, 255}, FontSize::MEDIUM);

        font_.DrawText(renderer_, cx, oy + 95, "ENTER to confirm   ESC to cancel", {140, 140, 160, 200}, FontSize::SMALL, TextAlign::CENTER);
    }

    // Footer
    int fy = height_ - 40;
    DrawRect(0, fy, width_, 40, {15, 15, 30, 220});
    DrawRect(0, fy, width_, 2, {60, 60, 110, 200});

    Color white = {240, 240, 255, 255};
    Color dim = {140, 140, 160, 200};
    int fx = 20;
    font_.DrawText(renderer_, fx, fy + 12, "UP/DOWN", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 65, fy + 12, "NAV", dim, FontSize::SMALL);
    fx += 110;
    font_.DrawText(renderer_, fx, fy + 12, "ENTER", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 45, fy + 12, "SELECT", dim, FontSize::SMALL);
    fx += 110;
    font_.DrawText(renderer_, fx, fy + 12, "DEL", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 30, fy + 12, "DELETE", dim, FontSize::SMALL);
    fx += 100;
    font_.DrawText(renderer_, fx, fy + 12, "ESC", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 30, fy + 12, "BACK", dim, FontSize::SMALL);
}


// ============================================================================
// Song Select Screen
// ============================================================================

    
void GameWindow::RenderOptions() {
    RenderBackground();
    DrawRect(0, 0, width_, height_, {10, 10, 20, 220});

    std::string title = "KEY CONFIGURATION";
    font_.DrawText(renderer_, width_ / 2, 40, title, {255, 255, 255, 255}, FontSize::TITLE, TextAlign::CENTER);

    auto GetKeyString = [](const BindInfo& b) -> std::string {
        if (b.type == BindInfo::NONE) return "---";
        if (b.type == BindInfo::KEY) return SDL_GetKeyName((SDL_Keycode)b.id);
        
        switch ((SDL_GameControllerButton)b.id) {
            case SDL_CONTROLLER_BUTTON_A: return "PAD A";
            case SDL_CONTROLLER_BUTTON_B: return "PAD B";
            case SDL_CONTROLLER_BUTTON_X: return "PAD X";
            case SDL_CONTROLLER_BUTTON_Y: return "PAD Y";
            case SDL_CONTROLLER_BUTTON_DPAD_UP:    return "DPAD U";
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  return "DPAD D";
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  return "DPAD L";
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return "DPAD R";
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return "LB";
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "RB";
            case SDL_CONTROLLER_BUTTON_START:  return "START";
            case SDL_CONTROLLER_BUTTON_BACK:   return "BACK";
            case SDL_CONTROLLER_BUTTON_GUIDE:  return "GUIDE";
            default: return "BTN " + std::to_string(b.id);
        }
    };

    const char* lane_names[] = {"LEFT", "DOWN", "UP", "RIGHT", "START", "SELECT"};
    Color header_col = {150, 150, 170, 255};
    int row_h = 50;

    for (int p = 0; p < 2; ++p) {
        int table_x = (p == 0) ? 20 : width_ / 2 + 10;
        int table_w = width_ / 2 - 30;
        int col_w = table_w / 4;

        font_.DrawText(renderer_, table_x + table_w / 2, 110, p == 0 ? "PLAYER 1" : "PLAYER 2", {255, 255, 100, 255}, FontSize::MEDIUM, TextAlign::CENTER);
        
        // Headers
        font_.DrawText(renderer_, table_x, 140, "ACTION", header_col, FontSize::SMALL, TextAlign::LEFT);
        font_.DrawText(renderer_, table_x + col_w, 140, "SLOT 1", header_col, FontSize::SMALL, TextAlign::CENTER);
        font_.DrawText(renderer_, table_x + col_w * 2, 140, "SLOT 2", header_col, FontSize::SMALL, TextAlign::CENTER);
        font_.DrawText(renderer_, table_x + col_w * 3, 140, "SLOT 3", header_col, FontSize::SMALL, TextAlign::CENTER);

        for (int l = 0; l < 6; ++l) {
            int y = 165 + l * row_h;
            
            font_.DrawText(renderer_, table_x, y + 10, lane_names[l], {255, 255, 255, 255}, FontSize::SMALL, TextAlign::LEFT);

            for (int s = 0; s < 3; ++s) {
                int x = table_x + col_w * (s + 1);
                bool is_current = (options_player_cursor_ == p && options_lane_cursor_ == l && options_slot_cursor_ == s);
                
                Color bg_col = is_current ? Color{40, 40, 80, 255} : Color{20, 20, 30, 255};
                DrawRect(x - col_w/2 + 2, y, col_w - 4, row_h - 4, bg_col);
                
                if (is_current) {
                    uint8_t pulse = static_cast<uint8_t>(200 + 55 * std::sin(SDL_GetTicks() * 0.01));
                    DrawRectOutline(x - col_w/2 + 2, y, col_w - 4, row_h - 4, {pulse, pulse, 100, 255});
                }

                const auto& b = custom_binds_[p][l][s];
                std::string val = is_current && is_rebinding_ ? "[ ? ]" : GetKeyString(b);
                Color val_col = (b.type == BindInfo::BUTTON) ? Color{100, 200, 255, 255} : Color{255, 255, 100, 255};
                if (b.type == BindInfo::NONE) val_col = {80, 80, 90, 255};

                font_.DrawText(renderer_, x, y + 10, val, val_col, FontSize::SMALL, TextAlign::CENTER);
            }
        }
    }

    font_.DrawText(renderer_, width_ / 2, height_ - 60, "ARROWS: NAVIGATE | ENTER: REBIND | DEL: CLEAR | ESC/F12: SAVE & EXIT", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
}

void GameWindow::RenderCalibration() {
    RenderBackground();
    DrawRect(0, 0, width_, height_, {10, 10, 20, 200});

    int cx = width_ / 2;
    font_.DrawText(renderer_, cx, 50, "AUDIO CALIBRATION", {255, 255, 255, 255}, FontSize::TITLE, TextAlign::CENTER);

    int start_y = 200;
    int spacing = 60;

    // Menu Items:
    // 0: Audio Offset
    // 1: Launch Calibration Chart

    // Item 0: Audio Offset
    {
        bool selected = (calibration_cursor_ == 0);
        Color text_col = selected ? Color{255, 255, 100, 255} : Color{200, 200, 220, 255};
        font_.DrawText(renderer_, cx - 150, start_y, "Global Offset:", text_col, FontSize::MEDIUM, TextAlign::LEFT);
        
        char offset_str[32];
        std::snprintf(offset_str, sizeof(offset_str), "%+d ms", (int)(audio_offset_ * 1000));
        font_.DrawText(renderer_, cx + 50, start_y, offset_str, text_col, FontSize::MEDIUM, TextAlign::LEFT);

        if (suggested_offset_ != 0.0) {
            char sug_str[64];
            std::snprintf(sug_str, sizeof(sug_str), "[Suggested: %+d ms]", (int)(suggested_offset_ * 1000));
            font_.DrawText(renderer_, cx + 50, start_y + 25, sug_str, {100, 255, 100, 255}, FontSize::SMALL, TextAlign::LEFT);
        }
    }

    // Item 1: Launch Chart
    {
        bool selected = (calibration_cursor_ == 1);
        Color text_col = selected ? Color{255, 255, 100, 255} : Color{200, 200, 220, 255};
        font_.DrawText(renderer_, cx, start_y + spacing, "Launch Calibration Chart (8ths)", text_col, FontSize::MEDIUM, TextAlign::CENTER);
    }

    // Instructions
    int hint_y = height_ - 150;
    font_.DrawText(renderer_, cx, hint_y, "Adjust global delay between audio and gameplay.", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, cx, hint_y + 25, "Positive (+) if audio is LATE. Negative (-) if audio is EARLY.", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, cx, height_ - 50, "ARROWS: ADJUST | ENTER: SELECT | ESC/F11: SAVE & EXIT", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);

    // Calibration Overlay
    if (is_calibrating_ && suggested_offset_ != 0.0) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "SUGGESTED OFFSET: %+d ms (SD: %.1f ms)", 
            (int)(suggested_offset_ * 1000), calibration_stdev_ * 1000);
        
        // Render centered overlay
        int cx = width_ / 2;
        font_.DrawText(renderer_, cx, height_ / 2 + 100, buf, {100, 255, 100, 255}, FontSize::MEDIUM, TextAlign::CENTER);
        font_.DrawText(renderer_, cx, height_ / 2 + 130, "PRESS [F11] TO FINISH & SAVE", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
    }
}

void GameWindow::RenderSongSelect() {
    // Dark-Navy Gradient Background (matching gameplay but brighter)
    for (int y = 0; y < height_; ++y) {
        uint8_t r = static_cast<uint8_t>(20 + (y * 10 / height_));
        uint8_t g = static_cast<uint8_t>(20 + (y * 10 / height_));
        uint8_t b = static_cast<uint8_t>(40 + (y * 20 / height_));
        SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
        SDL_RenderDrawLine(renderer_, 0, y, width_, y);
    }

    RenderSongList();

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) {
            RenderChartPanel(p);
        }
    }
    RenderSongSelectHUD(); // This now draws Top and Bottom bars
}

void GameWindow::RenderSongList() {
    const auto& songs = scanner_.GetSongs();
    int count = static_cast<int>(songs.size());

    if (count == 0) {
        int w1 = GetTextWidth("no songs found", 3);
        DrawText(width_ / 2 - w1 / 2, height_ / 2 - 20, "no songs found", {200, 100, 100, 255}, 3);
        int w2 = GetTextWidth("press t for test chart", 2);
        DrawText(width_ / 2 - w2 / 2, height_ / 2 + 20, "press t for test chart", {140, 140, 160, 200}, 2);
        return;
    }

    // --- Centered 3x3 Grid Music Wheel (Infinite Looping) ---
    // --- Centered 5x3 Grid Music Wheel (Infinite Looping) ---
    int jacket_size = 140; // Smaller to fit 5 rows
    int spacing = 15;
    int cols = 3;
    int rows_to_show = 5;
    
    int grid_w = cols * jacket_size + (cols - 1) * spacing;
    int grid_h = rows_to_show * jacket_size + (rows_to_show - 1) * spacing;
    
    int grid_left = (width_ - grid_w) / 2;
    int grid_top = (height_ - grid_h) / 2 + 30; // Centered vertically, offset for Top Bar

    int sel_row = selected_song_ / cols;
    int total_rows = (count + cols - 1) / cols;

    // We draw rows from -2 to +2 relative to the selected row
    for (int r = -2; r <= 2; ++r) {
        int actual_row = (sel_row + r + total_rows);
        while (actual_row < 0) actual_row += total_rows;
        actual_row %= total_rows;

        for (int c = 0; c < cols; ++c) {
            int song_idx = actual_row * cols + c;
            
            // Handle last row being partially empty
            if (song_idx >= count) {
                // To keep the grid looking "full" and infinite, we could wrap to S0
                // but for now let's just show an empty slot or skip
                continue;
            }

            const auto& song = songs[static_cast<size_t>(song_idx)];
            
            int x = grid_left + c * (jacket_size + spacing);
            int y = grid_top + (r + 2) * (jacket_size + spacing); // r+2 maps -2..2 to 0..4
            
            // Apply bounce animation to the selected item
            bool is_selected = (song_idx == selected_song_);
            double scale = 1.0;
            if (is_selected) {
                scale = 1.0 + 0.05 * std::sin(global_anim_timer_ * 5.0);
            }
            
            // Entrance slide
            double entrance = std::min(1.0, (global_anim_timer_ - (r+2)*0.1) / 0.5);
            if (entrance < 0) entrance = 0;
            int x_anim = static_cast<int>(x + (1.0 - std::pow(1.0 - entrance, 3.0)) * 50 - 50);

            // Jacket background
            int cur_size = static_cast<int>(jacket_size * scale);
            int cur_x = x_anim - (cur_size - jacket_size) / 2;
            int cur_y = y - (cur_size - jacket_size) / 2;

            DrawRect(cur_x, cur_y, cur_size, cur_size, {20, 20, 40, static_cast<uint8_t>(255 * entrance)});

            // Load and Draw Jacket
            SDL_Texture* jacket = GetJacketTexture(song.jacket_path);
            if (jacket) {
                int jw, jh;
                SDL_QueryTexture(jacket, nullptr, nullptr, &jw, &jh);
                double aspect = static_cast<double>(jw) / jh;
                SDL_Rect dest;
                if (aspect > 1.0) {
                    int h = static_cast<int>(cur_size / aspect);
                    dest = { cur_x, cur_y + (cur_size - h) / 2, cur_size, h };
                } else {
                    int w = static_cast<int>(cur_size * aspect);
                    dest = { cur_x + (cur_size - w) / 2, cur_y, w, cur_size };
                }
                SDL_SetTextureAlphaMod(jacket, static_cast<uint8_t>(255 * entrance));
                SDL_RenderCopy(renderer_, jacket, nullptr, &dest);
                SDL_SetTextureAlphaMod(jacket, 255);
            } else {
                int cx = cur_x + cur_size / 2;
                int cy = cur_y + cur_size / 2;
                DrawRect(cx - 5, cy - 15, 8, 30, {60, 60, 80, static_cast<uint8_t>(255 * entrance)});
                DrawRect(cx - 15, cy + 8, 18, 12, {60, 60, 80, static_cast<uint8_t>(255 * entrance)});
                font_.DrawText(renderer_, cx, cy + 30, "NO IMAGE", {50, 50, 70, static_cast<uint8_t>(255 * entrance)}, FontSize::SMALL, TextAlign::CENTER);
            }

            // Selection Border & Glow (Only if selected)
            if (is_selected) {
                uint8_t glow = static_cast<uint8_t>(180 + 75 * std::sin(SDL_GetTicks() * 0.01));
                DrawRectOutline(x - 4, y - 4, jacket_size + 8, jacket_size + 8, {255, 255, 255, glow});
                DrawRectOutline(x - 8, y - 8, jacket_size + 16, jacket_size + 16, {0, 180, 255, static_cast<uint8_t>(glow/3)});
                
                // Overlay Title/Artist for selected
                DrawRect(x, y + jacket_size - 35, jacket_size, 35, {10, 10, 20, 200});
                std::string title = song.title;
                if (title.length() > 16) title = title.substr(0, 14) + "..";
                font_.DrawText(renderer_, x + 5, y + jacket_size - 30, title, {255, 255, 255, 255}, FontSize::SMALL, TextAlign::LEFT, 1.0, "score");
                std::string artist = song.artist;
                if (artist.length() > 20) artist = artist.substr(0, 18) + "..";
                font_.DrawText(renderer_, x + 5, y + jacket_size - 15, artist, {180, 180, 200, 255}, FontSize::SMALL);
            } else {
                // Dim non-selected rows or columns
                DrawRect(x, y, jacket_size, jacket_size, {0, 0, 0, 120});
                DrawRectOutline(x, y, jacket_size, jacket_size, {40, 40, 60, 255});
            }
        }
    }
}

void GameWindow::RenderChartPanel(int p) {
    const auto& songs = scanner_.GetSongs();
    if (songs.empty()) return;

    const auto& song = songs[static_cast<size_t>(selected_song_)];

    // Position based on player
    int panel_width = 240;
    int panel_left = (p == 0) ? 15 : width_ - panel_width - 15;
    int panel_top = 100;
    TextAlign align = (p == 0) ? TextAlign::LEFT : TextAlign::RIGHT;

    // 1. Difficulty List
    int y = panel_top;
    for (size_t ci = 0; ci < song.charts.size(); ++ci) {
        const auto& chart = song.charts[ci];
        
        // Skip 8-lane charts in 2P
        if (num_active_players_ >= 2 && chart.Is8Lane()) continue;

        bool selected = (static_cast<int>(ci) == selected_chart_[p]);
        Color diff_col = GetDifficultyColor(chart.difficulty_name);
        
        if (chart.variant == ChartVariant::Wild) {
            diff_col = Color::Rainbow(SDL_GetTicks() / 1000.0, 0.7f, 0.6f);
        }

        int row_h = 32;
        if (selected) {
            DrawRect(panel_left - 10, y, panel_width + 10, row_h, {diff_col.r, diff_col.g, diff_col.b, 100});
            DrawRect((p == 0) ? (panel_left - 10) : (panel_left + panel_width + 5), y, 5, row_h, diff_col);
            uint8_t flash = static_cast<uint8_t>(180 + 75 * std::sin(SDL_GetTicks() * 0.01));
            DrawRect(panel_left - 10, y, panel_width + 10, 1, {255, 255, 255, flash});
            DrawRect(panel_left - 10, y + row_h - 1, panel_width + 10, 1, {255, 255, 255, flash});
        }

        std::string mode_name = GetChartModeName(chart.chart_type);
        bool is_routine = (chart.chart_type == "dance-routine" || chart.chart_type == "dance-couple");
        
        std::string label = mode_name;
        if (!is_routine) {
            if (chart.variant == ChartVariant::Wild) label += " Wild";
            else label += " " + chart.difficulty_name;
        }

        font_.DrawText(renderer_, (p == 0) ? panel_left : (panel_left + panel_width), y + 6, label, 
                       selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 180}, 
                       FontSize::MEDIUM, align);
        
        // Grade
        std::string score_key = song.filepath + "|" + std::to_string(ci);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            int gx = (p == 0) ? (panel_left + panel_width - 55) : (panel_left + 55);
            font_.DrawText(renderer_, gx, y + 6, it->second.grade, {255, 255, 200, 255}, FontSize::SMALL, (p == 0) ? TextAlign::RIGHT : TextAlign::LEFT);
        }

        int meter_x = (p == 0) ? (panel_left + panel_width - 10) : (panel_left + 10);
        TextAlign meter_align = (p == 0) ? TextAlign::RIGHT : TextAlign::LEFT;

        if (chart.variant == ChartVariant::Wild) {
            std::string k = chart.variant_kanji.empty() ? "鬼" : chart.variant_kanji;
            font_.DrawText(renderer_, meter_x, y + 4, k, 
                           selected ? Color{255, 255, 255, 255} : diff_col, 
                           FontSize::LARGE, meter_align);
        } else {
            std::string meter_str = FormatMeter(chart.custom_difficulty);
            font_.DrawText(renderer_, meter_x, y + 4, meter_str, 
                           selected ? Color{255, 255, 255, 255} : diff_col, 
                           FontSize::LARGE, meter_align);
        }

        y += row_h + 4;
    }

    // 2. Groove Radar
    y += 40;
    int radar_cx = panel_left + panel_width / 2;
    int radar_cy = y + 80;
    int radar_size = 60;

    SDL_SetRenderDrawColor(renderer_, 60, 60, 80, 100);
    for (int i = 0; i < 5; ++i) {
        double angle1 = (i * 72 - 90) * M_PI / 180.0;
        double angle2 = ((i + 1) * 72 - 90) * M_PI / 180.0;
        int x1 = radar_cx + static_cast<int>(radar_size * std::cos(angle1));
        int y1 = radar_cy + static_cast<int>(radar_size * std::sin(angle1));
        int x2 = radar_cx + static_cast<int>(radar_size * std::cos(angle2));
        int y2 = radar_cy + static_cast<int>(radar_size * std::sin(angle2));
        SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer_, radar_cx, radar_cy, x1, y1);
    }
    
    const auto& selected_c = song.charts[selected_chart_[p]];
    const auto& rv = selected_c.radar;
    double vals[5] = { rv.stream, rv.voltage, rv.air, rv.freeze, rv.chaos };
    SDL_Point points[6];
    
    for (int i=0; i<5; ++i) {
        double v = vals[i] / 30.0;
        if (v > 1.1) v = 1.1;
        double dist = radar_size * v; 
        double angle = (i * 72 - 90) * M_PI / 180.0;
        points[i].x = radar_cx + static_cast<int>(dist * std::cos(angle));
        points[i].y = radar_cy + static_cast<int>(dist * std::sin(angle));
    }
    points[5] = points[0];
    
    SDL_SetRenderDrawColor(renderer_, 100, 255, 255, 150);
    SDL_RenderDrawLines(renderer_, points, 6);

    const char* labels[5] = {"S", "V", "A", "F", "C"};
    for (int i=0; i<5; ++i) {
        double angle = (i * 72 - 90) * M_PI / 180.0;
        int lx = radar_cx + static_cast<int>((radar_size + 20) * std::cos(angle));
        int ly = radar_cy + static_cast<int>((radar_size + 20) * std::sin(angle));
        font_.DrawText(renderer_, lx, ly-10, labels[i], {200,200,220,255}, FontSize::SMALL, TextAlign::CENTER);
        int val_int = static_cast<int>(vals[i]);
        if (val_int > 99) val_int = 99;
        std::string val_str = std::to_string(val_int);
        font_.DrawText(renderer_, lx, ly+5, val_str, {100,255,200,255}, FontSize::SMALL, TextAlign::CENTER);
    }
}

void GameWindow::RenderSongSelectHUD() {
    // 1. Top Bar Overlay (Title/Artist)
    DrawRect(0, 0, width_, 80, {0, 0, 0, 180});
    DrawRect(0, 78, width_, 2, {100, 100, 255, 150}); // Accent line

    const auto& songs = scanner_.GetSongs();
    if (!songs.empty()) {
        const auto& song = songs[static_cast<size_t>(selected_song_)];
        
        // Draw Large Title (Centered)
        font_.DrawText(renderer_, width_ / 2, 10, song.title, {255, 255, 255, 255}, FontSize::TITLE, TextAlign::CENTER, 1.0, "score");
        // Draw Medium Artist (Centered)
        font_.DrawText(renderer_, width_ / 2, 45, song.artist, {200, 200, 220, 255}, FontSize::MEDIUM, TextAlign::CENTER);
    } else {
        font_.DrawText(renderer_, 20, 15, "SELECT MUSIC", {255, 215, 0, 255}, FontSize::LARGE);
    }
    
    // FPS Display (Optional but stylish)
    char fps_buf[16];
    std::snprintf(fps_buf, sizeof(fps_buf), "%d FPS", 120); // Placeholder or actual if available
    font_.DrawText(renderer_, width_ - 20, 15, fps_buf, {100, 100, 120, 200}, FontSize::SMALL, TextAlign::RIGHT);

    // --- Player Info Display (Top Bar Corners) ---
    std::string chart_type_p1 = "";
    if (!songs.empty()) {
        const auto& song = songs[static_cast<size_t>(selected_song_)];
        int c_idx = selected_chart_[0];
        if (c_idx >= 0 && c_idx < (int)song.charts.size()) {
            chart_type_p1 = song.charts[c_idx].chart_type;
        }
    }

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        auto& prof = players_[p].profile;
        
        int px = (p == 0) ? 15 : width_ - 15;
        TextAlign align = (p == 0) ? TextAlign::LEFT : TextAlign::RIGHT;
        
        // Player name (y=8)
        Color name_col = prof.is_guest ? Color{150, 150, 170, 200} : Color{80, 255, 200, 255};
        font_.DrawText(renderer_, px, 8, prof.name, name_col, FontSize::SMALL, align);
        
        // --- NEW: Difficulty Selection for this player ---
        if (!songs.empty()) {
            const auto& song = songs[static_cast<size_t>(selected_song_)];
            int c_idx = selected_chart_[p];
            if (c_idx >= 0 && c_idx < (int)song.charts.size()) {
                const auto& chart = song.charts[c_idx];
                
                // Diff Name + Meter
                std::string meter_str = FormatMeter(chart.custom_difficulty);
                
                Color d_col = GetDifficultyColor(chart.difficulty_name);

                if (chart.variant == ChartVariant::Wild) {
                    std::string wild_text = "WILD";
                    if (!chart.variant_kanji.empty()) wild_text = chart.variant_kanji;
                    font_.DrawText(renderer_, px, 55, wild_text, d_col, FontSize::MEDIUM, align);
                    int tw = font_.GetTextWidth(wild_text, FontSize::MEDIUM);
                    int mx = (align == TextAlign::LEFT) ? px + tw + 10 : px - tw - 10;
                    font_.DrawText(renderer_, mx, 55 + 4, meter_str, {255, 255, 255, 255}, FontSize::SMALL, align);
                } else {
                    std::string full_str = chart.difficulty_name + " " + meter_str;
                    font_.DrawText(renderer_, px, 55, full_str, d_col, FontSize::MEDIUM, align);
                }
                
                // Chart Type (Single/Double)
                font_.DrawText(renderer_, px, 35, chart.chart_type, {180, 180, 200, 200}, FontSize::SMALL, align);

                // Ready Status
                if (players_[p].ready) {
                    int ready_y = 100;
                    font_.DrawText(renderer_, px, ready_y, "READY", {100, 255, 100, 255}, FontSize::MEDIUM, align);
                }
            }
        }

        // Ratings (y=32)
        if (!prof.is_guest) {
            // ... (rest of rating code)
            bool is_single = (chart_type_p1 == "dance-single");
            bool is_double = (chart_type_p1 == "dance-double");

            if (is_single || is_double) {
                const char* label = is_single ? "S:" : "D:";
                double val = is_single ? prof.rating_single : prof.rating_double;
                
                font_.DrawText(renderer_, px, 32 + 8, label, {150, 200, 200, 200}, FontSize::SMALL, align);
                
                int r_off = (align == TextAlign::LEFT) ? 20 : -55;
                // px + r_off is where the rating number starts
                // For right align, we need to be careful. RenderRating handles align internally if we pass it, 
                // but here I'm passing TextAlign::LEFT and manual offsets.
                // Let's make RenderRating handle the alignment better if needed.
                // Currently RenderRating(px + r1_off, 32, ...) uses TextAlign::LEFT and cur_x += ...
                
                // If align is RIGHT, we should probably pass RIGHT to RenderRating and use px as base.
                if (align == TextAlign::LEFT) {
                    RenderRating(px + 20, 32, val, TextAlign::LEFT, false);
                } else {
                    RenderRating(px - 20, 32, val, TextAlign::RIGHT, true);
                }
            }
        } else {
            font_.DrawText(renderer_, px, 32 + 8, "GUEST", {120, 120, 140, 150}, FontSize::SMALL, align);
        }
    }

    // 2. Detailed Stats Panel (Bottom Left)
    if (!songs.empty()) {
        int sx = 20;
        int sy = height_ - 180;
        const auto& song = songs[static_cast<size_t>(selected_song_)];
        std::string score_key = song.filepath + "|" + std::to_string(selected_chart_[0]);
        auto it = high_scores_.find(score_key);

        if (it != high_scores_.end()) {
            DrawRect(sx - 5, sy - 5, 250, 120, {0, 0, 0, 150});
            DrawRectOutline(sx - 5, sy - 5, 250, 120, {80, 80, 100, 200});

            char buf[64];
            font_.DrawText(renderer_, sx, sy, "HIGH SCORE STATS", {255, 215, 0, 255}, FontSize::SMALL);
            
            std::snprintf(buf, sizeof(buf), "Grade: %s", it->second.grade.c_str());
            font_.DrawText(renderer_, sx, sy + 25, buf, {255, 255, 255, 255}, FontSize::MEDIUM);

            std::snprintf(buf, sizeof(buf), "Score: %.2f%%", it->second.percentage);
            font_.DrawText(renderer_, sx, sy + 50, buf, {220, 255, 220, 255}, FontSize::MEDIUM);

            std::snprintf(buf, sizeof(buf), "EX:    %.2f", it->second.ex_score);
            font_.DrawText(renderer_, sx, sy + 75, buf, {220, 220, 255, 255}, FontSize::MEDIUM);

            // Stars
            std::string stars_str = "Stars: ";
            for (int i = 0; i < it->second.stars; ++i) stars_str += "* ";
            font_.DrawText(renderer_, sx, sy + 100, stars_str, {255, 255, 100, 255}, FontSize::SMALL);
        }
    }

    // 3. Footer Bar
    int fy = height_ - 40;
    DrawRect(0, fy, width_, 40, {15, 15, 30, 220});
    DrawRect(0, fy, width_, 2, {60, 60, 110, 200}); // Top glow

    Color white = {240, 240, 255, 255};
    Color dim = {140, 140, 160, 200};
    
    int fx = 20;
    font_.DrawText(renderer_, fx, fy + 12, "LEFT/RIGHT", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 85, fy + 12, "SONG", dim, FontSize::SMALL);
    
    fx += 140;
    font_.DrawText(renderer_, fx, fy + 12, "UP/DOWN", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 65, fy + 12, "DIFF", dim, FontSize::SMALL);

    fx += 130;
    font_.DrawText(renderer_, fx, fy + 12, "ENTER", white, FontSize::SMALL);
    font_.DrawText(renderer_, fx + 45, fy + 12, "PLAY", dim, FontSize::SMALL);

    // Player Slots
    font_.DrawText(renderer_, width_ - 140, fy + 12, "[P1]", {100, 255, 100, 255}, FontSize::SMALL);
    font_.DrawText(renderer_, width_ - 80, fy + 12, "[P2]", {150, 150, 170, 150}, FontSize::SMALL);
}

// ============================================================================
// Jacket Cache
// ============================================================================

SDL_Texture* GameWindow::GetJacketTexture(const std::string& path) {
    if (path.empty()) return nullptr;

    // Check cache
    auto it = jacket_cache_.find(path);
    if (it != jacket_cache_.end()) return it->second;

    // Verify file exists before loading
    if (!std::filesystem::exists(path)) return nullptr;

    // Load new texture
    int w, h;
    SDL_Texture* tex = LoadTexture(path, &w, &h);
    if (tex) {
        jacket_cache_[path] = tex;
    }
    return tex;
}

// ============================================================================
// Gameplay Screen (same as before, refactored as methods)
// ============================================================================

void GameWindow::RenderDecide() {
    RenderBackground();
    // Dim
    DrawRect(0, 0, width_, height_, {0, 0, 0, 180});

    if (!active_simfile_ || !current_chart_) return;

    int center_x = width_ / 2;
    int jacket_size = 280;
    int top_y = 100;

    // --- 1. Top Area: Jacket and Song Info Side-by-Side ---
    SDL_Texture* jacket = GetJacketTexture(active_simfile_->jacket_path);
    if (!jacket) jacket = GetJacketTexture(active_simfile_->banner_path);

    int content_w = jacket_size + 40 + 400; // Jacket + spacing + text area
    int content_x = center_x - content_w / 2;

    if (jacket) {
        SDL_Rect jdst = { content_x, top_y, jacket_size, jacket_size };
        SDL_RenderCopy(renderer_, jacket, nullptr, &jdst);
        DrawRectOutline(jdst.x - 2, jdst.y - 2, jdst.w + 4, jdst.h + 4, {255, 255, 255, 200});
    }

    int text_x = content_x + jacket_size + 40;
    font_.DrawText(renderer_, text_x, top_y + 40, active_simfile_->title, {255, 255, 255, 255}, FontSize::TITLE, TextAlign::LEFT, 1.0, "score");
    font_.DrawText(renderer_, text_x, top_y + 90, active_simfile_->artist, {200, 200, 220, 255}, FontSize::MEDIUM, TextAlign::LEFT);

    // --- 2. Side Panels: Player Difficulty & Best Scores ---
    int panel_y = top_y + jacket_size + 60;
    const auto& songs = scanner_.GetSongs();

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        const NoteChart* chart = players_[p].current_chart;
        if (!chart) continue;

        int px = (p == 0) ? 100 : width_ - 100;
        TextAlign align = (p == 0) ? TextAlign::LEFT : TextAlign::RIGHT;

        std::string mode_name = GetChartModeName(chart->chart_type);
        bool is_routine = (chart->chart_type == "dance-routine" || chart->chart_type == "dance-couple");
        
        std::string diff_label = mode_name;
        if (!is_routine) diff_label += " " + chart->difficulty_name;
        
        Color diff_col = GetDifficultyColor(chart->difficulty_name);
        font_.DrawText(renderer_, px, panel_y, diff_label, diff_col, FontSize::MEDIUM, align);
        
        std::string meter_str = FormatMeter(chart->custom_difficulty);
        font_.DrawText(renderer_, px, panel_y + 35, meter_str, {255, 255, 255, 255}, FontSize::LARGE, align);

        // Best Score
        std::string score_key = songs[static_cast<size_t>(selected_song_)].filepath + "|" + std::to_string(selected_chart_[p]);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            char sbuf[64];
            std::snprintf(sbuf, sizeof(sbuf), "BEST: %.2f%% (%s)", it->second.percentage, it->second.grade.c_str());
            font_.DrawText(renderer_, px, panel_y + 80, sbuf, {220, 255, 220, 200}, FontSize::SMALL, align);
        }
    }

    // Prompt?
    font_.DrawText(renderer_, center_x, height_ - 50, "PREPARING...", {255, 255, 255, 128}, FontSize::SMALL, TextAlign::CENTER);
}

void GameWindow::RenderGameplay() {
    RenderBackground();

    static uint64_t last_eval_tick = 0;
    uint64_t current_tick = SDL_GetPerformanceCounter();
    double frame_dt = (last_eval_tick == 0) ? 0.016 : static_cast<double>(current_tick - last_eval_tick) / SDL_GetPerformanceFrequency();
    last_eval_tick = current_tick;

    // --- Per-player notefield rendering ---
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (!players_[p].joined) continue;
        auto& ps = players_[p];
        render_player_idx_ = p;
        render_x_offset_ = ps.field_x_offset;

        // Clip to this player's half if 2P
        if (num_active_players_ >= 2) {
            SDL_Rect clip = { (p == 0) ? 0 : width_ / 2, 0, width_ / 2, height_ };
            SDL_RenderSetClipRect(renderer_, &clip);
        }

        // --- ATTACK EVALUATION ---
        ActiveMods raw_mods;
        if (active_simfile_ && ps.current_chart && !active_simfile_->GetEffectiveAttacks(*ps.current_chart).empty()) {
            raw_mods = EvaluateAttacks(active_simfile_->GetEffectiveAttacks(*ps.current_chart), play_time_);
        }

        // Apply smoothing (seamless transitions) — ~100ms window
        // Use a 50ms window for 'Binary' effects like Blink to minimize lingering
        double smoothing_dur = 0.1; // 100ms
        double alpha = (smoothing_dur > 0) ? std::min(1.0, frame_dt / smoothing_dur) : 1.0;
        ps.smoothed_mods = LerpActiveMods(ps.smoothed_mods, raw_mods, alpha);
        
        ActiveMods mods = ps.smoothed_mods;
        // Snap Blink if it's too low to prevent lingering
        if (raw_mods.blink == 0.0 && mods.blink < 10.0) mods.blink = 0.0;

        // Prepare frame-local config that accounts for Reverse modulation
        NoteFieldConfig frame_cfg = ps.field_config;
        double r_pct = std::clamp(mods.reverse / 100.0, 0.0, 1.0);
        frame_cfg.reverse_pct = r_pct;

        // Interpolate receptor_y smoothly
        double y_up = height_ * 0.15;
        double y_down = height_ * 0.85;
        if (ps.field_config.downscroll) {
            frame_cfg.receptor_y = y_down * (1.0 - r_pct) + y_up * r_pct;
        } else {
            frame_cfg.receptor_y = y_up * (1.0 - r_pct) + y_down * r_pct;
        }

        RenderLanes();
        RenderMeasureLines(frame_cfg, mods);
        
        // Render notes even if failed (allows "ghosting")
        RenderNotes(frame_cfg, mods);

        // Suppress judgements/flashes for failed players
        if (!ps.failed_sequence) {
            RenderHitFlashes(frame_cfg, mods);
            RenderJudgement();
        }
        
        RenderReceptors(frame_cfg, mods);
        RenderHoldIndicators(p, frame_cfg, mods);

        // Refined Dimming overlay for failed players (Animated Gradient)
        if (ps.failed_sequence) {
            double fade_in = std::min(1.0, ps.fail_animation_timer / 0.8);
            uint8_t base_alpha = static_cast<uint8_t>(180 * fade_in);
            
            int area_x = (p == 0) ? 0 : width_ / 2;
            int area_w = (num_active_players_ >= 2) ? width_ / 2 : width_;
            
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            
            // Draw gradient using vertical strips
            int num_strips = 32;
            int strip_w = (area_w + num_strips - 1) / num_strips;
            
            for (int i = 0; i < num_strips; ++i) {
                // Calculate horizontal progress within the player's area (0.0 to 1.0)
                // P1: 0.0 is left edge, 1.0 is divider
                // P2: 0.0 is divider, 1.0 is right edge
                double progress = static_cast<double>(i) / (num_strips - 1);
                
                // We want opaque at the outer edges, transparent at the divider.
                double alpha_factor = 0.0;
                if (p == 0) {
                    alpha_factor = 1.0 - progress; // Opaque at x=0, Transparent at divider
                } else {
                    alpha_factor = progress;       // Transparent at divider, Opaque at x=width
                }
                
                uint8_t alpha = static_cast<uint8_t>(base_alpha * alpha_factor);
                SDL_SetRenderDrawColor(renderer_, 0, 0, 0, alpha);
                
                SDL_Rect strip = { area_x + i * strip_w, 0, strip_w, height_ };
                SDL_RenderFillRect(renderer_, &strip);
            }
        }

        if (num_active_players_ >= 2) {
            SDL_RenderSetClipRect(renderer_, NULL);
        }
    }

    render_x_offset_ = 0;
    render_player_idx_ = 0;

    // HUD Elements
    RenderHUD();
    
    // --- Get Ready Animation Rendering ---
    if (!is_transitioning_ && ready_animation_timer_ > 0.0) {
        double timer = ready_animation_timer_;
        
        // --- Word "GET" ---
        if (timer > 0.4) {
            double get_elapsed = 2.0 - timer;
            double get_alpha = (timer < 0.9) ? ((timer - 0.4) / 0.5) : 1.0;
            // Snappier pop: Start large (2.0x) and settle fast
            double get_scale = 1.0 + 1.0 * std::exp(-get_elapsed * 12.0);
            
            font_.DrawText(renderer_, width_ / 2, height_ / 2 - 80, "GET", 
                          {255, 255, 255, static_cast<uint8_t>(255 * get_alpha)}, FontSize::TITLE, TextAlign::CENTER, get_scale);
        }

        // --- Word "READY" ---
        if (timer < 1.4) {
            double ready_elapsed = 1.4 - timer;
            double ready_alpha = (timer < 0.5) ? (timer / 0.5) : 1.0;
            // Snappier pop: Start large (2.0x) and settle fast
            double ready_scale = 1.0 + 1.0 * std::exp(-ready_elapsed * 12.0);
            
            font_.DrawText(renderer_, width_ / 2, height_ / 2 + 0, "READY", 
                          {255, 255, 255, static_cast<uint8_t>(255 * ready_alpha)}, FontSize::TITLE, TextAlign::CENTER, ready_scale);
        }
    }

    // --- Clear Type Animation Rendering ---
    if (clear_animation_timer_ > 0.0 && clear_type_ >= ClearType::FULL_COMBO) {
        double t = 3.0 - clear_animation_timer_;
        uint8_t alpha = 255;
        if (clear_animation_timer_ < 0.5) alpha = static_cast<uint8_t>((clear_animation_timer_ / 0.5) * 255);

        double pop = (t < 0.3) ? (std::pow(1.0 - (t / 0.3), 3.0) * 0.5) : 0.0;
        double scale = 1.2 + pop;

        Color col = GetClearTypeColor(clear_type_);
        std::string text = ClearTypeName(clear_type_);

        // Add extra flair for Max Score tiers
        if (clear_type_ == ClearType::ALL_PERFECT_EXTRAORDINARY) {
            // Rainbow Cycle
            double time = SDL_GetTicks() / 1000.0;
            double h = std::fmod(time * 200.0, 360.0);
            // Simple HSV to RGB approximation
            auto r = static_cast<uint8_t>(127 + 127 * std::cos(h * M_PI / 180.0));
            auto g = static_cast<uint8_t>(127 + 127 * std::cos((h + 120.0) * M_PI / 180.0));
            auto b = static_cast<uint8_t>(127 + 127 * std::cos((h + 240.0) * M_PI / 180.0));
            col = {r, g, b, alpha};
        } else if (clear_type_ == ClearType::ALL_PERFECT_CRITICAL) {
            // Silver/Crystalline theme: Pulsating light blue/silver
            double pulse = 0.8 + 0.2 * std::sin(SDL_GetTicks() / 100.0);
            col = {static_cast<uint8_t>(200 * pulse), static_cast<uint8_t>(220 * pulse), static_cast<uint8_t>(255 * pulse), alpha};
        } else if (clear_type_ == ClearType::ALL_PERFECT_PLUS) {
            // Golden-Yellow theme with extra brightness
            double pulse = 0.9 + 0.1 * std::sin(SDL_GetTicks() / 80.0);
            col = {static_cast<uint8_t>(255 * pulse), static_cast<uint8_t>(215 * pulse), 0, alpha};
        }

        font_.DrawText(renderer_, width_ / 2, height_ / 2 - 80, text, col, FontSize::TITLE, TextAlign::CENTER, scale, "score");
        
        // Add a "CLEARED!!" subtitle for MFCs
        if (clear_type_ >= ClearType::ALL_PERFECT) {
            font_.DrawText(renderer_, width_ / 2, height_ / 2 - 20, "CLEARED!!", {255, 255, 255, alpha}, FontSize::MEDIUM, TextAlign::CENTER, 1.0);
        }
    }

    if (failed_sequence_) RenderFailOverlay();
    
    RenderMasks();
}

SDL_Texture* GameWindow::GetBGATexture(const std::string& path) {
    if (path.empty()) return nullptr;
    
    // Check if it's potentially a video file
    fs::path sim_dir(active_simfile_ ? active_simfile_->directory : "");
    fs::path target_path = sim_dir / path;
    
    std::string ext = target_path.extension().string();
    for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    bool is_video = (ext == ".mp4" || ext == ".avi" || ext == ".mpg" || ext == ".mpeg" || ext == ".flv" || ext == ".mov" || ext == ".wmv" || ext == ".mkv" || ext == ".webm");

    if (is_video) {
        // If this is a new video or the decoder isn't loaded, try to load it
        if (video_path_ != target_path.string()) {
            if (video_decoder_.Load(target_path.string(), renderer_)) {
                video_path_ = target_path.string();
            } else {
                // If video load failed, clear video_path_ so we don't keep trying this specific file as a video
                video_path_ = "";
            }
        }
        
        if (video_decoder_.IsLoaded() && video_path_ == target_path.string()) {
            return video_decoder_.GetTexture();
        }
    }

    // Not a video or video load failed - check image cache
    auto it = bga_textures_.find(path);
    if (it != bga_textures_.end()) return it->second;

    // 1. Try to load as a static image
    int bw, bh;
    SDL_Texture* tex = LoadTexture(target_path.string(), &bw, &bh);

    if (tex) {
        bga_textures_[path] = tex;
        return tex;
    }

    // 2. If image load failed (and it was a video), try image fallbacks
    if (is_video) {
        const char* img_exts[] = {".png", ".jpg", ".jpeg", ".bmp"};
        bool found_fallback = false;
        
        // 2a. Try replacing extension
        for (const char* ie : img_exts) {
            fs::path fallback = target_path;
            fallback.replace_extension(ie);
            if (fs::exists(fallback)) {
                target_path = fallback;
                found_fallback = true;
                break;
            }
        }

        // 2b. If not found, look for largest image in dir
        if (!found_fallback && fs::exists(sim_dir)) {
             uintmax_t max_size = 0;
             fs::path best_match;
             
             for (const auto& entry : fs::directory_iterator(sim_dir)) {
                 if (entry.is_regular_file()) {
                     std::string e = entry.path().extension().string();
                     std::transform(e.begin(), e.end(), e.begin(), ::tolower);
                     if (e == ".png" || e == ".jpg" || e == ".jpeg" || e == ".bmp") {
                         if (entry.file_size() > max_size) {
                             max_size = entry.file_size();
                             best_match = entry.path();
                         }
                     }
                 }
             }
             
             if (!best_match.empty()) {
                 target_path = best_match;
                 found_fallback = true;
             }
        }

        if (found_fallback) {
            tex = LoadTexture(target_path.string(), &bw, &bh);
        }
    }
    
    // Cache the result (image or fallback)
    bga_textures_[path] = tex; 
    return tex;
}

void GameWindow::RenderBackground() {
    // Pure black background by default
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    if (!active_simfile_) return;

    // Find current BGA event
    double current_beat = conductor_.TimeToBeat(play_time_);
    SDL_Texture* tex = bg_texture_; // Fallback to static background

    if (!active_simfile_->bg_changes.empty()) {
        // Find last event with beat <= current_beat
        const BGEvent* best_ev = nullptr;
        for (const auto& ev : active_simfile_->bg_changes) {
            if (ev.beat <= current_beat) {
                best_ev = &ev;
            } else {
                break;
            }
        }
        if (best_ev) {
            tex = GetBGATexture(best_ev->file);
        }
    } else if (!auto_bga_file_.empty() && current_beat >= 0.0) {
        // Fallback to auto-discovered BGA
        tex = GetBGATexture(auto_bga_file_);
    }

    if (tex) {
        int bw, bh;
        SDL_QueryTexture(tex, nullptr, nullptr, &bw, &bh);
        
        // Scale to FILL the screen (keeping aspect ratio)
        double screen_aspect = static_cast<double>(width_) / height_;
        double bg_aspect = static_cast<double>(bw) / bh;
        
        SDL_Rect dest;
        if (bg_aspect > screen_aspect) {
            // BG is wider than screen
            int w = static_cast<int>(height_ * bg_aspect);
            dest = { (width_ - w) / 2, 0, w, height_ };
        } else {
            // BG is taller than screen
            int h = static_cast<int>(width_ / bg_aspect);
            dest = { 0, (height_ - h) / 2, width_, h };
        }
        
        // Apply brightness from active player (or P1 fallback)
        // Lighter: 100% (255), Normal: 80% (204), Dark: 60% (153), Darker: 40% (102), Darkest: 20% (51)
        int brightness_val = 204; // Normal default
        
        int p_idx = 0;
        if (players_[active_player_idx_].joined) p_idx = active_player_idx_;
        else if (players_[0].joined) p_idx = 0;
        else if (players_[1].joined) p_idx = 1;

        if (players_[p_idx].joined) {
            switch (players_[p_idx].bga_brightness) {
                case BGABrightness::LIGHTER: brightness_val = 255; break;
                case BGABrightness::NORMAL:  brightness_val = 204; break;
                case BGABrightness::DARK:    brightness_val = 153; break;
                case BGABrightness::DARKER:  brightness_val = 102; break;
                case BGABrightness::DARKEST: brightness_val = 51;  break;
            }
        }

        SDL_SetTextureColorMod(tex, brightness_val, brightness_val, brightness_val);
        SDL_RenderCopy(renderer_, tex, nullptr, &dest);
    }
}

void GameWindow::RenderLanes() {
    // --- Screen Filter & borders removed as per request ---
    // (Transparent notefield)
}

void GameWindow::RenderReceptors(const NoteFieldConfig& cfg, const ActiveMods& mods) {
    auto& ps = players_[render_player_idx_];
    const NoteChart* chart = ps.current_chart;
    if (!chart) return;
    
    // Stealth: hide receptors if strength > 50%
    if (mods.stealth > 50.0) return;

    int num_cols = current_chart_->num_columns;
    int lane_w = GetLaneWidth();
    double mini_scale = mods.mini != 0.0 ? CalcMiniScale(mods.mini) : 1.0;
    int receptor_y = static_cast<int>(cfg.receptor_y);

    // Note: Receptors silhouettes were removed as per user request (only on hit)

    // --- Per-lane flash textures (kept from before) ---
    bool effectively_down = cfg.downscroll ^ (cfg.reverse_pct > 0.5);

    for (int i = 0; i < num_cols; ++i) {
        double tipsy_x = CalcTipsyOffset(mods.tipsy, play_time_, i);
        int x = GetLaneX(i) + static_cast<int>(tipsy_x);
        
        // Use per-player InputMappers if a player is being rendered, otherwise legacy
        const auto& lane = (render_player_idx_ < MAX_PLAYERS && players_[render_player_idx_].joined)
            ? players_[render_player_idx_].input.GetLaneState(i)
            : input_.GetLaneState(i);

        if (flash_texture_ && (lane.pressed || lane.press_flash > 0.0)) {
            double flash_intensity = lane.pressed ? 1.0 : lane.press_flash;
            uint8_t alpha = static_cast<uint8_t>(flash_intensity * 255);
            
            // Color based on judgement
            Color fcol = {255, 255, 255, 255};
            if (lane.last_judgement != Judgement::NONE) {
                fcol = GetJudgementColor(lane.last_judgement, ex_mode_);
            }

            SDL_SetTextureBlendMode(flash_texture_, SDL_BLENDMODE_ADD);
            SDL_SetTextureAlphaMod(flash_texture_, alpha);
            SDL_SetTextureColorMod(flash_texture_, fcol.r, fcol.g, fcol.b);
            
            int draw_w = static_cast<int>(lane_w * mini_scale);
            int draw_h = static_cast<int>(flash_tex_h_ * (static_cast<double>(lane_w) / flash_tex_w_) * mini_scale);
            
            // Center scaled receptor
            int rx = x;
            if (mini_scale != 1.0) {
                rx += (lane_w - draw_w) / 2;
            }

            double angle = effectively_down ? 180.0 : 0.0;
            SDL_Rect dst;
            if (effectively_down) {
                dst = { rx, receptor_y - draw_h + 50, draw_w, draw_h };
            } else {
                dst = { rx, receptor_y - 50, draw_w, draw_h };
            }
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            SDL_RenderCopyEx(renderer_, flash_texture_, nullptr, &dst, angle, nullptr, flip);
            SDL_SetTextureColorMod(flash_texture_, 255, 255, 255); // Reset
        }
    }

    // --- Arcade: Horizontal "Judgment Line" across all lanes" ---
    // Removed as per user request


    // --- SMX-style: Unified padding lines on left and right edges ---
    double field_tipsy_x = CalcTipsyOffset(mods.tipsy, play_time_, 0); // Global wobble
    int field_left = GetFieldLeft() + static_cast<int>(field_tipsy_x);
    int field_right = GetFieldRight() + static_cast<int>(field_tipsy_x);
    double scale = static_cast<double>(width_) / 900.0;
    int pad_w = static_cast<int>(5.0 * scale);   // 5px wide at 900px
    int pad_h = static_cast<int>(50.0 * scale);  // 50px tall at 900px
    int pad_half_h = pad_h / 2;
    // Receptor_y is the midpoint of the padding lines
    int pad_top = receptor_y - pad_half_h;

    // Left padding line (just outside the left edge of the field) — blinks on beat
    uint8_t pad_alpha = static_cast<uint8_t>(120 + static_cast<int>(beat_flash_ * 135));
    SDL_SetRenderDrawColor(renderer_, 200, 200, 220, pad_alpha);
    SDL_Rect left_pad = { field_left - pad_w - 2, pad_top, pad_w, pad_h };
    SDL_RenderFillRect(renderer_, &left_pad);

    // Right padding line (just outside the right edge of the field) — blinks on beat
    SDL_Rect right_pad = { field_right + 2, pad_top, pad_w, pad_h };
    SDL_RenderFillRect(renderer_, &right_pad);

    // --- Pulsating triangles (beat-synced) ---
    // Triangle size
    int tri_w = static_cast<int>(10.0 * scale);  // width of triangle base
    int tri_h = static_cast<int>(14.0 * scale);  // height of triangle

    // Pulsation: move outward on each beat, then ease back in (smooth quadratic ease-out)
    // beat_flash_ goes from 1.0 -> 0.0 after each beat
    double eased = beat_flash_ * (2.0 - beat_flash_); // quadratic ease-out
    int pulse_offset = static_cast<int>(eased * 6.0 * scale);

    // Left triangle (points right, toward the field) 窶・sits to the left of left_pad
    {
        int tri_cx = field_left - pad_w - 2 - 4 - tri_w - pulse_offset; // moves left on beat (outward)
        int tri_top = receptor_y - tri_h / 2;

        SDL_Vertex verts[3];
        verts[0].position = { static_cast<float>(tri_cx), static_cast<float>(tri_top) };
        verts[0].color = { 200, 200, 220, 180 };
        verts[0].tex_coord = {0, 0};
        verts[1].position = { static_cast<float>(tri_cx), static_cast<float>(tri_top + tri_h) };
        verts[1].color = verts[0].color;
        verts[1].tex_coord = {0, 0};
        verts[2].position = { static_cast<float>(tri_cx + tri_w), static_cast<float>(receptor_y) };
        verts[2].color = verts[0].color;
        verts[2].tex_coord = {0, 0};

        SDL_RenderGeometry(renderer_, nullptr, verts, 3, nullptr, 0);
    }

    // Right triangle (points left, toward the field) 窶・sits to the right of right_pad
    {
        int tri_cx = field_right + 2 + pad_w + 4 + tri_w + pulse_offset; // moves right on beat (outward)
        int tri_top = receptor_y - tri_h / 2;

        SDL_Vertex verts[3];
        verts[0].position = { static_cast<float>(tri_cx), static_cast<float>(tri_top) };
        verts[0].color = { 200, 200, 220, 180 };
        verts[0].tex_coord = {0, 0};
        verts[1].position = { static_cast<float>(tri_cx), static_cast<float>(tri_top + tri_h) };
        verts[1].color = verts[0].color;
        verts[1].tex_coord = {0, 0};
        verts[2].position = { static_cast<float>(tri_cx - tri_w), static_cast<float>(receptor_y) };
        verts[2].color = verts[0].color;
        verts[2].tex_coord = {0, 0};

        SDL_RenderGeometry(renderer_, nullptr, verts, 3, nullptr, 0);
    }
}

void GameWindow::RenderBeatFlash() {
    // Disabled as per user request
}

void GameWindow::RenderMasks() {
    if (sudden_plus_val_ <= 0.0f && hidden_plus_val_ <= 0.0f) return;
    const auto& cfg = players_[render_player_idx_].field_config;

    // We mask the playfield area
    int left = GetFieldLeft() - 10;
    int right = GetFieldRight() + 10;
    int w = right - left;

    if (sudden_plus_val_ > 0.0f) {
        int mask_h = static_cast<int>(height_ * sudden_plus_val_);
        int y = cfg.downscroll ? (height_ - mask_h) : 0;
        DrawRect(left, y, w, mask_h, {0, 0, 0, 255});
        DrawRect(left, cfg.downscroll ? y : mask_h - 2, w, 2, {255, 255, 255, 100}); // Guide line
    }

    if (hidden_plus_val_ > 0.0f) {
        int mask_h = static_cast<int>(height_ * hidden_plus_val_);
        int y = cfg.downscroll ? 0 : (height_ - mask_h);
        DrawRect(left, y, w, mask_h, {0, 0, 0, 255});
        DrawRect(left, cfg.downscroll ? mask_h - 2 : y, w, 2, {255, 255, 255, 100}); // Guide line
    }
}

void GameWindow::RenderModifierMenu(int p) {
    auto& ps = players_[p];
    
    // Simple linear animation for the menu slide-in
    double dt = 1.0 / 60.0; // Assume 60fps for simple logic if not tracking precisely
    if (ps.showing_modifier_menu) {
        ps.modifier_menu_anim = std::min(1.0, ps.modifier_menu_anim + dt * 10.0);
    } else {
        ps.modifier_menu_anim = std::max(0.0, ps.modifier_menu_anim - dt * 10.0);
    }

    if (ps.modifier_menu_anim <= 0.0) return;

    // Dimensions: Side-aligned "card"
    int mw = 260;
    int mh = 420;
    int mx = (p == 0) ? 20 : (width_ - mw - 20);
    int my = (height_ - mh) / 2;

    // Apply slide animation (ease-out cubic)
    double ease = 1.0 - std::pow(1.0 - ps.modifier_menu_anim, 3.0);
    int slide_x = static_cast<int>((p == 0) ? (-mw * (1.0 - ease)) : (mw * (1.0 - ease)));
    mx += slide_x;

    // 1. Glassmorphic Background
    // Outer shadow/border
    DrawRect(mx - 2, my - 2, mw + 4, mh + 4, {0, 0, 0, 100}); // Shadow
    DrawRect(mx, my, mw, mh, {30, 30, 45, 230}); // Main semi-transparent panel
    DrawRectOutline(mx, my, mw, mh, {255, 255, 255, 60}); // Subtle white outline
    
    // Vertical accent bar
    Color p_col = (p == 0) ? Color{100, 150, 255, 255} : Color{255, 100, 150, 255};
    DrawRect((p == 0) ? mx : (mx + mw - 4), my, 4, mh, p_col);

    // Header
    font_.DrawText(renderer_, mx + mw/2, my + 15, "PLAYER MODS", {255, 255, 255, 255}, FontSize::MEDIUM, TextAlign::CENTER);
    DrawRect(mx + 20, my + 35, mw - 40, 1, {255, 255, 255, 40});

    auto renderItem = [&](int idx, const std::string& label, const std::string& value) {
        int y_pos = my + 55 + idx * 30;
        bool selected = (ps.modifier_menu_cursor == idx);
        Color item_col = selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 180};
        
        if (selected) {
            // Highlight bar
            DrawRect(mx + 6, y_pos - 4, mw - 12, 28, {255, 255, 255, 30});
            DrawRectOutline(mx + 6, y_pos - 4, mw - 12, 28, {255, 255, 255, 50});
        }

        font_.DrawText(renderer_, mx + 15, y_pos, label, item_col, FontSize::SMALL, TextAlign::LEFT);
        font_.DrawText(renderer_, mx + mw - 15, y_pos, value, selected ? p_col : item_col, FontSize::SMALL, TextAlign::RIGHT);
    };

    char buf[64];
    // 0: Speed
    if (ps.field_config.mod_type == ScrollModType::CMod) std::snprintf(buf, sizeof(buf), "%d", (int)ps.field_config.speed_mod);
    else std::snprintf(buf, sizeof(buf), "%.2f", ps.field_config.speed_mod);
    renderItem(0, "Speed", buf);

    // 1: Type
    renderItem(1, "Mod Type", (ps.field_config.mod_type == ScrollModType::XMod) ? "X-Mod" : "C-Mod");
    
    // 2: Scroll
    renderItem(2, "Scroll", ps.field_config.downscroll ? "Downscroll" : "Upscroll");
    
    // 3: Sudden
    std::snprintf(buf, sizeof(buf), "%d%%", (int)(ps.sudden_plus_val * 100));
    renderItem(3, "Sudden+", buf);
    
    // 4: Hidden
    std::snprintf(buf, sizeof(buf), "%d%%", (int)(ps.hidden_plus_val * 100));
    renderItem(4, "Hidden+", buf);

    // 5: Skin
    renderItem(5, "Noteskin", available_noteskins_[ps.noteskin_index]);

    // 6: Effect
    const char* effect_names[] = {"None", "Mirror", "Random"};
    renderItem(6, "Effects", effect_names[ps.effect_mode]);

    // 7: Life
    std::string life_name = "Standard";
    if (ps.life_meter.GetType() == LifeType::LIFE4) life_name = "Life4";
    else if (ps.life_meter.GetType() == LifeType::RISKY) life_name = "Risky";
    else if (ps.life_meter.GetType() == LifeType::FLARE) {
        std::snprintf(buf, sizeof(buf), "Flare %d", ps.life_meter.GetFlareLevel());
        life_name = buf;
    }
    renderItem(7, "Life Mode", life_name);

    // 8: Scoring
    renderItem(8, "Scoring", ps.ex_mode ? "EX Mode" : "Normal");

    // 9: Center (P1 only)
    if (p == 0) {
        renderItem(9, "Center 1P", center_1p_ ? "On" : "Off");
    } else {
        renderItem(9, "-", "-");
    }

    // 10: Combo Display
    const char* combo_names[] = {
        "None", "Combo", "Score (+)", "Score (-)",
        "Border S", "Border S+", "Border SS", "Border SS+", "Border SSS", "Border SSS+",
        "EX (+)", "EX (-)",
        "Border 1*", "Border 2*", "Border 3*", "Border 4*", "Border 5*", "Border 6*",
        "Hit Offset"
    };
    renderItem(10, "Combo Opt", combo_names[static_cast<int>(ps.combo_display_mode)]);
    
    // 11: BGA Brightness
    const char* bga_names[] = {"Lighter", "Normal", "Dark", "Darker", "Darkest"};
    renderItem(11, "BGA Bright", bga_names[static_cast<int>(ps.bga_brightness)]);

    // Sync to profile if changed
    if (ps.profile.combo_display_mode != static_cast<int>(ps.combo_display_mode)) {
        ps.profile.combo_display_mode = static_cast<int>(ps.combo_display_mode);
        // Note: Profile saving happens automatically on chart exit or profile unjoin
    }
}

std::string GameWindow::GetComboDisplayText(int p) {
    auto& ps = players_[p];
    if (ps.combo_display_mode == ComboDisplayMode::None) return "";
    if (ps.combo_display_mode == ComboDisplayMode::Combo) {
        if (ps.combo <= 1) return "";
        return std::to_string(ps.combo);
    }
    if (ps.combo_display_mode == ComboDisplayMode::HitOffset) {
        if (ps.last_judgement == Judgement::NONE || ps.last_judgement == Judgement::MISS) return "";
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%+.1fms", ps.last_timing_error * 1000.0);
        return buf;
    }

    // Scoring logic (Subtractive / Diff)
    double total_notes = static_cast<double>(ps.total_hittable_notes);
    if (total_notes <= 0) return "100.00"; // Fallback

    int judged_notes = ps.total_hits + ps.total_miss;
    
    // Normal weights (101.0 max)
    double max_w_norm = 101.0;
    double max_possible_norm = total_notes * max_w_norm;
    double points_lost_norm = (judged_notes * max_w_norm) - ps.normal_score;

    // EX weights (100.0 max)
    double max_w_ex = 100.0;
    double max_possible_ex = total_notes * max_w_ex;
    double points_lost_ex = (judged_notes * max_w_ex) - ps.ex_score;
    
    char buf[64];
    switch (ps.combo_display_mode) {
        case ComboDisplayMode::AdditiveScore: {
            double acc = (total_notes > 0) ? (ps.normal_score / total_notes) : 0.0;
            std::snprintf(buf, sizeof(buf), "%.4f%%", acc);
            return buf;
        }
        case ComboDisplayMode::SubtractiveScore: {
            double acc = (total_notes > 0) ? ((max_possible_norm - points_lost_norm) / total_notes) : 101.0;
            std::snprintf(buf, sizeof(buf), "%.4f%%", acc);
            return buf;
        }
        case ComboDisplayMode::AdditiveEX: {
            double acc = (ps.ex_score / total_notes);
            std::snprintf(buf, sizeof(buf), "%.2f%%", acc);
            return buf;
        }
        case ComboDisplayMode::SubtractiveEX: {
            double potential = (max_possible_ex - points_lost_ex) / total_notes;
            std::snprintf(buf, sizeof(buf), "%.2f%%", potential);
            return buf;
        }
        case ComboDisplayMode::DiffS:
        case ComboDisplayMode::DiffS_Plus:
        case ComboDisplayMode::DiffSS:
        case ComboDisplayMode::DiffSS_Plus:
        case ComboDisplayMode::DiffSSS:
        case ComboDisplayMode::DiffSSS_Plus: {
            double target = 97.5;
            if (ps.combo_display_mode == ComboDisplayMode::DiffS_Plus) target = 99.0;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffSS) target = 100.0;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffSS_Plus) target = 100.5;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffSSS) target = 100.75;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffSSS_Plus) target = 100.9;
            
            double target_points = (target / 100.0) * max_possible_norm;
            double border_score = (max_possible_norm - points_lost_norm - target_points);
            double border_pct = (border_score / max_possible_norm) * 101.0;

            if (border_pct < 0.0) {
                if (ps.combo > 1) return std::to_string(ps.combo);
                return "";
            }

            std::snprintf(buf, sizeof(buf), "%.4f", border_pct);
            return buf;
        }
        case ComboDisplayMode::DiffEX_1Star:
        case ComboDisplayMode::DiffEX_2Star:
        case ComboDisplayMode::DiffEX_3Star:
        case ComboDisplayMode::DiffEX_4Star:
        case ComboDisplayMode::DiffEX_5Star:
        case ComboDisplayMode::DiffEX_6Star: {
            double target = 60.0;
            if (ps.combo_display_mode == ComboDisplayMode::DiffEX_2Star) target = 73.0;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffEX_3Star) target = 82.0;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffEX_4Star) target = 88.5;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffEX_5Star) target = 95.0;
            else if (ps.combo_display_mode == ComboDisplayMode::DiffEX_6Star) target = 100.0;
            
            double target_points = (target / 100.0) * max_possible_ex;
            double border_score = (max_possible_ex - points_lost_ex - target_points);
            double border_pct = (border_score / max_possible_ex) * 100.0;

            if (border_pct < 0.0) {
                if (ps.combo > 1) return std::to_string(ps.combo);
                return "";
            }

            std::snprintf(buf, sizeof(buf), "%.4f", border_pct);
            return buf;
        }
        default: return "";
    }
}

void GameWindow::RenderHitFlashes(const NoteFieldConfig& cfg, const ActiveMods& mods) {
    auto& ps = players_[render_player_idx_];
    if (!note_texture_ || ps.hit_flashes.empty()) return;

    int lane_w = GetLaneWidth();
    int fw = note_tex_w_ / 16;
    int fh = note_tex_h_ / 8;
    double note_scale = static_cast<double>(lane_w) / fw;
    int dh = static_cast<int>(fh * note_scale);

    for (const auto& hf : ps.hit_flashes) {
        double alpha_ratio = hf.timer / 0.3;  // 1.0 -> 0.0
        uint8_t alpha = static_cast<uint8_t>(alpha_ratio * 200);

        int x = GetLaneX(hf.lane);
        int yi = static_cast<int>(cfg.receptor_y);
        int qrow = GetQuantizationRow(hf.beat);
        int frame = 0;

        double angle = 0.0;
        int rot_lane = hf.lane % 4;
        if (rot_lane == 0) angle = 90.0;       // Left
        else if (rot_lane == 1) angle = 0.0;   // Down
        else if (rot_lane == 2) angle = 180.0; // Up
        else if (rot_lane == 3) angle = -90.0; // Right

        SDL_Rect src = { frame * fw, qrow * fh, fw, fh };
        SDL_Rect dst;
        // Hit flashes (silhouettes) always centered on hit_y
        dst = { x, yi - dh / 2, lane_w, dh };

        // Color based on judgement
        Color jcol = GetJudgementColor(hf.judge);

        // Use the dedicated silhouette texture (already white)
        SDL_SetTextureBlendMode(note_silhouette_texture_, SDL_BLENDMODE_ADD);
        SDL_SetTextureAlphaMod(note_silhouette_texture_, alpha);
        SDL_SetTextureColorMod(note_silhouette_texture_, jcol.r, jcol.g, jcol.b);
        
        SDL_RenderCopyEx(renderer_, note_silhouette_texture_, &src, &dst, angle, nullptr, SDL_FLIP_NONE);
        SDL_SetTextureColorMod(note_silhouette_texture_, 255, 255, 255);
    }
}

void GameWindow::RenderFailOverlay() {
    // --- Shutter bars sliding in ---
    double shutter_t = std::min(1.0, fail_animation_timer_ / 0.4);
    int shutter_h = static_cast<int>((height_ / 2) * shutter_t);
    
    SDL_SetRenderDrawColor(renderer_, 10, 0, 0, 255);
    SDL_Rect top_shutter = { 0, 0, width_, shutter_h };
    SDL_Rect bot_shutter = { 0, height_ - shutter_h, width_, shutter_h };
    SDL_RenderFillRect(renderer_, &top_shutter);
    SDL_RenderFillRect(renderer_, &bot_shutter);

    // Red vignette / dark overlay
    double fade_in = std::min(1.0, fail_animation_timer_ / 0.8);
    uint8_t overlay_alpha = static_cast<uint8_t>(150 * fade_in);
    SDL_SetRenderDrawColor(renderer_, 40, 0, 0, overlay_alpha);
    SDL_Rect full = {0, 0, width_, height_};
    SDL_RenderFillRect(renderer_, &full);

    // "FAILED" text appears after 0.3s with a scale pop
    if (fail_animation_timer_ > 0.3) {
        double text_elapsed = fail_animation_timer_ - 0.3;
        double text_alpha = std::min(1.0, text_elapsed / 0.3);

        // Pulsating red glow for FAILED
        double pulse = 0.85 + 0.15 * std::sin(fail_animation_timer_ * 8.0);
        Color fcol = { static_cast<uint8_t>(255 * pulse), 40, 40, static_cast<uint8_t>(255 * text_alpha) };

        font_.DrawText(renderer_, width_ / 2, height_ / 2 - 40, "FAILED", fcol, FontSize::TITLE, TextAlign::CENTER);

        // Subtitle: "press r to retry" 窶・appears after 1s
        if (fail_animation_timer_ > 1.0) {
            double sub_alpha = std::min(1.0, (fail_animation_timer_ - 1.0) / 0.4);
            font_.DrawText(renderer_, width_ / 2, height_ / 2 + 30, "press r to retry", 
                           {180, 180, 200, static_cast<uint8_t>(200 * sub_alpha)}, 
                           FontSize::MEDIUM, TextAlign::CENTER);
        }
    }
}

void GameWindow::RenderJudgement() {
    if (players_[render_player_idx_].judgement_timer <= 0.0 || players_[render_player_idx_].last_judgement == Judgement::NONE) return;

    // Center the judgement text on the note field
    int field_center_x = (GetFieldLeft() + GetFieldRight()) / 2;
    int judge_y = height_ / 2;

    // Use per-player state
    auto& pstate = players_[render_player_idx_];
    Judgement last_j = pstate.last_judgement;
    double j_timer = pstate.judgement_timer;
    double last_err = pstate.last_timing_error;
    int combo = pstate.combo;
    Judgement lowest_j = pstate.lowest_judgement_in_combo;

    // Judgement text with fade
    double alpha_mult = std::min(1.0, j_timer / 0.15);
    Color jcol = GetJudgementColor(last_j, ex_mode_);
    
    // Flashing effect for highest judgement
    bool is_highest = (ex_mode_ && last_j == Judgement::PEXTRA) || 
                      (!ex_mode_ && last_j == Judgement::PCRIT);
    if (is_highest) {
        bool flash_on = (static_cast<int>(play_time_ * 20.0) % 2 == 0);
        jcol = flash_on ? Color{255, 255, 255, 255} : Color{150, 150, 200, 255};
    }
    
    jcol.a = static_cast<uint8_t>(jcol.a * alpha_mult);

    // Sprite-based judgement
    SDL_Texture* active_judge_tex = ex_mode_ ? judge_ex_texture_ : judge_normal_texture_;
    if (active_judge_tex) {
        int row = -1;
        if (ex_mode_) {
            switch (last_j) {
                case Judgement::PEXTRA:      row = 0; break;
                case Judgement::PCRIT:       row = 1; break;
                case Judgement::PERFECT:     
                case Judgement::PERFECT_LOW: row = 2; break;
                case Judgement::GREAT_HIGH:
                case Judgement::GREAT:
                case Judgement::GREAT_LOW:   row = 3; break; // OKAY
                case Judgement::MISS:        row = 4; break;
                default: break;
            }
        } else {
            switch (last_j) {
                case Judgement::PCRIT:       row = 0; break;
                case Judgement::PERFECT:
                case Judgement::PERFECT_LOW: row = 1; break;
                case Judgement::GREAT_HIGH:
                case Judgement::GREAT:
                case Judgement::GREAT_LOW:   row = 2; break;
                case Judgement::GOOD:        row = 3; break;
                case Judgement::MISS:        row = 4; break;
                default: break;
            }
        }

        if (row >= 0) {
            int tex_w = ex_mode_ ? judge_ex_w_ : judge_normal_w_;
            int tex_h = ex_mode_ ? judge_ex_h_ : judge_normal_h_;
            int num_rows = 5;
            int rh = tex_h / num_rows;

            SDL_Rect src = { 0, row * rh, tex_w, rh };
            
            // --- Animations ---
            double duration = 0.6; // Matches InputMapper::JUDGEMENT_DISPLAY
            double elapsed = duration - j_timer;
            double t = std::clamp(elapsed / duration, 0.0, 1.0);

            // 1. Scale pop: 1.2x -> 1.0x with quick ease-out
            double scale_pop = 1.0 + 0.3 * std::pow(1.0 - std::min(1.0, elapsed / 0.12), 2.0);
            
            // Final dimensions
            double base_scale = (static_cast<double>(width_) / 900.0) * 0.4;
            double final_scale = base_scale * scale_pop;
            int dw = static_cast<int>(tex_w * final_scale);
            int dh = static_cast<int>(rh * final_scale);

            SDL_Rect dst = { field_center_x - dw / 2, judge_y - dh / 2 - 10, dw, dh };

            // 3. Highest Judgment Effect: Additive Glow
            bool is_highest = (ex_mode_ && last_j == Judgement::PEXTRA) || 
                              (!ex_mode_ && last_j == Judgement::PCRIT);
            
            if (is_highest) {
                SDL_SetTextureBlendMode(active_judge_tex, SDL_BLENDMODE_ADD);
                // Pulsating "ghost" glows
                for (int i = 0; i < 2; ++i) {
                    double pulse = 1.0 + 0.1 * std::sin(play_time_ * 15.0 + i);
                    uint8_t pulse_alpha = static_cast<uint8_t>(jcol.a * 0.4 * (1.0 - t));
                    SDL_SetTextureAlphaMod(active_judge_tex, pulse_alpha);
                    
                    int pw = static_cast<int>(dw * pulse);
                    int ph = static_cast<int>(dh * pulse);
                    SDL_Rect pdst = { field_center_x - pw / 2, judge_y - ph / 2 - 10, pw, ph };
                    SDL_RenderCopy(renderer_, active_judge_tex, &src, &pdst);
                }
            }

            // Main sprite
            SDL_SetTextureBlendMode(active_judge_tex, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(active_judge_tex, jcol.a);
            SDL_RenderCopy(renderer_, active_judge_tex, &src, &dst);
            SDL_SetTextureAlphaMod(active_judge_tex, 255);
        }
    } else {
        // Fallback to text
        const char* name = JudgementName(last_j, ex_mode_);
        int text_width = GetTextWidth(name, 3);
        DrawText(field_center_x - text_width / 2, judge_y - 10, name, jcol, 3);
    }

    // Timing error indicator (FAST/SLOW) 窶・Placed ABOVE the judgement
    // Hide for highest judgements as per request
    is_highest = (ex_mode_ && last_j == Judgement::PEXTRA) ||
                 (!ex_mode_ && last_j == Judgement::PCRIT);

    if (last_j != Judgement::MISS && last_j != Judgement::NONE && !is_highest) {
        std::string err_text = last_err < 0 ? "FAST" : "SLOW";
        Color err_col = last_err < 0 ? Color{80, 180, 255, static_cast<uint8_t>(200 * alpha_mult)}
                                                 : Color{255, 100, 100, static_cast<uint8_t>(200 * alpha_mult)};
        
        font_.DrawText(renderer_, field_center_x, judge_y - 50, err_text, err_col, FontSize::SMALL, TextAlign::CENTER);
    }

    // Numeric offset 窶・only when debug overlays are active
    if (show_debug_ && last_j != Judgement::MISS && last_j != Judgement::NONE) {
        char ms_buf[16];
        std::snprintf(ms_buf, sizeof(ms_buf), "%+.1fms", last_err * 1000.0);
        font_.DrawText(renderer_, field_center_x, judge_y + 40, ms_buf, {200, 200, 200, static_cast<uint8_t>(180 * alpha_mult)}, FontSize::SMALL, TextAlign::CENTER);
    }

    // --- Timing error visual bar ---
    if (show_debug_ && last_j != Judgement::MISS && last_j != Judgement::NONE) {
        int bar_y = judge_y + 35;
        int bar_w = 100;
        int bar_h = 2;
        int bx = field_center_x - bar_w / 2;
        
        // Background bar
        DrawRect(bx, bar_y, bar_w, bar_h, {100, 100, 120, static_cast<uint8_t>(100 * alpha_mult)});
        DrawRect(field_center_x - 1, bar_y - 2, 2, 6, {150, 150, 180, static_cast<uint8_t>(150 * alpha_mult)}); // Center tick
        
        // Hit marker
        double error_ms = last_err * 1000.0; 
        double ratio = std::clamp(error_ms / 100.0, -1.0, 1.0); // ﾂｱ100ms range
        int tx = field_center_x + static_cast<int>(ratio * (bar_w / 2));
        
        Color tick_col = (error_ms < 0) ? Color{80, 200, 255, 255} : Color{255, 150, 50, 255};
        tick_col.a = static_cast<uint8_t>(255 * alpha_mult);
        DrawRect(tx - 2, bar_y - 3, 4, 8, tick_col);
    }

    // --- Combo / Score / Custom Display ---
    std::string custom_text = GetComboDisplayText(render_player_idx_);
    if (!custom_text.empty()) {
        bool is_combo = (pstate.combo_display_mode == ComboDisplayMode::Combo);
        bool is_diff = (static_cast<int>(pstate.combo_display_mode) >= static_cast<int>(ComboDisplayMode::DiffS) &&
                        static_cast<int>(pstate.combo_display_mode) <= static_cast<int>(ComboDisplayMode::DiffEX_6Star));
        bool is_score = (pstate.combo_display_mode == ComboDisplayMode::AdditiveScore || 
                         pstate.combo_display_mode == ComboDisplayMode::SubtractiveScore ||
                         pstate.combo_display_mode == ComboDisplayMode::AdditiveEX ||
                         pstate.combo_display_mode == ComboDisplayMode::SubtractiveEX);

        // Animation logic: Only pop when text changes
        if (custom_text != pstate.last_combo_text) {
            pstate.score_anim_timer = 0.6; // Matches bounce_duration
            pstate.last_combo_text = custom_text;
        }
        
        // Color based on context
        Color text_col = {255, 255, 255, 255};
        if (is_combo) {
            switch (lowest_j) {
                case Judgement::PEXTRA:
                case Judgement::PCRIT:   text_col = {255, 255, 255, 255}; break; // White
                case Judgement::PERFECT: 
                case Judgement::PERFECT_LOW: text_col = {255, 215, 0, 255}; break;   // Gold
                case Judgement::GREAT_HIGH:
                case Judgement::GREAT:
                case Judgement::GREAT_LOW: text_col = {100, 255, 100, 255}; break; // Green
                case Judgement::GOOD:    text_col = {80, 160, 255, 255}; break;  // Blue
                default: break;
            }
        } else if (is_score && !is_diff) {
            // Scoring coloring (for Additive/Subtractive non-diff)
            if (custom_text[0] == '-') text_col = {255, 80, 80, 255};
            else if (custom_text[0] == '+') text_col = {100, 255, 100, 255};
        }
        // Diff modes are forced white as per request
        
        text_col.a = static_cast<uint8_t>(255 * alpha_mult);
        
        // Animation
        double bounce_duration = 0.6;
        double bounce_elapsed = bounce_duration - pstate.score_anim_timer;
        double bounce = 1.0 + 0.15 * std::pow(1.0 - std::min(1.0, bounce_elapsed / 0.1), 3.0);
        
        int combo_x = field_center_x;
        int combo_y = judge_y - 115;

        // Styled display for scores/diffs
        if (is_score || is_diff) 
        {
            font_.DrawStyledNumber(renderer_, combo_x, combo_y, custom_text, text_col, TextAlign::CENTER, bounce, true, {0,0,0,text_col.a});
        } else {
            font_.DrawMonoText(renderer_, combo_x, combo_y, custom_text.c_str(), text_col, FontSize::HUGE, TextAlign::CENTER, bounce, "score", -1, true);
        }

        if (is_combo) {
            font_.DrawText(renderer_, combo_x, combo_y + 50, "COMBO", {220, 220, 240, text_col.a}, FontSize::SMALL, TextAlign::CENTER);
        } else {
            // Label for what we are showing
            const char* label = "";
            switch(pstate.combo_display_mode) {
                case ComboDisplayMode::SubtractiveScore: label = "MAX POTENTIAL"; break;
                case ComboDisplayMode::SubtractiveEX:    label = "MAX EX POTENTIAL"; break;
                case ComboDisplayMode::DiffS:            label = "S BUFFER"; break;
                case ComboDisplayMode::DiffS_Plus:       label = "S+ BUFFER"; break;
                case ComboDisplayMode::DiffSS:           label = "SS BUFFER"; break;
                case ComboDisplayMode::DiffSS_Plus:      label = "SS+ BUFFER"; break;
                case ComboDisplayMode::DiffSSS:          label = "SSS BUFFER"; break;
                case ComboDisplayMode::DiffSSS_Plus:     label = "SSS+ BUFFER"; break;
                case ComboDisplayMode::DiffEX_1Star:     label = "1* BUFFER"; break;
                case ComboDisplayMode::DiffEX_2Star:     label = "2* BUFFER"; break;
                case ComboDisplayMode::DiffEX_3Star:     label = "3* BUFFER"; break;
                case ComboDisplayMode::DiffEX_4Star:     label = "4* BUFFER"; break;
                case ComboDisplayMode::DiffEX_5Star:     label = "5* BUFFER"; break;
                case ComboDisplayMode::DiffEX_6Star:     label = "6* BUFFER"; break;
                default: break;
            }
            if (label[0]) font_.DrawText(renderer_, combo_x, combo_y + 50, label, {200, 200, 220, text_col.a}, FontSize::SMALL, TextAlign::CENTER);
        }
    }
}

void GameWindow::RenderMeasureLines(const NoteFieldConfig& cfg, const ActiveMods& mods) {
    auto& ps = players_[render_player_idx_];
    const NoteChart* chart = ps.current_chart;
    if (!chart) return;

    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = cfg.receptor_y;
    int receptor_yi = static_cast<int>(receptor_y);
    double current_time = play_time_;
    constexpr double FADE_DURATION = 0.5;

    int start_m = std::max(0, static_cast<int>(std::floor((current_beat - 4.0) / 4.0)));
    int end_m = static_cast<int>(std::ceil((current_beat + 20.0) / 4.0));

    auto drawBeatLine = [&](double beat, uint8_t base_r, uint8_t base_g, uint8_t base_b, uint8_t base_a) {
        double y = NoteRenderer::GetYPosForBeat(beat, conductor_, cfg);
        
        // Apply speed curves to measure lines
        if (mods.HasAnyEffect()) {
            double raw_dist = y - receptor_y;
            y = receptor_y + ApplyScrollCurves(mods, raw_dist, static_cast<double>(height_));
        }

        bool effectively_down = cfg.downscroll ^ (cfg.reverse_pct > 0.5);
        bool passed_receptor = effectively_down ? (y > receptor_y) : (y < receptor_y);
        
        double tipsy_x = CalcTipsyOffset(mods.tipsy, play_time_, 0);
        int fl = GetFieldLeft() + static_cast<int>(tipsy_x);
        int fr = GetFieldRight() + static_cast<int>(tipsy_x);

        if (passed_receptor) {
            double beat_time = conductor_.BeatToTime(beat);
            double elapsed = current_time - beat_time;
            if (elapsed < 0.0) elapsed = 0.0;
            if (elapsed >= FADE_DURATION) return;
            double fade = 1.0 - (elapsed / FADE_DURATION);
            uint8_t alpha = static_cast<uint8_t>(base_a * fade);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, alpha);
            SDL_Rect line_rect = { fl, receptor_yi - 2, fr - fl, 4 };
            SDL_RenderFillRect(renderer_, &line_rect);
        } else {
            if (y < -10 || y > height_ + 10) return;
            int yi = static_cast<int>(y);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, base_a);
            SDL_Rect line_rect = { fl, yi - 2, fr - fl, 4 };
            SDL_RenderFillRect(renderer_, &line_rect);
        }
    };

    const auto& bpm_segs = active_simfile_ ? active_simfile_->GetEffectiveBPMs(*chart) : std::vector<TimingSegment>{};
    const auto& stop_segs = active_simfile_ ? active_simfile_->GetEffectiveStops(*chart) : std::vector<TimingSegment>{};

    // Helper: check if a Stop occurs at exactly this beat
    auto hasStopAtBeat = [&](double beat) -> bool {
        for (const auto& seg : stop_segs) {
            if (std::fabs(seg.start_beat - beat) < 0.001 && seg.value > 0.0) {
                return true;
            }
        }
        return false;
    };

    // Helper: check if a BPM change occurs at exactly this beat
    // Returns: 0 = no change, +1 = speed up, -1 = slow down
    auto getBpmChangeType = [&](double beat) -> int {
        constexpr double BPM_THRESHOLD = 0.1;
        for (size_t i = 1; i < bpm_segs.size(); ++i) {
            if (std::fabs(bpm_segs[i].start_beat - beat) < 0.001) {
                double prev_bpm = bpm_segs[i - 1].value;
                double new_bpm  = bpm_segs[i].value;
                double diff = new_bpm - prev_bpm;
                if (std::fabs(diff) < BPM_THRESHOLD) return 0;
                return (diff > 0) ? 1 : -1;
            }
        }
        return 0; // no BPM change
    };

    auto getBeatColor = [&](double beat, bool is_measure) {
        if (hasStopAtBeat(beat)) {
            return std::make_pair(SDL_Color{255, 255, 80, 200}, 200); // Stop: Yellow
        }
        int bpm_change = getBpmChangeType(beat);
        if (bpm_change > 0) {
            return std::make_pair(SDL_Color{255, 120, 50, 220}, 220);   // Speed up: orange-red
        } else if (bpm_change < 0) {
            return std::make_pair(SDL_Color{0, 255, 255, 255}, 255);    // Slow down: electric cyan
        }
        if (is_measure) {
            return std::make_pair(SDL_Color{0, 255, 255, 180}, 180);    // Normal measure: Cyan
        } else {
            return std::make_pair(SDL_Color{160, 160, 220, 160}, 160);    // Normal sub-beat
        }
    };

    for (int m = start_m; m <= end_m; ++m) {
        double beat = m * 4.0;
        auto [m_color, m_alpha] = getBeatColor(beat, true);
        drawBeatLine(beat, m_color.r, m_color.g, m_color.b, m_alpha);

        // Sub-beat lines
        for (int sub = 1; sub < 4; ++sub) {
            double sub_beat = beat + sub;
            auto [s_color, s_alpha] = getBeatColor(sub_beat, false);
            drawBeatLine(sub_beat, s_color.r, s_color.g, s_color.b, s_alpha);
        }
    }
}

void GameWindow::RenderNotes(const NoteFieldConfig& cfg, const ActiveMods& mods) {
    auto& ps = players_[render_player_idx_];
    const NoteChart* chart = ps.current_chart;
    if (!chart) return;

    auto [first, last] = NoteRenderer::GetVisibleNoteRange(
        *chart, conductor_, cfg, static_cast<double>(height_), 16.0);

    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = cfg.receptor_y;

    bool has_mods = mods.HasAnyEffect();
    bool effectively_down = cfg.downscroll ^ (cfg.reverse_pct > 0.5);
    bool blink_hidden = has_mods && CalcBlinkHidden(mods.blink, play_time_);
    double mini_scale = has_mods ? CalcMiniScale(mods.mini) : 1.0;

    // Helper to render a hold body
    auto renderHoldBody = [&](int col, const NoteRow& head_row, int tail_idx, NoteType type, size_t head_row_idx) {
        if (tail_idx < 0 || tail_idx >= static_cast<int>(chart->note_rows.size())) return;
        const auto& tail_row = chart->note_rows[tail_idx];

        double head_y = NoteRenderer::GetYPosForRow(head_row, conductor_, cfg);
        double tail_y = NoteRenderer::GetYPosForRow(tail_row, conductor_, cfg);
        
        if (has_mods) {
            double h_dist = head_y - receptor_y;
            head_y = receptor_y + ApplyScrollCurves(mods, h_dist, static_cast<double>(height_));
            double t_dist = tail_y - receptor_y;
            tail_y = receptor_y + ApplyScrollCurves(mods, t_dist, static_cast<double>(height_));
        }

        int x = GetLaneX(col);
        if (has_mods) {
            // Horizontal path offsets for hold body alignment
            double h_off = CalcDrunkOffset(mods.drunk, head_row.beat, col, play_time_);
            h_off += CalcTipsyOffset(mods.tipsy, play_time_, col);
            h_off += CalcTornadoOffset(mods.tornado, head_y, receptor_y, col, chart->num_columns);
            x += static_cast<int>(h_off);
        }
        int w = GetLaneWidth();
        if (has_mods) w = static_cast<int>(w * mini_scale);

        bool head_hit = (ps.note_hit_masks[head_row_idx] & (1 << col)) != 0;
        bool key_held = ps.input.GetLaneState(col).pressed;
        bool head_passed = head_hit || (effectively_down ? head_y > receptor_y : head_y < receptor_y);
        bool tail_past_receptor = effectively_down
            ? (tail_y > receptor_y)
            : (tail_y < receptor_y);
        bool hold_active = head_hit && key_held;

        // If hold was completed (head hit and tail passed receptor), don't render
        if (head_hit && tail_past_receptor) return;

        double head_render_y = head_y;

        // Pin body start to receptor when held OR released (not when never hit)
        if (head_hit) {
            head_render_y = receptor_y;
        }

        // Clamp tail at receptor ONLY when hold is active
        double tail_render_y = tail_y;
        if (hold_active) {
            if (effectively_down) {
                if (tail_y > receptor_y) tail_render_y = receptor_y;
            } else {
                if (tail_y < receptor_y) tail_render_y = receptor_y;
            }
        }

        int body_y_start, body_y_end;
        if (effectively_down) {
            body_y_start = static_cast<int>(tail_render_y);
            body_y_end = static_cast<int>(head_render_y);
        } else {
            body_y_start = static_cast<int>(head_render_y);
            body_y_end = static_cast<int>(tail_render_y);
        }

        int top = std::min(body_y_start, body_y_end);
        int bot = std::max(body_y_start, body_y_end);
        int body_height = bot - top;

        if (body_height <= 0) return;

        // Draw body TILE the texture vertically
        SDL_Texture* body_tex = (type == NoteType::HoldHead) ? hold_body_texture_ : roll_body_texture_;
        if (body_tex) {
            bool is_routine = (chart && (chart->chart_type == "dance-routine" || chart->chart_type == "dance-couple"));
            
            // Apply P1/P2 coloring for routine modes
            if (is_routine) {
                if (col < 4) SDL_SetTextureColorMod(body_tex, 255, 64, 64);   // P1 Red
                else        SDL_SetTextureColorMod(body_tex, 64, 128, 255);  // P2 Blue
            }

            // Darken inactive holds (multiplicative with P1/P2 tint if applicable)
            if (!hold_active && head_passed) {
                uint8_t r, g, b;
                SDL_GetTextureColorMod(body_tex, &r, &g, &b);
                SDL_SetTextureColorMod(body_tex, r/2, g/2, b/2);
            }

            int tex_w, tex_h;
            SDL_QueryTexture(body_tex, nullptr, nullptr, &tex_w, &tex_h);
            double tile_scale = static_cast<double>(w) / tex_w;
            int tile_h = std::max(1, static_cast<int>(tex_h * tile_scale));

            int drawn = 0;
            while (drawn < body_height) {
                int segment_h = std::min(tile_h, body_height - drawn);
                SDL_Rect src_r = { 0, 0, tex_w, static_cast<int>(segment_h / tile_scale) };
                SDL_Rect dst_r = { x, top + drawn, w, segment_h };
                SDL_RenderCopy(renderer_, body_tex, &src_r, &dst_r);
                drawn += segment_h;
            }

            // Reset color mod
            SDL_SetTextureColorMod(body_tex, 255, 255, 255);
        }

        // Draw cap at the tail position (only when tail hasn't passed receptor)
        if (!tail_past_receptor) {
            SDL_Texture* cap_tex = (type == NoteType::HoldHead) ? hold_cap_texture_ : roll_cap_texture_;
            if (cap_tex) {
                bool is_routine = (chart && (chart->chart_type == "dance-routine" || chart->chart_type == "dance-couple"));
                if (is_routine) {
                    if (col < 4) SDL_SetTextureColorMod(cap_tex, 255, 64, 64);   // P1 Red
                    else        SDL_SetTextureColorMod(cap_tex, 64, 128, 255);  // P2 Blue
                }

                if (!hold_active && head_passed) {
                    uint8_t r, g, b;
                    SDL_GetTextureColorMod(cap_tex, &r, &g, &b);
                    SDL_SetTextureColorMod(cap_tex, r/2, g/2, b/2);
                }
                int cap_h = static_cast<int>(w * 0.5);
                // Position cap at the END of the body, not centered on tail_y
                int cap_y = effectively_down
                    ? (static_cast<int>(tail_y) - cap_h)  // Above the body top
                    : static_cast<int>(tail_y);            // Below the body bottom
                SDL_Rect cap_dst = { x, cap_y, w, cap_h };
                // Cap points UP in downscroll (effectively), DOWN in upscroll
                SDL_RendererFlip flip = effectively_down ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer_, cap_tex, nullptr, &cap_dst, 0.0, nullptr, flip);
                
                // Reset color mod
                SDL_SetTextureColorMod(cap_tex, 255, 255, 255);
            }
        }
    };

    // 1. Scan for active holds that started BEFORE 'first'
    for (int col = 0; col < chart->num_columns; ++col) {
        for (int j = static_cast<int>(first) - 1; j >= 0; --j) {
            const auto& row = chart->note_rows[j];
            NoteType nt = row.columns[col];
            if (nt == NoteType::HoldHead || nt == NoteType::RollHead) {
                int tail_idx = row.tail_row_indices[col];
                if (tail_idx >= 0) {
                    const auto& tail_row = chart->note_rows[tail_idx];
                    // Keep rendering if tail is visible or hasn't been passed by receptor
                    double tail_vpos = tail_row.visual_pos;
                    if (tail_row.beat > current_beat - 4.0) { 
                        renderHoldBody(col, row, tail_idx, nt, j);
                    }
                }
                break;
            }
            if (nt == NoteType::HoldTail) break;
        }
    }

    // 2. Render visible notes and their hold bodies
    for (size_t i = first; i < last; ++i) {
        const auto& row = chart->note_rows[i];
        double y = NoteRenderer::GetYPosForRow(row, conductor_, cfg);

        // Apply Beat: vertical bounce
        double beat_y_offset = 0.0;
        if (has_mods && mods.beat != 0.0) {
            beat_y_offset = CalcBeatOffset(mods.beat, row.beat, current_beat);
        }

        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
            NoteType type = row.columns[static_cast<size_t>(col)];
            if (type == NoteType::None) continue;
            
            bool is_head = (type == NoteType::HoldHead || type == NoteType::RollHead);
            bool hit = (ps.note_hit_masks[i] & (1 << col)) != 0;

            // Skip non-head notes that were already hit
            if (!is_head && hit) continue;

            // Blink: hide notes periodically
            if (blink_hidden && type != NoteType::Mine) continue;

            // For hold/roll heads, find the tail beat
            // For hold/roll heads, use pre-calculated tail info
            if (is_head) {

                int tail_idx = row.tail_row_indices[col];
                if (tail_idx >= 0) {
                    renderHoldBody(col, row, tail_idx, type, i);
                }
            }

            // Per-note modifier calculations
            double note_x_offset = 0.0;
            double note_scale = mini_scale;
            double note_extra_angle = 0.0;
            double note_alpha = 1.0;
            double note_y_offset = beat_y_offset;

            if (has_mods) {
                // Horizontal Offsets
                note_x_offset = CalcDrunkOffset(mods.drunk, row.beat, col, play_time_);
                note_x_offset += CalcTipsyOffset(mods.tipsy, play_time_, col);
                note_x_offset += CalcTornadoOffset(mods.tornado, y, receptor_y, col, chart->num_columns);

                // Rotations
                note_extra_angle = CalcDizzyAngle(mods.dizzy, y, receptor_y);
                note_extra_angle += CalcConfusionAngle(mods.confusion);

                // Vertical Offsets
                note_y_offset += CalcBumpyOffset(mods.bumpy, row.beat);

                // Speed Curves (Apply to distance from receptor)
                double raw_dist = y - receptor_y;
                double curved_dist = ApplyScrollCurves(mods, raw_dist, static_cast<double>(height_));
                y = receptor_y + curved_dist;

                // Visibility
                note_alpha = CalcVisibilityAlpha(mods, y, receptor_y, static_cast<double>(height_), cfg.downscroll);
            }

            double final_y = y + note_y_offset;

            // Draw the note head (on top of body)
            if (type != NoteType::HoldTail) {
                if (is_head && hit) {
                    // Hold/roll head stays at receptor while key is held or tail hasn't passed
                    int tail_idx = row.tail_row_indices[col];
                    double tail_time = (tail_idx >= 0) ? chart->note_rows[tail_idx].time : 0.0;
                    bool key_held = ps.input.GetLaneState(col).pressed;
                    bool tail_still_active = (tail_idx >= 0) && (play_time_ < tail_time);
                    
                    if (key_held || tail_still_active) {
                        DrawNote(col, receptor_y, row.beat, type, note_x_offset, note_scale, note_extra_angle, note_alpha);
                        
                        // Roll Tick Meter
                        if (type == NoteType::RollHead) {
                            // Find corresponding ActiveHold
                            for (const auto& ah : ps.active_holds) {
                                if (ah.row_index == i && ah.col_index == col) {
                                    int x = GetLaneX(col);
                                    int lw = GetLaneWidth();
                                    int my = static_cast<int>(receptor_y + (cfg.downscroll ? 40 : -40));
                                    
                                    // Progress bar dimensions
                                    int bw = lw - 10;
                                    int bh = 6;
                                    
                                    // Hit percentage
                                    double hit_pct = (ah.required_ticks > 0) ? (double)ah.ticks_hit / ah.required_ticks : 1.0;
                                    hit_pct = std::clamp(hit_pct, 0.0, 1.0);
                                    
                                    // Outer frame
                                    DrawRect(x + 5, my, bw, bh, {0,0,0,180});
                                    DrawRectOutline(x + 5, my, bw, bh, {150,150,150,255});
                                    
                                    // Inner fill (Color shifts Green -> Yellow -> Red if dropping behind)
                                    Color pcol = {100, 255, 100, 255};
                                    if (hit_pct < 0.4) pcol = {255, 50, 50, 255};
                                    else if (hit_pct < 0.8) pcol = {255, 255, 100, 255};
                                    
                                    DrawRect(x + 6, my + 1, (int)((bw - 2) * hit_pct), bh - 2, pcol);
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    DrawNote(col, final_y, row.beat, type, note_x_offset, note_scale, note_extra_angle, note_alpha);
                }
            }
        }
    }
}

void GameWindow::RenderHUD() {
    // Global center elements (Title, BPM)
    if (active_simfile_) {
        font_.DrawText(renderer_, width_ / 2, 60, active_simfile_->title, {200, 200, 220, 180}, FontSize::SMALL, TextAlign::CENTER, 1.0, "score");
    }
    char bpm_buf[32];
    std::snprintf(bpm_buf, sizeof(bpm_buf), "%.1f", conductor_.GetCurrentBPM());
    font_.DrawText(renderer_, width_ / 2, 10, "BPM", {150, 150, 180, 255}, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, width_ / 2, 28, bpm_buf, {255, 255, 255, 255}, FontSize::MEDIUM, TextAlign::CENTER);

    // Autoplay indicator
    if (autoplay_) {
        double pulse = 0.7 + 0.3 * std::sin(global_anim_timer_ * 4.0);
        uint8_t a = static_cast<uint8_t>(255 * pulse);
        font_.DrawText(renderer_, width_ / 2, 78, "AUTOPLAY", {255, 180, 50, a}, FontSize::SMALL, TextAlign::CENTER);
    }

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) {
            RenderPlayerTopHUD(p);
            RenderPlayerLifeBar(p);
        }
    }

    RenderProgressBar();

    // --- P1/P2 Labels for Co-op/Routine ---
    if (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple")) {
        int p1_x = GetLaneX(0) + (GetLaneX(3) + GetLaneWidth() - GetLaneX(0)) / 2;
        int p2_x = GetLaneX(4) + (GetLaneX(7) + GetLaneWidth() - GetLaneX(4)) / 2;
        
        // Use a consistent height relative to the receptors or top
        int label_y = 120; 
        
        font_.DrawText(renderer_, p1_x, label_y, "P1", {100, 240, 255, 255}, FontSize::MEDIUM, TextAlign::CENTER);
        font_.DrawText(renderer_, p2_x, label_y, "P2", {255, 200, 100, 255}, FontSize::MEDIUM, TextAlign::CENTER);
    }

    // Calibration Overlay
    if (is_calibrating_ && suggested_offset_ != 0.0) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "SUGGESTED OFFSET: %+d ms (SD: %.1f ms)", 
            (int)(suggested_offset_ * 1000), calibration_stdev_ * 1000);
        
        // Render centered overlay
        int cx = width_ / 2;
        font_.DrawText(renderer_, cx, height_ / 2 + 100, buf, {100, 255, 100, 255}, FontSize::MEDIUM, TextAlign::CENTER);
        font_.DrawText(renderer_, cx, height_ / 2 + 130, "PRESS [F11] TO FINISH & SAVE", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
    }

    // Debug Overlay (F1)
    if (show_debug_) {
        Color cyan = {80, 220, 255, 255};
        Color green = {80, 255, 150, 255};
        Color dim = {160, 160, 180, 200};
        Color red = {255, 100, 100, 255};
        char buf[64];
        
        // Render debug info at bottom-right
        int debug_x = width_ - 180;
        int debug_y = height_ - 160;
        int line_h = 16;
        
        std::snprintf(buf, sizeof(buf), "time: %.3f", play_time_);
        DrawText(debug_x, debug_y, buf, cyan, 1); debug_y += line_h;
        std::snprintf(buf, sizeof(buf), "beat: %.3f", conductor_.GetCurrentBeat());
        DrawText(debug_x, debug_y, buf, green, 1); debug_y += line_h;
        std::snprintf(buf, sizeof(buf), "vpos: %.2f", conductor_.GetCurrentVisualPosition());
        DrawText(debug_x, debug_y, buf, dim, 1); debug_y += line_h;
        if (conductor_.IsInStop()) { DrawText(debug_x, debug_y, "STOP", red, 1); debug_y += line_h; }
        DrawText(debug_x, debug_y, audio_loaded_ ? "audio: ON" : "audio: OFF", dim, 1);
    }
}

void GameWindow::RenderPlayerTopHUD(int p) {
    auto& ps = players_[p];
    if (!ps.current_chart || ps.failed_sequence) return;

    TextAlign align = (p == 0) ? TextAlign::LEFT : TextAlign::RIGHT;
    
    // Align to the sides of the note field
    int anchor_x = 0;
    if (p == 0) {
        // P1: Left side of P1's field
        render_player_idx_ = 0;
        render_x_offset_ = ps.field_x_offset;
        anchor_x = GetFieldLeft() - 10; 
    } else {
        // P2: Right side of P2's field
        render_player_idx_ = 1;
        render_x_offset_ = ps.field_x_offset;
        anchor_x = GetFieldRight() + 10;
    }

    int icon_y = 55;
    int acc_y = 85;
    
    // Mod tags
    std::vector<std::string> mods;
    char speed_buf[16];
    std::snprintf(speed_buf, sizeof(speed_buf), "x%.2f", ps.field_config.speed_mod);
    mods.push_back(speed_buf);
    mods.push_back(ps.field_config.downscroll ? "DOWN" : "UP");
    if (ex_mode_) mods.push_back("EX");

    // Difficulty / Meter
    std::string diff_name = ps.current_chart->difficulty_name;
    std::transform(diff_name.begin(), diff_name.end(), diff_name.begin(), ::toupper);
    
    std::string meter_str = FormatMeter(ps.current_chart->custom_difficulty);
    Color diff_col = GetDifficultyColor(ps.current_chart->difficulty_name);
    
    int meter_y = icon_y - 25;
    if (ps.current_chart->variant == ChartVariant::Wild) {
        std::string wild_text = "WILD";
        if (!ps.current_chart->variant_kanji.empty()) wild_text = ps.current_chart->variant_kanji;
        font_.DrawText(renderer_, anchor_x, meter_y, wild_text, diff_col, FontSize::MEDIUM, align);
        int tw = font_.GetTextWidth(wild_text, FontSize::MEDIUM);
        int mx = (p == 0) ? anchor_x + tw + 10 : anchor_x - tw - 10;
        font_.DrawText(renderer_, mx, meter_y + 4, meter_str, {255, 255, 255, 255}, FontSize::SMALL, align);
    } else {
        // Draw Mode + Difficulty Name + Meter
        std::string mode_name = GetChartModeName(ps.current_chart->chart_type);
        bool is_routine = (ps.current_chart->chart_type == "dance-routine" || ps.current_chart->chart_type == "dance-couple");
        
        std::string full_diff;
        if (is_routine) full_diff = mode_name + " " + meter_str;
        else if (ps.current_chart->chart_type == "dance-double") full_diff = "DOUBLE " + diff_name + " " + meter_str;
        else full_diff = diff_name + " " + meter_str;

        font_.DrawText(renderer_, anchor_x, meter_y, full_diff, diff_col, FontSize::MEDIUM, align);
    }

    int cur_x = anchor_x;
    int icon_spacing = 8;
    
    for (size_t i = 0; i < mods.size(); ++i) {
        int tw = font_.GetTextWidth(mods[i], FontSize::SMALL);
        int bw = tw + 12;
        int bx = (p == 0) ? cur_x : cur_x - bw;
        
        DrawRect(bx, icon_y, bw, 22, {40, 40, 60, 180});
        DrawRectOutline(bx, icon_y, bw, 22, {80, 80, 120, 200});
        font_.DrawText(renderer_, bx + 6, icon_y + 3, mods[i], {220, 230, 255, 255}, FontSize::SMALL);
        
        if (p == 0) cur_x += bw + icon_spacing;
        else cur_x -= (bw + icon_spacing);
    }

    // Accuracy
    double accuracy = 0.0;
    if (ps.total_hittable_notes > 0) {
        accuracy = (ps.normal_score / ps.total_hittable_notes); // Base accuracy
    }
    
    double acc_scale = 1.0;
    if (ps.grade_popup_timer > 0.0) {
        acc_scale = 1.0 + 0.15 * (ps.grade_popup_timer / 0.2);
    }
    
    Color acc_color = {200, 200, 200, 255};
    if (accuracy >= 100.75)      acc_color = {180, 240, 255, 255};
    else if (accuracy >= 100.0)  acc_color = {255, 255, 255, 255};
    else if (accuracy >= 97.5)   acc_color = {255, 230, 50, 255};
    else if (accuracy >= 90.0)   acc_color = {100, 255, 150, 255};
    else if (accuracy >= 60.0)   acc_color = {80, 160, 255, 255};
    
    int precision = ex_mode_ ? 2 : 4;
    font_.DrawAccuracy(renderer_, anchor_x, acc_y, accuracy, acc_color, align, acc_scale, precision, true, true);
    
    // Shine effect for High Grades
    if (accuracy >= 97.5) {
        double sweep_time = std::fmod(play_time_, 3.0);
        if (sweep_time < 0.6) {
            double progress = sweep_time / 0.6;
            Color shine_col = (accuracy >= 100.0) ? Color{220, 240, 255, 180} : Color{255, 255, 180, 160};
            int total_w = 220;
            int sheen_w = 40;
            int sheen_start_x = (p == 0) ? anchor_x : anchor_x - total_w;
            int sheen_x = sheen_start_x - sheen_w + static_cast<int>(total_w * progress);
            
            SDL_Rect clip = { sheen_x, acc_y - 20, sheen_w, 80 };
            SDL_RenderSetClipRect(renderer_, &clip);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_ADD);
            font_.DrawAccuracy(renderer_, anchor_x, acc_y, accuracy, shine_col, align, acc_scale, precision, false, true);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_RenderSetClipRect(renderer_, NULL);
        }
    }
}


void GameWindow::RenderProgressBar() {
    if (song_duration_ <= 0.0) return;

    int py = height_ - 4;
    int pw = width_;
    int ph = 4;

    // Background
    DrawRect(0, py, pw, ph, {30, 30, 50, 150});

    // Fill
    double progress = std::clamp(play_time_ / song_duration_, 0.0, 1.0);
    int fill_w = static_cast<int>(pw * progress);
    
    // Cyan gradient fill
    DrawRect(0, py, fill_w, ph, {80, 220, 255, 200});
    if (fill_w > 0) {
        DrawRect(fill_w - 2, py, 2, ph, {200, 240, 255, 255}); // Leading edge glow
    }
}

void GameWindow::RenderPlayerLifeBar(int p) {
    if (p < 0 || p >= MAX_PLAYERS || !players_[p].joined) return;
    auto& ps = players_[p];
    
    int bar_w = 14;
    int bar_h = static_cast<int>(height_ * 0.5);
    int bar_x = (p == 0) ? 15 : width_ - 15 - bar_w;
    int bar_y = (height_ - bar_h) / 2 + 50;

    // Background/Frame
    DrawRect(bar_x - 2, bar_y - 2, bar_w + 4, bar_h + 4, {10, 10, 20, 180});
    // Top and bottom borders only
    DrawRect(bar_x - 3, bar_y - 3, bar_w + 6, 2, {60, 60, 100, 255});
    DrawRect(bar_x - 3, bar_y + bar_h + 1, bar_w + 6, 2, {60, 60, 100, 255});

    float life = ps.life_meter.GetLife();
    LifeType type = ps.life_meter.GetType();

    if (type == LifeType::STANDARD) {
        int num_segments = 20;
        int active_segments = static_cast<int>(std::ceil(life * num_segments));
        int gap = 2;

        Color base_c;
        if (life < 0.5f) {
            float m = life * 2.0f;
            base_c = {255, static_cast<uint8_t>(255 * m), 50, 255};
        } else {
            float m = (life - 0.5f) * 2.0f;
            base_c = {static_cast<uint8_t>(255 * (1.0f - m)), 255, 50, 255};
        }

        for (int i = 0; i < num_segments; ++i) {
            int bottom_y = bar_y + bar_h - static_cast<int>((bar_h * i) / num_segments);
            int top_y = bar_y + bar_h - static_cast<int>((bar_h * (i + 1)) / num_segments) + gap;
            int sh = bottom_y - top_y;
            
            double flash = ps.segment_flash_timers[i];
            
            int offset_x = 0;
            int offset_y = 0;
            if (flash > 0.0 && i < active_segments) {
                // Snap in with acceleration
                int dir = (i % 2 == 0) ? -1 : 1;
                offset_x = static_cast<int>(dir * (flash * flash) * bar_w * 3);
            } else if (flash < 0.0 && i >= active_segments) {
                // Fall out realistically
                double f = -flash; 
                double t = 1.0 - f;
                int dir = (i % 2 == 0) ? -1 : 1;
                offset_x = static_cast<int>(dir * t * bar_w * 2);
                offset_y = static_cast<int>(t * t * 60);
            }
            int draw_x = bar_x + offset_x;
            int draw_y = top_y + offset_y;
            
            if (i >= active_segments) {
                // Drawn beneath moving pieces
                DrawRect(bar_x, top_y, bar_w, sh, {40, 40, 40, 150});
            }
            
            if (i < active_segments) {
                Color c = base_c;
                if (flash > 0.0) {
                    c.r = static_cast<uint8_t>(c.r + (255 - c.r) * flash);
                    c.g = static_cast<uint8_t>(c.g + (255 - c.g) * flash);
                    c.b = static_cast<uint8_t>(c.b + (255 - c.b) * flash);
                }
                DrawRect(draw_x, draw_y, bar_w, sh, c);
            } else if (flash < 0.0) {
                double f = -flash;
                DrawRect(draw_x, draw_y, bar_w, sh, {255, static_cast<uint8_t>(50 * f), static_cast<uint8_t>(50 * f), static_cast<uint8_t>(150 + 105 * f)});
            }
        }
    } else if (type == LifeType::LIFE4 || type == LifeType::RISKY) {
        int total_lives = (type == LifeType::LIFE4) ? 4 : 1;
        int active_lives = ps.life_meter.GetBatteryLives();
        int gap = 4;
        
        for (int i = 0; i < total_lives; ++i) {
            int bottom_y = bar_y + bar_h - static_cast<int>((bar_h * i) / total_lives);
            int top_y = bar_y + bar_h - static_cast<int>((bar_h * (i + 1)) / total_lives) + gap;
            int sh = bottom_y - top_y;

            double flash = ps.battery_flash_timers[i];

            int offset_x = 0;
            int offset_y = 0;
            if (flash > 0.0 && i < active_lives) {
                int dir = (i % 2 == 0) ? -1 : 1;
                offset_x = static_cast<int>(dir * (flash * flash) * bar_w * 3);
            } else if (flash < 0.0 && i >= active_lives) {
                double f = -flash;
                double t = 1.0 - f;
                int dir = (i % 2 == 0) ? -1 : 1;
                offset_x = static_cast<int>(dir * t * bar_w * 2);
                offset_y = static_cast<int>(t * t * 60);
            }
            int draw_x = bar_x + offset_x;
            int draw_y = top_y + offset_y;

            if (i >= active_lives && !(type == LifeType::RISKY && ps.shatter_state == ShatterState::SHATTERING)) {
                // Background dead hole (hidden during shattering)
                DrawRect(bar_x, top_y, bar_w, sh, {40, 0, 0, 150});
                DrawRectOutline(bar_x, top_y, bar_w, sh, {80, 20, 20, 255});
            }

            if (i < active_lives) {
                Color base, out, in_c;
                if (type == LifeType::RISKY) {
                    // Pulsing white for RISKY
                    double pulse = 0.5 + 0.5 * std::sin(play_time_ * 6.0);
                    uint8_t wb = static_cast<uint8_t>(180 + 75 * pulse);
                    base = {wb, wb, wb, 255};
                    out  = {255, 255, 255, 255};
                    in_c = {255, 255, 255, static_cast<uint8_t>(180 + 75 * pulse)};
                } else {
                    // Cycling colors for LIFE4 segments
                    double phase = play_time_ * 1.5 + i * 0.8;
                    uint8_t cr = static_cast<uint8_t>(std::sin(phase) * 80 + 175);
                    uint8_t cg = static_cast<uint8_t>(std::sin(phase + 2.1) * 80 + 120);
                    uint8_t cb = static_cast<uint8_t>(std::sin(phase + 4.2) * 80 + 120);
                    base = {cr, cg, cb, 255};
                    out  = {static_cast<uint8_t>(std::min(255, cr + 60)), static_cast<uint8_t>(std::min(255, cg + 60)), static_cast<uint8_t>(std::min(255, cb + 60)), 255};
                    in_c = {static_cast<uint8_t>(std::min(255, cr + 40)), static_cast<uint8_t>(std::min(255, cg + 40)), static_cast<uint8_t>(std::min(255, cb + 40)), 255};
                }
                if (flash > 0.0) {
                    base = {255, static_cast<uint8_t>(std::min(255.0, base.g + 195.0 * flash)), static_cast<uint8_t>(std::min(255.0, base.b + 195.0 * flash)), 255};
                    out = {255, 255, 255, 255};
                    in_c = {255, 255, 255, 255};
                }
                DrawRect(draw_x, draw_y, bar_w, sh, base);
                DrawRectOutline(draw_x, draw_y, bar_w, sh, out);
                DrawRect(draw_x + 2, draw_y + 2, bar_w - 4, sh / 3, in_c);
            } else if (flash < 0.0) {
                double f = -flash;
                DrawRect(draw_x, draw_y, bar_w, sh, {static_cast<uint8_t>(255 * f), 0, 0, static_cast<uint8_t>(150 + 105 * f)});
                DrawRectOutline(draw_x, draw_y, bar_w, sh, {static_cast<uint8_t>(255 * f), static_cast<uint8_t>(50 * f), static_cast<uint8_t>(50 * f), static_cast<uint8_t>(255 * f)});
            }
        }

        // --- RISKY Shatter Particle Rendering ---
        if (type == LifeType::RISKY && !ps.shatter_particles.empty()) {
            for (const auto& sp : ps.shatter_particles) {
                if (sp.alpha <= 0.0f) continue;
                uint8_t a = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, sp.alpha)));
                // Draw triangular polygon shard at particle position with rotation
                float cx = sp.x + sp.w * 0.5f;
                float cy = sp.y + sp.h * 0.5f;
                float cosR = std::cos(sp.rot);
                float sinR = std::sin(sp.rot);
                // Three vertices of a spiky triangle shard
                float pts[3][2] = {
                    { -sp.w * 0.5f, -sp.h * 0.6f },
                    {  sp.w * 0.5f,  0.0f },
                    { -sp.w * 0.3f,  sp.h * 0.6f }
                };
                SDL_Vertex verts[3];
                for (int v = 0; v < 3; ++v) {
                    float rx = pts[v][0] * cosR - pts[v][1] * sinR + cx;
                    float ry = pts[v][0] * sinR + pts[v][1] * cosR + cy;
                    verts[v].position = {rx, ry};
                    verts[v].color = {sp.c.r, sp.c.g, sp.c.b, a};
                    verts[v].tex_coord = {0.0f, 0.0f};
                }
                SDL_RenderGeometry(renderer_, nullptr, verts, 3, nullptr, 0);
            }
        }

        // --- RISKY Cracking Overlay ---
        if (type == LifeType::RISKY && ps.shatter_state == ShatterState::CRACKING) {
            int bottom_y = bar_y + bar_h;
            int top_y_slot = bar_y + gap;
            int sh_slot = bottom_y - top_y_slot;
            // Flash red overlay
            double blink = 0.5 + 0.5 * std::sin(ps.shatter_timer * 14.0);
            DrawRect(bar_x, top_y_slot, bar_w, sh_slot, {255, 0, 0, static_cast<uint8_t>(120 * blink)});
            // Growing crack network - opacity and extent increase over time
            double crack_progress = std::min(1.0, ps.shatter_timer / 1.2);
            uint8_t crack_alpha = static_cast<uint8_t>(220 * crack_progress);
            int cx = bar_x + bar_w / 2;
            int cy = bar_y + bar_h / 2;
            auto drawCrack = [&](int x1, int y1, int x2, int y2) {
                SDL_SetRenderDrawColor(renderer_, 20, 0, 0, crack_alpha);
                SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
                SDL_SetRenderDrawColor(renderer_, 255, 255, 255, static_cast<uint8_t>(crack_alpha * 0.7));
                SDL_RenderDrawLine(renderer_, x1 + 1, y1, x2 + 1, y2);
            };
            // Main vertical crack
            drawCrack(cx - 1, top_y_slot + static_cast<int>(sh_slot * 0.1), cx + 1, cy);
            drawCrack(cx + 1, cy, cx - 2, top_y_slot + static_cast<int>(sh_slot * 0.85));
            // Branches (appear progressively)
            if (crack_progress > 0.3) {
                drawCrack(cx, cy - sh_slot / 5, cx + bar_w / 3, cy - sh_slot / 8);
                drawCrack(cx, cy + sh_slot / 6, cx - bar_w / 3, cy + sh_slot / 4);
            }
            if (crack_progress > 0.6) {
                drawCrack(cx - 1, cy - sh_slot / 3, cx - bar_w / 3, cy - sh_slot / 2);
                drawCrack(cx + 1, cy + sh_slot / 3, cx + bar_w / 4, cy + sh_slot / 2);
                drawCrack(cx, top_y_slot + static_cast<int>(sh_slot * 0.2), cx + bar_w / 4, top_y_slot + static_cast<int>(sh_slot * 0.1));
            }
            if (crack_progress > 0.85) {
                drawCrack(cx - 2, cy, cx - bar_w / 2, cy + sh_slot / 6);
                drawCrack(cx + 2, cy - sh_slot / 4, cx + bar_w / 2, cy - sh_slot / 3);
            }
        }
    } else if (type == LifeType::FLARE) {
        int fill_h = static_cast<int>(bar_h * life);
        int flare = ps.life_meter.GetFlareLevel();
        
        int trail_h = static_cast<int>(bar_h * ps.displayed_life);
        if (trail_h > fill_h) {
            DrawRect(bar_x, bar_y + bar_h - trail_h, bar_w, trail_h - fill_h, {255, 255, 255, 200});
        }
        
        for (int i = 0; i < fill_h; ++i) {
            float t = static_cast<float>(i) / bar_h;
            Color c;
            
            Color flare_color;
            if (flare == 1) {
                flare_color = {255, 200, 50, 255}; // Yellow
            } else if (flare == 2) {
                flare_color = {255, 100, 50, 255}; // Orange
            } else if (flare == 3) {
                flare_color = {255, 50, 50, 255};  // Red
            } else if (flare == 4) {
                flare_color = {200, 50, 255, 255}; // Purple
            }
            
            if (flare <= 4) {
                // Gradient from White at top (t=1) to Flare Color at bottom (t=0)
                c.r = static_cast<uint8_t>(flare_color.r + (255 - flare_color.r) * t);
                c.g = static_cast<uint8_t>(flare_color.g + (255 - flare_color.g) * t);
                c.b = static_cast<uint8_t>(flare_color.b + (255 - flare_color.b) * t);
                c.a = 255;
            } else {
                double rb = t + play_time_ * 2.0;
                c = {
                    static_cast<uint8_t>(std::sin(rb * 6.28) * 127 + 128),
                    static_cast<uint8_t>(std::sin((rb + 0.33) * 6.28) * 127 + 128),
                    static_cast<uint8_t>(std::sin((rb + 0.66) * 6.28) * 127 + 128),
                    255
                };
            }
            if (ps.flare_flash_timer > 0.0) {
                double f = ps.flare_flash_timer;
                c.r = static_cast<uint8_t>(c.r + (255 - c.r) * f);
                c.g = static_cast<uint8_t>(c.g + (255 - c.g) * f);
                c.b = static_cast<uint8_t>(c.b + (200 - c.b) * f);
            }
            SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, 255);
            SDL_RenderDrawLine(renderer_, bar_x, bar_y + bar_h - i, bar_x + bar_w - 1, bar_y + bar_h - i);
        }
    }
}

// ============================================================================
// Drawing helpers
// ============================================================================

Color GameWindow::GetNoteColor(double beat) const {
    double test4 = std::round(beat * 1.0);
    if (std::fabs(beat - test4) < 0.01) return LaneColors::QUARTER;
    double test8 = std::round(beat * 2.0);
    if (std::fabs(beat * 2.0 - test8) < 0.01) return LaneColors::EIGHTH;
    double test12 = std::round(beat * 3.0);
    if (std::fabs(beat * 3.0 - test12) < 0.01) return LaneColors::TWELFTH;
    double test16 = std::round(beat * 4.0);
    if (std::fabs(beat * 4.0 - test16) < 0.01) return LaneColors::SIXTEENTH;
    double test24 = std::round(beat * 6.0);
    if (std::fabs(beat * 6.0 - test24) < 0.01) return LaneColors::TWENTYFOURTH;
    double test32 = std::round(beat * 8.0);
    if (std::fabs(beat * 8.0 - test32) < 0.01) return LaneColors::THIRTYSECOND;
    return LaneColors::OTHER;
}

int GameWindow::GetQuantizationRow(double beat) const {
    double test4 = std::round(beat * 1.0);
    if (std::fabs(beat - test4) < 0.001) return 0; // 4th
    double test8 = std::round(beat * 2.0);
    if (std::fabs(beat * 2.0 - test8) < 0.001) return 1; // 8th
    double test12 = std::round(beat * 3.0);
    if (std::fabs(beat * 3.0 - test12) < 0.001) return 2; // 12th
    double test16 = std::round(beat * 4.0);
    if (std::fabs(beat * 4.0 - test16) < 0.001) return 3; // 16th
    double test24 = std::round(beat * 6.0);
    if (std::fabs(beat * 6.0 - test24) < 0.001) return 4; // 24th
    double test32 = std::round(beat * 8.0);
    if (std::fabs(beat * 8.0 - test32) < 0.001) return 5; // 32nd
    double test48 = std::round(beat * 12.0);
    if (std::fabs(beat * 12.0 - test48) < 0.001) return 6; // 48th
    double test64 = std::round(beat * 16.0);
    if (std::fabs(beat * 16.0 - test64) < 0.001) return 7; // 64th
    return 0; // Fallback to 4th
}

Color GameWindow::GetJudgementColor(Judgement j, bool ex_mode) {
    switch (j) {
        case Judgement::PEXTRA:      return {180, 240, 255, 255};  // Electric Cyan
        case Judgement::PCRIT:       return {255, 255, 255, 255};  // Pure White
        case Judgement::PERFECT:     return {255, 230, 50, 255};   // Bright Gold/Yellow
        case Judgement::PERFECT_LOW: return {220, 200, 40, 255};   // Dimmer Yellow
        case Judgement::GREAT_HIGH:  return ex_mode ? Color{150, 255, 100, 255} : Color{100, 255, 150, 255}; // OKAY vs GREAT
        case Judgement::GREAT:       return ex_mode ? Color{100, 220, 80, 255}  : Color{50, 220, 100, 255}; 
        case Judgement::GREAT_LOW:   return ex_mode ? Color{80, 180, 50, 255}   : Color{30, 180, 80, 255};
        case Judgement::GOOD:        return {80, 160, 255, 255};   // Blue
        case Judgement::MISS:      return {255, 50, 50, 255};    // Red
        default:                   return {200, 200, 200, 255};
    }
}

Color GameWindow::GetClearTypeColor(ClearType ct) {
    switch (ct) {
        case ClearType::ALL_PERFECT_EXTRAORDINARY: return {180, 240, 255, 255}; // Electric Cyan
        case ClearType::ALL_PERFECT_CRITICAL:    return {255, 255, 255, 255}; // White
        case ClearType::ALL_PERFECT_PLUS:        return {255, 230, 50, 255};  // Gold
        case ClearType::ALL_PERFECT:             return {220, 200, 40, 255};  // Yellow
        case ClearType::FULL_COMBO_PLUS:         return {100, 255, 150, 255}; // Bright Green
        case ClearType::FULL_COMBO:              return {50, 220, 100, 255};  // Green
        case ClearType::CLEAR:                   return {80, 160, 255, 255};  // Blue
        case ClearType::FAIL:                    return {255, 50, 50, 255};   // Red
        default:                                 return {200, 200, 200, 255};
    }
}

void GameWindow::DrawNote(int lane, double y, double beat, NoteType type,
                          double x_offset, double scale, double extra_angle,
                          double alpha) {
    int x = GetLaneX(lane) + static_cast<int>(x_offset);
    int w = static_cast<int>(GetLaneWidth() * scale);
    int yi = static_cast<int>(y);
    
    // Center the scaled note on the lane
    if (scale != 1.0) {
        int orig_w = GetLaneWidth();
        x = GetLaneX(lane) + static_cast<int>(x_offset) + (orig_w - w) / 2;
    }
    
    uint8_t a8 = static_cast<uint8_t>(255 * std::clamp(alpha, 0.0, 1.0));

    // --- Mine: use mine texture with spinning rotation ---
    if (type == NoteType::Mine) {
        if (mine_texture_) {
            double s = static_cast<double>(w) / mine_tex_w_;
            int dh = static_cast<int>(mine_tex_h_ * s);
            SDL_Rect mdst = { x, yi - dh / 2, w, dh };
            SDL_SetTextureAlphaMod(mine_texture_, a8);
            SDL_RenderCopyEx(renderer_, mine_texture_, nullptr, &mdst, play_time_ * 360.0, nullptr, SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(mine_texture_, 255);
        } else {
            // Fallback: draw X with rectangle outline
            Color color = LaneColors::MINE;
            color.a = a8;
            int half = w / 2;
            int cx = x + half;
            SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
            for (int d = -2; d <= 2; ++d) {
                SDL_RenderDrawLine(renderer_, cx - half + 2, yi - half + 2 + d,
                    cx + half - 2, yi + half - 2 + d);
                SDL_RenderDrawLine(renderer_, cx + half - 2, yi - half + 2 + d,
                    cx - half + 2, yi + half - 2 + d);
            }
            DrawRectOutline(x + 2, yi - half + 2, w - 4, w - 4, color);
        }
        return;
    }

    if (note_texture_) {
        int qrow = GetQuantizationRow(beat);
        if (type == NoteType::TapP1) qrow = 0;
        else if (type == NoteType::TapP2) qrow = 1;
        else if (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple")) {
            qrow = (lane < 4) ? 0 : 1; 
        }

        int frame = static_cast<int>(play_time_ * 20.0) % 16; 
        int fw = note_tex_w_ / 16;
        int fh = note_tex_h_ / 8;
        SDL_Rect src = { frame * fw, qrow * fh, fw, fh };
        double s = static_cast<double>(w) / fw;
        int dh = static_cast<int>(fh * s);
        
        // Centering: align the vertical middle of the note to 'yi'
        SDL_Rect dst = { x, yi - dh / 2, w, dh };
        
        double angle = 0.0;
        int rot_lane = lane % 4;
        if (rot_lane == 0) angle = 90.0;       // Left
        else if (rot_lane == 1) angle = 0.0;   // Down
        else if (rot_lane == 2) angle = 180.0; // Up
        else if (rot_lane == 3) angle = -90.0; // Right
        angle += extra_angle; // Apply modifier rotation (Dizzy)
        // --- Lift: use lift texture (1x8 sprite sheet 窶・1 column, 8 quantization rows) ---
        if (type == NoteType::Lift) {
            if (lift_texture_) {
                int lfw = lift_tex_w_;       // Full width (1 column)
                int lfh = lift_tex_h_ / 8;   // Each row is 1/8 of the height
                
                // Use the same qrow (potentially overridden for P1/P2) for lift quantization
                int lqrow = qrow; 

                SDL_Rect lsrc = { 0, lqrow * lfh, lfw, lfh };
                double lscale = static_cast<double>(w) / lfw;
                int ldh = static_cast<int>(lfh * lscale);
                SDL_Rect ldst = { x, yi - ldh / 2, w, ldh };
                SDL_SetTextureAlphaMod(lift_texture_, a8);
                SDL_RenderCopyEx(renderer_, lift_texture_, &lsrc, &ldst, angle, nullptr, SDL_FLIP_NONE);
                SDL_SetTextureAlphaMod(lift_texture_, 255);
                return;
            }
            // Fallback: tint the regular note texture
            SDL_SetTextureColorMod(note_texture_, 100, 255, 255);
        } else if (type == NoteType::Fake) {
            // Fakes are semi-transparent (multiply with modifier alpha)
            a8 = static_cast<uint8_t>(a8 * 0.5);
        }

        SDL_SetTextureAlphaMod(note_texture_, a8);
        SDL_RenderCopyEx(renderer_, note_texture_, &src, &dst, angle, nullptr, SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(note_texture_, 255);

        // Reset mods
        if (type == NoteType::Lift) SDL_SetTextureColorMod(note_texture_, 255, 255, 255);
        return;
    } else {
        // Fallback to rectangle drawing if texture failed to load
        Color color = GetNoteColor(beat);
        color.a = a8;
        if (type == NoteType::TapP1) color = LaneColors::QUARTER;
        else if (type == NoteType::TapP2) color = LaneColors::EIGHTH;
        else if (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple")) {
            color = (lane < 4) ? LaneColors::QUARTER : LaneColors::EIGHTH;
        }

        int h = NOTE_HEIGHT;
        
        // Special colors for special types in fallback
        if (type == NoteType::Fake) {
            color.a = 120;
        } else if (type == NoteType::Lift) {
            color = {100, 255, 255, 255};
        }

        DrawRect(x + 1, yi - h / 2, w - 2, h, color);
        
        if (type != NoteType::Fake) {
            Color bright = { static_cast<uint8_t>(std::min(255, color.r + 60)),
                static_cast<uint8_t>(std::min(255, color.g + 60)),
                static_cast<uint8_t>(std::min(255, color.b + 60)), color.a };
            DrawRect(x + 1, yi - h / 2, w - 2, 2, bright);
            Color dark = { static_cast<uint8_t>(color.r / 2),
                static_cast<uint8_t>(color.g / 2),
                static_cast<uint8_t>(color.b / 2), color.a };
            DrawRect(x + 1, yi + h / 2 - 2, w - 2, 2, dark);
        }

        if (type == NoteType::HoldHead || type == NoteType::RollHead) {
            int mx = x + w / 2;
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 180);
            SDL_RenderDrawLine(renderer_, mx - 3, yi + 2, mx, yi - 2);
            SDL_RenderDrawLine(renderer_, mx + 3, yi + 2, mx, yi - 2);
        } else if (type == NoteType::Lift) {
            // Draw a little up arrow for lift
            int mx = x + w / 2;
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 200);
            SDL_RenderDrawLine(renderer_, mx - 4, yi - 4, mx, yi - 8);
            SDL_RenderDrawLine(renderer_, mx + 4, yi - 4, mx, yi - 8);
        }
    }
}

SDL_Texture* GameWindow::LoadTexture(const std::string& path, int* w, int* h, bool make_white) {
    int channels;
    unsigned char* pixels = stbi_load(path.c_str(), w, h, &channels, 4);
    if (!pixels) {
        std::printf("Failed to load texture '%s': %s\n", path.c_str(), stbi_failure_reason());
        return nullptr;
    }

    if (make_white) {
        for (int i = 0; i < (*w) * (*h); ++i) {
            uint32_t* p = reinterpret_cast<uint32_t*>(pixels) + i;
            // Leave alpha as is, set RGB to 255 (white)
            // Stbi 4-channel load is RGBA
            pixels[i * 4 + 0] = 255;
            pixels[i * 4 + 1] = 255;
            pixels[i * 4 + 2] = 255;
        }
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels, *w, *h, 32, 4 * (*w), SDL_PIXELFORMAT_RGBA32);
    
    SDL_Texture* tex = nullptr;
    if (surface) {
        tex = SDL_CreateTextureFromSurface(renderer_, surface);
        if (tex) {
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        }
        SDL_FreeSurface(surface);
    }
    
    stbi_image_free(pixels);
    return tex;
}

void GameWindow::ScanNoteskins() {
    available_noteskins_.clear();
    fs::path npath("noteskin");
    if (fs::exists(npath) && fs::is_directory(npath)) {
        for (const auto& entry : fs::directory_iterator(npath)) {
            if (entry.is_directory()) {
                available_noteskins_.push_back(entry.path().filename().string());
            }
        }
    }
    if (available_noteskins_.empty()) {
        available_noteskins_.push_back("Default");
    }
    
    // Sort so order is consistent
    std::sort(available_noteskins_.begin(), available_noteskins_.end());
}

bool GameWindow::LoadNoteskin(const std::string& name) {
    auto load = [&](const std::string& subpath, SDL_Texture** tex, int* w, int* h, bool white = false) {
        fs::path p("noteskin");
        p /= name;
        p /= subpath;
        std::string full = p.string();

        *tex = LoadTexture(full, w, h, white);
        if (!*tex) {
            std::printf("Error: Failed to load texture: %s\n", full.c_str());
            return false;
        }
        return true;
    };

    // Cleanup old textures
    if (note_texture_) { SDL_DestroyTexture(note_texture_); note_texture_ = nullptr; }
    if (note_silhouette_texture_) { SDL_DestroyTexture(note_silhouette_texture_); note_silhouette_texture_ = nullptr; }
    if (flash_texture_) { SDL_DestroyTexture(flash_texture_); flash_texture_ = nullptr; }
    if (mine_texture_) { SDL_DestroyTexture(mine_texture_); mine_texture_ = nullptr; }
    if (lift_texture_) { SDL_DestroyTexture(lift_texture_); lift_texture_ = nullptr; }
    if (hold_body_texture_) { SDL_DestroyTexture(hold_body_texture_); hold_body_texture_ = nullptr; }
    if (hold_cap_texture_) { SDL_DestroyTexture(hold_cap_texture_); hold_cap_texture_ = nullptr; }
    if (roll_body_texture_) { SDL_DestroyTexture(roll_body_texture_); roll_body_texture_ = nullptr; }
    if (roll_cap_texture_) { SDL_DestroyTexture(roll_cap_texture_); roll_cap_texture_ = nullptr; }

    bool ok = true;
    ok &= load("_Down Tap Note 16x8 (doubleres).png", &note_texture_, &note_tex_w_, &note_tex_h_);
    ok &= load("_Down Tap Note 16x8 (doubleres).png", &note_silhouette_texture_, &note_tex_w_, &note_tex_h_, true);
    ok &= load("_Down Tap Flash (doubleres).png", &flash_texture_, &flash_tex_w_, &flash_tex_h_);
    ok &= load("Down Tap Mine (doubleres).png", &mine_texture_, &mine_tex_w_, &mine_tex_h_);
    ok &= load("_Down Tap Lift 1x8 (doubleres).png", &lift_texture_, &lift_tex_w_, &lift_tex_h_);

    int dummy_w, dummy_h;
    ok &= load("Down Hold Body Active (doubleres).png", &hold_body_texture_, &dummy_w, &dummy_h);
    ok &= load("Down Hold Bottomcap Active (doubleres).png", &hold_cap_texture_, &dummy_w, &dummy_h);
    ok &= load("Down Roll Body Active (doubleres).png", &roll_body_texture_, &dummy_w, &dummy_h);
    ok &= load("Down Roll Bottomcap Active (doubleres).png", &roll_cap_texture_, &dummy_w, &dummy_h);
    
    // Judgement sprites (stay in sprites/judgement for now)
    if (!judge_normal_texture_) {
        // Only load if not already loaded, since these aren't per-noteskin yet in this implementation
        judge_normal_texture_ = LoadTexture("sprites/judgement/normal.png", &judge_normal_w_, &judge_normal_h_);
    }
    if (!judge_ex_texture_) {
        judge_ex_texture_ = LoadTexture("sprites/judgement/EX.png", &judge_ex_w_, &judge_ex_h_);
    }

    if (ok) {
        std::printf("Noteskin '%s' loaded successfully.\n", name.c_str());
        // Find index
        for (size_t i = 0; i < available_noteskins_.size(); ++i) {
            if (available_noteskins_[i] == name) {
                noteskin_index_ = static_cast<int>(i);
                break;
            }
        }
    }
    
    return ok;
}

void GameWindow::DrawRect(int x, int y, int w, int h, Color color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer_, &rect);
}

void GameWindow::DrawRectOutline(int x, int y, int w, int h, Color color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer_, &rect);
}

Color GameWindow::GetDifficultyColor(const std::string& diff_name) {
    // SM difficulty colors
    std::string d = diff_name;
    for (auto& c : d) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (d == "beginner")  return {100, 200, 255, 255}; // Light blue
    if (d == "easy")      return {80, 255, 80, 255};   // Green
    if (d == "medium")    return {255, 200, 50, 255};   // Yellow
    if (d == "hard")      return {255, 80, 80, 255};    // Red
    if (d == "challenge") return {200, 80, 255, 255};   // Purple
    if (d == "edit")      return {160, 160, 160, 255};  // Gray
    return {180, 180, 200, 255};
}

// ============================================================================
// Settings
// ============================================================================

void GameWindow::SaveSettings() {
    std::FILE* f = std::fopen("settings.cfg", "w");
    if (!f) return;
    std::fprintf(f, "speed_mod=%.2f\n", field_config_.speed_mod);
    std::fprintf(f, "mod_type=%d\n", static_cast<int>(field_config_.mod_type));
    std::fprintf(f, "downscroll=%d\n", field_config_.downscroll ? 1 : 0);
    std::fprintf(f, "fullscreen=%d\n", fullscreen_ ? 1 : 0);
    std::fprintf(f, "audio_offset=%.4f\n", audio_offset_);
    std::fprintf(f, "center_1p=%d\n", center_1p_ ? 1 : 0);
    std::fprintf(f, "ex_mode=%d\n", ex_mode_ ? 1 : 0);
    
    for (int p = 0; p < 2; ++p) {
        for (int l = 0; l < 6; ++l) {
            for (int s = 0; s < 3; ++s) {
                const auto& b = custom_binds_[p][l][s];
                if (b.type != BindInfo::NONE) {
                    std::fprintf(f, "bind_p%d_%d_%d_type=%d\n", p + 1, l, s, (int)b.type);
                    std::fprintf(f, "bind_p%d_%d_%d_id=%d\n", p + 1, l, s, b.id);
                }
            }
        }
    }

    std::fclose(f);
}

void GameWindow::LoadSettings() {
    // Default values if settings.cfg is missing
    field_config_.speed_mod = 2.0;
    field_config_.mod_type  = ScrollModType::XMod;
    field_config_.downscroll = false;
    center_1p_ = false;
    audio_offset_ = 0.0;
    ex_mode_ = false;

    std::FILE* f = std::fopen("settings.cfg", "r");
    if (!f) {
        // Apply defaults even if no file exists
        field_config_.receptor_y = height_ * 0.15;
        ApplyInputBindings();
        return;
    }
    char line[128];
    while (std::fgets(line, sizeof(line), f)) {
        char key[64], val[64];
        if (std::sscanf(line, "%63[^=]=%63s", key, val) == 2) {
            std::string skey = key;
            if (skey == "speed_mod") field_config_.speed_mod = std::atof(val);
            else if (skey == "mod_type") field_config_.mod_type = static_cast<ScrollModType>(std::atoi(val));
            else if (skey == "downscroll") field_config_.downscroll = (std::atoi(val) != 0);
            else if (skey == "fullscreen") fullscreen_ = (std::atoi(val) != 0);
            else if (skey == "audio_offset") audio_offset_ = std::atof(val);
            else if (skey == "center_1p") center_1p_ = (std::atoi(val) != 0);
            else if (skey == "ex_mode") ex_mode_ = (std::atoi(val) != 0);
            else if (skey.find("bind_p") == 0) {
                // Format: bind_p{1,2}_{action}_{slot}_type or id
                int p = -1, l = -1, s = -1;
                char suffix[16];
                if (std::sscanf(skey.c_str(), "bind_p%d_%d_%d_%15s", &p, &l, &s, suffix) == 4) {
                    if (p >= 1 && p <= 2 && l >= 0 && l < 10 && s >= 0 && s < 3) {
                        std::string s_suffix = suffix;
                        if (s_suffix == "type") custom_binds_[p - 1][l][s].type = (BindInfo::Type)std::atoi(val);
                        else if (s_suffix == "id") custom_binds_[p - 1][l][s].id = std::atoi(val);
                    }
                }
            }
        }
    }
    std::fclose(f);

    // Update derived state
    if (field_config_.downscroll)
        field_config_.receptor_y = height_ * 0.85;
    else
        field_config_.receptor_y = height_ * 0.15;

    ApplyInputBindings();
}

void GameWindow::ApplyInputBindings() {
    int num_cols = current_chart_ ? current_chart_->num_columns : 4;
    std::string chart_type = current_chart_ ? current_chart_->chart_type : "dance-single";

    input_.Configure(chart_type, num_cols);

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        players_[p].input.ConfigureForPlayer(chart_type, num_cols, p);

        // Custom binds index 4=Start, 5=Select
        for (int action = 0; action < 6; ++action) {
            int virtual_col = (action < 4) ? action : (action == 4 ? InputMapper::COL_START : InputMapper::COL_SELECT);

            bool has_custom = false;
            for (int s = 0; s < 3; ++s) {
                if (custom_binds_[p][action][s].type != BindInfo::NONE) {
                    has_custom = true;
                    break;
                }
            }

            if (has_custom) {
                // Wipe defaults ONLY for this specific action if custom binds exist
                players_[p].input.ClearBindingsForColumn(virtual_col);

                for (int s = 0; s < 3; ++s) {
                    const auto& b = custom_binds_[p][action][s];
                    if (b.type == BindInfo::KEY) {
                        players_[p].input.AddBinding({ (SDL_Keycode)b.id, virtual_col });
                    } else if (b.type == BindInfo::BUTTON) {
                        // For now we don't have an AddButtonBinding but we can add it if needed
                        // or just use a local map. Since Select/Start are usually keys, this fix
                        // primarily targets the reported issue.
                        std::unordered_map<SDL_GameControllerButton, int> bb;
                        bb[(SDL_GameControllerButton)b.id] = virtual_col;
                        players_[p].input.SetButtonBindings(bb);
                    }
                }
            }
        }
    }
}

// ============================================================================
// Layout
// ============================================================================
// Layout
// ============================================================================

// ============================================================================
// Layout Helpers
// ============================================================================

int GameWindow::GetLaneWidth() const {
    double scale = static_cast<double>(width_) / 900.0;
    return static_cast<int>(70.0 * scale); // Widened from 64
}

int GameWindow::GetFieldLeft() const {
    auto& ps = players_[render_player_idx_];
    const NoteChart* chart = ps.current_chart;
    int num_cols = chart ? chart->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;

    if (num_cols == 4) {
        int lane_w = GetLaneWidth();
        int scaled_padding = static_cast<int>(LANE_PADDING * scale);
        int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
        return (width_ - total_w) / 2 + render_x_offset_;
    }

    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
    
    if (chart && chart->chart_type == "dance-couple" && num_cols == 8) {
        total_w += static_cast<int>(60 * scale);
    }
    
    return (width_ - total_w) / 2 + render_x_offset_;
}

int GameWindow::GetFieldRight() const {
    auto& ps = players_[render_player_idx_];
    const NoteChart* chart = ps.current_chart;
    int num_cols = chart ? chart->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    int field_left = GetFieldLeft();
    int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
    if (chart && chart->chart_type == "dance-couple" && num_cols == 8) {
        total_w += static_cast<int>(60 * scale);
    }
    return field_left + total_w;
}

int GameWindow::GetLaneX(int lane) const {
    auto& ps = players_[render_player_idx_];
    int actual_lane = lane;
    if (ps.effect_mode == 1 && ps.current_chart) { // Mirror
        actual_lane = ps.current_chart->num_columns - 1 - lane;
    }

    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    int field_left = GetFieldLeft();

    // --- Routine/Couple Split Logic ---
    // No gap between halves as per user request

    return field_left + actual_lane * (lane_w + scaled_padding);
}

std::string GameWindow::GetChartModeName(const std::string& type) {
    if (type == "dance-single")     return "SINGLE";
    if (type == "dance-double")     return "DOUBLE";
    if (type == "dance-couple")     return "COUPLE";
    if (type == "dance-routine")    return "ROUTINE";
    if (type == "dance-solo")       return "SOLO";
    if (type == "dance-threepanel") return "3-PANEL";
    if (type == "kb7-single")       return "7-KEYS";
    return "UNKNOWN";
}

// ============================================================================
// Built-in test chart (unchanged)
// ============================================================================


void GameWindow::UpdateScores(double abs_error, int num_notes, int p) {
    Judgement j_norm = ClassifyHit(abs_error, false);
    Judgement j_ex = ClassifyHit(abs_error, true);

    int ni = static_cast<int>(j_norm) - 1;
    if (ni >= 0 && ni < 9) normal_judge_counts_[ni] += num_notes;

    int ei = static_cast<int>(j_ex) - 1;
    if (ei >= 0 && ei < 9) ex_judge_counts_[ei] += num_notes;

    double norm_weighted = JudgeWeight(j_norm, false) * num_notes;
    double ex_weighted = JudgeWeight(j_ex, true) * num_notes;
    
    normal_score_ += norm_weighted;
    ex_score_     += ex_weighted;

    if (p >= 0 && p < MAX_PLAYERS && players_[p].joined) {
        auto& ps = players_[p];
        ps.normal_score += norm_weighted;
        ps.ex_score     += ex_weighted;
        
        int pni = static_cast<int>(j_norm) - 1;
        if (pni >= 0 && pni < 9) ps.normal_judge_counts[pni] += num_notes;
        
        int pei = static_cast<int>(j_ex) - 1;
        if (pei >= 0 && pei < 9) ps.ex_judge_counts[pei] += num_notes;

        if (j_norm == Judgement::MISS) ps.total_miss += num_notes;
        else ps.total_hits += num_notes;
        
        // Update Life
        ps.life_meter.OnJudgement(j_norm);

        // Check for Fail
        if (ps.life_meter.IsFailed() && !ps.failed_sequence) {
            ps.failed_sequence = true;
            ps.fail_animation_timer = 0.0;
            // Clear combo on fail
            ps.combo = 0;
            std::printf("Player %d FAILED!\n", p + 1);

            // Initiate RISKY shatter sequence
            if (ps.life_meter.GetType() == LifeType::RISKY) {
                ps.shatter_state = ShatterState::CRACKING;
                ps.shatter_timer = 0.0;
                // Stop audio immediately on RISKY fail
                audio_.Stop();
            }
        }
    }
    
    // Global Life (Legacy)
    if (p == active_player_idx_) life_meter_.OnJudgement(j_norm);
}

void GameWindow::UpdateClearType(Judgement j_norm, Judgement j_ex, int player_idx) {
    auto updateCT = [](ClearType& ct, Judgement jn, Judgement jx) {
        // 1. All Perfect Extraordinary: All notes are P-Extraordinary
        if (jx != Judgement::PEXTRA) {
            if (ct == ClearType::ALL_PERFECT_EXTRAORDINARY)
                ct = ClearType::ALL_PERFECT_CRITICAL;
        }

        // 2. All Perfect Critical: All notes are P-Critical (or higher in EX mode)
        if (jn > Judgement::PCRIT) {
            if (static_cast<int>(ct) >= static_cast<int>(ClearType::ALL_PERFECT_CRITICAL))
                ct = ClearType::ALL_PERFECT_PLUS;
        }

        // 3. All Perfect+: All notes are Perfect or higher
        if (jn > Judgement::PERFECT) {
            if (static_cast<int>(ct) >= static_cast<int>(ClearType::ALL_PERFECT_PLUS))
                ct = ClearType::ALL_PERFECT;
        }

        // 4. All Perfect: All notes are Perfect(Low) or higher
        if (jn > Judgement::PERFECT_LOW) {
            if (static_cast<int>(ct) >= static_cast<int>(ClearType::ALL_PERFECT))
                ct = ClearType::FULL_COMBO_PLUS;
        }

        // 5. Full Combo+: Great(Low) or higher (Normal) OR Okay(Low) or higher (EX)
        if (jn > Judgement::GREAT_LOW) {
            if (static_cast<int>(ct) >= static_cast<int>(ClearType::FULL_COMBO_PLUS))
                ct = ClearType::FULL_COMBO;
        }

        // 6. Full Combo: Good or higher
        if (jn > Judgement::GOOD) {
            if (static_cast<int>(ct) >= static_cast<int>(ClearType::FULL_COMBO))
                ct = ClearType::CLEAR;
        }

        // 7. Clear: Miss or higher
        if (jn == Judgement::MISS) {
            ct = ClearType::CLEAR; // If we were in FC and got a miss, we are now just CLEAR
        }
    };

    updateCT(clear_type_, j_norm, j_ex);

    if (player_idx >= 0 && player_idx < MAX_PLAYERS && players_[player_idx].joined) {
        updateCT(players_[player_idx].clear_type, j_norm, j_ex);
    }
}

// ============================================================================
// Rating Styling
// ============================================================================

GameWindow::RatingStyle GameWindow::GetRatingStyle(double rating) {
    RatingStyle style;
    style.main_color = {160, 160, 160, 255}; // Default Gray
    style.outline_color = {0, 0, 0, 255};    // Default Black outline

    // Outline Thresholds
    if (rating >= 30.00)      style.outline_color = {185, 242, 255, 255}; // Diamond
    else if (rating >= 29.00) style.outline_color = {255, 215, 0, 255};   // Gold
    else if (rating >= 28.00) style.outline_color = {192, 192, 192, 255}; // Silver
    else if (rating >= 26.50) style.outline_color = {205, 127, 50, 255};  // Bronze

    // Color Scale
    if (rating >= 32.00) {
        style.is_rainbow = true;
        style.rainbow_type = 2;
    } else if (rating >= 31.00) {
        style.is_rainbow = true;
        style.rainbow_type = 1;
    } else if (rating >= 29.00) style.main_color = {185, 242, 255, 255}; // Diamond
    else if (rating >= 28.00) style.main_color = {255, 215, 0, 255};   // Gold
    else if (rating >= 26.50) style.main_color = {192, 192, 192, 255}; // Silver
    else if (rating >= 25.00) style.main_color = {205, 127, 50, 255};  // Bronze
    else if (rating >= 22.00) style.main_color = {205, 50, 255, 255};  // Purple
    else if (rating >= 19.00) style.main_color = {255, 50, 50, 255};   // Red
    else if (rating >= 16.00) style.main_color = {255, 255, 0, 255};   // Yellow
    else if (rating >= 13.00) style.main_color = {50, 255, 50, 255};    // Lime
    else if (rating >= 10.00) style.main_color = {0, 255, 255, 255};    // Cyan
    else if (rating >= 7.00)  style.main_color = {200, 255, 255, 255};  // White-Cyan
    
    return style;
}

static Color HueToRGB(float h) {
    h = std::fmod(h, 1.0f);
    if (h < 0) h += 1.0f;
    float r=0, g=0, b=0;
    int i = static_cast<int>(h * 6);
    float f = h * 6 - i;
    float q = 1 - f;
    float t = f;
    switch (i % 6) {
        case 0: r = 1; g = t; b = 0; break;
        case 1: r = q; g = 1; b = 0; break;
        case 2: r = 0; g = 1; b = t; break;
        case 3: r = 0; g = q; b = 1; break;
        case 4: r = t; g = 0; b = 1; break;
        case 5: r = 1; g = 0; b = q; break;
    }
    return { static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(b * 255), 255 };
}

void GameWindow::RenderRating(int x, int y, double rating, TextAlign align, bool is_p2) {
    RatingStyle style = GetRatingStyle(rating);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f", rating);
    std::string text = buf;

    // Calculate widths for placement
    int int_char_w = font_.GetTextWidth("0", FontSize::LARGE, "score");
    int dec_char_w = font_.GetTextWidth("0", FontSize::MEDIUM, "score") - 2;
    int dot_w      = font_.GetTextWidth(".", FontSize::MEDIUM, "score");
    int spacing    = 7;

    int total_w = 0;
    bool past_dot = false;
    for (char c : text) {
        if (c == '.') {
            total_w += dot_w;
            past_dot = true;
        } else {
            total_w += (past_dot ? dec_char_w : int_char_w);
            total_w += spacing;
        }
    }
    // Remove last spacing if not a dot
    if (!text.empty() && text.back() != '.') total_w -= spacing;

    int cur_x = x;
    if (align == TextAlign::CENTER) cur_x -= total_w / 2;
    else if (align == TextAlign::RIGHT) cur_x -= total_w;

    double time = attraction_timer_;
    past_dot = false;

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        Color c = style.main_color;
        if (style.is_rainbow) {
            float hue = static_cast<float>(time * (style.rainbow_type == 2 ? 1.5 : 0.8) - i * 0.1);
            c = HueToRGB(hue);
            if (style.rainbow_type == 2) {
                c.r = std::min(255, c.r + 50);
                c.g = std::min(255, c.g + 50);
                c.b = std::min(255, c.b + 50);
            }
        }

        std::string char_str(1, ch);
        if (ch == '.') {
            font_.DrawMonoText(renderer_, cur_x, y + 5, char_str, c, FontSize::MEDIUM, TextAlign::LEFT, 
                               1.0, "score", dot_w, true, style.outline_color, 1, 2);
            cur_x += dot_w;
            past_dot = true;
        } else {
            FontSize fs = past_dot ? FontSize::MEDIUM : FontSize::LARGE;
            int cw = past_dot ? dec_char_w : int_char_w;
            int y_off = past_dot ? 5 : 0; // Align baselines roughly
            font_.DrawMonoText(renderer_, cur_x, y + y_off, char_str, c, fs, TextAlign::LEFT, 
                               1.0, "score", cw, true, style.outline_color, 1, 2);
            cur_x += cw + spacing;
        }
    }
}


std::string GameWindow::FormatMeter(double d) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%.1f", d);
    return std::string(buf);
}

void GameWindow::RenderHoldIndicators(int p, const NoteFieldConfig& cfg, const ActiveMods& mods) {
    if (p < 0 || p >= MAX_PLAYERS) return;
    auto& ps = players_[p];
    if (!ps.joined) return;

    bool effectively_down = cfg.downscroll ^ (cfg.reverse_pct > 0.5);

    int receptor_y = static_cast<int>(cfg.receptor_y);
    // Position indicators ABOVE the receptor line in downscroll (effective), BELOW in upscroll
    int base_y = effectively_down ? (receptor_y - 75) : (receptor_y + 75);

    for (const auto& hi : ps.hold_indicators) {
        int x = GetLaneX(hi.lane);
        int lane_w = GetLaneWidth();
        
        SDL_Texture* tex = nullptr;
        if (hi.grade == 0) tex = hold_good_texture_;
        else if (hi.grade == 1) tex = hold_ng_texture_;
        else if (hi.grade == 2) tex = hold_bad_texture_;

        if (tex) {
            double alpha_pct = hi.timer / 0.6;
            uint8_t alpha = static_cast<uint8_t>(255 * std::min(1.0, alpha_pct * 2.0));
            
            int offset_y = static_cast<int>((1.0 - alpha_pct) * 40.0 * (effectively_down ? -1 : 1));
            int draw_y = base_y + offset_y;

            int tw, th;
            SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
            double s = static_cast<double>(lane_w) / tw * 0.4; // Halved from 0.8
            int dw = static_cast<int>(tw * s);
            int dh = static_cast<int>(th * s);

            SDL_Rect dst = { x + (lane_w - dw) / 2, draw_y - dh / 2, dw, dh };
            SDL_SetTextureAlphaMod(tex, alpha);
            SDL_RenderCopy(renderer_, tex, nullptr, &dst);
            SDL_SetTextureAlphaMod(tex, 255);
        }
    }
}

} // namespace sml

#endif // HAS_SDL2
