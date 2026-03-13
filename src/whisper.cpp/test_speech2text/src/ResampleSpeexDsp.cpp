#include "ResampleSpeexDsp.h"

#include <speex/speex_resampler.h>

#include <iostream>

void resample_speex_f32(AudioFile & file, uint32_t out_rate)
{
    const uint32_t in_rate = file.sample_rate;

    if (in_rate == out_rate)
    {
        return;
    }

    int err = 0;
    SpeexResamplerState * st = speex_resampler_init(
        1,                // channels (mono)
        in_rate,
        out_rate,
        SPEEX_RESAMPLER_QUALITY_VOIP,  // gute Qualität, schnell
        &err
        );

    if (!st || err != RESAMPLER_ERR_SUCCESS)
    {
        std::cerr << "SpeexDSP init error: " << err << "\n";
        return;
    }

    // Zielgröße schätzen
    const double ratio = double(out_rate) / double(in_rate);
    spx_uint32_t src_frames  = file.frame_count;
    spx_uint32_t dst_frames = spx_uint32_t(src_frames * ratio);

    std::vector< uint8_t > tmp(dst_frames * file.channels * sizeof(float));

    err = speex_resampler_process_float(
        st, // state
        0,  // channel index                    // channel index (mono)
        reinterpret_cast<const float*>(file.samples.data()),
        &src_frames,
        reinterpret_cast<float*>(tmp.data()),
        &dst_frames
        );

    speex_resampler_destroy(st);

    if (err != RESAMPLER_ERR_SUCCESS)
    {
        std::cerr << "SpeexDSP process error: " << err << "\n";
        return;
    }

    tmp.resize(dst_frames * file.channels * sizeof(float));

    file.samples = std::move( tmp );

    file.sample_rate = out_rate;

    file.frame_count = dst_frames;
}

#if 0

void resample_speex_to_16k(const std::vector<float> & in,
                           uint32_t in_rate,
                           std::vector<float> & out)
{
    const uint32_t target_rate = 16000;

    if (in_rate == target_rate) {
        out = in;
        return;
    }

    int err = 0;
    SpeexResamplerState * st = speex_resampler_init(
        1,                // channels (mono)
        in_rate,
        target_rate,
        SPEEX_RESAMPLER_QUALITY_VOIP,  // gute Qualität, schnell
        &err
    );

    if (!st || err != RESAMPLER_ERR_SUCCESS) {
        std::cerr << "SpeexDSP init error: " << err << "\n";
        out = in;
        return;
    }

    // Zielgröße schätzen
    size_t out_len = size_t(double(in.size()) * double(target_rate) / double(in_rate)) + 8;
    out.resize(out_len);

    spx_uint32_t in_len  = in.size();
    spx_uint32_t out_len32 = out_len;

    err = speex_resampler_process_float(
        st,
        0,                      // channel index (mono)
        in.data(), &in_len,
        out.data(), &out_len32
    );

    if (err != RESAMPLER_ERR_SUCCESS) {
        std::cerr << "SpeexDSP process error: " << err << "\n";
    }

    out.resize(out_len32);

    speex_resampler_destroy(st);
}

#endif
