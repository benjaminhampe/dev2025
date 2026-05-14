#include "ALACDecoder.h"

/*

AlacDecoder dec;
dec.init(info.decoderConfig.data(), info.decoderConfig.size());

for (auto& frame : frames) {
    float* pcm = nullptr;
    int frameCount = dec.decodeFrame(frame.data(), frame.size(), pcm);

    // frameCount = samples per channel
    // pcm = interleaved float32
}

*/
struct AlacDecoder {
    ALACDecoder* dec = nullptr;
    uint32_t sampleRate = 0;
    uint32_t channels = 0;
    uint32_t bitDepth = 0;

    bool init(const uint8_t* cookie, size_t cookieSize) {
        dec = new ALACDecoder();
        if (!dec) return false;

        // Parse ALAC magic cookie
        BitBuffer bits;
        BitBufferInit(&bits, (uint8_t*)cookie, cookieSize);

        dec->Init(&bits);

        sampleRate = dec->mConfig.sampleRate;
        channels   = dec->mConfig.numChannels;
        bitDepth   = dec->mConfig.bitDepth;

        return true;
    }

    // Decode one ALAC packet → interleaved PCM
    // Returns frameCount = samples per channel
    int decodeFrame(const uint8_t* data, size_t size, float*& outPcm) {
        // ALAC outputs 32-bit signed integers
        static int32_t pcmBuffer[4096 * 8]; // enough for 8ch
        uint32_t outFrames = 0;

        dec->Decode(data, size, pcmBuffer, &outFrames);

        // outFrames = samples per channel (your definition!)
        // Convert to float
        int totalSamples = outFrames * channels;
        float* f = new float[totalSamples];

        const float scale = 1.0f / float(1 << (bitDepth - 1));
        for (int i = 0; i < totalSamples; i++)
            f[i] = pcmBuffer[i] * scale;

        outPcm = f;
        return outFrames; // samples per channel
    }

    void close() {
        delete dec;
        dec = nullptr;
    }
};
