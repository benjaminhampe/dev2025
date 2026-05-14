#include <vector>
#include <stdexcept>
#include <functional>
#include <cstdio>
#include <cstdint>

extern "C" {
#include "minimp4.h"
#include "alac/ALACDecoder.h"
#include "alac/ALACBitUtilities.h"
}

using PcmCallback = std::function<void(const int32_t* samples,
                                       int numFrames,
                                       int numChannels,
                                       int sampleRate)>;

struct FileReader {
    FILE* f = nullptr;

    FileReader(const char* path) {
        f = fopen(path, "rb");
        if (!f)
            throw std::runtime_error("Cannot open file");
    }
    ~FileReader() {
        if (f) fclose(f);
    }

    static int read_cb(int64_t offset, void* buffer, size_t size, void* token) {
        FileReader* self = (FileReader*)token;
        if (fseek(self->f, (long)offset, SEEK_SET) != 0)
            return 0;
        return (int)fread(buffer, 1, size, self->f);
    }
};

class AlacDecoderMinimp4 {
public:
    void decodeFile(const char* path, const PcmCallback& cb) {
        FileReader fr(path);

        // Dateigröße
        fseek(fr.f, 0, SEEK_END);
        int64_t fileSize = ftell(fr.f);
        fseek(fr.f, 0, SEEK_SET);

        MP4D_demux_t mp4{};
        if (!MP4D_open(&mp4, FileReader::read_cb, &fr, fileSize))
            throw std::runtime_error("MP4D_open failed");

        int trackIndex = findAlacTrack(mp4);
        if (trackIndex < 0)
            throw std::runtime_error("No ALAC track found");

        MP4D_track_t& tr = mp4.track[trackIndex];

        // ALAC Decoder initialisieren
        ALACDecoder decoder;
        decoder.Init(tr.dsi, tr.dsi_bytes);

        const uint32_t channels = decoder.mConfig.numChannels;
        const uint32_t sampleRate = decoder.mConfig.sampleRate;
        const uint32_t maxSamplesPerFrame = decoder.mConfig.frameLength; // .GetMaxSamplesPerFrame();

        std::vector<uint8_t> frameBuf;
        std::vector<int32_t> pcm(maxSamplesPerFrame * channels);

        BitBuffer bits;
        BitBufferInit(&bits, nullptr, 0);

        for (unsigned i = 0; i < tr.sample_count; ++i) {

            unsigned frameBytes = 0;
            unsigned timestamp = 0;
            unsigned duration = 0;

            MP4D_file_offset_t off =
                MP4D_frame_offset(&mp4,
                                  trackIndex,
                                  i,
                                  &frameBytes,
                                  &timestamp,
                                  &duration);

            if (frameBytes == 0)
                continue;

            frameBuf.resize(frameBytes);

            if (fseek(fr.f, (long)off, SEEK_SET) != 0)
                continue;

            if (fread(frameBuf.data(), 1, frameBytes, fr.f) != frameBytes)
                continue;

            // BitBuffer vorbereiten
            BitBufferInit(&bits, frameBuf.data(), frameBytes);

            uint32_t outSamples = 0;

            int32_t status = decoder.Decode(
                &bits,
                (uint8_t*)pcm.data(),
                maxSamplesPerFrame,
                channels,
                &outSamples
                );

            if (status != ALAC_noErr || outSamples == 0)
                continue;

            if (cb)
                cb(pcm.data(), outSamples, channels, sampleRate);
        }

        MP4D_close(&mp4);
    }

private:
    static bool isAlacObjectType(unsigned oti) {
        return (oti == 0xA9); // Apple Lossless
    }

    int findAlacTrack(const MP4D_demux_t& mp4) {
        for (unsigned i = 0; i < mp4.track_count; ++i) {
            if (isAlacObjectType(mp4.track[i].object_type_indication))
                return (int)i;
        }
        return -1;
    }
};
