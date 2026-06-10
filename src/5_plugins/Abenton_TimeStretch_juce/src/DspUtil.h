#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

struct AudioFifo
{
    std::vector<float> m_samples;
    size_t m_capacity;
    size_t m_readPos;
    size_t m_writePos;

    AudioFifo(size_t capacity)
    {
        m_samples.resize(capacity);
        m_capacity = capacity;
        m_readPos = 0;
        m_writePos = 0;
    }

    size_t available() const {
        return (m_writePos + m_capacity - m_readPos) % m_capacity;
    }

    size_t freeSpace() const {
        return m_capacity - 1 - available();
    }

    void push(const float* __restrict__ src, size_t n)
    {
        for (size_t i = 0; i < n; ++i)
        {
            m_samples[m_writePos] = src[i];
            m_writePos = (m_writePos + 1) % m_capacity;
        }
    }

    void pop(float* __restrict__ dst, size_t n)
    {
        for (size_t i = 0; i < n; ++i)
        {
            dst[i] = m_samples[m_readPos];
            m_readPos = (m_readPos + 1) % m_capacity;
        }
    }
};

} // end namespace audio
} // end namespace de
