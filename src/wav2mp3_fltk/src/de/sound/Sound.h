#pragma once
#include <de/sound/SampleTypeConverter.h>

namespace de {

// ===========================================================================
class Sound
// ===========================================================================
{
public:
    int64_t m_frames = 0;
    int32_t m_sampleRate = 0;
    int16_t m_channels = 0;
    SampleType m_sampleType = SampleType::Unknown;
    uint8_t m_flags = 0; // 0 = interleaved, 1 = planar
    TAlignedVector<uint8_t> m_samples; // Interleaved
    std::string m_uri;

    bool empty() const noexcept;

    void clear() noexcept;

    double duration() const noexcept; // In [s] seconds.

    const uint8_t* data() const noexcept { return m_samples.data(); }

    uint8_t* data() noexcept { return m_samples.data(); }

    int64_t size() const noexcept { return m_samples.size(); }

    int64_t byteCount() const noexcept;

    int64_t sampleCount() const noexcept;

    int32_t bytesPerSample() const noexcept;

    int64_t memoryConsumption() const noexcept;

    SampleType sampleType() const noexcept;

    std::string sampleTypeStr() const;

    std::string str(bool bWithUri = false) const;

    int64_t read_frames(void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const;

    int64_t read_frames_convert(SampleType dstType, void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const;

    int64_t read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const;

    bool validate() const;

    void allocFrames( int64_t frames );

    bool append(const Sound& other);

public:
/*
    float maximum() const;
*/

};


struct SoundSaveOptions
{
    int bitrate = 128; // in kB/s, mp3 needs more then opus
    int quality = 0; // 0 = highest, 9 = lowest

    SampleType sampleType = SampleType::Unknown; // Force an output format.

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
