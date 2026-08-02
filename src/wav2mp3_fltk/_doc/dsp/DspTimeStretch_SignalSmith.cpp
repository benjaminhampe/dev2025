#include <de/audio/dsp/DspTimeStretch_SignalSmith.h>

namespace de {
namespace audio {

SignalSmithStretchElement::SignalSmithStretchElement()
{
    // Default parameters
    m_params.windowMs      = 40.0;
    m_params.searchMs      = 15.0;
    m_params.pitchFactor   = 1.0;
    m_params.timeFactor    = 1.0;
    m_params.oversample    = 2;
    m_params.quality       = 1;
}

// ============================
// dsp_name
// ============================
std::string SignalSmithStretchElement::dsp_name() const
{
    return "SignalSmithStretch";
}

// ============================
// dsp_init
// ============================
void SignalSmithStretchElement::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    m_channels   = channels;
    m_sampleRate = sampleRate;

    // Stretch initialisieren
    m_stretch.reset(new signalsmith::stretch::SignalsmithStretch<float>());

    // m_stretch->s
    //     channels,
    //     sampleRate,
    //     m_params.windowMs,
    //     m_params.searchMs,
    //     m_params.oversample,
    //     m_params.quality

    // // Parameter setzen
    // m_stretch->setPitchFactor(m_params.pitchFactor);
    // m_stretch->setTimeFactor(m_params.timeFactor);

    // Buffers vorbereiten
    m_inPlanar.resize(channels);
    m_outPlanar.resize(channels);

    for (u32 ch = 0; ch < channels; ++ch) {
        m_inPlanar[ch].resize(frames);
        m_outPlanar[ch].resize(frames * 4); // Stretch kann mehr ausgeben
    }
}

// ============================
// dsp_read
// ============================
void SignalSmithStretchElement::dsp_read(f64 pts, u32 frames, u32 sampleRate,
              f32* __restrict__ L,
              f32* __restrict__ R)
{
    IDspChainElement* input = m_inputs[0];
    if (!input) {
        // Silence
        memset(L, 0, sizeof(float)*frames);
        memset(R, 0, sizeof(float)*frames);
        return;
    }

    // Input holen (interleaved planar)
    input->dsp_read(pts, frames, sampleRate,
                    m_inPlanar[0].data(),
                    m_channels > 1 ? m_inPlanar[1].data() : nullptr);

    // Stretch verarbeiten
    const int outFrames = m_stretch->process(
        m_inPlanar,
        frames,
        m_outPlanar
    );

    // Output zurück in interleaved L/R
    for (int i = 0; i < outFrames; ++i) {
        L[i] = m_outPlanar[0][i];
        if (m_channels > 1)
            R[i] = m_outPlanar[1][i];
    }
}

// ============================
// Input Signal Handling
// ============================
u32 SignalSmithStretchElement::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* SignalSmithStretchElement::dsp_getInputSignal(int i)
{
    return m_inputs[i];
}

void SignalSmithStretchElement::dsp_setInputSignal(IDspChainElement* input, int i)
{
    m_inputs[i] = input;
}

void SignalSmithStretchElement::dsp_clearInputSignals()
{
    m_inputs[0] = nullptr;
}

// ============================
// Realtime Parameter Setters
// ============================

void set_window_ms(double ms) {
    m_params.windowMs = ms;
    if (m_stretch) m_stretch->setWindowMs(ms);
}

void set_search_ms(double ms) {
    m_params.searchMs = ms;
    if (m_stretch) m_stretch->setSearchMs(ms);
}

void set_pitch_factor(double f) {
    m_params.pitchFactor = f;
    if (m_stretch) m_stretch->setPitchFactor(f);
}

void set_time_factor(double f) {
    m_params.timeFactor = f;
    if (m_stretch) m_stretch->setTimeFactor(f);
}

void set_oversample(int o) {
    m_params.oversample = o;
    // Muss neu initialisiert werden
}

void set_quality(int q) {
    m_params.quality = q;
    // Muss neu initialisiert werden
}


} // end namespace audio.
} // end namespace de.
