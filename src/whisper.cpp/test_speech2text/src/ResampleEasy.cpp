#include "ResampleEasy.h"

void resample_easy_f32(AudioFile & file, uint32_t out_rate)
{
    uint32_t in_rate = file.sample_rate;
    if (in_rate == out_rate)
    {
        return;
    }

    const double ratio = double(out_rate) / double(in_rate);
    const size_t out_frames = size_t(file.frame_count * ratio);

    // Assumes mono?
    // Assumes float?

    std::vector< uint8_t > tmp(out_frames * file.channels * sizeof(float));

    auto src = reinterpret_cast<const float*>(file.samples.data());

    auto dst = reinterpret_cast<float*>(tmp.data());

    for (size_t i = 0; i < out_frames; i++)
    {
        double src_pos = double(i) / ratio;
        size_t idx = size_t(src_pos);

        if (idx + 1 < out_frames)
        {
            float a = src[idx];
            float b = src[idx + 1];
            float t = float(src_pos - idx);
            dst[i] = a + (b - a) * t;  // lineare Interpolation
        } else {
            dst[i] = src[file.frame_count - 1];
        }
    }

    file.samples = std::move( tmp );
}
