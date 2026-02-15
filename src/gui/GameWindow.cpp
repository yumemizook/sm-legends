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
        screen_ = ScreenState::SONG_SELECT;
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
    last_tick_ = static_cast<double>(SDL_GetTicks()) / 1000.0;
    while (running_) {
        double now = static_cast<double>(SDL_GetTicks()) / 1000.0;
        double dt = now - last_tick_;
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
        case ScreenState::SONG_SELECT: HandleKeyDown_SongSelect(key); break;
        case ScreenState::GAMEPLAY:    HandleKeyDown_Gameplay(key); break;
        case ScreenState::RESULTS:     HandleKeyDown_Results(key); break;
        case ScreenState::OPTIONS:     HandleKeyDown_Options(key); break;
        case ScreenState::CALIBRATION: HandleKeyDown_Calibration(key); break;
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
    // Check if it's a lane key first
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
            SaveSettings();
            break;
        case SDLK_F3:
            if (field_config_.mod_type == ScrollModType::CMod)
                field_config_.speed_mod = std::max(50.0, field_config_.speed_mod - 50.0);
            else
                field_config_.speed_mod = std::max(0.25, field_config_.speed_mod - 0.25);
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
            SaveSettings();
            break;
        case SDLK_F4:
            field_config_.downscroll = !field_config_.downscroll;
            if (field_config_.downscroll)
                field_config_.receptor_y = height_ * 0.85; // Bottom
            else
                field_config_.receptor_y = height_ * 0.15; // Top
            SaveSettings();
            break;
        case SDLK_F1:
            show_debug_ = !show_debug_;
            break;
        case SDLK_F5:
            ex_mode_ = !ex_mode_;
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
    input_.OnKeyUp(key);
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

    switch (key) {
        case SDLK_UP:
            modifier_menu_cursor_ = (modifier_menu_cursor_ - 1 + NUM_ITEMS) % NUM_ITEMS;
            break;
        case SDLK_DOWN:
            modifier_menu_cursor_ = (modifier_menu_cursor_ + 1) % NUM_ITEMS;
            break;
        case SDLK_ESCAPE:
        case SDLK_BACKSLASH:
            showing_modifier_menu_ = false;
            SaveSettings();
            break;
        case SDLK_LEFT:
        case SDLK_RIGHT: {
            bool right = (key == SDLK_RIGHT);
            float delta = right ? 1.0f : -1.0f;

            switch (modifier_menu_cursor_) {
                case 0: // Speed
                    if (field_config_.mod_type == ScrollModType::CMod)
                        field_config_.speed_mod += delta * 25.0f;
                    else
                        field_config_.speed_mod += delta * 0.25f;
                    field_config_.speed_mod = std::max(0.25f, (float)field_config_.speed_mod);
                    break;
                case 1: // Mod Type
                    field_config_.mod_type = (field_config_.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod;
                    break;
                case 2: // Scroll
                    field_config_.downscroll = !field_config_.downscroll;
                    field_config_.receptor_y = field_config_.downscroll ? height_ * 0.85 : height_ * 0.15;
                    break;
                case 3: // Sudden+
                    sudden_plus_val_ = std::clamp(sudden_plus_val_ + delta * 0.05f, 0.0f, 0.8f);
                    break;
                case 4: // Hidden+
                    hidden_plus_val_ = std::clamp(hidden_plus_val_ + delta * 0.05f, 0.0f, 0.8f);
                    break;
                case 5: // Noteskin
                    if (!available_noteskins_.empty()) {
                        noteskin_index_ = (noteskin_index_ + (right ? 1 : -1) + (int)available_noteskins_.size()) % (int)available_noteskins_.size();
                        LoadNoteskin(available_noteskins_[noteskin_index_]);
                    }
                    break;
                case 6: // Effects
                    effect_mode_ = (effect_mode_ + (right ? 1 : -1) + 3) % 3;
                    break;
                case 7: // Life Mode
                    if (life_meter_.GetType() == LifeType::STANDARD && right) life_meter_.Init(LifeType::LIFE4);
                    else if (life_meter_.GetType() == LifeType::LIFE4) life_meter_.Init(right ? LifeType::RISKY : LifeType::STANDARD);
                    else if (life_meter_.GetType() == LifeType::RISKY) life_meter_.Init(right ? LifeType::FLARE : LifeType::LIFE4, 1);
                    else if (life_meter_.GetType() == LifeType::FLARE) {
                        int level = life_meter_.GetFlareLevel();
                        if (right) {
                            if (level < 5) life_meter_.Init(LifeType::FLARE, level + 1);
                            else life_meter_.Init(LifeType::STANDARD);
                        } else {
                            if (level > 1) life_meter_.Init(LifeType::FLARE, level - 1);
                            else life_meter_.Init(LifeType::RISKY);
                        }
                    }
                    else if (life_meter_.GetType() == LifeType::STANDARD && !right) {
                        life_meter_.Init(LifeType::FLARE, 5);
                    }
                    break;
                case 8: // Scoring
                    ex_mode_ = !ex_mode_;
                    break;
            }
            break;
        }
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            switch (modifier_menu_cursor_) {
                case 1: field_config_.mod_type = (field_config_.mod_type == ScrollModType::XMod) ? ScrollModType::CMod : ScrollModType::XMod; break;
                case 2: field_config_.downscroll = !field_config_.downscroll; field_config_.receptor_y = field_config_.downscroll ? height_ * 0.85 : height_ * 0.15; break;
                case 5: if(!available_noteskins_.empty()) { noteskin_index_ = (noteskin_index_ + 1) % (int)available_noteskins_.size(); LoadNoteskin(available_noteskins_[noteskin_index_]); } break;
                case 6: effect_mode_ = (effect_mode_ + 1) % 3; break;
                case 8: ex_mode_ = !ex_mode_; break;
                default: break;
            }
            break;
    }
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
    audio_offset_ = std::clamp(audio_offset_, -0.5, 0.5);
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
    conductor_.Initialize(*active_simfile_, *current_chart_);

    loaded_simfile_.reset();
    StartGameplayDirect();
    ChangeScreen(ScreenState::GAMEPLAY);

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
                if (nt == NoteType::Tap || nt == NoteType::HoldHead || 
                    nt == NoteType::RollHead || nt == NoteType::Lift) {
                    total_hittable_notes_++;
                    has_hittable = true;
                }
            }
            if (has_hittable) {
                if (first_note_beat < 0.0) first_note_beat = row.beat;
                if (row.beat > last_note_beat) last_note_beat = row.beat;
            }
        }

        double first_note_time = (first_note_beat >= 0.0) ? conductor_.BeatToTime(first_note_beat) : 0.0;
        chart_end_time_ = conductor_.BeatToTime(last_note_beat) + 2.0;
        song_duration_ = chart_end_time_;

        // 4. Conditional Padding Logic
        ready_animation_timer_ = 2.0;
        if (first_note_time < 2.0) {
            // First note is too close, delay audio
            play_time_ = -2.0;
            playing_ = false;
        } else {
            // First note is far enough, start audio immediately
            play_time_ = 0.0;
            playing_ = true;
            if (audio_loaded_) {
                audio_.Play(0.0);
            }
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
    }
}

void GameWindow::RenderResults() {
    // 1. Background
    RenderBackground(); 

    // 2. Dim overlay for results
    DrawRect(0, 0, width_, height_, {10, 10, 20, 180});

    // --- Header ---
    if (results_reveal_timer_ > 0.0) {
        font_.DrawText(renderer_, 40, 30, "RESULTS", {255, 255, 255, 255}, FontSize::TITLE, TextAlign::LEFT);
        
        int y_title = 30;
        font_.DrawText(renderer_, width_ - 40, y_title, results_title_, {200, 200, 220, 255}, FontSize::MEDIUM, TextAlign::RIGHT, 1.0, "score");
        font_.DrawText(renderer_, width_ - 40, y_title + 25, results_chart_info_, {150, 150, 170, 255}, FontSize::SMALL, TextAlign::RIGHT);
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
    if (results_reveal_timer_ > 1.2) {
        font_.DrawText(renderer_, width_ / 2, height_ - 30, "PRESS [ENTER] OR [ESC] TO CONTINUE", {150, 150, 170, 255}, FontSize::SMALL, TextAlign::CENTER);
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
    info_normal.value = normal_acc;
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
    info_ex.value = ex_acc;
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
    if (main_info.icon) {
        int tw, th;
        SDL_QueryTexture(main_info.icon, nullptr, nullptr, &tw, &th);
        double scale = 1.0;
        if (th > 0) scale = 100.0 / th; // Max height 100
        int dw = (int)(tw * scale);
        int dh = (int)(th * scale);
        SDL_Rect r = { left_x, cur_y, dw, dh };
        SDL_RenderCopy(renderer_, main_info.icon, nullptr, &r);
        
        // Score next to it
        font_.DrawAccuracy(renderer_, left_x + dw + 30, cur_y + dh/2 - 20, main_info.value, {255, 255, 255, 255}, TextAlign::LEFT, 1.5, main_info.is_percentage ? 4 : 2);
    } else {
        // Fallback text
        font_.DrawText(renderer_, left_x + 60, cur_y + 40, main_info.text_grade, main_info.color, FontSize::HUGE, TextAlign::CENTER);
        font_.DrawAccuracy(renderer_, left_x + 180, cur_y + 30, main_info.value, {255, 255, 255, 255}, TextAlign::LEFT, 1.2, main_info.is_percentage ? 4 : 2);
    }
    
    cur_y += 110;

    // --- SECONDARY DISPLAY (Small) ---
    // Icon on Left, Score on Right (Same layout, smaller)
    if (sec_info.icon) {
        int tw, th;
        SDL_QueryTexture(sec_info.icon, nullptr, nullptr, &tw, &th);
        double scale = 0.6; // Smaller
        int dw = (int)(tw * scale);
        int dh = (int)(th * scale);
        SDL_Rect r = { left_x, cur_y, dw, dh };
        SDL_RenderCopy(renderer_, sec_info.icon, nullptr, &r);
        
        font_.DrawAccuracy(renderer_, left_x + dw + 20, cur_y + dh/2 - 10, sec_info.value, {200, 200, 255, 255}, TextAlign::LEFT, 1.0, sec_info.is_percentage ? 4 : 2);
    } else {
        font_.DrawText(renderer_, left_x + 30, cur_y + 10, sec_info.text_grade, sec_info.color, FontSize::MEDIUM, TextAlign::CENTER);
        font_.DrawAccuracy(renderer_, left_x + 100, cur_y + 5, sec_info.value, {200, 200, 255, 255}, TextAlign::LEFT, 1.0, sec_info.is_percentage ? 4 : 2);
    }
    
    // Rating
    if (current_chart_) {
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
        char rate_buf[32];
        std::snprintf(rate_buf, sizeof(rate_buf), "RATING: %.2f", rating);
        
        // Fix: Push down rating text to avoid overlap
        // Previously: cur_y + 50
        // New: cur_y is at the top of secondary display.
        // Secondary display height ~50px.
        // Rating should be below it.
        font_.DrawText(renderer_, left_x, cur_y + 80, rate_buf, {80, 255, 255, 255}, FontSize::SMALL);
    }

    cur_y += 120; // Increased spacing to push stats down

    // --- Judgement Stats ---
    auto DrawStatRow = [&](const char* label, int count, Color c, int& ry) {
        font_.DrawText(renderer_, left_x, ry, label, c, FontSize::MEDIUM); // Bigger font
        char buf[16]; std::snprintf(buf, sizeof(buf), "%d", count);
        font_.DrawText(renderer_, right_x, ry, buf, {255,255,255,255}, FontSize::MEDIUM, TextAlign::RIGHT);
        ry += 32; // More vertical spacing for bigger font
    };

    int ry = cur_y;
    
    // Fix: Hide P-EXTRA if not in EX mode
    if (results_ex_mode_) {
        DrawStatRow("P-EXTRA", counts[0], GetJudgementColor(Judgement::PEXTRA), ry);
    }
    DrawStatRow("P-CRITICAL", counts[1], GetJudgementColor(Judgement::PCRIT), ry);
    DrawStatRow("PERFECT", counts[2] + counts[3], GetJudgementColor(Judgement::PERFECT), ry);
    DrawStatRow("GREAT", counts[4] + counts[5] + counts[6], GetJudgementColor(Judgement::GREAT), ry);
    DrawStatRow("GOOD", counts[7], GetJudgementColor(Judgement::GOOD), ry);
    DrawStatRow("MISS", counts[8], GetJudgementColor(Judgement::MISS), ry);

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
    
    // Split spacing: Early on left, Late on right (aligned with stats)? Or just spaced out?
    // "placed them spaced between" -> implies center-ish or separated.
    // Let's put Early at left_x, Late at right_x (aligned right) or center split.
    font_.DrawText(renderer_, left_x, ry, early_buf, {0, 255, 255, 255}, FontSize::SMALL); // Cyan
    font_.DrawText(renderer_, right_x, ry, late_buf, {255, 100, 50, 255}, FontSize::SMALL, TextAlign::RIGHT); // Orange/Red

    // --- Offset Graph (Bottom of panel) ---
    // Use remaining space
    int graph_h = 80;
    int graph_y = y + h - graph_h - 10;
    RenderOffsetGraph(left_x, graph_y, content_w, graph_h, hits);
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

void GameWindow::ProcessLaneHit(int lane, double forced_time) {
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
        if (nt != NoteType::Tap && nt != NoteType::HoldHead && nt != NoteType::RollHead && nt != NoteType::Lift) continue;

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
    note_hit_masks_[best_idx] |= (1 << lane);
    // Track the timing error of this hit (last hit in chord determines judgement)
    row_best_error_[best_idx] = best_error;

    // Record per-lane visual feedback
    double abs_error = std::fabs(best_error);
    Judgement j_visual = sml::ClassifyHit(abs_error, ex_mode_);
    this->input_.RecordHit(lane, j_visual, best_error);

    // Record hit for stats
    hit_history_.push_back({current_time, best_error, j_visual, lane, ex_mode_});

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
    double note_y = NoteRenderer::GetYPosForBeat(note_beat, conductor_, field_config_);
    hit_flashes_.push_back({lane, note_beat, 0.3, note_y, j_visual});

    // --- Chord Cohesion: check if the entire row is now fully hit ---
    if (IsRowFullyHit(best_idx)) {
        // Judgement is based on the LAST note hit in the chord (this one)
        double abs_err = std::fabs(row_best_error_[best_idx]);
        Judgement j_norm = ClassifyHit(abs_err, false);
        Judgement j_ex = ClassifyHit(abs_err, true);

        last_judgement_ = ex_mode_ ? j_ex : j_norm;
        last_timing_error_ = row_best_error_[best_idx];
        judgement_timer_ = 0.6;

        int row_notes = GetRowHittableCount(best_idx);

        // Track per-judgement counts (1 judgement per row, but counts as N notes)
        UpdateScores(abs_err, row_notes);
        UpdateClearType(j_norm, j_ex);

        if (j_norm != Judgement::MISS) {
            total_hits_++;
            combo_++;
            if (combo_ > max_combo_) max_combo_ = combo_;
            
            // Track lowest judgement for combo coloring
            if (combo_ == 1) {
                lowest_judgement_in_combo_ = last_judgement_;
            } else {
                // Judgement ranking (lower enum value is usually better, but let's be explicit)
                // PEXTRA=0, PCRIT=1, PERFECT=2, GREAT=3, GOOD=4, MISS=5 (approx based on typical VSG engines)
                // We want to keep the one with the HIGHER index (worse judgment)
                if ((int)last_judgement_ > (int)lowest_judgement_in_combo_) {
                    lowest_judgement_in_combo_ = last_judgement_;
                }
            }

            // Combo pop animation
            combo_pop_timer_ = 0.15;

            // Score pulse on combo milestones (every 100)
            if (combo_ > 0 && combo_ % 100 == 0) {
                grade_popup_timer_ = 0.2;
            }
            
            // Grade Milestones check (Granular Sync with grade.md)
            double acc = (total_hittable_notes_ > 0) ? (normal_score_ / total_hittable_notes_) : 0.0;
            static const std::vector<double> thresholds = {
                50.0, 60.0, 70.0, 80.0, 90.0, 92.5, 95.0, 97.5, 99.0, 100.0, 100.5, 100.75, 100.9
            };
            
            for (int i = static_cast<int>(thresholds.size()) - 1; i >= 0; --i) {
                if (acc >= thresholds[i] && last_grade_milestone_ < (i + 1)) {
                    last_grade_milestone_ = i + 1;
                    grade_popup_timer_ = 0.2; // Fast 0.2s tween
                    break;
                }
            }
        } else {
            total_miss_++;
            combo_ = 0;
            lowest_judgement_in_combo_ = Judgement::NONE;
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
    if (screen_ == ScreenState::GAMEPLAY) {
        // --- Get Ready Animation Delay ---
        if (!is_transitioning_ && ready_animation_timer_ > 0.0) {
            ready_animation_timer_ -= dt;
            
            if (!playing_) {
                // Delayed start case: move notefield but not music
                play_time_ += dt; 
                
                if (ready_animation_timer_ <= 0.0) {
                    ready_animation_timer_ = 0.0;
                    play_time_ = 0.0; // Precise start
                    if (audio_loaded_ && !is_calibrating_) {
                        audio_.Play(0.0);
                    }
                    playing_ = true;
                }
            } else {
                // Immediate start case: music is already playing, animation is just an overlay
                if (ready_animation_timer_ <= 0.0) {
                    ready_animation_timer_ = 0.0;
                }
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
                play_time_ = audio_.GetPlaybackTime() + audio_offset_;
            } else {
                play_time_ += dt;
            }
        }
        conductor_.Update(play_time_);
        input_.Update(dt);

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
                        for (int col = 0; col < static_cast<int>(row.columns.size()); ++col) {
                            NoteType nt = row.columns[col];
                            if ((nt == NoteType::Tap || nt == NoteType::HoldHead || nt == NoteType::RollHead || nt == NoteType::Lift) && !IsNoteHit(next_hittable_note_, col)) {
                                ProcessLaneHit(col, note_time);
                            }
                        }
                        // next_hittable_note_ is advanced inside ProcessLaneHit or by the loop below
                        continue; 
                    }
                }

                if (current_time - note_time > JudgeWindows::MISS) {
                    // Row missed (1 miss per row)
                    combo_ = 0;
                    total_miss_++;
                    
                    clear_type_ = ClearType::CLEAR; // Broken FC
                    
                    last_judgement_ = Judgement::MISS;
                    judgement_timer_ = 0.6;
                    int row_notes = GetRowHittableCount(next_hittable_note_);
                    UpdateScores(999.0, row_notes); // Now updates life
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
        case ScreenState::SONG_SELECT: RenderSongSelect(); break;
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
    int panel_left = 15;
    int panel_width = 180;
    int panel_top = 100;

// 1. Difficulty List (Vertical focus)
    int y = panel_top;
    for (size_t ci = 0; ci < song.charts.size(); ++ci) {
        const auto& chart = song.charts[ci];
        bool selected = (static_cast<int>(ci) == selected_chart_);
        Color diff_col = GetDifficultyColor(chart.difficulty_name);
        
        // Wild Color Override
        if (chart.variant == ChartVariant::Wild) {
            diff_col = Color::Rainbow(SDL_GetTicks() / 1000.0, 0.7f, 0.6f); // Dark Rainbow
        }

        int row_h = 32;
        if (selected) {
            // Bright selection highlight
            DrawRect(panel_left - 10, y, panel_width + 10, row_h, {diff_col.r, diff_col.g, diff_col.b, 100});
            DrawRect(panel_left - 10, y, 5, row_h, diff_col); // Lead edge
            
            // Flashing border
            uint8_t flash = static_cast<uint8_t>(180 + 75 * std::sin(SDL_GetTicks() * 0.01));
            DrawRect(panel_left - 10, y, panel_width + 10, 1, {255, 255, 255, flash});
            DrawRect(panel_left - 10, y + row_h - 1, panel_width + 10, 1, {255, 255, 255, flash});
        }

        // Difficulty Name & Meter
        std::string label = chart.difficulty_name;
        // Prefix with Variant if Special
        switch (chart.variant) {
            case ChartVariant::BeginnerPlus:  label += " (Beg+)"; break;
            case ChartVariant::EasyPlus:      label += " (Ez+)"; break;
            case ChartVariant::MediumPlus:    label += " (Med+)"; break;
            case ChartVariant::HardPlus:      label += " (Hrd+)"; break;
            case ChartVariant::ChallengePlus: label += " (Cha+)"; break;
            case ChartVariant::Wild:          label = "WILD"; break; // Override name 
            default: break;
        }

        font_.DrawText(renderer_, panel_left, y + 6, label, 
                       selected ? Color{255, 255, 255, 255} : Color{180, 180, 200, 180}, 
                       FontSize::MEDIUM);
        
        // --- NEW: Draw Grade if exists ---
        std::string score_key = song.filepath + "|" + std::to_string(ci);
        auto it = high_scores_.find(score_key);
        if (it != high_scores_.end()) {
            Color grade_col = {200, 200, 200, 255};
            // Simple color picking for grade briefly or just use white
            font_.DrawText(renderer_, panel_left + panel_width - 40, y + 6, "(" + it->second.grade + ")", {255, 255, 200, 255}, FontSize::SMALL);
        }

        char meter_buf[8];
        std::snprintf(meter_buf, sizeof(meter_buf), "%d", chart.custom_difficulty); // Custom 1-30
        font_.DrawText(renderer_, panel_left + panel_width - 10, y + 4, meter_buf, 
                       selected ? Color{255, 255, 255, 255} : diff_col, 
                       FontSize::LARGE, TextAlign::RIGHT);

        y += row_h + 4;
    }

    // 2. Groove Radar (Below diff list)
    y += 40;
    int radar_cx = panel_left + panel_width / 2;
    int radar_cy = y + 80;
    int radar_size = 60;

    // Draw Radar Hexagon Background
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
    
    // Draw Filled Radar Shape
    const auto& selected_c = song.charts[selected_chart_];
    const auto& rv = selected_c.radar;
    // Values are now roughly 1-30 scale
    double vals[5] = { rv.stream, rv.voltage, rv.air, rv.freeze, rv.chaos };
    SDL_Point points[6]; // +1 to close loop
    
    for (int i=0; i<5; ++i) {
        double v = vals[i] / 30.0; // Normalize 0..1 for visual
        if (v > 1.1) v = 1.1; // Visual Cap (allow slight overfill)
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
        
        // Draw Label
        font_.DrawText(renderer_, lx, ly-10, labels[i], {200,200,220,255}, FontSize::SMALL, TextAlign::CENTER);
        
        // Draw Value (1-30)
        int val_int = static_cast<int>(vals[i]);
        if (val_int > 99) val_int = 99;
        std::string val_str = std::to_string(val_int);
        font_.DrawText(renderer_, lx, ly+5, val_str, {100,255,200,255}, FontSize::SMALL, TextAlign::CENTER);
    }
    


    // 3. Metadata (Selected Song Title/Artist) - MOVED TO HUD OVERLAY
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

void GameWindow::RenderGameplay() {
    RenderBackground();
    RenderLanes();
    RenderMeasureLines();
    RenderNotes();
    RenderReceptors();
    RenderHitFlashes();
    RenderJudgement();
    RenderHUD(); // Contains Top/Bottom/Progress/Life bars
    
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
    int num_cols = current_chart_->num_columns;
    int lane_w = GetLaneWidth();
    int receptor_y = static_cast<int>(field_config_.receptor_y);

    // --- Per-lane flash textures (kept from before) ---
    for (int i = 0; i < num_cols; ++i) {
        int x = GetLaneX(i);
        const auto& lane = input_.GetLaneState(i);

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

            if (field_config_.downscroll) {
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

    // --- Arcade: Horizontal "Judgment Line" across all lanes ---
    int field_left = GetFieldLeft();
    int field_right = GetFieldRight();
    uint8_t line_alpha = static_cast<uint8_t>(100 + static_cast<int>(beat_flash_ * 155));
    SDL_SetRenderDrawColor(renderer_, 80, 220, 255, line_alpha);
    SDL_Rect judge_line = { field_left, receptor_y - 1, field_right - field_left, 2 };
    SDL_RenderFillRect(renderer_, &judge_line);
    
    // Add glowing edges
    SDL_SetRenderDrawColor(renderer_, 150, 240, 255, line_alpha / 2);
    SDL_Rect judge_line_glow = { field_left, receptor_y - 2, field_right - field_left, 4 };
    SDL_RenderFillRect(renderer_, &judge_line_glow);

    // --- SMX-style: Unified padding lines on left and right edges ---
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

    // We mask the playfield area
    int left = GetFieldLeft() - 10;
    int right = GetFieldRight() + 10;
    int w = right - left;

    if (sudden_plus_val_ > 0.0f) {
        int mask_h = static_cast<int>(height_ * sudden_plus_val_);
        int y = field_config_.downscroll ? (height_ - mask_h) : 0;
        DrawRect(left, y, w, mask_h, {0, 0, 0, 255});
        DrawRect(left, field_config_.downscroll ? y : mask_h - 2, w, 2, {255, 255, 255, 100}); // Guide line
    }

    if (hidden_plus_val_ > 0.0f) {
        int mask_h = static_cast<int>(height_ * hidden_plus_val_);
        int y = field_config_.downscroll ? 0 : (height_ - mask_h);
        DrawRect(left, y, w, mask_h, {0, 0, 0, 255});
        DrawRect(left, field_config_.downscroll ? mask_h - 2 : y, w, 2, {255, 255, 255, 100}); // Guide line
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
}

void GameWindow::RenderHitFlashes() {
    if (!note_texture_ || hit_flashes_.empty()) return;

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
    if (judgement_timer_ <= 0.0 || last_judgement_ == Judgement::NONE) return;

    // Center the judgement text on the note field
    int field_center_x = (GetFieldLeft() + GetFieldRight()) / 2;
    int judge_y = height_ / 2;

    // Judgement text with fade
    double alpha_mult = std::min(1.0, judgement_timer_ / 0.15);
    Color jcol = GetJudgementColor(last_judgement_, ex_mode_);
    
    // Flashing effect for highest judgement
    bool is_highest = (ex_mode_ && last_judgement_ == Judgement::PEXTRA) || 
                      (!ex_mode_ && last_judgement_ == Judgement::PCRIT);
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
            switch (last_judgement_) {
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
            switch (last_judgement_) {
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
            double elapsed = duration - judgement_timer_;
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
            bool is_highest = (ex_mode_ && last_judgement_ == Judgement::PEXTRA) || 
                              (!ex_mode_ && last_judgement_ == Judgement::PCRIT);
            
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
        const char* name = JudgementName(last_judgement_, ex_mode_);
        int text_width = GetTextWidth(name, 3);
        DrawText(field_center_x - text_width / 2, judge_y - 10, name, jcol, 3);
    }

    // Timing error indicator (FAST/SLOW) — Placed ABOVE the judgement
    // Hide for highest judgements as per request
    is_highest = (ex_mode_ && last_judgement_ == Judgement::PEXTRA) ||
                 (!ex_mode_ && last_judgement_ == Judgement::PCRIT);

    if (last_judgement_ != Judgement::MISS && last_judgement_ != Judgement::NONE && !is_highest) {
        std::string err_text = last_timing_error_ < 0 ? "FAST" : "SLOW";
        Color err_col = last_timing_error_ < 0 ? Color{80, 180, 255, static_cast<uint8_t>(200 * alpha_mult)}
                                                : Color{255, 100, 100, static_cast<uint8_t>(200 * alpha_mult)};
        
        font_.DrawText(renderer_, field_center_x, judge_y - 50, err_text, err_col, FontSize::SMALL, TextAlign::CENTER);
    }

    // Numeric offset — only when debug overlays are active
    if (show_debug_ && last_judgement_ != Judgement::MISS && last_judgement_ != Judgement::NONE) {
        char ms_buf[16];
        std::snprintf(ms_buf, sizeof(ms_buf), "%+.1fms", last_timing_error_ * 1000.0);
        font_.DrawText(renderer_, field_center_x, judge_y + 40, ms_buf, {200, 200, 200, static_cast<uint8_t>(180 * alpha_mult)}, FontSize::SMALL, TextAlign::CENTER);
    }

    // --- Timing error visual bar ---
    if (show_debug_ && last_judgement_ != Judgement::MISS && last_judgement_ != Judgement::NONE) {
        int bar_y = judge_y + 35;
        int bar_w = 100;
        int bar_h = 2;
        int bx = field_center_x - bar_w / 2;
        
        // Background bar
        DrawRect(bx, bar_y, bar_w, bar_h, {100, 100, 120, static_cast<uint8_t>(100 * alpha_mult)});
        DrawRect(field_center_x - 1, bar_y - 2, 2, 6, {150, 150, 180, static_cast<uint8_t>(150 * alpha_mult)}); // Center tick
        
        // Hit marker
        double error_ms = last_timing_error_ * 1000.0; 
        double ratio = std::clamp(error_ms / 100.0, -1.0, 1.0); // ±100ms range
        int tx = field_center_x + static_cast<int>(ratio * (bar_w / 2));
        
        Color tick_col = (error_ms < 0) ? Color{80, 200, 255, 255} : Color{255, 150, 50, 255};
        tick_col.a = static_cast<uint8_t>(255 * alpha_mult);
        DrawRect(tx - 2, bar_y - 3, 4, 8, tick_col);
    }

    // --- Combo display with BOUNCE ---
    if (combo_ > 1) {
        char combo_buf[16];
        std::snprintf(combo_buf, sizeof(combo_buf), "%d", combo_);
        
        // Combo Color based on lowest judgement in chain (DDR-style)
        Color combo_col = {255, 255, 255, 255}; // Default/Highest
        switch (lowest_judgement_in_combo_) {
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
        double bounce_elapsed = bounce_duration - judgement_timer_;
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
    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = field_config_.receptor_y;
    int receptor_yi = static_cast<int>(receptor_y);
    double current_time = play_time_;
    constexpr double FADE_DURATION = 0.5;

    int start_m = std::max(0, static_cast<int>(std::floor((current_beat - 4.0) / 4.0)));
    int end_m = static_cast<int>(std::ceil((current_beat + 20.0) / 4.0));

    // Helper: draw a single beat line with fade logic
    auto drawBeatLine = [&](double beat, uint8_t base_r, uint8_t base_g, uint8_t base_b, uint8_t base_a) {
        double y = NoteRenderer::GetYPosForBeat(beat, conductor_, field_config_);
        bool passed_receptor = field_config_.downscroll ? (y > receptor_y) : (y < receptor_y);

        if (passed_receptor) {
            // Line has passed the receptor — pin it there and fade out
            double beat_time = conductor_.BeatToTime(beat);
            double elapsed = current_time - beat_time;
            if (elapsed < 0.0) elapsed = 0.0;
            if (elapsed >= FADE_DURATION) return; // fully faded
            double fade = 1.0 - (elapsed / FADE_DURATION);
            uint8_t alpha = static_cast<uint8_t>(base_a * fade);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, alpha);
            SDL_Rect line_rect = { GetFieldLeft(), receptor_yi - 1, GetFieldRight() - GetFieldLeft(), 2 };
            SDL_RenderFillRect(renderer_, &line_rect);
        } else {
            // Line hasn't reached receptor yet — draw normally
            if (y < -10 || y > height_ + 10) return;
            int yi = static_cast<int>(y);
            SDL_SetRenderDrawColor(renderer_, base_r, base_g, base_b, base_a);
            SDL_Rect line_rect = { GetFieldLeft(), yi - 1, GetFieldRight() - GetFieldLeft(), 2 };
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
            return std::make_pair(SDL_Color{255, 120, 50, 200}, 200);   // Speed up: orange-red
        } else if (bpm_change < 0) {
            return std::make_pair(SDL_Color{0, 255, 255, 255}, 255);    // Slow down: electric cyan
        }
        if (is_measure) {
            return std::make_pair(SDL_Color{160, 160, 200, 200}, 200);    // Normal measure
        } else {
            return std::make_pair(SDL_Color{120, 120, 150, 140}, 140);       // Normal sub-beat
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
    auto [first, last] = NoteRenderer::GetVisibleNoteRange(
        *current_chart_, conductor_, field_config_, static_cast<double>(height_), 16.0);

    double current_beat = conductor_.GetCurrentBeat();
    double receptor_y = field_config_.receptor_y;

    // Helper to render a hold body
    auto renderHoldBody = [&](int col, double head_beat, double tail_beat, NoteType type, size_t head_idx) {
        double head_y = NoteRenderer::GetYPosForBeat(head_beat, conductor_, field_config_);
        double tail_y = NoteRenderer::GetYPosForBeat(tail_beat, conductor_, field_config_);
        
        int x = GetLaneX(col);
        int w = GetLaneWidth();

        bool head_hit = IsNoteHit(head_idx, col);
        bool key_held = input_.GetLaneState(col).pressed;
        bool head_passed = head_hit || (field_config_.downscroll ? head_y > receptor_y : head_y < receptor_y);
        bool tail_past_receptor = field_config_.downscroll
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
            if (field_config_.downscroll) {
                if (tail_y > receptor_y) tail_render_y = receptor_y;
            } else {
                if (tail_y < receptor_y) tail_render_y = receptor_y;
            }
        }

        int body_y_start, body_y_end;
        if (field_config_.downscroll) {
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
            // Darken inactive holds
            if (!hold_active && head_passed) {
                SDL_SetTextureColorMod(body_tex, 100, 100, 100);
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
            if (!hold_active && head_passed) {
                SDL_SetTextureColorMod(body_tex, 255, 255, 255);
            }
        }

        // Draw cap at the tail position (only when tail hasn't passed receptor)
        if (!tail_past_receptor) {
            SDL_Texture* cap_tex = (type == NoteType::HoldHead) ? hold_cap_texture_ : roll_cap_texture_;
            if (cap_tex) {
                if (!hold_active && head_passed) {
                    SDL_SetTextureColorMod(cap_tex, 100, 100, 100);
                }
                int cap_h = static_cast<int>(w * 0.5);
                // Position cap at the END of the body, not centered on tail_y
                int cap_y = field_config_.downscroll
                    ? (static_cast<int>(tail_y) - cap_h)  // Above the body top
                    : static_cast<int>(tail_y);            // Below the body bottom
                SDL_Rect cap_dst = { x, cap_y, w, cap_h };
                // Cap points UP in downscroll, DOWN in upscroll
                SDL_RendererFlip flip = field_config_.downscroll ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer_, cap_tex, nullptr, &cap_dst, 0.0, nullptr, flip);
                if (!hold_active && head_passed) {
                    SDL_SetTextureColorMod(cap_tex, 255, 255, 255);
                }
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
        double y = NoteRenderer::GetYPosForBeat(row.beat, conductor_, field_config_);
        
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
                    bool key_held = input_.GetLaneState(col).pressed;
                    bool tail_still_active = (tail_beat > 0.0) && (play_time_ < tail_time);
                    if (key_held || tail_still_active) {
                        DrawNote(col, static_cast<int>(receptor_y), row.beat, type);
                    }
                    // After tail passes, head disappears
                } else if (!hit) {
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
    else if (accuracy >= 80.0)   acc_color = {80, 160, 255, 255};  // B
    
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
                int lqrow = GetQuantizationRow(beat);
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

int GameWindow::GetLaneWidth() const {
    double scale = static_cast<double>(width_) / 900.0;
    return static_cast<int>(64.0 * scale);
}

int GameWindow::GetFieldLeft() const {
    return 100; // Moved 100px away from the border for 1P
}

int GameWindow::GetFieldRight() const {
    int num_cols = current_chart_ ? current_chart_->num_columns : 4;
    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    return GetFieldLeft() + num_cols * GetLaneWidth() + (num_cols - 1) * scaled_padding;
}

int GameWindow::GetLaneX(int lane) const {
    int actual_lane = lane;
    if (effect_mode_ == 1 && current_chart_) { // Mirror
        actual_lane = current_chart_->num_columns - 1 - lane;
    }

    double scale = static_cast<double>(width_) / 900.0;
    int scaled_padding = static_cast<int>(LANE_PADDING * scale);
    return GetFieldLeft() + actual_lane * (GetLaneWidth() + scaled_padding);
}

// ============================================================================
// Built-in test chart (unchanged)
// ============================================================================


void GameWindow::UpdateScores(double abs_error, int num_notes) {
    Judgement j_norm = ClassifyHit(abs_error, false);
    Judgement j_ex = ClassifyHit(abs_error, true);

    int ni = static_cast<int>(j_norm) - 1;
    if (ni >= 0 && ni < 9) normal_judge_counts_[ni] += num_notes;

    int ei = static_cast<int>(j_ex) - 1;
    if (ei >= 0 && ei < 9) ex_judge_counts_[ei] += num_notes;

    // Add weights to actual score variables
    normal_score_ += JudgeWeight(j_norm, false) * num_notes;
    ex_score_     += JudgeWeight(j_ex, true) * num_notes;

    // Update Life
    life_meter_.OnJudgement(j_norm);
}

void GameWindow::UpdateClearType(Judgement j_norm, Judgement j_ex) {
    // 1. All Perfect Extraordinary: All notes are P-Extraordinary
    if (j_ex != Judgement::PEXTRA) {
        if (clear_type_ == ClearType::ALL_PERFECT_EXTRAORDINARY)
            clear_type_ = ClearType::ALL_PERFECT_CRITICAL;
    }

    // 2. All Perfect Critical: All notes are P-Critical (or higher in EX mode)
    if (j_norm > Judgement::PCRIT) {
        if (static_cast<int>(clear_type_) >= static_cast<int>(ClearType::ALL_PERFECT_CRITICAL))
            clear_type_ = ClearType::ALL_PERFECT_PLUS;
    }

    // 3. All Perfect+: All notes are Perfect or higher
    if (j_norm > Judgement::PERFECT) {
        if (static_cast<int>(clear_type_) >= static_cast<int>(ClearType::ALL_PERFECT_PLUS))
            clear_type_ = ClearType::ALL_PERFECT;
    }

    // 4. All Perfect: All notes are Perfect(Low) or higher
    if (j_norm > Judgement::PERFECT_LOW) {
        if (static_cast<int>(clear_type_) >= static_cast<int>(ClearType::ALL_PERFECT))
            clear_type_ = ClearType::FULL_COMBO_PLUS;
    }

    // 5. Full Combo+: Great(Low) or higher (Normal) OR Okay(Low) or higher (EX)
    // Note: OKAY_LOW in JudgeWindowsEX is 100ms, GREAT_LOW in JudgeWindows is 100ms.
    if (j_norm > Judgement::GREAT_LOW) {
        if (static_cast<int>(clear_type_) >= static_cast<int>(ClearType::FULL_COMBO_PLUS))
            clear_type_ = ClearType::FULL_COMBO;
    }

    // 6. Full Combo: Good or higher
    if (j_norm > Judgement::GOOD) {
        if (static_cast<int>(clear_type_) >= static_cast<int>(ClearType::FULL_COMBO))
            clear_type_ = ClearType::CLEAR;
    }

    // 7. Clear: Cleared the chart (missed a note but finished)
    if (j_norm == Judgement::MISS) {
        clear_type_ = ClearType::CLEAR;
    }
}

} // namespace sml

#endif // HAS_SDL2
