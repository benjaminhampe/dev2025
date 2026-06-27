#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <QRect>
#include <QColor>
#include <de/session/Track.h>

namespace de {
namespace session {

class MixerItem
{
public:
    Track* m_inputSignal;
    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

    MixerItem()
        : m_inputSignal{ nullptr }
    {}

    void dsp_init( u64 frames, u32 channels, u32 sampleRate)
    {
        m_L.resize(frames);
        m_R.resize(frames);
        m_inputSignal->dsp_init(frames, channels, sampleRate);
    }

    void dsp_read(f64 pts, u32 frames, u32 sampleRate)
    {
        m_L.resize(frames);
        m_R.resize(frames);
        float* __restrict__ const Lout = m_L.data();
        float* __restrict__ const Rout = m_R.data();
        DE_ASSUME_NO_OVERLAP(Lout, Rout, frames * sizeof(float));
        m_inputSignal->dsp_read( pts, frames, sampleRate, Lout, Rout );
    }
};

// ===================================================================
class Mixer : public de::audio::IDspChainElement
// ===================================================================
{
    AlignedFloatVector m_Laccum;
    AlignedFloatVector m_Raccum;
public:
    std::vector<MixerItem> m_items;
public:

    Mixer();
    ~Mixer() override;

    std::string dsp_name() const override { return "Mixer"; }

    void dsp_init( u64 frames, u32 channels, u32 sampleRate ) override;

    void dsp_read( f64 pts, u32 frames, u32 sampleRate,
                   f32* __restrict__ L,
                   f32* __restrict__ R) override;

    void setTrack( Track* track, int i = 0 );

    void dsp_setInputSignal( IDspChainElement* inputSignal, int i = 0 ) override;

    void dsp_clearInputSignals() override;

    u32 dsp_getInputSignalCount() const override;

    IDspChainElement* dsp_getInputSignal(int i = 0) override;

    void dsp_removeInputSignal( IDspChainElement* inputSignal ); // override;

    void dsp_setInputSignalCount( uint32_t count ); // override;

};

} // end namespace session.
} // end namespace de.
