#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯MP4-FILE
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ minf (Media Information)
                    └─ stbl (Sample Table — THE HEART)
                        └─ stsd (Sample Descriptions)
                            ├─ mp4a (AAC)
                            │   ├─ data_reference_index
                            │   ├─ channelcount
                            │   ├─ samplerate
                            │   └─ esds
                            │       ├─ ES_Descriptor
                            │       ├─ DecoderConfigDescriptor
                            │       └─ DecoderSpecificInfo (ASC)
                            │           → AAC object type, sample rate index, channels
                            │
                            ├─ alac (Apple Lossless)
                            │   └─ alac atom
                            │       ├─ frameLength
                            │       ├─ compatibleVersion
                            │       ├─ bitDepth
                            │       ├─ pb / mb / kb (Rice parameters)
                            │       └─ maxFrameBytes
                            │
                            ├─ Opus
                            │   └─ dOps
                            │       ├─ version
                            │       ├─ output_channel_count
                            │       ├─ pre_skip
                            │       ├─ input_sample_rate
                            │       └─ output_gain
                            │
                            ├─ ac-3 / ec-3 (Dolby Digital)
                            │   └─ dac3 / dec3 (bitstream info)
                            │
                            ├─ avc1 / avc3 (H.264)
                            │   └─ avcC (SPS/PPS)
                            │
                            ├─ hvc1 / hev1 (H.265)
                            │   └─ hvcC (VPS/SPS/PPS)
                            │
                            ├─ dvh1 / dvhe (Dolby Vision)
                            │   └─ dvcC / dvvC
                            │
                            ├─ av01 (AV1)
                            │   └─ av1C
                            │
                            ├─ fLaC (FLAC)
                            │   └─ dfLa (FLAC metadata)
                            │
                            ├─ lpcm (uncompressed PCM)
                            │   ├─ sample_size
                            │   ├─ sample_rate
                            │   └─ channel_count
                            │
                            ├─ enca / encv (encrypted)
                            │   └─ sinf
                            │       ├─ frma (original format)
                            │       ├─ schm (scheme type)
                            │       └─ schi
                            │           └─ tenc (default_KID, isEncrypted)
                            │
                            └─ tx3g / stpp / wvtt (subtitles)
*/

struct Atom_ac_3
{
    Atom atom;


    void parse(File & file)
    {
        DE_ERROR("Not implemented.")
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str();
        return o.str();
    }
};

/*
🧩 
Here’s a tight, demuxer‑grade AC‑3 (ac-3) sample entry + dac3 box parser and the key info you actually need to drive a decoder.
1. Where AC‑3 lives in MP4

    Sample entry type: ac-3 (inside stsd)

    Child box: dac3 (AC‑3 specific config)

    Actual AC‑3 frames: MP4 samples (stsz/stsc/stco), raw syncframe bitstream

So you parse:

    ac-3 → base AudioSampleEntry

    child dac3 → AC‑3 bitstream parameters

2. AudioSampleEntry + AC‑3 sample entry structs
cpp

struct AC3SpecificBox {
    uint8_t fscod;          // sample rate code
    uint8_t bsid;           // bitstream id
    uint8_t bsmod;          // bitstream mode
    uint8_t acmod;          // audio coding mode (channels)
    uint8_t lfeon;          // LFE channel present
    uint8_t bit_rate_code;  // bitrate index
};

struct AC3SampleEntry {
    // base AudioSampleEntry
    uint16_t data_reference_index;

    uint16_t version;
    uint16_t revision_level;
    uint32_t vendor;
    uint16_t channelcount;
    uint16_t samplesize;
    uint16_t compression_id;
    uint16_t packet_size;
    uint32_t samplerate; // integer Hz

    AC3SpecificBox dac3; // parsed from 'dac3' box
};

3. ac-3 sample entry parser

Assumes you’re positioned just after the 8‑byte box header (size + type), and entry_end is the end offset of this sample entry.
cpp

bool parse_ac3_sample_entry(FileReader& f, uint64_t entry_end, AC3SampleEntry& out)
{
    // Skip 6 reserved bytes
    f.skip(6);
    out.data_reference_index = f.read_u16();

    out.version        = f.read_u16();
    out.revision_level = f.read_u16();
    out.vendor         = f.read_u32();
    out.channelcount   = f.read_u16();
    out.samplesize     = f.read_u16();
    out.compression_id = f.read_u16();
    out.packet_size    = f.read_u16();

    uint32_t sr_16_16  = f.read_u32();
    out.samplerate     = sr_16_16 >> 16; // integer Hz

    // Child boxes: expect 'dac3'
    while (f.tell() < entry_end) {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();
        uint64_t box_end = box_start + size;

        if (type == FOURCC("dac3")) {
            // AC-3 specific box
            parse_dac3(f, box_end, out.dac3);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

4. dac3 box layout and parser

dac3 is 3 bytes (plus optional padding/reserved in some specs). The core bits:

    byte 0:

        fscod (2 bits) – sample rate code

        bsid  (5 bits) – bitstream id

        reserved (1 bit)

    byte 1:

        bsmod (3 bits) – bitstream mode

        acmod (3 bits) – channel mode

        lfeon (1 bit) – LFE present

        reserved (1 bit)

    byte 2:

        bit_rate_code (5 bits) – bitrate index

        reserved (3 bits)

Parser:
cpp

bool parse_dac3(FileReader& f, uint64_t box_end, AC3SpecificBox& out)
{
    uint8_t b0 = f.read_u8();
    uint8_t b1 = f.read_u8();
    uint8_t b2 = f.read_u8();

    out.fscod = (b0 >> 6) & 0x03;
    out.bsid  = (b0 >> 1) & 0x1F;
    // 1 bit reserved in b0

    out.bsmod = (b1 >> 5) & 0x07;
    out.acmod = (b1 >> 2) & 0x07;
    out.lfeon = (b1 >> 1) & 0x01;
    // 1 bit reserved in b1

    out.bit_rate_code = (b2 >> 3) & 0x1F;
    // 3 bits reserved in b2

    f.seek(box_end);
    return true;
}

5. Interpreting dac3 for the decoder
Sample rate from fscod
cpp

static inline uint32_t ac3_sample_rate(uint8_t fscod)
{
    switch (fscod) {
    case 0: return 48000;
    case 1: return 44100;
    case 2: return 32000;
    default: return 0; // reserved
    }
}

You can cross‑check with out.samplerate from the AudioSampleEntry; they should match.
Channels from acmod + lfeon

acmod mapping (core AC‑3):

    0: 1+1 (dual mono)

    1: 1/0 (C)

    2: 2/0 (L,R)

    3: 3/0 (L,C,R)

    4: 2/1 (L,R,S)

    5: 3/1 (L,C,R,S)

    6: 2/2 (L,R,SL,SR)

    7: 3/2 (L,C,R,SL,SR)

If lfeon == 1, add an LFE channel.

You can derive a channel count and layout from this.
Bitrate from bit_rate_code

bit_rate_code indexes the standard AC‑3 bitrate table (32–640 kbps). You can map it if you care, but most decoders don’t need it.
6. How this ties into decoding

For a real pipeline:

    Use ac3_sample_rate(dac3.fscod) and acmod/lfeon to sanity‑check track parameters.

    Each MP4 sample is raw AC‑3 syncframe data; you feed it directly to liba52, FFmpeg, etc.

    No extra framing or headers needed—just pass the sample bytes as they are.

If you want, next I can give you the ec-3 / dec3 parser (E‑AC‑3 with multiple substreams) or a full MP4 → AC‑3 decode loop wired to FFmpeg or liba52.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
