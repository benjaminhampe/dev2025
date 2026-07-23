#pragma once

/*
Here’s the straight‑through MP4 → AV1 (libaom) decode pipeline wired to your existing demuxer.
1. What you already have

From your MP4 side:

    Track tables: stsc, stco/co64, stsz/stz2, stts, ctts

    Sample iterator:

cpp

struct Sample {
    uint64_t file_offset;
    uint32_t size;
    uint64_t dts;
    uint64_t cts;
    uint32_t sample_description_index;
};

    av01 sample entry with parsed av1C:

cpp

struct AV1CodecConfigurationRecord {
    uint8_t marker;
    uint8_t version;

    uint8_t seq_profile;
    uint8_t seq_level_idx_0;
    uint8_t seq_tier_0;

    bool    high_bitdepth;
    bool    twelve_bit;
    bool    monochrome;
    uint8_t chroma_subsampling_x;
    uint8_t chroma_subsampling_y;
    uint8_t chroma_sample_position;

    bool    initial_presentation_delay_present;
    uint8_t initial_presentation_delay;

    std::vector<uint8_t> configOBUs; // sequence header OBUs
};

2. Initialize libaom AV1 decoder
cpp

#include <aom/aom_decoder.h>
#include <aom/aomdx.h>

aom_codec_ctx_t codec;
aom_codec_dec_cfg_t cfg;
memset(&cfg, 0, sizeof(cfg));

cfg.w       = av01.width;   // from VisualSampleEntry
cfg.h       = av01.height;
cfg.threads = 0;            // 0 = libaom decides

if (aom_codec_dec_init(&codec, aom_codec_av1_dx(), &cfg, 0) != AOM_CODEC_OK) {
    // handle error
}

3. Feed AV1 sequence header (from av1C)

av1C.configOBUs is the AV1 sequence header OBUs.
Feed it once before any frames:
cpp

if (!av1C.configOBUs.empty()) {
    if (aom_codec_decode(&codec,
                         av1C.configOBUs.data(),
                         (unsigned int)av1C.configOBUs.size(),
                         nullptr) != AOM_CODEC_OK) {
        // handle error
    }
}

4. Decode loop: MP4 samples → libaom

Each MP4 sample is a raw AV1 OBU stream.
You pass it directly to aom_codec_decode.
cpp

Sample s;
aom_codec_iter_t iter = nullptr;
const aom_image_t* img = nullptr;

while (iterator.next(s)) {
    // Read sample bytes
    std::vector<uint8_t> buf(s.size);
    file.seek(s.file_offset);
    file.read_bytes(buf.data(), s.size);

    // Decode this sample
    if (aom_codec_decode(&codec,
                         buf.data(),
                         (unsigned int)buf.size(),
                         nullptr) != AOM_CODEC_OK) {
        // decode error, skip or log
        continue;
    }

    // Pull all available frames
    iter = nullptr;
    while ((img = aom_codec_get_frame(&codec, &iter)) != nullptr) {
        // img->d_w, img->d_h
        // img->fmt (AOM_IMG_FMT_I420, etc.)
        // img->planes[0..2], img->stride[0..2] = YUV planes

        // You map this frame to s.cts/s.dts externally:
        //   timestamp = s.cts / track_timescale
        // and push to your renderer or pipeline.
    }
}

5. Flush at end of stream
cpp

aom_codec_decode(&codec, nullptr, 0, nullptr); // flush

iter = nullptr;
while ((img = aom_codec_get_frame(&codec, &iter)) != nullptr) {
    // handle remaining frames
}

aom_codec_destroy(&codec);

6. Timing

    s.dts / s.cts are in track timescale (mdhd.timescale).

    libaom doesn’t track timestamps—you do:

cpp

double pts_seconds = double(s.cts) / double(track_timescale);

You associate pts_seconds with the frames produced after decoding that sample.

That’s the full MP4 → AV1 (libaom) pipeline:

    Parse av01 + av1C

    Init libaom

    Feed configOBUs once

    For each sample: read bytes → aom_codec_decode

    Pull aom_image_t frames and timestamp them from your iterator.
*/