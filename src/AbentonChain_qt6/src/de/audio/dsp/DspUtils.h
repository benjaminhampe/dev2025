#pragma once
#include <DarkImage.h>
#include <de/de_aligned_memory.h>

namespace de {
namespace audio {

// Reads one channel with sampleCount sc = fc * cc from src to dst.
inline void DSP_GET_CHANNEL(
    f32* __restrict__ dst,      // Destination mono channel buffer
    u64 dstFrames,              // Destination mono channel buffer frame count
    f32 const* __restrict__ src,// Source multi channel buffer start,
    u64 srcFrames,  // Source multi channel buffer frame count 'fc'.
    u32 srcChannel, // The channel you want to extract 'ci'
    u32 srcChannels)// The number of channels the source has 'cc'.
{
    src += srcChannel; // Advance to first sample of desired channel.
    auto n = std::min( srcFrames, dstFrames );
    for ( u64 i = 0; i < n; i++ )
    {
        *dst = (*src);
        dst++;
        src += srcChannels; // jump to next sample of srcChannel.
    }
}

inline void DSP_FUSE_STEREO_TO_MONO(
    float* mono,           // the destination single channel buffer
    float const* src,      // the source buffer with any num of channels
    uint64_t srcFrames,
    uint32_t srcChannels )
{
    if ( srcChannels < 1 )
    {
        return; // Nothing todo, no data.
    }
    else if ( srcChannels == 1 )  // Mono Rail
    {
        for ( size_t i = 0; i < srcFrames; i++ )
        {
            *mono++ = *src++; // directly read and write L.
        }
    }
    else if ( srcChannels == 2 ) // Optimized for stereo ( only one add! op. less )
    {
        for ( size_t i = 0; i < srcFrames; i++ )
        {
            float L = *src++;       // read iL
            float R = *src++;       // read iR
            *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
            mono++;                 // Advance to next sample oL
        }
    }
    else // if ( srcChannels > 2 )
    {
        auto skipAmount = srcChannels - 2;
        for ( size_t i = 0; i < srcFrames; i++ )
        {
            float L = *src++;       // read iL
            float R = *src++;       // read iR
            src += skipAmount;      // Advance to next sample iL
            *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
            mono++;                 // Advance to next sample oL
        }
    }

}

inline void DSP_FILLZERO( f32* __restrict__ p, u64 n)
{
    std::fill(p, p + n, 0.0f);
}

inline void DSP_FILLZERO( AlignedFloatVector & o )
{
    DSP_FILLZERO(o.data(), o.size());
}

inline void DSP_MONO(
    f32* __restrict__ mono,
    u32 channels,
    u32 frames,
    const f32* __restrict__ src,
    int channel_to_extract = -1)
{
    if (channels<1) return;
    if (frames<1) return;

    if (channel_to_extract<0)
    {
        // Compute average sum of all channels
        // For stereo this becomes: mono[sample] = (L+R)/2;
        const f32 fInv = 1.0f / f32(channels);
        for (u64 i = 0; i < frames; i++)
        {
            f32 fAccum = 0.0;
            for (u64 c = 0; c < channels; c++)
            {
                fAccum += *src++;
            }
            *mono++ = fAccum * fInv;
        }
    }
    else
    {
        // Extract specific channel
        if (channel_to_extract < channels)
        {
            // Goto channel ch[n] from ch[0].
            src += channel_to_extract;

            // Read channel ch[n], skip 'channels' samples
            // to reach next interleaved sample ch[n+1].
            for (u64 i = 0; i < frames; i++)
            {
                *mono++ = *src; // write channel to mono output; mono[i] = src[(channels*i)+channel_to_extract];
                src += channels; // src[channels*(i+1)] = src[channels*i] + channels;
            }
        }
    }
}

inline void DSP_ADD(float* __restrict dst, u64 n, const float* __restrict src_a, const float* __restrict src_b )
{
    for (u64 i = 0; i < n; ++i) { dst[i] = src_a[i] + src_b[i]; }
}

inline void DSP_ADD(float* __restrict__ dst, u64 n, const float* __restrict__ src)
{
    for (u64 i = 0; i < n; ++i) { dst[i] += src[i]; }
}

inline void DSP_COPY(float* __restrict__ dst, u64 n, const float* __restrict__ src)
{
    for (u64 i = 0; i < n; ++i) { dst[i] = src[i]; }
}

inline void DSP_COPY(float* __restrict__ dst, u64 n, const float* __restrict__ src, u64 src_stride, u64 dst_stride)
{
    for (u64 i = 0; i < n; i++)
    {
        *dst = *src;
        src += src_stride;
        dst += dst_stride;
    }
}

inline void DSP_MUL(
    f32* __restrict__ dst_values,
    u64 n,
    const f32* __restrict__ src_values,
    f32 src_factor)
{
    if (n<1) return;
    for (u64 i = 0; i < n; i++)
    {
        (*dst_values) = (*src_values) * src_factor;
        src_values++;
        dst_values++;
    }
}

inline void DSP_MUL(
    f32* __restrict__ dst_values,
    u64 n,
    const f32* __restrict__ src_values,
    const f32* __restrict__ src_factors)
{
    if (n<1) return;
    for (u64 i = 0; i < n; i++)
    {
        (*dst_values) = (*src_values) * (*src_factors);
        src_values++;
        dst_values++;
        src_factors++;
    }
}

inline void DSP_MUL(
    const f32* __restrict__ src,
    f32* __restrict__ dst,
    u64 n,
    f32 factor)
{
    if (n<1) return;
    for (u64 i = 0; i < n; i++)
    {
        (*dst) = (*src) * factor;
        src++;
        dst++;
    }
}

inline void DSP_MUL(
    const DE_AlignedFloatVector & src,
    DE_AlignedFloatVector & dst,
    f32 factor )
{
    if (src.empty()) { DE_WARN("") return; }
    if (src.size() != dst.size()) { DE_WARN("") return; }
    DSP_MUL(src.data(),dst.data(),src.size(),factor);
}

inline void DSP_MUL( DE_AlignedFloatVector & dst, f32 factor )
{
    for (f32 & v : dst) { v *= factor; }
}
inline void DSP_ADD( DE_AlignedFloatVector & dst, f32 offset )
{
    for (f32 & v : dst) { v += offset; }
}
inline void DSP_FMA( DE_AlignedFloatVector & dst, f32 factor, f32 offset )
{
    for (f32 & v : dst) { v = (v * factor) + offset; }
}
inline void DSP_RESIZE( DE_AlignedFloatVector & dst, u64 n )
{
    if (dst.size() != n)
    {
        dst.resize( n );
    }
}
inline void DSP_ZEROES( DE_AlignedFloatVector & dst )
{
    for (f32 & val : dst) { val = 0.0f; }
}

inline void DSP_RESIZE( DE_AlignedFloatShiftVector & dst, u64 n )
{
    if (dst.size() != n)
    {
        dst.resize( n );
    }
}

inline void DSP_CLEAR( DE_AlignedFloatShiftVector & dst )
{
    dst.clear();
}

inline void DSP_RESIZE( DE_AlignedFloatShiftMatrix & dst, u32 nCols, u32 nRows )
{
    if (dst.columnCount() != nCols)
    {
        dst.resize( nCols, nRows );
    }
}

} // end namespace audio.
} // end namespace de.
