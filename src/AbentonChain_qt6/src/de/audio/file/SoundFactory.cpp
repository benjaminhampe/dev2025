#include <de/audio/file/SoundFactory.h>
#include <de/audio/file/WAV/SoundReader_WAV.h>
#include <de/audio/file/MP3/SoundReader_MP3.h>
//#include <de/audio/file/FLAC/SoundReader_FLAC.h>
#include <de/audio/file/MP4/SoundReader_MP4.h>

namespace de {
namespace audio {

// static
// ===========================================================================
bool SoundFactory::load(Sound & sound, std::string uri)
// ===========================================================================
{
    bool ok = false;

    auto suffix = dbFileSuffix(uri);
    if (suffix == "mp3")
    {
        ok = load_sound_mp3_f32(sound, uri );
    }
    else if (suffix == "wav")
    {
        ok = load_sound_wav_f32(sound, uri );
    }
    else if ((suffix == "mp4") || (suffix == "m4a"))
    {
        ok = load_sound_mp4_f32(sound, uri );
    }
    else
    {
        DE_ERROR("Unsupported file ",uri)
    }

    if (!ok)
    {
        DE_ERROR("Not loaded. ",uri)
        return false;
    }

    DE_OK("//==================================")
    DE_OK(dbFileName(sound.m_uri))
    DE_OK(dbFileDir(sound.m_uri))
    DE_OK("//==================================")
    DE_OK(sound.m_fileInfo.str())
    //DE_OK("dat.size() = ",sound.m_samples.size())
    //DE_OK("dat.frames() = ",sound.m_samples.size() / sound.m_fileInfo.channelCount)
    return true;
}

} // end namespace audio.
} // end namespace de.
