#include <de/sound/Sound.h>

namespace de {

bool Sound::empty() const
{
    if (m_frames < 1) { return true; }
    if (m_channels < 1) { return true; }
    if (m_sampleRate < 1) { return true; }
    if (m_sampleType == ST_Unknown) { return true; }
    return false;
}

double Sound::duration() const // In [s] seconds.
{
    if (m_sampleRate < 1)
    {
        DE_ERROR("Invalid sampleRate ",str())
        return 0.0;
    }

    return double(m_frames) / double(m_sampleRate);
}

int Sound::getBytesPerSample() const
{
    switch (m_sampleType)
    {
        case ST_S8: return 1;
        case ST_S16: return 2;
        case ST_S24: return 3;
        case ST_S32: return 4;
        case ST_F32: return 4;
        case ST_F64: return 8;
        default:
        {
            DE_ERROR("Invalid sampleType ",getSampleTypeStr(m_sampleType))
            return 0;
        }
    }
}

std::string Sound::str(bool bWithUri) const
{
    std::ostringstream o;
    o << dbStrSeconds(duration()) << ","
    " " << m_channels << "x"
    " " << m_sampleRate << "Hz,"
    " fc(" << m_frames << "),"
    " st(" << getSampleTypeStr(m_sampleType) << ")";
    if (bWithUri)
    {
        o << ", uri(" << m_uri << ")";
    }
    return o.str();
}

// static
std::string Sound::getSampleTypeStr(int sampleType)
{
    switch (sampleType)
    {
        case ST_S8: return "ST_S8";
        case ST_S16: return "ST_S16";
        case ST_S24: return "ST_S24";
        case ST_S32: return "ST_S32";
        case ST_F32: return "ST_F32";
        case ST_F64: return "ST_F64";
        default: return "ST_Unknown";
    }
}

int64_t Sound::read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const
{
    if (frameIndex >= m_frames)
    {
        DE_ERROR("frameIndex(",frameIndex,") already at end(",m_frames,")")
        return 0;
    }

    const int64_t avail = m_frames - frameIndex;
    if (avail < 1)
    {
        return 0;
    }

    const int64_t maxframes = std::min(avail,frameCount);

    const int64_t byteCount = maxframes * m_channels * sizeof(float);
    if (byteCount < 1)
    {
        DE_ERROR("Got byteCount(",byteCount,")")
        return 0;
    }

    const int64_t byteIndex = frameIndex * m_channels * sizeof(float);
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

    const uint8_t* __restrict__ src = m_samples.data() + byteIndex;

    DE_ASSUME_NO_OVERLAP(src, dst, byteCount);

    std::memcpy(dst,src,byteCount);

    return maxframes;
}

bool Sound::validate() const
{
    int64_t expected = m_frames * m_channels * getBytesPerSample();
    if ( expected != m_samples.size() )
    {
        DE_ERROR("expected(",expected,") != m_samples(",m_samples.size(),")")
        return false;
    }
    return true;
}

/*
float Sound::maximum() const
{
    float maxv = 0.0f;

    auto p = reinterpret_cast<const float*>(samples.data());

    for (size_t i = 0; i < frame_count * channels; ++i)
    {
        float s = *p++;
        maxv = std::max(maxv, fabs(s));
    }
    return maxv;
}
*/

} // end namespace de.

