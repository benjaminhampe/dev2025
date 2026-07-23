#pragma once
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

🌳 Top‑Level MP4 Boxes (Atoms)

    MP4 File
    ├── ftyp      (Unique, Optional but recommended)
    ├── moov      (Unique, Required)
    ├── moof      (Multiple, Optional — only in fragmented MP4)
    ├── mdat      (Multiple, Optional — but required if media exists)
    ├── free      (Multiple, Optional)
    ├── skip      (Multiple, Optional)
    ├── meta      (Optional, Unique at top-level)
    ├── udta      (Optional, Unique at top-level)
    ├── sidx      (Multiple, Optional — indexing for streaming)
    ├── prft      (Multiple, Optional — producer reference time)
    ├── styp      (Optional, Unique — segment type box)
    ├── ssix      (Multiple, Optional — subsegment index)
    ├── emsg      (Multiple, Optional — event message)
    ├── mehd      (Optional, Unique — movie extends header)
    ├── mfra      (Optional, Unique — movie fragment random access)
    └── uuid      (Multiple, Optional — user-defined boxes)

🌳 Top‑Level MP4 Boxes — Correct Occurrence Rules

    Box   | Unique | Required | Notes
    ------+--------+----------+------------------------------
    ftyp  | Yes    | No*      | Strongly recommended
    moov  | Yes    | Yes      | Always required (frag or not)
    mdat  | No     | No       | Required if media exists
    moof  | No     | No       | Required for fragmented MP4
    mfra  | Yes    | No       | Optional (random access)
    free  | No     | No       | Padding
    skip  | No     | No       | Padding
    meta  | Yes    | No       | Metadata
    udta  | Yes    | No       | User data
    sidx  | No     | No       | Segment index
    styp  | Yes    | No       | Segment type (CMAF)
    ssix  | No     | No       | Subsegment index
    emsg  | No     | No       | Event messages
    prft  | No     | No       | Producer reference time
    uuid  | No     | No       | Vendor-specific

📌 Other MP4 boxes can also appear multiple times

    MP4 is a hierarchical box format. Many boxes are repeatable:
        mdat — multiple allowed
        moof — one per fragment
        traf — one per track fragment
        trak — one per track
        stsc — only one per track
        uuid — unlimited
        free — unlimited
        skip — unlimited

    Some boxes must be unique:
        moov — exactly one
        mvhd — exactly one
        [mdat] — unlimited
        [moof] — unlimited
        [trak] — one per track
    
📌 Why multiple mdat exist

    1. Fragmented MP4 (fMP4)

        Every fragment has:
            moof
            mdat

        This is used in:
            DASH
            HLS fMP4
            CMAF
            Low‑latency streaming

    2. Progressive download

        Some encoders interleave metadata and media:

        mdat
        moov
        mdat

🌳 MP4 (ISO Base Media File) — Tree Structure (ASCII)

    MP4 File
    ├── ftyp
    ├── moov
    │   ├── mvhd
    │   ├── trak
    │   │   ├── tkhd
    │   │   ├── mdia
    │   │   │   ├── mdhd
    │   │   │   ├── hdlr
    │   │   │   └── minf
    │   │   │       ├── vmhd / smhd
    │   │   │       ├── dinf
    │   │   │       └── stbl
    │   │   │           ├── stsd
    │   │   │           ├── stts
    │   │   │           ├── stsc
    │   │   │           ├── stsz
    │   │   │           ├── stco / co64
    │   │   │           └── stss (optional)
    │   └── trak (second track)
    │       └── ...
    ├── mdat   (media data block #1)
    ├── mdat   (media data block #2)
    └── mdat   (media data block #3)

🌳 Fragmented MP4 (fMP4) — Tree Structure

    MP4 File (Fragmented)
    ├── ftyp
    ├── moov
    │   ├── mvhd
    │   ├── trak
    │   └── ...
    ├── moof   (fragment #1)
    │   ├── mfhd
    │   └── traf
    │       ├── tfhd
    │       ├── tfdt
    │       └── trun
    ├── mdat   (fragment #1 media)
    ├── moof   (fragment #2)
    │   └── traf
    ├── mdat   (fragment #2 media)
    ├── moof   (fragment #3)
    │   └── traf
    └── mdat   (fragment #3 media)


🌳 Interleaved MP4 (progressive download)

    MP4 File (Interleaved)
    ├── ftyp
    ├── moov
    ├── free
    ├── mdat   (chunk 1)
    ├── free
    ├── mdat   (chunk 2)
    └── free

🌳 Non‑Fragmented MP4 (classic)

    MP4
    ├── ftyp
    ├── moov   <-- contains full sample tables (stbl)
    └── mdat   <-- media data (1 or more)

    moov is required  
    mdat is required if media exists  
    moof must NOT appear

🌳 Fragmented MP4 (fMP4)

    MP4
    ├── ftyp
    ├── moov   <-- required, but contains NO sample tables
    │   └── mvex   <-- signals fragmented mode
    ├── moof   <-- fragment #1 metadata
    ├── mdat   <-- fragment #1 media
    ├── moof   <-- fragment #2 metadata
    ├── mdat   <-- fragment #2 media
    └── mfra   <-- optional random access index

    moov is still required  
    moof + mdat pairs appear multiple times  
    mdat may appear multiple times  
    moov does NOT contain stbl sample tables  
    mvex must appear inside moov

📌 Why moov is always required because:

    It defines tracks (trak)
    It defines timescales (mvhd, mdhd)
    It defines codecs (stsd)
    It defines track IDs
    It defines default sample properties (trex in fragmented mode)

    Even in fragmented MP4, the player cannot interpret moof without the metadata in moov.

📌 When moov is different

    Non‑fragmented MP4 moov contains:

        trak
         └── mdia
              └── minf
                   └── stbl   <-- full sample tables (stts, stsc, stsz, stco)

    Fragmented MP4 moov contains:

        mvex
         └── trex   <-- default sample properties

    And no sample tables.

    🧷 YES, moov is required in all MP4 files.
    🧷 NO, fragmented MP4 does not omit moov.
    🧷 YES, fragmented MP4 moves sample tables into moof/traf.
    🧷 YES, multiple mdat and moof are allowed.


🌳 All MP4 Variants — Detection Summary Table

    Type                     | Required Boxes                     | Forbidden Boxes | Detection Rule
    -------------------------+------------------------------------+-----------------+----------------------------------------------
    Classic MP4              | ftyp, moov, mdat                   | moof            | moov has stbl sample tables; no moof present
    Fragmented MP4 (fMP4)    | ftyp, moov(mvex), moof, mdat       | none            | moov contains mvex; at least one moof present
    Progressive Download MP4 | ftyp, moov, mdat                   | none            | Same as classic MP4; difference is interleaving
    CMAF Segment             | styp, moof, mdat                   | none            | styp present; moof present; may lack moov
    DASH Segment             | sidx, moof, mdat                   | none            | sidx present; moof present
    HLS fMP4 Segment         | moof, mdat                         | none            | moof present; may lack ftyp/moov
    ISM Fragment (Smooth)    | uuid(MS), moof, mdat               | none            | Microsoft UUID boxes present
    QuickTime MOV            | ftyp(qt), moov, mdat               | none            | ftyp major brand = 'qt  '

🌳 ASCII‑Tree Examples for Each Type

    1. Classic MP4 (non‑fragmented)

        MP4
        ├── ftyp
        ├── moov
        │   └── trak
        │       └── mdia
        │           └── minf
        │               └── stbl   <-- full sample tables
        └── mdat

    Detection:

        moov contains stbl with stts/stsc/stsz/stco

        no moof boxes

    2. Fragmented MP4 (fMP4)

        MP4
        ├── ftyp
        ├── moov
        │   └── mvex   <-- signals fragmented mode
        ├── moof
        ├── mdat
        ├── moof
        └── mdat

    Detection:
        moov contains mvex
        at least one moof present

    3. Progressive Download MP4

        MP4
        ├── ftyp
        ├── moov
        ├── mdat
        ├── free
        ├── mdat
        └── free

    Detection:
        Same as classic MP4
        mdat may be interleaved
        No moof

    4. CMAF Segment

        Segment
        ├── styp   <-- required
        ├── moof
        └── mdat

    Detection:
        styp present
        moof present
        moov may be missing (segments don’t need it)

    5. DASH Segment

        Segment
        ├── sidx   <-- segment index
        ├── moof
        └── mdat

    Detection:
        sidx present
        moof present

    6. HLS fMP4 Segment

        Segment
        ├── moof
        └── mdat

    Detection:
        moof present
        ftyp/moov often missing (HLS doesn’t require them)

    7. Microsoft Smooth Streaming (ISM)

        Fragment
        ├── uuid (MS proprietary)
        ├── moof
        └── mdat

    Detection:
        uuid boxes with Microsoft GUIDs
        moof present

    8. QuickTime MOV

        MOV
        ├── ftyp (major brand = 'qt  ')
        ├── moov
        └── mdat

    Detection:
        ftyp major brand = 'qt  '
        Sample tables differ slightly from MP4

🎯 The 5‑Step Detection Algorithm (robust)

    You can detect any MP4 variant with this logic:

    Scan top‑level boxes
    If moof exists → it’s fragmented (fMP4, CMAF, DASH, HLS)
    If mvex exists inside moov → definitely fragmented
    If styp exists → CMAF segment
    If sidx exists → DASH segment
    If ftyp major brand = 'qt  ' → QuickTime MOV
    If uuid with MS GUID → Smooth Streaming
    If moov contains stbl sample tables → classic MP4

    This is exactly how Bento4 and FFmpeg detect MP4 types.

🧷 TL;DR

    moov is always required except in standalone segments (CMAF/HLS).
    moof means fragmented.
    mvex inside moov confirms fragmented mode.
    styp = CMAF, sidx = DASH, qt = MOV.
    mdat can appear multiple times in any variant.
    
*/
/*
🎯 How MP4 locates media samples

MP4 sample tables (stco, co64, trun, etc.) store absolute file offsets, not:

    not “mdat #1”

    not “mdat index”

    not “mdat ID”

    not relative offsets

Just absolute byte positions from the start of the file.

This is why multiple mdat boxes are allowed — the sample tables don’t care which mdat the data is in.
🌳 Example (ASCII)


File layout (realistic)
0x00000000  ftyp
0x00000200  moov
0x0000A3F0  mdat #1 (size ~0x00150000)
0x001F8C20  mdat #2 (size ~0x00012000)
0x0020B7A0  mdat #3 (size ~0x0000F000)

Sample table might say:

Sample 0 offset = 0x0000A4C0   (inside mdat #1)
Sample 1 offset = 0x001F8D10   (inside mdat #2)
Sample 2 offset = 0x0020B800   (inside mdat #3)

Sample table:
Sample offset = 0x001F8D10

Your demuxer checks:
mdat #1: [0x0000A3F0 .. 0x0015A3F0]   <-- no
mdat #2: [0x001F8C20 .. 0x0020AC20]   <-- yes
mdat #3: [0x0020B7A0 .. 0x0021A7A0]   <-- no

Your demuxer must:

    Collect all mdat boxes with their file offsets

    For each sample offset, find which mdat range it falls into

    Seek to that absolute offset and read the sample

📌 Which boxes contain sample offsets?
Non‑fragmented MP4

    stco — 32‑bit chunk offsets

    co64 — 64‑bit chunk offsets

These point directly to absolute file positions.
Fragmented MP4

    trun — sample offsets relative to moof

    tfhd — default base offset

    tfdt — decode time (not file offset)

    moof — has its own absolute file offset

Fragmented MP4 uses:
Code

absolute_offset = moof_file_offset + trun_data_offset

Still absolute in the end.
🧠 Why MP4 uses absolute offsets

Because MP4 is a box-based container, not a chunk-indexed one.

    mdat boxes have no IDs

    They have no names

    They have no internal structure

    They are just raw byte blobs

So the only reliable way to reference media is:

    Absolute byte offsets from the start of the file.

🧷 Yes, metadata always uses absolute file offsets.

    No, MP4 never references a specific mdat by ID.

    Yes, your class design with optional for unique boxes and vector for multi‑occurrence boxes is correct.

    Yes, detection logic should operate over the collected box set.

🧷 For a full remux, not a reorder, you must rewrite:
    stco
    co64
    trun
    tfhd
    sidx
    mfra
    any other offset‑based box
    
struct Mp4File 
{
    std::optional<Atom> ftyp;
    std::optional<Atom> moov;

    std::vector<Atom> mdat;   // keep in file order
    std::vector<Atom> moof;   // keep in file order
    std::vector<Atom> free;   // keep in file order
    std::vector<Atom> skip;   // keep in file order
    std::vector<Atom> sidx;   // keep in file order
    std::vector<Atom> styp;    // usually 1, but vector is fine
    std::vector<Atom> uuid;   // keep in file order

    // detection flags
    bool is_fragmented;
    bool is_cmaf;
    bool is_dash;
    bool is_mov;
};

*/