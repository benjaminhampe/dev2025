#pragma once
#include <de/audio/file/Sound.h>

namespace de {
namespace audio {

// ===========================================================================
class SoundFactory
// ===========================================================================
{
public:



    static bool load(Sound & sound, std::string uri);
};


} // end namespace audio.
} // end namespace de.
