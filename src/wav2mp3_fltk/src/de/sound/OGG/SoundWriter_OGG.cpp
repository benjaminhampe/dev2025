#include <de/sound/OGG/SoundWriter_OGG.h>
#include <de/sound/OGG/SoundWriter_OGG_Vorbis.h>
#include <de/sound/OGG/SoundWriter_OGG_Opus.h>

namespace de {
namespace sound {

bool
save_sound_ogg(
    const Sound& sound,
    const std::string & uri,
    const SoundSaveOptions& options)
{
    auto ext = dbFileSuffix(uri);
    if (ext == "ogg")
    {
        return save_sound_ogg_vorbis(sound, uri, options);
    }
    else if (ext == "opus")
    {
        return save_sound_ogg_opus(sound, uri, options);
    }
    else
    {
        DE_ERROR("Unsupported format. ",uri)
        return false;
    }

    /*

    switch (codec)
    {
        case OggCodec::Vorbis:
            //DE_ERROR("Unsupported OGG Vorbis format. ",uri)
            return load_sound_ogg_vorbis(sound, uri, options);

        case OggCodec::Opus:
            // TODO: loadOpus(path, snd);
            DE_ERROR("Unsupported OGG Opus format. ",uri)
            return false;

        case OggCodec::Speex:
            // TODO: loadSpeex(path, snd);
            DE_ERROR("Unsupported OGG Speex format. ",uri)
            return false;

        case OggCodec::Flac:
            // TODO: loadOggFlac(path, snd);
            DE_ERROR("Unsupported OGG Flac format. ",uri)
            return false;

        case OggCodec::Pcm:
            // TODO: loadOggPcm(path, snd);
            DE_ERROR("Unsupported OGG Xiph PCM format. ",uri)
            return false;

        case OggCodec::Theora:
            DE_ERROR("Unsupported OGG Theora format. ",uri)
            return false;

        case OggCodec::Dirac:
            DE_ERROR("Unsupported OGG Dirac format. ",uri)
            return false;

        // case OggCodec::Skeleton:
        // case OggCodec::Cmml:
        default:
            DE_ERROR("Unsupported or non audio OGG format. ",uri)
            return false;
    }
    */

    return false;
}

} // end namespace sound.
} // end namespace de.
