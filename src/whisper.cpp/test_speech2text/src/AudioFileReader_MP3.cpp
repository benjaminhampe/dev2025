#include "AudioFileReader_MP3.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include <iostream>

bool load_mp3_f32(AudioFile & file, const std::string & uri )
{
    std::vector<float> pcm;

    drmp3 mp3;
    if (!drmp3_init_file(&mp3, uri.c_str(), nullptr))
    {
        std::cerr << "Fehler: MP3 konnte nicht geöffnet werden\n";
        return false;
    }

    const uint64_t nFrames = drmp3_get_pcm_frame_count(&mp3);
    const uint32_t nChannels = mp3.channels;
    const uint64_t nSamples = nFrames * nChannels;
    file.frame_count = nFrames;
    file.channels = nChannels;
    file.sample_rate = mp3.sampleRate;
    file.sample_type = AudioFile::ST_F32 | AudioFile::ST_Interleaved;
    file.samples.resize( nSamples * sizeof(float) );

    auto dst = reinterpret_cast<float*>(file.samples.data());
    drmp3_read_pcm_frames_f32(&mp3, nFrames, dst);
    drmp3_uninit(&mp3);

    if (nFrames == 0)
    {
        std::cerr << "Fehler: MP3 enthält keine Daten\n";
        return false;
    }

    return true;
}
