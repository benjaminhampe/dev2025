#pragma once
#include <cstdint>
#include <functional>
#include <de/audio/IDspChainElement.h>

namespace de {
namespace audio {

// uint64_t dsp_read( double pts, float* __restrict__ dst, uint32_t frames, uint32_t channels, uint32_t sampleRate ) = 0;

typedef std::function<uint64_t      // Out - num processed samples.
                      (double,      // In - pts
                       float*,      // In - pointer
                       uint32_t,    // In - frames
                       uint32_t,    // In - channels
                       uint32_t)>   // In - sampleRate
    FN_DSP_processSamples;

// bool dsp_init(uint32_t frames, uint32_t channels, uint32_t sampleRate ) = 0;

typedef std::function<bool (uint32_t, uint32_t, uint32_t)>
    FN_DSP_init;


// uint64_t dsp_write( double pts, const float* __restrict__ src, uint32_t frames, uint32_t channels, uint32_t sampleRate ) = 0;

typedef std::function<uint64_t( double, const float*, uint32_t, uint32_t, uint32_t )>
    FN_DSP_write;

// void dsp_setInputSignal( IDspChainElement* input, int i = 0 ) = 0;

// typedef std::function<void( IDspChainElement*, uint32_t )>
//  FN_DSP_setInputSignal;

} // end namespace audio.
} // end namespace de.
