#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/fft/approx_math.h>
#include <de/audio/fft/DE_FFT_pffft.h>

namespace de {
namespace audio {

// ===================================================================
class DspSampleCollector : public IDspChainElement
// ===================================================================
{
    IDspChainElement* m_inputSignal;
    u32 m_cols;
    u32 m_rows;
    int m_windowFunc;
    bool m_bStopped;
    bool m_bBypassed;
    bool m_bCollectAccumMatrix;
    DE_FFT_pffft m_accum_fft;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

    // L+R are summed up for FFT
    AlignedFloatVector m_sum;

    // blockSize FFT (potentially not a power of 2)
    // AlignedFloatShiftVector m_raw;
    // AlignedFloatVector m_raw_fft_in;
    // AlignedFloatVector m_raw_fft_out;
    // AlignedFloatShiftMatrix m_raw_fft_matrix;

    // shiftVector FFT
    AlignedFloatShiftVector m_accum;
    AlignedFloatVector m_accum_vec_in;
    AlignedFloatVector m_accum_vec_out;
    AlignedFloatShiftMatrix m_accum_mat;

public:
    const AlignedFloatVector& getL() const { return m_L; }
    const AlignedFloatVector& getR() const { return m_R; }

    const AlignedFloatVector& getAccumVecIn() const { return m_accum_vec_in; }
    const AlignedFloatVector& getAccumVecOut() const { return m_accum_vec_out; }
    const AlignedFloatShiftMatrix& getAccumMat() const { return m_accum_mat; }

    DspSampleCollector();
    ~DspSampleCollector();

    void stop()
    {
        m_bStopped = true;
        m_inputSignal = nullptr;
        m_accum.setCallback_onFullVector(
            [](const TAlignedVector<float>& ){});
    }

    void setBypassed( bool bBypassed )
    {
        m_bBypassed = bBypassed;
    }

    void setCollectAccumMatrix( bool bBypassed )
    {
        m_bCollectAccumMatrix = bBypassed;
    }

    void setWindowFunc( int windowFunc )
    {
        m_windowFunc = windowFunc;
    }

    void setColumnCount( u32 columns )
    {
        m_cols = std::max(128u,columns);
    }

    void setRowCount( u32 rows )
    {
        m_rows = std::max(8u,rows);
    }

    void applyWindow(int winType,
                     const float* __restrict__ src,
                     float* __restrict__ dst,
                     size_t n);

    void dsp_init( u64 frames, u32 channels, u32 sampleRate ) override;

    void dsp_read( f64 pts, u32 frames, u32 sampleRate,
                   f32* __restrict__ L,
                   f32* __restrict__ R) override;

    void dsp_setInputSignal( IDspChainElement* inputSignal, int i = 0 ) override;

    void dsp_clearInputSignals() override { m_inputSignal = nullptr; }

    u32 dsp_getInputSignalCount() const override
    {
        return m_inputSignal ? 1 : 0;
    }

    IDspChainElement* dsp_getInputSignal(int i = 0) override
    {
        return m_inputSignal;
    }
};

} // end namespace audio.
} // end namespace de.
