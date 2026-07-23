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

🧩 1. hdlr — Detect Audio Track
hdlr.handler_type = "soun"
This tells you:
    This trak is audio
    You should parse its stbl


🧩 4. stsz / stz2 — Sample Sizes

This gives you:

    size of each encoded audio sample

    OR a single default size for all samples

For AAC/ALAC/Opus:

    Each sample = one encoded audio packet
    (AAC access unit, ALAC frame, Opus packet)

This is the exact number of bytes you must read from mdat.
🧩 5. stco / co64 — Chunk Offsets

These give you:
Code

absolute file offsets into mdat

Combine with stsc and stsz to compute:
Code

sample[i].offset = chunk_offset + offset_inside_chunk
sample[i].size   = stsz[i]

This is how you locate each encoded packet.
🧩 6. stts — Decode Time to Sample

For audio:

    Usually constant duration per sample

    Needed only if you want timestamps

Not needed for raw demuxing.
🧩 7. ctts — Composition Time to Sample

Audio almost never uses this.
Ignore unless you see non‑zero entries.
🧩 8. mdat — Raw Encoded Bytes

This contains:

    AAC access units

    ALAC frames

    Opus packets

    FLAC frames

    PCM samples

Your demuxer uses:

    stco/co64 → chunk offsets

    stsc → samples per chunk

    stsz → sample sizes

…to extract the exact byte ranges.
🎧 HOW THESE ATOMS WORK TOGETHER

Here is the real pipeline:
Code

hdlr → is this audio?
stsd → which codec? (AAC/ALAC/Opus/FLAC/PCM)
esds/alac/dOps → decoder config
stsc → how samples map to chunks
stsz → sample sizes
stco/co64 → chunk offsets
mdat → raw bytes

Then:
Code

for each sample:
    offset = chunk_offset + running_offset
    size   = stsz[i]
    read(size bytes from mdat)
    feed to decoder

🧠 WHICH ATOMS YOU CAN IGNORE

You can safely ignore:

    mvhd
    tkhd
    mdhd
    edts
    stss
    stsh
    stdp
    sdtp
    sgpd
    sbgp
    subs
    cslg
    udta
    meta
    keys
    ilst
    pasp
    colr
    clap
    pclr
    pasp
    pasp
    mvex, moof, traf (unless fragmented MP4)

🟢 FINAL SUMMARY — THE ATOMS THAT MATTER
Atom                Purpose	Required?
hdlr                detect audio track	✔
stsd                detect codec	✔
esds / alac / dOps	decoder config	✔
stsc                sample→chunk mapping	✔
stsz                sample sizes	✔
stco / co64         chunk offsets	✔
mdat                raw encoded bytes	✔
stts                timestamps	optional
ctts                B‑frame offsets	almost never

Everything else is irrelevant for audio demuxing.

If you want, I can now give you:

    a complete audio demuxer architecture
    a full AAC demuxer pipeline
    a full ALAC demuxer pipeline
    a full Opus demuxer pipeline

Just tell me which codec you want to implement first.
Overview

For audio demuxing only (getting decoder‑ready encoded packets from mdat), you need a very small, precise subset of ISO BMFF atoms.

Below is the complete, practical set that matters—and what you can safely ignore.
1. Track and codec identification

    moov → trak → mdia → hdlr
        Purpose: Decide which tracks are audio.
        Key field:
            handler_type
                "soun" → audio track
                "vide" → video (ignore for audio demux)

        Why it matters: You only demux trak where handler_type == "soun".

    moov → trak → mdia → minf → stbl → stsd
        Purpose: Identify the audio codec and find its config.
        Key fields:
            Sample entry type (first 4 bytes of each entry):
                "mp4a" → AAC
                "alac" → ALAC
                "Opus" → Opus
                "fLaC" → FLAC
                "lpcm" → PCM
                "ac-3" / "ec-3" → AC‑3 / E‑AC‑3

        Why it matters: This tells you which decoder to use.

2. Codec configuration atoms (inside stsd entries)

These are codec‑specific children of the sample entry in stsd:

    AAC (mp4a → esds)
        Atom: esds → DecoderSpecificInfo → AudioSpecificConfig (ASC)
        Key fields (from ASC):
            object_type (AAC LC = 2)
            sample_rate_index
            channel_config

        Why it matters: Needed to init FAAD2 / any AAC decoder and to build ADTS headers if required.

    ALAC (alac → alac)
        Atom: inner alac box
        Key fields:
            frameLength
            bitDepth
            numChannels
            maxFrameBytes
            Rice parameters

        Why it matters: This is the ALAC “magic cookie” for the decoder.

    Opus (Opus → dOps)
        Atom: dOps
        Key fields:
            output_channel_count
            pre_skip
            input_sample_rate
            output_gain

        Why it matters: Needed to init Opus decoder and handle pre‑skip correctly.

    FLAC (fLaC → dfLa)
        Atom: dfLa
        Key fields: FLAC STREAMINFO block.
        Why it matters: Decoder config.

    PCM (lpcm)
        Atom: sample entry fields
        Key fields:
            sample_size
            sample_rate
            channel_count

        Why it matters: Direct PCM layout.

3. Sample location and size (core demuxing atoms)

All under: moov → trak (audio) → mdia → minf → stbl

    stsc — Sample‑to‑Chunk
        Purpose: Tells you how many samples are in each chunk.
        Key fields per entry:
            first_chunk
            samples_per_chunk
            sample_description_index

        Why it matters: You need this to know how to walk through stco/co64 and stsz correctly.

    stsz / stz2 — Sample Sizes
        Purpose: Gives the size in bytes of each encoded audio sample, or a single default size.
        Key fields:
            sample_size (0 = variable)
            sample_count
            entry_size[i] (if variable)

        Why it matters: This tells you exactly how many bytes to read for each packet from mdat.

    stco / co64 — Chunk Offsets
        Purpose: Gives absolute file offsets of each chunk in mdat.
        Key fields:
            chunk_offset[i] (32‑bit in stco, 64‑bit in co64)

        Why it matters: Combined with stsc and stsz, this gives you sample.offset.

    mdat — Media Data
        Purpose: Contains the raw encoded bytes:
            AAC access units
            ALAC frames
            Opus packets
            FLAC frames
            PCM samples

        Why it matters: This is what you actually feed to the decoder, using offsets/sizes from the tables above.

4. Timing (optional for pure demuxing)

These are only needed if you care about timestamps:

    stts — Decoding Time to Sample
        Purpose: Maps sample index → decode time.
        Key fields: (sample_count, sample_delta) entries.
        Use: Compute DTS.

    ctts — Composition Time to Sample
        Purpose: Maps sample index → composition offset.
        Use: Compute PTS (rarely used for audio).

You can demux and decode audio without these if you don’t need timestamps.
5. Atoms you can ignore for audio demuxing

For pure audio packet extraction, you can ignore:
    Movie/track headers: mvhd, tkhd, mdhd
    Edit lists: edts, elst
    Sync/keyframe info: stss, stsh
    Quality/priority: stdp, sdtp
    Sample groups: sgpd, sbgp, subs, cslg
    User data / metadata: udta, meta, keys, ilst, ©nam, ©ART, covr, etc.
    Color / video‑only: pasp, colr, clap, pclr, etc.
    Fragment stuff (if not using fMP4): mvex, moof, traf, tfhd, trun, tfdt, sidx, ssix, mfra, tfra, mfro
    DRM: pssh, senc, saiz, saio, tenc, sinf, schi, schm, frma (unless you handle encrypted streams)

6. Minimal “must‑parse” set for audio demuxing

You absolutely need:

    hdlr — to pick audio tracks ("soun")
    stsd — to detect codec (mp4a, alac, Opus, …)
    esds / alac / dOps / dfLa / PCM fields — decoder config
    stsc — sample‑to‑chunk mapping
    stsz / stz2 — sample sizes
    stco / co64 — chunk offsets
    mdat — raw encoded data

Everything else is optional for demuxing.

If you want, next step I can lay out a concrete function‑level pipeline:
parse_moov → pick_audio_trak → parse_stsd → parse_tables → build_sample_list → feed_decoder.
*/
/*
🧠 NOTES ON WHAT MATTERS FOR AUDIO DEMUXING
You MUST parse:

    hdlr → detect audio track

    stsd → detect codec (mp4a, alac, Opus)

    esds / alac / dOps → decoder config

    stsc → sample‑to‑chunk mapping

    stsz → sample sizes

    stco/co64 → chunk offsets

You do NOT need:

    mvhd, tkhd, mdhd (unless you want timestamps)

    ctts (audio rarely uses it)

    stts (only for timing)

    udta (metadata only)

    mvex, moof, traf (unless fragmented MP4)
*/


// 1. What 'hdlr' tells you (codec type detection)
// Inside each trak → mdia → hdlr:
// handler_type = "soun"   → this is an audio track
// handler_type = "vide"   → video track
// handler_type = "hint"   → hint track
// So first:

// Only parse tracks where handler_type == "soun"

// But hdlr does NOT tell you which audio codec.
// It only tells you “this is audio”.

// To determine AAC vs ALAC vs Opus, you must read the sample entry inside:
// trak
//  └── mdia
//       └── minf
//            └── stbl
//                 └── stsd   ← sample descriptions
// Inside stsd, you get entries like:
//     Codec	SampleEntry type
//     AAC      "mp4a"
//     ALAC     "alac"
//     Opus     "Opus"
//     FLAC     "fLaC"
//     PCM      "lpcm"   The codec is determined by the 4‑byte type of the sample entry in stsd.
// 'stsc' (Sample‑to‑Chunk)
//  first_chunk = 1, samples_per_chunk = 12
//  first_chunk = 100, samples_per_chunk = 8
//    Meaning:
//      chunks 1–99 have 12 samples each
//      chunks 100–end have 8 samples each
//    So to reconstruct sample offsets:
//      Walk chunks in order
//      For each chunk, read its offset from stco/co64
//      For each sample in that chunk, assign size from stsz
//      Accumulate offsets

// how to detect AAC / ALAC / Opus
// Step 1 — find audio track (hdlr == "soun")
// Step 2 — inside that track, parse stsd

// Inside stsd:
// Code

// [0..3] size
// [4..7] type  ← THIS determines codec

// So:
// c++

// if (atom_is(type, 'm','p','4','a')) codec = AAC;
// if (atom_is(type, 'a','l','a','c')) codec = ALAC;
// if (atom_is(type, 'O','p','u','s')) codec = OPUS;

// Step 3 — parse codec‑specific info

// AAC → esds atom contains AudioSpecificConfig

// ALAC → alac atom contains ALAC magic cookie

// Opus → dOps atom contains Opus header

// You must extract these and pass them to the decoder.
