#pragma once
#include <de/Core.h>
#include <de/AlignedMemory.h>

namespace de {

// ===========================================================================
class Sound
// ===========================================================================
{
public:
    enum eSampleType
    {
        ST_Unknown = 0, ST_S8, ST_S16, ST_S24, ST_S32, ST_F32, ST_F64
    };

    uint64_t m_frameCount = 0;
    uint32_t m_sampleRate = 0;
    uint16_t m_sampleType = 0;
    uint16_t m_channelCount = 0;
    double m_duration = 0; // In seconds

    TAlignedVector<float> m_samples; // Interleaved

    std::string m_uri;

    double duration() const
    {
        if (m_sampleRate < 1)
        {
            return 0.0;
        }

        return double(m_frameCount) / double(m_sampleRate);
    }

    std::string str() const
    {
        std::ostringstream o; o <<
        dbStrSeconds(duration()) << ", "
        << m_channelCount << "x "
        << m_sampleRate << "Hz, "
        //"dur(" << dbStrSeconds(durationInSec()) << ")"
        //"ch(" << channelCount << "), "
        //"sr(" << sampleRate << "Hz), "
        "fc(" << m_frameCount << "), "
        "st(" << getString((eSampleType)m_sampleType) << "), "
        ;
        return o.str();
    }

    static std::string getString(eSampleType sampleType)
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

public:
/*
    float maximum() const
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

};

} // end namespace de.
