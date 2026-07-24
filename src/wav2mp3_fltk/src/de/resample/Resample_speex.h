#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool resample_speex_f32(Sound & sound, int32_t outRate);

} // end namespace sound.
} // end namespace de.

