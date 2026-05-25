#include <de/audio/dsp/DspMixer.h>

namespace de {
namespace audio {

// ===================================================================
DspMixer::DspMixer()
    : m_initFrames(0)       // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.
    , m_initChannels(0)     // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.
    , m_initSampleRate(0)   // On dsp_setInputSignal() we call inputSignals->dsp_init() with those values.

{
    DE_TRACE("")
}

// ===================================================================
DspMixer::~DspMixer()
{
    DE_TRACE("")
}

void
DspMixer::dsp_init( u64 frames, u32 channels, u32 sampleRate )
{
    m_L.resize(frames);
    m_R.resize(frames);
    m_initFrames = frames;
    m_initChannels = channels;
    m_initSampleRate = sampleRate;

    for (auto m_inputSignal : m_inputSignals)
    {
        if (m_inputSignal)
        {
            m_inputSignal->dsp_init(frames, channels, sampleRate);
        }
    }
}

void
DspMixer::dsp_read( f64 pts, u32 frames, u32 sampleRate,
                    f32* __restrict__ L,
                    f32* __restrict__ R)
{
    m_L.resize(frames);
    m_R.resize(frames);
    m_initFrames = frames;
    m_initSampleRate = sampleRate;

    for (auto inputSignal : m_inputSignals)
    {
        if (!inputSignal)
        {
            continue;
        }

        inputSignal->dsp_read( pts, frames, sampleRate, m_L.data(), m_R.data() );

        for (size_t i = 0; i < frames; ++i)
        {
            L[i] += m_L[i];
            R[i] += m_R[i];
        }
    }
}

void
DspMixer::dsp_setInputSignal( IDspChainElement* inputSignal, int /* i */ )
{
    if (!inputSignal)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    auto it = std::find_if(m_inputSignals.begin(),m_inputSignals.end(), [inputSignal]( IDspChainElement* cached ) { return cached == inputSignal; });

    if (it != m_inputSignals.end())
    {
        DE_ERROR("inputSignal already added, abort")
        return;
    }

    inputSignal->dsp_init(m_initFrames,m_initChannels,m_initSampleRate);

    m_inputSignals.emplace_back(inputSignal);
}

void
DspMixer::dsp_clearInputSignals()
{
    m_inputSignals.clear();
}

u32
DspMixer::dsp_getInputSignalCount() const
{
    return m_inputSignals.size();
}

IDspChainElement*
DspMixer::dsp_getInputSignal(int i)
{
    return m_inputSignals.at(i);
}

void
DspMixer::dsp_removeInputSignal( IDspChainElement* inputSignal )
{
    if (!inputSignal)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    auto it = std::find_if( m_inputSignals.begin(),
                            m_inputSignals.end(),
                            [inputSignal]( IDspChainElement* cached )
                            { return cached == inputSignal; } );

    if (it == m_inputSignals.end())
    {
        DE_ERROR("inputSignal already removed.")
        return;
    }

    m_inputSignals.erase(std::remove(m_inputSignals.begin(), m_inputSignals.end(), inputSignal), m_inputSignals.end());
}

} // end namespace audio.
} // end namespace de.
