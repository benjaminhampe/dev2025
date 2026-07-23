#include <DarkSound.h>
#include <de/sound/WAV/SoundReader_WAV.h>
#include <de/sound/MP3/SoundReader_MP3.h>
#include <de/sound/MP4/SoundReader_MP4.h>
//#include <de/sound/FLAC/SoundReader_FLAC.h>

bool dbLoadSound( de::Sound & sound, const std::string& uri )
{
    bool ok = false;

    auto suffix = dbFileSuffix(uri);
    if (suffix == "mp3")
    {
        ok = de::sound::load_sound_mp3_f32(sound, uri );
    }
    else if (suffix == "wav")
    {
        ok = de::sound::load_sound_wav_f32(sound, uri );
    }
    else if ((suffix == "mp4") || (suffix == "m4a"))
    {
        ok = de::sound::load_sound_mp4_f32(sound, uri );
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
    DE_OK(sound.str())
    //DE_OK("dat.size() = ",sound.m_samples.size())
    //DE_OK("dat.frames() = ",sound.m_samples.size() / sound.m_fileInfo.channelCount)
    return true;
}
