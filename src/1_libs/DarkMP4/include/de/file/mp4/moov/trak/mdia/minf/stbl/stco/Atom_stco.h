#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

moov
 └─ trak (audio track)
      └─ mdia
           ├─ hdlr
           ├─ minf
           │    └─ stbl
           │         ├─ stsd
           │         │    └─ (codec-specific box)
           │         ├─ stsc
           │         ├─ stsz / stz2
           │         ├─ stco / co64
           │         ├─ stts
           │         └─ ctts (rare for audio)
           └─ edts (optional)
mdat


stco {
    uint32 size
    uint32 'stco'
    uint8  version
    uint24 flags
    uint32 entry_count
    uint32 chunk_offset[entry_count]
}
*/

// 🧩 Atom Header
struct stcoHeader
{
    uint8_t version;
    uint8_t flags[3];
    uint32_t entry_count;
    // uint32 chunk_offset[entry_count];
};

// 🧩 Atom Header
typedef std::vector<uint64_t> MP4_ChunkOffsets; // file offsets of chunks

// 🧩 Atom Header
struct Atom_stco
{
    Atom atom;

    stcoHeader m_header;

    MP4_ChunkOffsets m_chunk_offsets;

    int version() const { return m_header.version; }

    u32 expected() const { return m_header.entry_count; }

    void parse(File & file)
    {
        file.seek(atom.dataBeg());
        file.read_u8(&m_header.version);
        file.read_u8(&m_header.flags[0]);
        file.read_u8(&m_header.flags[1]);
        file.read_u8(&m_header.flags[2]);
        file.read_u32_be(&m_header.entry_count);

        m_chunk_offsets.clear();
        m_chunk_offsets.reserve(m_header.entry_count);

        for (uint32_t i = 0; i < m_header.entry_count; ++i)
        {
            // if (f.tell() + 4 > stco_end)
            //     return false;

            uint32_t chunk_offset = 0;
            file.read_u32_be(&chunk_offset);

            m_chunk_offsets.push_back(chunk_offset);
        }
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ""
        ", version(" << version() << ")"
        ", count(" << m_chunk_offsets.size() << ")"
        ", expected(" << expected() << ")";
        return o.str();
    }
};

/*

// Chunk Offsets (32-bit)
bool
parse_stco(int fd, uint64_t payload_size, TrackTables &t)
{
    // stco = chunk offsets (32-bit)
    // version+flags
    read_be32(fd);

    uint32_t entry_count = read_be32(fd);
    payload_size -= 8;

    t.chunk_offsets.resize(entry_count);
    for (uint32_t i = 0; i < entry_count; ++i)
        t.chunk_offsets[i] = read_be32(fd);

    return true;
}

// Chunk Offsets (64-bit)
bool
parse_co64(int fd, uint64_t payload_size, TrackTables &t)
{
    // co64 = chunk offsets (64-bit)
    read_be32(fd); // version+flags

    uint32_t entry_count = read_be32(fd);
    payload_size -= 8;

    t.chunk_offsets.resize(entry_count);
    for (uint32_t i = 0; i < entry_count; ++i)
        t.chunk_offsets[i] = read_be64(fd);

    return true;
}


What stco is

stco = Chunk Offset Box
It gives you the file offset (32‑bit) of each chunk in a track.

Layout:
text

stco {
    uint32 size
    uint32 'stco'
    uint8  version
    uint24 flags
    uint32 entry_count
    uint32 chunk_offset[entry_count]
}

C++ structs
cpp

struct STCO {
    uint8_t  version;
    uint32_t flags;
    std::vector<uint32_t> offsets; // file offsets of chunks
};

Parser
cpp

bool parse_stco(FileReader& f, uint64_t stco_end, STCO& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    uint32_t entry_count = f.read_u32();
    out.offsets.clear();
    out.offsets.reserve(entry_count);

    for (uint32_t i = 0; i < entry_count; ++i) {
        if (f.tell() + 4 > stco_end)
            return false;

        uint32_t off = f.read_u32();
        out.offsets.push_back(off);
    }

    f.seek(stco_end);
    return true;
}

How you use it

    offsets.size() = number of chunks

    offsets[i] (0‑based) = file offset of chunk (i+1)

    Combine with stsc (samples per chunk) and stsz (sample sizes) to walk all samples in the track.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
