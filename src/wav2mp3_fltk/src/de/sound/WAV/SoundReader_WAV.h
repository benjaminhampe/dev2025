#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool load_sound_wav(
    Sound& sound,
    const std::string& uri,
    const de::SoundLoadOptions& options);

} // end namespace sound.
} // end namespace de.
