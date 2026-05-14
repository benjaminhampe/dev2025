#include <neaacdec.h>

/*
AacDecoder dec;
dec.init(info.decoderConfig.data(), info.decoderConfig.size());

for (auto& frame : frames) {
    float* pcm = nullptr;
    int samples = dec.decodeFrame(frame.data(), frame.size(), pcm);

    if (samples > 0) {
        // pcm = float32 samples
        // samples = number of floats
        // channels = dec.channels
    }
}

dec.close();
*/

struct AacDecoder {
    NeAACDecHandle h = nullptr;
    uint32_t sampleRate = 0;
    uint8_t channels = 0;

    bool init(const uint8_t* asc, size_t ascSize) {
        h = NeAACDecOpen();
        if (!h) return false;

        // Request float output
        NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(h);
        cfg->outputFormat = FAAD_FMT_FLOAT; // float32
        NeAACDecSetConfiguration(h, cfg);

        long r = NeAACDecInit2(h, asc, ascSize, &sampleRate, &channels);
        return (r >= 0);
    }

    // Decode one AAC frame → float PCM
    // Returns number of float samples
    int decodeFrame(const uint8_t* data, size_t size, float*& outPcm) {
        NeAACDecFrameInfo info;
        void* pcm = NeAACDecDecode2(h, &info, data, size);

        if (info.error != 0 || !pcm)
            return 0;

        outPcm = reinterpret_cast<float*>(pcm);
        return info.samples; // number of float samples
    }

    void close() {
        if (h) NeAACDecClose(h);
        h = nullptr;
    }
};
