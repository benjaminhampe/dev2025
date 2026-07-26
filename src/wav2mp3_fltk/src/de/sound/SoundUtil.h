#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

// ===========================================================================
struct SoundUtil
// ===========================================================================
{
    static int64_t
    copy( const Sound& src, Sound& dst, int64_t frameCount, int64_t srcFrameStart = 0);

    static int64_t
    convert( const Sound& src, Sound& dst, SampleType dstType);

    //🔥Deinterleave (interleaved → planar)
    static int64_t
    deinterleave( const Sound& src, Sound& dst );

    //🔥Interleave (planar → interleaved)
    static int64_t
    interleave( const Sound& src, Sound& dst );

    static void
    deinterleaveConvert(
        int32_t srcChannels,
        SampleType srcType,
        const TAlignedVector<uint8_t>& srcSamples,
        TAlignedVector<uint8_t>& tmpSamples,
        SampleType dstType,
        TAlignedVector<uint8_t>& dstSamples,
        int64_t maxFrameCount,
        int64_t srcFrameStart = 0);

/*
    float maximum() const;
*/

};

} // end namespace sound.
} // end namespace de.
