#include <de/sound/FLAC/SoundWriter_FLAC_libflac-1.3.3.h>

#include <FLAC/stream_encoder.h>

namespace de {
namespace sound {

namespace {
// ------------------------------------------------------------
// Converter function type
// ------------------------------------------------------------
using ConverterFn = FLAC__int32 (*)(const uint8_t*);

// ------------------------------------------------------------
// Converters (one per SampleType)
// ------------------------------------------------------------

// converters

static FLAC__int32 convU8(const uint8_t* p)
{
    int32_t s = int32_t(p[0]) - 128;
    return s << 8; // S16
}

static FLAC__int32 convS8(const uint8_t* p)
{
    int8_t s = (int8_t)p[0];
    return int32_t(s) << 8; // S16
}

static FLAC__int32 convS16(const uint8_t* p)
{
    int16_t s = int16_t(p[0] | (p[1] << 8));
    return s; // S16
}

static FLAC__int32 convS24(const uint8_t* p)
{
    return (int32_t(p[2]) << 16) |
           (int32_t(p[1]) << 8)  |
           (int32_t(p[0]) << 0);
}

static FLAC__int32 convS32(const uint8_t* p)
{
    int32_t s = *(const int32_t*)p;
    return s >> 8; // top 24 bits
}

static FLAC__int32 convF32(const uint8_t* p)
{
    float f = *(const float*)p;
    if (f > 1.0f) f = 1.0f;
    if (f < -1.0f) f = -1.0f;
    return (FLAC__int32)(f * 0x7FFFFF);
}

static FLAC__int32 convF64(const uint8_t* p)
{
    double f = *(const double*)p;
    if (f > 1.0) f = 1.0;
    if (f < -1.0) f = -1.0;
    return (FLAC__int32)(f * 0x7FFFFF);
}

/*
+-------------------+---------------------------+------------------------------+
| Input PCM         | libFLAC expects          | Conversion needed?           |
+-------------------+---------------------------+------------------------------+
| U8                | S16                       | Yes (unsigned -> signed)     |
| S8                | S16                       | Yes (8-bit unsupported)      |
| S16               | S16                       | No                           |
| S24               | S24                       | No                           |
| S32               | S24                       | Yes (truncate)               |
| F32               | S24                       | Yes (scale float)            |
| F64               | S24                       | Yes (scale float)            |
+-------------------+---------------------------+------------------------------+
*/

static ConverterFn selectConverter(SampleType t)
{
    switch (t)
    {
        case SampleType::U8:  return convU8;
        case SampleType::S8:  return convS8;
        case SampleType::S16: return convS16;
        case SampleType::S24: return convS24;
        case SampleType::S32: return convS32;
        case SampleType::F32: return convF32;
        case SampleType::F64: return convF64;
        default: return nullptr;
    }
}

static uint32_t flacBitsPerSample(SampleType t)
{
    switch (t)
    {
        case SampleType::U8:  return 16;
        case SampleType::S8:  return 16;
        case SampleType::S16: return 16;
        case SampleType::S24: return 24;
        case SampleType::S32: return 24;
        case SampleType::F32: return 24;
        case SampleType::F64: return 24;
        default: return 0;
    }
}

// ------------------------------------------------------------
// Convert Sound → libFLAC PCM buffer
// ------------------------------------------------------------
bool convertToFLACPCM(const Sound& snd, TAlignedVector<FLAC__int32>& output_samples)
{
    const int64_t totalSamples = snd.sampleCount();

    output_samples.resize(totalSamples);

    const uint8_t* __restrict__ pSrc = snd.m_samples.data();

    FLAC__int32* __restrict__ pDst = output_samples.data();

    ConverterFn converter = selectConverter(snd.m_sampleType);

    const int32_t bpp = snd.bytesPerSample();

    for (int64_t i = 0; i < totalSamples; ++i)
    {
        pDst[i] = converter(pSrc);
        pSrc += bpp;
    }

    return true;
}

} // end namespace.

bool
save_sound_flac(
    const Sound& snd,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (snd.empty())
    {
        DE_WARN("Got empty sound. ",uri)
        return false;
    }

    if (options.sampleType != SampleType::Unknown)
    {
        DE_WARN("Wish to convert ignored (yet). ",uri)
    }

    TAlignedVector<FLAC__int32> pcm;
    convertToFLACPCM(snd, pcm);

    FLAC__StreamEncoder* enc = FLAC__stream_encoder_new();
    if (!enc)
    {
        DE_ERROR("Failed to open FLAC encoder, ",uri)
        return false;
    }

    FLAC__stream_encoder_set_channels(enc, snd.m_channels);
    FLAC__stream_encoder_set_sample_rate(enc, snd.m_sampleRate);
    FLAC__stream_encoder_set_bits_per_sample(enc, flacBitsPerSample(snd.m_sampleType));

    if (FLAC__stream_encoder_init_file(enc, uri.c_str(), nullptr, nullptr) != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
    {
        std::fprintf(stderr, "Failed to init FLAC encoder\n");
        FLAC__stream_encoder_delete(enc);
        return 1;
    }

    if (!FLAC__stream_encoder_process_interleaved(enc, pcm.data(), snd.m_frames))
    {
        DE_ERROR("FLAC encoding failed.", uri)
        FLAC__stream_encoder_finish(enc);
        FLAC__stream_encoder_delete(enc);
        return false;
    }

    FLAC__stream_encoder_finish(enc);
    FLAC__stream_encoder_delete(enc);
    return true;
}

} // end namespace sound.
} // end namespace de.


/*

int main()
{
    Sound snd;
    snd.sampleRate = 48000;
    snd.channels   = 2;
    snd.sampleType = SampleType::S24;

    const uint32_t frames = snd.sampleRate;
    snd.data.resize(frames * snd.channels * snd.bytesPerSample());

    for (uint32_t i = 0; i < frames; ++i) {
        float t = float(i) / snd.sampleRate;
        float s = 0.2f * std::sin(2.0f * 3.14159265f * 440.0f * t);
        int32_t si = (int32_t)(s * 0x7FFFFF);

        uint8_t* L = &snd.data[(i * snd.channels + 0) * 3];
        uint8_t* R = &snd.data[(i * snd.channels + 1) * 3];

        L[0] = (si >> 0) & 0xFF;
        L[1] = (si >> 8) & 0xFF;
        L[2] = (si >> 16) & 0xFF;

        R[0] = L[0];
        R[1] = L[1];
        R[2] = L[2];
    }

    std::vector<FLAC__int32> pcm = convertToFLACPCM(snd);
    uint32_t framesOut = pcm.size() / snd.channels;

    return 0;
}
*/
