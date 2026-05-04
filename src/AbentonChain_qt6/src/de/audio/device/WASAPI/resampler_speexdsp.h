#include <speex/speex_resampler.h>

class SpeexResampler : public ResamplerBase {
public:
    SpeexResampler() {
        int err;
        st = speex_resampler_init(channels, inRate, outRate, quality, &err);
    }

    ~SpeexResampler() override {
        if (st) speex_resampler_destroy(st);
    }

    void setRates(uint32_t inR, uint32_t outR) override {
        inRate = inR; outRate = outR;
        speex_resampler_set_rate(st, inRate, outRate);
    }

    void setChannels(uint32_t ch) override {
        channels = ch;
        recreate();
    }

    void setQuality(int q) override {
        quality = q;
        speex_resampler_set_quality(st, quality);
    }

    void dsp_read(double pts, uint32_t frames, uint32_t sampleRate,
                  float* __restrict__ L,
                  float* __restrict__ R) override
    {
        // Interleave input
        inBuffer.resize(frames * channels);
        for (uint32_t i = 0; i < frames; i++) {
            inBuffer[i * 2 + 0] = L[i];
            inBuffer[i * 2 + 1] = R[i];
        }

        // Resize output buffer
        uint32_t outFrames = (uint64_t)frames * outRate / inRate + 8;
        outBuffer.resize(outFrames * channels);

        uint32_t inLen = frames;
        uint32_t outLen = outFrames;

        speex_resampler_process_interleaved_float(st,
            inBuffer.data(), &inLen,
            outBuffer.data(), &outLen);

        // De-interleave back into L/R
        for (uint32_t i = 0; i < outLen; i++) {
            L[i] = outBuffer[i * 2 + 0];
            R[i] = outBuffer[i * 2 + 1];
        }
    }

private:
    void recreate() {
        if (st) speex_resampler_destroy(st);
        int err;
        st = speex_resampler_init(channels, inRate, outRate, quality, &err);
    }

    SpeexResamplerState* st = nullptr;
    int quality = 5;
};
