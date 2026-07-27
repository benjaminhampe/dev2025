#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

// ===========================================================================
struct SoundUtil
// ===========================================================================
{
    //🔥
    static int64_t
    copy( const Sound& src, Sound& dst, int64_t frameCount, int64_t srcFrameStart = 0);

    //🔥
    static int64_t
    convert( const Sound& src, Sound& dst, SampleType dstType);

    //🔥Make planar, L|R|L|R -> L|L|R|R
    static int64_t
    deinterleave( const Sound& src, Sound& dst );

    //🔥Make interleaved, L|L|R|R -> L|R|L|R
    static int64_t
    interleave( const Sound& src, Sound& dst );

    //🔥Determine most negative of all samples
    static float
    mostNegative( const Sound& sound );

    //🔥Determine most positive of all samples
    static float
    mostPositive( const Sound& sound );

    //🔥Determine absolute biggest sample value
    static float
    maximum( const Sound& sound );
};

} // end namespace sound.
} // end namespace de.
