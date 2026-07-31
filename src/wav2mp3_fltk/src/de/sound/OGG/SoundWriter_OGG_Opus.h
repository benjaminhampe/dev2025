#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool
save_sound_ogg_opus(
    const Sound& sound,
    const std::string& url,
    const SoundSaveOptions& options);

} // end namespace sound.
} // end namespace de.
