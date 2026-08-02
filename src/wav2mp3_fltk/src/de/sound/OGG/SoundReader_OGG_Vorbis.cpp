#include <de/sound/OGG/SoundReader_OGG_Vorbis.h>

//#include <opusfile.h>

#include <vorbis/vorbisfile.h>

namespace de {
namespace sound {

namespace {

// int32_t file64_open(const char* path, eFileMode fileMode, int32_t permission = 0);
// int32_t file64_close(int32_t fd);
// int32_t file64_read(int32_t fd, void* buf, int64_t bytes);
// int32_t file64_write(int32_t fd, const void* buf, int64_t bytes);
// int64_t file64_seek(int32_t fd, int64_t offset, eSeekMode seekMode);
// int64_t file64_tell(int32_t fd);

struct VorbisIO
{
    int fd;
};

size_t VorbisIO_read(void* ptr, size_t size, size_t nmemb, void* ds)
{
    VorbisIO* src = (VorbisIO*)ds;
    size_t bytes = size * nmemb;
    int r = file64_read(src->fd, ptr, int64_t(bytes));
    return (r < 0 ? 0 : r / size);
}

int VorbisIO_seek(void* ds, ogg_int64_t offset, int whence)
{
    VorbisIO* src = (VorbisIO*)ds;
    eSeekMode seekMode = eSeekMode::Set;
    switch (whence)
    {
        case SEEK_CUR: seekMode = eSeekMode::Cur; break;
        case SEEK_END: seekMode = eSeekMode::End; break;
        default: break;
    }
    int64_t pos = file64_seek(src->fd, (int64_t)offset, seekMode);
    return (pos < 0 ? -1 : 0);
}

long VorbisIO_tell(void* ds)
{
    VorbisIO* src = (VorbisIO*)ds;
    return file64_tell(src->fd);
}

int VorbisIO_close(void* ds)
{
    VorbisIO* src = (VorbisIO*)ds;
    file64_close(src->fd);
    return 0;
}

static ov_callbacks VorbisIO_callbacks = {
    VorbisIO_read,
    VorbisIO_seek,
    VorbisIO_close,
    VorbisIO_tell
};

} // end namespace.

bool load_sound_ogg_vorbis(
    Sound & sound,
    const std::string & uri,
    const SoundLoadOptions& options)
{
    int fd = file64_open(uri.c_str(), eFileMode::Read);
    if (fd < 0)
    {
        DE_ERROR("Cannot open OGG/Vorbis file ", uri)
        return false;
    }

    VorbisIO vio;
    vio.fd = fd;

    OggVorbis_File vf;
    if (ov_open_callbacks(&vio, &vf, nullptr, 0, VorbisIO_callbacks) < 0) // OV_CALLBACKS_NOCLOSE
    {
        VorbisIO_close(&vio);
        DE_ERROR("ov_open_callbacks failed. ", uri)
        return false;
    }

    const vorbis_info* head = ov_info(&vf, -1);
    if (!head)
    {
        VorbisIO_close(&vio);
        DE_ERROR("ov_info failed. ", uri)
        return false;
    }

    sound.m_uri = uri;
    sound.m_sampleType = SampleType::F32;
    sound.m_sampleRate = head->rate;
    sound.m_channels   = head->channels;
    sound.m_flags      = 0; // interleaved
    sound.m_frames     = 0;
    sound.m_samples.clear();

    const int64_t n = ov_pcm_total(&vf, -1);
    if (n > 0)
    {
        sound.m_frames = n;
        sound.m_samples.reserve(n * sound.m_channels * long(sizeof(float)));
    }

    int64_t framesRead = 0;
    float** pcm = nullptr;

    while (true)
    {
        long ret = ov_read_float(&vf, &pcm, 4096, nullptr);
        if (ret == 0)
            break;      // EOF
        if (ret < 0)
        {
            DE_ERROR("ov_read_float failed with ",ret)
            break;
        }

        // Interleave float PCM
        const size_t oldSize = sound.m_samples.size();
        const size_t addBytes = ret * sound.m_channels * sizeof(float);
        sound.m_samples.resize(oldSize + addBytes);

        float* dst = reinterpret_cast<float*>(&sound.m_samples[oldSize]);

        for (long i = 0; i < ret; ++i)
        {
            for (int ch = 0; ch < sound.m_channels; ++ch)
            {
                *dst++ = pcm[ch][i];
            }
        }

        framesRead += ret;
    }
    /*
    while (true)
    {
        long ret = ov_read_float(&vf, &pcm, 4096, nullptr);
        if (ret == 0)
            break;      // EOF
        if (ret < 0)
        {
            DE_ERROR("ov_read_float failed with ",ret)
            break;
        }

        auto beg = reinterpret_cast<const uint8_t*>(pcm);
        auto end = beg + (int64_t(ret) * sound.m_channels * sizeof(float));
        sound.m_samples.insert( sound.m_samples.end(), beg, end);
        framesRead += ret;
    }
    */
    sound.m_frames = framesRead;

    ov_clear(&vf);
    return true;
}

} // end namespace sound.
} // end namespace de.


/*
    while (true)
    {
        long n = ov_read_float(&vf, &pcm, 4096, nullptr);
        if (n <= 0)
            break;

        // Interleave float PCM
        const size_t oldSize = sound.m_samples.size();
        const size_t addBytes = n * sound.m_channels * sizeof(float);
        sound.m_samples.resize(oldSize + addBytes);

        float* dst = reinterpret_cast<float*>(&sound.m_samples[oldSize]);

        for (long i = 0; i < n; ++i)
        {
            for (int ch = 0; ch < sound.m_channels; ++ch)
            {
                *dst++ = pcm[ch][i];
            }
        }

        framesRead += n;
    }
*/
/*

#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

// ------------------------------------------------------------
// Simple Ogg codec detector (first packet magic)
// ------------------------------------------------------------
enum class OggCodec {
    Unknown,
    Vorbis,
    Opus,
    Flac,
    Speex,
    Theora,
    Pcm
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

    // PCM-in-Ogg is not standardized; you can define your own magic here.
    // For now, treat unknown as PCM only if you want.
    return OggCodec::Unknown;
}

// ------------------------------------------------------------
// Vorbis decode path using libvorbisfile
// ------------------------------------------------------------
bool decode_vorbis(const char* path)
{
    OggVorbis_File vf;
    if (ov_fopen(path, &vf) < 0) {
        std::fprintf(stderr, "Not a valid Vorbis file: %s\n", path);
        return false;
    }

    vorbis_info* info = ov_info(&vf, -1);
    std::printf("Vorbis: %d channels, %ld Hz\n", info->channels, info->rate);

    float** pcm;
    long samples;

    while ((samples = ov_read_float(&vf, &pcm, 4096, nullptr)) > 0) {
        // pcm[c][i] = sample
        // You can interleave, process, or feed into your DSP chain here.
        // This example just counts samples.
        std::printf("Decoded %ld samples\n", samples);
    }

    ov_clear(&vf);
    return true;
}

// ------------------------------------------------------------
// Main entry point
// ------------------------------------------------------------
int main(int argc, char** argv)
{
    if (argc < 2) {
        std::printf("Usage: decode <file.ogg>\n");
        return 1;
    }

    const char* path = argv[1];

    OggCodec codec = detect_ogg_codec(path);

    switch (codec) {
        case OggCodec::Vorbis:
            std::printf("Detected: Vorbis\n");
            decode_vorbis(path);
            break;

        case OggCodec::Opus:
            std::printf("Detected: Opus\n");
            // TODO: call your Opus-in-Ogg decoder
            break;

        case OggCodec::Flac:
            std::printf("Detected: Ogg/FLAC\n");
            // TODO: call your FLAC-in-Ogg decoder
            break;

        case OggCodec::Speex:
            std::printf("Detected: Speex\n");
            // TODO: call your Speex-in-Ogg decoder
            break;

        case OggCodec::Theora:
            std::printf("Detected: Theora (video)\n");
            // TODO: handle or reject
            break;

        case OggCodec::Pcm:
            std::printf("Detected: PCM-in-Ogg\n");
            // TODO: call your PCM-in-Ogg decoder
            break;

        default:
            std::printf("Unknown Ogg codec\n");
            break;
    }

    return 0;
}

⭐ Full decode function: bool loadVorbis(const char* path, Sound& snd)
cpp

#include <vorbis/vorbisfile.h>

bool loadVorbis(const char* path, Sound& snd)
{
    snd.m_uri = path;

    OggVorbis_File vf;
    if (ov_fopen(path, &vf) < 0) {
        return false;
    }

    vorbis_info* info = ov_info(&vf, -1);
    snd.m_sampleRate = info->rate;
    snd.m_channels   = info->channels;
    snd.m_sampleType = SampleType::F32;
    snd.m_flags      = 0; // interleaved

    // Total PCM frames (may be -1 for streaming)
    const ogg_int64_t totalFrames = ov_pcm_total(&vf, -1);
    snd.m_frames = (totalFrames > 0 ? totalFrames : 0);

    // Pre-allocate if totalFrames known
    if (snd.m_frames > 0) {
        const size_t bytes = snd.m_frames * snd.m_channels * sizeof(float);
        snd.m_samples.resize(bytes);
    }

    float** pcm = nullptr;
    long framesRead = 0;

    // Decode loop
    while (true) {
        long n = ov_read_float(&vf, &pcm, 4096, nullptr);
        if (n <= 0)
            break;

        // Interleave float PCM
        const size_t oldSize = snd.m_samples.size();
        const size_t addBytes = n * snd.m_channels * sizeof(float);
        snd.m_samples.resize(oldSize + addBytes);

        float* dst = reinterpret_cast<float*>(&snd.m_samples[oldSize]);

        for (long i = 0; i < n; ++i) {
            for (int ch = 0; ch < snd.m_channels; ++ch) {
                *dst++ = pcm[ch][i];
            }
        }

        framesRead += n;
    }

    // If totalFrames was unknown, set it now
    snd.m_frames = framesRead;

    ov_clear(&vf);
    return true;
}

*/
