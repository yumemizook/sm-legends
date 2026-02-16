#pragma once
// ============================================================================
// AudioEngine.h — SDL2_mixer audio engine for music playback
//
// Wraps SDL2_mixer to load and play song audio files (.ogg, .mp3, .wav).
// Provides a DSP-style time query for syncing with the Conductor.
// ============================================================================

#if HAS_SDL2

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

namespace sml {

/// Audio playback engine using SDL2_mixer.
/// Provides precise playback time tracking for Conductor synchronization.
class AudioEngine {
public:
    AudioEngine() = default;
    ~AudioEngine();

    /// Initialize the audio subsystem with SDL2_mixer.
    /// @param frequency  Sample rate (default: 44100 Hz)
    /// @param channels   Output channels (1=mono, 2=stereo)
    /// @param chunk_size Audio buffer size (smaller = lower latency, default: 1024)
    /// @return true if audio was initialized successfully
    bool Init(int frequency = 44100, int channels = 2, int chunk_size = 1024);

    /// Load a music file from disk.
    /// Supports .ogg, .mp3, .wav, .flac formats.
    /// @param filepath  Full path to the audio file
    /// @return true if the file was loaded successfully
    bool LoadMusic(const std::string& filepath);

    /// Start playing the loaded music from the beginning.
    /// The playback offset is used to sync with the simfile's #OFFSET.
    /// @param offset_seconds  The simfile offset (positive = music leads beat 0)
    void Play(double offset_seconds = 0.0);

    /// Pause playback.
    void Pause();

    /// Resume paused playback.
    void Resume();

    /// Stop playback and reset to beginning.
    void Stop();

    /// Seek to a specific time in the song (seconds).
    void SeekTo(double seconds);

    /// Get the current playback time in seconds.
    /// This is the time value you should pass to Conductor::Update().
    /// Uses SDL_GetTicks with offset tracking for timing.
    [[nodiscard]] double GetPlaybackTime() const;

    /// Check if music is currently playing.
    [[nodiscard]] bool IsPlaying() const;

    /// Check if music is paused.
    [[nodiscard]] bool IsPaused() const;

    /// Check if music has finished playing.
    [[nodiscard]] bool IsFinished() const;

    /// Unload the current music file.
    void UnloadMusic();

    /// Shut down the audio subsystem.
    void Shutdown();

    /// Set the global music volume (0.0 to 1.0).
    void SetVolume(float volume);

    /// Get the last error message.
    [[nodiscard]] const std::string& GetLastError() const { return last_error_; }

private:
    Mix_Music*  music_         = nullptr;
    bool        initialized_   = false;
    bool        playing_       = false;
    bool        paused_        = false;
    bool        finished_      = false;

    // Timing tracking
    uint64_t    play_start_perf_ = 0;  ///< SDL_GetPerformanceCounter when Play() was called
    uint64_t    pause_perf_      = 0;  ///< SDL_GetPerformanceCounter when paused
    double      offset_        = 0.0;     ///< Simfile offset adjustment
    double      seek_offset_   = 0.0;     ///< Accumulated seek offsets

    std::string last_error_;

    /// SDL mixer finished callback (static so it can be a C function pointer).
    static void OnMusicFinished();
    static AudioEngine* active_instance_;
};

} // namespace sml

#endif // HAS_SDL2
