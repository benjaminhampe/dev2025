#pragma once
#include "AudioFile.h"

inline void DownMixToMono(AudioFile & file)
{
    const size_t nChannels = file.channels;
    if (nChannels < 1)
    {
        return;
    }

    const size_t nFrames = file.frame_count;
    if (nFrames < 1)
    {
        return;
    }

    std::vector< uint8_t > tmp( nFrames * sizeof(float) ); // Mono!

    auto src = reinterpret_cast<const float*>(file.samples.data());

    auto dst = reinterpret_cast<float*>(tmp.data());

    size_t i = 0;
    size_t j = 0;

    const float inv = 1.0f / nChannels;

    for (size_t f = 0; f < nFrames; f++)
    {
        float sum = 0.0f;
        for (size_t c = 0; c < nChannels; c++)
        {
            sum += src[i];
            i++;
        }
        dst[j] = sum * inv;
        j++;
    }

    file.samples = std::move( tmp );

    file.channels = 1;
}
