#include <Ap4.h>
#include <neaacdec.h>
#include <opus/opus.h>
#include "ALACDecoder.h"

class UnifiedDecoder {
public:
    AudioStreamInfo info {};

    UnifiedDecoder() = default;
    ~UnifiedDecoder() { close(); }

    bool openMp4(const char* path);

    // Decode next block; returns false on end or error
    bool decodeNext(DecodedBlock& out);

    void close();

private:
    // Bento4
    AP4_ByteStream* m_input = nullptr;
    AP4_File*       m_file  = nullptr;
    AP4_Movie*      m_movie = nullptr;
    AP4_Track*      m_track = nullptr;
    AP4_Ordinal     m_sampleIndex = 0;
    AP4_Ordinal     m_sampleCount = 0;

    // Codec type
    AudioCodec m_codec = AudioCodec::Unknown;

    // AAC (faad2)
    NeAACDecHandle m_aac = nullptr;

    // ALAC
    ALACDecoder* m_alac = nullptr;

    // Opus
    OpusDecoder* m_opus = nullptr;

    // temp buffer for ALAC int32
    std::vector<int32_t> m_alacIntBuf;
};
