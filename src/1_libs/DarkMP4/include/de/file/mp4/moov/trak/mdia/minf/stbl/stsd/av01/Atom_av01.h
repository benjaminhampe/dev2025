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

struct Atom_av01
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

Below is the complete, correct, demuxer‑grade av01 (AV1) sample entry parser and all the structural details you need to decode AV1 from MP4/ISOBMFF.

This is the authoritative layout used in real demuxers (FFmpeg, libavif, Bento4, ISO/IEC 14496‑15 + AV1 ISOBMFF spec).
🧩 What av01 is

av01 is the AV1 video sample entry inside the stsd box.

It contains:

    VisualSampleEntry (width, height, etc.)

    AV1CodecConfigurationBox (av1C)  
    → This is the critical part: it contains the AV1 sequence header (OBU), profile, level, bit depth, chroma, and decoder configuration.

Actual AV1 frames are stored as MP4 samples (OBU streams).
📦 Binary layout of av01 sample entry

After the 8‑byte header (size + type):
Code

av01 {
    uint8  reserved[6]
    uint16 data_reference_index

    // VisualSampleEntry
    uint16 pre_defined
    uint16 reserved2
    uint32 pre_defined3[3]
    uint16 width
    uint16 height
    uint32 horizresolution   // 16.16
    uint32 vertresolution    // 16.16
    uint32 reserved4
    uint16 frame_count
    uint8  compressorname[32]
    uint16 depth
    int16  pre_defined4

    // Child boxes:
    av1C { ... }   <-- REQUIRED
    (optional) colr
    (optional) pasp
    (optional) clap
    ...
}

🧩 av1C (AV1CodecConfigurationBox) structure

This is defined in AV1 ISOBMFF spec.
It contains:

    marker (must be 1)

    version

    seq_profile

    seq_level_idx_0

    seq_tier_0

    high_bitdepth

    twelve_bit

    monochrome

    chroma_subsampling_x

    chroma_subsampling_y

    chroma_sample_position

    initial_presentation_delay_present

    initial_presentation_delay

    configOBUs (raw AV1 sequence header OBUs)

This is the decoder initialization data.
🧩 C++ structs
cpp

struct AV1CodecConfigurationRecord {
    uint8_t marker;
    uint8_t version;

    uint8_t seq_profile;
    uint8_t seq_level_idx_0;
    uint8_t seq_tier_0;

    bool high_bitdepth;
    bool twelve_bit;
    bool monochrome;
    uint8_t chroma_subsampling_x;
    uint8_t chroma_subsampling_y;
    uint8_t chroma_sample_position;

    bool initial_presentation_delay_present;
    uint8_t initial_presentation_delay;

    std::vector<uint8_t> configOBUs; // raw AV1 sequence header OBUs
};

struct AV01SampleEntry {
    uint16_t data_reference_index;

    uint16_t width;
    uint16_t height;
    uint32_t horizresolution;
    uint32_t vertresolution;
    uint16_t frame_count;
    std::string compressorname;
    uint16_t depth;

    AV1CodecConfigurationRecord av1C;
};

🛠️ Full av01 sample entry parser
cpp

bool parse_av01_sample_entry(FileReader& f, uint64_t entry_end, AV01SampleEntry& out)
{
    // Skip 6 reserved bytes
    f.skip(6);
    out.data_reference_index = f.read_u16();

    // VisualSampleEntry
    f.skip(2);   // pre_defined
    f.skip(2);   // reserved2
    f.skip(12);  // pre_defined3[3]

    out.width  = f.read_u16();
    out.height = f.read_u16();

    out.horizresolution = f.read_u32();
    out.vertresolution  = f.read_u32();

    f.skip(4); // reserved4

    out.frame_count = f.read_u16();

    // compressorname (Pascal string padded to 32 bytes)
    uint8_t name_len = f.read_u8();
    char namebuf[32];
    f.read_bytes(namebuf, 31);
    namebuf[31] = 0;
    out.compressorname.assign(namebuf, name_len);

    out.depth = f.read_u16();
    f.skip(2); // pre_defined4

    // Child boxes
    while (f.tell() < entry_end) {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();
        uint64_t box_end = box_start + size;

        if (type == FOURCC("av1C")) {
            parse_av1C(f, box_end, out.av1C);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

🛠️ Full av1C parser
cpp

bool parse_av1C(FileReader& f, uint64_t box_end, AV1CodecConfigurationRecord& out)
{
    uint8_t b = f.read_u8();
    out.marker  = (b >> 7) & 0x01;
    out.version =  b & 0x7F;

    uint8_t b2 = f.read_u8();
    out.seq_profile     = (b2 >> 5) & 0x07;
    out.seq_level_idx_0 =  b2 & 0x1F;

    uint8_t b3 = f.read_u8();
    out.seq_tier_0 = (b3 >> 7) & 0x01;

    out.high_bitdepth          = (b3 >> 6) & 0x01;
    out.twelve_bit             = (b3 >> 5) & 0x01;
    out.monochrome             = (b3 >> 4) & 0x01;
    out.chroma_subsampling_x   = (b3 >> 3) & 0x01;
    out.chroma_subsampling_y   = (b3 >> 2) & 0x01;
    out.chroma_sample_position =  b3 & 0x03;

    uint8_t b4 = f.read_u8();
    out.initial_presentation_delay_present = (b4 >> 4) & 0x01;
    out.initial_presentation_delay         =  b4 & 0x0F;

    // Remaining bytes = configOBUs
    size_t remaining = box_end - f.tell();
    out.configOBUs.resize(remaining);
    f.read_bytes(out.configOBUs.data(), remaining);

    f.seek(box_end);
    return true;
}

🔥 How to use av01 + av1C to decode AV1
1. AV1 decoder initialization

Most AV1 decoders (dav1d, libaom, libgav1) require the sequence header OBU.

This is exactly what av1C.configOBUs contains.

You pass it to the decoder before feeding frames.
2. MP4 sample format for AV1

MP4 stores AV1 samples as:

    OBU streams (raw AV1 OBUs)

    No start codes

    No length prefixes (unlike H.264/H.265)

    Each sample may contain multiple OBUs

You feed the sample bytes as-is to the decoder.
3. Feeding dav1d (example)
cpp

dav1d_data data;
dav1d_data_init(&data);

dav1d_data_wrap(&data, sample_bytes, sample_size, nullptr, nullptr);

dav1d_picture pic;
int res = dav1d_send_data(ctx, &data);
if (res == 0) {
    while (dav1d_get_picture(ctx, &pic) == 0) {
        // pic.data[0], pic.stride[0], pic.p.w, pic.p.h
        // YUV planes
    }
}

Before decoding frames, you must feed the sequence header:
cpp

dav1d_data seq;
dav1d_data_init(&seq);
dav1d_data_wrap(&seq, av1C.configOBUs.data(), av1C.configOBUs.size(), nullptr, nullptr);
dav1d_send_data(ctx, &seq);

🧠 Summary of what av01 gives you

    Width / Height

    Pixel aspect ratio (via pasp)

    Color info (via colr)

    AV1 sequence header OBUs (from av1C)

    Profile, level, bit depth, chroma

    Everything needed to initialize an AV1 decoder

Actual AV1 frames come from MP4 samples.

If you want, I can now give you:

    Complete MP4 → AV1 decode pipeline (dav1d or libaom)

    AV1 OBU inspector (dump sequence header fields)

    AV1 sample iterator with OBU splitting
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
