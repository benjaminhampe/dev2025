/*

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <cstdint>
#include <vector>
#include <string>
#include <cstdio>

enum class SampleType {
    U8,
    S8,
    S16,
    S24,
    S32,
    F32,
    F64
};

struct Sound {
    std::string uri;
    uint32_t sampleRate = 0;
    uint32_t channels   = 0;
    uint64_t frames     = 0;
    SampleType sampleType = SampleType::S16;
    std::vector<uint8_t> data;   // interleaved
    bool interleaved = true;

    uint32_t bytesPerSample() const {
        switch (sampleType) {
            case SampleType::S16: return 2;
            case SampleType::S32: return 4;
            case SampleType::F32: return 4;
            default: return 0;
        }
    }
};

bool loadRM(const std::string& path, Sound& snd)
{
    snd.uri = path;

    av_register_all();

    AVFormatContext* fmt = nullptr;
    if (avformat_open_input(&fmt, path.c_str(), nullptr, nullptr) < 0) {
        std::fprintf(stderr, "Failed to open RM file\n");
        return false;
    }

    if (avformat_find_stream_info(fmt, nullptr) < 0) {
        std::fprintf(stderr, "Failed to read RM stream info\n");
        avformat_close_input(&fmt);
        return false;
    }

    int audioStream = -1;
    for (unsigned i = 0; i < fmt->nb_streams; ++i) {
        if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            break;
        }
    }

    if (audioStream < 0) {
        std::fprintf(stderr, "No audio stream found\n");
        avformat_close_input(&fmt);
        return false;
    }

    AVCodecParameters* cp = fmt->streams[audioStream]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(cp->codec_id);
    if (!codec) {
        std::fprintf(stderr, "No decoder for RM audio codec\n");
        avformat_close_input(&fmt);
        return false;
    }

    AVCodecContext* ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(ctx, cp);

    if (avcodec_open2(ctx, codec, nullptr) < 0) {
        std::fprintf(stderr, "Failed to open RM audio decoder\n");
        avcodec_free_context(&ctx);
        avformat_close_input(&fmt);
        return false;
    }

    snd.sampleRate = ctx->sample_rate;
    snd.channels   = ctx->channels;
    snd.sampleType = SampleType::S16; // we output S16 PCM

    SwrContext* swr = swr_alloc_set_opts(
        nullptr,
        av_get_default_channel_layout(ctx->channels),
        AV_SAMPLE_FMT_S16,
        ctx->sample_rate,
        av_get_default_channel_layout(ctx->channels),
        ctx->sample_fmt,
        ctx->sample_rate,
        0, nullptr
    );
    swr_init(swr);

    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    uint64_t totalFrames = 0;

    while (av_read_frame(fmt, pkt) >= 0) {
        if (pkt->stream_index != audioStream) {
            av_packet_unref(pkt);
            continue;
        }

        if (avcodec_send_packet(ctx, pkt) < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (avcodec_receive_frame(ctx, frame) == 0) {
            int outSamples = swr_get_out_samples(swr, frame->nb_samples);

            int neededBytes = outSamples * ctx->channels * 2;
            size_t oldSize = snd.data.size();
            snd.data.resize(oldSize + neededBytes);

            uint8_t* out = snd.data.data() + oldSize;

            uint8_t* outArr[1] = { out };
            int written = swr_convert(
                swr,
                outArr,
                outSamples,
                (const uint8_t**)frame->extended_data,
                frame->nb_samples
            );

            totalFrames += written;
        }

        av_packet_unref(pkt);
    }

    snd.frames = totalFrames;

    av_frame_free(&frame);
    av_packet_free(&pkt);
    swr_free(&swr);
    avcodec_free_context(&ctx);
    avformat_close_input(&fmt);

    return true;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s input.rm\n", argv[0]);
        return 1;
    }

    Sound snd;
    if (!loadRM(argv[1], snd)) {
        std::fprintf(stderr, "Failed to load RM\n");
        return 1;
    }

    std::printf("Loaded RM:\n");
    std::printf("  URI: %s\n", snd.uri.c_str());
    std::printf("  Rate: %u\n", snd.sampleRate);
    std::printf("  Channels: %u\n", snd.channels);
    std::printf("  Frames: %llu\n", (unsigned long long)snd.frames);
    std::printf("  Bytes: %llu\n", (unsigned long long)snd.data.size());

    return 0;
}

*/