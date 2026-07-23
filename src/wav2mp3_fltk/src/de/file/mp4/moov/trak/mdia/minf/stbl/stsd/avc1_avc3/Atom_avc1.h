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

struct Atom_avc1
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
🎯 What avc1 actually is

avc1 is an AVC/H.264 video sample entry inside the stsd box.

It contains:

    Base VisualSampleEntry (width, height, compressor name, etc.)

    AVCConfigurationBox (avcC)  
    This is the critical part: it contains SPS/PPS NALUs and decoder configuration.

The avcC box is what initializes your H.264 decoder.
📦 Binary layout of avc1 sample entry
Code

avc1 {
    // SampleEntry (8 bytes already consumed)
    uint8  reserved[6]
    uint16 data_reference_index

    // VisualSampleEntry fields
    uint16 pre_defined
    uint16 reserved
    uint32 pre_defined[3]
    uint16 width
    uint16 height
    uint32 horizresolution  // 16.16 fixed
    uint32 vertresolution   // 16.16 fixed
    uint32 reserved2
    uint16 frame_count
    uint8  compressorname[32]
    uint16 depth
    int16  pre_defined2

    // Child boxes:
    avcC { ... }
    (optional) btrt
    (optional) colr
    (optional) pasp
    ...
}

🧩 C++ structs
cpp

struct AVC1SampleEntry {
    uint16_t data_reference_index;

    uint16_t width;
    uint16_t height;
    uint32_t horizresolution;
    uint32_t vertresolution;
    uint16_t frame_count;
    std::string compressorname;
    uint16_t depth;

    // Parsed avcC box
    AVCDecoderConfigurationRecord avcC;
};

And the avcC structure:
cpp

struct AVCDecoderConfigurationRecord {
    uint8_t configurationVersion;
    uint8_t AVCProfileIndication;
    uint8_t profile_compatibility;
    uint8_t AVCLevelIndication;

    uint8_t lengthSizeMinusOne; // NAL length field size = (value & 3) + 1

    std::vector<std::vector<uint8_t>> sps_list;
    std::vector<std::vector<uint8_t>> pps_list;
};

🛠️ Full avc1 parser
cpp

bool parse_avc1_sample_entry(FileReader& f, uint64_t entry_end, AVC1SampleEntry& out)
{
    // Skip 6 reserved bytes
    f.skip(6);
    out.data_reference_index = f.read_u16();

    // VisualSampleEntry
    f.skip(2); // pre_defined
    f.skip(2); // reserved
    f.skip(12); // pre_defined[3]

    out.width  = f.read_u16();
    out.height = f.read_u16();

    out.horizresolution = f.read_u32();
    out.vertresolution  = f.read_u32();

    f.skip(4); // reserved2

    out.frame_count = f.read_u16();

    // compressorname (Pascal string padded to 32 bytes)
    uint8_t name_len = f.read_u8();
    char namebuf[32];
    f.read_bytes(namebuf, 31);
    namebuf[31] = 0;
    out.compressorname.assign(namebuf, name_len);

    out.depth = f.read_u16();
    f.skip(2); // pre_defined2

    // Now parse child boxes (avcC, btrt, colr, pasp, ...)
    while (f.tell() < entry_end) {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();
        uint64_t box_end = box_start + size;

        if (type == FOURCC("avcC")) {
            parse_avcC(f, box_end, out.avcC);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

🧩 Full avcC parser (critical)
cpp

bool parse_avcC(FileReader& f, uint64_t box_end, AVCDecoderConfigurationRecord& out)
{
    out.configurationVersion  = f.read_u8();
    out.AVCProfileIndication  = f.read_u8();
    out.profile_compatibility = f.read_u8();
    out.AVCLevelIndication    = f.read_u8();

    uint8_t tmp = f.read_u8();
    out.lengthSizeMinusOne = tmp & 0x03;

    uint8_t num_sps = f.read_u8() & 0x1F;

    out.sps_list.clear();
    out.pps_list.clear();

    // SPS
    for (uint8_t i = 0; i < num_sps; ++i) {
        uint16_t len = f.read_u16();
        std::vector<uint8_t> sps(len);
        f.read_bytes(sps.data(), len);
        out.sps_list.push_back(std::move(sps));
    }

    // PPS
    uint8_t num_pps = f.read_u8();
    for (uint8_t i = 0; i < num_pps; ++i) {
        uint16_t len = f.read_u16();
        std::vector<uint8_t> pps(len);
        f.read_bytes(pps.data(), len);
        out.pps_list.push_back(std::move(pps));
    }

    f.seek(box_end);
    return true;
}

🔥 How to use avc1 + avcC to feed an H.264 decoder

Your decoder (FFmpeg, libavcodec, OpenH264, etc.) needs:

    SPS (Sequence Parameter Set)

    PPS (Picture Parameter Set)

    NAL length size (1, 2, or 4 bytes)

From avcC:
cpp

int nal_length_size = (avcC.lengthSizeMinusOne & 3) + 1;

Each MP4 sample contains length‑prefixed NAL units:
Code

[4-byte length][NALU][4-byte length][NALU]...

You must convert them to Annex B if your decoder expects start codes:
Code

00 00 00 01 [NALU]
00 00 00 01 [NALU]

Plus prepend SPS/PPS before the first frame.
🧠 Summary of what avc1 gives you

    Width / Height

    Pixel aspect ratio (via optional pasp)

    Color info (via optional colr)

    Decoder configuration (via avcC)

    SPS/PPS NALUs

    NAL length size

    Everything needed to initialize an H.264 decoder

This is the core of H.264 playback in MP4.

If you want, I can now give you:

    Full HEVC (hvc1 / hev1) parser

    Annex‑B converter for H.264 samples

    Complete MP4 → H.264 decode pipeline

Just tell me what you want next, Benjamin.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
