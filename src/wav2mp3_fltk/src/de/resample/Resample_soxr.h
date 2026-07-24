#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool resample_soxr_f32(Sound & sound, int32_t out_rate);

} // end namespace sound.
} // end namespace de.

