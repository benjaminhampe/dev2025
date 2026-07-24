#include "Resample_soxr.h"

#include <soxr.h>

namespace de {
namespace sound {

bool resample_soxr_f32(Sound & sound, int32_t outRate)
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

    if (sound.m_sampleType != Sound::ST_F32)
    {
        DE_ERROR("Bad sampleType, must be ST_F32, ",sound.str())
        return false;
    }

    // Soxr quality preset: HQ linear-phase
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);

    // Soxr IO spec: float32 interleaved
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);

    // Soxr runtime spec: default
    soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(0);

    // Create resampler
    soxr_error_t e;

    int64_t inFrames = sound.m_frames;
    int32_t inRate = sound.m_sampleRate;
    int32_t channels = sound.m_channels;

    soxr_t soxr = soxr_create(
        inRate,
        outRate,
        channels,
        &e,
        &io_spec,
        &q_spec,
        &runtime_spec
    );

    if (!soxr || e)
    {
        DE_ERROR("No soxr, error = ", e)
        return false;
    }

    // Estimate output size
    double ratio = double(outRate) / double(inRate);

    int64_t outFrames = std::llround(double(inFrames) * ratio) + 16;

    TAlignedVector<uint8_t> output_samples(outFrames * channels * sizeof(float));

    size_t outFramesDone = 0;

    // Perform resampling
    e = soxr_process( soxr,
        sound.m_samples.data(), inFrames, nullptr,
        output_samples.data(), outFrames, &outFramesDone
    );

    if (e)
    {
        DE_ERROR("soxr_process error ",e)
    }

    // Trim
    output_samples.resize(outFramesDone * channels);

    sound.m_sampleRate = outRate;
    sound.m_samples.swap(output_samples);
    sound.m_frames = outFramesDone;

    soxr_delete(soxr);
    return true;
}

} // end namespace sound.
} // end namespace de.



#if 0
void resample_soxr_to_16k(const std::vector<float> & in, uint32_t in_rate, std::vector<float> & out)
{
    const uint32_t target_rate = 16000;

    if (in_rate == target_rate) {
        out = in;
        return;
    }

    // Verhältnis bestimmen
    double ratio = double(target_rate) / double(in_rate);

    // Zielgröße schätzen
    size_t out_len_est = size_t(in.size() * ratio) + 16;
    out.resize(out_len_est);

    size_t odone = 0;

    soxr_error_t err = soxr_oneshot(
        in_rate,            // input rate
        target_rate,        // output rate
        1,                  // channels (mono)
        in.data(),          // input buffer
        in.size(),          // input samples
        nullptr,            // input samples actually used (optional)
        out.data(),         // output buffer
        out_len_est,        // output buffer size
        &odone,             // output samples actually produced
        nullptr, nullptr, nullptr // quality presets (default HQ)
    );

    if (err) {
        std::cerr << "soxr error: " << err << "\n";
        out = in;
        return;
    }

    out.resize(odone);
}

#endif
