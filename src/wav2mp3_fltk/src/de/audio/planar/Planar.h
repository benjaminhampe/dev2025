#pragma once
#include <de/sound/SampleTypeConverter.h>

namespace de {
namespace audio {

// =================================================================
class Planar
// =================================================================
{
public:
    int64_t m_frames = 0;
    int32_t m_sampleRate = 0;
    int16_t m_channels = 0;
    SampleType m_sampleType = SampleType::Unknown;
    uint8_t m_flags = 1; // 0 = interleaved, 1 = planar
    TAlignedVector<TAlignedVector<uint8_t>> m_samples;
    std::string m_uri;

    bool empty() const noexcept;

    void clear() noexcept;

    void shrink_to_fit() noexcept;

    double duration() const noexcept; // In [s] seconds.

    int32_t channels() const noexcept { return m_samples.size(); }

    // int64_t bytesPerChannel() const noexcept;

    int32_t sampleRate() const noexcept { return m_sampleRate; }

    int64_t frames() const noexcept { return m_frames; }

    const uint8_t* data(int32_t channel) const noexcept { return m_samples[channel].data(); }

    uint8_t* data(int32_t channel) noexcept { return m_samples[channel].data(); }

    int64_t size(int32_t channel) const noexcept { return m_samples[channel].size(); }

    int64_t sampleCount() const noexcept;

    int32_t bytesPerSample() const noexcept;

    int64_t byteCount() const noexcept;

    int64_t memoryConsumption() const noexcept;

    SampleType sampleType() const noexcept;

    std::string sampleTypeStr() const;

    std::string str(bool bWithFileName = false, bool bWithDir = false) const;

    int64_t read_frames(
            SampleTypeConverter::Converter_t converter,
            uint8_t* __restrict__ L,
            uint8_t* __restrict__ R,
            int64_t frameCount,
            int64_t frameStart = 0) const;

    // int64_t read_frames(
            // void** __restrict__ dst,
            // int64_t frameCount,
            // int64_t frameStart = 0) const;

    // int64_t read_frames_convert(SampleType dstType, void* __restrict__ dst, int64_t frameCount, int64_t frameStart) const;

    // int64_t read_frames_f32(float* __restrict__ dst, int64_t frameCount, int64_t frameIndex) const;

    bool validate() const;

    void allocFrames( int64_t frames );

    // bool append(const Sound& other);

    // Makes mono to stereo by duplicating channel
    // Deletes all channels until only 2 remain.
    // void forceStereo();

};

} // end namespace audio.
} // end namespace de.
