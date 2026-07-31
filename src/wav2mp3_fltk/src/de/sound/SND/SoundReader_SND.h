#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

bool load_sound_sndfile(
    Sound & ret,
    const std::string & url,
    const SoundLoadOptions& options );

} // end namespace sound.
} // end namespace de.
