// ============================================================================
// GameWindow.cpp — SDL2 test GUI with Song Select + Gameplay screens
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

    return true;
}

int GameWindow::ScanSongs(const std::string& songs_path) {
    int count = scanner_.ScanDirectory(songs_path);
    if (count > 0) {
        // Don't force SONG_SELECT here — let the Attraction screen handle transitions
        selected_song_ = 0;
        selected_chart_ = 0;
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

    for (auto& pair : jacket_cache_) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    jacket_cache_.clear();

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
    if (key == SDLK_BACKSLASH) {
        if (screen_ == ScreenState::SONG_SELECT || screen_ == ScreenState::GAMEPLAY) {
            showing_modifier_menu_ = !showing_modifier_menu_;
            if (showing_modifier_menu_) {
                modifier_menu_cursor_ = 0;
            } else {
                SaveSettings();
            }
            return;
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

    if (showing_modifier_menu_) {
        HandleKeyDown_ModifierMenu(key);
        return;
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

    switch (key) {
        case SDLK_ESCAPE:
            running_ = false;
            break;

        case SDLK_LEFT:
            selected_song_ = (selected_song_ - 1 + song_count) % song_count;
            selected_chart_ = 0;
            break;

        case SDLK_RIGHT:
            selected_song_ = (selected_song_ + 1) % song_count;
            selected_chart_ = 0;
            break;

        case SDLK_UP: {
            double now = SDL_GetTicks() / 1000.0;
            if (now - last_up_press_time_ < 0.35) {
                // Double tap UP -> Previous Chart (Visually Up / Easier)
                const auto& song = scanner_.GetSongs()[static_cast<size_t>(selected_song_)];
                int chart_count = static_cast<int>(song.charts.size());
                if (chart_count > 0)
                    selected_chart_ = (selected_chart_ - 1 + chart_count) % chart_count;
                last_up_press_time_ = 0.0;
            } else {
                last_up_press_time_ = now;
            }
            break;
        }

        case SDLK_DOWN: {
            double now = SDL_GetTicks() / 1000.0;
            if (now - last_down_press_time_ < 0.35) {
                // Double tap DOWN -> Next Chart (Visually Down / Harder)
                const auto& song = scanner_.GetSongs()[static_cast<size_t>(selected_song_)];
                int chart_count = static_cast<int>(song.charts.size());
                if (chart_count > 0)
                    selected_chart_ = (selected_chart_ + 1) % chart_count;
                last_down_press_time_ = 0.0;
            } else {
                last_down_press_time_ = now;
            }
            break;
        }

        case SDLK_LEFTBRACKET: {
            const auto& song = scanner_.GetSongs()[static_cast<size_t>(selected_song_)];
            int chart_count = static_cast<int>(song.charts.size());
            if (chart_count > 0)
                selected_chart_ = (selected_chart_ - 1 + chart_count) % chart_count;
            break;
        }

        case SDLK_RIGHTBRACKET: {
            const auto& song = scanner_.GetSongs()[static_cast<size_t>(selected_song_)];
            int chart_count = static_cast<int>(song.charts.size());
            if (chart_count > 0)
                selected_chart_ = (selected_chart_ + 1) % chart_count;
            break;
        }

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            StartGameplay(
                static_cast<size_t>(selected_song_),
                static_cast<size_t>(selected_chart_));
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
            if (lane >= 0 && playing_) {
                ProcessLaneHit(lane, -1.0, p);
            }
            return;
        }
    }

    // Legacy single-player fallback (when no players are joined or no match)
    if (input_.IsLaneKey(key)) {
        int lane = input_.OnKeyDown(key);
        if (lane >= 0 && playing_) {
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
            players_[p].input.OnKeyUp(key);
        }
    }
    input_.OnKeyUp(key);  // Legacy fallback
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
void GameWindow::HandleKeyDown_ModifierMenu(SDL_Keycode key) {
    const int NUM_ITEMS = 9;

    // The content of this function is replaced by HandleModifierMenuInput
    // and then HandleModifierMenuInput is called from here.
    HandleModifierMenuInput(key);
}

void GameWindow::HandleModifierMenuInput(SDL_Keycode key) {
    if (!showing_modifier_menu_) return;
    int num_items = 10; // Updated to 10 for the new item
    
    switch (key) {
        case SDLK_UP:
            modifier_menu_cursor_ = (modifier_menu_cursor_ - 1 + num_items) % num_items;
            break;
        case SDLK_DOWN:
            modifier_menu_cursor_ = (modifier_menu_cursor_ + 1) % num_items;
            break;
        case SDLK_LEFT: {
            switch (modifier_menu_cursor_) {
                case 0: { // Speed
                    if (field_config_.mod_type == ScrollModType::CMod)
                        field_config_.speed_mod = std::max(50.0, field_config_.speed_mod - 50.0);
                    else
                        field_config_.speed_mod = std::max(0.25, field_config_.speed_mod - 0.25);
                    players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
                } break;
                case 1: // Mod Type
                    field_config_.mod_type = (field_config_.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod;
                    field_config_.speed_mod = (field_config_.mod_type == ScrollModType::XMod) ? 2.0 : 400.0;
                    players_[active_player_idx_].field_config.mod_type = field_config_.mod_type;
                    players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
                    break;
                case 2: // Scroll
                    field_config_.downscroll = !field_config_.downscroll;
                    field_config_.receptor_y = field_config_.downscroll ? height_ * 0.85 : height_ * 0.15;
                    players_[active_player_idx_].field_config.downscroll = field_config_.downscroll;
                    players_[active_player_idx_].field_config.receptor_y = field_config_.receptor_y;
                    break;
                case 3: // Sudden+
                    sudden_plus_val_ = std::max(0.0f, sudden_plus_val_ - 0.05f);
                    break;
                case 4: // Hidden+
                    hidden_plus_val_ = std::max(0.0f, hidden_plus_val_ - 0.05f);
                    break;
                case 5: // Noteskin
                    if (!available_noteskins_.empty()) {
                        noteskin_index_ = (noteskin_index_ - 1 + (int)available_noteskins_.size()) % (int)available_noteskins_.size();
                        LoadNoteskin(available_noteskins_[noteskin_index_]);
                    }
                    break;
                case 6: // Effects
                    effect_mode_ = (effect_mode_ - 1 + 3) % 3;
                    break;
                case 7: { // Life Mode
                    int flare = life_meter_.GetFlareLevel();
                    if (life_meter_.GetType() == LifeType::STANDARD) life_meter_.Init(LifeType::FLARE, 5);
                    else if (life_meter_.GetType() == LifeType::LIFE4) life_meter_.Init(LifeType::STANDARD);
                    else if (life_meter_.GetType() == LifeType::RISKY) life_meter_.Init(LifeType::LIFE4);
                    else if (life_meter_.GetType() == LifeType::FLARE) {
                        if (flare > 1) life_meter_.Init(LifeType::FLARE, flare - 1);
                        else life_meter_.Init(LifeType::RISKY);
                    }
                } break;
                case 8: // Scoring
                    ex_mode_ = !ex_mode_;
                    break;
                case 9: // Center 1P
                    center_1p_ = !center_1p_;
                    SetupPlayerFieldLayout();
                    break;
                default: break;
            }
            break;
        }
        case SDLK_RIGHT:
        case SDLK_RETURN:
        case SDLK_KP_ENTER: {
            switch (modifier_menu_cursor_) {
                case 0: { // Speed
                    if (field_config_.mod_type == ScrollModType::CMod)
                        field_config_.speed_mod += 50.0;
                    else
                        field_config_.speed_mod += 0.25;
                    players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
                } break;
                case 1: // Mod Type
                    field_config_.mod_type = (field_config_.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod;
                    field_config_.speed_mod = (field_config_.mod_type == ScrollModType::XMod) ? 2.0 : 400.0;
                    players_[active_player_idx_].field_config.mod_type = field_config_.mod_type;
                    players_[active_player_idx_].field_config.speed_mod = field_config_.speed_mod;
                    break;
                case 2: // Scroll
                    field_config_.downscroll = !field_config_.downscroll;
                    field_config_.receptor_y = field_config_.downscroll ? height_ * 0.85 : height_ * 0.15;
                    players_[active_player_idx_].field_config.downscroll = field_config_.downscroll;
                    players_[active_player_idx_].field_config.receptor_y = field_config_.receptor_y;
                    break;
                case 3: // Sudden+
                    sudden_plus_val_ = std::min(0.8f, sudden_plus_val_ + 0.05f);
                    break;
                case 4: // Hidden+
                    hidden_plus_val_ = std::min(0.8f, hidden_plus_val_ + 0.05f);
                    break;
                case 5: // Noteskin
                    if (!available_noteskins_.empty()) {
                        noteskin_index_ = (noteskin_index_ + 1) % (int)available_noteskins_.size();
                        LoadNoteskin(available_noteskins_[noteskin_index_]);
                    }
                    break;
                case 6: // Effects
                    effect_mode_ = (effect_mode_ + 1) % 3;
                    break;
                case 7: { // Life Mode
                    int flare = life_meter_.GetFlareLevel();
                    if (life_meter_.GetType() == LifeType::STANDARD) life_meter_.Init(LifeType::LIFE4);
                    else if (life_meter_.GetType() == LifeType::LIFE4) life_meter_.Init(LifeType::RISKY);
                    else if (life_meter_.GetType() == LifeType::RISKY) life_meter_.Init(LifeType::FLARE, 1);
                    else if (life_meter_.GetType() == LifeType::FLARE) {
                        if (flare < 5) life_meter_.Init(LifeType::FLARE, flare + 1);
                        else life_meter_.Init(LifeType::STANDARD);
                    }
                } break;
                case 8: // Scoring
                    ex_mode_ = !ex_mode_;
                    break;
                case 9: // Center 1P
                    center_1p_ = !center_1p_;
                    SetupPlayerFieldLayout();
                    break;
                default: break;
            }
            break;
        }
        case SDLK_ESCAPE:
        case SDLK_BACKSLASH:
            showing_modifier_menu_ = false;
            SaveSettings();
            break;
    }
    SaveSettings(); // Save settings after any change in the menu
}

void GameWindow::HandleKeyDown_Options(SDL_Keycode key) {
    if (is_rebinding_) {
        if (key == SDLK_ESCAPE) {
            is_rebinding_ = false;
            return;
        }
        // Bind key to the selected slot
        custom_binds_4k_[options_lane_cursor_][options_slot_cursor_] = { BindInfo::KEY, (int)key };
        is_rebinding_ = false;
        return;
    }

    switch (key) {
        case SDLK_UP:
            options_lane_cursor_ = (options_lane_cursor_ - 1 + 4) % 4;
            break;
        case SDLK_DOWN:
            options_lane_cursor_ = (options_lane_cursor_ + 1) % 4;
            break;
        case SDLK_LEFT:
            options_slot_cursor_ = (options_slot_cursor_ - 1 + 3) % 3;
            break;
        case SDLK_RIGHT:
            options_slot_cursor_ = (options_slot_cursor_ + 1) % 3;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            is_rebinding_ = true;
            break;
        case SDLK_BACKSPACE:
        case SDLK_DELETE:
            custom_binds_4k_[options_lane_cursor_][options_slot_cursor_] = { BindInfo::NONE, 0 };
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
             total_offset
         );
    }
}

void GameWindow::HandleControllerButtonDown(SDL_GameControllerButton button) {
    if (is_rebinding_) {
        // Bind button to the selected slot
        custom_binds_4k_[options_lane_cursor_][options_slot_cursor_] = { BindInfo::BUTTON, (int)button };
        is_rebinding_ = false;
        return;
    }
    
    int lane = input_.OnButtonDown(button);
    if (lane >= 0 && playing_ && screen_ == ScreenState::GAMEPLAY) {
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
    if (chart_index >= active_simfile_->charts.size()) chart_index = 0;
    selected_chart_ = static_cast<int>(chart_index);
    current_chart_ = &active_simfile_->charts[chart_index];

    // --- Routine/Pseudo-Routine Handling ---
    bool is_routine = (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple");
    
    // Pseudo-routine check: if dance-double but description/credit contains keywords
    std::string desc_lower = current_chart_->description;
    std::string cred_lower = current_chart_->credit;
    std::string title_lower = active_simfile_->title;
    std::transform(desc_lower.begin(), desc_lower.end(), desc_lower.begin(), ::tolower);
    std::transform(cred_lower.begin(), cred_lower.end(), cred_lower.begin(), ::tolower);
    std::transform(title_lower.begin(), title_lower.end(), title_lower.begin(), ::tolower);

    bool has_routine_keyword = (desc_lower.find("routine") != std::string::npos || 
                                desc_lower.find("couple") != std::string::npos ||
                                desc_lower.find("gloves") != std::string::npos ||
                                desc_lower.find("glove") != std::string::npos ||
                                cred_lower.find("routine") != std::string::npos ||
                                cred_lower.find("couple") != std::string::npos);

    // Heuristic: if dance-double and has 2s or 4s but zero 1s
    bool uses_pseudo_types = false;
    bool has_standard_taps = false;
    if (current_chart_->chart_type == "dance-double") {
        int count_2 = 0;
        int count_4 = 0;
        int count_1 = 0;
        for (const auto& row : current_chart_->note_rows) {
            for (auto nt : row.columns) {
                if (nt == NoteType::Tap) count_1++;
                else if (nt == NoteType::HoldHead) count_2++;
                else if (nt == NoteType::RollHead) count_4++;
            }
        }
        if ((count_2 > 0 || count_4 > 0) && count_1 == 0) uses_pseudo_types = true;
        if (count_1 > 0) has_standard_taps = true;
    }

    if (current_chart_->chart_type == "dance-double" && (has_routine_keyword || uses_pseudo_types)) {
        is_routine = true;
    }

    // copy to runtime_chart_ for potential modification
    runtime_chart_ = *current_chart_;
    bool is_pseudo_routine = (current_chart_->chart_type == "dance-double" && (has_routine_keyword || uses_pseudo_types));

    if (is_routine || is_pseudo_routine) {
        // Force type for downstream logic
        if (runtime_chart_.chart_type == "dance-double") {
             runtime_chart_.chart_type = "dance-routine"; 
        }

        // Convert notes logic
        for (auto& row : runtime_chart_.note_rows) {
            for (size_t c = 0; c < row.columns.size(); ++c) {
                NoteType& nt = row.columns[c];
                if (is_pseudo_routine) {
                    // Pseudo-routine specific: convert holds/rolls to taps
                    if (nt == NoteType::HoldHead) {
                        nt = NoteType::TapP1;
                    } else if (nt == NoteType::RollHead) {
                        nt = NoteType::TapP2;
                    } else if (nt == NoteType::Tap) {
                        nt = (c < 4) ? NoteType::TapP1 : NoteType::TapP2;
                    } else if (nt == NoteType::HoldTail) {
                        nt = NoteType::None;
                    }
                } else {
                    // Actual Routine/Couple: Keep holds/rolls/lifts, but force color if it's a standard Tap
                    if (nt == NoteType::Tap) {
                        nt = (c < 4) ? NoteType::TapP1 : NoteType::TapP2;
                    }
                }
            }
        }
    }
    current_chart_ = &runtime_chart_;

    double total_offset = active_simfile_->GetEffectiveOffset(*current_chart_) + audio_offset_;
    conductor_.Initialize(
        active_simfile_->GetEffectiveBPMs(*current_chart_),
        active_simfile_->GetEffectiveStops(*current_chart_),
        active_simfile_->GetEffectiveScrolls(*current_chart_),
        total_offset
    );

    loaded_simfile_.reset();
    
    // Transition to DECIDE first with 7s timer
    decide_timer_ = 7.0; 
    ChangeScreen(ScreenState::DECIDE);

    std::printf("Playing: %s - %s [%s %s %d]\n",
        active_simfile_->artist.c_str(),
        active_simfile_->title.c_str(),
        current_chart_->chart_type.c_str(),
        current_chart_->difficulty_name.c_str(),
        current_chart_->difficulty_meter);
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
    hit_flashes_.clear();
    hit_history_.clear();
    results_ex_mode_ = false;
    autoplay_ = false;
    std::memset(normal_judge_counts_, 0, sizeof(normal_judge_counts_));
    std::memset(ex_judge_counts_, 0, sizeof(ex_judge_counts_));

    // 2. Load Assets
    audio_loaded_ = TryLoadSongAudio();
    
    if (bg_texture_) {
        SDL_DestroyTexture(bg_texture_);
        bg_texture_ = nullptr;
    }
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
        // Find timings
        for (const auto& row : current_chart_->note_rows) {
            bool has_hittable = false;
            for (auto nt : row.columns) {
                if (IsTap(nt)) {
                    total_hittable_notes_++;
                    has_hittable = true;
                }
            }
            if (has_hittable) {
                if (first_note_beat < 0.0) first_note_beat = row.beat;
                if (row.beat > last_note_beat) last_note_beat = row.beat;
            }
        }

        double total_offset = active_simfile_->GetEffectiveOffset(*current_chart_) + audio_offset_;
        conductor_.Initialize(
            active_simfile_->GetEffectiveBPMs(*current_chart_),
            active_simfile_->GetEffectiveStops(*current_chart_),
            active_simfile_->GetEffectiveScrolls(*current_chart_),
            total_offset
        );

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
        row_best_error_.assign(current_chart_->note_rows.size(), 0.0);

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
        input_.Configure(current_chart_->chart_type, current_chart_->num_columns);
        
        // Apply custom bindings if we are in 4-key mode
        if (current_chart_->num_columns == 4) {
            std::vector<KeyBinding> kb;
            std::unordered_map<SDL_GameControllerButton, int> bb;
            
            for (int l = 0; l < 4; ++l) {
                for (int s = 0; s < 3; ++s) {
                    const auto& b = custom_binds_4k_[l][s];
                    if (b.type == BindInfo::KEY) {
                        kb.push_back({ (SDL_Keycode)b.id, l });
                    } else if (b.type == BindInfo::BUTTON) {
                        bb[(SDL_GameControllerButton)b.id] = l;
                    }
                }
            }
            if (!kb.empty()) input_.SetBindings(kb);
            if (!bb.empty()) input_.SetButtonBindings(bb);
        }

        // 9. Configure per-player InputMappers for 2P mode
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (!players_[p].joined) continue;
            players_[p].input.ConfigureForPlayer(
                current_chart_->chart_type, current_chart_->num_columns, p);
            players_[p].ResetGameplay();
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
    loaded_simfile_.reset();
    input_.Reset();

    // Recalculate and save profiles to ensure rating is current
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (players_[p].joined) {
            players_[p].profile.RecalculateRating();
            SaveActiveProfile(p);
        }
    }
}

void GameWindow::OnEnterSongSelect() {
    if (scanner_.GetSongs().empty()) return;
    // This function is intended to be called when entering the song select screen.
    // The previous code here was for starting gameplay, which is incorrect for an "OnEnterSongSelect" handler.
    // If there's specific initialization needed for song select, it should go here.
    // For now, it's left empty as per the instruction's implied context.
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
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s %s %d",
            current_chart_->chart_type.c_str(),
            current_chart_->difficulty_name.c_str(),
            current_chart_->difficulty_meter);
        results_chart_info_ = buf;

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

        std::string score_key = scanner_.GetSongs()[static_cast<size_t>(selected_song_)].filepath + "|" + std::to_string(selected_chart_);
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

        font_.DrawText(renderer_, 40 + x_off, 30, "RESULTS", {255, 255, 255, a}, FontSize::TITLE, TextAlign::LEFT);
        
        int y_title = 30;
        font_.DrawText(renderer_, width_ - 40 - x_off, y_title, results_title_, {200, 200, 220, a}, FontSize::MEDIUM, TextAlign::RIGHT, 1.0, "score");
        font_.DrawText(renderer_, width_ - 40 - x_off, y_title + 25, results_chart_info_, {150, 150, 170, a}, FontSize::SMALL, TextAlign::RIGHT);
    }

    // --- 2P-friendly Columns ---
    if (results_reveal_timer_ > 0.2) {
        // Player 1 Column (Left Half)
        RenderResultsPanel(20, 100, width_ / 2 - 40, height_ - 150, hit_history_);
        
        // Player 2 Column (Placeholder)
        // DrawRect(width_ / 2 + 20, 100, width_ / 2 - 40, height_ - 150, {30, 30, 50, 100});
        font_.DrawText(renderer_, width_ * 0.75, 100, "[P2 SLOT EMPTY]", {100, 100, 120, 100}, FontSize::MEDIUM, TextAlign::CENTER);
    }

    // --- Footer ---
    if (results_reveal_timer_ > 4.5) {
        double footer_alpha = std::min(1.0, (results_reveal_timer_ - 4.5) / 0.5);
        uint8_t a = static_cast<uint8_t>(255 * footer_alpha);
        font_.DrawText(renderer_, width_ / 2, height_ - 30, "PRESS [ENTER] OR [ESC] TO CONTINUE", {150, 150, 170, a}, FontSize::SMALL, TextAlign::CENTER);
    }
}

void GameWindow::RenderResultsPanel(int x, int y, int w, int h, const std::vector<HitRecord>& hits) {
    // 1. Calculations & Dynamic Re-judging
    // Recalculate stats based on current mode (results_ex_mode_)
    // Use local variables for display, do not modify member variables directly here if they are persistent session stats (unless expected).
    // Original code used members normal_score_, ex_score_, etc. Let's recalculate them for display purposes to support toggling.
    
    double d_normal_score = 0;
    double d_ex_score = 0;
    int total_notes = total_hittable_notes_;
    
    // Stats counters (for display)
    int counts[9] = {0};
    int fast_count = 0, slow_count = 0;
    double error_sum = 0.0, error_sq_sum = 0.0;
    int valid_hits = 0;
    // int max_combo = 0; // If we want to recalculate max combo too? Maybe just keep stored one. Stored one is fine.

    // Re-judge
    for (const auto& hit : hits) {
        if (hit.judge == Judgement::MISS) {
            if (results_ex_mode_) counts[8]++; else counts[8]++; // Increment miss for display
            continue;
        }
        
        // 1. Calculate Normal Score
        Judgement j_norm = sml::ClassifyHit(std::abs(hit.error), false);
        d_normal_score += sml::JudgeWeight(j_norm, false);

        // 2. Calculate EX Score
        Judgement j_ex = sml::ClassifyHit(std::abs(hit.error), true);
        d_ex_score += sml::JudgeWeight(j_ex, true);

        // 3. Update Display Counts (Based on Mode)
        Judgement j_display = results_ex_mode_ ? j_ex : j_norm;
        
        int idx = -1;
        switch (j_display) {
            case Judgement::PEXTRA: idx=0; break;
            case Judgement::PCRIT: idx=1; break;
            case Judgement::PERFECT: idx=2; break; 
            case Judgement::PERFECT_LOW: idx=3; break;
            case Judgement::GREAT_HIGH: idx=4; break;
            case Judgement::GREAT: idx=5; break;
            case Judgement::GREAT_LOW: idx=6; break;
            case Judgement::GOOD: idx=7; break;
            case Judgement::MISS: idx=8; break;
            default: break;
        }
        
        if (idx >= 0) {
            counts[idx]++;
            valid_hits++;
            error_sum += hit.error;
            error_sq_sum += (hit.error * hit.error);
            if (hit.error < -0.003) fast_count++;
            else if (hit.error > 0.003) slow_count++;
        }
    }
    // Add missing misses to counts
    if ((int)hits.size() < total_notes) counts[8] += (total_notes - (int)hits.size());

    // Recalculate percentages
    // Normal Score is traditionally Percentage of max points. 
    // Simplified: Just use the stored members for SCORES, but update COUNTS.
    // Wait, if we toggle EX mode, the score numbers normally don't change in some games, but in others they do.
    // Sm-legends seems to track them separately.
    // Let's use the stored scores for now to avoid logic drift, but use the re-calculated counts.
    double normal_acc = (total_notes > 0) ? (d_normal_score / total_notes) : 0.0;
    double ex_acc     = (total_notes > 0) ? (d_ex_score / total_notes) : 0.0;

    // --- Determine Main vs Secondary Display ---
    bool show_ex_primary = results_ex_mode_;
    
    // --- Animation Logic ---
    double t = results_reveal_timer_ - 0.2; // Start after 0.2s of panel reveal (delay from RenderResults)
    if (t < 0) t = 0;
    
    double acc_progress = std::min(1.0, t / 1.0); // 1.0s duration for score rise
    // Ease out quadratic for smoother feel
    acc_progress = 1.0 - (1.0 - acc_progress) * (1.0 - acc_progress);

    double anim_normal_acc = normal_acc * acc_progress;
    double anim_ex_acc     = ex_acc * acc_progress;

    // Helper to get Texture/Color/Text for display
    struct DisplayInfo {
        SDL_Texture* icon;
        std::string text_grade; // Fallback
        Color color;
        double value;
        bool is_percentage; // Format as % or raw
    };

    // 1. Prepare Normal Info
    DisplayInfo info_normal;
    info_normal.value = anim_normal_acc;
    info_normal.is_percentage = true;
    {
        std::string g = "D"; Color c = {150,150,150,255};
        if (clear_type_ == ClearType::FAIL) { g="E"; c={255,40,40,255}; }
        else if (normal_acc >= 100.9) { g="SSS+"; c={255,255,255,255}; }
        else if (normal_acc >= 100.75){ g="SSS"; c={255,255,240,255}; }
        else if (normal_acc >= 100.5) { g="SS+"; c={255,255,200,255}; }
        else if (normal_acc >= 100.0) { g="SS"; c={255,255,100,255}; }
        else if (normal_acc >= 99.0)  { g="S+"; c={255,240,100,255}; }
        else if (normal_acc >= 97.5)  { g="S"; c={255,220,100,255}; }
        else if (normal_acc >= 95.0)  { g="AAA"; c={220,200,255,255}; }
        else if (normal_acc >= 92.5)  { g="AA"; c={180,180,255,255}; }
        else if (normal_acc >= 90.0)  { g="A"; c={150,150,255,255}; }
        else if (normal_acc >= 80.0)  { g="BBB"; c={150,255,150,255}; }
        else if (normal_acc >= 70.0)  { g="BB"; c={100,255,100,255}; }
        else if (normal_acc >= 60.0)  { g="B"; c={50,255,50,255}; }
        else if (normal_acc >= 50.0)  { g="C"; c={255,150,50,255}; }
        
        info_normal.text_grade = g;
        info_normal.color = c;
        info_normal.icon = nullptr;
        // Try texture
        std::string key = g;
        size_t plus = key.find('+');
        if (plus != std::string::npos) key.replace(plus, 1, "plus");
        if (normal_grade_textures_.count(key)) info_normal.icon = normal_grade_textures_[key];
    }

    // 2. Prepare EX Info
    DisplayInfo info_ex;
    info_ex.value = anim_ex_acc;
    info_ex.is_percentage = true; 
    {
        int stars = 0; Color c = {100,100,100,255};
        if (ex_acc >= 100.0)     { stars=6; c={255,215,0,255}; }
        else if (ex_acc >= 95.0) { stars=5; c={255,255,255,255}; }
        else if (ex_acc >= 88.5) { stars=4; c={100,255,100,255}; }
        else if (ex_acc >= 82.0) { stars=3; c={100,100,255,255}; }
        else if (ex_acc >= 73.0) { stars=2; c={255,100,255,255}; }
        else if (ex_acc >= 60.0) { stars=1; c={255,60,60,255}; }
        
        info_ex.text_grade = "";
        for(int i=0;i<stars;++i) info_ex.text_grade += "* ";
        info_ex.color = c;
        info_ex.icon = nullptr;
        if (stars > 0 && ex_grade_textures_.count(stars)) info_ex.icon = ex_grade_textures_[stars];
    }

    DisplayInfo main_info = show_ex_primary ? info_ex : info_normal;
    DisplayInfo sec_info  = show_ex_primary ? info_normal : info_ex;

    // 2. Layout & Spacing
    int px = static_cast<int>(w * 0.1); 
    int content_w = w - (px * 2);
    int left_x = x + px;
    int right_x = x + w - px;
    
    int cur_y = y + static_cast<int>(h * 0.05);

    // --- MAIN DISPLAY (Large) ---
    // Icon on Left, Score on Right
    if (acc_progress > 0.5) {
        if (main_info.icon) {
            int tw, th;
            SDL_QueryTexture(main_info.icon, nullptr, nullptr, &tw, &th);
            double scale = 1.0;
            if (th > 0) scale = 100.0 / th; // Max height 100
            int dw = (int)(tw * scale);
            int dh = (int)(th * scale);
            SDL_Rect r = { left_x, cur_y, dw, dh };
            SDL_RenderCopy(renderer_, main_info.icon, nullptr, &r);
            
            // Score next to it (Outline=true, Bold=true, Resolution=GIANT)
            // High-res fix: Use Giant font (120px) and scale down slightly (0.8) for crisp 100px height
            // Adjusted thickness/weight to match Gameplay (HUGE 54px @ weight 1/outline 2 -> GIANT 120px @ weight 2/outline 5)
            font_.DrawAccuracy(renderer_, left_x + dw + 30, cur_y + dh/2 - 20, main_info.value, {255, 255, 255, 255}, TextAlign::LEFT, 0.8, main_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
        } else {
            // Fallback text
            font_.DrawText(renderer_, left_x + 60, cur_y + 40, main_info.text_grade, main_info.color, FontSize::HUGE, TextAlign::CENTER);
            font_.DrawAccuracy(renderer_, left_x + 180, cur_y + 30, main_info.value, {255, 255, 255, 255}, TextAlign::LEFT, 0.7, main_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
        }
    } else {
        // Just rising accuracy
        font_.DrawAccuracy(renderer_, left_x + 60, cur_y + 30, main_info.value, {255, 255, 255, 255}, TextAlign::LEFT, 0.7, main_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
    }
    
    cur_y += 110;

    // --- SECONDARY DISPLAY (Small) ---
    // Icon on Left, Score on Right (Same layout, smaller)
    if (acc_progress > 0.3) {
        if (sec_info.icon) {
            int tw, th;
            SDL_QueryTexture(sec_info.icon, nullptr, nullptr, &tw, &th);
            double scale = 0.6; // Smaller
            int dw = (int)(tw * scale);
            int dh = (int)(th * scale);
            SDL_Rect r = { left_x, cur_y, dw, dh };
            SDL_RenderCopy(renderer_, sec_info.icon, nullptr, &r);
            
            // Secondary Score (Outline=true, Bold=true, High-Res GIANT)
            font_.DrawAccuracy(renderer_, left_x + dw + 20, cur_y + dh/2 - 10, sec_info.value, {200, 200, 255, 255}, TextAlign::LEFT, 0.5, sec_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
        } else {
            font_.DrawText(renderer_, left_x + 30, cur_y + 10, sec_info.text_grade, sec_info.color, FontSize::MEDIUM, TextAlign::CENTER);
            font_.DrawAccuracy(renderer_, left_x + 100, cur_y + 5, sec_info.value, {200, 200, 255, 255}, TextAlign::LEFT, 0.4, sec_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
        }
    } else {
        font_.DrawAccuracy(renderer_, left_x + 100, cur_y + 5, sec_info.value, {200, 200, 255, 255}, TextAlign::LEFT, 0.4, sec_info.is_percentage ? 4 : 2, true, true, {0,0,0,0}, FontSize::GIANT, FontSize::LARGE, 2, 5);
    }

    
    // --- Rating Reveal ---
    if (current_chart_ && t > 1.2) {
        // ... (Rating calculation same as before) ...
        double diff = current_chart_->custom_difficulty;
        double score_val = normal_acc * 10000.0; 
        double bonus = 0.0;
        if (score_val >= 1007500.0) bonus = 2.0 + (score_val - 1007500.0) * 0.0001;
        else if (score_val >= 1005000.0) bonus = 1.5 + (score_val - 1005000.0) * 0.0002;
        else if (score_val >= 1000000.0) bonus = 1.0 + (score_val - 1000000.0) * 0.0001;
        else if (score_val >= 975000.0) bonus = (score_val - 975000.0) * 0.00004;
        else bonus = (score_val - 975000.0) / 15000.0;
        
        double rating = std::max(0.0, diff + bonus);

        // Animate rating rising as well?
        double rating_progress = std::min(1.0, (t - 1.2) / 0.5);
        double anim_rating = rating * rating_progress;

        // Reformat rating: Label above, large numbers below
        font_.DrawText(renderer_, right_x, cur_y + 60, "RATING", {150, 200, 200, 255}, FontSize::SMALL, TextAlign::RIGHT);

        char rate_val_buf[16];
        std::snprintf(rate_val_buf, sizeof(rate_val_buf), "%.2f", anim_rating);
        std::string full_rate(rate_val_buf);
        size_t dot_pos = full_rate.find('.');
        if (dot_pos != std::string::npos) {
            std::string int_part = full_rate.substr(0, dot_pos);
            std::string dec_part = full_rate.substr(dot_pos);
            
            int digitW = font_.GetTextWidth("0", FontSize::HUGE, "score");
            int int_w = static_cast<int>(int_part.length() * digitW);
            
            // Draw integer part (larger)
            font_.DrawMonoText(renderer_, right_x - font_.GetTextWidth(dec_part, FontSize::LARGE, "score"), cur_y + 80, int_part, {80, 255, 255, 255}, FontSize::HUGE, TextAlign::RIGHT, 1.0, "score", -1, true, {0, 0, 0, 0}, 2, 5);
            // Draw decimal part (smaller)
            font_.DrawMonoText(renderer_, right_x, cur_y + 95, dec_part, {80, 255, 255, 255}, FontSize::LARGE, TextAlign::RIGHT, 1.0, "score", -1, true, {0, 0, 0, 0}, 1, 3);
        } else {
            font_.DrawMonoText(renderer_, right_x, cur_y + 80, full_rate, {80, 255, 255, 255}, FontSize::HUGE, TextAlign::RIGHT, 1.0, "score", -1, true, {0, 0, 0, 0}, 2, 5);
        }
    }

    cur_y += 120; // Increased spacing to push stats down

    // --- Judgement Stats (Staggered) ---
    auto DrawStatRow = [&](const char* label, int count, Color c, int& ry, double delay) {
        if (t < delay) return;
        
        double row_t = t - delay;
        double row_progress = std::min(1.0, row_t / 0.3);
        int anim_count = static_cast<int>(count * row_progress);
        
        font_.DrawText(renderer_, left_x, ry, label, c, FontSize::MEDIUM); // Bigger font
        char buf[16]; std::snprintf(buf, sizeof(buf), "%d", anim_count);
        font_.DrawText(renderer_, right_x, ry, buf, {255,255,255,255}, FontSize::MEDIUM, TextAlign::RIGHT);
        ry += 32; // More vertical spacing for bigger font
    };

    int ry = cur_y;
    double base_delay = 1.8;
    double step = 0.1;

    // Fix: Hide P-EXTRA if not in EX mode (index shifted if hidden)
    if (results_ex_mode_) {
        DrawStatRow("P-EXTRA", counts[0], GetJudgementColor(Judgement::PEXTRA), ry, base_delay);
        base_delay += step;
    }
    DrawStatRow("P-CRITICAL", counts[1], GetJudgementColor(Judgement::PCRIT), ry, base_delay); base_delay += step;
    DrawStatRow("PERFECT", counts[2] + counts[3], GetJudgementColor(Judgement::PERFECT), ry, base_delay); base_delay += step;
    DrawStatRow("GREAT", counts[4] + counts[5] + counts[6], GetJudgementColor(Judgement::GREAT), ry, base_delay); base_delay += step;
    DrawStatRow("GOOD", counts[7], GetJudgementColor(Judgement::GOOD), ry, base_delay); base_delay += step;
    DrawStatRow("MISS", counts[8], GetJudgementColor(Judgement::MISS), ry, base_delay); base_delay += step;

    if (t > base_delay) {
        ry += 15;
        double mean = (valid_hits > 0) ? (error_sum / valid_hits) : 0.0;
        double variance = (valid_hits > 0) ? (error_sq_sum / valid_hits) - (mean * mean) : 0.0;
        double sd = (variance > 0.0) ? std::sqrt(variance) : 0.0;

        char timing_buf[64];
        std::snprintf(timing_buf, sizeof(timing_buf), "Mean: %.2fms  SD: %.2fms", mean * 1000.0, sd * 1000.0);
        font_.DrawText(renderer_, left_x, ry, timing_buf, {180, 180, 200, 255}, FontSize::SMALL);
        
        ry += 30;
        
        // Early / Late with Colors & Spacing
        char early_buf[32]; std::snprintf(early_buf, sizeof(early_buf), "Early: %d", fast_count);
        char late_buf[32]; std::snprintf(late_buf, sizeof(late_buf), "Late: %d", slow_count);
        
        font_.DrawText(renderer_, left_x, ry, early_buf, {0, 255, 255, 255}, FontSize::SMALL); // Cyan
        font_.DrawText(renderer_, right_x, ry, late_buf, {255, 100, 50, 255}, FontSize::SMALL, TextAlign::RIGHT); // Orange/Red
    }

    // --- Offset Graph (Bottom of panel) ---
    if (t > base_delay + 0.5) {
        int graph_h = 80;
        int graph_y = y + h - graph_h - 10;
        RenderOffsetGraph(left_x, graph_y, content_w, graph_h, hits);
    }

}

void GameWindow::RenderOffsetGraph(int x, int y, int w, int h, const std::vector<HitRecord>& hits) {
    // 1. Background
    DrawRect(x, y, w, h, {0, 0, 0, 100});
    DrawRectOutline(x, y, w, h, {100, 100, 100, 255});

    // 2. Center line (0ms)
    // Y-Axis mapped: Top = -50ms (Early), Bottom = +50ms (Late)? 
    // Wait, user asked for: Flip Y-axis so it goes early->late when going down.
    // So Top=Early, Bottom=Late.
    int cy = y + h/2; // 0ms line
    DrawRect(x, cy, w, 1, {150, 150, 150, 255});

    // 3. Limit lines (+/- 45ms roughly?)
    // Let's say Graph Range is +/- 100ms
    double GRAPH_RANGE = 0.100; 

    // 4. Plot Dots
    double duration = (song_duration_ > 0.0) ? song_duration_ : 1.0;

    for (const auto& hit : hits) {
        if (hit.judge == Judgement::MISS) continue;

        double nx = hit.time / duration;
        if (nx < 0) nx = 0; if (nx > 1) nx = 1;
        
        // Error mapping: -0.1 -> Top, +0.1 -> Bottom
        // cy is 0.
        // y_offset = (error / range) * (h/2)
        double y_offset = (hit.error / GRAPH_RANGE) * (h/2);
        
        // Clamp
        if (y_offset < -h/2) y_offset = -h/2;
        if (y_offset > h/2) y_offset = h/2;

        int dot_x = x + static_cast<int>(nx * w);
        int dot_y = cy + static_cast<int>(y_offset);

        Color c = GetJudgementColor(hit.judge, results_ex_mode_);
        
        // Small 3x3 dot
        SDL_Rect r = {dot_x-1, dot_y-1, 3, 3};
        SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, 255);
        SDL_RenderFillRect(renderer_, &r);
    }
}

// ===================================

// ============================================================================
// Hit Detection
// ============================================================================

void GameWindow::ProcessLaneHit(int lane, double forced_time, int player_idx) {
    int actual_lane = lane;
    if (effect_mode_ == 1 && current_chart_) { // Mirror
        actual_lane = current_chart_->num_columns - 1 - lane;
    }

    if (!current_chart_ || !playing_) return;
    
    bool is_autoplay = (forced_time >= 0.0);
    double current_time = is_autoplay ? forced_time : play_time_;
    double best_error = 999.0;
    size_t best_idx = SIZE_MAX;

    // Search around the current hittable note index
    size_t search_start = (next_hittable_note_ > 5) ? next_hittable_note_ - 5 : 0;
    size_t search_end = std::min(current_chart_->note_rows.size(), next_hittable_note_ + 20);

    for (size_t i = search_start; i < search_end; ++i) {
        const auto& row = current_chart_->note_rows[i];
        // Check if this lane has a hittable note that hasn't been hit yet
        if (lane >= static_cast<int>(row.columns.size())) continue;
        if (IsNoteHit(i, lane)) continue; // Already hit this lane in this row
        NoteType nt = row.columns[static_cast<size_t>(lane)];
        if (!IsTap(nt)) continue;

        double note_time = conductor_.BeatToTime(row.beat);
        double error = current_time - note_time;
        double abs_error = std::fabs(error);

        double max_window = ex_mode_ ? JudgeWindowsEX::MISS : JudgeWindows::GOOD;

        // Only consider notes within the max timing window
        if (abs_error <= max_window && abs_error < std::fabs(best_error)) {
            best_error = error;
            best_idx = i;
        }
    }

    if (best_idx == SIZE_MAX) return; // No note found in window

    // Mark this lane as hit in the row
    note_hit_masks_[best_idx] |= (1 << actual_lane);
    
    // Track the timing error of this hit
    // For Solo: Judgement is based on the WORST (most inaccurate) note hit in the chord
    double abs_error_cur = std::fabs(best_error);
    double abs_error_prev = std::fabs(row_best_error_[best_idx]);
    
    if (abs_error_cur > abs_error_prev || row_best_error_[best_idx] == 999.0) {
        row_best_error_[best_idx] = best_error;
    }

    // Record per-lane visual feedback on the correct player's InputMapper
    Judgement j_visual = sml::ClassifyHit(abs_error_cur, ex_mode_);
    if (player_idx >= 0 && player_idx < MAX_PLAYERS && players_[player_idx].joined) {
        players_[player_idx].input.RecordHit(actual_lane, j_visual, best_error);
    } else {
        this->input_.RecordHit(actual_lane, j_visual, best_error);
    }

    // Record hit for stats
    hit_history_.push_back({current_time, best_error, j_visual, actual_lane, ex_mode_});

    // Calibration Sampling
    if (is_calibrating_ && j_visual != Judgement::MISS) {
        cal_errors_.push_back(best_error);
        if (cal_errors_.size() >= 24) {
            double sum = 0.0;
            for (double e : cal_errors_) sum += e;
            double mean = sum / cal_errors_.size();

            double sq_sum = 0.0;
            for (double e : cal_errors_) sq_sum += (e - mean) * (e - mean);
            calibration_stdev_ = std::sqrt(sq_sum / cal_errors_.size());

            // Suggested offset: if mean is positive (late), we need to reduce current offset
            suggested_offset_ = audio_offset_ - mean;
            cal_errors_.clear();
        }
    }

    // Spawn hit flash silhouette for this individual note
    double note_beat = current_chart_->note_rows[best_idx].beat;
    const auto& cfg = (player_idx >= 0 && player_idx < MAX_PLAYERS) 
        ? players_[player_idx].field_config 
        : field_config_;
    double note_y = NoteRenderer::GetYPosForBeat(note_beat, conductor_, cfg);
    hit_flashes_.push_back({actual_lane, note_beat, 0.3, note_y, j_visual});

    // --- Chord Cohesion: check if the entire row is now fully hit ---
    bool is_coop = (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple"));

    if (is_coop) {
        // Individual note scoring for co-op/routine: Score immediately!
        double abs_err = std::fabs(best_error);
        Judgement j_norm = ClassifyHit(abs_err, false);
        Judgement j_ex = ClassifyHit(abs_err, true);

        last_judgement_ = ex_mode_ ? j_ex : j_norm;
        last_timing_error_ = best_error;
        judgement_timer_ = 0.6;

        UpdateScores(abs_err, 1, player_idx);
        UpdateClearType(j_norm, j_ex, player_idx);

        if (j_norm != Judgement::MISS) {
            total_hits_++;
            combo_++;
            if (combo_ > max_combo_) max_combo_ = combo_;
            
            if (combo_ == 1) {
                lowest_judgement_in_combo_ = last_judgement_;
            } else if ((int)last_judgement_ > (int)lowest_judgement_in_combo_) {
                lowest_judgement_in_combo_ = last_judgement_;
            }

            combo_pop_timer_ = 0.15;
            if (combo_ > 0 && combo_ % 100 == 0) grade_popup_timer_ = 0.2;
        } else {
            combo_ = 0;
            lowest_judgement_in_combo_ = Judgement::NONE;
        }

        // Sync to per-player state
        if (player_idx >= 0 && player_idx < MAX_PLAYERS && players_[player_idx].joined) {
            auto& p = players_[player_idx];
            p.last_judgement = last_judgement_;
            p.last_timing_error = last_timing_error_;
            p.judgement_timer = judgement_timer_;
            if (j_norm != Judgement::MISS) {
                p.combo++;
                if (p.combo > p.max_combo) p.max_combo = p.combo;
                if (p.combo == 1) p.lowest_judgement_in_combo = p.last_judgement;
                else if ((int)p.last_judgement > (int)p.lowest_judgement_in_combo)
                    p.lowest_judgement_in_combo = p.last_judgement;
                p.combo_pop_timer = 0.15;
            } else {
                p.combo = 0;
                p.lowest_judgement_in_combo = Judgement::NONE;
            }
        }
    } else if (IsRowFullyHit(best_idx)) {
        // Standard Single-Player Logic: Judgement is based on the LAST note hit in the chord
        double abs_err = std::fabs(row_best_error_[best_idx]);
        Judgement j_norm = ClassifyHit(abs_err, false);
        Judgement j_ex = ClassifyHit(abs_err, true);

        last_judgement_ = ex_mode_ ? j_ex : j_norm;
        last_timing_error_ = row_best_error_[best_idx];
        judgement_timer_ = 0.6;

        int row_notes = GetRowHittableCount(best_idx);
        UpdateScores(abs_err, row_notes, player_idx);
        UpdateClearType(j_norm, j_ex, player_idx);

        if (j_norm != Judgement::MISS) {
            total_hits_++;
            combo_++;
            if (combo_ > max_combo_) max_combo_ = combo_;
            
            if (combo_ == 1) {
                lowest_judgement_in_combo_ = last_judgement_;
            } else if ((int)last_judgement_ > (int)lowest_judgement_in_combo_) {
                lowest_judgement_in_combo_ = last_judgement_;
            }

            combo_pop_timer_ = 0.15;
            if (combo_ > 0 && combo_ % 100 == 0) grade_popup_timer_ = 0.2;
        } else {
            total_miss_++;
            combo_ = 0;
            lowest_judgement_in_combo_ = Judgement::NONE;
        }

        // Sync to per-player state
        if (player_idx >= 0 && player_idx < MAX_PLAYERS && players_[player_idx].joined) {
            auto& p = players_[player_idx];
            p.last_judgement = last_judgement_;
            p.last_timing_error = last_timing_error_;
            p.judgement_timer = judgement_timer_;
            if (j_norm != Judgement::MISS) {
                p.combo++;
                if (p.combo > p.max_combo) p.max_combo = p.combo;
                if (p.combo == 1) p.lowest_judgement_in_combo = p.last_judgement;
                else if ((int)p.last_judgement > (int)p.lowest_judgement_in_combo)
                    p.lowest_judgement_in_combo = p.last_judgement;
                p.combo_pop_timer = 0.15;
                
                // Grade Milestones check per-player
                double acc = (p.total_hittable_notes > 0) ? (p.normal_score / p.total_hittable_notes) : 0.0;
                static const std::vector<double> thresholds = {
                    50.0, 60.0, 70.0, 80.0, 90.0, 92.5, 95.0, 97.5, 99.0, 100.0, 100.5, 100.75, 100.9
                };
                for (int i = static_cast<int>(thresholds.size()) - 1; i >= 0; --i) {
                    if (acc >= thresholds[i] && p.last_grade_milestone < (i + 1)) {
                        p.last_grade_milestone = i + 1;
                        p.grade_popup_timer = 0.2;
                        
                        // Also sync to global if this is the active player (for HUD display consistency)
                        if (player_idx == active_player_idx_) {
                            last_grade_milestone_ = p.last_grade_milestone;
                            grade_popup_timer_ = 0.2;
                        }
                        break;
                    }
                }

            } else {
                p.combo = 0;
                p.lowest_judgement_in_combo = Judgement::NONE;
            }
        }
    }

    // Advance next_hittable_note_ past fully-hit or non-hittable rows
    while (next_hittable_note_ < current_chart_->note_rows.size() &&
           (GetRowRequiredMask(next_hittable_note_) == 0 || IsRowFullyHit(next_hittable_note_))) {
        next_hittable_note_++;
    }
}

// Update
// ============================================================================

void GameWindow::ChangeScreen(ScreenState next) {
    if (is_transitioning_) return;
    next_screen_ = next;
    is_transitioning_ = true;
    transition_timer_ = 0.0;
}

void GameWindow::Update(double dt) {
    // --- Screen Transition Update ---
    if (is_transitioning_) {
        double prev_timer = transition_timer_;
        transition_timer_ += dt;
        double half_duration = transition_duration_ / 2.0;

        // Peak of transition (perfectly black)
        if (prev_timer < half_duration && transition_timer_ >= half_duration) {
            ScreenState old_screen = screen_;
            screen_ = next_screen_;
            
            // Special initialization logic when entering screens
            if (screen_ == ScreenState::RESULTS) {
                ShowResults(); // Populate stats
                results_reveal_timer_ = 0.0;
            } else if (screen_ == ScreenState::SONG_SELECT) {
                ReturnToSongSelect();
                playing_ = false;
                audio_.Stop();
            } else if (screen_ == ScreenState::CALIBRATION) {
                // When coming back from Gameplay Calibration
                playing_ = false;
                audio_.Stop();
                is_calibrating_ = false;
            }
        }

        if (transition_timer_ >= transition_duration_) {
            is_transitioning_ = false;
            transition_timer_ = 0.0;
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
                play_time_ += dt;
            }
        } else {
            // Even when not playing music (lead-in), we must advance time for the notefield
            play_time_ += dt;

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

        // Update per-player timers
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (players_[p].joined) {
                auto& ps = players_[p];
                if (ps.judgement_timer > 0.0) ps.judgement_timer = std::max(0.0, ps.judgement_timer - dt);
                if (ps.combo_pop_timer > 0.0) ps.combo_pop_timer = std::max(0.0, ps.combo_pop_timer - dt);
                if (ps.grade_popup_timer > 0.0) ps.grade_popup_timer = std::max(0.0, ps.grade_popup_timer - dt);
            }
        }

        // Update hit flash timers
        for (auto& hf : hit_flashes_) {
            hf.timer -= dt;
        }
        hit_flashes_.erase(
            std::remove_if(hit_flashes_.begin(), hit_flashes_.end(),
                [](const HitFlash& hf) { return hf.timer <= 0.0; }),
            hit_flashes_.end());

        // Auto-miss: check for rows that passed the miss window
        if (playing_ && current_chart_) {
            double current_time = play_time_;
            while (next_hittable_note_ < current_chart_->note_rows.size()) {
                const auto& row = current_chart_->note_rows[next_hittable_note_];
                uint32_t required = GetRowRequiredMask(next_hittable_note_);

                // Skip rows with no hittable notes (e.g. hold tails, fakes, mines)
                if (required == 0) {
                    next_hittable_note_++;
                    continue;
                }

                // Skip rows that are already fully hit
                if (IsRowFullyHit(next_hittable_note_)) {
                    next_hittable_note_++;
                    continue;
                }

                // Check if failed — begin fail sequence
                if (life_meter_.IsFailed() && !failed_sequence_) {
                    failed_sequence_ = true;
                    fail_animation_timer_ = 0.0;
                    clear_type_ = ClearType::FAIL;
                    playing_ = false;
                    audio_.Stop();
                    break;
                }

                double note_time = conductor_.BeatToTime(row.beat);

                // --- Autoplay Logic ---
                if (autoplay_ && playing_) {
                    double offset_time = play_time_;
                    // If we've reached or passed the note time, hit all notes in this row
                    if (offset_time >= note_time) {
                        // FIX: Use a stable index for the column loop because ProcessLaneHit 
                        // might advance next_hittable_note_ if the row finishes!
                        size_t row_idx = next_hittable_note_;
                        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                            NoteType nt = current_chart_->note_rows[row_idx].columns[col];
                            if (IsTap(nt) && !IsNoteHit(row_idx, col)) {
                                ProcessLaneHit(col, note_time);
                            }
                        }
                        // Pointer advancement is handled by ProcessLaneHit or the auto-miss/skip logic
                        continue; 
                    }
                }

                if (current_time - note_time > JudgeWindows::MISS) {
                    // Row missed - Apply to all active players who haven't hit this row
                    bool is_coop = (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple");
                    
                    int row_notes = GetRowHittableCount(next_hittable_note_);

                    for (int p = 0; p < MAX_PLAYERS; ++p) {
                        if (!players_[p].joined) continue;

                        auto& pstate = players_[p];
                        
                        // Check if this player already cleared this row (unlikely if we are here, but good safety)
                        // Actually, next_hittable_note_ is shared in single-player but might need per-player tracking 
                        // if we want true independence. For now, we assume next_hittable_note_ is the global cursor.
                        
                        if (is_coop) {
                            // In coop, we check individual columns
                            // But ProcessLaneHit logic suggests we just count unhit notes
                             int unhit_count = 0;
                            for (size_t c = 0; c < row.columns.size(); ++c) {
                                if (IsTap(row.columns[c]) && !IsNoteHit(next_hittable_note_, (int)c)) {
                                    unhit_count++;
                                }
                            }
                            if (unhit_count > 0) {
                                pstate.combo = 0;
                                pstate.total_miss += unhit_count;
                                UpdateScores(999.0, unhit_count, p);
                            }
                        } else {
                            // Standard: if the row passed and wasn't fully hit, it's a miss for this player
                            // WE ASSUME if we are here, the row was NOT fully hit (checked above)
                            // In standard mode, a row is "active" for all players. If it passes, it's a miss for everyone.
                            pstate.combo = 0;
                            pstate.total_miss++;
                            // Register miss
                            pstate.hit_history.push_back({note_time, 999.0, Judgement::MISS, -1, ex_mode_});
                            UpdateScores(999.0, row_notes, p);
                        }

                        // Trigger Miss judgement visual
                        pstate.last_judgement = Judgement::MISS;
                        pstate.judgement_timer = 0.6;
                        if (pstate.clear_type > ClearType::CLEAR) pstate.clear_type = ClearType::CLEAR; // Drop to Clear (or Fail if life dies)
                    }

                    // Also update legacy for safety/rendering if used elsewhere (though we should move away from it)
                    combo_ = 0;
                    total_miss_++;
                    last_judgement_ = Judgement::MISS;
                    judgement_timer_ = 0.6;
                    
                    next_hittable_note_++;
                } else {
                     break;
                }
            }

            // --- Mine detection: check for mines passing the receptor while key is held ---
            double current_beat = conductor_.GetCurrentBeat();
            // Search around current beat
            size_t m_start = (next_hittable_note_ > 20) ? next_hittable_note_ - 20 : 0;
            size_t m_end = std::min(current_chart_->note_rows.size(), next_hittable_note_ + 50);

            for (size_t i = m_start; i < m_end; ++i) {
                const auto& row = current_chart_->note_rows[i];
                if (row.beat > current_beat + 0.5) break; 
                if (row.beat < current_beat - 0.2) continue; // Passed window for hit

                for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                    if (row.columns[col] == NoteType::Mine && !IsNoteHit(i, col)) {
                        // Mine is near receptor. Is key held?
                        if (input_.GetLaneState(col).pressed) {
                            // MINE HIT!
                            note_hit_masks_[i] |= (1 << col);
                            life_meter_.OnMineHit();
                            // Visual feedback (optional flash/sound)
                        }
                    }
                }
            }

            // End-of-chart detection: all notes processed and past end time
            if (!chart_finished_ && !failed_sequence_ && chart_end_time_ > 0.0 &&
                next_hittable_note_ >= current_chart_->note_rows.size() &&
                play_time_ > chart_end_time_) {
                chart_finished_ = true;
                
                // Trigger Clear Type Animation for FC+
                if (clear_type_ >= ClearType::FULL_COMBO) {
                    clear_animation_timer_ = 3.0;
                    results_delay_ = 4.5; // Long delay to show animation (3s + 1.5s base)
                } else {
                    results_delay_ = 1.5; // Standard 1.5s delay
                }
            }

            // Countdown to results screen (normal end-of-chart)
            if (chart_finished_ && !failed_sequence_) {
                if (is_calibrating_) {
                    // Loop calibration chart
                    StartGameplayDirect();
                    audio_.Play(0.0);
                    playing_ = true;
                } else {
                    results_delay_ -= dt;
                    if (results_delay_ <= 0.0) {
                        ChangeScreen(ScreenState::RESULTS);
                    }
                }
            }

            // Fail sequence animation timer
            if (failed_sequence_) {
                fail_animation_timer_ += dt;
                if (fail_animation_timer_ >= 2.0) {
                    ChangeScreen(ScreenState::RESULTS);
                }
            }
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

    if (showing_modifier_menu_) {
        RenderModifierMenu();
    }

    // Polish transitions

    // --- Screen Transition Overlay ---
    if (is_transitioning_) {
        double alpha_factor = 0.0;
        double half_duration = transition_duration_ / 2.0;

        if (transition_timer_ < half_duration) {
            // Fading OUT (to black)
            alpha_factor = transition_timer_ / half_duration;
        } else {
            // Fading IN (from black)
            alpha_factor = 1.0 - ((transition_timer_ - half_duration) / half_duration);
        }

        int alpha = static_cast<int>(255 * alpha_factor);
        if (alpha > 0) {
            SDL_Rect screen_rect = { 0, 0, width_, height_ };
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer_, &screen_rect);
        }
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

    // P1 Start = Enter or Return
    if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
        JoinPlayer(0);
        ChangeScreen(ScreenState::PROFILE_LOAD);
        return;
    }

    // P2 Start = Numpad 0 or Right Shift
    if (key == SDLK_KP_0 || key == SDLK_RSHIFT) {
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

    // P1 Start (Enter) — join P1 if not joined, or confirm if joined
    if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
        if (!players_[0].joined) {
            JoinPlayer(0);
        } else {
            profile_load_state_[0].ready = true;
        }
    }

    // P2 Start (Numpad 0 / RShift) — join P2 if not joined, or confirm if joined
    if (key == SDLK_KP_0 || key == SDLK_RSHIFT) {
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
    // Dark blue/purple gradient background
    DrawRect(0, 0, width_, height_, {8, 8, 24, 255});
    
    // Pulsing overlay
    double pulse = 0.5 + 0.5 * std::sin(attraction_timer_ * 1.5);
    int overlay_alpha = static_cast<int>(20 * pulse);
    DrawRect(0, 0, width_, height_, {60, 40, 120, static_cast<uint8_t>(overlay_alpha)});

    // Title
    int center_x = width_ / 2;
    int title_y = height_ / 3;
    font_.DrawText(renderer_, center_x, title_y, "SM-LEGENDS",
        {255, 255, 255, 255}, FontSize::GIANT, TextAlign::CENTER);

    // Subtitle
    font_.DrawText(renderer_, center_x, title_y + 80, "STEP MANIA LEGENDS",
        {180, 180, 200, 200}, FontSize::LARGE, TextAlign::CENTER);

    // "Press Start" with pulsing alpha
    uint8_t press_alpha = static_cast<uint8_t>(120 + 135 * pulse);
    int press_y = height_ * 2 / 3;
    font_.DrawText(renderer_, center_x, press_y, "PRESS START",
        {255, 220, 100, press_alpha}, FontSize::TITLE, TextAlign::CENTER);

    // USB hint
    font_.DrawText(renderer_, center_x, press_y + 50,
        "or insert USB drive",
        {120, 120, 140, 180}, FontSize::SMALL, TextAlign::CENTER);

    // Version / Credits at bottom
    font_.DrawText(renderer_, center_x, height_ - 40,
        "v0.1 // SM-Legends",
        {80, 80, 100, 150}, FontSize::SMALL, TextAlign::CENTER);
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
            // Not joined — prompt
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
        players_[player_idx].profile.RecalculateRating();
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
                    // Center it for 1P centered mode or for 8-lane doubles
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
        // Two players: center each player's notefield in their half
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            players_[i].field_config = field_config_;
            int target_center = (i == 0) ? p1_center_side : p2_center_side;
            players_[i].field_x_offset = target_center - natural_center;
        }
    }
}

// ============================================================================
// Profile Management
// ============================================================================

void GameWindow::ScanLocalProfiles() {
    local_profile_names_ = Profile::ScanLocalProfiles("profiles");
}

void GameWindow::SaveActiveProfile(int player_idx) {
    if (player_idx < 0 || player_idx >= MAX_PLAYERS) return;
    auto& prof = players_[player_idx].profile;
    if (prof.is_guest || prof.source_path.empty()) return;

    // Sync current mods into profile before saving
    prof.speed_mod    = field_config_.speed_mod;
    prof.mod_type     = static_cast<int>(field_config_.mod_type);
    prof.downscroll   = field_config_.downscroll;
    prof.audio_offset = audio_offset_;
    prof.noteskin     = (noteskin_index_ < static_cast<int>(available_noteskins_.size()))
                          ? available_noteskins_[noteskin_index_] : "Default";
    prof.effect_mode  = effect_mode_;

    prof.Save(prof.source_path);
}

void GameWindow::ApplyProfileMods(int player_idx) {
    if (player_idx < 0 || player_idx >= MAX_PLAYERS) return;
    auto& prof = players_[player_idx].profile;
    
    // 1. Sync per-player field config with profile settings (guest or registered)
    auto& cfg = players_[player_idx].field_config;
    cfg.speed_mod = prof.speed_mod;
    cfg.mod_type  = static_cast<ScrollModType>(prof.mod_type);
    cfg.downscroll = prof.downscroll;
    // Calculate receptor_y for this player
    cfg.receptor_y = cfg.downscroll ? height_ * 0.85 : height_ * 0.15;

    // 2. Also sync global field_config_ if this is the primary player slot being used for setup
    if (player_idx == active_player_idx_) {
        field_config_ = cfg;
        audio_offset_ = prof.audio_offset;
        effect_mode_  = prof.effect_mode;
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
            // Select existing profile — load it
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

        Color text_col = selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 200};
        
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

    int cx = width_ / 2;
    font_.DrawText(renderer_, cx, 50, "KEY CONFIGURATION (4-KEY)", {255, 255, 255, 255}, FontSize::TITLE, TextAlign::CENTER);

    int table_x = cx - 350;
    int table_y = 180;
    int col_w = 200;
    int row_h = 70;

    auto GetKeyString = [](const BindInfo& b) -> std::string {
        if (b.type == BindInfo::NONE) return "---";
        if (b.type == BindInfo::KEY) return SDL_GetKeyName((SDL_Keycode)b.id);
        
        // Pad button names
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
            default: return "BTN " + std::to_string(b.id);
        }
    };

    const char* lane_names[] = {"LEFT", "DOWN", "UP", "RIGHT"};
    Color header_col = {150, 150, 170, 255};

    // Table Headers
    font_.DrawText(renderer_, table_x, table_y - 40, "LANE", header_col, FontSize::SMALL, TextAlign::LEFT);
    font_.DrawText(renderer_, table_x + col_w, table_y - 40, "SLOT 1", header_col, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, table_x + col_w * 2, table_y - 40, "SLOT 2", header_col, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, table_x + col_w * 3, table_y - 40, "SLOT 3", header_col, FontSize::SMALL, TextAlign::CENTER);

    for (int l = 0; l < 4; ++l) {
        int y = table_y + l * row_h;
        
        // Lane Label
        font_.DrawText(renderer_, table_x, y + 20, lane_names[l], {255, 255, 255, 255}, FontSize::MEDIUM, TextAlign::LEFT);

        for (int s = 0; s < 3; ++s) {
            int x = table_x + col_w * (s + 1);
            bool is_selected = (options_lane_cursor_ == l && options_slot_cursor_ == s);
            
            // Slot Background
            Color bg_col = is_selected ? Color{40, 40, 80, 255} : Color{20, 20, 30, 255};
            DrawRect(x - col_w/2 + 5, y, col_w - 10, row_h - 10, bg_col);
            
            if (is_selected) {
                // Pulsing selection outline
                uint8_t pulse = static_cast<uint8_t>(200 + 55 * std::sin(SDL_GetTicks() * 0.01));
                DrawRectOutline(x - col_w/2 + 5, y, col_w - 10, row_h - 10, {pulse, pulse, 100, 255});
            }

            const auto& b = custom_binds_4k_[l][s];
            std::string val = is_selected && is_rebinding_ ? "[ ? ]" : GetKeyString(b);
            Color val_col = (b.type == BindInfo::BUTTON) ? Color{100, 200, 255, 255} : Color{255, 255, 100, 255};
            if (b.type == BindInfo::NONE) val_col = {100, 100, 110, 255};

            font_.DrawText(renderer_, x, y + 20, val, val_col, FontSize::MEDIUM, TextAlign::CENTER);
        }
    }

    font_.DrawText(renderer_, cx, height_ - 80, "ARROWS: NAVIGATE | ENTER: REBIND | BACKSPACE: CLEAR", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, cx, height_ - 50, "ESC/F12: SAVE & EXIT", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
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
    RenderChartPanel();
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
            
            bool is_selected = (song_idx == selected_song_);

            // Jacket background
            DrawRect(x, y, jacket_size, jacket_size, {20, 20, 40, 255});

            // Load and Draw Jacket
            SDL_Texture* jacket = GetJacketTexture(song.jacket_path);
            if (jacket) {
                int jw, jh;
                SDL_QueryTexture(jacket, nullptr, nullptr, &jw, &jh);
                double aspect = static_cast<double>(jw) / jh;
                SDL_Rect dest;
                if (aspect > 1.0) {
                    int h = static_cast<int>(jacket_size / aspect);
                    dest = { x, y + (jacket_size - h) / 2, jacket_size, h };
                } else {
                    int w = static_cast<int>(jacket_size * aspect);
                    dest = { x + (jacket_size - w) / 2, y, w, jacket_size };
                }
                SDL_RenderCopy(renderer_, jacket, nullptr, &dest);
            } else {
                int cx = x + jacket_size / 2;
                int cy = y + jacket_size / 2;
                DrawRect(cx - 5, cy - 15, 8, 30, {60, 60, 80, 255});
                DrawRect(cx - 15, cy + 8, 18, 12, {60, 60, 80, 255});
                font_.DrawText(renderer_, cx, cy + 30, "NO IMAGE", {50, 50, 70, 255}, FontSize::SMALL, TextAlign::CENTER);
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

void GameWindow::RenderChartPanel() {
    const auto& songs = scanner_.GetSongs();
    if (songs.empty()) return;

    const auto& song = songs[static_cast<size_t>(selected_song_)];

    // Left Side Panel Layout (Arcade Style)
    int panel_left = 15; // Could be right-aligned if P2 is active in future
    int panel_width = 240; // Widened to accommodate Mode + Meter better
    int panel_top = 100;

    // 1. Difficulty List
    int y = panel_top;
    for (size_t ci = 0; ci < song.charts.size(); ++ci) {
        const auto& chart = song.charts[ci];
        bool selected = (static_cast<int>(ci) == selected_chart_);
        Color diff_col = GetDifficultyColor(chart.difficulty_name);
        
        if (chart.variant == ChartVariant::Wild) {
            diff_col = Color::Rainbow(SDL_GetTicks() / 1000.0, 0.7f, 0.6f);
        }

        int row_h = 32;
        if (selected) {
            DrawRect(panel_left - 10, y, panel_width + 10, row_h, {diff_col.r, diff_col.g, diff_col.b, 100});
            DrawRect(panel_left - 10, y, 5, row_h, diff_col);
            uint8_t flash = static_cast<uint8_t>(180 + 75 * std::sin(SDL_GetTicks() * 0.01));
            DrawRect(panel_left - 10, y, panel_width + 10, 1, {255, 255, 255, flash});
            DrawRect(panel_left - 10, y + row_h - 1, panel_width + 10, 1, {255, 255, 255, flash});
        }

        // --- Optimized Labeling ---
        std::string mode_name = GetChartModeName(chart.chart_type);
        bool is_routine = (chart.chart_type == "dance-routine" || chart.chart_type == "dance-couple");
        
        std::string label = mode_name;
        if (!is_routine) {
            if (chart.variant == ChartVariant::Wild) {
                label += " Wild";
            } else {
                label += " " + chart.difficulty_name;
            }
        }

        font_.DrawText(renderer_, panel_left, y + 6, label, 
                       selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 180}, 
                       FontSize::MEDIUM);
        
        // Grade next to the Meter
        std::string score_key = song.filepath + "|" + std::to_string(ci);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            font_.DrawText(renderer_, panel_left + panel_width - 55, y + 6, it->second.grade, {255, 255, 200, 255}, FontSize::SMALL, TextAlign::RIGHT);
        }

        if (chart.variant == ChartVariant::Wild) {
            font_.DrawText(renderer_, panel_left + panel_width - 10, y + 4, "狂", 
                           selected ? Color{255, 255, 255, 255} : diff_col, 
                           FontSize::LARGE, TextAlign::RIGHT);
        } else {
            char meter_buf[8];
            std::snprintf(meter_buf, sizeof(meter_buf), "%d", chart.custom_difficulty);
            font_.DrawText(renderer_, panel_left + panel_width - 10, y + 4, meter_buf, 
                           selected ? Color{255, 255, 255, 255} : diff_col, 
                           FontSize::LARGE, TextAlign::RIGHT);
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
    
    const auto& selected_c = song.charts[selected_chart_];
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
        font_.DrawText(renderer_, width_ / 2, 10, song.title, {255, 255, 255, 255}, FontSize::LARGE, TextAlign::CENTER, 1.0, "score");
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
    std::string chart_type = "";
    if (!songs.empty()) {
        const auto& song = songs[static_cast<size_t>(selected_song_)];
        if (selected_chart_ >= 0 && selected_chart_ < (int)song.charts.size()) {
            chart_type = song.charts[selected_chart_].chart_type;
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
        
        // Ratings (y=32)
        if (!prof.is_guest) {
            bool is_single = (chart_type == "dance-single");
            bool is_double = (chart_type == "dance-double");

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
        std::string score_key = song.filepath + "|" + std::to_string(selected_chart_);
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
    
    // Player 1 (Left)
    {
        int p1_x = 100;
        std::string mode_name = GetChartModeName(current_chart_->chart_type);
        bool is_routine = (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple");
        
        std::string diff_label = mode_name;
        if (!is_routine) diff_label += " " + current_chart_->difficulty_name;
        
        Color diff_col = GetDifficultyColor(current_chart_->difficulty_name);
        font_.DrawText(renderer_, p1_x, panel_y, diff_label, diff_col, FontSize::MEDIUM, TextAlign::LEFT);
        
        char meter_buf[16];
        std::snprintf(meter_buf, sizeof(meter_buf), "%d", current_chart_->difficulty_meter);
        font_.DrawText(renderer_, p1_x, panel_y + 35, meter_buf, {255, 255, 255, 255}, FontSize::LARGE, TextAlign::LEFT);

        // Best Score
        const auto& songs = scanner_.GetSongs();
        std::string score_key = songs[static_cast<size_t>(selected_song_)].filepath + "|" + std::to_string(selected_chart_);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            char sbuf[64];
            std::snprintf(sbuf, sizeof(sbuf), "BEST: %.2f%% (%s)", it->second.percentage, it->second.grade.c_str());
            font_.DrawText(renderer_, p1_x, panel_y + 80, sbuf, {220, 255, 220, 200}, FontSize::SMALL, TextAlign::LEFT);
        }
    }

    // Player 2 (Right) - Symmetric
    {
        int p2_x = width_ - 100;
        std::string mode_name = GetChartModeName(current_chart_->chart_type);
        bool is_routine = (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple");
        
        std::string diff_label = mode_name;
        if (!is_routine) diff_label += " " + current_chart_->difficulty_name;

        Color diff_col = GetDifficultyColor(current_chart_->difficulty_name);
        font_.DrawText(renderer_, p2_x, panel_y, diff_label, diff_col, FontSize::MEDIUM, TextAlign::RIGHT);
        
        char meter_buf[16];
        std::snprintf(meter_buf, sizeof(meter_buf), "%d", current_chart_->difficulty_meter);
        font_.DrawText(renderer_, p2_x, panel_y + 35, meter_buf, {255, 255, 255, 255}, FontSize::LARGE, TextAlign::RIGHT);

        // Best Score
        const auto& songs = scanner_.GetSongs();
        std::string score_key = songs[static_cast<size_t>(selected_song_)].filepath + "|" + std::to_string(selected_chart_);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            char sbuf[64];
            std::snprintf(sbuf, sizeof(sbuf), "BEST: %.2f%% (%s)", it->second.percentage, it->second.grade.c_str());
            font_.DrawText(renderer_, p2_x, panel_y + 80, sbuf, {220, 255, 220, 200}, FontSize::SMALL, TextAlign::RIGHT);
        }
    }

    // Prompt?
    font_.DrawText(renderer_, center_x, height_ - 50, "PREPARING...", {255, 255, 255, 128}, FontSize::SMALL, TextAlign::CENTER);
}

void GameWindow::RenderGameplay() {
    RenderBackground();

    // --- Per-player notefield rendering ---
    if (num_active_players_ >= 2) {
        // 2P mode: render each player's notefield separately
        for (int p = 0; p < MAX_PLAYERS; ++p) {
            if (!players_[p].joined) continue;
            render_x_offset_ = players_[p].field_x_offset;
            render_player_idx_ = p;

            // Clip to this player's half to prevent visual bleed
            SDL_Rect clip = { (p == 0) ? 0 : width_ / 2, 0, width_ / 2, height_ };
            SDL_RenderSetClipRect(renderer_, &clip);

            RenderLanes();
            RenderMeasureLines();
            RenderNotes();
            RenderReceptors();
            RenderHitFlashes();
            RenderJudgement();
            RenderTopBar();
            RenderLifeBar();
        }
        SDL_RenderSetClipRect(renderer_, NULL);
        render_x_offset_ = 0;
        render_player_idx_ = 0;

        // Shared HUD elements (progress bar, bottom bar, P1/P2 divider)
        RenderProgressBar();
        RenderBottomBar();

        // Divider line between P1 and P2
        DrawRect(width_ / 2 - 1, 0, 2, height_, {40, 40, 70, 200});
    } else {
        // 1P mode: render normally
        render_player_idx_ = active_player_idx_;
        render_x_offset_ = players_[active_player_idx_].field_x_offset;

        RenderLanes();
        RenderMeasureLines();
        RenderNotes();
        RenderReceptors();
        RenderHitFlashes();
        RenderJudgement();
        RenderHUD();
    }
    
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

void GameWindow::RenderBackground() {
    // Pure black background by default
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    if (bg_texture_) {
        int bw, bh;
        SDL_QueryTexture(bg_texture_, nullptr, nullptr, &bw, &bh);
        
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
        
        // Slightly dim the background image (Pulse removed as per request)
        int final_dim = 100;

        SDL_SetTextureColorMod(bg_texture_, final_dim, final_dim, final_dim);
        SDL_RenderCopy(renderer_, bg_texture_, nullptr, &dest);
    }
}

void GameWindow::RenderLanes() {
    // --- Screen Filter & borders removed as per request ---
    // (Transparent notefield)
}

void GameWindow::RenderReceptors() {
    if (!current_chart_) return;
    const auto& cfg = players_[render_player_idx_].field_config;
    int num_cols = current_chart_->num_columns;
    int lane_w = GetLaneWidth();
    int receptor_y = static_cast<int>(cfg.receptor_y);

    // --- Per-lane flash textures (kept from before) ---
    for (int i = 0; i < num_cols; ++i) {
        int x = GetLaneX(i);
        // Use per-player InputMapper if a player is being rendered, otherwise legacy
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
            
            int draw_w = lane_w;
            int draw_h = static_cast<int>(flash_tex_h_ * (static_cast<double>(lane_w) / flash_tex_w_));
            
            SDL_Rect dst;
            SDL_RendererFlip flip = SDL_FLIP_NONE;

            if (cfg.downscroll) {
                dst = { x, receptor_y - draw_h + 50, draw_w, draw_h };
                flip = SDL_FLIP_VERTICAL;
            } else {
                dst = { x, receptor_y - 50, draw_w, draw_h };
                flip = SDL_FLIP_NONE;
            }
            SDL_RenderCopyEx(renderer_, flash_texture_, nullptr, &dst, 0.0, nullptr, flip);
            SDL_SetTextureColorMod(flash_texture_, 255, 255, 255); // Reset
        }
    }

    // --- Arcade: Horizontal "Judgment Line" across all lanes" ---
    // Removed as per user request


    // --- SMX-style: Unified padding lines on left and right edges ---
    int field_left = GetFieldLeft();
    int field_right = GetFieldRight();
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

    // Left triangle (points right, toward the field) — sits to the left of left_pad
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

    // Right triangle (points left, toward the field) — sits to the right of right_pad
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

void GameWindow::RenderModifierMenu() {
    int field_center = (GetFieldLeft() + GetFieldRight()) / 2;
    int mx = field_center - 200;
    int my = height_ / 2 - 150;
    int mw = 400;
    int mh = 300;

    // Subdued background for the menu itself
    DrawRect(mx, my, mw, mh, {0, 0, 0, 180});
    DrawRectOutline(mx, my, mw, mh, {255, 255, 255, 255});
    font_.DrawText(renderer_, field_center, my + 10, "MODIFIERS", {255, 255, 255, 255}, FontSize::MEDIUM, TextAlign::CENTER);

    auto renderItem = [&](int idx, const std::string& label, const std::string& value) {
        int y = my + 50 + idx * 25;
        Color col = (modifier_menu_cursor_ == idx) ? Color{255, 255, 0, 255} : Color{200, 200, 200, 255};
        if (modifier_menu_cursor_ == idx) {
            font_.DrawText(renderer_, mx + 10, y, ">", col, FontSize::SMALL, TextAlign::LEFT);
        }
        font_.DrawText(renderer_, mx + 30, y, label, col, FontSize::SMALL, TextAlign::LEFT);
        font_.DrawText(renderer_, mx + mw - 10, y, value, col, FontSize::SMALL, TextAlign::RIGHT);
    };

    char buf[64];
    if (field_config_.mod_type == ScrollModType::CMod) std::snprintf(buf, sizeof(buf), "%d", (int)field_config_.speed_mod);
    else std::snprintf(buf, sizeof(buf), "%.2f", field_config_.speed_mod);
    renderItem(0, "Speed", buf);

    renderItem(1, "Mod Type", (field_config_.mod_type == ScrollModType::XMod) ? "X-Mod" : "C-Mod");
    renderItem(2, "Scroll", field_config_.downscroll ? "Downscroll" : "Upscroll");
    
    std::snprintf(buf, sizeof(buf), "%d%%", (int)(sudden_plus_val_ * 100));
    renderItem(3, "Sudden+", buf);
    
    std::snprintf(buf, sizeof(buf), "%d%%", (int)(hidden_plus_val_ * 100));
    renderItem(4, "Hidden+", buf);

    renderItem(5, "Noteskin", available_noteskins_[noteskin_index_]);

    const char* effect_names[] = {"None", "Mirror", "Random"};
    renderItem(6, "Effects", effect_names[effect_mode_]);

    std::string life_name = "Standard";
    if (life_meter_.GetType() == LifeType::LIFE4) life_name = "Life4";
    else if (life_meter_.GetType() == LifeType::RISKY) life_name = "Risky";
    else if (life_meter_.GetType() == LifeType::FLARE) {
        std::snprintf(buf, sizeof(buf), "Flare %d", life_meter_.GetFlareLevel());
        life_name = buf;
    }
    renderItem(7, "Life Mode", life_name);

    renderItem(8, "Scoring", ex_mode_ ? "EX Mode" : "Normal");
    renderItem(9, "Center 1P", center_1p_ ? "On" : "Off");
}

void GameWindow::RenderHitFlashes() {
    if (!note_texture_ || hit_flashes_.empty()) return;
    const auto& cfg = players_[render_player_idx_].field_config;

    int lane_w = GetLaneWidth();
    int fw = note_tex_w_ / 16;
    int fh = note_tex_h_ / 8;
    double note_scale = static_cast<double>(lane_w) / fw;
    int dh = static_cast<int>(fh * note_scale);

    for (const auto& hf : hit_flashes_) {
        double alpha_ratio = hf.timer / 0.3;  // 1.0 -> 0.0
        uint8_t alpha = static_cast<uint8_t>(alpha_ratio * 200);

        int x = GetLaneX(hf.lane);
        int yi = static_cast<int>(hf.hit_y);
        int qrow = GetQuantizationRow(hf.beat);
        int frame = 0;

        double angle = 0.0;
        if (hf.lane == 0) angle = 90.0;       // Left
        else if (hf.lane == 1) angle = 0.0;   // Down
        else if (hf.lane == 2) angle = 180.0; // Up
        else if (hf.lane == 3) angle = -90.0; // Right

        SDL_Rect src = { frame * fw, qrow * fh, fw, fh };
        SDL_Rect dst = { x, yi - dh / 2, lane_w, dh };

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

        // Subtitle: "press r to retry" — appears after 1s
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

    // Timing error indicator (FAST/SLOW) — Placed ABOVE the judgement
    // Hide for highest judgements as per request
    is_highest = (ex_mode_ && last_j == Judgement::PEXTRA) ||
                 (!ex_mode_ && last_j == Judgement::PCRIT);

    if (last_j != Judgement::MISS && last_j != Judgement::NONE && !is_highest) {
        std::string err_text = last_err < 0 ? "FAST" : "SLOW";
        Color err_col = last_err < 0 ? Color{80, 180, 255, static_cast<uint8_t>(200 * alpha_mult)}
                                                 : Color{255, 100, 100, static_cast<uint8_t>(200 * alpha_mult)};
        
        font_.DrawText(renderer_, field_center_x, judge_y - 50, err_text, err_col, FontSize::SMALL, TextAlign::CENTER);
    }

    // Numeric offset — only when debug overlays are active
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
        double ratio = std::clamp(error_ms / 100.0, -1.0, 1.0); // ±100ms range
        int tx = field_center_x + static_cast<int>(ratio * (bar_w / 2));
        
        Color tick_col = (error_ms < 0) ? Color{80, 200, 255, 255} : Color{255, 150, 50, 255};
        tick_col.a = static_cast<uint8_t>(255 * alpha_mult);
        DrawRect(tx - 2, bar_y - 3, 4, 8, tick_col);
    }

    // --- Combo display with BOUNCE ---
    if (combo > 1) {
        char combo_buf[16];
        std::snprintf(combo_buf, sizeof(combo_buf), "%d", combo);
        
        // Combo Color based on lowest judgement in chain (DDR-style)
        Color combo_col = {255, 255, 255, 255}; // Default/Highest
        switch (lowest_j) {
            case Judgement::PEXTRA:
            case Judgement::PCRIT:   combo_col = {255, 255, 255, 255}; break; // White
            case Judgement::PERFECT: 
            case Judgement::PERFECT_LOW: combo_col = {255, 215, 0, 255}; break;   // Gold
            case Judgement::GREAT_HIGH:
            case Judgement::GREAT:
            case Judgement::GREAT_LOW: combo_col = {100, 255, 100, 255}; break; // Green
            case Judgement::GOOD:    combo_col = {80, 160, 255, 255}; break;  // Blue
            default: break;
        }
        
        combo_col.a = static_cast<uint8_t>(255 * alpha_mult);
        
        // Bounce animation: Scale to 1.15x on each increment
        double bounce_duration = 0.6;
        double bounce_elapsed = bounce_duration - j_timer;
        double bounce = 1.0 + 0.15 * std::pow(1.0 - std::min(1.0, bounce_elapsed / 0.1), 3.0);
        
        // Center of the playfield horizontally
        int combo_x = field_center_x;
        int combo_y = judge_y - 115;

        // Draw outlined bold combo number using optimized Score.otf monospaced
        font_.DrawMonoText(renderer_, combo_x, combo_y, combo_buf, combo_col, FontSize::HUGE, TextAlign::CENTER, bounce, "score", -1, true);

        font_.DrawText(renderer_, combo_x, combo_y + 50, "COMBO", {220, 220, 240, combo_col.a}, FontSize::SMALL, TextAlign::CENTER);
    }
}

void GameWindow::RenderMeasureLines() {
    if (!current_chart_) return;
    const auto& cfg = players_[render_player_idx_].field_config;
    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = cfg.receptor_y;
    int receptor_yi = static_cast<int>(receptor_y);
    double current_time = play_time_;
    constexpr double FADE_DURATION = 0.5;

    int start_m = std::max(0, static_cast<int>(std::floor((current_beat - 4.0) / 4.0)));
    int end_m = static_cast<int>(std::ceil((current_beat + 20.0) / 4.0));

    // Helper: draw a single beat line with fade logic
    auto drawBeatLine = [&](double beat, uint8_t base_r, uint8_t base_g, uint8_t base_b, uint8_t base_a) {
        double y = NoteRenderer::GetYPosForBeat(beat, conductor_, cfg);
        bool passed_receptor = cfg.downscroll ? (y > receptor_y) : (y < receptor_y);

        if (passed_receptor) {
            // Line has passed the receptor — pin it there and fade out
            double beat_time = conductor_.BeatToTime(beat);
            double elapsed = current_time - beat_time;
            if (elapsed < 0.0) elapsed = 0.0;
            if (elapsed >= FADE_DURATION) return; // fully faded
            double fade = 1.0 - (elapsed / FADE_DURATION);
            uint8_t alpha = static_cast<uint8_t>(base_a * fade);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, alpha);
            SDL_Rect line_rect = { GetFieldLeft(), receptor_yi - 2, GetFieldRight() - GetFieldLeft(), 4 };
            SDL_RenderFillRect(renderer_, &line_rect);
        } else {
            // Line hasn't reached receptor yet — draw normally
            if (y < -10 || y > height_ + 10) return;
            int yi = static_cast<int>(y);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, base_a);
            SDL_Rect line_rect = { GetFieldLeft(), yi - 2, GetFieldRight() - GetFieldLeft(), 4 };
            SDL_RenderFillRect(renderer_, &line_rect);
        }
    };

    // Gather BPM change and stop beats for coloring
    const auto& bpm_segs = active_simfile_ && current_chart_
        ? active_simfile_->GetEffectiveBPMs(*current_chart_)
        : std::vector<TimingSegment>{};
    const auto& stop_segs = active_simfile_ && current_chart_
        ? active_simfile_->GetEffectiveStops(*current_chart_)
        : std::vector<TimingSegment>{};

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

void GameWindow::RenderNotes() {
    if (!current_chart_) return;
    const auto& cfg = players_[render_player_idx_].field_config;
    auto [first, last] = NoteRenderer::GetVisibleNoteRange(
        *current_chart_, conductor_, cfg, static_cast<double>(height_), 16.0);

    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = cfg.receptor_y;

    // Helper to render a hold body
    auto renderHoldBody = [&](int col, double head_beat, double tail_beat, NoteType type, size_t head_idx) {
        double head_y = NoteRenderer::GetYPosForBeat(head_beat, conductor_, cfg);
        double tail_y = NoteRenderer::GetYPosForBeat(tail_beat, conductor_, cfg);
        
        int x = GetLaneX(col);
        int w = GetLaneWidth();

        bool head_hit = IsNoteHit(head_idx, col);
        bool key_held = players_[render_player_idx_].input.GetLaneState(col).pressed;
        bool head_passed = head_hit || (cfg.downscroll ? head_y > receptor_y : head_y < receptor_y);
        bool tail_past_receptor = cfg.downscroll
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
            if (cfg.downscroll) {
                if (tail_y > receptor_y) tail_render_y = receptor_y;
            } else {
                if (tail_y < receptor_y) tail_render_y = receptor_y;
            }
        }

        int body_y_start, body_y_end;
        if (cfg.downscroll) {
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

        // Draw body — TILE the texture vertically
        SDL_Texture* body_tex = (type == NoteType::HoldHead) ? hold_body_texture_ : roll_body_texture_;
        if (body_tex) {
            bool is_routine = (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple"));
            
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
                bool is_routine = (current_chart_ && (current_chart_->chart_type == "dance-routine" || current_chart_->chart_type == "dance-couple"));
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
                int cap_y = cfg.downscroll
                    ? (static_cast<int>(tail_y) - cap_h)  // Above the body top
                    : static_cast<int>(tail_y);            // Below the body bottom
                SDL_Rect cap_dst = { x, cap_y, w, cap_h };
                // Cap points UP in downscroll, DOWN in upscroll
                SDL_RendererFlip flip = cfg.downscroll ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer_, cap_tex, nullptr, &cap_dst, 0.0, nullptr, flip);
                
                // Reset color mod
                SDL_SetTextureColorMod(cap_tex, 255, 255, 255);
            }
        }
    };

    // 1. Scan for active holds that started BEFORE 'first'
    for (int col = 0; col < current_chart_->num_columns; ++col) {
        for (int j = static_cast<int>(first) - 1; j >= 0; --j) {
            NoteType nt = current_chart_->note_rows[j].columns[col];
            if (nt == NoteType::HoldHead || nt == NoteType::RollHead) {
                // Find tail
                double tail_beat = -1.0;
                for (size_t k = j + 1; k < current_chart_->note_rows.size(); ++k) {
                    if (col < (int)current_chart_->note_rows[k].columns.size() &&
                        current_chart_->note_rows[k].columns[col] == NoteType::HoldTail) {
                        tail_beat = current_chart_->note_rows[k].beat;
                        break;
                    }
                }
                if (tail_beat > current_beat - 4.0) { // Keep rendering a bit after tail passes
                    renderHoldBody(col, current_chart_->note_rows[j].beat, tail_beat, nt, j);
                }
                break;
            }
            if (nt == NoteType::HoldTail) break;
        }
    }

    // 2. Render visible notes and their hold bodies
    for (size_t i = first; i < last; ++i) {
        const auto& row = current_chart_->note_rows[i];
        double y = NoteRenderer::GetYPosForBeat(row.beat, conductor_, cfg);
        
        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
            NoteType type = row.columns[static_cast<size_t>(col)];
            if (type == NoteType::None) continue;
            
            bool is_head = (type == NoteType::HoldHead || type == NoteType::RollHead);
            bool hit = IsNoteHit(i, col);

            // Skip non-head notes that were already hit
            if (!is_head && hit) continue;

            // For hold/roll heads, find the tail beat
            double tail_beat = -1.0;
            if (is_head) {
                for (size_t j = i + 1; j < current_chart_->note_rows.size(); ++j) {
                    if (col < (int)current_chart_->note_rows[j].columns.size() &&
                        current_chart_->note_rows[j].columns[col] == NoteType::HoldTail) {
                        tail_beat = current_chart_->note_rows[j].beat;
                        break;
                    }
                }

                // Draw body FIRST (behind the head)
                if (tail_beat > 0.0) {
                    renderHoldBody(col, row.beat, tail_beat, type, i);
                }
            }

            // Draw the note head (on top of body)
            if (type != NoteType::HoldTail) {
                if (is_head && hit) {
                    // Hold/roll head stays at receptor while key is held or tail hasn't passed
                    double tail_time = (tail_beat > 0.0) ? conductor_.BeatToTime(tail_beat) : 0.0;
                    bool key_held = players_[render_player_idx_].input.GetLaneState(col).pressed;
                    bool tail_still_active = (tail_beat > 0.0) && (play_time_ < tail_time);
                    if (key_held || tail_still_active) {
                        DrawNote(col, static_cast<int>(receptor_y), row.beat, type);
                    }
                } else {
                    DrawNote(col, static_cast<int>(y), row.beat, type);
                }
            }
        }
    }
}

void GameWindow::RenderHUD() {
    RenderTopBar();
    RenderBottomBar();
    RenderProgressBar();
    RenderLifeBar();

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

void GameWindow::RenderTopBar() {
    int cx = width_ / 2;
    // Background removed as per request
    
    if (active_simfile_) {
        // Song title slightly smaller and dimmed if needed, or kept clean
        font_.DrawText(renderer_, width_ / 2, 60, active_simfile_->title, {200, 200, 220, 180}, FontSize::SMALL, TextAlign::CENTER, 1.0, "score");
    }

    // --- Combo & Judgement ---
    int combo_y = 350;
    // Combo display removed from Top Bar (duplicate)
    
    // Grade Level Up Popup (Text removed, now animates Accuracy)
    // BPM at the Top Center (Kept centered)
    char bpm_buf[32];
    std::snprintf(bpm_buf, sizeof(bpm_buf), "%.1f", conductor_.GetCurrentBPM());
    font_.DrawText(renderer_, width_ / 2, 10, "BPM", {150, 150, 180, 255}, FontSize::SMALL, TextAlign::CENTER);
    font_.DrawText(renderer_, width_ / 2, 28, bpm_buf, {255, 255, 255, 255}, FontSize::MEDIUM, TextAlign::CENTER);

    // Modifier Icons (Tags) & Accuracy — Aligned above the Notefield (1P/Left)
    int field_left = GetFieldLeft();
    int icon_y = 55;
    int acc_y = 85;
    
    std::vector<std::string> mods;
    // Speed Mod
    char speed_buf[16];
    std::snprintf(speed_buf, sizeof(speed_buf), "x%.2f", field_config_.speed_mod);
    mods.push_back(speed_buf);
    
    // Scroll Dir
    mods.push_back(field_config_.downscroll ? "DOWN" : "UP");
    
    // EX Mode icon if active
    if (ex_mode_) mods.push_back("EX");

    // Autoplay icon if active
    if (autoplay_) mods.push_back("AUTOPLAY");

    int icon_spacing = 8;
    int cur_x = field_left;
    for (size_t i = 0; i < mods.size(); ++i) {
        int tw = font_.GetTextWidth(mods[i], FontSize::SMALL);
        int bw = tw + 12;
        // Draw tag box
        DrawRect(cur_x, icon_y, bw, 22, {40, 40, 60, 180});
        DrawRectOutline(cur_x, icon_y, bw, 22, {80, 80, 120, 200});
        font_.DrawText(renderer_, cur_x + 6, icon_y + 3, mods[i], {220, 230, 255, 255}, FontSize::SMALL);
        cur_x += bw + icon_spacing;
    }

    // Accuracy display: "99.85%" (Below Icons, Left Aligned with field)
    double accuracy = 0.0;
    if (total_hittable_notes_ > 0) {
        accuracy = ex_mode_ ? (ex_score_ / total_hittable_notes_) 
                            : (normal_score_ / total_hittable_notes_);
    }
    
    // Accuracy scale pop animation
    double acc_scale = 1.0;
    if (grade_popup_timer_ > 0.0) {
        // Immediate pop to 1.15x then settle back over 0.2s
        double t = grade_popup_timer_; // 0.2 -> 0.0
        acc_scale = 1.0 + 0.15 * (t / 0.2);
    }
    
    // Determine color based on Accuracy/Grade thresholds (matching internal scoring)
    Color acc_color = {200, 200, 200, 255}; // Default
    if (accuracy >= 100.75)      acc_color = {180, 240, 255, 255}; // SSS
    else if (accuracy >= 100.0)  acc_color = {255, 255, 255, 255}; // SS
    else if (accuracy >= 97.5)   acc_color = {255, 230, 50, 255};  // S (Gold)
    else if (accuracy >= 90.0)   acc_color = {100, 255, 150, 255}; // A
    else if (accuracy >= 60.0)   acc_color = {80, 160, 255, 255};  // B (and BB, BBB)
    
    int precision = ex_mode_ ? 2 : 4;

    // Draw with Outline and Bold Integer part
    font_.DrawAccuracy(renderer_, field_left, acc_y, accuracy, acc_color, TextAlign::LEFT, acc_scale, precision, true, true);

    // --- Shine Effect (Grade based Sheen) ---
    // S rank (97.5%) and above gets a sweeping shine
    if (accuracy >= 97.5) {
        // Sweep frequency: once every 3 seconds
        double sweep_time = std::fmod(play_time_, 3.0);
        if (sweep_time < 0.6) { // 0.6s sweep duration
            double progress = sweep_time / 0.6;
            
            // Highlight color based on grade
            Color shine_col = {255, 255, 255, 150}; // Default white shine
            if (accuracy >= 100.0) shine_col = {220, 240, 255, 180}; // Blue-ish for SS/SSS
            else if (accuracy >= 97.5) shine_col = {255, 255, 180, 160}; // Gold-ish for S
            
            // To implement a "sheen", we can render the text again with a clipping rectangle
            // that moves across the text area.
            int total_w = 200; // Estimated max width, or we could calculate but let's be safe
            int sheen_w = 40;
            int sheen_x = field_left - sheen_w + static_cast<int>(total_w * progress);
            
            SDL_Rect clip = { sheen_x, acc_y - 20, sheen_w, 80 };
            SDL_RenderSetClipRect(renderer_, &clip);
            
            // Draw again with shine color (no outline, just additive highlight)
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_ADD);
            font_.DrawAccuracy(renderer_, field_left, acc_y, accuracy, shine_col, TextAlign::LEFT, acc_scale, precision, false, true);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            
            SDL_RenderSetClipRect(renderer_, NULL);
        }
    }
}

void GameWindow::RenderBottomBar() {
    // Dark translucent bar removed - making it cleaner
    // font_.DrawText calls removed as BPM info moved to top
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

void GameWindow::RenderLifeBar() {
    int bar_w = 20;
    int bar_h = static_cast<int>(height_ * 0.6);
    int bar_x = 15; // Vertical on the left side
    int bar_y = (height_ - bar_h) / 2;

    // Draw frame (neon/metallic)
    DrawRectOutline(bar_x - 3, bar_y - 3, bar_w + 6, bar_h + 6, {60, 60, 100, 255});
    DrawRectOutline(bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2, {100, 100, 180, 200});
    DrawRect(bar_x - 2, bar_y - 2, bar_w + 4, bar_h + 4, {10, 10, 20, 180});

    if (life_meter_.GetType() == LifeType::STANDARD || life_meter_.GetType() == LifeType::FLARE) {
        float life = life_meter_.GetLife();
        int fill_h = static_cast<int>(bar_h * life);
        
        // Gradient approx: Bottom (Red) to Top (Green)
        // Since we draw from top-left, we draw the fill from the bottom up
        for (int i = 0; i < fill_h; ++i) {
            float t = static_cast<float>(i) / bar_h; // t goes from 0 at top of fill to ~1 at bottom
            // Colors: Top = Green {50, 255, 100}, Mid = Yellow {255, 200, 50}, Bottom = Red {255, 50, 50}
            Color c;
            if (t < 0.5f) {
                float mix = t * 2.0f;
                c.r = static_cast<uint8_t>(50 + (255 - 50) * mix);
                c.g = static_cast<uint8_t>(255 + (200 - 255) * mix);
                c.b = static_cast<uint8_t>(100 + (50 - 100) * mix);
            } else {
                float mix = (t - 0.5f) * 2.0f;
                c.r = 255;
                c.g = static_cast<uint8_t>(200 + (50 - 200) * mix);
                c.b = 50;
            }
            c.a = 255;
            SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
            SDL_RenderDrawLine(renderer_, bar_x, bar_y + bar_h - i, bar_x + bar_w - 1, bar_y + bar_h - i);
        }
        
        // Label
        std::string label = "LIFE";
        if (life_meter_.GetType() == LifeType::FLARE) {
            int level = life_meter_.GetFlareLevel();
            label = (level == 5) ? "FLARE EX" : "FLARE " + std::to_string(level);
        }
        font_.DrawText(renderer_, bar_x + bar_w / 2, bar_y - 10, label, {200, 200, 230, 255}, FontSize::SMALL, TextAlign::CENTER);
    } else {
        // Battery/Risky (Vertical stack)
        int lives = life_meter_.GetBatteryLives();
        int max_lives = (life_meter_.GetType() == LifeType::LIFE4) ? 4 : 1;
        
        int seg_h = bar_h / std::max(1, max_lives);
        for (int i = 0; i < max_lives; ++i) {
            bool active = (max_lives - 1 - i < lives); // Fill from bottom
            Color col = active ? Color{255, 255, 255, 255} : Color{50, 50, 70, 150};
            DrawRect(bar_x, bar_y + i * seg_h + 1, bar_w, seg_h - 2, col);
        }
        
        std::string label = (life_meter_.GetType() == LifeType::LIFE4) ? "LIFE4" : "RISKY";
        font_.DrawText(renderer_, bar_x + bar_w / 2, bar_y - 10, label, {200, 200, 230, 255}, FontSize::SMALL, TextAlign::CENTER);
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

void GameWindow::DrawNote(int lane, double y, double beat, NoteType type) {
    int x = GetLaneX(lane);
    int w = GetLaneWidth();
    int yi = static_cast<int>(y);
    
    // --- Mine: use mine texture with spinning rotation ---
    if (type == NoteType::Mine) {
        if (mine_texture_) {
            double scale = static_cast<double>(w) / mine_tex_w_;
            int dh = static_cast<int>(mine_tex_h_ * scale);
            SDL_Rect mdst = { x, yi - dh / 2, w, dh };
            SDL_RenderCopyEx(renderer_, mine_texture_, nullptr, &mdst, play_time_ * 360.0, nullptr, SDL_FLIP_NONE);
        } else {
            // Fallback: draw X with rectangle outline
            Color color = LaneColors::MINE;
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
            qrow = (lane < 4) ? 0 : 1; // P1: 4th (Red), P2: 8th (Blue) fallback
        }

        int frame = static_cast<int>(play_time_ * 20.0) % 16; // 20fps animation
        
        int fw = note_tex_w_ / 16;
        int fh = note_tex_h_ / 8;
        
        SDL_Rect src = { frame * fw, qrow * fh, fw, fh };
        
        double scale = static_cast<double>(w) / fw;
        int dh = static_cast<int>(fh * scale);
        
        // Centering: align the vertical middle of the note to 'yi'
        SDL_Rect dst = { x, yi - dh / 2, w, dh };
        
        double angle = 0.0;
        if (lane == 0) angle = 90.0;       // Left
        else if (lane == 1) angle = 0.0;   // Down
        else if (lane == 2) angle = 180.0; // Up
        else if (lane == 3) angle = -90.0; // Right
        
        // --- Lift: use lift texture (1x8 sprite sheet — 1 column, 8 quantization rows) ---
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
                SDL_RenderCopyEx(renderer_, lift_texture_, &lsrc, &ldst, angle, nullptr, SDL_FLIP_NONE);
                return;
            }
            // Fallback: tint the regular note texture
            SDL_SetTextureColorMod(note_texture_, 100, 255, 255);
        } else if (type == NoteType::Fake) {
            // Fakes are semi-transparent
            SDL_SetTextureAlphaMod(note_texture_, 128);
        }

        SDL_RenderCopyEx(renderer_, note_texture_, &src, &dst, angle, nullptr, SDL_FLIP_NONE);

        // Reset mods
        if (type == NoteType::Fake) SDL_SetTextureAlphaMod(note_texture_, 255);
        if (type == NoteType::Lift) SDL_SetTextureColorMod(note_texture_, 255, 255, 255);

    } else {
        // Fallback to rectangle drawing if texture failed to load
        Color color = GetNoteColor(beat);
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
    
    for (int l = 0; l < 4; ++l) {
        for (int s = 0; s < 3; ++s) {
            const auto& b = custom_binds_4k_[l][s];
            if (b.type != BindInfo::NONE) {
                std::fprintf(f, "bind_4k_%d_%d_type=%d\n", l, s, (int)b.type);
                std::fprintf(f, "bind_4k_%d_%d_id=%d\n", l, s, b.id);
            }
        }
    }

    std::fclose(f);
}

void GameWindow::LoadSettings() {
    std::FILE* f = std::fopen("settings.cfg", "r");
    if (!f) return;
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
            else if (skey.find("bind_4k_") == 0) {
                // Format: bind_4k_L_S_type or bind_4k_L_S_id
                int l = -1, s = -1;
                char suffix[16];
                if (std::sscanf(skey.c_str(), "bind_4k_%d_%d_%15s", &l, &s, suffix) == 3) {
                    if (l >= 0 && l < 4 && s >= 0 && s < 3) {
                        std::string s_suffix = suffix;
                        if (s_suffix == "type") custom_binds_4k_[l][s].type = (BindInfo::Type)std::atoi(val);
                        else if (s_suffix == "id") custom_binds_4k_[l][s].id = std::atoi(val);
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
    int num_cols = current_chart_ ? current_chart_->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;

    // Single mode (4-lane): center the field, then apply offset
    if (num_cols == 4) {
        int lane_w = GetLaneWidth();
        int scaled_padding = static_cast<int>(LANE_PADDING * scale);
        int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
        return (width_ - total_w) / 2 + render_x_offset_;
    }

    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    
    int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
    
    // Add gap for Couple mode
    if (current_chart_ && current_chart_->chart_type == "dance-couple" && num_cols == 8) {
        total_w += static_cast<int>(60 * scale);
    }
    
    return (width_ - total_w) / 2 + render_x_offset_;
}

int GameWindow::GetFieldRight() const {
    int num_cols = current_chart_ ? current_chart_->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    int field_left = GetFieldLeft();
    int total_w = num_cols * lane_w + (num_cols - 1) * scaled_padding;
    if (current_chart_ && current_chart_->chart_type == "dance-couple" && num_cols == 8) {
        total_w += static_cast<int>(60 * scale);
    }
    return field_left + total_w;
}

int GameWindow::GetLaneX(int lane) const {
    int actual_lane = lane;
    if (effect_mode_ == 1 && current_chart_) { // Mirror
        actual_lane = current_chart_->num_columns - 1 - lane;
    }

    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    int lane_w = GetLaneWidth();
    int field_left = GetFieldLeft();

    // --- Routine/Couple Split Logic ---
    if (current_chart_ && current_chart_->num_columns == 8) {
        if (current_chart_->chart_type == "dance-couple") {
            int gap = static_cast<int>(60 * scale);
            if (actual_lane < 4) {
                return field_left + actual_lane * (lane_w + scaled_padding);
            } else {
                return field_left + actual_lane * (lane_w + scaled_padding) + gap;
            }
        }
    }

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


void GameWindow::UpdateScores(double abs_error, int num_notes, int player_idx) {
    Judgement j_norm = ClassifyHit(abs_error, false);
    Judgement j_ex = ClassifyHit(abs_error, true);

    int ni = static_cast<int>(j_norm) - 1;
    if (ni >= 0 && ni < 9) normal_judge_counts_[ni] += num_notes;

    int ei = static_cast<int>(j_ex) - 1;
    if (ei >= 0 && ei < 9) ex_judge_counts_[ei] += num_notes;

    // Add weights to actual score variables
    double norm_weighted = JudgeWeight(j_norm, false) * num_notes;
    double ex_weighted = JudgeWeight(j_ex, true) * num_notes;
    
    normal_score_ += norm_weighted;
    ex_score_     += ex_weighted;

    // Support per-player state
    if (player_idx >= 0 && player_idx < MAX_PLAYERS && players_[player_idx].joined) {
        players_[player_idx].normal_score += norm_weighted;
        players_[player_idx].ex_score     += ex_weighted;
        if (j_norm == Judgement::MISS) players_[player_idx].total_miss += num_notes;
        else players_[player_idx].total_hits += num_notes;
    }

    // Update Life
    life_meter_.OnJudgement(j_norm);
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

} // namespace sml

#endif // HAS_SDL2
