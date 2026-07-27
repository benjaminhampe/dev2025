#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool
save_sound_opus_f32(
    const Sound& sound,
    const std::string& url,
    const SoundSaveOptions& options);

} // end namespace sound.
} // end namespace de.
