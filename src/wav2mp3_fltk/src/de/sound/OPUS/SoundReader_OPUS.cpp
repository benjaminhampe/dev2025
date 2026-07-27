#include <de/sound/OPUS/SoundReader_OPUS.h>

#include <opusfile.h>

namespace de {
namespace sound {

bool
load_sound_opus_f32(Sound & sound, const std::string & uri)
{
    if (!sound.empty())
    {
        DE_ERROR("Got empty sound: ", uri)
        return false;
    }

    if (sound.m_sampleRate != 48000)
    {
        DE_WARN("Opus wants 48000 sound data, resample first. ", uri)
    }

    if (sound.m_sampleType != SampleType::F32)
    {
        DE_ERROR("Opus wants ST_F32 sampleType, convert first. ", uri)
        return false;
    }

    int err = 0;
    OggOpusFile* of = op_open_file(uri.c_str(), &err);
    if (!of)
    {
        DE_ERROR("op_open_file failed: ", err)
        return false;
    }

    sound.m_sampleType = SampleType::F32;
    sound.m_sampleRate = 48000;
    sound.m_channels = op_channel_count(of, -1);
    sound.m_samples.clear();

    if (op_seekable(of))
    {
        int li = op_current_link(of);
        //const OpusHead* head = op_head(of,li);
        //const OpusTags* tags;
        //int             ci;
        //int             binary_suffix_len;
        int64_t duration = op_pcm_total(of,li);
        int64_t size = op_raw_total(of,li);
        DE_BENNI("OPUS seekable(1), "
                 "duration(",dbStrSeconds(double(duration)/48000.0),"), "
                 "size(",size,").")
    }
    else
    {
        DE_BENNI("OPUS seekable(0).")
    }

    constexpr int BLOCK = 4096;
    TAlignedVector<float> buf(BLOCK * sound.m_channels + 1024);

    //int64_t iIteration = 0;
    int64_t frameCount = 0;
    while (true)
    {
        //DE_TRACE("frameCount(",frameCount,"), chunk = ",ret)
        int ret = op_read_float(of, buf.data(), BLOCK * sound.m_channels, nullptr);
        if (ret == 0)
        {
            break; // EOF
        }
        else if (ret < 0)
        {
            DE_ERROR("op_read_float(",ret,"), frameCount(",frameCount,").")
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
