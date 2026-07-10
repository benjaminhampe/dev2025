#pragma once
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

struct StereoAudioFifo 
{
    TAlignedVector<float> m_L;
    TAlignedVector<float> m_R;
    uint64_t m_readPos = 0;
    uint64_t m_writePos = 0;
    uint64_t m_size = 0;

    StereoAudioFifo(uint64_t capacity)
    {
        m_L.resize(capacity);
        m_R.resize(capacity);
        m_size = capacity;
    }

    uint64_t available() const
    {
        return (m_writePos + m_size - m_readPos) % m_size;
    }

    uint64_t freeSpace() const
    {
        return m_size - 1 - available();
    }

    void push(const float* __restrict__ Lin, const float* __restrict__ Rin, uint64_t n)
    {
        float* __restrict__ Lout = m_L.data();
        float* __restrict__ Rout = m_R.data();

        DE_ASSUME_NO_OVERLAP(Lin,Lout,n*sizeof(float));
        DE_ASSUME_NO_OVERLAP(Rin,Rout,n*sizeof(float));

        for (uint64_t i = 0; i < n; ++i)
        {
            Lout[m_writePos] = Lin[i];
            Rout[m_writePos] = Rin[i];
            m_writePos = (m_writePos + 1) % m_size;
        }
    }

    void pop(float* __restrict__ Lout, float* __restrict__ Rout, uint64_t n)
    {
        const float* __restrict__ Lin = m_L.data();
        const float* __restrict__ Rin = m_R.data();

        DE_ASSUME_NO_OVERLAP(Lin,Lout,n*sizeof(float));
        DE_ASSUME_NO_OVERLAP(Rin,Rout,n*sizeof(float));

        for (uint64_t i = 0; i < n; ++i)
        {
            Lout[i] = Lin[m_readPos];
            Rout[i] = Rin[m_readPos];
            m_readPos = (m_readPos + 1) % m_size;
        }
    }
};

} // end namespace audio.
} // end namespace de.
