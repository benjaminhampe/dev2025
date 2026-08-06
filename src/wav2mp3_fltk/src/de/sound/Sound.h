#pragma once
#include <de/sound/SampleTypeConverter.h>

namespace de {

// =================================================================
class Sound
// =================================================================
{
public:
    int64_t m_frames = 0;
    int32_t m_sampleRate = 0;
    int16_t m_channels = 0;
    SampleType m_sampleType = SampleType::Unknown;

    enum eFlags {
        Interleaved = 0,
        Planar = 1,
        Looping = 2
    };
    uint8_t m_flags = 0; // 0 = interleaved, 1 = planar
    TAlignedVector<uint8_t> m_samples; // Interleaved
    std::string m_uri;

    bool empty() const noexcept;

    void clear() noexcept;
    void shrink_to_fit() noexcept;

    double duration() const noexcept; // In [s] seconds.

    int32_t channels() const noexcept { return m_channels; }

    // int64_t bytesPerChannel() const noexcept;

    int32_t sampleRate() const noexcept { return m_sampleRate; }

    int64_t frames() const noexcept { return m_frames; }

    const uint8_t* data() const noexcept { return m_samples.data(); }

    uint8_t* data() noexcept { return m_samples.data(); }

    int64_t size() const noexcept { return m_samples.size(); }

    int64_t sampleCount() const noexcept;

    int32_t bytesPerSample() const noexcept;

    int64_t byteCount() const noexcept;

    int64_t memoryConsumption() const noexcept;

    SampleType sampleType() const noexcept;

    std::string sampleTypeStr() const;

    std::string str(bool bWithFileName = false, bool bWithDir = false) const;

    // int64_t read_frames(
    //         void* __restrict__ dst,
    //         int64_t frameCount,
    //         int64_t frameStart = 0) const;

    int64_t read_frames(
            SampleTypeConverter::Converter_t converter,
            void* __restrict__ dst,
            int64_t frameCount,
            int64_t frameStart = 0) const;

    // int64_t read_frames_convert(SampleType dstType, void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const;

    // int64_t read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const;

    bool validate() const;

    void allocFrames( int64_t frames );

    bool append(const Sound& other);
};

// =================================================================
struct SoundLoadOptions
// =================================================================
{
    SampleType sampleType = SampleType::Unknown; // Force an output format.

    bool bDebug = false;
    bool bThrowOnFail = false;

    std::atomic<bool>* bCancelFlag = nullptr;

    std::function<void (int)>
        onProgress = [](int percent){};

    // 0 = OK/Success
    std::function<void (int)>
        onFinish = [](int errorCode){};

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

// =================================================================
struct SoundSaveOptions
// =================================================================
{
    int bitrate = 128; // in kilobit/s, mp3 needs more then opus
    int quality = 0; // 0 = highest, 9 = lowest
    int vbr = 0;    // 0 = disabled, 1 = enabled

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
