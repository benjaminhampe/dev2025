#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

MP4-FILE
    │
    ├─ moov  (Movie Box — ALL metadata)
    │   │
    │   ├─ trak (Track Box — one per stream)
    │   │   ├─ tkhd (Track Header)
    │   │   │   ├─ track_id
    │   │   │   ├─ duration
    │   │   │   ├─ layer
    │   │   │   ├─ alternate_group
    │   │   │   ├─ volume (audio)
    │   │   │   └─ matrix (video)
    │   │   │
    │   │   ├─ tref (Track References)
    │   │   │   ├─ dpnd (depends-on)
    │   │   │   ├─ ipir (IPMP)
    │   │   │   ├─ mpod (OD)
    │   │   │   └─ tmcd (timecode)
    │   │   │
    │   │   ├─ edts (Edit Box)
    │   │   │   └─ elst (Edit List)
    │   │   │       ├─ segment_duration
    │   │   │       ├─ media_time
    │   │   │       └─ media_rate
    │   │   │       → used for trimming, delay, gaps
    │   │   │
    │   │   └─ mdia (Media Box)
    │   │       ├─ mdhd (Media Header)
    │   │       │   ├─ timescale
    │   │       │   └─ duration
    │   │       │
    │   │       ├─ hdlr (Handler)
    │   │       │   └─ handler_type = "vide" / "soun" / "text"
    │   │       │      → tells you if this is audio or video
    │   │       │
    │   │       └─ minf (Media Information)    
*/

struct tkhdHeader
{
    uint8_t version;
    uint32_t flags;
};

struct tkhdVersion0 // (32‑bit)
{
    uint32_t  creation_time;
    uint32_t  modification_time;
    uint32_t  track_ID;
    uint32_t  reserved1;
    uint32_t  duration;
};

struct tkhdVersion1 // (64‑bit)
{
    uint64_t  creation_time;
    uint64_t  modification_time;
    uint32_t  track_ID;
    uint32_t  reserved1;
    uint64_t  duration;
};

struct tkhdCommon
{
    uint32_t  reserved2[2];

    int16_t   layer;
    int16_t   alternate_group;
    int16_t   volume;
    uint16_t  reserved3;

    int32_t   matrix[9];

    uint32_t  width;
    uint32_t  height;
};

// (Track Header)
struct Atom_tkhd
{
    Atom atom;

    tkhdHeader header;
    std::optional<tkhdVersion0> version0;
    std::optional<tkhdVersion1> version1;
    tkhdCommon common;

    // ✔ Width/height are 16.16 fixed‑point
    double width() const { return double(common.width) / 65536.0; }
    double height() const { return double(common.height) / 65536.0; }

    void parse(File & file)
    {
        file.seek(atom.dataBeg());

        // struct tkhdHeader
        file.read_u8(&header.version);
        file.read_u24_be(&header.flags);

        // struct tkhdVersion0
        if (header.version == 0)
        {
            tkhdVersion0 v0;
            file.read_u32_be(&v0.creation_time);
            file.read_u32_be(&v0.modification_time);
            file.read_u32_be(&v0.track_ID);
            file.read_u32_be(&v0.reserved1);
            file.read_u32_be(&v0.duration);
            version0 = v0;
        }
        // struct tkhdVersion1
        else if (header.version == 1)
        {
            tkhdVersion1 v1;
            file.read_u64_be(&v1.creation_time);
            file.read_u64_be(&v1.modification_time);
            file.read_u32_be(&v1.track_ID);
            file.read_u32_be(&v1.reserved1);
            file.read_u64_be(&v1.duration);
            version1 = v1;
        }
        else
        {
            DE_ERROR("unsupported version ",header.version)
            return;
        }

        // struct tkhdCommon
        file.read_u32_be(&common.reserved2[0]);
        file.read_u32_be(&common.reserved2[1]);
        file.read_s16_be(&common.layer);
        file.read_s16_be(&common.alternate_group);
        file.read_s16_be(&common.volume);
        file.read_u16_be(&common.reserved3);
        for (int i = 0; i < 9; ++i)
        {
            file.read_s32_be(&common.matrix[i]);
        }

        // ✔ Width/height are 16.16 fixed‑point
        // 0x00048000 = 4.5
        // double w = width  / 65536.0;
        // double h = height / 65536.0;
        file.read_u32_be(&common.width);
        file.read_u32_be(&common.height);

        // store in your structure
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << "\n"
        "width(" << width() << ")\n"
        "height(" << height() << ")";
        return o.str();
    }
};

/*

🌳 Meaning of each flag
1. track_enabled (0x000001)
    If this bit is not set, the track is disabled.
    Disabled tracks:
        are not played
        are not shown
        may be metadata or alternate tracks

2. track_in_movie (0x000002)
    Indicates the track is intended to be used during normal playback.
    If unset, the track might be:
        only for preview
        only for editing
        only for thumbnails

3. track_in_preview (0x000004)
    Indicates the track is intended to be used during preview mode.
    Preview = short summary playback (e.g., editing software).

✔ Version 0 layout

uint8   version
uint24  flags

uint32  creation_time
uint32  modification_time
uint32  track_ID
uint32  reserved
uint32  duration

uint32  reserved[2]

int16   layer
int16   alternate_group
int16   volume
uint16  reserved

int32   matrix[9]

uint32  width
uint32  height

✔ Version 1 layout (64‑bit times)

uint8   version
uint24  flags

uint64  creation_time
uint64  modification_time
uint32  track_ID
uint32  reserved
uint64  duration

uint32  reserved[2]

int16   layer
int16   alternate_group
int16   volume
uint16  reserved

int32   matrix[9]

uint32  width
uint32  height

🌳 Parsing logic (C++11‑style)
cpp

void parse_tkhd(const Atom& atom, std::ifstream& file)
{
    file.seekg(atom.m_fileOffset + atom.m_headerSize);

    uint8_t version = read_u8(file);
    uint32_t flags  = read_u24_be(file);

    uint64_t creation_time;
    uint64_t modification_time;
    uint64_t duration;
    uint32_t track_ID;

    if (version == 0) {
        creation_time     = read_u32_be(file);
        modification_time = read_u32_be(file);
        track_ID          = read_u32_be(file);
        read_u32_be(file); // reserved
        duration          = read_u32_be(file);
    } else if (version == 1) {
        creation_time     = read_u64_be(file);
        modification_time = read_u64_be(file);
        track_ID          = read_u32_be(file);
        read_u32_be(file); // reserved
        duration          = read_u64_be(file);
    } else {
        // unsupported version
        return;
    }

    // reserved[2]
    read_u32_be(file);
    read_u32_be(file);

    int16_t layer           = read_s16_be(file);
    int16_t alternate_group = read_s16_be(file);
    int16_t volume          = read_s16_be(file);
    read_u16_be(file); // reserved

    int32_t matrix[9];
    for (int i = 0; i < 9; ++i)
        matrix[i] = read_s32_be(file);

    uint32_t width  = read_u32_be(file);
    uint32_t height = read_u32_be(file);

    // store in your structure
}

This is exactly what every correct MP4 demuxer does.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
