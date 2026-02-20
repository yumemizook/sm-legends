#include "video/VideoDecoder.h"
#include <cstdio>

namespace sml {

VideoDecoder::VideoDecoder() {
    // Allocation of packet/frame should happen here or in Load
    packet_ = av_packet_alloc();
    frame_ = av_frame_alloc();
    frame_rgba_ = av_frame_alloc();
    last_good_frame_ = av_frame_alloc();
}

VideoDecoder::~VideoDecoder() {
    Cleanup();
    if (packet_) av_packet_free(&packet_);
    if (frame_) av_frame_free(&frame_);
    if (frame_rgba_) av_frame_free(&frame_rgba_);
    if (last_good_frame_) av_frame_free(&last_good_frame_);
}

void VideoDecoder::Cleanup() {
    quit_flag_ = true;
    cv_.notify_all();
    if (decode_thread_.joinable()) {
        decode_thread_.join();
    }

    if (sws_ctx_) {
        sws_freeContext(sws_ctx_);
        sws_ctx_ = nullptr;
    }
    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (codec_ctx_) {
        avcodec_free_context(&codec_ctx_);
        codec_ctx_ = nullptr;
    }
    if (fmt_ctx_) {
        avformat_close_input(&fmt_ctx_);
        fmt_ctx_ = nullptr;
    }
    if (rgba_buffer_) {
        av_free(rgba_buffer_);
        rgba_buffer_ = nullptr;
    }
    if (last_good_frame_) {
        av_frame_unref(last_good_frame_);
    }
    is_loaded_ = false;
    video_stream_idx_ = -1;
    first_pts_seconds_ = -1.0;
}

bool VideoDecoder::Load(const std::string& filepath, SDL_Renderer* renderer) {
    Cleanup();
    renderer_ref_ = renderer;

    // 1. Open Input
    if (avformat_open_input(&fmt_ctx_, filepath.c_str(), nullptr, nullptr) < 0) {
        std::fprintf(stderr, "[VideoDecoder] Failed to open video file: %s\n", filepath.c_str());
        return false;
    }

    // 2. Find Stream Info
    if (avformat_find_stream_info(fmt_ctx_, nullptr) < 0) {
        std::fprintf(stderr, "[VideoDecoder] Failed to find stream info\n");
        return false;
    }

    // 3. Find Video Stream
    video_stream_idx_ = -1;
    for (unsigned int i = 0; i < fmt_ctx_->nb_streams; ++i) {
        if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx_ = i;
            break;
        }
    }

    if (video_stream_idx_ == -1) {
        std::fprintf(stderr, "[VideoDecoder] No video stream found\n");
        return false;
    }

    // 4. Find/Open Codec
    AVCodecParameters* codec_par = fmt_ctx_->streams[video_stream_idx_]->codecpar;
    codec_ = avcodec_find_decoder(codec_par->codec_id);
    if (!codec_) {
        std::fprintf(stderr, "[VideoDecoder] Codec not found\n");
        return false;
    }

    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (avcodec_parameters_to_context(codec_ctx_, codec_par) < 0) {
        std::fprintf(stderr, "[VideoDecoder] Failed to copy codec params\n");
        return false;
    }

    // Optimize for multi-threading if possible
    // codec_ctx_->thread_count = 0; // Let FFmpeg decide. Note: May cause higher CPU spikes on load.

    if (avcodec_open2(codec_ctx_, codec_, nullptr) < 0) {
        std::fprintf(stderr, "[VideoDecoder] Failed to open codec\n");
        return false;
    }

    // Calculate time base
    AVStream* st = fmt_ctx_->streams[video_stream_idx_];
    time_base_ = av_q2d(st->time_base);

    // 5. Setup Texture & Buffer
    int w = codec_ctx_->width;
    int h = codec_ctx_->height;

    // Allocate buffer for RGBA
    buffer_size_ = av_image_get_buffer_size(AV_PIX_FMT_RGBA, w, h, 1);
    rgba_buffer_ = (uint8_t*)av_malloc(buffer_size_ * sizeof(uint8_t));

    // Fill the wrapper frame to point to the buffer
    av_image_fill_arrays(frame_rgba_->data, frame_rgba_->linesize, rgba_buffer_,
                         AV_PIX_FMT_RGBA, w, h, 1);

    // Create SDL Texture (Streaming/Access Static? Streaming is better for frequent updates)
    texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!texture_) {
        std::fprintf(stderr, "[VideoDecoder] Failed to create SDL texture\n");
        return false;
    }
    
    // Create Scaler
    sws_ctx_ = sws_getContext(w, h, codec_ctx_->pix_fmt,
                              w, h, AV_PIX_FMT_RGBA,
                              SWS_BILINEAR, nullptr, nullptr, nullptr);

    is_loaded_ = true;
    last_decoded_pts_ = -1.0;
    first_pts_seconds_ = -1.0;
    current_playback_time_ = -1.0;
    
    // Clear Queue
    while(!frame_queue_.empty()) frame_queue_.pop();

    std::printf("[VideoDecoder] Loaded %s (%dx%d)\n", filepath.c_str(), w, h);
    
    // Start Thread
    quit_flag_ = false;
    decode_thread_ = std::thread(&VideoDecoder::DecodingLoop, this);
    
    return true;
}

void VideoDecoder::DecodingLoop() {
    while (!quit_flag_) {
        // Wait if the queue is full to prevent buffering the whole video into RAM
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return quit_flag_ || frame_queue_.size() < MAX_QUEUE_SIZE; });
        }
        if (quit_flag_) break;

        // Fetch current playback time safely
        double target_time = 0.0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            target_time = current_playback_time_;
        }

        // Handle large jumps
        if (last_decoded_pts_ >= 0.0 && (target_time < last_decoded_pts_ - 1.0 || target_time > last_decoded_pts_ + 5.0)) {
            double real_target_seconds = target_time;
            if (first_pts_seconds_ >= 0.0) {
                real_target_seconds += first_pts_seconds_;
            }
            int64_t target_ts = static_cast<int64_t>(real_target_seconds / time_base_);
            av_seek_frame(fmt_ctx_, video_stream_idx_, target_ts, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(codec_ctx_);
            last_decoded_pts_ = -1.0;
            av_frame_unref(last_good_frame_);
        }

        // Catch up the internal PTS to target time + small buffer
        bool keep_reading = true;
        bool needs_texture_update = false;

        while (last_decoded_pts_ <= target_time + 0.1 && keep_reading && !quit_flag_) {
            int ret = avcodec_receive_frame(codec_ctx_, frame_);
            
            if (ret == 0) {
                // Successfully got a frame from the decoder buffer
                int64_t pts = frame_->best_effort_timestamp;
                if (pts == AV_NOPTS_VALUE) pts = frame_->pts;
                if (pts == AV_NOPTS_VALUE) pts = frame_->pkt_dts;
                
                double frame_duration = 1.0 / 30.0;
                AVRational fps = fmt_ctx_->streams[video_stream_idx_]->avg_frame_rate;
                if (fps.num > 0 && fps.den > 0) {
                    frame_duration = (double)fps.den / fps.num;
                }

                double pts_seconds = -1.0;
                if (pts != AV_NOPTS_VALUE) {
                    pts_seconds = pts * time_base_;
                    if (first_pts_seconds_ < 0.0) {
                        first_pts_seconds_ = pts_seconds;
                    }
                    pts_seconds -= first_pts_seconds_;
                }
                
                // Robust monotonic PTS assignment
                if (pts_seconds < 0.0) {
                    pts_seconds = (last_decoded_pts_ < 0.0) ? 0.0 : last_decoded_pts_ + frame_duration;
                } else {
                    if (last_decoded_pts_ >= 0.0) {
                        double diff = pts_seconds - last_decoded_pts_;
                        if (diff <= 0.0 || diff > 2.0) {
                            pts_seconds = last_decoded_pts_ + frame_duration;
                        }
                    }
                }

                last_decoded_pts_ = pts_seconds;
                av_frame_unref(last_good_frame_);
                av_frame_move_ref(last_good_frame_, frame_);
                needs_texture_update = true;
            }
            else if (ret == AVERROR(EAGAIN)) {
                int pkt_ret = av_read_frame(fmt_ctx_, packet_);
                if (pkt_ret < 0) {
                    if (pkt_ret == AVERROR_EOF) {
                        av_seek_frame(fmt_ctx_, video_stream_idx_, 0, AVSEEK_FLAG_BACKWARD);
                        avcodec_flush_buffers(codec_ctx_);
                    } else {
                        keep_reading = false;
                    }
                } else {
                    if (packet_->stream_index == video_stream_idx_) {
                        int send_ret = avcodec_send_packet(codec_ctx_, packet_);
                        if (send_ret < 0 && send_ret != AVERROR(EAGAIN)) {
                            keep_reading = false;
                        }
                    }
                    av_packet_unref(packet_);
                }
            }
            else {
                keep_reading = false;
            }
        }

        if (needs_texture_update && last_good_frame_->data[0] && !quit_flag_) {
            // Heavy scaling on the background thread
            sws_scale(sws_ctx_,
                      (const uint8_t* const*)last_good_frame_->data, last_good_frame_->linesize,
                      0, codec_ctx_->height,
                      frame_rgba_->data, frame_rgba_->linesize);

            // Create a packet of pixels to push to the main thread
            DecodedFrame dframe;
            dframe.pts = last_decoded_pts_;
            size_t bytes = static_cast<size_t>(frame_rgba_->linesize[0]) * codec_ctx_->height;
            dframe.rgba_data.assign(rgba_buffer_, rgba_buffer_ + bytes);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                frame_queue_.push(std::move(dframe));
            }
            cv_.notify_all(); // Wake up main thread if waiting
        }
        else if (!keep_reading) {
            // Nothing to do, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void VideoDecoder::Update(double current_time) {
    if (!is_loaded_) return;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        // Handle large jumps
        if (current_time < current_playback_time_) { 
            last_decoded_pts_ = current_time;
        }

        current_playback_time_ = current_time;

        // Drain the queue until we find the most applicable frame
        bool updated_texture = false;
        while (!frame_queue_.empty()) {
            auto& front = frame_queue_.front();
            
            // If the frame's PTS is ahead of our time, hold it for the future
            if (front.pts > current_time) {
                break;
            }

            // We are uploading this frame.
            SDL_UpdateTexture(texture_, nullptr, front.rgba_data.data(), frame_rgba_->linesize[0]);
            updated_texture = true;
            frame_queue_.pop();
        }

        if (updated_texture) {
            cv_.notify_all(); // Tell the worker thread to brew more frames
        }
    }
}

void VideoDecoder::Render(SDL_Renderer* renderer, const SDL_Rect* dest) {
    if (texture_) {
        SDL_RenderCopy(renderer, texture_, nullptr, dest);
    }
}

} // namespace sml
