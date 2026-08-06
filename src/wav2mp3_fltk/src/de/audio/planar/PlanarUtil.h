#pragma once
#include <de/sound/Sound.h>
#include <de/audio/planar/Planar.h>

namespace de {
namespace audio {

// ===========================================================================
struct PlanarUtil
// ===========================================================================
{
    //🔥
    static int64_t
    copy( const Sound& src, Planar& dst, int64_t frameCount, int64_t srcFrameStart = 0);
};

} // end namespace sound.
} // end namespace de.
