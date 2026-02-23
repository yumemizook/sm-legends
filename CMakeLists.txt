cmake_minimum_required(VERSION 3.20)
project(sm-legends VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Manual dependency setup - Bypass vcpkg manifest mode to avoid hangs
if(EXISTS "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows")
    set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows")
else()
    set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows")
endif()
message(STATUS "Using local dependencies from: ${VCPKG_ROOT}")

# FFmpeg
set(FFMPEG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg")
foreach(lib avcodec avformat avutil swscale)
    if(NOT TARGET FFmpeg::${lib})
        add_library(FFmpeg::${lib} UNKNOWN IMPORTED)
        set_target_properties(FFmpeg::${lib} PROPERTIES
            IMPORTED_LOCATION "${FFMPEG_ROOT}/lib/${lib}.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_ROOT}/include")
    endif()
endforeach()

# SDL2
if(NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LOCATION_DEBUG "${VCPKG_ROOT}/debug/lib/SDL2d.lib"
        IMPORTED_LOCATION_RELEASE "${VCPKG_ROOT}/lib/SDL2.lib"
        IMPORTED_LOCATION "${VCPKG_ROOT}/lib/SDL2.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_ROOT}/include;${VCPKG_ROOT}/include/SDL2")
endif()

# SDL2_mixer
if(NOT TARGET SDL2_mixer::SDL2_mixer)
    add_library(SDL2_mixer::SDL2_mixer UNKNOWN IMPORTED)
    set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
        IMPORTED_LOCATION_DEBUG "${VCPKG_ROOT}/debug/lib/SDL2_mixerd.lib"
        IMPORTED_LOCATION_RELEASE "${VCPKG_ROOT}/lib/SDL2_mixer.lib"
        IMPORTED_LOCATION "${VCPKG_ROOT}/lib/SDL2_mixer.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_ROOT}/include;${VCPKG_ROOT}/include/SDL2")
endif()

# SDL2_ttf
if(NOT TARGET SDL2_ttf::SDL2_ttf)
    add_library(SDL2_ttf::SDL2_ttf UNKNOWN IMPORTED)
    set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
        IMPORTED_LOCATION_DEBUG "${VCPKG_ROOT}/debug/lib/SDL2_ttfd.lib"
        IMPORTED_LOCATION_RELEASE "${VCPKG_ROOT}/lib/SDL2_ttf.lib"
        IMPORTED_LOCATION "${VCPKG_ROOT}/lib/SDL2_ttf.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_ROOT}/include;${VCPKG_ROOT}/include/SDL2")
endif()

# SDL2main (Required for WIN32 executable)
if(NOT TARGET SDL2::SDL2main)
    add_library(SDL2::SDL2main UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2main PROPERTIES
        IMPORTED_LOCATION_DEBUG "${VCPKG_ROOT}/debug/lib/manual-link/SDL2maind.lib"
        IMPORTED_LOCATION_RELEASE "${VCPKG_ROOT}/lib/manual-link/SDL2main.lib"
        IMPORTED_LOCATION "${VCPKG_ROOT}/lib/manual-link/SDL2main.lib")
endif()

# Explicitly add include directories globally for reliability
include_directories("${VCPKG_ROOT}/include")
include_directories("${VCPKG_ROOT}/include/SDL2")
include_directories("${FFMPEG_ROOT}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

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
    src/video/VideoDecoder.cpp
)

set(GUI_HEADERS
    src/gui/GameWindow.h
    src/gui/FontManager.h
    src/parsing/SongScanner.h
    src/audio/AudioEngine.h
    src/input/InputMapper.h
    src/video/VideoDecoder.h
)

# =============================================================================
# Engine static library (shared between all executables)
# =============================================================================
add_library(sm_engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(sm_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(sm_engine PUBLIC 
    SDL2::SDL2 
    SDL2_mixer::SDL2_mixer 
    SDL2_ttf::SDL2_ttf
)
target_compile_definitions(sm_engine PUBLIC HAS_SDL2=1)

# =============================================================================
# Console demo executable (no SDL2 required)
# =============================================================================
add_executable(sm-legends src/main.cpp)
target_link_libraries(sm-legends PRIVATE sm_engine)

# =============================================================================
# GUI test executable (requires SDL2)
# =============================================================================
add_library(sm_gui STATIC ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(sm_gui PUBLIC sm_engine)

# Link FFmpeg libraries (avcodec, avformat, avutil, swscale)
# Note: FindFFmpeg.cmake or vcpkg usually provides these variables or imported targets.
# vcpkg usually provides FFmpeg::avcodec, etc.
if(TARGET FFmpeg::avcodec)
    target_link_libraries(sm_gui PRIVATE 
        FFmpeg::avcodec 
        FFmpeg::avformat 
        FFmpeg::avutil 
        FFmpeg::swscale
    )
else()
    # Fallback to variables if imported targets aren't defined
    target_link_libraries(sm_gui PRIVATE 
        ${FFMPEG_LIBRARIES}
    )
    target_include_directories(sm_gui PRIVATE ${FFMPEG_INCLUDE_DIRS})
endif()

add_executable(sm-legends-gui src/gui_main.cpp)
target_link_libraries(sm-legends-gui PRIVATE sm_gui)
target_link_options(sm-legends-gui PRIVATE $<$<CONFIG:Debug>:/NODEFAULTLIB:msvcrt.lib>)

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

    # Copy DLLs from vcpkg to the output directory so the executable can run
    add_custom_command(TARGET sm-legends-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<IF:$<CONFIG:Debug>,${VCPKG_ROOT}/debug/bin,${VCPKG_ROOT}/bin>"
        "$<TARGET_FILE_DIR:sm-legends-gui>"
    )
    
    # Copy DLLs from FFmpeg bin directory
    if(EXISTS "${FFMPEG_ROOT}/bin")
        add_custom_command(TARGET sm-legends-gui POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${FFMPEG_ROOT}/bin"
            "$<TARGET_FILE_DIR:sm-legends-gui>"
        )
    endif()
endif()

# =============================================================================
# Test executable (no SDL2 required)
# =============================================================================
add_executable(test_timing tests/test_timing.cpp)
target_link_libraries(test_timing PRIVATE sm_engine)

enable_testing()
add_test(NAME TimingEngineTests COMMAND test_timing)
