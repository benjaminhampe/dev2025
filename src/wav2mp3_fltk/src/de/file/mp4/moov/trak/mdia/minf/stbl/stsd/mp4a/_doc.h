#pragma once

/*
🟦 ASCII‑TREE MIT BEISPIEL‑ADRESSEN (REALISTISCH)

Angenommen:

    esds payload start = 0x1000

Dann sieht der echte Byte‑Baum so aus:

    0x1000: 03                    ES_Descriptor tag
    0x1001: 81 30                 vlen = 0x30 (48 bytes)

    0x1003: ES_ID (2 bytes)
    0x1005: flags (1 byte)

    # optionals je nach flags
    0x1006: [dependsOn_ES_ID]     (optional)
    0x1008: [URLlength]           (optional)
    0x1009: [URLstring]           (optional)
    0x1009+x: [OCR_ES_ID]         (optional)

    # jetzt kommt 0x04
    0x1010: 04                    DecoderConfigDescriptor tag
    0x1011: 81 20                 vlen = 0x20 (32 bytes)
    0x1013: objectTypeIndication (1)
    0x1014: streamType/upStream/reserved (1)
    0x1015: bufferSizeDB (3)
    0x1018: maxBitrate (4)
    0x101C: avgBitrate (4)

    # jetzt kommt 0x05
    0x1020: 05                    DecoderSpecificInfo tag
    0x1021: 0x0A                  vlen = 10 bytes
    0x1022..0x102B: ASC bytes     (AudioSpecificConfig)

    # jetzt kommt 0x06
    0x102C: 06                    SLConfigDescriptor tag
    0x102D: 01                    vlen = 1
    0x102E: 00                    SLConfigData

    # Ende des 0x03 payloads
    0x1030: end of ESDS

🟩 WAS DU PARSEN MUSST – FELD FÜR FELD
    1. ES_Descriptor (0x03)
    Feld                            Größe   Pflicht?
    tag                             1       ja
    vlen                            1–4     ja
    ES_ID                           2       ja
    flags                           1       ja
    dependsOn_ES_ID                 2       optional
    URLlength                       1       optional
    URLstring                       URLlength	optional
    OCR_ES_ID                       2       optional

    2. DecoderConfigDescriptor (0x04)
    Feld                            Größe
    tag                             1
    vlen                            1–4
    objectTypeIndication            1
    streamType/upStream/reserved	1
    bufferSizeDB                    3
    maxBitrate                      4
    avgBitrate                      4

    3. DecoderSpecificInfo (0x05)
    Feld                            Größe
    tag                             1
    vlen                            1–4
    AudioSpecificConfig             vlen bytes

    4. SLConfigDescriptor (0x06)
    Feld                            Größe
    tag                             1
    vlen                            1–4
    SLConfigData                    vlen bytes

🟧 DER WICHTIGSTE PUNKT:

    **0x03 ist ein CONTAINER.

    0x04 und 0x05 liegen IMMER IN SEINER PAYLOAD.**

Darum:

    Wenn du nach 0x03 einfach cur += len machst → du springst über 0x04/0x05 hinweg

    Du musst innerhalb der 0x03‑Payload erneut Descriptoren parsen
*/






/*
Kurzfassung (präzise, technisch, vollständig):
Der esds‑Atom ist ein FullBox innerhalb der AudioSampleEntry (z. B. mp4a) und enthält genau eine MPEG‑4 Elementary Stream Descriptor‑Struktur gemäß ISO/IEC 14496‑1.
Für AAC brauchst du nur 1 einziges Feld: die DecoderSpecificInfo (ASC).
Alles andere ist optional oder kann ignoriert werden, wenn du nur AudioConfig → Decoder willst.

Unten bekommst du:

    Vollständigen ESDS‑Atom‑Parser (Byte‑genau, TLV‑basiert)

    Memory‑Layout aller Descriptor‑Typen

    Welche Felder optional sind

    Minimal‑Parser, der nur ASC extrahiert (best‑case)

🧩 ESDS Atom – Struktur (FullBox)

Der esds‑Atom ist ein FullBox:
Code

aligned(8) class ESDS extends FullBox('esds', version, flags) {
    ES_Descriptor es;
}

FullBox header:

    1 byte: version

    3 bytes: flags (meist 0)

Danach folgt ein einziger Descriptor‑Block im MPEG‑4 Systems Descriptor Format (TLV‑Struktur).
🧬 Elementary Stream Descriptor – TLV Memory Layout

Alle Descriptoren folgen diesem Format:
Code

descriptor_tag:      1 byte
size:                1–4 bytes (7-bit continuation)
payload[size]        variable

Die size ist ein 7‑bit continuation integer:
Code

byte0: 0xxxxxxx  → last
byte0: 1xxxxxxx  → more bytes follow

🧱 Relevante Descriptor‑Typen (AAC)

Tag	Name	Pflicht?	Zweck
0x03	ES_Descriptor	Ja	Enthält ES_ID, Flags, DecoderConfigDescriptor
0x04	DecoderConfigDescriptor	Ja	Enthält ObjectTypeIndication, StreamType, BufferSize, MaxBitrate, AvgBitrate
0x05	DecoderSpecificInfo	Nur dieses Feld brauchst du für AAC	Enthält AudioSpecificConfig (ASC)
0x06	SLConfigDescriptor	Optional	Meist 0x02 (predefined)

🧩 Vollständiges Memory‑Layout (Byte‑genau)
1) ES_Descriptor (tag 0x03)

    0x03
    <size>
    ES_ID (2 bytes)
    flags (1 byte)
       bit7: streamDependenceFlag
       bit6: URL_Flag
       bit5: OCRstreamFlag
       bit4..0: reserved
    [optional fields depending on flags]
    DecoderConfigDescriptor (tag 0x04)
    [optional SLConfigDescriptor (tag 0x06)]

2) DecoderConfigDescriptor (tag 0x04)

    0x04
    <size>
    objectTypeIndication (1 byte)   // AAC = 0x40
    streamType (1 byte)             // high 6 bits = type, low 2 bits = upstream/reserved
    bufferSizeDB (3 bytes)
    maxBitrate (4 bytes)
    avgBitrate (4 bytes)
    DecoderSpecificInfo (tag 0x05)

3) DecoderSpecificInfo (tag 0x05)

    0x05
    <size>
    AudioSpecificConfig (ASC bytes)

Für AAC ist ASC typischerweise 2 bytes (manchmal 3–5 bei SBR/PS).

🧪 Welche Felder sind OPTIONAL?
    Feld                        Optional?	Wann ignorierbar?
    ES_ID                       Ja          Für Decoder irrelevant
    Flags + optionale Felder	Ja          Nur für Systems‑Layer
    URL                         Ja          Nie bei AAC in MP4
    OCR                         Ja          Nie bei AAC in MP4
    SLConfigDescriptor          Ja          Meist 0x02 → ignorierbar
    bufferSizeDB                Ja          Decoder braucht es nicht
    maxBitrate / avgBitrate     Ja          Nur informativ
    DecoderSpecificInfo         NEIN        Einzige Pflicht für AAC‑Decoder

struct EsdsInfo
{
    const uint8_t* asc = nullptr;
    uint32_t asc_size = 0;
};

*/






/*

🧩 1. MPEG‑4 Descriptor TLV‑Header (alle Deskriptoren)
cpp

struct Mp4DescrHeader {
    uint8_t tag;      // 0x03, 0x04, 0x05, 0x06, ...
    uint32_t size;    // decoded 7‑bit continuation value
    const uint8_t* payload; // pointer to first byte after size field
};

Wichtig:
size ist NICHT direkt im File als 32‑bit gespeichert.
Es ist ein 1–4‑Byte 7‑bit continuation integer.
🧩 2. ES_Descriptor (tag = 0x03)

Memorylayout exakt wie im File:
cpp

struct EsDescriptor {
    uint16_t es_id;

    uint8_t flags;
    // flags:
    // bit7 = streamDependenceFlag
    // bit6 = URL_Flag
    // bit5 = OCRstreamFlag
    // bit4..0 = reserved

    // optional:
    uint16_t dependsOnEsId;   // only if bit7
    uint8_t  urlLength;       // only if bit6
    const uint8_t* urlString; // only if bit6
    uint16_t ocrEsId;         // only if bit5

    // nested descriptors follow:
    // DecoderConfigDescriptor (0x04)
    // SLConfigDescriptor (0x06)
};

🧩 3. DecoderConfigDescriptor (tag = 0x04)

Memorylayout exakt wie im File:
cpp

struct DecoderConfigDescriptor {
    uint8_t objectTypeIndication; // AAC = 0x40
    uint8_t streamType;           // high 6 bits = type, low 2 bits reserved
    uint32_t bufferSizeDB;        // 24-bit big endian
    uint32_t maxBitrate;          // 32-bit big endian
    uint32_t avgBitrate;          // 32-bit big endian

    // nested:
    // DecoderSpecificInfo (0x05)  <-- ASC
};

🧩 4. DecoderSpecificInfo (tag = 0x05)

Memorylayout exakt wie im File:
cpp

struct DecoderSpecificInfo {
    const uint8_t* asc;     // pointer to AudioSpecificConfig bytes
    uint32_t asc_size;      // length of ASC
};

Das ist das EINZIGE, was ein AAC‑Decoder braucht.
🧩 5. SLConfigDescriptor (tag = 0x06)

Memorylayout:
cpp

struct SLConfigDescriptor {
    uint8_t predefined; // meistens 0x02
};

Mehr brauchst du nicht.
🧩 6. ESDS Atom (FullBox) Memorylayout
cpp

struct EsdsBox {
    uint8_t version;
    uint8_t flags[3];

    EsDescriptor es; // contains nested descriptors
};

🧩 7. Minimaler Parser (LESBAR, KEIN BITREADER)

Du hast read_u8(), read_u16_be(), read_u24_be(), read_u32_be().
cpp

bool parse_esds(const uint8_t* data, size_t size, DecoderSpecificInfo& out)
{
    const uint8_t* p = data;
    const uint8_t* end = data + size;

    // FullBox header
    if (size < 4) return false;
    p += 4;

    while (p < end) {
        uint8_t tag = *p++;

        // decode 7-bit size
        uint32_t len = 0;
        for (int i = 0; i < 4; i++) {
            if (p >= end) return false;
            uint8_t b = *p++;
            len = (len << 7) | (b & 0x7F);
            if (!(b & 0x80)) break;
        }
        if (p + len > end) return false;

        const uint8_t* payload = p;

        if (tag == 0x05) {
            // DecoderSpecificInfo → ASC
            out.asc = payload;
            out.asc_size = len;
            return true;
        }

        p += len;
    }

    return false;
}

🧨 8. Was passiert, wenn du KEIN 0x05 findest?

Das ist wichtig.
Fall A: ESDS enthält 0x03 + 0x04, aber KEIN 0x05

→ Das File ist ungültig für AAC.

Denn:

    AAC benötigt zwingend eine AudioSpecificConfig (ASC)

    Ohne ASC weißt du NICHT:

        AudioObjectType (LC/HE/HEv2/ELD…)

        Sample Rate

        Channel Config

        SBR/PS Flags

Fall B: Manche Encoder packen ASC in mp4a statt ESDS

Sehr selten, aber möglich bei kaputten Encodern.

Dann findest du ASC in:

    mp4a atom → esds → 0x05
    ODER

    mp4a atom → dac3 (AC‑3)
    ODER

    mp4a atom → alac atom (ALAC)

Für AAC muss es 0x05 geben.
Fall C: Du findest nur 0x03 (ES_Descriptor) und sonst nichts

→ Das ist kaputt oder kein AAC.
🧩 9. Vollständige, lesbare C++‑Struktur‑Darstellung aller Descriptoren
cpp

struct Mp4Esds {
    uint8_t version;
    uint8_t flags[3];

    // ES_Descriptor (0x03)
    uint16_t es_id;
    uint8_t flags03;

    bool streamDependenceFlag;
    bool urlFlag;
    bool ocrFlag;

    uint16_t dependsOnEsId;   // optional
    uint8_t  urlLength;       // optional
    const uint8_t* urlString; // optional
    uint16_t ocrEsId;         // optional

    // DecoderConfigDescriptor (0x04)
    uint8_t objectTypeIndication;
    uint8_t streamType;
    uint32_t bufferSizeDB;
    uint32_t maxBitrate;
    uint32_t avgBitrate;

    // DecoderSpecificInfo (0x05)
    const uint8_t* asc;
    uint32_t asc_size;

    // SLConfigDescriptor (0x06)
    uint8_t sl_predefined;
};

🧠 10. Fazit für dich

    ASC (0x05) ist Pflicht für AAC.

    Wenn du kein 0x05 findest → File ist ungültig oder kein AAC.

    Du kannst ESDS komplett als C++‑Structs modellieren (oben).

    Dein FileReader reicht völlig — du brauchst keinen Bitreader für ESDS.

Wenn du willst, mache ich dir jetzt:

    kompletten ESDS‑Parser, der alle Structs oben füllt

    kompletten ASC‑Parser ebenfalls als Struct‑Memorylayout, NICHT Bitreader

    Integration in deinen MP4‑Demuxer (minimp4)

Sag einfach:
Vollen ESDS Parser oder ASC als Struct

C++ structs and functions for a practical MP4 esds parser (AAC‑oriented)

Below is a self‑contained, no‑abstractions esds parser that handles the usual MP4 case:

    esds box

    ES_Descriptor (tag 0x03)

    DecoderConfigDescriptor (tag 0x04)

    DecoderSpecificInfo (tag 0x05) → AAC AudioSpecificConfig

Descriptor tags
cpp

enum {
    ESDS_TAG_ES_DESCRIPTOR            = 0x03,
    ESDS_TAG_DECODER_CONFIG           = 0x04,
    ESDS_TAG_DECODER_SPECIFIC_INFO    = 0x05,
    ESDS_TAG_SL_CONFIG                = 0x06
};

Helper: read ISO/IEC 14496‑1 “sizeOfInstance” (7‑bit continuation)
cpp

static uint32_t esds_read_size(FileReader& f)
{
    uint32_t size = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t b = f.read_u8();
        size = (size << 7) | (b & 0x7F);
        if ((b & 0x80) == 0)
            break;
    }
    return size;
}

Data structs
cpp

struct ESDS_DecoderSpecificInfo {
    const uint8_t* data;
    uint32_t       size;
};

struct ESDS_DecoderConfig {
    uint8_t  objectTypeIndication;
    uint8_t  streamType;          // high 6 bits
    uint8_t  upStream;            // bit
    uint32_t bufferSizeDB;
    uint32_t maxBitrate;
    uint32_t avgBitrate;

    ESDS_DecoderSpecificInfo decSpecific;
};

struct ESDS_ESDescriptor {
    uint16_t esID;
    uint8_t  streamDependenceFlag;
    uint8_t  urlFlag;
    uint8_t  ocrStreamFlag;
    uint8_t  streamPriority;

    uint16_t dependsOnESID;   // if streamDependenceFlag
    uint8_t  urlLength;       // if urlFlag
    // urlString skipped
    uint16_t ocrESID;         // if ocrStreamFlag

    ESDS_DecoderConfig decConfig;
};

struct ESDS {
    uint8_t version;
    uint32_t flags;

    ESDS_ESDescriptor esd;
};

Parse DecoderSpecificInfo (tag 0x05)

We don’t interpret AAC AudioSpecificConfig here—just capture the blob.
cpp

static bool esds_parse_decoder_specific_info(FileReader& f,
                                             uint32_t size,
                                             ESDS_DecoderSpecificInfo& out)
{
    uint64_t start = f.tell();
    if (size == 0)
        return true;

    out.size = size;
    // You probably want to copy this into your own buffer.
    // Here we assume you have a way to get a pointer or you replace this with a copy.
    // For now, we just read into a temp buffer and store pointer as nullptr.
    // Adapt to your own memory model.

    // Example: skip but record size
    f.seek(start + size);
    out.data = nullptr;

    return true;
}

Parse DecoderConfigDescriptor (tag 0x04)
cpp

static bool esds_parse_decoder_config(FileReader& f,
                                      uint32_t size,
                                      ESDS_DecoderConfig& out)
{
    uint64_t start = f.tell();
    uint64_t end   = start + size;

    if (size < 13) // minimal size
        return false;

    out.objectTypeIndication = f.read_u8();

    uint8_t tmp = f.read_u8();
    out.streamType = (tmp >> 2) & 0x3F;
    out.upStream   = (tmp >> 1) & 0x01;
    // reserved bit (LSB) ignored

    out.bufferSizeDB = f.read_u24();
    out.maxBitrate   = f.read_u32();
    out.avgBitrate   = f.read_u32();

    out.decSpecific.data = nullptr;
    out.decSpecific.size = 0;

    while (f.tell() < end) {
        uint8_t tag = f.read_u8();
        uint32_t dsize = esds_read_size(f);
        uint64_t dstart = f.tell();
        uint64_t dend   = dstart + dsize;

        if (dend > end)
            return false;

        if (tag == ESDS_TAG_DECODER_SPECIFIC_INFO) {
            if (!esds_parse_decoder_specific_info(f, dsize, out.decSpecific))
                return false;
        } else {
            // skip unknown sub-descriptor
            f.seek(dend);
        }
    }

    f.seek(end);
    return true;
}

Parse ES_Descriptor (tag 0x03)
cpp

static bool esds_parse_es_descriptor(FileReader& f,
                                     uint32_t size,
                                     ESDS_ESDescriptor& out)
{
    uint64_t start = f.tell();
    uint64_t end   = start + size;

    if (size < 3)
        return false;

    out.esID = f.read_u16();

    uint8_t flags = f.read_u8();
    out.streamDependenceFlag = (flags >> 7) & 0x01;
    out.urlFlag              = (flags >> 6) & 0x01;
    out.ocrStreamFlag        = (flags >> 5) & 0x01;
    out.streamPriority       =  flags & 0x1F;

    if (out.streamDependenceFlag) {
        if (f.tell() + 2 > end) return false;
        out.dependsOnESID = f.read_u16();
    } else {
        out.dependsOnESID = 0;
    }

    if (out.urlFlag) {
        if (f.tell() + 1 > end) return false;
        out.urlLength = f.read_u8();
        if (f.tell() + out.urlLength > end) return false;
        // skip URL string
        f.seek(f.tell() + out.urlLength);
    } else {
        out.urlLength = 0;
    }

    if (out.ocrStreamFlag) {
        if (f.tell() + 2 > end) return false;
        out.ocrESID = f.read_u16();
    } else {
        out.ocrESID = 0;
    }

    // Now parse nested descriptors (DecoderConfig, SLConfig, etc.)
    while (f.tell() < end) {
        uint8_t tag = f.read_u8();
        uint32_t dsize = esds_read_size(f);
        uint64_t dstart = f.tell();
        uint64_t dend   = dstart + dsize;

        if (dend > end)
            return false;

        if (tag == ESDS_TAG_DECODER_CONFIG) {
            if (!esds_parse_decoder_config(f, dsize, out.decConfig))
                return false;
        } else {
            // skip unknown or SLConfig etc.
            f.seek(dend);
        }
    }

    f.seek(end);
    return true;
}

Top‑level esds box parser

Call this when you’ve already read the esds atom header (size + type) and know esds_end_offset.

bool parse_esds(FileReader& f, uint64_t esds_end_offset, ESDS& out)
{
    uint64_t start = f.tell();

    if (start + 4 > esds_end_offset)
        return false;

    out.version = f.read_u8();
    out.flags   = f.read_u24();

    while (f.tell() < esds_end_offset)
    {
        uint8_t tag = f.read_u8();
        uint32_t size = esds_read_size(f);
        uint64_t dstart = f.tell();
        uint64_t dend   = dstart + size;

        if (dend > esds_end_offset)
            return false;

        if (tag == ESDS_TAG_ES_DESCRIPTOR) {
            if (!esds_parse_es_descriptor(f, size, out.esd))
                return false;
        } else {
            // skip unknown top-level descriptor
            f.seek(dend);
        }
    }

    f.seek(esds_end_offset);
    return true;
}

If you want, next step I can give you a tiny AAC AudioSpecificConfig parser that consumes DecoderSpecificInfo and gives you sampleRate, channels, and profile directly.
*/




































//🧩 BAD/TRASH
/*
struct ESDS_Parser
{
    int64_t tag_offset;     // Position des Tag-Bytes
    int64_t payload_offset; // Anfang der Payload
    uint32_t payload_size;  // Größe der Payload
    uint8_t tag;            // 0x03, 0x04, 0x05, 0x06

    typedef std::function<void(const ESDS_Parser&)> FN_onDescriptor;

    static void
    parse_recursive(File& file, const int64_t beg, const int64_t end,
                    const FN_onDescriptor& onDescriptor)
    {
        int64_t cur = beg;
        while (cur < end)
        {
            file.seek(cur);

            uint8_t tag = 0;
            cur += file.read_u8(&tag);

            uint32_t len;
            cur += ESDS_Util::read_vlen(file, &len);

            DE_ERROR("tag(",int(tag),"), len(",len,"), cur(",cur,")")

            int64_t payload_beg = cur;
            int64_t payload_end = cur + len;

            if (payload_end > end) return;

            ESDS_Parser desc;
            //desc.tag_offset     = tag_off;
            desc.payload_offset = payload_beg;
            desc.payload_size   = len;
            desc.tag            = tag;

            // Callback feuern
            onDescriptor(desc);

            // Wenn 0x03 oder 0x04 → verschachtelte Descriptoren drin
            if (tag == 0x03 || tag == 0x04)
            {
                // ES_Descriptor oder DecoderConfigDescriptor
                // → enthält weitere Descriptoren
                parse_recursive(file, payload_beg, payload_end, onDescriptor);
            }

            // Weiter zum nächsten Descriptor
            cur = payload_end;
        }
    }

    static void
    parse_esds(File& file, int64_t esds_offset, int64_t esds_size,
                    const FN_onDescriptor& onDescriptor)
    {
        int64_t payload_start = esds_offset + 4; // skip version+flags
        int64_t payload_end   = esds_offset + esds_size;

        parse_recursive(file, payload_start, payload_end, onDescriptor);
    }

    parse_esds(file, esds_offset, esds_size,
        [&](const EsdsState& st)
        {
            if (st.tag == 0x05)
            {
                printf("ASC found at 0x%llX, size=%u\n",
                       (unsigned long long)st.payload_offset,
                       st.payload_size);

                std::vector<uint8_t> asc(st.payload_size);
                file.seek(st.payload_offset);
                file.read(asc.data(), st.payload_size);

                // asc enthält jetzt AudioSpecificConfig
            }
        }
    );
};
*/
