// ============================================================================
// gui_main.cpp 窶・Entry point for the SDL2 visual test GUI
//
// Usage:
//   sm-legends-gui                            # Scan 'charts' folder (default)
//   sm-legends-gui --test                     # Load built-in test chart
//   sm-legends-gui <songs_directory>          # Scan directory for songs
//   sm-legends-gui <simfile.sm|.ssc>          # Load single file
//   sm-legends-gui <simfile.sm|.ssc> <index>  # Load specific chart
// ============================================================================

#include <cstdio>
#include <cstring>

#if HAS_SDL2

#include "gui/GameWindow.h"
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // Ensure the working directory is the root of the project (parent of 'build')
    // so that 'assets', 'charts', and 'sprites' can be found regardless of launch location.
    try {
        fs::path exe_path = fs::absolute(fs::path(argv[0]));
        fs::path exe_dir = exe_path.parent_path();
        
        // If the executable is in a 'build' or 'bin' folder, go up one level to the root
        if (exe_dir.filename() == "build" || exe_dir.filename() == "bin" || exe_dir.filename() == "Debug" || exe_dir.filename() == "Release") {
            fs::current_path(exe_dir.parent_path());
        } else {
            fs::current_path(exe_dir);
        }
    } catch (const std::exception& e) {
        std::printf("Warning: Could not set working directory: %s\n", e.what());
    }

    freopen("debug_out.txt", "w", stdout);
    freopen("debug_err.txt", "w", stderr);
    
    sml::GameWindow window;

    if (!window.Init(1000, 700, "SM-Legends")) {
        return 1;
    }

    if (argc >= 2) {
        std::string arg = argv[1];

        if (arg == "--test") {
            std::printf("Loading built-in test chart (--test).\n");
            window.LoadTestChart();
        } else if (fs::is_directory(arg)) {
            // Scan directory for songs
            int count = window.ScanSongs(arg);
            if (count == 0) {
                std::printf("No songs found in '%s'. Falling back to test chart.\n", arg.c_str());
                window.LoadTestChart();
            }
        } else if (fs::is_regular_file(arg)) {
            // Load a single simfile
            int chart_idx = (argc >= 3) ? std::atoi(argv[2]) : 0;
            if (!window.LoadSimfile(arg, chart_idx)) {
                std::printf("Failed to load simfile. Falling back to test chart.\n");
                window.LoadTestChart();
            }
        } else {
            std::printf("Path not found: %s\n", arg.c_str());
            std::printf("Falling back to test chart.\n");
            window.LoadTestChart();
        }
    } else {
        // No path specified, default to 'charts' folder
        if (fs::exists("charts") && fs::is_directory("charts")) {
            std::printf("Scanning default 'charts' folder...\n");
            int count = window.ScanSongs("charts");
            if (count == 0) {
                std::printf("No songs found in 'charts' folder.\n");
                std::printf("Usage:\n");
                std::printf("  %s                      Scan 'charts' folder (default)\n", argv[0]);
                std::printf("  %s --test               Load built-in test chart\n", argv[0]);
                std::printf("  %s <songs_directory>    Scan specific directory\n", argv[0]);
                std::printf("\n");
                std::printf("Falling back to test chart.\n");
                window.LoadTestChart();
            }
        } else {
            std::printf("Default 'charts' folder not found.\n");
            std::printf("Usage:\n");
            std::printf("  %s                      Scan 'charts' folder (default)\n", argv[0]);
            std::printf("  %s --test               Load built-in test chart\n", argv[0]);
            std::printf("  %s <songs_directory>    Scan specific directory\n", argv[0]);
            std::printf("\n");
            std::printf("Falling back to test chart.\n");
            window.LoadTestChart();
        }
    }

    std::printf("\n--- Song Select Controls ---\n");
    std::printf("  Up / Down    : Navigate songs\n");
    std::printf("  Left / Right : Navigate charts\n");
    std::printf("  Enter        : Play selected chart\n");
    std::printf("  T            : Load test chart\n");
    std::printf("  Escape       : Quit\n");
    std::printf("\n--- Gameplay Controls ---\n");
    std::printf("  D F J K      : Lane keys (4-key)\n");
    std::printf("  ZX./         : Alt lane keys (4-key)\n");
    std::printf("  Arrows       : Alt lane keys (4-key)\n");
    std::printf("  Space        : Play / Pause\n");
    std::printf("  R            : Reset to beginning\n");
    std::printf("  F2 / F3      : Speed up / down\n");
    std::printf("  Tab          : Toggle X-Mod / C-Mod\n");
    std::printf("  F4           : Toggle Downscroll\n");
    std::printf("  F1           : Toggle debug overlay\n");
    std::printf("  Escape       : Back to song select\n");
    std::printf("----------------------------\n\n");

    window.Run();
    window.Shutdown();
    return 0;
}

#else

int main() {
    std::printf("Error: SM-Legends GUI requires SDL2.\n");
    std::printf("Install SDL2 via vcpkg:  vcpkg install sdl2\n");
    std::printf("Then rebuild with cmake.\n");
    return 1;
}

#endif
