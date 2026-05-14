#pragma once
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

struct FileInfo
{
    enum eSampleType
    {
        ST_Unknown = 0, ST_S8, ST_S16, ST_S24, ST_S32, ST_F32, ST_F64
    };
    
    u64 frameCount = 0;
    u32 sampleRate = 0;
    u16 sampleType = 0;
    u16 channelCount = 0;
    u64 duration = 0; // In nanoseconds
    
    double durationInSec() const
    {
        if (sampleRate < 1)
        {
            return 0.0;
        }

        return double(frameCount) / double(sampleRate);
    }

    std::string str() const
    {
        std::ostringstream o; o <<
        dbStrSeconds(durationInSec()) << ", "
        << channelCount << "x "
        << sampleRate << "Hz, "
        //"dur(" << dbStrSeconds(durationInSec()) << ")"
        //"ch(" << channelCount << "), "
        //"sr(" << sampleRate << "Hz), "
        "fc(" << frameCount << "), "
        "st(" << getString((eSampleType)sampleType) << "), "
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

};

} // end namespace audio.
} // end namespace de.
