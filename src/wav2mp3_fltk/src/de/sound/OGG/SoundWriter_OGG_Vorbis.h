#pragma once
#include <de/sound/Sound.h>

/*
OGG/Vorbis
OGG/Opus
OGG/FLAC
OGG/Speex
OGG/Theora
*/

namespace de {
namespace sound {

bool
save_sound_ogg_vorbis(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options);

} // end namespace sound.
} // end namespace de.
