#pragma once
#include <de/sound/Sound.h>
#include <de/image/Image.h>

namespace de {
namespace sound {

// ===========================================================================
struct Sound2Image
// ===========================================================================
{
    //🔥Draw a waveform
    static void
    draw( const Sound& sound, int32_t channel, int64_t frameBeg, int64_t frameEnd,
          Image& img, const Recti& pos, uint32_t penColor, uint32_t fillColor = 0);
};

} // end namespace sound.
} // end namespace de.
