#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool
load_sound_ogg_vorbis(
    Sound & sound,
    const std::string & uri,
    const SoundLoadOptions& options );

} // end namespace sound.
} // end namespace de.
