#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {
namespace aac {

//🧩 AOT
enum eAudioObjectType
{
    AOT_NULL        = 0,
    AOT_AAC_MAIN    = 1,
    AOT_AAC_LC      = 2,
    AOT_AAC_SSR     = 3,
    AOT_AAC_LTP     = 4,
    AOT_SBR         = 5,
    AOT_AAC_SCAL    = 6,
    AOT_TWINVQ      = 7,
    AOT_CELP        = 8,
    AOT_HVXC        = 9,
    AOT_ER_AAC_LC   = 17,
    AOT_ER_AAC_LTP  = 19,
    AOT_ER_AAC_SCAL = 20,
    AOT_ER_AAC_LD   = 23,
    AOT_ER_AAC_ELD  = 39,
    AOT_PS          = 29
};

//🧩 AOT -> std::string
inline std::string eAudioObjectType_to_str( uint8_t aot )
{
    switch (aot)
    {
        case AOT_NULL: return "AOT_NULL";
        case AOT_AAC_MAIN: return "AOT_AAC_MAIN";
        case AOT_AAC_LC: return "AOT_AAC_LC";
        case AOT_AAC_SSR: return "AOT_AAC_SSR";
        case AOT_AAC_LTP: return "AOT_AAC_LTP";
        case AOT_SBR: return "AOT_SBR";
        case AOT_AAC_SCAL: return "AOT_AAC_SCAL";
        case AOT_TWINVQ: return "AOT_TWINVQ";
        case AOT_CELP: return "AOT_CELP";
        case AOT_HVXC: return "AOT_HVXC";
        case AOT_ER_AAC_LC: return "AOT_ER_AAC_LC";
        case AOT_ER_AAC_LTP: return "AOT_ER_AAC_LTP";
        case AOT_ER_AAC_SCAL: return "AOT_ER_AAC_SCAL";
        case AOT_ER_AAC_LD: return "AOT_ER_AAC_LD";
        case AOT_ER_AAC_ELD: return "AOT_ER_AAC_ELD";
        case AOT_PS: return "AOT_PS";
        default: return "AOT_UNKNOWN";
    }
};

//🧩 SampleRate
inline uint32_t aac_sample_rate_from_index(uint32_t idx)
{
    static const uint32_t table[16] = {
        96000, 88200, 64000, 48000,
        44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000,
        7350, 0, 0, 0
    };
    return idx < 16 ? table[idx] : 0;
}
/*
📘 Channel configuration table

Here is the official meaning of each value:
Value	Meaning	Used by
0       PCE-defined (Program Config Element)	Rare; used by encoders with custom layouts
1       1 channel (mono)	Common
2       2 channels (stereo)	Most AAC files (YouTube, Apple, Android)
3       3 channels	Rare
4       4 channels	Surround content
5       5 channels	Surround content
6       5.1 channels	Blu‑ray, broadcast AAC
7       7.1 channels	Rare in AAC
8–15	Reserved	Not used

🔥 Important rule
If channel_configuration != 0:

You must not parse a PCE.
The channel layout is fully defined by this 3‑bit value.
If channel_configuration == 0:

You must parse a Program Config Element from the AAC raw bitstream.

This is the only case where AAC channel layout becomes complicated.
🧠 Why this matters for MP4 demuxing

Inside MP4:

    The ASC is stored in the DecoderSpecificInfo (0x05) descriptor.

    The channel_configuration is the only reliable channel count unless it is 0.

    MP4 does not store a PCE separately — the decoder must parse it from the AAC frame.

So your demuxer should: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (asc.channel_configuration != 0)
        channels = asc.channel_configuration;
    else
        channels = parse_pce_from_raw_aac();

🎯 AAC AudioSpecificConfig (ASC) — Field‑by‑Field Explanation
cpp

struct AscInfo
{
    uint8_t  aot = 0; // audio_object_type
    uint8_t  eaot = 0; // extension_audio_object_type, SBR/PS if present

    uint8_t  sampling_frequency_index = 0;
    uint32_t sampling_frequency       = 0;

    uint8_t  channel_configuration    = 0;

    bool     sbr_present = false;
    bool     ps_present = false;

    uint8_t  ext_sampling_frequency_index = 0;
    uint32_t ext_sampling_frequency       = 0;
};

🔥 1. aot — Audio Object Type (5 bits)

Where it comes from:
The first 5 bits of the ASC.

Meaning:
Defines the AAC profile or codec mode.

Examples:

    2 → AAC LC (most common)

    5 → SBR (HE‑AAC)

    29 → PS (HE‑AAC v2)

    23 → AAC LD

    39 → AAC ELD

Used by:
All AAC decoders (FAAD2, FDK‑AAC, CoreAudio, Android).
🔥 2. eaot — Extension Audio Object Type

Where it comes from:
If the ASC contains an extension (SBR/PS), the decoder reads a second AOT.

Meaning:
Defines the extension profile, usually:

    5 → SBR (Spectral Band Replication)

    29 → PS (Parametric Stereo)

When present:
Only when the ASC contains an extension element:
Code

extensionAudioObjectType
extensionSamplingFrequencyIndex

Used by:
HE‑AAC and HE‑AAC v2 decoders.
🔥 3. sampling_frequency_index (4 bits)

Where it comes from:
Bits 5–8 of the ASC.

Meaning:
Index into the AAC sampling frequency table:
Index	Frequency
0	96000
1	88200
2	64000
3	48000
4	44100
5	32000
6	24000
7	22050
8	16000
9	12000
10	11025
11	8000
12	7350

Used by:
Decoder initialization.
🔥 4. sampling_frequency (computed)

Meaning:
The actual sample rate in Hz, derived from the index.

Example:
Code

sampling_frequency_index = 4 → sampling_frequency = 44100

🔥 5. channel_configuration (3 bits)

Where it comes from:
Bits 9–11 of the ASC.

Meaning:
Defines the channel layout, unless it is 0.
Value	Meaning
0	PCE-defined (must parse Program Config Element)
1	Mono
2	Stereo
3	3 channels
4	4 channels
5	5 channels
6	5.1
7	7.1

Used by:
Decoder to determine channel count.
🔥 6. sbr_present — SBR flag

Meaning:
True if the ASC contains an SBR extension.

How detected:
When the extension AOT is 5.

Used by:
HE‑AAC decoders to know that the stream is dual‑rate (core AAC + SBR).
🔥 7. ps_present — Parametric Stereo flag

Meaning:
True if the ASC contains a PS extension.

How detected:
When the extension AOT is 29.

Used by:
HE‑AAC v2 decoders.
🔥 8. ext_sampling_frequency_index

Where it comes from:
Immediately after the extension AOT.

Meaning:
The SBR sampling frequency index, which is usually:
Code

ext_sampling_frequency = sampling_frequency * 2

Example:

    Core AAC: 44100

    SBR: 88200

Used by:
HE‑AAC decoders to set the output sample rate.
🔥 9. ext_sampling_frequency (computed)

Meaning:
Actual extension sample rate in Hz.

Example:

ext_sampling_frequency_index = 3 → 48000 Hz

🎉 Summary Table
Field	Meaning	Used by
aot	Core AAC profile	All decoders
eaot	Extension profile (SBR/PS)	HE‑AAC
sampling_frequency_index	Index into AAC table	Decoder init
sampling_frequency	Actual Hz	Decoder init
channel_configuration	Channel layout or PCE	Decoder
sbr_present	SBR extension present	HE‑AAC
ps_present	PS extension present	HE‑AAC v2
ext_sampling_frequency_index	SBR sample rate index	HE‑AAC
ext_sampling_frequency	SBR sample rate	HE‑AAC
👉 Want the next step?

I can generate:

    A full ASC parser that fills this struct

    A full ESDS parser that extracts the ASC

    A full AAC decoder initializer (FAAD2 or FDK‑AAC compatible)

*/

//🧩 ChannelConfig -> String
inline std::string channel_cfg_str(int cfg)
{
    switch (cfg)
    {
        case 0: return "PCE-defined (Program Config Element)";
        case 1: return "1 channel (Mono)";
        case 2: return "2 channels (Stereo)";
        case 3: return "3 channels";
        case 4: return "4 channels";
        case 5: return "5 channels";
        case 6: return "5.1 channels";
        case 7: return "7.1 channels";
        default: return "Reserved / Invalid";
    }
}

//🔥 -> FEED into FAAD2 AAC decoder lib
struct AscInfo
{
    std::vector<uint8_t> raw; // AAC decoder wants these precious 16 bytes.

    // This all is kinda for debugging purposes

    uint8_t  aot = 0; // audio_object_type
    uint8_t  eaot = 0; // extension_audio_object_type, SBR/PS if present

    uint8_t  sampling_frequency_index = 0;
    uint32_t sampling_frequency       = 0;

    uint8_t  channel_configuration    = 0;

    bool     sbr_present = false;
    bool     ps_present = false;

    uint8_t  ext_sampling_frequency_index = 0;
    uint32_t ext_sampling_frequency       = 0;

    std::string str() const
    {
        std::ostringstream o; o <<
        "aot(" << eAudioObjectType_to_str(aot) << "), "
        "eaot(" << eAudioObjectType_to_str(eaot) << "), "
        "sampleRate(" << sampling_frequency << "), "
        "channel_cfg(" << channel_cfg_str(channel_configuration) << ")"
        ;
        // info.audio_object_type
        // info.sampling_frequency
        // info.channel_configuration
        // info.sbr_present, info.ps_present
        // info.ext_sampling_frequency (if SBR/PS)
        return o.str();
    }

};

// This class is kinda only for debugging purposes the parser

//🧩 DebugHelper
struct BitReader
{
    File*    file;
    int64_t  pos;        // aktuelles Byteoffset im File
    int64_t  end;        // exklusives Ende
    uint32_t cache;      // 8-bit Cache
    int      bits_left;  // wie viele Bits im Cache noch übrig

    BitReader(File* f, int64_t beg, int64_t end_)
        : file(f), pos(beg), end(end_), cache(0), bits_left(0)
    {}

    inline bool refill_byte()
    {
        if (pos >= end)
            return false;

        // Fileposition setzen
        file->seek(pos);

        uint8_t b;
        pos += file->read_u8(&b);   // du hast read_u8()

        cache = b;
        bits_left = 8;
        return true;
    }

    uint32_t get_bits(int n)
    {
        uint32_t out = 0;

        while (n > 0)
        {
            if (bits_left == 0)
            {
                if (!refill_byte())
                    return out; // truncated
            }

            int take = (n < bits_left) ? n : bits_left;

            out = (out << take) |
                  ((cache >> (bits_left - take)) & ((1u << take) - 1u));

            bits_left -= take;
            n -= take;
        }
        return out;
    }

    uint32_t peek_bits(int n)
    {
        BitReader tmp = *this;
        return tmp.get_bits(n);
    }

    bool more_data() const
    {
        return (pos < end) || (bits_left > 0);
    }
};

//🧩 DebugHelper
inline uint8_t read_audio_object_type(BitReader& br)
{
    uint8_t aot = (uint8_t)br.get_bits(5);
    if (aot == 31) {
        // 31 means: 32 + 6 bits
        aot = (uint8_t)(32 + br.get_bits(6));
    }
    return aot;
}

//🧩 DebugHelper
inline bool parse_asc(File& file, int64_t beg, int64_t end, AscInfo& out)
{
    int64_t len = end - beg;

    if (len < 1)
    {
        DE_ERROR("malformed len ",len)
        return false;
    }

    // <precious> What AAC decoder wants -> 16 raw ASC bytes.
    out.raw.resize(len);
    file.seek(beg);
    file.read(out.raw.data(),len);
    // </precious>

    // <debug> Benni's debug ASC parser
    // TODO: Memory bitreader operating on 'out.raw', not file.
    BitReader br(&file, beg, end);

    // audioObjectType
    out.aot = read_audio_object_type(br);

    // samplingFrequencyIndex
    out.sampling_frequency_index = (uint8_t)br.get_bits(4);
    if (out.sampling_frequency_index == 0x0F)
    {
        // explicit frequency
        out.sampling_frequency = br.get_bits(24);
    }
    else
    {
        out.sampling_frequency = aac_sample_rate_from_index(out.sampling_frequency_index);
    }

    // channelConfiguration
    out.channel_configuration = (uint8_t)br.get_bits(4);

    // We skip GASpecificConfig / other core details.
    // For LC/HE/HEv2 this is usually enough.

    // --- Extension: SBR / PS detection ---
    // We only try if there are still bits left.
    if (!br.more_data())
        return true;

    // Some streams embed SBR/PS as extensionAudioObjectType
    // pattern: syncExtensionType (11 bits) == 0x2b7, then extensionAudioObjectType
    // But some encoders just put AOT_SBR directly after core AOT.
    // We'll implement the standard syncExtensionType path.

    // Save reader state to allow a quick check
    BitReader br_ext = br;

    // Try to find syncExtensionType (0x2b7)
    // We don't know exact alignment, but spec says it's byte-aligned after GASpecificConfig.
    // For a minimal parser, we assume current alignment is correct.
    if (br_ext.more_data())
    {
        uint32_t sync = br_ext.get_bits(11);
        if (sync == 0x2b7)
        {
            uint8_t ext_aot = read_audio_object_type(br_ext);
            out.eaot = ext_aot;

            if (ext_aot == AOT_SBR || ext_aot == AOT_PS)
            {
                out.sbr_present = true;
                if (ext_aot == AOT_PS)
                    out.ps_present = true;

                out.ext_sampling_frequency_index = (uint8_t)br_ext.get_bits(4);
                if (out.ext_sampling_frequency_index == 0x0F)
                {
                    out.ext_sampling_frequency = br_ext.get_bits(24);
                }
                else
                {
                    out.ext_sampling_frequency = aac_sample_rate_from_index(out.ext_sampling_frequency_index);
                }

                // Commit extension reader state
                br = br_ext;
            }
        }
    }
    //</debug>
    return true;
}

/*
// asc_ptr, asc_size from esds (tag 0x05)
AscInfo info;
if (parse_aac_audio_specific_config(asc_ptr, asc_size, info)) {
    // info.audio_object_type
    // info.sampling_frequency
    // info.channel_configuration
    // info.sbr_present, info.ps_present
    // info.ext_sampling_frequency (if SBR/PS)
}
*/

} // end namespace aac.
} // end namespace mp4.
} // end namespace file.
} // end namespace de.

/*

1. Bit layout recap

AudioSpecificConfig (ASC), high‑level:

    audioObjectType: 5 bits

    samplingFrequencyIndex: 4 bits

        if 1111 → samplingFrequency: 24 bits

    channelConfiguration: 4 bits

    Optionally followed by GASpecificConfig (we mostly skip)

    Optionally extensionAudioObjectType (SBR/PS) with:

        extensionSamplingFrequencyIndex / extensionSamplingFrequency

We only care about:

    core AOT (LC, HE, HEv2, etc.)

    core sample rate

    channel config

    SBR/PS presence + extension sample rate

2. Data structures

enum AacAudioObjectType {
    AOT_NULL        = 0,
    AOT_AAC_MAIN    = 1,
    AOT_AAC_LC      = 2,
    AOT_AAC_SSR     = 3,
    AOT_AAC_LTP     = 4,
    AOT_SBR         = 5,
    AOT_AAC_SCAL    = 6,
    AOT_TWINVQ      = 7,
    AOT_CELP        = 8,
    AOT_HVXC        = 9,
    AOT_ER_AAC_LC   = 17,
    AOT_ER_AAC_LTP  = 19,
    AOT_ER_AAC_SCAL = 20,
    AOT_ER_AAC_LD   = 23,
    AOT_ER_AAC_ELD  = 39,
    AOT_PS          = 29
};

static inline uint32_t aac_sample_rate_from_index(uint32_t idx) {
    static const uint32_t table[16] = {
        96000, 88200, 64000, 48000,
        44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000,
        7350, 0, 0, 0
    };
    return idx < 16 ? table[idx] : 0;
}

struct AacAscInfo {
    uint8_t  audio_object_type        = 0;
    uint8_t  extension_audio_object_type = 0; // SBR/PS if present

    uint8_t  sampling_frequency_index = 0;
    uint32_t sampling_frequency       = 0;

    uint8_t  channel_configuration    = 0;

    bool     sbr_present              = false;
    bool     ps_present               = false;

    uint8_t  ext_sampling_frequency_index = 0;
    uint32_t ext_sampling_frequency       = 0;
};

3. Minimal bitreader
cpp

struct BitReader {
    const uint8_t* p;
    const uint8_t* end;
    uint32_t       cache;
    int            bits_left;

    BitReader(const uint8_t* data, size_t size)
        : p(data), end(data + size), cache(0), bits_left(0) {}

    uint32_t get_bits(int n) {
        uint32_t out = 0;
        while (n > 0) {
            if (bits_left == 0) {
                if (p >= end) return out; // truncated; caller should guard
                cache = *p++;
                bits_left = 8;
            }
            int take = n < bits_left ? n : bits_left;
            out = (out << take) | ((cache >> (bits_left - take)) & ((1u << take) - 1u));
            bits_left -= take;
            n -= take;
        }
        return out;
    }

    uint32_t peek_bits(int n) {
        BitReader tmp = *this;
        return tmp.get_bits(n);
    }

    bool more_data() const {
        return (p < end) || (bits_left > 0);
    }
};

4. Core ASC parsing
cpp

static uint8_t read_audio_object_type(BitReader& br) {
    uint8_t aot = (uint8_t)br.get_bits(5);
    if (aot == 31) {
        // 31 means: 32 + 6 bits
        aot = (uint8_t)(32 + br.get_bits(6));
    }
    return aot;
}

bool parse_aac_audio_specific_config(const uint8_t* asc, size_t asc_size,
                                     AacAscInfo& out)
{
    if (!asc || asc_size == 0) return false;

    BitReader br(asc, asc_size);

    // audioObjectType
    out.audio_object_type = read_audio_object_type(br);

    // samplingFrequencyIndex
    out.sampling_frequency_index = (uint8_t)br.get_bits(4);
    if (out.sampling_frequency_index == 0x0F) {
        // explicit frequency
        out.sampling_frequency = br.get_bits(24);
    } else {
        out.sampling_frequency = aac_sample_rate_from_index(out.sampling_frequency_index);
    }

    // channelConfiguration
    out.channel_configuration = (uint8_t)br.get_bits(4);

    // We skip GASpecificConfig / other core details.
    // For LC/HE/HEv2 this is usually enough.

    // --- Extension: SBR / PS detection ---
    // We only try if there are still bits left.
    if (!br.more_data())
        return true;

    // Some streams embed SBR/PS as extensionAudioObjectType
    // pattern: syncExtensionType (11 bits) == 0x2b7, then extensionAudioObjectType
    // But some encoders just put AOT_SBR directly after core AOT.
    // We'll implement the standard syncExtensionType path.

    // Save reader state to allow a quick check
    BitReader br_ext = br;

    // Try to find syncExtensionType (0x2b7)
    // We don't know exact alignment, but spec says it's byte-aligned after GASpecificConfig.
    // For a minimal parser, we assume current alignment is correct.
    if (br_ext.more_data()) {
        uint32_t sync = br_ext.get_bits(11);
        if (sync == 0x2b7) {
            uint8_t ext_aot = read_audio_object_type(br_ext);
            out.extension_audio_object_type = ext_aot;

            if (ext_aot == AOT_SBR || ext_aot == AOT_PS) {
                out.sbr_present = true;
                if (ext_aot == AOT_PS)
                    out.ps_present = true;

                out.ext_sampling_frequency_index = (uint8_t)br_ext.get_bits(4);
                if (out.ext_sampling_frequency_index == 0x0F) {
                    out.ext_sampling_frequency = br_ext.get_bits(24);
                } else {
                    out.ext_sampling_frequency =
                        aac_sample_rate_from_index(out.ext_sampling_frequency_index);
                }

                // Commit extension reader state
                br = br_ext;
            }
        }
    }

    return true;
}

5. Usage example

// asc_ptr, asc_size from esds (tag 0x05)
AacAscInfo info;
if (parse_aac_audio_specific_config(asc_ptr, asc_size, info)) {
    // info.audio_object_type
    // info.sampling_frequency
    // info.channel_configuration
    // info.sbr_present, info.ps_present
    // info.ext_sampling_frequency (if SBR/PS)
}

This gives you exactly what you need for:

    configuring your AAC decoder,

    detecting HE‑AAC / HE‑AACv2,

    choosing core vs. extension sample rate.

If you want, next step we can do:
map channelConfiguration → channel count &
layout and/or derive final output sample rate for HE/HEv2.
*/
