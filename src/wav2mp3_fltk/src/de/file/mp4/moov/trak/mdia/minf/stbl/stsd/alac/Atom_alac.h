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
                            └─ alac (Apple Lossless)
                                └─ alac atom
                                    ├─ frameLength
                                    ├─ compatibleVersion
                                    ├─ bitDepth
                                    ├─ pb / mb / kb (Rice parameters)
                                    └─ maxFrameBytes
*/

// 🎧
struct alacData
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
struct Atom_alac
{
    Atom atom;

    uint32_t m_entryIndex;      // Additional Benni data

    alacData m_data;            // From MemoryLayout

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
    ALAC (alac)

    alac atom gives:
      frameLength
      bitDepth
      channels
      maxFrameBytes
      Rice parameters

Below is a complete, correct, minimal ALACSpecificConfig parser for MP4 (alac box inside the AudioSampleEntry).
This is the exact structure defined by Apple’s ALAC atom (not ISO).
It is always 36 bytes long.

No abstractions, no wrappers, no signature changes — just the structs and parsing logic you need.
🎯 ALACSpecificConfig structure (36 bytes)

Inside the alac child box:
Code

struct ALACSpecificConfig {
    uint32_t frameLength;
    uint8_t  compatibleVersion;
    uint8_t  bitDepth;
    uint8_t  pb;
    uint8_t  mb;
    uint8_t  kb;
    uint8_t  numChannels;
    uint16_t maxRun;
    uint32_t maxFrameBytes;
    uint32_t avgBitRate;
    uint32_t sampleRate;
};

This is exactly what Apple uses in .m4a ALAC files.
📦 C++ struct
cpp

struct ALACSpecificConfig {
    uint32_t frameLength;
    uint8_t  compatibleVersion;
    uint8_t  bitDepth;
    uint8_t  pb;
    uint8_t  mb;
    uint8_t  kb;
    uint8_t  numChannels;
    uint16_t maxRun;
    uint32_t maxFrameBytes;
    uint32_t avgBitRate;
    uint32_t sampleRate;
};

🧠 Parser function

Call this when you are inside the alac box (child of the AudioSampleEntry), and you know its end offset.
cpp

bool parse_alac_specific_box(FileReader& f, uint64_t box_end, ALACSpecificConfig& out)
{
    uint64_t start = f.tell();
    uint64_t size  = box_end - start;

    // ALACSpecificConfig is always 36 bytes
    if (size < 36)
        return false;

    out.frameLength      = f.read_u32();
    out.compatibleVersion= f.read_u8();
    out.bitDepth         = f.read_u8();
    out.pb               = f.read_u8();
    out.mb               = f.read_u8();
    out.kb               = f.read_u8();
    out.numChannels      = f.read_u8();
    out.maxRun           = f.read_u16();
    out.maxFrameBytes    = f.read_u32();
    out.avgBitRate       = f.read_u32();
    out.sampleRate       = f.read_u32();

    // Skip any padding (rare but legal)
    f.seek(box_end);
    return true;
}

🔍 What each field means

    frameLength
    Usually 4096 or 4608. ALAC frame size.

    compatibleVersion
    Always 0.

    bitDepth
    16, 20, 24, or 32.

    pb, mb, kb
    Predictor parameters (Rice coding).

    numChannels
    1–8.

    maxRun
    Maximum Rice run.

    maxFrameBytes
    Maximum encoded frame size.

    avgBitRate
    Usually 0 for ALAC.

    sampleRate
    44100, 48000, etc.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
