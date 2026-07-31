#include <de/sound/OGG/SoundReader_OGG.h>
#include <de/sound/OGG/SoundReader_OGG_Vorbis.h>
#include <de/sound/OGG/SoundReader_OGG_Opus.h>

#include <de/sound/SND/SoundReader_SND.h>

#include <ogg/ogg.h>

namespace de {
namespace sound {

namespace {

struct OggCodec
{
    enum eCodec {
        Unknown,
        Vorbis,
        Opus,
        Flac,
        Speex,
        Theora,
        Pcm,
        Dirac,
        Cmml,
        Skeleton // Fishead
    };

    uint8_t m_codec = Unknown;

    OggCodec() noexcept : m_codec(Unknown) {}
    OggCodec(eCodec codec) noexcept : m_codec(codec) {}
    operator int32_t() const noexcept { return m_codec; }
    std::string str() const { return getString(m_codec); }

    static std::string getString(int codec)
    {
        switch (codec)
        {
        case OggCodec::Vorbis: return "Vorbis";
        case OggCodec::Opus: return "Opus";
        case OggCodec::Flac: return "Flac";
        case OggCodec::Speex: return "Speex";
        case OggCodec::Theora: return "Theora";
        case OggCodec::Pcm: return "Pcm";
        case OggCodec::Dirac: return "Dirac";
        case OggCodec::Cmml: return "Ccml"; // CMML (Continuous Media Markup Language)
        case OggCodec::Skeleton: return "Skeleton"; // (Fishead meta‑track), not audio
        default: return "Unknown";
        }
    }
};

OggCodec detectOggCodec(const std::string& uri)
{
    File file(uri, eFileMode::Read);
    if (!file.is_open())
    {
        return OggCodec::Unknown;
    }

    // --- Step 1: read ONLY 4 bytes ---
    uint8_t magic[4];
    if (file.read(magic, 4) != 4)
        return OggCodec::Unknown;

    // --- Step 2: verify Ogg container magic BEFORE ANY ogg calls ---
    if (memcmp(magic, "OggS", 4) != 0)
        return OggCodec::Unknown; // NOT OGG

    // --- Step 3: SEEK BACK TO START ---
    if (file.seek(0) < 0)
        return OggCodec::Unknown;

    ogg_sync_state oy;
    ogg_sync_init(&oy);

    char* buffer = ogg_sync_buffer(&oy, 4096);
    size_t bytes = file.read(buffer, 4096);
    ogg_sync_wrote(&oy, bytes);

    ogg_page og;
    if (ogg_sync_pageout(&oy, &og) != 1)
    {
        ogg_sync_clear(&oy);
        return OggCodec::Unknown;
    }

    ogg_stream_state os;
    ogg_stream_init(&os, ogg_page_serialno(&og));
    ogg_stream_pagein(&os, &og);

    ogg_packet op;
    if (ogg_stream_packetout(&os, &op) != 1)
    {
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return OggCodec::Unknown;
    }

    const unsigned char* p = op.packet;

    if (op.bytes >= 7 && std::memcmp(p, "\x01" "vorbis", 7) == 0)   // 01 76 6F 72 62 69 73
        return OggCodec::Vorbis;                                    // { 0x01 'v' 'o' 'r' 'b' 'i' 's' }

    if (op.bytes >= 8 && std::memcmp(p, "OpusHead", 8) == 0)        // 4F 70 75 73 48 65 61 64
        return OggCodec::Opus;

    if (op.bytes >= 5 && std::memcmp(p, "\x7F" "FLAC", 5) == 0)     // 7F 46 4C 41 43
        return OggCodec::Flac;                                      // { 0x7F, 'F', 'L', 'A', 'C', '\0' }

    if (op.bytes >= 8 && std::memcmp(p, "Speex   ", 8) == 0)        // 53 70 65 65 78 20 20 20
        return OggCodec::Speex;

    if (op.bytes >= 7 && std::memcmp(p, "\x80" "theora", 7) == 0)   // 80 74 68 65 6F 72 61
        return OggCodec::Theora;                                    // { 0x80 't' 'h' 'e' 'o' 'r' 'a' }

    if (op.bytes >= 8 && std::memcmp(p, "PCM     ", 8) == 0)        // 50 43 4D 20 20 20 20 20
        return OggCodec::Pcm;

    if (op.bytes >= 4 && std::memcmp(p, "BBCD", 4) == 0)            // Dirac magic
        return OggCodec::Dirac;

    if (op.bytes >= 5 && std::memcmp(p, "\x00" "cmml", 5) == 0)     // 00 63 6D 6D 6C
        return OggCodec::Cmml;

    if (op.bytes >= 8 && std::memcmp(p, "fishead", 7) == 0)         // 66 69 73 68 65 61 64
        return OggCodec::Skeleton;

    // PCM-in-Ogg is not standardized; you can define your own magic here.
    // For now, treat unknown as PCM only if you want.
    return OggCodec::Unknown;
}

} // end namespace.

bool
load_sound_ogg(
    Sound & sound,
    const std::string & uri,
    const SoundLoadOptions& options)
{
    OggCodec codec = detectOggCodec(uri);
    DE_BENNI("Detected OGG Codec: ",codec.str(), ", uri = ",uri)

    switch (codec)
    {
        case OggCodec::Vorbis:
            //DE_ERROR("Unsupported OGG Vorbis format. ",uri)
            return load_sound_ogg_vorbis(sound, uri, options);

        case OggCodec::Opus:
            // DE_ERROR("Unsupported OGG Opus format. ",uri)
            return load_sound_ogg_opus(sound, uri, options);

        case OggCodec::Speex:
            // TODO: loadSpeex(path, snd);
            DE_ERROR("Unsupported OGG Speex format. ",uri)
            return false;

        case OggCodec::Flac:
            // TODO: loadOggFlac(path, snd);
            DE_ERROR("Unsupported OGG Flac format. ",uri)
            return false;

        case OggCodec::Pcm:
            // TODO: loadOggPcm(path, snd);
            DE_ERROR("Unsupported OGG Xiph PCM format. ",uri)
            return false;

        case OggCodec::Theora:
            DE_ERROR("Unsupported OGG Theora format. ",uri)
            return false;

        case OggCodec::Dirac:
            DE_ERROR("Unsupported OGG Dirac format. ",uri)
            return false;

        // case OggCodec::Skeleton:
        // case OggCodec::Cmml:
        default:
            DE_ERROR("Unsupported OGG, fallback to libSNDFILE. ",uri)
            return load_sound_sndfile(sound, uri, options);
    }

    return false;
}

} // end namespace sound.
} // end namespace de.
