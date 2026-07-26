#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool resample_r8brain_f32(Sound & sound, int32_t outRate);

} // end namespace sound.
} // end namespace de.

