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

struct Atom_stts
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
Below is the full, correct, demuxer‑grade parser for the MP4 stts (Decoding Time‑to‑Sample) atom — plus the real explanation of how it works and how to use it in a decoder.

This is one of the core timing tables in MP4.
If you get stts wrong, timestamps will be wrong.
🎯 What stts actually means

stts maps:
Code

sample index → decode timestamp (DTS)

It is a run‑length table of:
Code

(count, delta)

Meaning:
Code

The next <count> samples each have a duration of <delta> time units.

Time units = track timescale (from mdhd).
📦 Binary layout
Code

stts {
    uint32 size
    uint32 'stts'
    uint8  version
    uint24 flags
    uint32 entry_count

    for i in 0..entry_count-1:
        uint32 sample_count
        uint32 sample_delta
}

🧩 C++ structs
cpp

struct STTS_Entry {
    uint32_t sample_count;   // number of consecutive samples
    uint32_t sample_delta;   // duration of each sample (in timescale units)
};

struct STTS {
    uint8_t  version;
    uint32_t flags;
    std::vector<STTS_Entry> entries;
};

🛠️ Full stts parser
cpp

bool parse_stts(FileReader& f, uint64_t stts_end, STTS& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    uint32_t entry_count = f.read_u32();
    out.entries.clear();
    out.entries.reserve(entry_count);

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (f.tell() + 8 > stts_end)
            return false;

        STTS_Entry e;
        e.sample_count = f.read_u32();
        e.sample_delta = f.read_u32();

        out.entries.push_back(e);
    }

    f.seek(stts_end);
    return true;
}

🔥 How to use stts at runtime

You need to compute DTS for sample n (0‑based).
Algorithm
cpp

uint64_t stts_compute_dts(const STTS& stts, uint32_t sample_index)
{
    uint64_t dts = 0;
    uint32_t remaining = sample_index;

    for (const auto& e : stts.entries)
    {
        if (remaining < e.sample_count) {
            // inside this run
            dts += uint64_t(remaining) * e.sample_delta;
            return dts;
        }

        // skip whole run
        dts += uint64_t(e.sample_count) * e.sample_delta;
        remaining -= e.sample_count;
    }

    // sample_index out of range
    return dts;
}

This is exactly how FFmpeg, Bento4, and QuickTime do it.
🧠 Example

Given:
Code

entry 0: sample_count=100, sample_delta=1024
entry 1: sample_count=200, sample_delta=960

Then:

    sample 0 → DTS = 0

    sample 1 → 1024

    sample 99 → 99 * 1024

    sample 100 → 100 * 1024

    sample 101 → 100*1024 + 960

    sample 299 → 1001024 + 199960

This is how variable‑frame‑rate audio/video is represented.
🧩 How stts interacts with ctts

    stts gives DTS (decode timestamp)

    ctts gives CTS offset (composition offset)

If ctts exists:
Code

CTS = DTS + ctts_offset

If not:
Code

CTS = DTS

🧩 How stts interacts with mdhd

mdhd.timescale tells you how to convert timestamps to seconds:
Code

seconds = timestamp / timescale

Example:
Code

timescale = 48000
delta = 1024
duration = 1024 / 48000 = 21.333 ms


*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
