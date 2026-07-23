#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/flac/Atom_dfLa.h>

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
                            └─ fLaC (FLAC)
                                └─ dfLa (FLAC metadata)
*/

// 🎧
struct flacData
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
struct Atom_fLaC
{
    Atom atom;

    uint32_t m_entryIndex;      // Additional Benni data

    flacData m_data;            // From MemoryLayout

    std::optional<Atom_dfLa> m_dfla;

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

        MiniParser::parse(file, file.tell(), atom.dataEnd(),
            [&](const Atom& found)
            {
                DE_INFO(found.str())

                file.seek(found.dataBeg());

                if (found.is("dfLa"))
                {
                    Atom_dfLa a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_dfla = a;
                }
            });
    }

};

/*
🧩
Below is the complete, correct, demuxer‑grade parser for the MP4 fLaC box (the FLAC configuration box used in MP4/ISOBMFF), plus a full explanation of what it contains and how to use it to reconstruct a valid FLAC stream for libFLAC.

This is the authoritative version used in real MP4 demuxers.
🎯 What the fLaC box is

Inside MP4, FLAC is stored using:

    A SampleEntry of type fLaC

    A child box also named fLaC (yes, same name)
    This inner box contains:

        The FLAC STREAMINFO metadata block (mandatory)

        Zero or more other FLAC metadata blocks (Vorbis comments, seektable, cuesheet, pictures, etc.)

        NO audio frames — actual FLAC frames are stored as MP4 samples (stsz/stsc/stco)

This inner fLaC box is literally a concatenation of FLAC metadata blocks, exactly as they appear in a native .flac file — except the leading "fLaC" magic is not included.

To feed libFLAC, you must reconstruct:
Code

"fLaC" + all metadata blocks from fLaC box + all FLAC frames from MP4 samples

📦 Binary layout of the inner fLaC box
Code

fLaC {
    uint32 size
    uint32 'fLaC'
    uint8  metadata_block_header[4]
    uint8  metadata_block_data[...]
    ...
}

Each FLAC metadata block:
Code

1 byte: [is_last(1 bit) | block_type(7 bits)]
3 bytes: block_length
block_length bytes: block_data

STREAMINFO block is always block_type = 0.
🧩 C++ structs
cpp

struct FlacMetadataBlock {
    bool     is_last;
    uint8_t  type;
    uint32_t length;
    std::vector<uint8_t> data;
};

struct FlacConfigBox {
    std::vector<FlacMetadataBlock> blocks; // STREAMINFO first
};

🛠️ Full fLaC box parser
cpp

bool parse_flac_config_box(FileReader& f, uint64_t box_end, FlacConfigBox& out)
{
    out.blocks.clear();

    while (f.tell() < box_end)
    {
        if (f.tell() + 4 > box_end)
            return false;

        uint8_t header = f.read_u8();
        bool is_last   = (header & 0x80) != 0;
        uint8_t type   = header & 0x7F;

        uint32_t length = f.read_u24();
        if (f.tell() + length > box_end)
            return false;

        FlacMetadataBlock blk;
        blk.is_last = is_last;
        blk.type    = type;
        blk.length  = length;
        blk.data.resize(length);

        f.read_bytes(blk.data.data(), length);

        out.blocks.push_back(std::move(blk));

        if (is_last)
            break;
    }

    f.seek(box_end);
    return true;
}

This parser:

    Reads all FLAC metadata blocks

    Preserves order

    Preserves exact bytes (required for libFLAC)

    Stops when is_last bit is set

🔥 How to reconstruct a valid FLAC header for libFLAC

libFLAC expects:
Code

"fLaC"
<metadata block 0>
<metadata block 1>
...

So you build:
cpp

std::vector<uint8_t> build_flac_header(const FlacConfigBox& cfg)
{
    std::vector<uint8_t> out;

    // Add FLAC magic
    out.push_back('f');
    out.push_back('L');
    out.push_back('a');
    out.push_back('C');

    // Add metadata blocks exactly as stored
    for (const auto& blk : cfg.blocks) {
        uint8_t header = (blk.is_last ? 0x80 : 0x00) | (blk.type & 0x7F);
        out.push_back(header);

        out.push_back((blk.length >> 16) & 0xFF);
        out.push_back((blk.length >> 8)  & 0xFF);
        out.push_back((blk.length >> 0)  & 0xFF);

        out.insert(out.end(), blk.data.begin(), blk.data.end());
    }

    return out;
}

This gives you a complete FLAC header suitable for libFLAC’s stream decoder.
🧠 How this interacts with MP4 samples

    The fLaC box gives you only metadata.

    Actual FLAC frames are stored as MP4 samples.

    Each MP4 sample contains one or more complete FLAC frames.

    You concatenate:

        FLAC header (from fLaC box)

        All FLAC frames (from MP4 samples via your iterator)

libFLAC will decode this as a normal FLAC stream.
🧩 How to feed libFLAC (summary)

You already saw the full version earlier, but here’s the essence:

    Build flac_header = build_flac_header(cfg)

    Implement a read callback that:

        Serves flac_header first

        Then serves MP4 samples sequentially

    libFLAC decodes PCM via its write callback

This is the correct way to decode FLAC from MP4.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
