#include <cstdio>
#include <cstring>
#include <ogg/ogg.h>

/*
⭐ Was dieser Inspector erkennt
Codec	Magic‑Bytes	Erkennung
Vorbis	0x01 'vorbis'	100% sicher
Opus	"OpusHead"	100% sicher
FLAC (Ogg/FLAC)	0x7F 'FLAC'	100% sicher
Speex	"Speex "	100% sicher
Theora	0x80 'theora'	100% sicher
Unknown	alles andere	korrekt
*/

enum class OggCodec {
    Unknown,
    Vorbis,
    Opus,
    Flac,
    Speex,
    Theora
};

OggCodec detect_ogg_codec(const char* path)
{
    FILE* fp = std::fopen(path, "rb");
    if (!fp) return OggCodec::Unknown;

    ogg_sync_state oy;
    ogg_sync_init(&oy);

    char* buffer = ogg_sync_buffer(&oy, 4096);
    size_t bytes = std::fread(buffer, 1, 4096, fp);
    ogg_sync_wrote(&oy, bytes);

    ogg_page og;
    if (ogg_sync_pageout(&oy, &og) != 1) {
        std::fclose(fp);
        ogg_sync_clear(&oy);
        return OggCodec::Unknown;
    }

    ogg_stream_state os;
    ogg_stream_init(&os, ogg_page_serialno(&og));
    ogg_stream_pagein(&os, &og);

    ogg_packet op;
    if (ogg_stream_packetout(&os, &op) != 1) {
        std::fclose(fp);
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return OggCodec::Unknown;
    }

    const unsigned char* p = op.packet;

    if (op.bytes >= 7 && std::memcmp(p, "\x01vorbis", 7) == 0)
        return OggCodec::Vorbis;

    if (op.bytes >= 8 && std::memcmp(p, "OpusHead", 8) == 0)
        return OggCodec::Opus;

    if (op.bytes >= 5 && std::memcmp(p, "\x7FFLAC", 5) == 0)
        return OggCodec::Flac;

    if (op.bytes >= 8 && std::memcmp(p, "Speex   ", 8) == 0)
        return OggCodec::Speex;

    if (op.bytes >= 7 && std::memcmp(p, "\x80theora", 7) == 0)
        return OggCodec::Theora;

    return OggCodec::Unknown;
}

const char* codec_name(OggCodec c)
{
    switch (c) {
        case OggCodec::Vorbis: return "Vorbis";
        case OggCodec::Opus:   return "Opus";
        case OggCodec::Flac:   return "FLAC";
        case OggCodec::Speex:  return "Speex";
        case OggCodec::Theora: return "Theora";
        default:               return "Unknown";
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::printf("Usage: ogg_inspector <file.ogg>\n");
        return 1;
    }

    OggCodec c = detect_ogg_codec(argv[1]);
    std::printf("Codec: %s\n", codec_name(c));
    return 0;
}
