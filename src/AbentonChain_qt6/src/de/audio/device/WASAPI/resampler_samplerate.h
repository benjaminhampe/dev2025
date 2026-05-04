#include <samplerate.h>

class LibSampleRateResampler : public ResamplerBase {
public:
    LibSampleRateResampler() {
        src = src_new(SRC_SINC_MEDIUM_QUALITY, channels, &err);
    }

    ~LibSampleRateResampler() override {
        if (src) src_delete(src);
    }

    void setRates(uint32_t inR, uint32_t outR) override {
        inRate = inR; outRate = outR;
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
        // Interleave input
        inBuffer.resize(frames * channels);
        for (uint32_t i = 0; i < frames; i++) {
            inBuffer[i * 2 + 0] = L[i];
            inBuffer[i * 2 + 1] = R[i];
        }

        // Prepare output buffer
        uint32_t outFrames = (uint64_t)frames * outRate / inRate + 8;
        outBuffer.resize(outFrames * channels);

        SRC_DATA data{};
        data.data_in = inBuffer.data();
        data.data_out = outBuffer.data();
        data.input_frames = frames;
        data.output_frames = outFrames;
        data.src_ratio = double(outRate) / double(inRate);
        data.end_of_input = 0;

        src_process(src, &data);

        // De-interleave
        for (uint32_t i = 0; i < data.output_frames_gen; i++) {
            L[i] = outBuffer[i * 2 + 0];
            R[i] = outBuffer[i * 2 + 1];
        }
    }

private:
    void recreate() {
        if (src) src_delete(src);
        src = src_new(quality, channels, &err);
    }

    SRC_STATE* src = nullptr;
    int quality = SRC_SINC_MEDIUM_QUALITY;
    int err = 0;
};
