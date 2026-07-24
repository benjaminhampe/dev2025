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

    int64_t m_frames = 0;
    int32_t m_sampleRate = 0;
    int16_t m_sampleType = 0;
    int16_t m_channels = 0;
    TAlignedVector<uint8_t> m_samples; // Interleaved
    std::string m_uri;

    bool empty() const;

    double duration() const; // In [s] seconds.

    int getBytesPerSample() const;

    std::string str(bool bWithUri = false) const;

    static std::string getSampleTypeStr(int sampleType);

    int64_t read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const;

    bool validate() const;

public:
/*
    float maximum() const;
*/

};


struct SoundSaveOptions
{
    int bitrate = 128; // in kB/s, mp3 needs more then opus
    int quality = 0; // 0 = highest, 9 = lowest

    bool bDebug = false;
    bool bThrowOnFail = false;

    std::atomic<bool>* bCancelFlag = nullptr;

    std::function<void (int)>
        onProgress = [](int percent){};

    enum eLogLevel
    {
        Debug = 0,
        Ok,
        Warn,
        Error
    };

    std::function<void (int,std::string)>
        onLogger = [](int level, std::string msg){};

};

} // end namespace de.
