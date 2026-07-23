#pragma once

/*

Here’s the clean, straight‑through MP4 → AV1 → dav1d pipeline wired to your existing demuxer.
1. What you already have

From your MP4 side:
cpp

struct Sample {
    uint64_t file_offset;
    uint32_t size;
    uint64_t dts;
    uint64_t cts;
    uint32_t sample_description_index;
};

And from av01/av1C:
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

    std::vector<uint8_t> configOBUs; // AV1 sequence header OBUs
};

Each MP4 sample = raw AV1 OBU stream.
2. Initialize dav1d
cpp

#include <dav1d/dav1d.h>

Dav1dContext* ctx = nullptr;
Dav1dSettings settings;
dav1d_default_settings(&settings);

// You can tune threads, max frame delay, etc. here
settings.n_threads = 0; // auto

if (dav1d_open(&ctx, &settings) < 0) {
    // handle error
}

3. Helper: wrap a buffer into Dav1dData
cpp

static int dav1d_data_wrap_simple(Dav1dData* data,
                                  uint8_t* buf,
                                  size_t size)
{
    dav1d_data_init(data);
    // No custom free callback: dav1d will not free, you own buf lifetime
    return dav1d_data_wrap(data, buf, size, nullptr, nullptr);
}

(Or you can use dav1d_data_wrap_user_data if you want dav1d to own/free.)
4. Feed AV1 sequence header (from av1C)

Do this once before any frames:
cpp

if (!av1C.configOBUs.empty()) {
    Dav1dData seq;
    dav1d_data_init(&seq);

    // Make a copy or ensure lifetime > decode
    std::vector<uint8_t> seq_buf = av1C.configOBUs;

    dav1d_data_wrap_simple(&seq, seq_buf.data(), seq_buf.size());
    if (dav1d_send_data(ctx, &seq) < 0) {
        // handle error
    }
    // seq_buf must stay alive until consumed; easiest is keep it in scope
}

5. Decode loop: MP4 samples → dav1d
cpp

Sample s;
Dav1dPicture pic;

while (iterator.next(s)) {
    // Read sample bytes
    std::vector<uint8_t> buf(s.size);
    file.seek(s.file_offset);
    file.read_bytes(buf.data(), s.size);

    // Wrap into Dav1dData
    Dav1dData data;
    dav1d_data_init(&data);
    dav1d_data_wrap_simple(&data, buf.data(), buf.size());

    // Send to decoder
    int r = dav1d_send_data(ctx, &data);
    if (r < 0) {
        // decode error or backpressure; you can retry or skip
        continue;
    }

    // Pull all available frames
    while (dav1d_get_picture(ctx, &pic) == 0) {
        // pic.p.w, pic.p.h
        // pic.p.layout, pic.p.bpc
        // pic.data[0..2], pic.stride[0..2] = Y, U, V planes

        // Timestamping: associate this picture with s.cts / s.dts externally
        // e.g. double pts = double(s.cts) / track_timescale;

        // When done with the picture:
        dav1d_picture_unref(&pic);
    }
}

6. Flush at end of stream
cpp

// Signal end-of-stream: send empty data until dav1d_send_data returns < 0
Dav1dData eos;
dav1d_data_init(&eos);
dav1d_send_data(ctx, &eos); // often enough to flush

while (dav1d_get_picture(ctx, &pic) == 0) {
    // handle remaining frames
    dav1d_picture_unref(&pic);
}

dav1d_close(&ctx);

7. Timing

    s.cts / s.dts are in track timescale (mdhd.timescale).

    dav1d does not track timestamps—you do:

cpp

double pts_seconds = double(s.cts) / double(track_timescale);

You carry that alongside each decoded Dav1dPicture in your own pipeline.

That’s the full MP4 → AV1 (dav1d) path:

    Parse av01 + av1C

    Open dav1d

    Feed configOBUs once

    For each sample: read bytes → dav1d_send_data

    Pull Dav1dPicture frames with dav1d_get_picture, timestamp from your iterator.



*/