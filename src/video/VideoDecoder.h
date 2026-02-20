#pragma once

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <SDL.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace sml {

class VideoDecoder {
public:
    VideoDecoder();
    ~VideoDecoder();

    bool Load(const std::string& filepath, SDL_Renderer* renderer);
    void Update(double current_time);
    void Render(SDL_Renderer* renderer, const SDL_Rect* dest);
    
    // Returns the current texture (internal). Maybe nullptr if not ready.
    SDL_Texture* GetTexture() const { return texture_; }
    bool IsLoaded() const { return is_loaded_; }

private:
    void Cleanup();
    bool InitScaler(int src_w, int src_h, AVPixelFormat src_fmt);
    void DecodingLoop();

    struct DecodedFrame {
        double pts;
        std::vector<uint8_t> rgba_data;
    };

    // FFmpeg context
    AVFormatContext* fmt_ctx_ = nullptr;
    AVCodecContext* codec_ctx_ = nullptr;
    int video_stream_idx_ = -1;
    const AVCodec* codec_ = nullptr;

    AVFrame* frame_ = nullptr;      // Decoded frame (YUV usually)
    AVFrame* last_good_frame_ = nullptr; // Latest valid decoded frame
    AVFrame* frame_rgba_ = nullptr; // Converted frame (RGBA)
    AVPacket* packet_ = nullptr;
    SwsContext* sws_ctx_ = nullptr;

    // SDL Resources
    SDL_Texture* texture_ = nullptr;
    SDL_Renderer* renderer_ref_ = nullptr; // Weak ref

    // State
    bool is_loaded_ = false;
    double last_decoded_pts_ = -1.0;
    double first_pts_seconds_ = -1.0;
    double time_base_ = 0.0; // Time base of the video stream (seconds per tick)
    
    // Buffer for RGBA data
    uint8_t* rgba_buffer_ = nullptr;
    int buffer_size_ = 0;

    // Threading
    std::thread decode_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool quit_flag_ = false;
    double current_playback_time_ = 0.0;
    
    std::queue<DecodedFrame> frame_queue_;
    static constexpr size_t MAX_QUEUE_SIZE = 3;
};

} // namespace sml
