#include <iostream>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: probe <file>\n";
        return 1;
    }

    std::string file = argv[1];
    AVFormatContext* fmt_ctx = nullptr;

    if (avformat_open_input(&fmt_ctx, file.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Failed to open input: " << file << "\n";
        return 1;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::cerr << "Failed to find stream info\n";
        return 1;
    }

    std::cout << "Format: " << fmt_ctx->iformat->name << "\n";
    std::cout << "Duration: " << fmt_ctx->duration << "\n";
    std::cout << "Streams: " << fmt_ctx->nb_streams << "\n";

    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVCodecParameters* par = fmt_ctx->streams[i]->codecpar;
        std::cout << " Stream " << i << ": \n";
        std::cout << "   Codec type: " << av_get_media_type_string(par->codec_type) << "\n";
        std::cout << "   Codec ID: " << par->codec_id << "\n";
        
        const AVCodec* codec = avcodec_find_decoder(par->codec_id);
        if (codec) {
            std::cout << "   Codec Name: " << codec->name << " (" << codec->long_name << ")\n";
        } else {
            std::cout << "   Codec: NOT FOUND for ID " << par->codec_id << "\n";
        }

        if (par->codec_type == AVMEDIA_TYPE_VIDEO) {
            std::cout << "   Resolution: " << par->width << "x" << par->height << "\n";
        }
    }

    avformat_close_input(&fmt_ctx);
    return 0;
}
