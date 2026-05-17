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

struct Atom_dvh1
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
Here’s the whole Dolby Vision HEVC story in one go: what dvh1 is, why it exists, who uses it, and a full demuxer‑grade parser (including Dolby Vision config).
1. What dvh1 is

    FourCC: dvh1

    Meaning: Dolby Vision over HEVC, stored in MP4/ISOBMFF

    Structure:

        Base: VisualSampleEntry (like hvc1)

        Child: hvcC (HEVCDecoderConfigurationRecord)

        Child: dvcC (Dolby Vision configuration box)

So dvh1 = “HEVC (hvc1) + Dolby Vision metadata (dvcC)”.
2. High‑level layout

After the 8‑byte box header (size + type):
text

dvh1 {
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
    hvcC { ... }   // HEVC config (VPS/SPS/PPS, NAL length size)
    dvcC { ... }   // Dolby Vision config
    (optional) colr, pasp, clap, ...
}

3. C++ structs
cpp

struct HEVCNALArray {
    uint8_t  nal_unit_type;
    bool     array_completeness;
    std::vector<std::vector<uint8_t>> nal_units;
};

struct HEVCDecoderConfigurationRecord {
    uint8_t configurationVersion;
    uint8_t general_profile_space;
    uint8_t general_tier_flag;
    uint8_t general_profile_idc;
    uint32_t general_profile_compatibility_flags;
    uint64_t general_constraint_indicator_flags;
    uint8_t general_level_idc;
    uint8_t lengthSizeMinusOne;
    std::vector<HEVCNALArray> arrays;
};

struct DolbyVisionConfig {
    uint8_t dv_version_major;
    uint8_t dv_version_minor;
    uint8_t dv_profile;
    uint8_t dv_level;
    bool    rpu_present_flag;
    bool    el_present_flag;
    bool    bl_present_flag;
    uint8_t dv_bl_signal_compatibility_id;
};

struct DVH1SampleEntry {
    uint16_t data_reference_index;

    uint16_t width;
    uint16_t height;
    uint32_t horizresolution;
    uint32_t vertresolution;
    uint16_t frame_count;
    std::string compressorname;
    uint16_t depth;

    HEVCDecoderConfigurationRecord hvcc;
    DolbyVisionConfig              dvcC;
};

4. dvh1 sample entry parser
cpp

bool parse_dvh1_sample_entry(FileReader& f, uint64_t entry_end, DVH1SampleEntry& out)
{
    f.skip(6);
    out.data_reference_index = f.read_u16();

    f.skip(2);   // pre_defined
    f.skip(2);   // reserved2
    f.skip(12);  // pre_defined3[3]

    out.width  = f.read_u16();
    out.height = f.read_u16();

    out.horizresolution = f.read_u32();
    out.vertresolution  = f.read_u32();

    f.skip(4); // reserved4

    out.frame_count = f.read_u16();

    uint8_t name_len = f.read_u8();
    char namebuf[32];
    f.read_bytes(namebuf, 31);
    namebuf[31] = 0;
    out.compressorname.assign(namebuf, name_len);

    out.depth = f.read_u16();
    f.skip(2); // pre_defined4

    while (f.tell() < entry_end) {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();
        uint64_t box_end = box_start + size;

        if (type == FOURCC("hvcC")) {
            parse_hvcc(f, box_end, out.hvcc);
        } else if (type == FOURCC("dvcC")) {
            parse_dvcc(f, box_end, out.dvcC);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

5. hvcC parser (HEVC config – abbreviated core)
cpp

bool parse_hvcc(FileReader& f, uint64_t box_end, HEVCDecoderConfigurationRecord& out)
{
    out.configurationVersion = f.read_u8();

    uint8_t tmp = f.read_u8();
    out.general_profile_space = (tmp >> 6) & 0x03;
    out.general_tier_flag     = (tmp >> 5) & 0x01;
    out.general_profile_idc   =  tmp       & 0x1F;

    out.general_profile_compatibility_flags = f.read_u32();

    uint64_t hi = f.read_u32();
    uint64_t lo = f.read_u32();
    out.general_constraint_indicator_flags = (hi << 32) | lo;

    out.general_level_idc = f.read_u8();

    f.read_u16(); // min_spatial_segmentation_idc + reserved
    f.read_u8();  // parallelismType + reserved
    f.read_u8();  // chromaFormat + reserved
    f.read_u8();  // bitDepthLumaMinus8 + reserved
    f.read_u8();  // bitDepthChromaMinus8 + reserved

    f.read_u16(); // avgFrameRate

    uint8_t misc = f.read_u8();
    out.lengthSizeMinusOne = misc & 0x03;

    uint8_t numOfArrays = f.read_u8();
    out.arrays.clear();
    out.arrays.reserve(numOfArrays);

    for (uint8_t i = 0; i < numOfArrays && f.tell() < box_end; ++i) {
        uint8_t hdr = f.read_u8();
        bool array_completeness = (hdr & 0x80) != 0;
        uint8_t nal_unit_type   =  hdr & 0x3F;

        uint16_t numNalus = f.read_u16();

        HEVCNALArray arr;
        arr.nal_unit_type      = nal_unit_type;
        arr.array_completeness = array_completeness;

        for (uint16_t n = 0; n < numNalus; ++n) {
            uint16_t nal_len = f.read_u16();
            std::vector<uint8_t> nal(nal_len);
            f.read_bytes(nal.data(), nal_len);
            arr.nal_units.push_back(std::move(nal));
        }

        out.arrays.push_back(std::move(arr));
    }

    f.seek(box_end);
    return true;
}

6. dvcC parser (Dolby Vision config)

Core Dolby Vision configuration (from Dolby spec):
cpp

bool parse_dvcc(FileReader& f, uint64_t box_end, DolbyVisionConfig& out)
{
    out.dv_version_major = f.read_u8();
    out.dv_version_minor = f.read_u8();
    out.dv_profile       = f.read_u8();
    out.dv_level         = f.read_u8();

    uint8_t b = f.read_u8();
    out.rpu_present_flag             = (b >> 7) & 0x01;
    out.el_present_flag              = (b >> 6) & 0x01;
    out.bl_present_flag              = (b >> 5) & 0x01;
    out.dv_bl_signal_compatibility_id=  b & 0x1F;

    // Some variants have extra reserved bytes; skip to end
    f.seek(box_end);
    return true;
}

Key meanings:

    dv_profile: e.g. 5, 7, 8.x (different Dolby Vision profiles)

    dv_level: similar to HEVC level (complexity / resolution)

    rpu_present_flag: dynamic metadata (RPU) present

    el_present_flag: enhancement layer present (dual‑layer DV)

    bl_present_flag: base layer present (usually HEVC HDR10/SDR)

    dv_bl_signal_compatibility_id: how BL is compatible (HDR10, SDR, etc.)

7. How decoders use dvh1 + dvcC

    HEVC decoder uses hvcC:

        VPS/SPS/PPS NALs

        NAL length size

    Dolby Vision logic uses dvcC:

        Profile/level → choose DV pipeline

        Flags → whether there’s:

            Only RPU over HDR10 BL (single‑layer)

            BL + EL (dual‑layer)

    Actual DV data:

        Base layer: HEVC video (HDR10 or SDR) in the normal samples

        RPU / EL: carried in additional NAL units / tracks depending on profile

Players that don’t understand Dolby Vision just decode the HEVC BL and ignore DV metadata.
8. History, why Dolby Vision exists, who uses it

Origin:

    Created by Dolby Laboratories as a next‑gen HDR system.

    First appeared in cinema and UHD Blu‑ray, then streaming.

Why it exists:

    HDR10 (static metadata) only describes one set of mastering parameters for the whole movie.

    Dolby Vision adds dynamic metadata (per‑scene / per‑frame) to:

        Preserve creative intent across displays

        Optimize tone‑mapping for each scene

        Support both SDR and HDR displays from the same master

    It also supports:

        Dual‑layer (BL + EL) for higher quality

        Single‑layer (RPU over HDR10 BL) for streaming efficiency

Who uses it:

    Streaming platforms:

        Netflix, Disney+, Apple TV+, Prime Video, HBO Max, etc.

    Discs:

        Many UHD Blu‑ray titles

    Hardware:

        TVs (LG, Sony, TCL, Hisense, etc.)

        Streaming boxes (Apple TV 4K, Fire TV, Chromecast, etc.)

        Game consoles (Xbox Series X/S)

    Why it stuck:

        Visibly better HDR on capable displays

        Strong studio backing

        Backward compatibility (HDR10 BL)

        Works well with HEVC and now AV1 (other DV profiles)

If you want, next we can wire this into a full MP4 → Dolby Vision HEVC pipeline (BL + RPU handling, profile 5/7/8 detection, and fallback to plain HDR10).
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
