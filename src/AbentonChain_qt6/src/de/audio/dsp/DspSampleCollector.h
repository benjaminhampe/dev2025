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
    u32 m_fftSize;
    u32 m_cols;
    u32 m_rows;
    int m_windowFunc;
    bool m_bStopped;
    bool m_bBypassed;
    bool m_bCollectAccumMatrix;

    DE_GuardedBuffer m_L;
    DE_GuardedBuffer m_R;

    // L+R are summed up for FFT
    DE_GuardedBuffer m_sum;

    // blockSize FFT (potentially not a power of 2)
    // AlignedFloatShiftVector m_raw;
    // AlignedFloatVector m_raw_fft_in;
    // AlignedFloatVector m_raw_fft_out;
    // AlignedFloatShiftMatrix m_raw_fft_matrix;

    // shiftVector FFT
    AlignedFloatShiftVector m_accum;
    DE_GuardedBuffer m_accum_vec_in;
    DE_GuardedBuffer m_accum_vec_out;
    AlignedFloatShiftMatrix m_accum_mat;

    // float a1[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a2[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a3[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a4[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a5[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a6[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a7[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float a8[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    // float b1[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b2[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b3[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b4[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b5[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b6[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b7[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // float b8[8] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    std::shared_ptr<DE_FFT_pffft> m_accum_fft;
public:
    const AlignedFloatVector& getL() const { return m_L.data; }
    const AlignedFloatVector& getR() const { return m_R.data; }

    const AlignedFloatVector& getAccumVecIn() const { return m_accum_vec_in.data; }
    const AlignedFloatVector& getAccumVecOut() const { return m_accum_vec_out.data; }
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
