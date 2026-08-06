#include <de/audio/planar/PlanarUtil.h>

namespace de {
namespace audio {

// static
int64_t PlanarUtil::copy( const Sound& src, Planar& dst, int64_t frameCount, int64_t frameStart)
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
    dst.m_flags = 1;
    dst.allocFrames(maxFrames);

    // Copy + Deinterleave

    const int32_t srcBPP = dst.bytesPerSample();

    for (int32_t c = 0; c < src.channels(); ++c)
    {
        uint8_t* __restrict__ pDst = dst.m_samples[c].data();

        for (int64_t f = 0; f < maxFrames; ++f)
        {
            const int64_t byteStart = frameStart * src.m_channels * srcBPP;
            const uint8_t* __restrict__ pSrc = src.m_samples.data() + byteStart;

            std::memcpy(pDst, pSrc, srcBPP);
            pDst++;
        }
    }

    return maxFrames;
}

} // end namespace sound.
} // end namespace de.

