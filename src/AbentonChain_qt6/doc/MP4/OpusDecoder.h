#include <opus/opus.h>

/*
auto* opus = AP4_DYNAMIC_CAST(AP4_OpusAudioSampleEntry, sd);
int channels = opus->GetChannelCount();

OpusDecoderWrapper dec;
dec.init(channels);

for (auto& frame : frames) {
    float pcm[5760 * 8]; // max Opus frame = 120 ms = 5760 samples
    int frameCount = dec.decodeFrame(frame.data(), frame.size(), pcm, 5760);

    // frameCount = samples per channel
    // pcm = interleaved float32
}
*/

struct OpusDecoderWrapper {
    OpusDecoder* dec = nullptr;
    int sampleRate = 48000;   // Opus is ALWAYS 48 kHz internally
    int channels = 0;

    bool init(int channels) {
        this->channels = channels;

        int err = 0;
        dec = opus_decoder_create(sampleRate, channels, &err);
        return (err == OPUS_OK);
    }

    // Decode one Opus packet → float PCM
    // Returns frameCount = samples per channel
    int decodeFrame(const uint8_t* data, size_t size, float* outPcm, int maxFrames) {
        // opus_decode_float returns samples per channel
        int frameCount = opus_decode_float(dec, data, size, outPcm, maxFrames, 0);

        if (frameCount < 0)
            return 0;

        return frameCount; // samples per channel
    }

    void close() {
        if (dec) opus_decoder_destroy(dec);
        dec = nullptr;
    }
};
