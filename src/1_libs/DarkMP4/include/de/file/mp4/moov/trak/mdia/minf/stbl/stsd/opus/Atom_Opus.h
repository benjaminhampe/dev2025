#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/opus/Atom_dOps.h>

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
                            └─ Opus
                                └─ dOps
                                    ├─ version
                                    ├─ output_channel_count
                                    ├─ pre_skip
                                    ├─ input_sample_rate
                                    └─ output_gain
*/

// 🎧
struct opusData
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
struct Atom_Opus
{
    Atom atom;

    uint32_t m_entryIndex;      // Additional Benni data

    opusData m_data;            // From MemoryLayout

    std::optional<Atom_dOps> m_dOps;

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
What matters inside Opus

    dOps gives:
      version
      output_channel_count
      pre_skip
      input_sample_rate
      output_gain

    => These configs are required to initialize FAAD2, ALAC decoder, or Opus decoder.

Here’s a practical, self‑contained dOps (OpusSpecificBox) parser for Opus in MP4.
Assumes you’re already inside the dOps box (child of Opus SampleEntry) and have box_end.
Structs
cpp

struct OpusChannelMapping {
    uint8_t streamCount;
    uint8_t coupledCount;
    uint8_t channelMapping[255]; // max 255 channels, usually far less
};

struct OpusSpecificBox {
    uint8_t  version;
    uint8_t  outputChannelCount;
    uint16_t preSkip;
    uint32_t inputSampleRate;
    int16_t  outputGain;          // Q8.8 in Opus, but stored as int16 here
    uint8_t  channelMappingFamily;

    // Present only if channelMappingFamily != 0
    bool     hasMapping;
    OpusChannelMapping mapping;
};

Parser
cpp

bool parse_dops(FileReader& f, uint64_t box_end, OpusSpecificBox& out)
{
    uint64_t start = f.tell();
    uint64_t size  = box_end - start;

    // Minimum size without mapping: 1+1+2+4+2+1 = 11 bytes
    if (size < 11)
        return false;

    out.version            = f.read_u8();
    out.outputChannelCount = f.read_u8();
    out.preSkip            = f.read_u16();
    out.inputSampleRate    = f.read_u32();
    out.outputGain         = (int16_t)f.read_u16();
    out.channelMappingFamily = f.read_u8();

    out.hasMapping = false;

    if (out.channelMappingFamily != 0) {
        // Need at least 2 more bytes for streamCount + coupledCount
        if (f.tell() + 2 > box_end)
            return false;

        out.hasMapping = true;

        OpusChannelMapping& m = out.mapping;
        m.streamCount   = f.read_u8();
        m.coupledCount  = f.read_u8();

        // Then channelMapping[outputChannelCount] bytes
        if (out.outputChannelCount > 255)
            return false;

        if (f.tell() + out.outputChannelCount > box_end)
            return false;

        for (uint8_t i = 0; i < out.outputChannelCount; ++i) {
            m.channelMapping[i] = f.read_u8();
        }
    }

    // Skip any padding
    f.seek(box_end);
    return true;
}

This gives you everything you need to configure libopus:

    inputSampleRate (usually 48000)

    preSkip

    outputChannelCount

    channelMappingFamily + mapping (for surround layouts)
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
