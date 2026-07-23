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

struct Atom_hvc1
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
What hvc1 is

hvc1 is the HEVC/H.265 video sample entry in stsd.
Structurally it’s like avc1, but its config box is hvcC instead of avcC.

    Base: VisualSampleEntry (width, height, etc.)

    Child: hvcC = HEVCDecoderConfigurationRecord (VPS/SPS/PPS, NAL length size, profile/level)

Binary layout of hvc1 sample entry

After the 8‑byte box header (size + type):
text

hvc1 {
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
    hvcC { ... }
    (optional) btrt
    (optional) colr
    (optional) pasp
    ...
}

C++ structs
cpp

struct HEVCNALArray {
    uint8_t  nal_unit_type;          // 32 = VPS, 33 = SPS, 34 = PPS, etc.
    bool     array_completeness;
    std::vector<std::vector<uint8_t>> nal_units;
};

struct HEVCDecoderConfigurationRecord {
    uint8_t configurationVersion;

    uint8_t general_profile_space;   // 2 bits
    uint8_t general_tier_flag;       // 1 bit
    uint8_t general_profile_idc;     // 5 bits

    uint32_t general_profile_compatibility_flags;
    uint64_t general_constraint_indicator_flags;

    uint8_t general_level_idc;

    uint8_t lengthSizeMinusOne;      // NAL length size = (value & 3) + 1

    std::vector<HEVCNALArray> arrays; // VPS/SPS/PPS/SEI...
};

struct HVC1SampleEntry {
    uint16_t data_reference_index;

    uint16_t width;
    uint16_t height;
    uint32_t horizresolution;
    uint32_t vertresolution;
    uint16_t frame_count;
    std::string compressorname;
    uint16_t depth;

    HEVCDecoderConfigurationRecord hvcc;
};

hvc1 sample entry parser
cpp

bool parse_hvc1_sample_entry(FileReader& f, uint64_t entry_end, HVC1SampleEntry& out)
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

    // compressorname (Pascal string, padded to 32 bytes)
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

        if (type == FOURCC("hvcC")) {
            parse_hvcc(f, box_end, out.hvcc);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

hvcC (HEVCDecoderConfigurationRecord) parser
cpp

bool parse_hvcc(FileReader& f, uint64_t box_end, HEVCDecoderConfigurationRecord& out)
{
    out.configurationVersion = f.read_u8(); // usually 1

    uint8_t tmp = f.read_u8();
    out.general_profile_space = (tmp >> 6) & 0x03;
    out.general_tier_flag     = (tmp >> 5) & 0x01;
    out.general_profile_idc   =  tmp       & 0x1F;

    out.general_profile_compatibility_flags = f.read_u32();

    uint64_t hi = f.read_u32();
    uint64_t lo = f.read_u32();
    out.general_constraint_indicator_flags = (hi << 32) | lo;

    out.general_level_idc = f.read_u8();

    // reserved (4 bits) + min_spatial_segmentation_idc (12 bits)
    uint16_t seg = f.read_u16();
    // reserved (6 bits) + parallelismType (2 bits)
    uint8_t parallelism = f.read_u8();
    // reserved (6 bits) + chromaFormat (2 bits)
    uint8_t chroma = f.read_u8();
    // reserved (5 bits) + bitDepthLumaMinus8 (3 bits)
    uint8_t bd_luma = f.read_u8();
    // reserved (5 bits) + bitDepthChromaMinus8 (3 bits)
    uint8_t bd_chroma = f.read_u8();

    uint16_t avgFrameRate = f.read_u16();

    uint8_t misc = f.read_u8();
    uint8_t constantFrameRate = (misc >> 6) & 0x03;
    uint8_t numTemporalLayers = (misc >> 3) & 0x07;
    uint8_t temporalIdNested  = (misc >> 2) & 0x01;
    out.lengthSizeMinusOne    = misc & 0x03;

    uint8_t numOfArrays = f.read_u8();

    out.arrays.clear();
    out.arrays.reserve(numOfArrays);

    for (uint8_t i = 0; i < numOfArrays; ++i) {
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

How to use hvc1/hvcC for decoding

    NAL length size:

cpp

int nal_length_size = (out.hvcc.lengthSizeMinusOne & 3) + 1;

    VPS/SPS/PPS are in hvcc.arrays:

        nal_unit_type == 32 → VPS

        nal_unit_type == 33 → SPS

        nal_unit_type == 34 → PPS

    MP4 samples contain length‑prefixed HEVC NAL units (same pattern as H.264):

        [len][NAL][len][NAL]... with len being nal_length_size bytes

    To feed most decoders (FFmpeg, etc.), you usually convert to Annex B:

        Replace each length with 00 00 00 01

        Prepend VPS/SPS/PPS (from hvcC) once before the first frame

The logic is almost identical to your avc1/avcC path—just with HEVC’s richer hvcC structure and NAL types.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
