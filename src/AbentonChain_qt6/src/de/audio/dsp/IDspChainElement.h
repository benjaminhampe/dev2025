#pragma once
#include <de/audio/dsp/DspUtils.h>

namespace de {
namespace audio {

// ============================
struct DspInitParams // 16 bytes
// ============================
{
    u64 frames;
    u32 channels;
    u32 sampleRate;
};

// ============================
struct DspReadParams // 32 bytes
// ============================
{
    f64 pts;
    u32 frames;
    u32 sampleRate;
    f32* __restrict__ L;
    f32* __restrict__ R;
};

// ============================
struct IDspChainElement
// ============================
{
    virtual ~IDspChainElement() {}

    virtual void dsp_init(u64 frames, u32 channels, u32 sampleRate) = 0;

    virtual void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R ) = 0;

    virtual u32 dsp_getInputSignalCount() const = 0;

    virtual IDspChainElement* dsp_getInputSignal(int i = 0) = 0;

    virtual void dsp_setInputSignal(IDspChainElement* input, int i = 0) = 0;

    virtual void dsp_clearInputSignals() = 0;
};

// ============================
struct SampleMinMax
// ============================
{
/*
    // ==============================================
    // Feed (L+R) AudioLevelMeter
    // ==============================================
    m_Lmin = std::numeric_limits< float >::max();
    m_Lmax = std::numeric_limits< float >::lowest();
    m_Rmin = std::numeric_limits< float >::max();
    m_Rmax = std::numeric_limits< float >::lowest();
    if ( dstChannels == 2 )   // Fast interleaved stereo path O(1) = one loop over samples
    {
        float const* pSrc = dst;
        for ( uint64_t i = 0; i < dstFrames; ++i )
        {
            float L = *pSrc++;
            float R = *pSrc++;
            m_Lmin = std::min( m_Lmin, L );
            m_Lmax = std::max( m_Lmax, R );
            m_Rmin = std::min( m_Rmin, L );
            m_Rmax = std::max( m_Rmax, R );
        }
    }
    else   // Slower path O(N) = one loop for each channel of N channels.
    {
        float const* pSrc = dst;
        for ( uint64_t i = 0; i < dstFrames; ++i )
        {
            float sample = *pSrc;
            m_Lmin = std::min( m_Lmin, sample );
            m_Lmax = std::max( m_Lmax, sample );
            pSrc += dstChannels;
        }

        if ( dstChannels > 1 )
        {
            pSrc = dst + 1;
            for ( uint64_t i = 0; i < dstFrames; ++i )
            {
                float sample = *pSrc;
                m_Rmin = std::min( m_Rmin, sample );
                m_Rmax = std::max( m_Rmax, sample );
                pSrc += dstChannels;
            }
        }
    }
*/
};

} // end namespace audio.
} // end namespace de.
