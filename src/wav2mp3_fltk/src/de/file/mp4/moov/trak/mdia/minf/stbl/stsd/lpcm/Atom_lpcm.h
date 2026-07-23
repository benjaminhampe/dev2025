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
                            └─ lpcm (uncompressed PCM)
                                ├─ sample_size
                                ├─ sample_rate
                                └─ channel_count
*/

// 🎧
struct lpcmData
{
    uint8_t  reserved[6];       // From SampleEntry
    uint16_t data_ref_index;    // From SampleEntry
    uint32_t reserved1;         // From AudioEntry 2 bytes pre_defined + 2 bytes reserved
    uint32_t reserved2;         // From AudioEntry 4 bytes reserved
    uint16_t channelcount;      // From AudioEntry
    uint16_t samplesize;        // From AudioEntry
    uint16_t pre_defined;       // From AudioEntry
    uint16_t reserved3;         // From AudioEntry
    uint32_t samplerate;        // From AudioEntry 16.16 fixed point
};

// 🎧
struct Atom_lpcm
{
    Atom atom;

    uint32_t m_entryIndex;      // Additional Benni data

    lpcmData m_data;            // From MemoryLayout


    int channels() const { return m_data.channelcount; }

    int sampleSize() const { return m_data.samplesize; }

    double sampleRate() const { return double(m_data.samplerate) / 65536.0; }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        "channels(" << channels() << "), "
        "sampleSize(" << sampleSize() << "), "
        "sampleRate(" << sampleRate() << ")"
        ;
        return o.str();
    }

    void parse(File & file, uint32_t entryIndex )
    {
        m_entryIndex = entryIndex;              // Additional Benni info

        file.seek(atom.dataBeg());

        file.read(&m_data.reserved, 6);          // From SampleEntry
        file.read_u16_be(&m_data.data_ref_index);// From SampleEntry

        file.read_u32_be(&m_data.reserved1);    // From AudioEntry
        file.read_u32_be(&m_data.reserved2);    // From AudioEntry
        file.read_u16_be(&m_data.channelcount); // From AudioEntry
        file.read_u16_be(&m_data.samplesize);   // From AudioEntry
        file.read_u16_be(&m_data.pre_defined);  // From AudioEntry
        file.read_u16_be(&m_data.reserved3);    // From AudioEntry
        file.read_u32_be(&m_data.samplerate);   // From AudioEntry
    }

};

/*
🧩 Here’s a practical, demuxer‑grade lpcm sample entry parser plus how to turn it into the LPCMFormat you used for your custom decoder.
1. Binary layout of an audio sample entry (lpcm)

All audio sample entries share the same base layout:
text

AudioSampleEntry {
    uint32 size
    uint32 type  // 'lpcm', 'sowt', 'twos', 'in24', 'in32', 'fl32', 'fl64', ...
    uint8  reserved[6]
    uint16 data_reference_index

    // AudioSampleEntry fields (28 bytes)
    uint16 version
    uint16 revision_level
    uint32 vendor
    uint16 channelcount
    uint16 samplesize
    uint16 compression_id
    uint16 packet_size
    uint32 samplerate_16_16   // 16.16 fixed-point
}

For lpcm (QuickTime/ISO LPCM), this is followed by CoreAudio LPCM extension:
text

LPCM extension {
    uint32 format_flags          // CoreAudio AudioStreamBasicDescription flags
    uint32 bytes_per_packet      // constant bytes per packet (or 0)
    uint32 frames_per_packet     // constant frames per packet (or 0)
    uint32 bytes_per_frame       // constant bytes per frame (or 0)
    uint32 channels_per_frame    // usually == channelcount
    uint32 bits_per_channel      // usually == samplesize
}

In many files, bytes_per_packet, frames_per_packet, bytes_per_frame, channels_per_frame, bits_per_channel are redundant with the base fields, but you should still parse them.
2. C++ structs
cpp

struct LPCMSampleEntry {
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

    // LPCM extension (for 'lpcm')
    uint32_t format_flags;
    uint32_t bytes_per_packet;
    uint32_t frames_per_packet;
    uint32_t bytes_per_frame;
    uint32_t channels_per_frame;
    uint32_t bits_per_channel;
};

3. Parser for lpcm sample entry

Assumes you’re positioned after the 8‑byte header (size + type), and entry_end is the end offset of this sample entry box.
cpp

bool parse_lpcm_sample_entry(FileReader& f, uint64_t entry_end, LPCMSampleEntry& out)
{
    // skip 6 reserved bytes
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

    // If there is no LPCM extension, we’re done (for 'sowt', 'twos', etc.)
    if (f.tell() + 24 > entry_end) {
        out.format_flags      = 0;
        out.bytes_per_packet  = 0;
        out.frames_per_packet = 0;
        out.bytes_per_frame   = 0;
        out.channels_per_frame= out.channelcount;
        out.bits_per_channel  = out.samplesize;
        f.seek(entry_end);
        return true;
    }

    // LPCM extension (CoreAudio style)
    out.format_flags      = f.read_u32();
    out.bytes_per_packet  = f.read_u32();
    out.frames_per_packet = f.read_u32();
    out.bytes_per_frame   = f.read_u32();
    out.channels_per_frame= f.read_u32();
    out.bits_per_channel  = f.read_u32();

    f.seek(entry_end);
    return true;
}

You can reuse this for lpcm; for sowt/twos/in24/in32/fl32/fl64 you typically only have the base AudioSampleEntry (no extension).
4. Mapping to your LPCMFormat

Recall:
cpp

struct LPCMFormat {
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bits_per_sample;
    bool     is_float;
    bool     is_big_endian;
};

CoreAudio format_flags bits (most relevant):

    kAudioFormatFlagIsFloat        = 1 << 0

    kAudioFormatFlagIsBigEndian    = 1 << 1

    kAudioFormatFlagIsSignedInteger= 1 << 2

    kAudioFormatFlagIsPacked       = 1 << 3

    kAudioFormatFlagIsNonInterleaved = 1 << 5 (you usually want this off)

Mapping:
cpp

LPCMFormat to_lpcm_format(const LPCMSampleEntry& e, uint32_t fourcc)
{
    LPCMFormat fmt{};
    fmt.sample_rate    = e.samplerate;
    fmt.channels       = (uint16_t)(e.channels_per_frame ? e.channels_per_frame : e.channelcount);
    fmt.bits_per_sample= (uint16_t)(e.bits_per_channel ? e.bits_per_channel : e.samplesize);

    uint32_t flags = e.format_flags;

    fmt.is_float      = (flags & (1u << 0)) != 0;
    fmt.is_big_endian = (flags & (1u << 1)) != 0;

    // For legacy types without extension, infer from fourcc
    if (e.format_flags == 0) {
        switch (fourcc) {
        case 'sowt': // little-endian signed int16
            fmt.is_float      = false;
            fmt.is_big_endian = false;
            break;
        case 'twos': // big-endian signed int16
            fmt.is_float      = false;
            fmt.is_big_endian = true;
            break;
        case 'fl32':
            fmt.is_float      = true;
            fmt.is_big_endian = false; // usually LE
            break;
        case 'fl64':
            fmt.is_float      = true;
            fmt.is_big_endian = false;
            break;
        default:
            break;
        }
    }

    return fmt;
}

With this, you can plug directly into the decode_lpcm_frame() you already have.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
