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
                            └─ Opus
                                └─ dOps
                                    ├─ version
                                    ├─ output_channel_count
                                    ├─ pre_skip
                                    ├─ input_sample_rate
                                    └─ output_gain
*/

// 🎧
struct Atom_dOps
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
