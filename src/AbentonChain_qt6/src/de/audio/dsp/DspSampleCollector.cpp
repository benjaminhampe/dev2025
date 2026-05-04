#include <de/audio/dsp/DspSampleCollector.h>

namespace de {
namespace audio {

// ===================================================================
DspSampleCollector::DspSampleCollector()
    : m_inputSignal{ nullptr }
    , m_blockSize{ 0 }
    , m_fftSize{ 2048 }
    , m_cols{ 512 }
    , m_rows{ 64 }
    , m_bBypassed{ true }
    , m_bStopped{ false }
    , m_bCollectAccumMatrix{ true }
    //, m_L("samcoll_L")
    //, m_R("samcoll_R")
    //, m_sum("samcoll_sum")
    //, m_accum_vec_in("samcoll_accum_vec_in")
    //, m_accum_vec_out("samcoll_accum_vec_out")
{
    DE_TRACE("")

    m_accum_win.setFunction(WindowFunction::Blackman);

    m_accum_fft = std::make_shared<DE_FFT_pffft>();

    m_accum.setCallback_onFullVector(
        [&](const DE_AlignedFloatVector& v)
        {
            auto n = v.size();
            if (n<8)
            {
                DE_ERROR("")
                return;
            }

            // Apply window function
            m_accum_win.apply( m_accum.data(),
                               m_accum.size(),
                               m_accum_vec_in.data(),
                               m_accum_vec_in.size());

            // Apply fft
            if (m_accum_fft)
            {
                m_accum_fft->fft(
                    m_accum_vec_in.data(), m_accum_vec_in.size(),
                    m_accum_vec_out.data(), m_accum_vec_out.size());
            }

            // Push fft row to AccumShiftMatrix.
            if (m_bCollectAccumMatrix)
            {
                m_accum_mat.push(
                    m_accum_vec_out.data(),
                    m_accum_vec_out.size());
            }
        }
    );
}

// ===================================================================
DspSampleCollector::~DspSampleCollector()
{
    // DE_TRACE("")
}

void
DspSampleCollector::dsp_setInputSignal( IDspChainElement* inputSignal, int i )
{
    m_inputSignal = inputSignal;
}

void
DspSampleCollector::dsp_init( u64 frames, u32 channels, u32 sampleRate )
{
    if (frames < 1)
    {
        DE_ERROR("")
        return;
    }

    m_blockSize = frames;
    m_L.resize(frames);
    m_R.resize(frames);
    m_sum.resize(frames);

    m_accum.resize(m_fftSize);
    if (m_accum_fft) m_accum_fft->resize(m_fftSize);
    m_accum_vec_in.resize(m_fftSize);
    m_accum_vec_out.resize(m_cols);
    m_accum_mat.resize(m_cols, m_rows);

    DE_WARN("blockSize = ", m_blockSize)
    DE_WARN("fftSize = ", m_fftSize)
    DE_WARN("cols = ", m_cols)
    DE_WARN("rows = ", m_rows)

    if (m_inputSignal)
    {
        m_inputSignal->dsp_init(frames, channels, sampleRate);
    }
}

void
DspSampleCollector::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                             f32* __restrict__ L, f32* __restrict__ R)
{
    if (m_bStopped || frames < 1)
    {
        DE_ERROR("")
        return;
    }

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

    m_L.resize(frames);
    m_R.resize(frames);
    m_sum.resize(frames);
    m_accum.resize(m_fftSize);
    if (m_accum_fft) m_accum_fft->resize(m_fftSize);
    m_accum_vec_in.resize(m_fftSize);
    m_accum_vec_out.resize(m_cols);
    m_accum_mat.resize(m_cols, m_rows);

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
    m_accum.push(m_sum.data(), m_sum.size());

    // Relay data
    std::memcpy(L, m_L.data(), frames * sizeof(float));
    std::memcpy(R, m_R.data(), frames * sizeof(float));
}

} // end namespace audio.
} // end namespace de.
