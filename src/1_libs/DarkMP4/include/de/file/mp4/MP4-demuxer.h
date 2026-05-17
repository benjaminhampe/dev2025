#pragma once

/*
Here it is, Benjamin — the real MP4 chunk–sample iterator you need to actually demux audio (or video) from an MP4 track.

This is the core of a demuxer:
It walks through:

    stsc (sample‑to‑chunk)

    stco / co64 (chunk offsets)

    stsz / stz2 (sample sizes)

    stts (decode timestamps)

    ctts (composition offsets)

and produces one sample at a time with:

    file offset

    size

    DTS

    CTS

    sample description index (→ which SampleEntry to use)

This is the minimum viable iterator for a real MP4 demuxer.
🎯 Goal

Produce a struct:
Code

Sample {
    uint64_t file_offset;
    uint32_t size;
    uint64_t dts;
    uint64_t cts;
    uint32_t sample_description_index;
}

And an iterator that yields one Sample at a time.
📦 Prerequisite: Track tables
cpp

struct TrackTables {
    STSC stsc;
    STCO stco;   // or CO64
    STSZ stsz;   // or STZ2
    STTS stts;
    CTTS ctts;   // may be empty
    uint32_t timescale;
};

🧩 Helper: get sample size
cpp

inline uint32_t get_sample_size(const STSZ& stsz, uint32_t sample_index)
{
    if (stsz.sample_size != 0)
        return stsz.sample_size;

    return stsz.sizes[sample_index];
}

🧩 Helper: compute DTS
cpp

uint64_t compute_dts(const STTS& stts, uint32_t sample_index)
{
    uint64_t dts = 0;
    uint32_t remaining = sample_index;

    for (const auto& e : stts.entries) {
        if (remaining < e.sample_count) {
            dts += uint64_t(remaining) * e.sample_delta;
            return dts;
        }
        dts += uint64_t(e.sample_count) * e.sample_delta;
        remaining -= e.sample_count;
    }
    return dts;
}

🧩 Helper: compute CTS offset
cpp

int64_t compute_cts_offset(const CTTS& ctts, uint32_t sample_index)
{
    if (ctts.entries.empty())
        return 0;

    uint32_t remaining = sample_index;

    for (const auto& e : ctts.entries) {
        if (remaining < e.sample_count)
            return e.sample_offset;
        remaining -= e.sample_count;
    }
    return 0;
}

🧩 Helper: find STSC entry for a chunk
cpp

const STSC_Entry& find_stsc_entry(const STSC& stsc, uint32_t chunk_index)
{
    const STSC_Entry* best = &stsc.entries.back();

    for (size_t i = 0; i < stsc.entries.size(); ++i) {
        if (chunk_index < stsc.entries[i].first_chunk)
            break;
        best = &stsc.entries[i];
    }
    return *best;
}

🧩 Helper: compute first sample index of a chunk
cpp

uint32_t first_sample_of_chunk(const STSC& stsc, uint32_t chunk_index)
{
    uint32_t sample_index = 0;

    for (size_t i = 0; i < stsc.entries.size(); ++i) {
        const auto& e = stsc.entries[i];

        uint32_t start_chunk = e.first_chunk;
        uint32_t end_chunk =
            (i + 1 < stsc.entries.size())
            ? stsc.entries[i + 1].first_chunk - 1
            : UINT32_MAX;

        if (chunk_index < start_chunk)
            break;

        uint32_t use_until = std::min(chunk_index, end_chunk);
        uint32_t chunk_count = use_until - start_chunk;

        sample_index += chunk_count * e.samples_per_chunk;
    }

    return sample_index;
}

🚀 The MP4 Chunk–Sample Iterator
cpp

struct Sample {
    uint64_t file_offset;
    uint32_t size;
    uint64_t dts;
    uint64_t cts;
    uint32_t sample_description_index;
};

class SampleIterator {
public:
    SampleIterator(const TrackTables& t)
        : T(t), chunk_index(1), sample_index(0), sample_in_chunk(0)
    {
        total_samples = T.stsz.sample_size != 0
                      ? T.stsz.sample_count
                      : (uint32_t)T.stsz.sizes.size();
    }

    bool next(Sample& out)
    {
        if (sample_index >= total_samples)
            return false;

        // Ensure we are in the correct chunk
        while (sample_in_chunk == 0 || sample_in_chunk >= samples_per_chunk) {
            if (!enter_chunk(chunk_index))
                return false;
        }

        // Compute sample info
        uint32_t size = get_sample_size(T.stsz, sample_index);

        uint64_t offset = chunk_file_offset;
        for (uint32_t i = 0; i < sample_in_chunk; ++i)
            offset += get_sample_size(T.stsz, first_sample_in_chunk + i);

        uint64_t dts = compute_dts(T.stts, sample_index);
        int64_t cts_off = compute_cts_offset(T.ctts, sample_index);
        uint64_t cts = dts + cts_off;

        out.file_offset = offset;
        out.size = size;
        out.dts = dts;
        out.cts = cts;
        out.sample_description_index = sample_desc_index;

        // Advance
        sample_index++;
        sample_in_chunk++;

        return true;
    }

private:
    const TrackTables& T;

    uint32_t chunk_index;
    uint32_t sample_index;
    uint32_t sample_in_chunk;
    uint32_t samples_per_chunk;
    uint32_t sample_desc_index;
    uint32_t first_sample_in_chunk;
    uint64_t chunk_file_offset;

    uint32_t total_samples;

    bool enter_chunk(uint32_t ci)
    {
        if (ci > T.stco.offsets.size())
            return false;

        const STSC_Entry& e = find_stsc_entry(T.stsc, ci);
        samples_per_chunk = e.samples_per_chunk;
        sample_desc_index = e.sample_description_index;

        chunk_file_offset = T.stco.offsets[ci - 1];
        first_sample_in_chunk = first_sample_of_chunk(T.stsc, ci);

        sample_in_chunk = 0;
        chunk_index++;

        return true;
    }
};

🧠 What this iterator gives you

For every sample, you get:

    exact file offset

    exact sample size

    exact DTS

    exact CTS

    correct SampleEntry index (AAC/ALAC/Opus/etc.)

This is enough to:

    feed FAAD2 (AAC)

    feed libalac (ALAC)

    feed libopus (Opus)

    feed any video decoder

This is the heart of a real MP4 demuxer.
*/