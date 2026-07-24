#include <de/sound/OPUS/SoundReader_OPUS.h>

// opus_load.cpp
#include <opusfile.h>
//#include <stdexcept>

/*
struct OpusAudio {
    int channels = 0;
    int sampleRate = 48000; // Opus always 48k
    std::vector<float> pcm; // interleaved float
};
*/

namespace de {
namespace sound {

bool load_sound_opus_f32(Sound & sound, const std::string & uri )
{
    int err = 0;
    OggOpusFile* of = op_open_file(uri.c_str(), &err);
    if (!of)
    {
        DE_ERROR("op_open_file failed: ", err)
        return false;
    }

    sound.m_sampleType = Sound::ST_F32;
    sound.m_sampleRate = 48000;
    sound.m_channels = op_channel_count(of, -1);
    sound.m_samples.clear();

    if (op_seekable(of))
    {
        DE_BENNI("OPUS seekable = 1")
        int li = op_current_link(of);
        const OpusHead *head=op_head(of,li);
        const OpusTags *tags;
        int             binary_suffix_len;
        int             ci;
        // fprintf(stderr,"  Channels: %i\n",head->channel_count);
        int64_t duration = op_pcm_total(of,li);
        DE_BENNI("OPUS duration = ",duration)
        DE_BENNI("OPUS duration = ",dbStrSeconds(double(duration)/48000.0))
        int64_t size = op_raw_total(of,li);
        DE_BENNI("OPUS size = ",size)
    }
    else
    {
        DE_BENNI("OPUS seekable = 0")
    }

    constexpr int BLOCK = 4096;
    TAlignedVector<float> buf(BLOCK * sound.m_channels + 1024);

    int64_t iIteration = 0;
    int64_t frameCount = 0;
    while (true)
    {
        int ret = op_read_float(of, buf.data(), BLOCK * sound.m_channels, nullptr);
        //DE_TRACE("frameCount(",frameCount,"), chunk = ",ret)
        if (ret == 0)
        {
            break; // EOF
        }
        else if (ret < 0)
        {
            DE_ERROR("op_read_float failed: ",ret, ", frameCount(",frameCount,")")
            break;
        }
        else
        {
            auto beg = reinterpret_cast<uint8_t*>(buf.data());
            auto end = reinterpret_cast<uint8_t*>(buf.data()) + (ret * sound.m_channels * sizeof(float));
            sound.m_samples.insert(sound.m_samples.end(), beg, end);
            frameCount += ret;
        }
    }

    op_free(of);

    sound.m_frames = frameCount;

    return true;
}

} // end namespace sound.
} // end namespace de.


#if 0
bool load_sound_snd_f32(Sound & sound, const std::string & uri )
{
    auto ext = dbFileSuffix( uri );
    int fmt = Utils::getSndFormatFromFileExt( ext );
    if (fmt < 0)
    {
        DE_ERROR("Unsupported SNDFILE format, uri = ", uri)
        return false;
    }

    SF_INFO info{};
    info.format = 0;

    SNDFILE* file = sf_open(uri.c_str(), SFM_READ, &info);
    if ( !file )
    {
        DE_ERROR("Cant open uri ", uri )
        return false;
    }

    DE_DEBUG("SNDFILE "
             "Seekable(",info.seekable,"), "
             "Sections(",info.sections,"), "
             "Uri(", uri,")")
    DE_DEBUG("SNDFILE "
             "Fmt(",info.format,"), "
             "Ext(",Utils::getFormatStr(info.format),"), "
             "SampleType(",Utils::getSampleTypeStr(info.format),"), "
             "Endian(",Utils::getEndianessStr(info.format),")")
    DE_DEBUG("SNDFILE "
             "SampleRate(",info.samplerate,"), "
             "Channels(",info.channels,"), "
             "Frames(",info.frames,")")

    sound.m_sampleType = Sound::ST_F32;
    sound.m_sampleRate = info.samplerate;
    sound.m_frames = info.frames;
    sound.m_channels = info.channels;
    sound.m_samples.resize(sound.m_frames * sound.m_channels);

    sf_count_t gotten = sf_readf_float(file, sound.m_samples.data(), sound.m_frames);
    sf_close(file);

    if (gotten != sound.m_frames)
    {
        DE_ERROR("Gotten(",gotten,") != sound.m_frameCount(",sound.m_frames,")")
    }

    return true;
}

#endif
