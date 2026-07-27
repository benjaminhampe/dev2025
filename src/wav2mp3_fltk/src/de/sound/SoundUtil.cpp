#include <de/sound/SoundUtil.h>

namespace de {
namespace sound {

// static
void SoundUtil::byteSwap16_inplace(void* __restrict__ p, int64_t nElements)
{
    uint8_t b[2];

    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(p);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(p);

    for (int64_t i = 0; i < nElements; ++i)
    {
        b[0] = pSrc[0];
        b[1] = pSrc[1];
        pDst[0] = b[1];
        pDst[1] = b[0];
        pSrc += 2;
        pDst += 2;
    }
}

// static
void SoundUtil::byteSwap24_inplace(void* __restrict__ p, int64_t nElements)
{
    uint8_t b[3];

    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(p);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(p);

    for (int64_t i = 0; i < nElements; ++i)
    {
        b[0] = pSrc[0];
        b[1] = pSrc[1];
        b[2] = pSrc[2];
        pDst[0] = b[2];
        pDst[1] = b[1];
        pDst[2] = b[0];
        pSrc += 3;
        pDst += 3;
    }
}

// static
void SoundUtil::byteSwap32_inplace(void* __restrict__ p, int64_t nElements)
{
    uint8_t b[4];

    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(p);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(p);

    for (int64_t i = 0; i < nElements; ++i)
    {
        b[0] = pSrc[0];
        b[1] = pSrc[1];
        b[2] = pSrc[2];
        b[3] = pSrc[3];
        pDst[0] = b[3];
        pDst[1] = b[2];
        pDst[2] = b[1];
        pDst[3] = b[0];
        pSrc += 4;
        pDst += 4;
    }
}

// static
void SoundUtil::byteSwap64_inplace(void* __restrict__ p, int64_t nElements)
{
    uint8_t b[8];

    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(p);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(p);

    for (int64_t i = 0; i < nElements; ++i)
    {
        b[0] = pSrc[0];
        b[1] = pSrc[1];
        b[2] = pSrc[2];
        b[3] = pSrc[3];
        b[4] = pSrc[4];
        b[5] = pSrc[5];
        b[6] = pSrc[6];
        b[7] = pSrc[7];
        pDst[0] = b[7];
        pDst[1] = b[6];
        pDst[2] = b[5];
        pDst[3] = b[4];
        pDst[4] = b[3];
        pDst[5] = b[2];
        pDst[6] = b[1];
        pDst[7] = b[0];
        pSrc += 8;
        pDst += 8;
    }
}

// static
void SoundUtil::byteSwap16(const void* __restrict__ src, void* __restrict__ dst, int64_t nElements)
{
    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(src);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(dst);

    for (int64_t i = 0; i < nElements; ++i)
    {
        pDst[0] = pSrc[1];
        pDst[1] = pSrc[0];
        pSrc += 2;
        pDst += 2;
    }
}

// static
void SoundUtil::byteSwap24(const void* __restrict__ src, void* __restrict__ dst, int64_t nElements)
{
    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(src);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(dst);

    for (int64_t i = 0; i < nElements; ++i)
    {
        pDst[0] = pSrc[2];
        pDst[1] = pSrc[1];
        pDst[2] = pSrc[0];
        pSrc += 3;
        pDst += 3;
    }
}

// static
void SoundUtil::byteSwap32(const void* __restrict__ src, void* __restrict__ dst, int64_t nElements)
{
    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(src);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(dst);

    for (int64_t i = 0; i < nElements; ++i)
    {
        pDst[0] = pSrc[3];
        pDst[1] = pSrc[2];
        pDst[2] = pSrc[1];
        pDst[3] = pSrc[0];
        pSrc += 4;
        pDst += 4;
    }
}

// static
void SoundUtil::byteSwap64(const void* __restrict__ src, void* __restrict__ dst, int64_t nElements)
{
    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(src);
          uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(dst);

    for (int64_t i = 0; i < nElements; ++i)
    {
        pDst[0] = pSrc[7];
        pDst[1] = pSrc[6];
        pDst[2] = pSrc[5];
        pDst[3] = pSrc[4];
        pDst[4] = pSrc[3];
        pDst[5] = pSrc[2];
        pDst[6] = pSrc[1];
        pDst[7] = pSrc[0];
        pSrc += 8;
        pDst += 8;
    }
}

// static
int64_t SoundUtil::copy( const Sound& src, Sound& dst, int64_t frameCount, int64_t frameStart)
{
    const int64_t avail = src.m_frames - frameStart;
    if (avail < 1)
    {
        return 0; // Nothing todo...
    }

    const int64_t maxFrames = std::min(avail, frameCount);
    if (maxFrames < 1)
    {
        return 0; // Nothing todo...
    }

    dst.m_sampleType = src.m_sampleType;
    dst.m_sampleRate = src.m_sampleRate;
    dst.m_channels = src.m_channels;
    dst.m_flags = src.m_flags;
    dst.m_frames = maxFrames;

    const int32_t bpp = dst.bytesPerSample();
    const int64_t dstByteCount = maxFrames * dst.m_channels * bpp;
    const int64_t srcByteStart = frameStart * src.m_channels * bpp;

    dst.m_samples.resize(dstByteCount);

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

    const uint8_t* __restrict__ pSrc = src.m_samples.data() + srcByteStart;

          uint8_t* __restrict__ pDst = dst.m_samples.data();

    std::memcpy(pDst, pSrc, dstByteCount);

    return maxFrames;
}

// static
int64_t
SoundUtil::convert( const Sound& src, Sound& dst, SampleType dstType)
{
    if (src.empty())
    {
        DE_WARN("empty")
        dst.clear();
        return 0;
    }

    if (dstType == SampleType::Unknown)
    {
        DE_WARN("fallback to copy")
        return copy(src, dst, src.m_frames, 0);
    }

    auto srcType = src.m_sampleType;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter")
        return 0;
    }

    dst.m_sampleType = dstType;
    dst.m_sampleRate = src.m_sampleRate;
    dst.m_channels = src.m_channels;
    dst.m_flags = src.m_flags;
    dst.m_frames = src.m_frames;
    dst.m_samples.resize( dst.byteCount() );

    {
        const uint8_t* __restrict__ pSrc = src.m_samples.data();
              uint8_t* __restrict__ pDst = dst.m_samples.data();

        converter(pSrc,pDst,src.sampleCount());
    }

    return dst.m_frames;
}

//🔥Deinterleave (interleaved → planar)

// pSrc = src + ch*bpp
// pDst = dst + ch*channelStride

// pSrc += frameStride
// pDst += bpp

// static
int64_t
SoundUtil::deinterleave( const Sound& src, Sound& dst )
{
    if (src.empty())
    {
        DE_WARN("empty")
        dst.clear();
        return 0;
    }

    if (src.m_flags != 0)
    {
        DE_WARN("Not interleaved!")
    }

    dst.m_sampleType = src.m_sampleType;
    dst.m_sampleRate = src.m_sampleRate;
    dst.m_channels = src.m_channels;
    dst.m_frames = src.m_frames;
    dst.m_flags = 1; // Planar flag.
    dst.m_samples.resize( dst.byteCount() );

    // Deinterleave: tmp -> dst
    const int32_t bpp = src.bytesPerSample();
    const int64_t frameStride = src.m_channels * bpp; // how many samples to jump to next sample of same channel
    const int64_t channelStride = src.m_frames * bpp;

    // Move to first sample of dstBuffer of current channel
    // uint8_t* __restrict__ pDst = dst.m_samples.data();

    // ForEach(channel):
    for (int32_t ch = 0; ch < src.m_channels; ++ch)
    {
        // Move to first sample of interleaved srcBuffer of current channel
        const uint8_t* __restrict__ pSrc =
            src.m_samples.data() + (bpp * ch);

        // Move to first sample of planar dstBuffer of current channel
        uint8_t* __restrict__ pDst =
            dst.m_samples.data() + (channelStride * ch);

        // ForEach(frame) in current channel
        for (int64_t i = 0; i < src.m_frames; ++i)
        {
            std::memcpy(pDst, pSrc, bpp); // Copy Sample
            pSrc += frameStride; // Next interleaved sample of same input channel
            pDst += bpp;     // Next planar sample of same output channel
        }
    }

    return src.m_frames;
}

//🔥Interleave (planar → interleaved)

// pSrc = src + ch*channelStride
// pDst = dst + ch*bpp

// pSrc += bpp
// pDst += frameStride

// static
int64_t
SoundUtil::interleave( const Sound& src, Sound& dst )
{
    if (src.empty())
    {
        DE_WARN("empty")
        dst.clear();
        return 0;
    }

    if (src.m_flags != 1)
    {
        DE_WARN("Not planar!")
    }

    dst.m_sampleType = src.m_sampleType;
    dst.m_sampleRate = src.m_sampleRate;
    dst.m_channels = src.m_channels;
    dst.m_frames = src.m_frames;
    dst.m_flags = 0; // Interleaved flag
    dst.m_samples.resize( dst.byteCount() );

    // Deinterleave: tmp -> dst
    const int32_t bpp = src.bytesPerSample();
    const int64_t frameStride = src.m_channels * bpp; // how many samples to jump to next sample of same channel
    const int64_t channelStride = src.m_frames * bpp;

    // Move to first sample of dstBuffer of current channel
    // uint8_t* __restrict__ pDst = dst.m_samples.data();

    // ForEach(channel):
    for (int32_t ch = 0; ch < src.m_channels; ++ch)
    {
        // Move to first sample of planar srcBuffer of current channel
        const uint8_t* __restrict__ pSrc =
            src.m_samples.data() + (channelStride * ch);

        // Move to first sample of interleaved dstBuffer of current channel
        uint8_t* __restrict__ pDst =
            dst.m_samples.data() + (bpp * ch);

        // ForEach(frame) in current channel
        for (int64_t i = 0; i < src.m_frames; ++i)
        {
            std::memcpy(pDst, pSrc, bpp); // Copy Sample
            pSrc += bpp; // Next planar sample of same output channel
            pDst += frameStride; // Next interleaved sample of same input channel
        }
    }

    return src.m_frames;
}

float SoundUtil::mostNegative( const Sound& sound )
{
    if (sound.m_sampleType != SampleType::F32)
    {
        DE_ERROR("Not F32")
        return 0.0;
    }

    float m = 0.0f;

    auto p = reinterpret_cast<const float*>(sound.m_samples.data());

    for (size_t i = 0; i < sound.sampleCount(); ++i)
    {
        float v = *p++;
        if (v < 0.0f) m = std::fmaxf(-v, m);
    }
    return m;
}

float SoundUtil::mostPositive( const Sound& sound )
{
    if (sound.m_sampleType != SampleType::F32)
    {
        DE_ERROR("Not F32")
        return 0.0;
    }

    float m = 0.0f;

    auto p = reinterpret_cast<const float*>(sound.m_samples.data());

    for (size_t i = 0; i < sound.sampleCount(); ++i)
    {
        float v = *p++;
        if (v > 0.0f) m = std::fmaxf(v, m);
    }
    return m;
}

float SoundUtil::maximum( const Sound& sound )
{
    if (sound.m_sampleType != SampleType::F32)
    {
        DE_ERROR("Not F32")
        return 0.0;
    }

    float m = 0.0f;

    auto p = reinterpret_cast<const float*>(sound.m_samples.data());

    for (size_t i = 0; i < sound.sampleCount(); ++i)
    {
        float v = *p++;
        m = std::fmaxf( std::fabsf(v), m);
    }
    return m;
}

} // end namespace sound.
} // end namespace de.



/*

static void
deinterleaveConvert(
    int32_t srcChannels,
    SampleType srcType,
    const TAlignedVector<uint8_t>& srcSamples,
    TAlignedVector<uint8_t>& tmpSamples,
    SampleType dstType,
    TAlignedVector<uint8_t>& dstSamples,
    int64_t maxFrameCount,
    int64_t srcFrameStart = 0);

// static
void
SoundUtil::deinterleaveConvert(
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
