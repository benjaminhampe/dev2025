#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 MP4
    │
    └─ moov  (Movie Box — ALL metadata)
        └─ mvhd (Movie Header)
            ├─ creation_time
            ├─ modification_time
            ├─ timescale
            ├─ duration
            ├─ rate
            ├─ volume
            └─ matrix (video transform)
*/

struct mvhdHeader
{
    uint8_t version;
    uint32_t flags;
};

struct mvhdVersion0
{
    uint32_t creation_time;
    uint32_t modification_time;
    uint32_t timescale; // ticks per second, 1000 = 1/ms
    uint32_t duration; // duration_seconds = duration / timescale, duration_seconds = 123456 / 1000 = 123.456 seconds
};

struct mvhdVersion1
{
    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t timescale; // ticks per second, 1000 = 1/ms
    uint64_t duration; // duration_seconds = duration / timescale, duration_seconds = 123456 / 1000 = 123.456 seconds
};

struct mvhdCommon
{
    int32_t rate; // 16.16 fixed‑point -> double rate_d = double(rate) / 65536.0;
    int16_t volume; // 8.8 fixed‑point -> double volume_d = double(volume) / 256.0;
    uint16_t reserved1;
    uint32_t reserved2[2];

    int32_t matrix[9]; // 16.16 fixed‑point -> double m = double(matrix[i]) / 65536.0;

    uint32_t pre_defined[6];
    uint32_t next_track_ID;
};

// (Movie Header)
struct Atom_mvhd
{
    Atom atom;

    mvhdHeader header;
    std::optional<mvhdVersion0> version0;
    std::optional<mvhdVersion1> version1;
    mvhdCommon common;

    double rate() const { return double(common.rate) / 65536.0; }
    double volume() const { return double(common.volume) / 256.0; }

    double duration() const // in seconds
    {
        if (version1)
        {
            return double(version1->duration) / double(version1->timescale);
        }
        else if (version0)
        {
            return double(version0->duration) / double(version0->timescale);
        }
        else
        {
            DE_ERROR("No duration or timescale")
            return 0.0;
        }
    }
    uint32_t timescale() const
    {
        if (version1)
        {
            return version1->timescale;
        }
        else if (version0)
        {
            return version0->timescale;
        }
        else
        {
            DE_ERROR("No timescale")
            return 0;
        }
    }

    void parse(File & file)
    {
        file.seek(atom.dataBeg());

        // struct mvhdHeader
        file.read_u8(&header.version);
        file.read_u24_be(&header.flags);

        // struct mvhdVersion0
        // uint32   creation_time
        // uint32   modification_time
        // uint32   timescale
        // uint32   duration
        if (header.version == 0)
        {
            mvhdVersion0 v0;
            file.read_u32_be(&v0.creation_time);
            file.read_u32_be(&v0.modification_time);
            file.read_u32_be(&v0.timescale);
            file.read_u32_be(&v0.duration);
            version0 = v0;
        }
        // struct mvhdVersion1
        // uint64   creation_time
        // uint64   modification_time
        // uint32   timescale
        // uint64   duration
        else if (header.version == 1)
        {
            mvhdVersion1 v1;
            file.read_u64_be(&v1.creation_time);
            file.read_u64_be(&v1.modification_time);
            file.read_u32_be(&v1.timescale);
            file.read_u64_be(&v1.duration);
            version1 = v1;
        }
        else
        {
            DE_ERROR("unsupported version ",header.version)
            return;
        }

        // struct mvhdCommon
        // int32    rate
        // int16    volume
        // uint16   reserved
        // uint32   reserved[2]
        // int32    matrix[9]
        // uint32   pre_defined[6]
        // uint32   next_track_ID
        file.read_s32_be(&common.rate);
        file.read_s16_be(&common.volume);
        file.read_u16_be(&common.reserved1);
        file.read_u32_be(&common.reserved2[0]);
        file.read_u32_be(&common.reserved2[1]);
        for (int i = 0; i < 9; ++i) { file.read_s32_be(&common.matrix[i]); }
        for (int i = 0; i < 6; ++i) { file.read_u32_be(&common.pre_defined[i]); }
        file.read_u32_be(&common.next_track_ID);

        // store in your structure
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        // "version(" << int(header.version) << "), "
        "duration(" << duration() << "), "
        "rate(" << rate() << "), "
        "volume(" << volume() << ")";
        return o.str();
    }
};

/*
🌳 Binary layout (version‑dependent)

✔ Version 0 layout

    uint8    version
    uint24   flags

    uint32   creation_time
    uint32   modification_time
    uint32   timescale
    uint32   duration

    int32    rate
    int16    volume
    uint16   reserved
    uint32   reserved[2]

    int32    matrix[9]

    uint32   pre_defined[6]
    uint32   next_track_ID

✔ Version 1 layout (64‑bit times)

    uint8    version
    uint24   flags

    uint64   creation_time
    uint64   modification_time
    uint32   timescale
    uint64   duration

    int32    rate
    int16    volume
    uint16   reserved
    uint32   reserved[2]

    int32    matrix[9]

    uint32   pre_defined[6]
    uint32   next_track_ID

🌳 C++11 parsing implementation
cpp

void parse_mvhd(const Atom& atom, std::istream& file)
{
    file.seekg(atom.m_fileOffset + atom.m_headerSize);

    uint8_t  version = read_u8(file);
    uint32_t flags   = read_u24_be(file); // always 0 for mvhd

    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t timescale;
    uint64_t duration;

    if (version == 0)
    {
        creation_time     = read_u32_be(file);
        modification_time = read_u32_be(file);
        timescale         = read_u32_be(file);
        duration          = read_u32_be(file);
    }
    else if (version == 1)
    {
        creation_time     = read_u64_be(file);
        modification_time = read_u64_be(file);
        timescale         = read_u32_be(file);
        duration          = read_u64_be(file);
    }
    else
    {
        // unsupported mvhd version
        return;
    }

    int32_t rate   = read_s32_be(file);   // 16.16 fixed point
    int16_t volume = read_s16_be(file);   // 8.8 fixed point

    read_u16_be(file); // reserved

    read_u32_be(file); // reserved[0]
    read_u32_be(file); // reserved[1]

    int32_t matrix[9];
    for (int i = 0; i < 9; ++i)
        matrix[i] = read_s32_be(file);

    // pre_defined[6]
    for (int i = 0; i < 6; ++i)
        read_u32_be(file);

    uint32_t next_track_ID = read_u32_be(file);

    // store these in your structure as needed
}

🌳 Important details
✔ rate is 16.16 fixed‑point
double rate_d = double(rate) / 65536.0;
rate = 0x00010000
rate_d = 1.0

Default is 0x00010000 = 1.0 (normal speed).
✔ volume is 8.8 fixed‑point
double volume_d = double(volume) / 256.0;
volume = 0x0100
volume_d = 1.0

Default is 0x0100 = 1.0 (full volume).
✔ matrix is always 9 × 32‑bit signed integers

Default identity matrix:
Code

0x00010000 0x00000000 0x00000000
0x00000000 0x00010000 0x00000000
0x00000000 0x00000000 0x40000000

✔ next_track_ID

This is the ID to assign to the next new track if the file is edited.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
