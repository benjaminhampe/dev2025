#pragma once
#include <de/audio/dsp/StereoAudioFifo.h>

namespace de {
namespace audio {

// ===================================================================
class DspResampler : public IDspChainElement
// ===================================================================
{
    IDspChainElement* m_inputSignal;
    void* m_handleL;
    void* m_handleR;
    u32 m_sampleRateIn;
    u32 m_sampleRateOut;
    u32 m_blockSizeIn;
    u32 m_blockSizeOut;
    double m_minRatio;
    double m_maxRatio;
    double m_ratio;     // m_effectiveRatio = sr_output / sr_input * userFactor
    double m_userFactor;    // user

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

    StereoAudioFifo m_fifo;
public:
    DspResampler();
    ~DspResampler() override;

    void setSampleRateIn(u32 sampleRate);

    std::string dsp_name() const override { return "DspResampler-1.8"; }

    void dsp_init( u64 frames, u32 channels, u32 sampleRate ) override;

    void dsp_read( f64 pts, u32 frames, u32 sampleRate,
                   f32* __restrict__ L,
                   f32* __restrict__ R) override;

    void dsp_setInputSignal( IDspChainElement* inputSignal, int i = 0 ) override;

    void dsp_clearInputSignals() override { m_inputSignal = nullptr; }

    u32 dsp_getInputSignalCount() const override { return m_inputSignal ? 1 : 0; }

    IDspChainElement* dsp_getInputSignal(int i = 0) override { return m_inputSignal; }
};

} // end namespace audio.
} // end namespace de.
