#include <de/audio/dsp/DspSampleCollector.h>

namespace de {
namespace audio {

namespace {

void shiftRight(TAlignedVector<float> & ori,
                TAlignedVector<float> & tmp,
                const float* __restrict__ pIn, u32 nIn)
{
    typedef float T;

    if (ori.size() != tmp.size())
    {
        DE_ERROR("ori.size() != tmp.size()")
        return;
    }

    const u32 nOut = ori.size();
    if (nIn >= nOut)
    {
        T* __restrict__ pOut = ori.data();
        std::memcpy(pOut, pIn, nOut * sizeof(T));
        return;
    }

    // nIn < nOut

    //   ori = |0|1|2|3|4|5|
    //    in = |A|B|C|D|E|

    //   tmp = |x|x|x|x|x|0| after shift right
    //   tmp = |A|B|C|D|E|0| after inserting new data to left begin

    // -> Finally swap(ori,tmp) -> ori = |A|B|C|D|E|0| now

    // Shift kept data to the right
    {
        size_t bytesOld = sizeof(T) * (nOut - nIn); // 6-5 = keep 1
        const T* __restrict__ pSrc = ori.data();
              T* __restrict__ pDst = tmp.data() + nIn;

        std::memcpy(pDst, pSrc, bytesOld);
    }

    // Add new data to the left begin
    {
        size_t bytesNew = sizeof(T) * nIn;
        const T* __restrict__ pSrc = pIn;
              T* __restrict__ pDst = tmp.data();
        std::memcpy(pDst, pSrc, bytesNew);
    }

    // Make 'temp' the new 'orig'...
    // std::swap(orig,temp);
}

}

// ===================================================================
DspSampleCollector::DspSampleCollector()
    : m_inputSignal{ nullptr }
    , m_blockSize{ 0 }
    , m_blockCount{ 16 } // fftSize / blockSize = 2048 / 128 = 16
    , m_blockIndex{ 0 }
    , m_fftSize{ 2048 }
    , m_cols{ 1024 }
    , m_rows{ 64 }
    , m_bBypassed{ true }
    , m_bCollectAccumMatrix{ true }
    //, m_L("samcoll_L")
    //, m_R("samcoll_R")
    //, m_sum("samcoll_sum")
    //, m_accum_vec_in("samcoll_accum_vec_in")
    //, m_accum_vec_out("samcoll_accum_vec_out")
{
    //DE_TRACE("")

    m_accum_win.setFunction(WindowFunction::Blackman);

#if 0
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
    #endif
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
    m_accum_ori.resize(m_fftSize);
    m_accum_tmp.resize(m_fftSize);
    m_accum_fft.resize(m_fftSize);
    m_accum_vec_in.resize(m_fftSize);
    m_accum_vec_out.resize(m_cols);
    m_accum_mat.resize(m_cols, m_rows);

    DE_WARN("{"
            " blockSize:",m_blockSize,";"
            " fftSize:",m_fftSize,";"
            " cols:",m_cols,";"
            " rows:",m_rows,"; }")

    if (m_inputSignal)
    {
        m_inputSignal->dsp_init(frames, channels, sampleRate);
    }
    else
    {
        DE_ERROR("No inputSignal. Dsp meaningless")
    }
}

void
DspSampleCollector::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                             f32* __restrict__ L,
                             f32* __restrict__ R)
{
    if (!m_inputSignal)
    {
        DE_ERROR("No inputSignal. Dsp meaningless")
        return;
    }

    if (m_bBypassed)
    {
        // Relay signal
        DE_ASSUME(L != R);
        m_inputSignal->dsp_read( pts, frames, sampleRate, L, R );
        return;
    }

    m_L.resize(frames);
    m_R.resize(frames);
    m_sum.resize(frames);
    m_accum_ori.resize(m_fftSize);
    m_accum_tmp.resize(m_fftSize);
    m_accum_fft.resize(m_fftSize);
    m_accum_vec_in.resize(m_fftSize);
    m_accum_vec_out.resize(m_cols);
    m_accum_mat.resize(m_cols, m_rows);

    // Get input signal
    {
        float* __restrict__ l = m_L.data();
        float* __restrict__ r = m_R.data();
        DE_ASSUME(l != r);
        m_inputSignal->dsp_read( pts, frames, sampleRate, l, r );
    }

    // Relay signal
    {
#if 1
        de_memcpy_no_overlap(L, m_L.data(), frames * sizeof(float));
        de_memcpy_no_overlap(R, m_R.data(), frames * sizeof(float));
#else
        const float* __restrict__ l = m_L.data();
        const float* __restrict__ r = m_R.data();
        DE_ASSUME(l != r);
        DE_ASSUME(l != L);
        DE_ASSUME(r != R);
        DE_ASSUME(L != R);
        std::memcpy(L, l, frames * sizeof(float));
        std::memcpy(R, r, frames * sizeof(float));
#endif
    }

    // Sum L+R
    {
        const float* __restrict__ l = m_L.data();
        const float* __restrict__ r = m_R.data();
        float* __restrict__ s = m_sum.data();
        DE_ASSUME(l != r);
        DE_ASSUME(l != s);
        DE_ASSUME(r != s);
        for (size_t i = 0; i < frames; ++i)
        {
            s[i] = 0.5f * (l[i] + r[i]);
        }
    }

    // Push sum L+R into accumulator
    // m_accum.push(m_sum.data(), m_sum.size());

    shiftRight( m_accum_ori,
                m_accum_tmp,
                m_sum.data(),
                m_sum.size());

    std::swap( m_accum_ori, m_accum_tmp );

    // Collect m_blockCount before doing an fft
    if (m_blockIndex < m_blockCount)
    {
        m_blockIndex++;
        return;
    }
    else
    {
        m_blockIndex = 0;
    }

    // Apply window function
    m_accum_win.apply( m_accum_ori.data(),
                       m_accum_ori.size(),
                       m_accum_vec_in.data(),
                       m_accum_vec_in.size());

    // Apply fft
    m_accum_fft.fft(
            m_accum_vec_in.data(), m_accum_vec_in.size(),
            m_accum_vec_out.data(), m_accum_vec_out.size());

    // Push fft row to AccumShiftMatrix.
    if (m_bCollectAccumMatrix)
    {
        m_accum_mat.push(
            m_accum_vec_out.data(),
            m_accum_vec_out.size());
    }

}

} // end namespace audio.
} // end namespace de.
