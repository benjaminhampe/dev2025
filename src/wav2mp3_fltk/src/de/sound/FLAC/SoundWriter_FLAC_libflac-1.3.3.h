#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool
save_sound_flac(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options);

} // end namespace sound.
} // end namespace de.
