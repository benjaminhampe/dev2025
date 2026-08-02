#include <de/sound/Sound.h>

namespace de {

bool Sound::empty() const noexcept
{
    if (m_frames < 1) { return true; }
    if (m_channels < 1) { return true; }
    if (m_sampleRate < 1) { return true; }
    if (m_sampleType == SampleType::Unknown) { return true; }
    return false;
}

void Sound::clear() noexcept
{
    m_frames = 0;
    m_channels = 0;
    m_sampleRate = 0;
    m_sampleType = SampleType::Unknown;
    m_flags = 0;
    m_samples.clear();
}

void Sound::shrink_to_fit() noexcept
{
    clear();
    m_samples.shrink_to_fit();
}

double Sound::duration() const noexcept // In [s] seconds.
{
    if (m_sampleRate < 1)
    {
        DE_ERROR("Invalid sampleRate ",str())
        return 0.0;
    }

    return double(m_frames) / double(m_sampleRate);
}

int64_t Sound::sampleCount() const noexcept
{
    return m_frames * m_channels;
}

int64_t Sound::byteCount() const noexcept
{
    return m_frames * m_channels * bytesPerSample();
}

int32_t Sound::bytesPerSample() const noexcept
{
    return m_sampleType.bytesPerSample();
}

SampleType Sound::sampleType() const noexcept
{
    return m_sampleType;
}

std::string Sound::sampleTypeStr() const
{
    return m_sampleType.str();
}

int64_t Sound::memoryConsumption() const noexcept
{
    int64_t n = sizeof(*this);
    n += m_samples.capacity();
    return n;
}

bool Sound::validate() const
{
    const int64_t header = byteCount();
    const int64_t memory = m_samples.size();
    const int64_t delta = header - memory;
    const int64_t pc = 100.0 * double(delta) / double(memory);
    if ( delta != 0 )
    {
        DE_ERROR("header(",header,") != memory(",memory,"), delta(",delta,"), percent(",pc,")")
        return false;
    }
    return true;
}

void Sound::allocFrames( int64_t frames )
{
    m_frames = frames;
    m_samples.resize( frames * m_channels * bytesPerSample() );
}

bool Sound::append(const Sound& other)
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

std::string Sound::str(bool bWithFileName, bool bWithDir) const
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

int64_t Sound::read_frames(void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const
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

int64_t Sound::read_frames(
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

/*
int64_t Sound::read_frames_convert(SampleType dstType, void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const
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

    auto converter = SampleTypeConverter::getConverter(m_sampleType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter")
        return 0;
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
        DE_ERROR("byteIndex(",byteIndex,") > m_samples(",m_samples.size(),")")
        return 0;
    }

    if (byteIndex + byteCount >= m_samples.size())
    {
        DE_ERROR("byteIndex(",byteIndex,") + byteCount(",byteCount,") > m_samples(",m_samples.size(),")")
        return 0;
    }
    // </debug>

    const uint8_t* __restrict__ src = m_samples.data() + byteIndex;

    converter(src, dst, maxFrames * m_channels);

    return maxFrames;
}

int64_t Sound::read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const
{
    const int64_t avail = m_frames - frameIndex;
    if (avail < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t maxFrames = std::min(avail,frameCount);
    if (maxFrames < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t byteCount = maxFrames * m_channels * sizeof(float);
    const int64_t byteIndex = frameIndex * m_channels * sizeof(float);

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

    DE_ASSUME_NO_OVERLAP(src, dst, byteCount);

    std::memcpy(dst,src,byteCount);

    return maxFrames;
}
*/

} // end namespace de.


/*
    static void deinterleave(
            int32_t srcChannels,
            SampleType srcType,
            const TAlignedVector<uint8_t>& srcSamples,
            TAlignedVector<uint8_t>& tmpSamples,
            SampleType dstType,
            TAlignedVector<uint8_t>& dstSamples,
            int64_t frameCount,
            int64_t frameStart = 0);


void Sound::deinterleave(
        int32_t channels,
        SampleType srcType,
        const TAlignedVector<uint8_t>& srcSamples,
        TAlignedVector<uint8_t>& tmpSamples,
        SampleType dstType,
        TAlignedVector<uint8_t>& dstSamples,
        int64_t frameCount,
        int64_t frameStart)
{
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        return;
    }

    const int32_t dstBPP = SampleType::getBytesPerSample(dstType);
    const int64_t dstByteCount = frameCount * channels * dstBPP;
    if (dstByteCount < 1)
    {
        DE_WARN("Nothing todo")
        return;
    }

    tmpSamples.resize(dstByteCount);
    dstSamples.resize(dstByteCount);

    // ConvertOnce: src -> tmp (still interleaved)
    {
        const int32_t srcBPP = SampleType::getBytesPerSample(srcType);
        const int64_t srcByteStart = frameStart * channels * srcBPP;
        const int64_t srcByteCount = frameCount * channels * srcBPP;
        if (srcByteStart >= srcSamples.size())
        {
            DE_WARN("EOB")
            return;
        }

        if (srcByteStart + srcByteCount > srcSamples.size())
        {
            DE_WARN("EOB II")
            return;
        }

        const uint8_t* __restrict__ pSrc = srcSamples.data() + srcByteStart;

        uint8_t* __restrict__ pDst = tmpSamples.data();

        const int64_t sampleCount = frameCount * channels;

        converter(pSrc,pDst,sampleCount);
    }

    // Deinterleave: tmp -> dst
    const int64_t dstBytesPerChannel = frameCount * dstBPP;
    const int32_t dstStride = channels * dstBPP;

    // ForEach(channel):
    for (int32_t ch = 0; ch < channels; ++ch)
    {
        // Move to first sample of tmpBuffer of current channel
        const uint8_t* __restrict__ pSrc =
            tmpSamples.data() + dstBPP * ch;

        // Move to first sample of dstBuffer of current channel
        uint8_t* __restrict__ pDst =
            dstSamples.data() + dstBytesPerChannel * ch;

        // ForEach(frame) in current channel
        for (int64_t i = 0; i < frameCount; ++i)
        {
            std::memcpy(pDst, pSrc, dstBPP); // Copy Sample
            pSrc += dstStride;  // Advance to next interleaved sample of same channel
            pDst += dstBPP;     // Advance to next planar sample of same channel
        }
    }
}

*/
