// AacDecoder_Minimp4_Faad2.cpp
#include <cstdint>
#include <cstdio>
#include <vector>
#include <functional>
#include <stdexcept>
#include <string>

// ---------- FAAD2 ----------
extern "C" {
#include <neaacdec.h>
}

// ---------- minimp4 ----------
extern "C" {
#include <minimp4.h>   // from https://github.com/lieff/minimp4
}

#include <vector>
#include <stdexcept>
#include <functional>
#include <cstdio>
#include <cstdint>

extern "C" {
#include "minimp4.h"
#include "neaacdec.h"
}

using PcmCallback = std::function<void(const float*, int, int, int)>;

struct FileReader {
    FILE* f = nullptr;

    FileReader(const char* path) {
        f = fopen(path, "rb");
        if (!f) throw std::runtime_error("Cannot open file");
    }
    ~FileReader() { if (f) fclose(f); }

    static int read_cb(int64_t offset, void* buffer, size_t size, void* token) {
        FileReader* self = (FileReader*)token;
        if (fseek(self->f, (long)offset, SEEK_SET) != 0)
            return 0;
        return (int)fread(buffer, 1, size, self->f);
    }
};

class AacDecoderMinimp4Faad2 {
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

        int trackIndex = findAacTrack(mp4);
        if (trackIndex < 0)
            throw std::runtime_error("No AAC track found");

        MP4D_track_t& tr = mp4.track[trackIndex];

        // FAAD2 initialisieren
        NeAACDecHandle h = NeAACDecOpen();
        NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(h);
        cfg->outputFormat = FAAD_FMT_FLOAT;
        NeAACDecSetConfiguration(h, cfg);

        unsigned long sampleRate = 0;
        unsigned char channels = 0;

        if (NeAACDecInit2(h,
                          tr.dsi,
                          tr.dsi_bytes,
                          &sampleRate,
                          &channels) < 0)
            throw std::runtime_error("NeAACDecInit2 failed");

        std::vector<uint8_t> frameBuf;
        std::vector<float> pcm;

        for (unsigned i = 0; i < tr.sample_count; ++i) {

            unsigned frameBytes = 0;
            unsigned timestamp = 0;
            unsigned duration = 0;

            // minimp4 gibt dir Offset + Größe
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

            // Frame aus Datei lesen
            if (fseek(fr.f, (long)off, SEEK_SET) != 0)
                continue;

            if (fread(frameBuf.data(), 1, frameBytes, fr.f) != frameBytes)
                continue;

            // AAC Frame decodieren
            NeAACDecFrameInfo fi{};
            float* out = (float*)NeAACDecDecode(h, &fi,
                                                  frameBuf.data(), frameBytes);

            if (fi.error || fi.samples == 0)
                continue;

            pcm.assign(out, out + fi.samples);

            if (cb)
                cb(pcm.data(),
                   fi.samples / fi.channels,
                   fi.channels,
                   (int)sampleRate);
        }

        NeAACDecClose(h);
        MP4D_close(&mp4);
    }

private:
    static bool isAacObjectType(unsigned oti) {
        switch (oti) {
        case 0x40: // MPEG-4 AAC
        case 0x66: // MPEG-2 AAC Main
        case 0x67: // MPEG-2 AAC LC
        case 0x68: // MPEG-2 AAC SSR
            return true;
        default:
            return false;
        }
    }

    int findAacTrack(const MP4D_demux_t& mp4) {
        for (unsigned i = 0; i < mp4.track_count; ++i) {
            if (isAacObjectType(mp4.track[i].object_type_indication))
                return (int)i;
        }
        return -1;
    }
};