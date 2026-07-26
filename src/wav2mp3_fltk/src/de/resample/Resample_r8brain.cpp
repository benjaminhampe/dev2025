#include "Resample_r8brain.h"

#include <speex/speex_resampler.h>

namespace de {
namespace sound {

bool resample_r8brain_f32(Sound & sound, int32_t outRate)
{
    if (sound.empty())
    {
        DE_WARN("Nothing todo, ",sound.str())
        return false;
    }

    if (outRate < 1000)
    {
        DE_ERROR("Bad sampleRate ",outRate, ", ",sound.str())
        return false;
    }

    if (sound.m_sampleType != SampleType::F32)
    {
        DE_ERROR("Bad sampleType, must be ST_F32, ",sound.str())
        return false;
    }

    const int32_t inRate = sound.m_sampleRate;
    const double ratio = double(outRate) / double(inRate);
    const int32_t channels = sound.m_channels;
    const int64_t inFrames = sound.m_frames;
    const int64_t outFrames = std::llround(double(inFrames) * ratio) + 8;

    if (inFrames > int64_t(std::numeric_limits<uint32_t>::max()))
    {
        DE_ERROR("File too big (yet), "
                 "inFrames(",inFrames,") > max(",std::numeric_limits<uint32_t>::max(),")")
        return false;
    }

    if (outFrames > int64_t(std::numeric_limits<uint32_t>::max()))
    {
        DE_ERROR("File too big (yet), "
                 "outFrames(",outFrames,") > max(",std::numeric_limits<uint32_t>::max(),")")
        return false;
    }

    // SpeexDSP Resampler initialisieren
    int e = 0;
    SpeexResamplerState* ctx = speex_resampler_init(
        channels,
        inRate,
        outRate,
        SPEEX_RESAMPLER_QUALITY_MAX,   // beste Qualität
        &e
    );

    if (!ctx || e != RESAMPLER_ERR_SUCCESS)
    {
        DE_ERROR("Speex resampler init failed ",e)
        return false;
    }

    // Resample:
    spx_uint32_t inFramesDone  = inFrames;
    spx_uint32_t outFramesDone = outFrames;

    TAlignedVector<uint8_t> output_samples(outFrames * channels * sizeof(float));

    e = speex_resampler_process_interleaved_float( ctx,
            reinterpret_cast<const float*>(sound.m_samples.data()),
            &inFramesDone,
            reinterpret_cast<float*>(output_samples.data()),
            &outFramesDone
        );

    if (e != RESAMPLER_ERR_SUCCESS)
    {
        DE_ERROR("Speex resampler process failed ",e)
        // return false;
    }

    // Trim:
    output_samples.resize(outFramesDone * channels);

    sound.m_sampleRate = outRate;
    sound.m_samples.swap(output_samples);
    sound.m_frames = outFramesDone;

    speex_resampler_destroy(ctx);
    return true;
}

} // end namespace sound.
} // end namespace de.
