#include <DarkSound.h>
#include <de/sound/SoundUtil.h>
#include <de/sound/MP3/SoundReader_MP3.h>
#include <de/sound/MP4/SoundReader_MP4.h>
#include <de/sound/SND/SoundReader_SND.h>
#include <de/sound/OPUS/SoundReader_OPUS.h>
//#include <de/sound/WAV/SoundReader_WAV.h>
//#include <de/sound/FLAC/SoundReader_FLAC.h>
#include <de/sound/MP3/SoundWriter_MP3.h>


#include <de/resample/Resampler_r8brain.h>

namespace {

bool supportedByLibSNDFILE(const std::string & ext)
{
    if (ext.empty())
    {
        DE_ERROR("Got empty suffix")
        return false;
    }
         if (ext=="wav")  return true;
    else if (ext=="ogg")  return true;
    else if (ext=="aif")  return true;
    else if (ext=="aiff") return true;
    else if (ext=="flac") return true;
    else if (ext=="au")   return true;
    else if (ext=="raw")  return true;
    else if (ext=="paf")  return true;
    else if (ext=="svx")  return true;
    else if (ext=="nist") return true;
    else if (ext=="voc")  return true;
    else if (ext=="sf")   return true;
    else if (ext=="w64")  return true;
    else if (ext=="mat4") return true;
    else if (ext=="mat5") return true;
    else if (ext=="pvf")  return true;
    else if (ext=="xi")   return true;
    else if (ext=="htk")  return true;
    else if (ext=="sds")  return true;
    else if (ext=="avr")  return true;
    else if (ext=="sd2")  return true;
    else if (ext=="caf")  return true;
    else if (ext=="wve")  return true;
    else if (ext=="mpc2k")return true;
    else if (ext=="rf64") return true;
    else
    {
        return false;
    }
}

} // end namespace.

bool dbLoadSound( de::Sound & sound, const std::string& uri )
{
    de::PerformanceTimer perf;
    perf.start();

    bool ok = false;

    auto suffix = dbFileSuffix(uri);
    if (suffix == "mp3")
    {
        ok = de::sound::load_sound_mp3_f32(sound, uri );
    }
    else if ((suffix == "mp4") || (suffix == "m4a"))
    {
        ok = de::sound::load_sound_mp4_f32(sound, uri );
    }
    else if (suffix == "opus")
    {
        ok = de::sound::load_sound_opus_f32(sound, uri );
    }
    else if (supportedByLibSNDFILE(suffix))
    {
        ok = de::sound::load_sound_snd_f32(sound, uri );
    }
    // else if (suffix == "wav")
    // {
    //     ok = de::sound::load_sound_wav_f32(sound, uri );
    // }
    else
    {
        DE_ERROR("Unsupported decoder for ",uri)
    }

    if (!ok)
    {
        DE_ERROR("Not loaded. ",uri)
        return false;
    }

    // DE_OK("//==================================")
    // DE_OK(dbFileName(sound.m_uri))
    // DE_OK(dbFileDir(sound.m_uri))
    // DE_OK("//==================================")

    perf.stop();

    DE_OK("[Needed] ",perf.ms(), " ms, Sound(",sound.str(),")")

    sound.validate();
    //DE_OK("dat.size() = ",sound.m_samples.size())
    //DE_OK("dat.frames() = ",sound.m_samples.size() / sound.m_fileInfo.channelCount)
    return true;
}



bool
dbSaveSound(
    const de::Sound & sound,
    const std::string& uri,
    const de::SoundSaveOptions& options)
{
    de::PerformanceTimer perf;
    perf.start();

    bool ok = false;

    auto suffix = dbFileSuffix(uri);
    if (suffix == "mp3")
    {
        ok = de::sound::save_sound_mp3_f32(sound, uri, options);
    }
    // else if (suffix == "wav")
    // {
    //     ok = de::sound::save_sound_wav_f32(sound, uri );
    // }
    // else if ((suffix == "mp4") || (suffix == "m4a"))
    // {
    //     ok = de::sound::load_sound_mp4_f32(sound, uri );
    // }
    // else if (suffix == "opus")
    // {
    //     ok = de::sound::load_sound_opus_f32(sound, uri );
    // }
    // else if (supportedByLibSNDFILE(suffix))
    // {
    //     ok = de::sound::load_sound_snd_f32(sound, uri );
    // }

    else
    {
        DE_ERROR("Unsupported encoder for ",uri)
    }

    if (!ok)
    {
        DE_ERROR("Not saved. ",uri)
        return false;
    }

    // DE_OK("//==================================")
    // DE_OK(dbFileName(sound.m_uri))
    // DE_OK(dbFileDir(sound.m_uri))
    // DE_OK("//==================================")

    perf.stop();

    DE_OK("[Needed] ",perf.ms(), " ms, Sound(",sound.str(),")")

    // sound.validate();
    //DE_OK("dat.size() = ",sound.m_samples.size())
    //DE_OK("dat.frames() = ",sound.m_samples.size() / sound.m_fileInfo.channelCount)
    return true;
}


bool dbResampleSound(
        const de::Sound & src,
        de::Sound & dst,
        int32_t sampleRate,
        int32_t quality)
{
    de::sound::Resampler_r8brain resampler;
    return resampler.resample(src,dst,sampleRate);
}

int64_t dbCopySound(
        const de::Sound& src,
        de::Sound& dst,
        int64_t srcFrameCount,
        int64_t srcFrameStart)
{
    return de::sound::SoundUtil::copy(src, dst, srcFrameCount, srcFrameStart);
}

int64_t dbDeinterleaveSound(
        const de::Sound & src,
        de::Sound & dst)
{
    return de::sound::SoundUtil::deinterleave(src,dst);
}

int64_t dbInterleaveSound(
        const de::Sound & src,
        de::Sound & dst)
{
    return de::sound::SoundUtil::interleave(src,dst);
}

int64_t dbConvertSound(
        const de::Sound & src,
        de::Sound & dst,
        de::SampleType dstType)
{
    return de::sound::SoundUtil::convert(src,dst,dstType);
}

