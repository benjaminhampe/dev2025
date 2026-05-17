#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/mp4a/esds.h>

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
                            └─ mp4a (AAC)
                                ├─ data_reference_index
                                ├─ channelcount
                                ├─ samplerate
                                └─ esds
                                    ├─ ES_Descriptor
                                    ├─ DecoderConfigDescriptor
                                    └─ DecoderSpecificInfo (ASC)
                                        → AAC object type, sample rate index, channels
*/

// 🎧
struct mp4aData
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
struct Atom_mp4a
{
    Atom atom;

    uint32_t m_entryIndex;      // Additional Benni data

    mp4aData m_data;            // From MemoryLayout

    std::optional<Atom_esds> m_esds;

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
/*
    mp4a
     ├── esds
     │     └── 03 (len=48)
     │
     ├── 04 (DecoderConfigDescriptor)
     │
     └── 05 (DecoderSpecificInfo / ASC)
*/
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

        MiniParser::parse(file, file.tell(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("esds"))
                {
                    Atom_esds a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_esds = a;
                }
                else
                {
                    DE_WARN(found.str())
                }
            });
    }

};

/*
🧩 2. stsd — Detect Codec + Extract Decoder Config
    This is the most important atom for codec detection.

    Inside stsd you get sample entries:
    Sample Entry	Codec	What you extract
    mp4a	AAC	esds → AudioSpecificConfig
    alac	ALAC	alac atom → magic cookie
    Opus	Opus	dOps → Opus header
    fLaC	FLAC	dfLa → FLAC STREAMINFO
    lpcm	PCM	sample size, channels, rate
    ac-3 / ec-3	Dolby	dac3 / dec3

What matters inside stsd:

    AAC (mp4a)

    esds → DecoderSpecificInfo → AudioSpecificConfig (ASC)
    ASC gives:
      object_type (AAC LC = 2)
      sample_rate_index
      channel_config

    ALAC (alac)

    alac atom gives:
      frameLength
      bitDepth
      channels
      maxFrameBytes
      Rice parameters

    Opus (Opus)

    dOps gives:
      version
      output_channel_count
      pre_skip
      input_sample_rate
      output_gain

    => These configs are required to initialize FAAD2, ALAC decoder, or Opus decoder.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
