#pragma once

/*

Here you go, Benjamin — the exact, minimal, correct way to feed FAAD2 using the MP4 sample iterator you just built.  
No abstractions, no wrappers, no signature changes.
Just raw AAC frames → FAAD2 → PCM.

This is the part where your demuxer becomes a decoder.
🎯 FAAD2 expects:

    Raw AAC frames (NOT ADTS, NOT MP4 boxes)

    AudioSpecificConfig (from esds → DecoderSpecificInfo)

    One frame per decode call

    Preallocated PCM buffer (you like that)

📦 FAAD2 initialization

You must extract the AudioSpecificConfig from the esds box:
cpp

// from esds.decConfig.decSpecific
const uint8_t* asc = esds.decConfig.decSpecific.data;
uint32_t asc_size  = esds.decConfig.decSpecific.size;

Then:
cpp

faacDecHandle h = faacDecOpen();

faacDecConfigurationPtr cfg = faacDecGetCurrentConfiguration(h);
cfg->outputFormat = FAAD_FMT_FLOAT;   // you want float32
cfg->downMatrix   = 0;
faacDecSetConfiguration(h, cfg);

// Initialize decoder with ASC
if (faacDecInit2(h, asc, asc_size, &sample_rate, &channels) < 0) {
    // error
}

Now FAAD2 is ready to decode raw AAC frames.
🎧 Feeding samples from your MP4 iterator

Your iterator gives you:
Code

file_offset
size
dts
cts
sample_description_index

For AAC, sample_description_index must point to the mp4a SampleEntry.

Now decode:
cpp

Sample s;

while (iterator.next(s))
{
    // Read raw AAC frame from file
    std::vector<uint8_t> frame(s.size);
    f.seek(s.file_offset);
    f.read_bytes(frame.data(), s.size);

    // Decode
    void* pcm = nullptr;
    uint32_t pcm_samples = faacDecDecode(h, &frameInfo, frame.data(), s.size);

    if (frameInfo.error > 0) {
        // handle decode error
        continue;
    }

    // pcm_samples = number of *samples per channel*
    // output is float32 if FAAD_FMT_FLOAT

    float* pcm_f32 = (float*)frameInfo.samples;

    // Now you have PCM for this sample
    // You can copy it into your preallocated buffer
}

🧠 FAAD2 output details

frameInfo.samples points to:
Code

float pcm[channels * pcm_samples]

Interleaved:
Code

L R L R L R ...

If mono:
Code

M M M M ...

🔥 Putting it all together
1. Parse MP4

You already have:

    stsc

    stco / co64

    stsz / stz2

    stts

    ctts

    esds (AAC ASC)

2. Build iterator

You already have it.
3. Initialize FAAD2

Using ASC from esds.
4. For each sample:

    Seek to file_offset

    Read size bytes

    Call faacDecDecode

    Get PCM float32

    Store or play

🧩 Minimal full decode loop
cpp

faacDecHandle h = faacDecOpen();
faacDecConfigurationPtr cfg = faacDecGetCurrentConfiguration(h);
cfg->outputFormat = FAAD_FMT_FLOAT;
faacDecSetConfiguration(h, cfg);

uint32_t sample_rate = 0, channels = 0;
faacDecInit2(h, asc, asc_size, &sample_rate, &channels);

SampleIterator it(trackTables);
Sample s;

while (it.next(s))
{
    std::vector<uint8_t> frame(s.size);
    file.seek(s.file_offset);
    file.read_bytes(frame.data(), s.size);

    faacDecFrameInfo info;
    void* pcm_void = faacDecDecode(h, &info, frame.data(), s.size);

    if (info.error > 0)
        continue;

    float* pcm = (float*)pcm_void;
    uint32_t samples_per_channel = info.samples / channels;

    // pcm now contains float32 interleaved samples
    // do whatever you want with it
}
*/