#include <soxr.h>

class SoxrResampler : public ResamplerBase {
public:
    SoxrResampler() {
        recreate();
    }

    ~SoxrResampler() override {
        if (soxr) soxr_delete(soxr);
    }

    void setRates(uint32_t inR, uint32_t outR) override {
        inRate = inR; outRate = outR;
        recreate();
    }

    void setChannels(uint32_t ch) override {
        channels = ch;
        recreate();
    }

    void setQuality(int q) override {
        quality = q;
        recreate();
    }

    void dsp_read(double pts, uint32_t frames, uint32_t sampleRate,
                  float* __restrict__ L,
                  float* __restrict__ R) override
    {
        // Interleave
        inBuffer.resize(frames * channels);
        for (uint32_t i = 0; i < frames; i++) {
            inBuffer[i * 2 + 0] = L[i];
            inBuffer[i * 2 + 1] = R[i];
        }

        uint32_t outFrames = (uint64_t)frames * outRate / inRate + 8;
        outBuffer.resize(outFrames * channels);

        size_t odone = 0;
        soxr_process(soxr,
            inBuffer.data(), frames, nullptr,
            outBuffer.data(), outFrames, &odone);

        // De-interleave
        for (uint32_t i = 0; i < odone; i++) {
            L[i] = outBuffer[i * 2 + 0];
            R[i] = outBuffer[i * 2 + 1];
        }
    }

private:
    void recreate() {
        if (soxr) soxr_delete(soxr);

        soxr_quality_spec_t qspec = soxr_quality_spec(quality, 0);
        soxr_io_spec_t iospec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
        soxr_runtime_spec_t rspec = soxr_runtime_spec(0);

        soxr = soxr_create(
            inRate, outRate, channels,
            nullptr, &iospec, &qspec, &rspec);
    }

    soxr_t soxr = nullptr;
    int quality = SOXR_MQ;
};
