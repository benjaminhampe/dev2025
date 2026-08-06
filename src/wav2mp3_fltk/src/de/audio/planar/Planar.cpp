#include <de/audio/planar/Planar.h>

namespace de {
namespace audio {

bool Planar::empty() const noexcept
{
    if (m_frames < 1) { return true; }
    if (m_channels < 1) { return true; }
    if (m_sampleRate < 1) { return true; }
    if (m_sampleType == SampleType::Unknown) { return true; }
    return false;
}

void Planar::clear() noexcept
{
    m_frames = 0;
    m_channels = 0;
    m_sampleRate = 0;
    m_sampleType = SampleType::Unknown;
    m_flags = 0;
    m_samples.clear();
}

void Planar::shrink_to_fit() noexcept
{
    clear();
    m_samples.shrink_to_fit();
}

double Planar::duration() const noexcept // In [s] seconds.
{
    if (m_sampleRate < 1)
    {
        DE_ERROR("Invalid sampleRate ",str())
        return 0.0;
    }

    return double(m_frames) / double(m_sampleRate);
}

int64_t Planar::sampleCount() const noexcept
{
    return m_frames * m_channels;
}

int64_t Planar::byteCount() const noexcept
{
    return m_frames * m_channels * bytesPerSample();
}

int32_t Planar::bytesPerSample() const noexcept
{
    return m_sampleType.bytesPerSample();
}

SampleType Planar::sampleType() const noexcept
{
    return m_sampleType;
}

std::string Planar::sampleTypeStr() const
{
    return m_sampleType.str();
}

int64_t Planar::memoryConsumption() const noexcept
{
    int64_t n = sizeof(*this);
    n += m_samples.capacity();
    return n;
}

bool Planar::validate() const
{
    int64_t memory = 0;
    for (size_t c = 0; c < m_samples.size(); ++c)
    {
        memory += m_samples[c].size();
    }

    const int64_t header = byteCount();

    const int64_t delta = header - memory;
    const int64_t pc = 100.0 * double(delta) / double(memory);
    if ( delta != 0 )
    {
        DE_ERROR("header(",header,") != memory(",memory,"), delta(",delta,"), percent(",pc,")")
        return false;
    }
    return true;
}

void Planar::allocFrames( int64_t frames )
{
    m_frames = frames;
    m_samples.resize(m_channels);
    for (size_t c = 0; c < m_samples.size(); ++c)
    {
        m_samples[c].resize( frames * bytesPerSample() );
    }
}

std::string Planar::str(bool bWithFileName, bool bWithDir) const
{
    std::ostringstream o;
    o << dbStrSeconds(duration()) << ","
    " " << m_sampleRate << "Hz,"
    " " << m_channels << " x"
    " " << sampleTypeStr() << " x"
    " " << m_frames << ", "
    " mem(" << dbStrBytes(memoryConsumption()) << ")";
    if (bWithFileName)
    {
        o << ", file(" << dbFileName(m_uri) << ")";
    }
    if (bWithFileName)
    {
        o << ", dir(" << dbFileDir(m_uri) << ")";
    }
    return o.str();
}

int64_t Planar::read_frames(
    SampleTypeConverter::Converter_t converter,
    uint8_t* __restrict__ Lout,
    uint8_t* __restrict__ Rout,
    int64_t frameCount,
    int64_t frameStart) const
{
    if (empty())
    {
        DE_ERROR("Empty")
        return 0;
    }

    if (m_channels != 2)
    {
        DE_WARN("m_channels(",m_channels,") != 2, ",str())
    }

    const int64_t avail = m_frames - frameStart;
    if (avail < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t maxFrames = std::min(avail, frameCount);
    if (maxFrames < 1)
    {
        return 0; // Nothing todo...
    }

    const int32_t bpp = bytesPerSample();
    const int64_t byteCount = maxFrames * bpp;
    const int64_t byteIndex = frameStart * bpp;

// <debug>
    for (int c = 0; c < m_channels; ++c)
    {
        if (byteCount < 1)
        {
            DE_ERROR("Channel[",c,"] Got byteCount(",byteCount,")")
            return 0;
        }

        if (byteIndex > m_samples[c].size())
        {
            DE_ERROR("Channel[",c,"] byteIndex(",byteIndex,") > m_samples(",m_samples.size(),")")
            return 0;
        }

        if (byteIndex + byteCount > m_samples[c].size())
        {
            DE_ERROR("Channel[",c,"] byteIndex(",byteIndex,") + byteCount(",byteCount,") > m_samples(",m_samples.size(),")")
            return 0;
        }
    }
// </debug>

    if (m_channels == 1)
    {
        const uint8_t* __restrict__ Lin = m_samples[0].data() + byteIndex;
        converter(Lin, Lout, maxFrames);
        converter(Lin, Rout, maxFrames);
    }
    else // if (m_channels == 2)
    {
        const uint8_t* __restrict__ Lin = m_samples[0].data() + byteIndex;
        const uint8_t* __restrict__ Rin = m_samples[1].data() + byteIndex;
        converter(Lin, Lout, maxFrames);
        converter(Rin, Rout, maxFrames);
    }

    // Needs dstBPP, but not avail, only converter, caller must fill silence!
    /*
    if (maxFrames < frameCount)
    {
        int64_t remainBytes = (frameCount - maxFrames) *
        uint8_t* __restrict__ Lsilence = Lout + byteIndex;
        uint8_t* __restrict__ Rsilence = Rout + byteIndex;
        std::memset(lsilence, 0,
    }

    if (maxFrames < frameCount)
    {
        int64_t remainFrames = (frameCount - maxFrames);


        uint8_t* __restrict__ Lsilence = Lout + byteIndex;
        uint8_t* __restrict__ Rsilence = Rout + byteIndex;
        std::memset(lsilence, 0,
    }
    */

    return maxFrames;
}

/*
int64_t Planar::read_frames(void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const
{
    const int64_t avail = m_frames - frameStart;
    if (avail < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t maxFrames = std::min(avail, frameCount);
    if (maxFrames < 1)
    {
        return 0; // Nothing todo...
    }

    const int32_t bpp = bytesPerSample();
    const int64_t byteCount = maxFrames * m_channels * bpp;
    const int64_t byteIndex = frameStart * m_channels * bpp;

#if 0
// <debug>
    if (byteCount < 1)
    {
        DE_ERROR("Got byteCount(",byteCount,")")
        return 0;
    }

    if (byteIndex >= m_samples.size())
    {
        DE_ERROR("byteIndex(",byteIndex,") > m_samples(",m_samples.size(),")")
        return 0;
    }

    if (byteIndex + byteCount >= m_samples.size())
    {
        DE_ERROR("byteIndex(",byteIndex,") + byteCount(",byteCount,") > m_samples(",m_samples.size(),")")
        return 0;
    }
// </debug>
#endif

    const uint8_t* __restrict__ src = m_samples.data() + byteIndex;

    std::memcpy(dst, src, byteCount);

    return maxFrames;
}


bool Planar::append(const Planar& other)
{
    if ( m_sampleType != other.m_sampleType )
    {
        if (m_sampleType == SampleType::Unknown &&
            other.m_sampleType != SampleType::Unknown)
        {
            m_sampleType = other.m_sampleType;
        }
        else
        {
            DE_ERROR("m_sampleType(",m_sampleType,") != other.m_sampleType(",other.m_sampleType,")")
            return false;
        }
    }

    if ( m_channels != other.m_channels )
    {
        if (m_channels == 0 && other.m_channels > 0)
        {
            m_channels = other.m_channels;
        }
        else
        {
            DE_ERROR("m_channels(",m_channels,") != other.m_channels(",other.m_channels,")")
            return false;
        }
    }

    if ( m_sampleRate != other.m_sampleRate )
    {
        if (m_sampleRate < 1 && other.m_sampleRate > 0)
        {
            m_sampleRate = other.m_sampleRate;
        }
        else
        {
            DE_WARN("m_sampleRate(",m_sampleRate,") != other.m_sampleRate(",other.m_sampleRate,")")
        }
    }

    if ( m_flags != other.m_flags )
    {
        DE_WARN("m_flags(",m_flags,") != other.m_flags(",other.m_flags,")")
        m_flags = other.m_flags;
    }

    m_frames += other.m_frames;

    const uint8_t* beg = other.m_samples.data();
    const uint8_t* end = beg + other.byteCount();
    m_samples.insert( m_samples.end(), beg, end );

    return true;
}



int64_t Planar::read_loop_frames(
    SampleTypeConverter::Converter_t converter,
    void* __restrict__ dst,
    int64_t frameCount,
    int64_t frameStart) const
{
    const int64_t avail = m_frames - frameStart;
    if (avail < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t maxFrames = std::min(avail, frameCount);
    if (maxFrames < 1)
    {
        return 0; // Nothing todo...
    }

    const int32_t bpp = bytesPerSample();
    const int64_t byteCount = maxFrames * m_channels * bpp;
    const int64_t byteIndex = frameStart * m_channels * bpp;

    // <debug>
    if (byteCount < 1)
    {
        DE_ERROR("Got byteCount(",byteCount,")")
        return 0;
    }

    if (byteIndex >= m_samples.size())
    {
        //DE_ERROR("byteIndex(",byteIndex,") > m_samples(",m_samples.size(),")")
        return 0;
    }

    if (byteIndex + byteCount > m_samples.size())
    {
        //DE_ERROR("byteIndex(",byteIndex,") + byteCount(",byteCount,") > m_samples(",m_samples.size(),")")
        return 0;
    }
    // </debug>

    const uint8_t* __restrict__ src = m_samples.data() + byteIndex;

    converter(src, dst, maxFrames * m_channels);

    return maxFrames;
}
*/

} // end namespace audio.
} // end namespace de.

