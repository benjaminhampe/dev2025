#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/mp4a/_doc.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/mp4a/asc.h>

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

🟥 esds
    ├── size (4)
    ├── type 'esds' (4)
    └── FullBox
          ├── version (1)
          └── flags (3)
          └── DescriptorStream (variable)
                └ ES_Descriptor (tag=0x03)
                    ├── tag (1)
                    ├── vlen (1–4)  ← variable length integer
                    ├── ES_ID (2)
                    ├── flags (1)
                    │     bit7: streamDependenceFlag
                    │     bit6: URL_Flag
                    │     bit5: OCRstreamFlag
                    │     bit4..0: reserved
                    │
                    ├── dependsOn_ES_ID (2)     [optional if bit7=1]
                    ├── URLlength (1)           [optional if bit6=1]
                    ├── URLstring (URLlength)   [optional if bit6=1]
                    ├── OCR_ES_ID (2)           [optional if bit5=1]
                    │
                    ├── DecoderConfigDescriptor (tag=0x04)
                    │     ├── tag (1)
                    │     ├── vlen (1–4)
                    │     ├── objectTypeIndication (1)
                    │     ├── streamType (6 bits)
                    │     ├── upStream (1 bit)
                    │     ├── reserved (1 bit)
                    │     ├── bufferSizeDB (3)
                    │     ├── maxBitrate (4)
                    │     ├── avgBitrate (4)
                    │     │
                    │     └── DecoderSpecificInfo (tag=0x05)
                    │           ├── tag (1)
                    │           ├── vlen (1–4)
                    │           └── AudioSpecificConfig (vlen bytes)
                    │
                    └── SLConfigDescriptor (tag=0x06)
                        ├── tag (1)
                        ├── vlen (1–4)
                        └── SLConfigData (vlen bytes)
*/

//🧩 Helper: read ISO/IEC 14496‑1 “sizeOfInstance” (7‑bit continuation)
struct ESDS_Util
{
/*
    static int32_t
    parseVLQ(File & file, uint32_t* value )
    {
        int32_t consumed = 0;
        uint8_t c;     // Read 1st byte, hopefully the last.
        consumed += file.read_u8(&c);

        uint32_t r = c & 0x7F;  // Extract value part from 'c' into 'r'.
        while ( c >= 0x80 )
        {
            consumed += file.read_u8(&c); // Read next byte, hopefully the last.
            r = ( r << 7 ) | ( 0x7F & c );   // Extract value part from 'c' into 'r'.
        }

        //DE_DEBUG("[",hexStr(uint64_t(ptr)),"] Parsed VLQ = ",retVal," after ",n," bytes")
        *value = r;
        return consumed;
    }
*/
    static int32_t
    read_vlen(File& file, uint32_t* out)
    {
        int32_t consumed = 0;
        uint32_t size = 0;
        for (int i = 0; i < 4; ++i)
        {
            uint8_t b;
            consumed += file.read_u8(&b);
            size = (size << 7) | (b & 0x7F);
            if ((b & 0x80) == 0)
                break;
        }

        DE_WARN("Consumed(",consumed,"), vlen(",size,")")

        if (out)
        {
            *out = size;
        }

        return consumed;
    }
};


//🧩 Descriptor tags
enum
{
    ESDS_TAG_ES_DESCRIPTOR            = 0x03,
    ESDS_TAG_DECODER_CONFIG           = 0x04,
    ESDS_TAG_DECODER_SPECIFIC_INFO    = 0x05,
    ESDS_TAG_SL_CONFIG                = 0x06
};

//🧩 0x05 ASC container
struct ESDS_DecoderSpecificInfo // DSI -> contains ASC (AudioSpecificConfig)
{
    const uint8_t* data;
    uint32_t       size;

    aac::AscInfo m_asc;
};

//🧩 0x04
struct ESDS_DecoderConfig
{
    uint8_t  objectTypeIndication;
    uint8_t  streamType;          // high 6 bits
    uint8_t  upStream;            // 1 bit
    uint8_t  reserved;            // 1 bit
    uint32_t bufferSizeDB;
    uint32_t maxBitrate;
    uint32_t avgBitrate;

    ESDS_DecoderSpecificInfo decSpecific;
};

//🧩 0x03
struct ES_Descriptor // tag = 0x03
{
    uint16_t esID;
    uint8_t flags;

    bool bStreamDependence;
    bool bUrl;
    bool bOcrStream;
    bool bStreamPriority;

    uint16_t dependsOnESID;   // if bStreamDependence
    uint8_t  urlLength;       // if bUrl
    std::string url;          // if bUrl
    uint16_t ocrESID;         // if bOcrStream

    ESDS_DecoderConfig decConfig;
};

//🧩
struct esdsHeader
{
    uint8_t version;
    uint8_t flags[3];
};

// 🎧
struct Atom_esds
{
    Atom atom;

    esdsHeader m_header;

    ES_Descriptor m_esd;

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", ";
        o << m_esd.decConfig.decSpecific.m_asc.str();
        return o.str();
    }


    static bool parse_decoder_specific_info(File& file, int64_t beg, int64_t end,
                                            ESDS_DecoderSpecificInfo& dsi)
    {
        file.seek(beg);

        if (end - beg == 0)
        {
            DE_WARN("Got empty 0x05 ASC")
            return true;
        }

        dsi.size = end-beg;
        dsi.data = nullptr;


        if (aac::parse_asc(file, beg, end, dsi.m_asc))
        {
            // info.audio_object_type
            // info.sampling_frequency
            // info.channel_configuration
            // info.sbr_present, info.ps_present
            // info.ext_sampling_frequency (if SBR/PS)
        }

        DE_OK("Got ASC with ",end-beg," bytes")


        // You probably want to copy this into your own buffer.
        // Here we assume you have a way to get a pointer or you replace this with a copy.
        // For now, we just read into a temp buffer and store pointer as nullptr.
        // Adapt to your own memory model.

        return true;
    }

/*
esds
  version+flags (4)

DescriptorStream
└ ES_Descriptor (tag=0x03)
    │
    ├── DecoderConfigDescriptor (tag=0x04)
    │     ├── tag (1)
    │     ├── vlen (1–4)
    │     │------------------------------------ parse_decoder_config()
    │     ├── objectTypeIndication (1)
    │     ├── streamType (6 bits)
    │     ├── upStream (1 bit)
    │     ├── reserved (1 bit)
    │     ├── bufferSizeDB (3)
    │     ├── maxBitrate (4)
    │     ├── avgBitrate (4)
    │     │
    │     └── DecoderSpecificInfo (tag=0x05)
    │           ├── tag (1)
    │           ├── vlen (1–4)
    │           └── AudioSpecificConfig (vlen bytes)
    │
    └── SLConfigDescriptor (tag=0x06)
*/

    static bool parse_decoder_config(File& file, int64_t beg, int64_t end,
                                          ESDS_DecoderConfig& dc)
    {
        if (end-beg < 13)
        {
            DE_ERROR("No minimal data size 13")
            return false;
        }

        file.seek(beg);

        // (1 Byte)
        file.read_u8(&dc.objectTypeIndication);

        // (1 Byte) = streamType (6 bits) + upStream (1 bit) + reserved (1 bit)
        uint8_t tmp;
        file.read_u8(&tmp);
        dc.streamType = (tmp >> 2) & 0x3F; // 6 bit
        dc.upStream   = (tmp >> 1) & 0x01; // 1 bit
        dc.reserved   = tmp & 0x01; // 1 bit lsb

        // (3 Byte) bufferSizeDB
        file.read_u24_be(&dc.bufferSizeDB);

        // (4 Byte) maxBitrate
        file.read_u32_be(&dc.maxBitrate);

        // (4 Byte) avgBitrate
        file.read_u32_be(&dc.avgBitrate);

        // DecoderSpecificInfo (tag=0x05)
        dc.decSpecific.data = nullptr;
        dc.decSpecific.size = 0;

        int64_t cur = file.tell();
        while (cur < end)
        {
            file.seek(cur);

            uint8_t tag;
            cur += file.read_u8(&tag);

            uint32_t len;
            cur += ESDS_Util::read_vlen(file,&len);

            if (cur + len > end)
            {
                DE_ERROR("")
                return false;
            }

            if (tag == ESDS_TAG_DECODER_SPECIFIC_INFO)
            {
                DE_WARN("0x05 Nested ASC offset(",cur,"), vlen(",len,")")
                if (!parse_decoder_specific_info(file, cur, cur + len, dc.decSpecific))
                {
                    DE_ERROR("")
                    return false;
                }
            }
            else
            {
                DE_ERROR("skip unknown sub-descriptor")
            }

            cur += len;
        }

        return true;
    }

/*
esds
  version+flags (4)

DescriptorStream
└ ES_Descriptor (tag=0x03)
    ├── tag (1)
    ├── vlen (1–4)  ← variable length integer
    ├── ES_ID (2)
    ├── flags (1)
    │     bit7: streamDependenceFlag
    │     bit6: URL_Flag
    │     bit5: OCRstreamFlag
    │     bit4..0: reserved
    │
    ├── dependsOn_ES_ID (2)     [optional if bit7=1]
    ├── URLlength (1)           [optional if bit6=1]
    ├── URLstring (URLlength)   [optional if bit6=1]
    ├── OCR_ES_ID (2)           [optional if bit5=1]
    │
    ├── DecoderConfigDescriptor (tag=0x04)
    │     ├── tag (1)
    │     ├── vlen (1–4)
    │     ├── objectTypeIndication (1)
    │     ├── streamType (6 bits)
    │     ├── upStream (1 bit)
    │     ├── reserved (1 bit)
    │     ├── bufferSizeDB (3)
    │     ├── maxBitrate (4)
    │     ├── avgBitrate (4)
    │     │
    │     └── DecoderSpecificInfo (tag=0x05)
    │           ├── tag (1)
    │           ├── vlen (1–4)
    │           └── AudioSpecificConfig (vlen bytes)
    │
    └── SLConfigDescriptor (tag=0x06)
        ├── tag (1)
        ├── vlen (1–4)
        └── SLConfigData (vlen bytes)
*/
    static bool parse_descriptor(File& file, int64_t beg, int64_t end, ES_Descriptor& desc)
    {
        file.seek(beg);

        file.read_u16_be(&desc.esID);   // 2 Bytes
        file.read_u8(&desc.flags);      // 1 Byte

        desc.bStreamDependence = (desc.flags >> 7) & 0x01;
        desc.bUrl              = (desc.flags >> 6) & 0x01;
        desc.bOcrStream        = (desc.flags >> 5) & 0x01;
        desc.bStreamPriority   =  desc.flags & 0x1F;

        if (desc.bStreamDependence)
        {
            file.read_u16_be(&desc.dependsOnESID);
        }
        else
        {
            desc.dependsOnESID = 0;
        }

        if (desc.bUrl)
        {
            file.read_u8(&desc.urlLength);

            if (file.tell() + desc.urlLength > end)
            {
                DE_ERROR("")
                return false;
            }

            // skip URL string
            char url[257] {};
            file.read(url,desc.urlLength);
            desc.url = url;
        }
        else
        {
            desc.urlLength = 0;
        }

        if (desc.bOcrStream)
        {
            if (file.tell() + 2 > end)
            {
                DE_ERROR("")
                return false;
            }

            file.read_u16_be(&desc.ocrESID);
        }
        else
        {
            desc.ocrESID = 0;
        }

        // Now parse nested descriptors (DecoderConfig, SLConfig, etc.)
        int64_t cur = file.tell();
        while (cur < end)
        {
            file.seek(cur);

            uint8_t tag;
            cur += file.read_u8(&tag);

            uint32_t len;
            cur += ESDS_Util::read_vlen(file,&len);

            if (cur + len > end)
            {
                DE_ERROR("")
                return false;
            }
            if (tag == ESDS_TAG_DECODER_CONFIG)
            {
                DE_WARN("0x04 Nested DecoderConfigDescriptor offset(",cur,"), vlen(",len,")")
                if (!parse_decoder_config(file, cur, cur+len, desc.decConfig))
                {
                    DE_ERROR("")
                    return false;
                }
            }

            cur += len;
        }
        return true;
    }

    bool parse(File & file)
    {
        DE_WARN(atom.str())
        file.seek(atom.dataBeg());

        int64_t beg = atom.dataBeg();
        int64_t end = atom.dataEnd();

        if (beg + 4 > end)
        {
            DE_ERROR("")
            return false;
        }

        // 4 Bytes version + flags.
        file.read_u8(&m_header.version);
        file.read_u8(&m_header.flags[0]);
        file.read_u8(&m_header.flags[1]);
        file.read_u8(&m_header.flags[2]);

        int64_t cur = file.tell();
        while (cur < end)
        {
            file.seek(cur);
            //DE_DEBUG("cur(",dbHex(uint32_t(cur)),") end(",dbHex(uint32_t(end)),"), remain(",end - cur,")")

            uint8_t tag;
            cur += file.read_u8(&tag);
            //DE_DEBUG("cur(",dbHex(uint32_t(cur)),") end(",dbHex(uint32_t(end)),"), remain(",end - cur,")")

            uint32_t len;
            cur += ESDS_Util::read_vlen(file, &len);
            //cur += ESDS_Util::parseVLQ(file, &len);
            //DE_DEBUG("cur(",dbHex(uint32_t(cur)),") end(",dbHex(uint32_t(end)),"), remain(",end - cur,")")

            // DE_DEBUG("tag(", dbHex(tag), "), len(",len,")")

            // int64_t payload_beg = cur;
            // int64_t payload_end = payload_beg + len;
            // //cur = payload_beg; // wir parsen erstmal IN der Payload

            if (cur + len > end)
            {
                DE_ERROR("cur + len > end")
                return false;
            }

            // if (cur + len > end)
            // {
            //     DE_ERROR("cur + len > end")
            //     return false;
            // }

            cur = file.tell();

            switch (tag)
            {
                case 0x03: // ES_Descriptor
                {
                    DE_WARN("0x03 ES Descriptor offset(",cur,"), vlen(",len,")")
                    parse_descriptor(file, cur, cur + len, m_esd);

                } break;

                case 0x04: // DecoderConfigDescriptor
                {
                    DE_WARN("0x04 Flat DecoderConfigDescriptor offset(",cur,"), vlen(",len,")")
                    parse_decoder_config(file, cur, cur + len, m_esd.decConfig);
                    // skip to nested descriptors
                } break;

                case 0x05: // DecoderSpecificInfo (ASC)
                {
                    DE_WARN("0x05 Flat ASC offset(",cur,"), vlen(",len,")")
                    parse_decoder_specific_info(file, cur, cur + len, m_esd.decConfig.decSpecific);
                } break;

                default:
                    DE_ERROR("Unknown tag(",dbHex(tag),"), len(",len,")")
                    break;
            }

            DE_DEBUG("Next")
            cur += len;
            DE_DEBUG("cur(",dbHex(uint32_t(cur)),") end(",dbHex(uint32_t(end)),"), remain(",end - cur,")")
        }
        return true;
    }
};



} // end namespace mp4.
} // end namespace file.
} // end namespace de.
