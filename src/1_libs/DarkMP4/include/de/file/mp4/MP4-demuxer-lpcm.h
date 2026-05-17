#pragma once

/*
Below is the clean, correct, low‑level way to feed a custom LPCM decoder using your MP4 chunk/sample iterator.
This is the simplest of all pipelines because LPCM is not compressed — you just reinterpret bytes as PCM according to the SampleEntry parameters.

No wrappers.
No abstractions.
No signature changes.
Just raw bytes → your decoder → PCM.
🎯 What LPCM in MP4 actually is

Inside MP4, uncompressed audio appears as:

    lpcm (ISO Base Media File Format)

    sowt (little‑endian PCM)

    twos (big‑endian PCM)

    in24 (24‑bit PCM)

    in32 (32‑bit PCM)

    fl32 (float32 PCM)

    fl64 (float64 PCM)

The SampleEntry tells you:

    bit depth (8/16/24/32/float)

    endianness

    channels

    sample rate

    format flags (for lpcm)

Your MP4 parser must extract these fields from the AudioSampleEntry.
📦 Step 1 — Extract LPCM parameters from SampleEntry

Example struct:
cpp

struct LPCMFormat {
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bits_per_sample;
    bool     is_float;
    bool     is_big_endian;
};

You fill this from the SampleEntry (lpcm, sowt, twos, in24, etc.).
🎧 Step 2 — Your custom LPCM decoder

Your decoder is trivial:
cpp

void decode_lpcm_frame(
    const uint8_t* src,
    uint32_t size,
    LPCMFormat fmt,
    float* out_f32,
    uint32_t& out_samples)
{
    uint32_t bytes_per_sample = fmt.bits_per_sample / 8;
    uint32_t frames = size / (bytes_per_sample * fmt.channels);

    out_samples = frames * fmt.channels;

    if (fmt.is_float) {
        // float32 or float64
        if (fmt.bits_per_sample == 32) {
            const float* f = reinterpret_cast<const float*>(src);
            for (uint32_t i = 0; i < out_samples; ++i)
                out_f32[i] = f[i];
        }
        else if (fmt.bits_per_sample == 64) {
            const double* d = reinterpret_cast<const double*>(src);
            for (uint32_t i = 0; i < out_samples; ++i)
                out_f32[i] = float(d[i]);
        }
    }
    else {
        // integer PCM → convert to float
        if (fmt.bits_per_sample == 16) {
            const int16_t* p = reinterpret_cast<const int16_t*>(src);
            for (uint32_t i = 0; i < out_samples; ++i)
                out_f32[i] = p[i] * (1.0f / 32768.0f);
        }
        else if (fmt.bits_per_sample == 24) {
            for (uint32_t i = 0; i < out_samples; ++i) {
                int32_t v = (src[i*3+0] << 8) |
                            (src[i*3+1] << 16) |
                            (src[i*3+2] << 24);
                v >>= 8;
                out_f32[i] = v * (1.0f / 8388608.0f);
            }
        }
        else if (fmt.bits_per_sample == 32) {
            const int32_t* p = reinterpret_cast<const int32_t*>(src);
            for (uint32_t i = 0; i < out_samples; ++i)
                out_f32[i] = p[i] * (1.0f / 2147483648.0f);
        }
    }
}

This is a complete LPCM decoder.
🚀 Step 3 — Feed samples from your MP4 iterator

Your iterator gives you:

    file_offset

    size

    dts

    cts

    sample_description_index

Decode loop:
cpp

Sample s;

while (iterator.next(s))
{
    // Read raw LPCM frame
    std::vector<uint8_t> frame(s.size);
    file.seek(s.file_offset);
    file.read_bytes(frame.data(), s.size);

    // Decode to float32
    uint32_t out_samples = 0;
    decode_lpcm_frame(
        frame.data(),
        s.size,
        lpcm_format,     // from SampleEntry
        pcm_buffer,      // float32*
        out_samples
    );

    // pcm_buffer[0 .. out_samples-1] now contains float32 PCM
    // Use s.dts / s.cts for timing
}

This is the entire LPCM pipeline.
🧠 Important notes
1. LPCM has no compression

Each MP4 sample is literally raw PCM bytes.
2. Sample sizes come from stsz

For LPCM, stsz.sample_size is usually constant:
Code

sample_size = channels * (bits_per_sample/8) * frames_per_sample

3. Endianness matters

    sowt = little‑endian

    twos = big‑endian

    lpcm = flags specify endianness

4. Float LPCM is easiest

fl32 and fl64 require no scaling.
*/