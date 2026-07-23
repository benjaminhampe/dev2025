#include <de/audio/file/MP3/FileStreamer_MP3.h>

#if 0
// #include <stdint.h>
// #include <string>
// #include <vector>
// #include <cstdio>

#include "dr_mp3.h"

namespace de {
namespace audio {

class FileReader_MP3_Impl
{
public:
    bool m_isOpen;
    uint32_t m_sampleRate;
    uint32_t m_channels;
    uint64_t m_frameCount;
    uint64_t m_frameIndex;
    drmp3 m_mp3;
    std::string m_uri;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;
    AlignedFloatVector m_I; // interleaved

public:
    FileReader_MP3_Impl()
        : m_isOpen{ false }
        , m_sampleRate{ 0 }
        , m_channels{ 0 }
        , m_frameCount{ 0 }
        , m_frameIndex{ 0 }
        , m_mp3{}
    {
    }

    ~FileReader_MP3_Impl()
    {
        close();
    }

    void open(const std::string& uri)
    {
        close();

        if (!drmp3_init_file(&m_mp3, uri.c_str(), nullptr))
        {
            return;
        }
        m_isOpen = true;
        m_sampleRate   = m_mp3.sampleRate;
        m_channels     = m_mp3.channels;
        m_frameCount  = drmp3_get_pcm_frame_count(&m_mp3);
        m_frameIndex = 0;
        m_uri     = uri;


        m_I.resize(m_channels * 1024);
    }

    void close()
    {
        if (!m_isOpen) return;
        drmp3_uninit(&m_mp3);
        m_sampleRate = 0;
        m_channels = 0;
        m_frameCount = 0;
        m_frameIndex = 0;
        m_uri.clear();
        m_isOpen = false;
    }

    // Decode N frames into output buffer (interleaved float)
    // Returns number of frames actually decoded.
    uint64_t decodeFrames(uint64_t framesToRead, float* out)
    {
        return drmp3_read_pcm_frames_f32(&m_mp3, framesToRead, out);
    }

    // Reset stream to beginning
    void rewind()
    {
        drmp3_seek_to_pcm_frame(&m_mp3, 0);
    }

    // Duration in seconds
    double durationSeconds() const
    {
        if (m_sampleRate == 0) return 0.0;
        return double(m_frameCount) / double(m_sampleRate);
    }

    // Duration in seconds
    u64 durationNanoseconds() const
    {
        if (m_sampleRate == 0) return 0ull;
        return static_cast<u64>(std::ceil(1.0e9 * double(m_frameCount) / double(m_sampleRate)));
    }

    u64 tellFrame() const
    {
        return m_frameIndex;
    }

    bool seekFrame( u64 frame )
    {
        return drmp3_seek_to_pcm_frame(&m_mp3, frame) == DRMP3_TRUE;
    }

    u64 readFrames( u64 framesToRead, f32* __restrict__ L, f32* __restrict__ R )
    {
        u64 nSamples = framesToRead * m_channels;
        if (m_I.size() < nSamples)
        {
            m_I.resize(nSamples);
        }

        u64 nFrames = drmp3_read_pcm_frames_f32(&m_mp3, framesToRead, m_I.data());

        if (nFrames > framesToRead)
        {
            DE_ERROR("nFrames > framesToRead")
            return 0;
        }

        if (m_channels > 0)
        {
            for (size_t i = 0; i < nFrames; ++i)
            {
                L[i] = m_I[ i * m_channels ];
            }
            for (size_t i = nFrames; i < framesToRead; ++i)
            {
                L[i] = 0.0f;
            }
        }

        if (m_channels > 1)
        {
            for (size_t i = 0; i < nFrames; ++i)
            {
                R[i] = m_I[ i * m_channels + 1u ];
            }
            for (size_t i = nFrames; i < framesToRead; ++i)
            {
                R[i] = 0.0f;
            }
        }

        return nFrames;
    }

    // Dump all info we can get
    void dumpInfo() const
    {
        DE_DEBUG("MP3 File = ",m_uri)
        DE_TRACE("MP3 SampleRate = ",m_sampleRate," Hz")
        DE_TRACE("MP3 Channels = ",m_channels)
        DE_TRACE("MP3 Frames = ",m_frameCount)
        DE_TRACE("MP3 Duration = ",durationSeconds()," sec")
        DE_TRACE("MP3 Bitrate = ",estimateBitrateKbps()," kbps")
        // DE_TRACE("MP3 MPEG Layer = ",mpegLayerString())
        DE_TRACE("MP3 VBR = ", m_mp3.isVBR ? "yes" : "no")
    }

    // --- Extra Info Helpers --------------------------------------------------

    // dr_mp3 exposes MPEG info via internal fields
    // const char* mpegLayerString() const {
    //     switch (m_mp3.mp3FrameInfo.layer) {
    //         case DRMP3_MPEG_LAYER_I:   return "Layer I";
    //         case DRMP3_MPEG_LAYER_II:  return "Layer II";
    //         case DRMP3_MPEG_LAYER_III: return "Layer III";
    //         default:                   return "Unknown";
    //     }
    // }

    float estimateBitrateKbps() const
    {
        // if (mp3.mp3FrameInfo.bitrate_kbps > 0)
        //     return float(mp3.mp3FrameInfo.bitrate_kbps);

        // fallback: estimate from file size + duration
        FILE* f = fopen(m_uri.c_str(), "rb");
        if (!f) return 0.0f;
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fclose(f);

        double sec = durationSeconds();
        if (sec <= 0.0) return 0.0f;

        return float((size * 8.0) / 1000.0 / sec);
    }

};





// ===========================================================================
FileReader_MP3::FileReader_MP3()
    : _d( new FileReader_MP3_Impl() )
{
}

FileReader_MP3::~FileReader_MP3()
{
    delete _d;
}

std::string FileReader_MP3::getUri() const
{
    return _d->m_uri;
}

u64 FileReader_MP3::getFrameCount() const
{
    return _d->m_frameCount;
}

u32 FileReader_MP3::getSampleRate() const
{
    return _d->m_sampleRate;
}

u32 FileReader_MP3::getChannelCount() const
{
    return _d->m_channels;
}

u32 FileReader_MP3::getSampleType() const
{
    return 0;
}

u64 FileReader_MP3::getDuration() const
{
    return _d->durationNanoseconds();
}

void FileReader_MP3::openStream( std::string uri )
{
    _d->open(uri);
}

void FileReader_MP3::closeStream()
{
    _d->close();
}

bool FileReader_MP3::isStreamOpen() const
{
    return _d->m_isOpen;
}

// void play() const override;

// void stop() const override;

// bool isPlaying() const override;

// void pause() const override;

// void resume() const override;

u64 FileReader_MP3::tellFrame() const
{
    return _d->m_frameIndex;
}

bool FileReader_MP3::seekFrame( u64 frame ) const
{
    return _d->seekFrame(frame);
}

u64 FileReader_MP3::readFrames( u64 frames, f32* __restrict__ L, f32* __restrict__ R )
{
    return _d->readFrames(frames,L,R);
}

} // end namespace audio.
} // end namespace de.

#endif