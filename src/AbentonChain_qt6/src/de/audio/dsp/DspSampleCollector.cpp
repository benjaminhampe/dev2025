#include <de/audio/dsp/DspSampleCollector.h>

namespace de {
namespace audio {

// ===================================================================
DspSampleCollector::DspSampleCollector()
    : m_inputSignal{ nullptr }
    , m_cols{ 64 * 8 }
    , m_rows{ 32 }
    , m_windowFunc{ 2 }
    , m_bBypassed{ true }
{
    m_accum.setCallback_onFullVector(
        [&](const DE_AlignedFloatVector& v)
        {
            auto n = v.size();

            // Apply window function
            applyWindow(m_windowFunc, m_fft_input.data(), m_accum.data(), n);

            // Apply fft
            m_fft.fft(m_fft_input.data(), m_fft_output.data(), n);

            m_matrix.push(m_fft_output.data(), m_fft_output.size());
        }
    );
}
void
DspSampleCollector::applyWindow(int winType, float* __restrict__ dst, const float* __restrict__ src, size_t n)
{
    if (winType == 1)
    {
        de::audio::math::apply_blackman(dst, src, n);
    }
    else if (winType == 2)
    {
        de::audio::math::apply_hamming(dst, src, n);
    }
    else if (winType == 3)
    {
        de::audio::math::apply_hann(dst, src, n);
    }
}

void
DspSampleCollector::dsp_setInputSignal( IDspChainElement* inputSignal, int i )
{
    m_inputSignal = inputSignal;
}

void
DspSampleCollector::dsp_init( u64 frames, u32 channels, u32 sampleRate )
{
    if (m_cols < frames / 2)
    {
        m_cols = frames / 2;
        DE_WARN("Limit cols to ", m_cols)
    }
    m_L.resize(frames);
    m_R.resize(frames);
    m_sum.resize(frames);
    m_accum.resize(m_cols*2);
    m_fft_input.resize(m_cols*2);
    m_fft_output.resize(m_cols);
    m_matrix.resize(m_cols, m_rows);

    if (m_inputSignal)
    {
        m_inputSignal->dsp_init(frames, channels, sampleRate);
    }
}

void
DspSampleCollector::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                             f32* __restrict__ L, f32* __restrict__ R)
{
    if (m_cols < frames / 2)
    {
        m_cols = frames / 2;
        DE_WARN("Limit cols to ", m_cols)
    }
    m_L.resize(frames);
    m_R.resize(frames);
    m_sum.resize(frames);
    m_accum.resize(m_cols*2);
    m_fft_input.resize(m_cols*2);
    m_fft_output.resize(m_cols);
    m_matrix.resize(m_cols, m_rows);

    if (m_bBypassed)
    {
        if ( m_inputSignal )
        {
            m_inputSignal->dsp_read( pts, frames, sampleRate, L, R );
        }
        // else
        // {
        //     std::fill(L, L + frames, 0.0f);
        //     std::fill(R, R + frames, 0.0f);
        // }
        return;
    }

    if ( m_inputSignal )
    {
        m_inputSignal->dsp_read( pts, frames, sampleRate,
                                 m_L.data(), m_R.data() );
    }
    else
    {
        std::fill(m_L.data(), m_L.data() + frames, 0.0f);
        std::fill(m_R.data(), m_R.data() + frames, 0.0f);
    }

    // Sum L+R
    for (size_t i = 0; i < frames; ++i)
    {
        m_sum[i] = 0.5f * (m_L[i] + m_R[i]);
    }

    // Push sum L+R into accumulator
    m_accum.push( "L+R accum", m_sum.data(), frames, false );

    // Relay data
    std::memcpy(L, m_L.data(), frames * sizeof(float));
    std::memcpy(R, m_R.data(), frames * sizeof(float));
}

} // end namespace audio.
} // end namespace de.
