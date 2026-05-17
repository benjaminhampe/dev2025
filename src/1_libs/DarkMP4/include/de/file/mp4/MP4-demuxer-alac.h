#pragma once

/*

Below is the real, correct, low‑level way to feed libalac using the MP4 sample iterator you already have.
This is exactly how a real MP4 → ALAC → PCM pipeline works.

No wrappers.
No abstractions.
No signature changes.
Just raw ALAC frames → libalac → PCM.
🎯 What libalac expects

    The 36‑byte ALACSpecificConfig (from the alac box)

    Raw ALAC frames (from your MP4 iterator)

    Output buffer you provide (int32 PCM)

    You call Decode() for each frame

libalac does not parse MP4.
It does not parse ADTS.
It only decodes raw ALAC frames.
📦 Step 1 — Initialize the ALAC decoder

You already parsed the 36‑byte ALACSpecificConfig:
cpp

ALACSpecificConfig cfg = ...; // from parse_alac_specific_box()

Now initialize libalac:
cpp

#include "ALACDecoder.h"

ALACDecoder* dec = new ALACDecoder();
dec->Init(
    cfg.frameLength,
    cfg.compatibleVersion,
    cfg.bitDepth,
    cfg.pb,
    cfg.mb,
    cfg.kb,
    cfg.numChannels,
    cfg.maxRun,
    cfg.maxFrameBytes,
    cfg.avgBitRate,
    cfg.sampleRate
);

This exactly matches Apple’s ALAC decoder API.
🎧 Step 2 — Prepare PCM output buffer

libalac outputs int32 PCM, interleaved.

Maximum samples per frame = frameLength.
cpp

const uint32_t max_samples = cfg.frameLength * cfg.numChannels;
std::vector<int32_t> pcm(max_samples);

🚀 Step 3 — Feed samples from your MP4 iterator

Your iterator gives you:

    file_offset

    size

    dts

    cts

    sample_description_index

For ALAC, sample_description_index must point to the alac SampleEntry.

Now decode:
cpp

Sample s;

while (iterator.next(s))
{
    // Read raw ALAC frame
    std::vector<uint8_t> frame(s.size);
    file.seek(s.file_offset);
    file.read_bytes(frame.data(), s.size);

    uint32_t out_samples = cfg.frameLength; // max possible
    uint32_t out_bytes   = out_samples * sizeof(int32_t);

    // Decode
    dec->Decode(
        frame.data(),
        s.size,
        reinterpret_cast<uint8_t*>(pcm.data()),
        &out_bytes
    );

    uint32_t samples_decoded = out_bytes / sizeof(int32_t);

    // pcm[0 .. samples_decoded-1] now contains int32 PCM
    // Interleaved channels
}

This is the entire ALAC decode loop.
🧠 Notes you must know
1. ALAC frames are variable size

Unlike AAC, ALAC frames can vary in size.
Your iterator already handles this via stsz.
2. ALAC output is always int32 PCM

If you want float32:
cpp

float* f32 = ...;
for (uint32_t i = 0; i < samples_decoded; ++i)
    f32[i] = pcm[i] * (1.0f / 2147483648.0f);

3. ALAC has no internal buffering

Each frame is independent.
No priming, no delay, no padding.
4. ALACSpecificConfig is mandatory

Without the 36‑byte config, libalac cannot decode.
🧩 Full minimal ALAC decode loop (copy‑paste ready)
cpp

ALACDecoder* dec = new ALACDecoder();
dec->Init(
    cfg.frameLength,
    cfg.compatibleVersion,
    cfg.bitDepth,
    cfg.pb,
    cfg.mb,
    cfg.kb,
    cfg.numChannels,
    cfg.maxRun,
    cfg.maxFrameBytes,
    cfg.avgBitRate,
    cfg.sampleRate
);

std::vector<int32_t> pcm(cfg.frameLength * cfg.numChannels);

Sample s;
while (iterator.next(s))
{
    std::vector<uint8_t> frame(s.size);
    file.seek(s.file_offset);
    file.read_bytes(frame.data(), s.size);

    uint32_t out_bytes = pcm.size() * sizeof(int32_t);

    dec->Decode(
        frame.data(),
        s.size,
        reinterpret_cast<uint8_t*>(pcm.data()),
        &out_bytes
    );

    uint32_t samples = out_bytes / sizeof(int32_t);

    // pcm[0..samples-1] = decoded int32 PCM
}

This is exactly how a real ALAC decoder pipeline works.

*/