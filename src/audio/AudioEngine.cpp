// ============================================================================
// AudioEngine.cpp — SDL2_mixer audio implementation
// ============================================================================

#if HAS_SDL2

#include "audio/AudioEngine.h"
#include <cstdio>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

namespace sml {

// Static instance pointer for the finished callback
AudioEngine* AudioEngine::active_instance_ = nullptr;

// ============================================================================
// Lifecycle
// ============================================================================

AudioEngine::~AudioEngine() {
    Shutdown();
}

bool AudioEngine::Init(int frequency, int channels, int chunk_size) {
    if (initialized_) return true;

    // Init SDL audio if not already done
    if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            last_error_ = std::string("SDL audio init failed: ") + SDL_GetError();
            std::printf("AudioEngine: %s\n", last_error_.c_str());
            return false;
        }
    }

    // Open the mixer
    if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, chunk_size) < 0) {
        last_error_ = std::string("Mix_OpenAudio failed: ") + Mix_GetError();
        std::printf("AudioEngine: %s\n", last_error_.c_str());
        return false;
    }

    // Initialize decoders for common formats
    int flags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_FLAC;
    int initted = Mix_Init(flags);
    if ((initted & flags) != flags) {
        // Not fatal — some formats may still work
        std::printf("AudioEngine: Some audio decoders unavailable: %s\n", Mix_GetError());
    }

    // Set the finished callback
    active_instance_ = this;
    Mix_HookMusicFinished(OnMusicFinished);

    initialized_ = true;
    std::printf("AudioEngine: Initialized (freq=%d, channels=%d, chunk=%d)\n",
        frequency, channels, chunk_size);
    return true;
}

void AudioEngine::Shutdown() {
    UnloadMusic();
    if (initialized_) {
        Mix_HookMusicFinished(nullptr);
        active_instance_ = nullptr;
        Mix_CloseAudio();
        Mix_Quit();
        initialized_ = false;
    }
}

// ============================================================================
// Music loading
// ============================================================================

bool AudioEngine::LoadMusic(const std::string& filepath) {
    if (!initialized_) {
        last_error_ = "Audio not initialized";
        return false;
    }

    // Unload previous music
    UnloadMusic();

    if (!fs::exists(filepath)) {
        last_error_ = "File not found: " + filepath;
        std::printf("AudioEngine: %s\n", last_error_.c_str());
        return false;
    }

    music_ = Mix_LoadMUS(filepath.c_str());
    if (!music_) {
        last_error_ = std::string("Mix_LoadMUS failed: ") + Mix_GetError();
        std::printf("AudioEngine: %s\n", last_error_.c_str());
        return false;
    }

    std::printf("AudioEngine: Loaded '%s'\n", filepath.c_str());
    return true;
}

void AudioEngine::UnloadMusic() {
    Stop();
    if (music_) {
        Mix_FreeMusic(music_);
        music_ = nullptr;
    }
}

// ============================================================================
// Playback controls
// ============================================================================

void AudioEngine::Play(double offset_seconds) {
    if (!music_) return;

    // Set internal tracking
    offset_ = offset_seconds;
    seek_offset_ = std::max(0.0, offset_seconds);
    finished_ = false;

    if (Mix_PlayMusic(music_, 0) < 0) {
        last_error_ = std::string("Mix_PlayMusic failed: ") + Mix_GetError();
        std::printf("AudioEngine: %s\n", last_error_.c_str());
        return;
    }

    // Actually seek to the requested position
    if (seek_offset_ > 0.0) {
        Mix_SetMusicPosition(seek_offset_);
    }

    play_start_perf_ = SDL_GetPerformanceCounter();
    playing_ = true;
    paused_ = false;

    std::printf("AudioEngine: Playing at %.4fs (raw time from SDL_GetPerformanceCounter)\n", seek_offset_);
}

void AudioEngine::Pause() {
    if (!playing_ || paused_) return;
    Mix_PauseMusic();
    pause_perf_ = SDL_GetPerformanceCounter();
    paused_ = true;
}

void AudioEngine::Resume() {
    if (!paused_) return;
    Mix_ResumeMusic();
    // Adjust start ticks by the pause duration
    // We move the start time forward by the duration of the pause
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t duration = now - pause_perf_;
    play_start_perf_ += duration;
    paused_ = false;
}

void AudioEngine::Stop() {
    if (playing_) {
        Mix_HaltMusic();
        playing_ = false;
        paused_ = false;
    }
}

void AudioEngine::SeekTo(double seconds) {
    if (!music_ || !playing_) return;

    // SDL_mixer's Mix_SetMusicPosition behavior varies by format.
    // For OGG/MP3 it seeks from the beginning; for others it may not work.
    if (Mix_SetMusicPosition(seconds) < 0) {
        std::printf("AudioEngine: Seek failed: %s\n", Mix_GetError());
        return;
    }

    // Reset our time tracking to match
    play_start_perf_ = SDL_GetPerformanceCounter();
    seek_offset_ = seconds;
    if (paused_) {
        pause_perf_ = play_start_perf_;
    }
}

// ============================================================================
// Time query — THE CRITICAL FUNCTION for Conductor sync
// ============================================================================

double AudioEngine::GetPlaybackTime() const {
    if (!playing_) return 0.0;

    uint64_t now;
    if (paused_) {
        now = pause_perf_;
    } else {
        now = SDL_GetPerformanceCounter();
    }

    double elapsed = static_cast<double>(now - play_start_perf_) / static_cast<double>(SDL_GetPerformanceFrequency());

    // Elapsed time since Play() was called, plus any seek offset.
    // Simfile offset is NO LONGER applied here to avoid double-handling.
    return elapsed + seek_offset_;
}

// ============================================================================
// State queries
// ============================================================================

bool AudioEngine::IsPlaying() const {
    return playing_ && !paused_ && !finished_;
}

bool AudioEngine::IsPaused() const {
    return paused_;
}

bool AudioEngine::IsFinished() const {
    return finished_;
}

void AudioEngine::SetVolume(float volume) {
    int v = static_cast<int>(std::clamp(volume, 0.0f, 1.0f) * MIX_MAX_VOLUME);
    Mix_VolumeMusic(v);
}

// ============================================================================
// Callback
// ============================================================================

void AudioEngine::OnMusicFinished() {
    if (active_instance_) {
        active_instance_->playing_ = false;
        active_instance_->finished_ = true;
        std::printf("AudioEngine: Music finished\n");
    }
}

} // namespace sml

#endif // HAS_SDL2
