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
*/

struct Atom_ctts
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
Below is the full, correct, demuxer‑grade parser for the MP4 ctts (Composition Time‑to‑Sample) atom — plus the real explanation of how it works and how to use it in a decoder.

This is the second half of MP4 timing (together with stts).
If you get ctts wrong, video frames will be out of order and audio may drift.
🎯 What ctts actually means

ctts maps:
Code

sample index → composition offset (CTS offset)

It is a run‑length table of:
Code

(count, offset)

Meaning:
Code

The next <count> samples each have a CTS offset of <offset> time units.

Time units = track timescale (from mdhd).
🧠 Why ctts exists

Because decode order ≠ presentation order.

Examples:

    H.264 B‑frames

    HEVC B‑frames

    AV1 reordering

    Some audio codecs with lookahead

stts gives DTS.
ctts gives CTS offset.

Final timestamp:
Code

CTS = DTS + CTS_offset

📦 Binary layout

There are two versions:
Version 0 (most common)

Offsets are unsigned.
Code

ctts {
    uint32 size
    uint32 'ctts'
    uint8  version = 0
    uint24 flags
    uint32 entry_count

    for i in 0..entry_count-1:
        uint32 sample_count
        uint32 sample_offset   // unsigned
}

Version 1 (rare)

Offsets are signed (can be negative).
Code

ctts {
    uint8  version = 1
    ...
    uint32 sample_offset   // signed int32
}

🧩 C++ structs
cpp

struct CTTS_Entry {
    uint32_t sample_count;
    int32_t  sample_offset; // signed for version 1, cast for version 0
};

struct CTTS {
    uint8_t  version;
    uint32_t flags;
    std::vector<CTTS_Entry> entries;
};

🛠️ Full ctts parser
cpp

bool parse_ctts(FileReader& f, uint64_t ctts_end, CTTS& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    uint32_t entry_count = f.read_u32();
    out.entries.clear();
    out.entries.reserve(entry_count);

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (f.tell() + 8 > ctts_end)
            return false;

        CTTS_Entry e;
        e.sample_count = f.read_u32();

        if (out.version == 0) {
            // unsigned offset
            e.sample_offset = (int32_t)f.read_u32();
        } else {
            // version 1: signed offset
            e.sample_offset = (int32_t)f.read_u32();
        }

        out.entries.push_back(e);
    }

    f.seek(ctts_end);
    return true;
}

🔥 How to use ctts at runtime

You need to compute CTS offset for sample n (0‑based).
Algorithm
cpp

int64_t ctts_compute_offset(const CTTS& ctts, uint32_t sample_index)
{
    uint32_t remaining = sample_index;

    for (const auto& e : ctts.entries)
    {
        if (remaining < e.sample_count) {
            return e.sample_offset;
        }

        remaining -= e.sample_count;
    }

    return 0; // no offset (should not happen if sample_index is valid)
}

🎬 Final timestamp calculation

Given:

    DTS = stts_compute_dts(n)

    CTS_offset = ctts_compute_offset(n)

Then:
Code

CTS = DTS + CTS_offset

Convert to seconds:
Code

seconds = CTS / mdhd.timescale

🧠 Example

Given:
Code

entry 0: sample_count=3, sample_offset=0
entry 1: sample_count=2, sample_offset=1024
entry 2: sample_count=5, sample_offset=2048

Then:

    sample 0 → offset = 0

    sample 2 → offset = 0

    sample 3 → offset = 1024

    sample 4 → offset = 1024

    sample 5 → offset = 2048

    sample 9 → offset = 2048

This is typical for B‑frames.
🧩 How ctts interacts with stts

    stts gives decode order

    ctts gives presentation order

If ctts is missing:
Code

CTS = DTS

If ctts exists:
Code

CTS = DTS + offset

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
