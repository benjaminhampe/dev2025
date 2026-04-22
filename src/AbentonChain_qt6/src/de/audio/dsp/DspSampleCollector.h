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
    bool m_bBypassed;
    DE_FFT_pffft m_fft;

    // L+R are summed up into, before injected into shift-buffer.
    DE_AlignedFloatVector m_L;
    DE_AlignedFloatVector m_R;
    DE_AlignedFloatVector m_sum;

    // ->
    DE_AlignedFloatShiftVector m_accum;
    DE_AlignedFloatVector m_fft_input;
    DE_AlignedFloatVector m_fft_output;
    DE_AlignedFloatShiftMatrix m_matrix;

public:
    const DE_AlignedFloatShiftMatrix& getMatrix() const { return m_matrix; }

    DspSampleCollector();

    void setBypassed( bool bBypassed )
    {
        m_bBypassed = bBypassed;
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

    void applyWindow(int winType, float* __restrict__ dst, const float* __restrict__ src, size_t n);

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
