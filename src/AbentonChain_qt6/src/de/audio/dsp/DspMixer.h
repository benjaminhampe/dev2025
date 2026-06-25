#pragma once
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

// ===================================================================
class DspMixer : public IDspChainElement
// ===================================================================
{
    u64 m_initFrames;       // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.
    u32 m_initChannels;     // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.
    u32 m_initSampleRate;   // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.
    std::vector<IDspChainElement*> m_inputSignals;
    AlignedFloatVector m_L;
    AlignedFloatVector m_R;
public:
    DspMixer();
    ~DspMixer() override;

    void dsp_init( u64 frames, u32 channels, u32 sampleRate ) override;

    void dsp_read( f64 pts, u32 frames, u32 sampleRate,
                   f32* __restrict__ L,
                   f32* __restrict__ R) override;


    void dsp_setInputSignal( IDspChainElement* inputSignal, int i = 0 ) override;

    void dsp_clearInputSignals() override;

    u32 dsp_getInputSignalCount() const override;

    IDspChainElement* dsp_getInputSignal(int i = 0) override;

    void dsp_removeInputSignal( IDspChainElement* inputSignal ); // override;

    void dsp_setInputSignalCount( uint32_t count ); // override;

};

} // end namespace audio.
} // end namespace de.
