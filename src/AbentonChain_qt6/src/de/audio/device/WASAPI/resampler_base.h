#pragma once
#include <cstdint>
#include <vector>

class ResamplerBase {
public:
    virtual ~ResamplerBase() = default;

    virtual void setRates(uint32_t inRate, uint32_t outRate) = 0;
    virtual void setChannels(uint32_t ch) = 0;
    virtual void setQuality(int q) = 0;

    // pts = presentation timestamp (optional)
    virtual void dsp_read(double pts,
                          uint32_t frames,
                          uint32_t sampleRate,
                          float* __restrict__ L,
                          float* __restrict__ R) = 0;

protected:
    uint32_t inRate = 48000;
    uint32_t outRate = 48000;
    uint32_t channels = 2;

    std::vector<float> inBuffer;
    std::vector<float> outBuffer;
};
