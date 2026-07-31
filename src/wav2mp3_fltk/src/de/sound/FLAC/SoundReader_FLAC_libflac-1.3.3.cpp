#include <de/sound/FLAC/SoundWriter_FLAC_libflac-1.3.3.h>

#include <FLAC/stream_decoder.h>

#ifdef _WIN32
#include <FLAC/share/windows_unicode_filenames.h>
#endif

namespace de {
namespace sound {

namespace {

} // end namespace.

bool
load_sound_flac(
    Sound& sound,
    const std::string& uri,
    const SoundLoadOptions& options)
{
    if (sound.empty())
    {
        DE_WARN("Got empty sound. ",uri)
        return false;
    }

    return false;
}

} // end namespace sound.
} // end namespace de.

