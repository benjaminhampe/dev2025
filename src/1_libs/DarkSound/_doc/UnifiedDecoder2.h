// UnifiedDecoder.h
#pragma once
#include <de/audio/file/Sound.h>

#if 0

#include <string>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstring>

#include <Ap4.h>
extern "C" {
    #include <neaacdec.h>
    #include <opus/opus.h>
}
#include <alac/ALACDecoder.h>

namespace de {
namespace audio {


/*
Sound s;
UnifiedDecoder dec;

if (dec.open(s.m_uri)) {
    dec.fastScanFrameCount(); // optional, fills dec.info.frameCount when possible
    s.m_fileInfo = dec.info;

    if (s.m_fileInfo.frameCount > 0) {
        s.m_samples.resize(s.m_fileInfo.frameCount * s.m_fileInfo.channelCount);
        u64 offset = 0;

        dec.stream([&](const float* src, u64 frames, u32 ch){
            u64 count = frames * ch;
            std::memcpy(s.m_samples.data() + offset, src, count * sizeof(float));
            offset += count;
        }, 4096);
    } else {
        // streaming-only path
        dec.stream([&](const float* src, u64 frames, u32 ch){
            // push to ringbuffer, etc.
        }, 1024);
    }
}
*/

using StreamCallback = std::function<void(const float* interleaved, u64 frames, u32 channels)>;

// You fill these from Bento4 (or whatever demuxer you use)
struct AacConfig {
    const u8* ascData = nullptr;
    u32       ascSize = 0;
};

struct AlacConfig {
    const u8* cookieData = nullptr;
    u32       cookieSize = 0;
    u32       sampleRate = 0;
    u16       channels   = 0;
    u16       bitDepth   = 0;
    u32       samplesPerFrame = 0;
};

struct OpusConfig {
    u32 channels   = 0;
    u32 sampleRate = 48000; // decode rate
};

// You provide compressed packets via this callback
using PacketReader = std::function<bool(const u8*& data, u32& size)>;
// returns false on EOF; data must remain valid until next call

enum class CodecType { AAC, ALAC, OPUS, UNKNOWN };

class UnifiedDecoder
{
public:
    FileInfo info;

    UnifiedDecoder();
    ~UnifiedDecoder();

    bool initAac (const AacConfig&  cfg, const FileInfo& baseInfo, PacketReader reader);
    bool initAlac(const AlacConfig& cfg, const FileInfo& baseInfo, PacketReader reader);
    bool initOpus(const OpusConfig& cfg, const FileInfo& baseInfo, PacketReader reader);

    // optional: for Opus, decode all packets and sum frames
    bool fastScanFrameCount();

    // decode and push chunks to callback
    bool stream(const StreamCallback& cb, u32 chunkFrames = 1024);

    void close();

private:
    CodecType    m_codec    = CodecType::UNKNOWN;
    PacketReader m_reader   = nullptr;

    // decoders
    NeAACDecHandle m_aac  = nullptr;
    ALACDecoder*   m_alac = nullptr;
    OpusDecoder*   m_opus = nullptr;

    // ALAC specifics
    u16 m_bitDepth = 0;
    u32 m_alacSpf  = 0;

    // temp buffers
    std::vector<int32_t> m_alacIntBuf;
    std::vector<float>   m_chunkBuf;

    bool decodeNextPacket(float*& out, u32& frames); // frames = per channel
};

#if 0
using StreamCallback = std::function<void(const float* interleaved, u64 frames, u32 channels)>;

class UnifiedDecoder
{
public:
    FileInfo info;

    UnifiedDecoder() = default;
    ~UnifiedDecoder() { close(); }

    bool open(const std::string& uri);
    bool fastScanFrameCount();                 // optional, fills info.frameCount if possible
    bool stream(const StreamCallback& cb,
                u32 chunkFrames = 1024);       // decode and push chunks to callback

    void close();

private:
    enum Codec { AAC, ALAC, OPUS, UNKNOWN };

    // Bento4
    AP4_ByteStream* m_input  = nullptr;
    AP4_File*       m_file   = nullptr;
    AP4_Movie*      m_movie  = nullptr;
    AP4_Track*      m_track  = nullptr;
    AP4_Ordinal     m_sampleIndex = 0;
    AP4_Ordinal     m_sampleCount = 0;

    Codec m_codec = UNKNOWN;

    // decoders
    NeAACDecHandle m_aac  = nullptr;
    ALACDecoder*   m_alac = nullptr;
    OpusDecoder*   m_opus = nullptr;

    // temp buffers
    std::vector<int32_t> m_alacIntBuf;
    std::vector<float>   m_chunkBuf;   // for stream() accumulation

    bool initAac(AP4_SampleDescription* sd, AP4_AudioSampleDescription* asd);
    bool initAlac(AP4_SampleDescription* sd, AP4_AudioSampleDescription* asd);
    bool initOpus(AP4_SampleDescription* sd, AP4_AudioSampleDescription* asd);

    bool decodeNextPacket(float*& out, u32& frames); // frames = per channel
};
#endif

} // end namespace audio.
} // end namespace de.

#endif