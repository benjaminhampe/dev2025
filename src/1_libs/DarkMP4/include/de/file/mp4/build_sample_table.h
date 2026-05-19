#pragma once
#include <de/file/mp4/moov/trak/mdia/minf/stbl/Atom_stbl.h>

namespace de {
namespace file {
namespace mp4 {

struct MP4_SampleInfo
{
    uint64_t offset;
    uint32_t size;
    uint64_t dts;
    uint32_t duration;
    uint64_t cts;
};

typedef std::vector<MP4_SampleInfo> MP4_SampleInfoTable;

MP4_SampleInfoTable build_sample_table(

    // --- stsz/stz2 --- size() = sampleCount
    const MP4_SampleSizes& stsz,

    // --- stsc ---
    const MP4_SampleToChunk& stsc,

    // --- stco/co64 ---
    const MP4_ChunkOffsets& stco,

    // --- stts ---
    const MP4_DecodingTimeToSample& stts,

    // --- ctts (optional) ---
    const MP4_CompositionTimeToSample& ctts
)
{
    const uint32_t sampleCount = static_cast<uint32_t>(stsz.size());
    const uint32_t chunkCount  = static_cast<uint32_t>(stco.size());

    MP4_SampleInfoTable out(sampleCount);

    // -----------------------------------------
    // 1) Sample sizes (stsz)
    // -----------------------------------------
    for (uint32_t i = 0; i < sampleCount; ++i)
    {
        out[i].size   = stsz[i];
        out[i].offset = 0;
        out[i].dts    = 0;
        out[i].duration = 0;
        out[i].cts    = 0;
    }

    // -----------------------------------------
    // 2) Expand stsc → chunkSamples[]
    //    ISO: Ein Eintrag gilt von first_chunk[e]
    //    bis first_chunk[e+1] - 1 (inklusive).
    // -----------------------------------------
    std::vector<uint32_t> chunkSamples(chunkCount, 0);

    if (!stsc.empty())
    {
        for (uint32_t e = 0; e < stsc.size(); ++e)
        {
            const uint32_t firstChunk = stsc[e].first_chunk; // 1‑basiert
            const uint32_t samplesPerChunk = stsc[e].samples_per_chunk;

            // inclusive last chunk index (1‑basiert)
            uint32_t lastChunk1Based =
                (e + 1 < stsc.size())
                    ? (stsc[e + 1].first_chunk - 1)
                    : chunkCount; // bis zum letzten Chunk

            if (firstChunk == 0 || firstChunk > chunkCount)
                continue; // defensiv

            if (lastChunk1Based > chunkCount)
                lastChunk1Based = chunkCount;

            const uint32_t first = firstChunk - 1;        // 0‑basiert
            const uint32_t last  = lastChunk1Based - 1;   // 0‑basiert

            for (uint32_t c = first; c <= last; ++c)
            {
                chunkSamples[c] = samplesPerChunk;
            }
        }
    }

    // -----------------------------------------
    // 3) Compute sample offsets (stco + stsz)
    //    Annahme: alle Samples eines Chunks liegen
    //    direkt hintereinander in diesem Track.
    // -----------------------------------------
    uint32_t s = 0;
    for (uint32_t c = 0; c < chunkCount && s < sampleCount; ++c)
    {
        uint64_t off = stco[c];
        const uint32_t n = chunkSamples[c];

        for (uint32_t i = 0; i < n && s < sampleCount; ++i, ++s)
        {
            out[s].offset = off;
            off += out[s].size;
        }
    }

    // -----------------------------------------
    // 4) DTS + duration (stts)
    // -----------------------------------------
    uint64_t dts = 0;
    uint32_t idx = 0;

    for (uint32_t e = 0; e < stts.size() && idx < sampleCount; ++e)
    {
        const uint32_t count = stts[e].sample_count;
        const uint32_t delta = stts[e].sample_delta;

        for (uint32_t i = 0; i < count && idx < sampleCount; ++i, ++idx)
        {
            out[idx].dts      = dts;
            out[idx].duration = delta;
            dts += delta;
        }
    }

    // Falls stts weniger Samples abdeckt, bleiben die restlichen
    // mit duration = 0; das ist ein harter Fehler im File, aber
    // wir lassen es so stehen.

    // -----------------------------------------
    // 5) CTS (ctts optional)
    // -----------------------------------------
    if (ctts.empty())
    {
        for (uint32_t i = 0; i < sampleCount; ++i)
            out[i].cts = out[i].dts;
    }
    else
    {
        uint32_t idx2 = 0;
        for (uint32_t e = 0; e < ctts.size() && idx2 < sampleCount; ++e)
        {
            const uint32_t count = ctts[e].sample_count;
            const int32_t  offs  = ctts[e].sample_offset;

            for (uint32_t i = 0; i < count && idx2 < sampleCount; ++i, ++idx2)
            {
                out[idx2].cts = static_cast<uint64_t>(
                    static_cast<int64_t>(out[idx2].dts) + static_cast<int64_t>(offs)
                );
            }
        }

        // Falls ctts weniger Samples abdeckt: Rest = DTS
        while (idx2 < sampleCount)
        {
            out[idx2].cts = out[idx2].dts;
            ++idx2;
        }
    }

    return out;
}


#if 0




struct MP4_SampleInfo
{
    uint64_t offset;
    uint32_t size;
    uint64_t dts;
    uint32_t duration;
    uint64_t cts;
};

typedef std::vector<MP4_SampleInfo> MP4_SampleInfoTable;

MP4_SampleInfoTable build_sample_table(

    // --- stsz/stz2 --- size() = sampleCount
    const MP4_SampleSizes& stsz,

    // --- stsc ---
    const MP4_SampleToChunk& stsc,

    // --- stco/co64 ---
    const MP4_ChunkOffsets& stco,

    // --- stts ---
    const MP4_DecodingTimeToSample& stts,

    // --- ctts (optional) ---
    const MP4_CompositionTimeToSample& ctts
)
{
    const uint32_t sampleCount = stsz.size();
    const uint32_t chunkCount  = stco.size();

    MP4_SampleInfoTable out(sampleCount);

    // -----------------------------------------
    // 1) Sample sizes (stsz)
    // -----------------------------------------
    for (uint32_t i = 0; i < sampleCount; i++)
        out[i].size = stsz[i];

    // -----------------------------------------
    // 2) Expand stsc → chunkSamples[]
    // -----------------------------------------
    std::vector<uint32_t> chunkSamples(chunkCount);

    for (uint32_t e = 0; e < stsc.size(); e++)
    {
        uint32_t first = stsc[e].first_chunk - 1;
        uint32_t last  = (e + 1 < stsc.size())
                         ? stsc[e + 1].first_chunk - 1
                         : chunkCount - 1;

        for (uint32_t c = first; c <= last; c++)
        {
            chunkSamples[c] = stsc[e].samples_per_chunk;
        }
    }

    // -----------------------------------------
    // 3) Compute sample offsets
    // -----------------------------------------
    uint32_t s = 0;
    for (uint32_t c = 0; c < chunkCount; c++)
    {
        uint64_t off = stco[c];
        uint32_t n   = chunkSamples[c];

        for (uint32_t i = 0; i < n && s < sampleCount; i++, s++)
        {
            out[s].offset = off;
            off += out[s].size;
        }
    }

    // -----------------------------------------
    // 4) DTS + duration (stts)
    // -----------------------------------------
    uint64_t dts = 0;
    uint32_t idx = 0;

    for (uint32_t e = 0; e < stts.size(); e++)
    {
        uint32_t count = stts[e].sample_count;
        uint32_t delta = stts[e].sample_delta;

        for (uint32_t i = 0; i < count && idx < sampleCount; i++, idx++)
        {
            out[idx].dts = dts;
            out[idx].duration = delta;
            dts += delta;
        }
    }

    // -----------------------------------------
    // 5) CTS (ctts optional)
    // -----------------------------------------
    if (ctts.empty())
    {
        for (uint32_t i = 0; i < sampleCount; i++)
        {
            out[i].cts = out[i].dts;
        }
    }
    else
    {
        uint32_t idx2 = 0;
        for (uint32_t e = 0; e < ctts.size(); e++)
        {
            uint32_t count = ctts[e].sample_count;
            int32_t  offs  = ctts[e].sample_offset;

            for (uint32_t i = 0; i < count && idx2 < sampleCount; i++, idx2++)
            {
                out[idx2].cts = out[idx2].dts + offs;
            }
        }
    }

    return out;
}
#endif

} // end namespace mp4.
} // end namespace file.
} // end namespace de.

/*

struct SampleInfo
{
    uint64_t offset;
    uint32_t size;
    uint64_t dts;
    uint32_t duration;
    uint64_t cts;
};

std::vector<SampleInfo>
build_sample_table(
    // --- stsz ---
    const std::vector<uint32_t>& stsz_sizes,   // size = sampleCount

    // --- stsc ---
    const std::vector<uint32_t>& stsc_first_chunk,
    const std::vector<uint32_t>& stsc_samples_per_chunk,

    // --- stco/co64 ---
    const std::vector<uint64_t>& stco_chunk_offsets,

    // --- stts ---
    const std::vector<uint32_t>& stts_sample_count,
    const std::vector<uint32_t>& stts_sample_delta,

    // --- ctts (optional) ---
    const std::vector<uint32_t>& ctts_sample_count,
    const std::vector<int32_t>&  ctts_sample_offset
)
{
    const uint32_t sampleCount = stsz_sizes.size();
    const uint32_t chunkCount  = stco_chunk_offsets.size();

    std::vector<SampleInfo> out(sampleCount);

    // -----------------------------------------
    // 1) Sample sizes (stsz)
    // -----------------------------------------
    for (uint32_t i = 0; i < sampleCount; i++)
        out[i].size = stsz_sizes[i];

    // -----------------------------------------
    // 2) Expand stsc → chunkSamples[]
    // -----------------------------------------
    std::vector<uint32_t> chunkSamples(chunkCount);

    for (uint32_t e = 0; e < stsc_first_chunk.size(); e++) {
        uint32_t first = stsc_first_chunk[e] - 1;
        uint32_t last  = (e + 1 < stsc_first_chunk.size())
                         ? stsc_first_chunk[e + 1] - 2
                         : chunkCount - 1;

        for (uint32_t c = first; c <= last; c++)
            chunkSamples[c] = stsc_samples_per_chunk[e];
    }

    // -----------------------------------------
    // 3) Compute sample offsets
    // -----------------------------------------
    uint32_t s = 0;
    for (uint32_t c = 0; c < chunkCount; c++) {
        uint64_t off = stco_chunk_offsets[c];
        uint32_t n   = chunkSamples[c];

        for (uint32_t i = 0; i < n && s < sampleCount; i++, s++) {
            out[s].offset = off;
            off += out[s].size;
        }
    }

    // -----------------------------------------
    // 4) DTS + duration (stts)
    // -----------------------------------------
    uint64_t dts = 0;
    uint32_t idx = 0;

    for (uint32_t e = 0; e < stts_sample_count.size(); e++) {
        uint32_t count = stts_sample_count[e];
        uint32_t delta = stts_sample_delta[e];

        for (uint32_t i = 0; i < count && idx < sampleCount; i++, idx++) {
            out[idx].dts = dts;
            out[idx].duration = delta;
            dts += delta;
        }
    }

    // -----------------------------------------
    // 5) CTS (ctts optional)
    // -----------------------------------------
    if (ctts_sample_count.empty()) {
        for (uint32_t i = 0; i < sampleCount; i++)
            out[i].cts = out[i].dts;
    } else {
        uint32_t idx2 = 0;
        for (uint32_t e = 0; e < ctts_sample_count.size(); e++) {
            uint32_t count = ctts_sample_count[e];
            int32_t  offs  = ctts_sample_offset[e];

            for (uint32_t i = 0; i < count && idx2 < sampleCount; i++, idx2++) {
                out[idx2].cts = out[idx2].dts + offs;
            }
        }
    }

    return out;
}

*/
