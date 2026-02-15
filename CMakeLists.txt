cmake_minimum_required(VERSION 3.20)
project(sm-legends VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()

# =============================================================================
# Dependencies
# =============================================================================
find_package(SDL2 QUIET)
find_package(SDL2_mixer QUIET)
find_package(SDL2_ttf QUIET)

# If SDL2 is not found via find_package, try pkg-config as a fallback
if(NOT SDL2_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 IMPORTED_TARGET sdl2)
        if(NOT SDL2_mixer_FOUND)
            pkg_check_modules(SDL2_mixer IMPORTED_TARGET SDL2_mixer)
        endif()
        if(NOT SDL2_ttf_FOUND)
            pkg_check_modules(SDL2_ttf IMPORTED_TARGET SDL2_ttf)
        endif()
    endif()
endif()

# =============================================================================
# Source files
# =============================================================================
set(ENGINE_SOURCES
    src/parsing/SimfileParser.cpp
    src/parsing/ChartAnalyzer.cpp
    src/timing/Conductor.cpp
    src/rendering/NoteRenderer.cpp
)

set(ENGINE_HEADERS
    src/timing/TimingData.h
    src/timing/Conductor.h
    src/parsing/Simfile.h
    src/parsing/NoteChart.h
    src/parsing/SimfileParser.h
    src/parsing/ChartAnalyzer.h
    src/rendering/NoteRenderer.h
)

set(GUI_SOURCES
    src/gui/GameWindow.cpp
    src/gui/FontManager.cpp
    src/parsing/SongScanner.cpp
    src/audio/AudioEngine.cpp
)

set(GUI_HEADERS
    src/gui/GameWindow.h
    src/gui/FontManager.h
    src/parsing/SongScanner.h
    src/audio/AudioEngine.h
    src/input/InputMapper.h
)

# =============================================================================
# Engine static library (shared between all executables)
# =============================================================================
add_library(sm_engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(sm_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(SDL2_FOUND)
    target_link_libraries(sm_engine PUBLIC SDL2::SDL2)
    target_compile_definitions(sm_engine PUBLIC HAS_SDL2=1)
    message(STATUS "SDL2 found - building with GUI support")

    # SDL2_mixer
    if(TARGET SDL2_mixer::SDL2_mixer)
        target_link_libraries(sm_engine PUBLIC SDL2_mixer::SDL2_mixer)
        message(STATUS "SDL2_mixer found - building with audio support")
    elseif(TARGET SDL2_mixer::SDL2_mixer-static)
        target_link_libraries(sm_engine PUBLIC SDL2_mixer::SDL2_mixer-static)
        message(STATUS "SDL2_mixer found (static) - building with audio support")
    else()
        message(STATUS "SDL2_mixer not found - building without audio")
    endif()

    # SDL2_ttf
    if(TARGET SDL2_ttf::SDL2_ttf)
        target_link_libraries(sm_engine PUBLIC SDL2_ttf::SDL2_ttf)
        message(STATUS "SDL2_ttf found - building with TTF support")
    elseif(TARGET SDL2_ttf::SDL2_ttf-static)
        target_link_libraries(sm_engine PUBLIC SDL2_ttf::SDL2_ttf-static)
        message(STATUS "SDL2_ttf found (static) - building with TTF support")
    else()
        message(STATUS "SDL2_ttf not found - building without TTF")
    endif()
else()
    message(STATUS "SDL2 not found - building timing engine only (no GUI)")
    target_compile_definitions(sm_engine PUBLIC HAS_SDL2=0)
endif()

# =============================================================================
# Console demo executable (no SDL2 required)
# =============================================================================
add_executable(sm-legends src/main.cpp)
target_link_libraries(sm-legends PRIVATE sm_engine)

# =============================================================================
# GUI test executable (requires SDL2)
# =============================================================================
if(SDL2_FOUND)
    add_library(sm_gui STATIC ${GUI_SOURCES} ${GUI_HEADERS})
    target_link_libraries(sm_gui PUBLIC sm_engine)

    add_executable(sm-legends-gui src/gui_main.cpp)
    target_link_libraries(sm-legends-gui PRIVATE sm_gui)

    # On Windows, use WinMain entry point for release but console for debug
    if(WIN32)
        # Keep console window for debug output
        set_target_properties(sm-legends-gui PROPERTIES
            WIN32_EXECUTABLE $<CONFIG:Release>
        )
        # Link SDL2main for WinMain wrapper
        if(TARGET SDL2::SDL2main)
            target_link_libraries(sm-legends-gui PRIVATE SDL2::SDL2main)
        endif()
    endif()
endif()

# =============================================================================
# Test executable (no SDL2 required)
# =============================================================================
add_executable(test_timing tests/test_timing.cpp)
target_link_libraries(test_timing PRIVATE sm_engine)

enable_testing()
add_test(NAME TimingEngineTests COMMAND test_timing)
