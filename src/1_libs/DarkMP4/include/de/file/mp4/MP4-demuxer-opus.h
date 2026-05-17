#pragma once

/*
Here’s the clean, low‑level way to feed libopus from your MP4 chunk/sample iterator and dOps parser.

No wrappers, no abstractions—just Opus frames → libopus → PCM.
1. Get Opus config from dOps

From your parse_dops result:
cpp

OpusSpecificBox dops = ...; // from parse_dops()

int version              = dops.version;
int channels             = dops.outputChannelCount;
uint16_t pre_skip        = dops.preSkip;
uint32_t input_sample_rate = dops.inputSampleRate; // usually 48000
int16_t output_gain_q8_8 = dops.outputGain;
uint8_t mapping_family   = dops.channelMappingFamily;

2. Create libopus decoder

For mapping family 0 (mono/stereo, default mapping):
cpp

#include <opus/opus_multistream.h>

int streams       = 1;
int coupled       = (channels == 2) ? 1 : 0;
unsigned char mapping[255];

if (mapping_family == 0) {
    // default mapping: 0..channels-1
    for (int i = 0; i < channels; ++i)
        mapping[i] = (unsigned char)i;
} else {
    // use mapping from dOps
    streams  = dops.mapping.streamCount;
    coupled  = dops.mapping.coupledCount;
    for (int i = 0; i < channels; ++i)
        mapping[i] = dops.mapping.channelMapping[i];
}

int error = 0;
OpusMSDecoder* dec = opus_multistream_decoder_create(
    48000,              // Opus always decodes at 48kHz
    channels,
    streams,
    coupled,
    mapping,
    &error
);
if (!dec || error != OPUS_OK) {
    // handle error
}

Apply output gain (Q8.8 → dB):
cpp

if (output_gain_q8_8 != 0) {
    float gain_db = output_gain_q8_8 / 256.0f;
    opus_multistream_decoder_ctl(dec, OPUS_SET_GAIN((int)(gain_db * 256.0f)));
}

3. Prepare PCM buffer

Max Opus frame is 120 ms → 5760 samples at 48 kHz.
cpp

const int max_frame_samples = 5760;
std::vector<float> pcm(max_frame_samples * channels);

4. Feed samples from your MP4 iterator

Your iterator gives you:

    file_offset

    size

    dts

    cts

    sample_description_index

For Opus, sample_description_index must point to the Opus SampleEntry.

Decode loop:
cpp

Sample s;

while (iterator.next(s))
{
    // Read raw Opus packet
    std::vector<unsigned char> packet(s.size);
    file.seek(s.file_offset);
    file.read_bytes(packet.data(), s.size);

    int frame_size = opus_multistream_decode_float(
        dec,
        packet.data(),
        (opus_int32)packet.size(),
        pcm.data(),
        max_frame_samples,
        0 // decode FEC = 0
    );

    if (frame_size < 0) {
        // decode error, skip packet
        continue;
    }

    int samples_per_channel = frame_size;

    // pcm[0 .. samples_per_channel*channels-1] = float32 interleaved at 48kHz
    // use s.dts/cts (in track timescale) for timing
}

5. Handle pre-skip

Opus streams have an initial preSkip (in samples at 48 kHz) that must be discarded:
cpp

int remaining_preskip = pre_skip;

while (iterator.next(s))
{
    // read packet, decode to pcm[], get frame_size as above

    if (frame_size <= 0)
        continue;

    int samples = frame_size;

    float* out = pcm.data();
    int start = 0;

    if (remaining_preskip > 0) {
        int skip = std::min(remaining_preskip, samples);
        start += skip * channels;
        samples -= skip;
        remaining_preskip -= skip;
        if (samples <= 0)
            continue;
    }

    // use out[start .. start + samples*channels - 1]
}

You now have a full MP4 → Opus → libopus → float32 PCM pipeline wired directly to your chunk/sample iterator.
*/