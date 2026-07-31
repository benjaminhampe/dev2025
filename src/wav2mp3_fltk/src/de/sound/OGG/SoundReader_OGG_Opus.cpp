#include <de/sound/OGG/SoundReader_OGG_Opus.h>

#include <opusfile.h>

namespace de {
namespace sound {

namespace {

struct OpusIO
{
    int fd;
};

int OpusIO_read(void* _stream, unsigned char* _ptr, int _nbytes)
{
    OpusIO* io = (OpusIO*)_stream;

    // libopusfile's op_read_func contract: return bytes or negative
    return file64_read(io->fd, _ptr, (int64_t)_nbytes);;
}

int OpusIO_seek(void* ds, opus_int64 offset, int whence)
{
    OpusIO* src = (OpusIO*)ds;
    eSeekMode mode = eSeekMode::Set;
    switch (whence)
    {
        case SEEK_CUR: mode = eSeekMode::Cur; break;
        case SEEK_END: mode = eSeekMode::End; break;
        default: break;
    }
    int64_t pos = file64_seek(src->fd, offset, mode);
    return (pos < 0 ? -1 : 0);
}

int OpusIO_close(void* ds)
{
    OpusIO* src = (OpusIO*)ds;
    file64_close(src->fd);
    return 0;
}

opus_int64 OpusIO_tell(void* ds)
{
    OpusIO* src = (OpusIO*)ds;
    return (opus_int64)file64_tell(src->fd);
}

static OpusFileCallbacks OpusIO_callbacks = {
    OpusIO_read,
    OpusIO_seek,
    OpusIO_tell,
    OpusIO_close
};

} // end namespace

bool load_sound_ogg_opus(
    Sound& sound,
    const std::string& uri,
    const SoundLoadOptions& options)
{
    sound.m_uri = uri;

    int fd = file64_open(uri.c_str(), eFileMode::Read);
    if (fd < 0)
    {
        DE_ERROR("Cannot open OGG/Opus file ", uri)
        return false;
    }

    OpusIO io;
    io.fd = fd;

    int err = 0;
    OggOpusFile* of = op_open_callbacks(&io, &OpusIO_callbacks, nullptr, 0, &err);
    if (!of)
    {
        OpusIO_close(&io);
        DE_ERROR("op_open_callbacks failed: ", err, " ", uri)
        return false;
    }

    const OpusHead* head = op_head(of, -1);
    if (!head)
    {
        op_free(of);
        DE_ERROR("op_head failed: ", uri)
        return false;
    }

    sound.m_sampleType = SampleType::F32;
    sound.m_sampleRate = head->input_sample_rate;
    sound.m_channels   = head->channel_count;
    sound.m_flags      = 0;

    ogg_int64_t totalFrames = op_pcm_total(of, -1);
    sound.m_frames = (totalFrames > 0 ? totalFrames : 0);

    // if (sound.m_frames > 0)
    // {
    //     size_t bytes = sound.m_frames * sound.m_channels * sizeof(float);
    //     sound.m_samples.resize(bytes);
    // }

    // float* dst = reinterpret_cast<float*>(sound.m_samples.data());
    int64_t framesRead = 0;

    std::vector<float> buf(4096 * sound.m_channels);

    while (true)
    {
        int ret = op_read_float(of, buf.data(), (int)buf.size(), nullptr);

        if (ret == 0)
            break;      // EOF
        if (ret < 0)
        {
            DE_ERROR("op_read_float failed with ",ret)
            break;
        }

        auto beg = reinterpret_cast<const uint8_t*>(buf.data());
        auto end = beg + (ret * sound.m_channels * sizeof(float));
        sound.m_samples.insert( sound.m_samples.end(), beg, end);

        framesRead += ret;
    }

    sound.m_frames = framesRead;

    op_free(of);
    return true;
}


} // end namespace sound.
} // end namespace de.


#if 0


bool
load_sound_ogg_opus(
    Sound & sound,
    const std::string & uri,
    const SoundLoadOptions& options)
{
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

#endif
