#pragma once

#if 0


// How to determine AAC / ALAC / Opus
//     Step	Where	What
// 1	hdlr	Check if track is audio ("soun")
// 2	stsd	Read sample entry type ("mp4a", "alac", "Opus")
// 3	codec atom	AAC → esds, ALAC → alac, Opus → dOps
// 4	decoder	Feed each MP4 sample (one encoded unit)

// 🎯 What ADTS actually is

// ADTS = Audio Data Transport Stream
// It’s a 7‑byte header that wraps a raw AAC access unit.

// MP4 stores raw AAC access units (no ADTS).
// FAAD2 (ADTS API) expects ADTS‑wrapped AAC.

// So you must generate:
// Code

// [ADTS header (7 bytes)]
// [raw AAC sample (from MP4)]

// 🎧 Inputs you MUST know

// You need two things:
// 1. AudioSpecificConfig (ASC) from esds atom

// This gives:

// object_type (AAC LC = 2)

// sample_rate_index

// channel_config

// 2. raw AAC sample size

// From MP4 stsz.
// 🧩 ADTS Header Format (7 bytes)
// Code

// AAAA AAAA  AAAA AAAA  AAAA AAAA  AAAA AAAA  AAAA AAAA  AAAA AAAA  AAAA AAAA
// 1111 1111 1111 0001  .... ....  .... ....  .... ....  .... ....  .... ....

// But you don’t need to memorize this — the generator below builds it correctly.

// 🔥 Minimal ADTS Generator (C++)
static inline void make_adts_header(
    uint8_t hdr[7],
    int aac_object_type,      // 2 = AAC LC
    int sample_rate_index,    // 0..12
    int channel_config,       // 1..7
    int aac_frame_length      // raw AAC size + 7
    )
{
    // ADTS fixed header
    hdr[0] = 0xFF;
    hdr[1] = 0xF1; // 1111 0001 (sync + MPEG-4 + layer + protection_absent)

    hdr[2] = ((aac_object_type - 1) << 6) |
             (sample_rate_index << 2) |
             ((channel_config >> 2) & 0x1);

    hdr[3] = ((channel_config & 3) << 6) |
             ((aac_frame_length >> 11) & 0x03);

    hdr[4] = (aac_frame_length >> 3) & 0xFF;

    hdr[5] = ((aac_frame_length & 7) << 5) | 0x1F;

    hdr[6] = 0xFC;
}

// 🧪 Example usage
/*
uint8_t adts[7];
int raw_size = sample.size;          // from stsz
int full_size = raw_size + 7;        // ADTS header + AAC data

make_adts_header(
    adts,
    asc.object_type,                 // usually 2 (AAC LC)
    asc.sample_rate_index,           // from esds
    asc.channel_config,              // from esds
    full_size
    );

// Now write:
// [adts][raw AAC bytes]
*/

// 🎯 How to get ASC (AudioSpecificConfig)
// From the MP4 'esds' atom:
// object_type = (ASC >> 11) & 0x1F
// sample_rate_index = (ASC >> 7) & 0x0F
// channel_config = (ASC >> 3) & 0x0F

// 🟢 Final summary

// MP4 stores raw AAC access units
// FAAD2 ADTS API needs ADTS‑wrapped AAC
// ADTS header = 7 bytes
// Use the generator above
// Prepend header to each sample before decoding

// 🎯 What esds actually contains
// stsd
//  └── mp4a (sample entry)
//       └── esds  ← THIS atom contains AAC decoder config

// esds is an Elementary Stream Descriptor defined by MPEG‑4 Systems.

// It contains:
//  ES_Descriptor
//  DecoderConfigDescriptor
//  DecoderSpecificInfo  ← this contains AudioSpecificConfig
//  SLConfigDescriptor
// You only care about DecoderSpecificInfo, which contains ASC.
// ASC is 2 bytes (sometimes more) that encode:
//  AAC object type (LC = 2)
//  sample rate index
//  channel config

// 🧩 ESDS structure (simplified to what we need)
/*
esds
{
    version (1 byte)
    flags   (3 bytes)

    ES_Descriptor
    {
        tag = 0x03
        length = variable

        ES_ID (2 bytes)
        flags (1 byte)

        DecoderConfigDescriptor
        {
            tag = 0x04
            length = variable

            objectTypeIndication (1 byte)
            streamType (1 byte)
            bufferSizeDB (3 bytes)
            maxBitrate (4 bytes)
            avgBitrate (4 bytes)

            DecoderSpecificInfo
            {
                tag = 0x05
                length = variable

                AudioSpecificConfig (ASC) ← **THIS is what we need**
            }
        }

        SLConfigDescriptor
        {
            tag = 0x06
            length = variable
        }
    }
}
*/

//🔥 Minimal ESDS parser (C++), This extracts ASC into a byte vector.
static uint32_t read_mp4_descr_len(int fd)
{
    // ISO 14496-1 variable length field
    uint32_t len = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t b;
        if (_read(fd, &b, 1) != 1) return 0;
        len = (len << 7) | (b & 0x7F);
        if (!(b & 0x80))
            break;
    }
    return len;
}

bool parse_esds(int fd, uint64_t payload_size, std::vector<uint8_t> &asc_out)
{
    // skip version + flags
    if (payload_size < 4) return false;
    lseek(fd, 4, SEEK_CUR);
    payload_size -= 4;

    // ES_Descriptor tag (0x03)
    uint8_t tag;
    if (_read(fd, &tag, 1) != 1) return false;
    if (tag != 0x03) return false;

    uint32_t es_len = read_mp4_descr_len(fd);
    if (es_len < 3) return false;

    // ES_ID (2 bytes) + flags (1 byte)
    lseek(fd, 3, SEEK_CUR);

    // DecoderConfigDescriptor tag (0x04)
    if (_read(fd, &tag, 1) != 1) return false;
    if (tag != 0x04) return false;

    uint32_t dec_len = read_mp4_descr_len(fd);

    // objectTypeIndication (1 byte)
    uint8_t oti;
    _read(fd, &oti, 1);

    // streamType (1 byte)
    uint8_t st;
    _read(fd, &st, 1);

    // bufferSizeDB (3 bytes)
    lseek(fd, 3, SEEK_CUR);

    // maxBitrate + avgBitrate (8 bytes)
    lseek(fd, 8, SEEK_CUR);

    // DecoderSpecificInfo tag (0x05)
    if (_read(fd, &tag, 1) != 1) return false;
    if (tag != 0x05) return false;

    uint32_t dsi_len = read_mp4_descr_len(fd);

    // Read AudioSpecificConfig
    asc_out.resize(dsi_len);
    if (_read(fd, asc_out.data(), dsi_len) != (ssize_t)dsi_len)
        return false;

    return true;
}

// 🎧 How to extract AAC parameters from ASC

// ASC is usually 2 bytes:

//      aaaaa bbbb  cccc dddd

// aaaaa = object type (AAC LC = 2)
// bbbb = sample rate index
// cccc = channel config

// Decode like this:

struct AacAsc {
    int object_type;
    int sample_rate_index;
    int channel_config;
};

AacAsc decode_asc(const std::vector<uint8_t> &asc)
{
    AacAsc out{};

    uint16_t x = (asc[0] << 8) | asc[1];

    out.object_type        = (x >> 11) & 0x1F;
    out.sample_rate_index  = (x >> 7)  & 0x0F;
    out.channel_config     = (x >> 3)  & 0x0F;

    return out;
}

/*
🧪 Example: using it

std::vector<uint8_t> asc;
parse_esds(fd, payload_size, asc);

AacAsc cfg = decode_asc(asc);

printf("AAC object type = %d\n", cfg.object_type);
printf("sample rate index = %d\n", cfg.sample_rate_index);
printf("channel config = %d\n", cfg.channel_config);
*/



static void
build_samples(const TrackTables &t, std::vector<Sample> &out)
{
    size_t n = t.sample_sizes.size();
    out.resize(n);

    // Minimal assumption: 1 sample per chunk
    for (size_t i = 0; i < n; ++i) {
        out[i].offset = t.chunk_offsets[i];
        out[i].size   = t.sample_sizes[i];
    }
}




#endif