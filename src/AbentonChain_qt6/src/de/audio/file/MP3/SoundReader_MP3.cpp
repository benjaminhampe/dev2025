#include <de/audio/file/MP3/SoundReader_MP3.h>

#include "dr_mp3.h"

namespace de {
namespace audio {

bool load_sound_mp3_f32(Sound & sound, const std::string & uri )
{
    drmp3 mp3;
    if (!drmp3_init_file(&mp3, uri.c_str(), nullptr))
    {
        DE_ERROR("No MP3 file opened. ",uri)
        return false;
    }

    const uint64_t nFrames = drmp3_get_pcm_frame_count(&mp3);
    const uint32_t nChannels = mp3.channels;
    const uint64_t nSamples = nFrames * nChannels;
    sound.m_uri = de::FileSystem::makeAbsolute(uri);
    sound.m_fileInfo.frameCount = nFrames;
    sound.m_fileInfo.channelCount = nChannels;
    sound.m_fileInfo.sampleRate = mp3.sampleRate;
    sound.m_fileInfo.sampleType = FileInfo::ST_F32; // | FileInfo::ST_Interleaved;
    sound.m_samples.resize( nSamples );

    float* __restrict__ dst = reinterpret_cast<float*>(sound.m_samples.data());
    drmp3_read_pcm_frames_f32(&mp3, nFrames, dst);
    drmp3_uninit(&mp3);

    if (nFrames == 0)
    {
        DE_ERROR("No MP3 data")
        return false;
    }

    return true;
}

} // end namespace audio.
} // end namespace de.
