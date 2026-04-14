#pragma once
#include <de/de_aligned_memory.h>
#include <cstdint>
#include <cstring> // for memcpy
#include <vector>
#include <atomic>

namespace de {
namespace audio {

class AudioRingBuffer
{
private:
    size_t m_bufferSize;
    int m_frames;
    int m_channels;
    std::atomic<int> m_writeIndex;
    std::atomic<int> m_readIndex;
    TAlignedVector<float> m_buffer; // AVX2 aligned 32 bytes.
public:
    AudioRingBuffer()
        : m_bufferSize{ 0 }
        , m_frames{ 0 }
        , m_channels{ 0 }
        , m_writeIndex{ 0 }
        , m_readIndex{ 0 }
    {}

    // ~AudioRingBuffer() {}

    void resize(u32 frames, u32 channels)
    {
        if ((frames == m_frames) && (channels == m_channels))
        {
            return; // Nothing todo.
        }

        m_bufferSize = static_cast<size_t>(frames) * channels;
        m_buffer.resize(m_bufferSize);
        m_frames = frames;
        m_channels = channels;
        m_writeIndex = 0;
        m_readIndex = 0;

        DE_OK("frames(",m_frames,"), channels(",m_channels,")")
    }
    // How many frames can be written without overwriting unread data
    int getWritableFrames() const
    {
        int w = m_writeIndex.load(std::memory_order_acquire);
        int r = m_readIndex.load(std::memory_order_acquire);

        if (w >= r)
            return m_frames - (w - r) - 1; // leave one frame gap
        else
            return (r - w) - 1;
    }

    // How many frames are available to read
    int getReadableFrames() const
    {
        int w = m_writeIndex.load(std::memory_order_acquire);
        int r = m_readIndex.load(std::memory_order_acquire);

        if (w >= r)
            return w - r;
        else
            return m_frames - (r - w);
    }

    // Write interleaved frames: input samples = frames * channels
    bool write(const float* __restrict__ input, int frames)
    {
        if (frames <= 0)
            return true;

        if (getWritableFrames() < frames)
            return false; // not enough space

        int w = m_writeIndex.load(std::memory_order_relaxed);

        int framesToEnd = m_frames - w;
        int firstPart = (frames <= framesToEnd) ? frames : framesToEnd;
        int secondPart = frames - firstPart;

        // copy first part
        std::memcpy(
            m_buffer.data() + (size_t)w * m_channels,
            input,
            (size_t)firstPart * m_channels * sizeof(float));

        // wrap and copy second part
        if (secondPart > 0)
        {
            std::memcpy(
                m_buffer.data(),
                input + (size_t)firstPart * m_channels,
                (size_t)secondPart * m_channels * sizeof(float));
        }

        int newWrite = (w + frames) % m_frames;
        m_writeIndex.store(newWrite, std::memory_order_release);
        return true;
    }

    // Read interleaved frames: output samples = frames * channels
    bool read(float* __restrict__ output, int frames)
    {
        if (frames <= 0)
            return true;

        if (getReadableFrames() < frames)
            return false; // not enough data

        int r = m_readIndex.load(std::memory_order_relaxed);

        int framesToEnd = m_frames - r;
        int firstPart = (frames <= framesToEnd) ? frames : framesToEnd;
        int secondPart = frames - firstPart;

        // copy first part
        std::memcpy(
            output,
            m_buffer.data() + (size_t)r * m_channels,
            (size_t)firstPart * m_channels * sizeof(float));

        // wrap and copy second part
        if (secondPart > 0)
        {
            std::memcpy(
                output + (size_t)firstPart * m_channels,
                m_buffer.data(),
                (size_t)secondPart * m_channels * sizeof(float));
        }

        int newRead = (r + frames) % m_frames;
        m_readIndex.store(newRead, std::memory_order_release);
        return true;
    }
};

} // end namespace audio.
} // end namespace de.
