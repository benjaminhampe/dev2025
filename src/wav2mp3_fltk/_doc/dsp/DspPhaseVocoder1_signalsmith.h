#pragma once

#include <de/audio/dsp/IDspChainElement.h>

// #include <signalsmith-stretch/signalsmith-stretch.h>
#include "shift-stretch_phaseVocoder.h"

namespace de {
namespace audio {

class PhaseVocoderDspElement final : public IDspChainElement 
{
public:
    PhaseVocoderDspElement()
    {
        // Defaults wie im CLI-Tool
        m_purePhase   = false;
        m_gain        = 1.0;
        m_timeFactor  = 1.0;
        m_freqFactor  = 1.0;
        m_blockMs     = 120.0;
        m_overlap     = 4.0;
        m_zeroPadding = 2.0;
    }

    std::string dsp_name() const override {
        return "PhaseVocoderStretch";
    }

    // ============================================================
    // dsp_init
    // ============================================================
    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override
    {
        m_channels   = channels;
        m_sampleRate = sampleRate;

        const int blockSamples    = int(m_blockMs * 0.001 * sampleRate + 0.5);
        const int intervalSamples = int(blockSamples / m_overlap);

        m_stretch.reset(new PhaseVocoderStretch(m_purePhase));
        m_stretch->configure(channels, blockSamples, intervalSamples, m_zeroPadding);

        m_stretch->gain = m_gain;
        m_stretch->setTimeFactor(m_timeFactor);
        m_stretch->setFreqFactor(m_freqFactor);

        // Buffers
        m_inPlanar.resize(channels);
        m_outPlanar.resize(channels);

        for (u32 c = 0; c < channels; ++c) {
            m_inPlanar[c].resize(blockSamples * 4);
            m_outPlanar[c].resize(blockSamples * 4);
        }
    }

    // ============================================================
    // dsp_read
    // ============================================================
    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R) override
    {
        IDspChainElement* input = m_inputs[0];
        if (!input) {
            memset(L, 0, sizeof(float)*frames);
            memset(R, 0, sizeof(float)*frames);
            return;
        }

        // Signalsmith: samplesForOutput() ist Pflicht
        const int inputSamples = m_stretch->samplesForOutput(frames);

        // Buffergröße sicherstellen
        for (u32 c = 0; c < m_channels; ++c) {
            if ((int)m_inPlanar[c].size() < inputSamples)
                m_inPlanar[c].resize(inputSamples);
            if ((int)m_outPlanar[c].size() < frames)
                m_outPlanar[c].resize(frames);
        }

        // Input holen
        input->dsp_read(pts, inputSamples, sampleRate,
                        m_inPlanar[0].data(),
                        m_channels > 1 ? m_inPlanar[1].data() : nullptr);

        // Pointer-Arrays bauen
        std::vector<double*> inPtrs(m_channels);
        std::vector<double*> outPtrs(m_channels);

        for (u32 c = 0; c < m_channels; ++c) {
            inPtrs[c]  = m_inPlanar[c].data();
            outPtrs[c] = m_outPlanar[c].data();
        }

        // PhaseVocoderStretch::process()
        m_stretch->process(inPtrs.data(), inputSamples, outPtrs.data(), frames);

        // Output zurückgeben
        for (u32 i = 0; i < frames; ++i) {
            L[i] = float(m_outPlanar[0][i]);
            if (m_channels > 1)
                R[i] = float(m_outPlanar[1][i]);
        }
    }

    // ============================================================
    // Input handling
    // ============================================================
    u32 dsp_getInputSignalCount() const override { return 1; }
    IDspChainElement* dsp_getInputSignal(int i = 0) override { return m_inputs[i]; }
    void dsp_setInputSignal(IDspChainElement* in, int i = 0) override { m_inputs[i] = in; }
    void dsp_clearInputSignals() override { m_inputs[0] = nullptr; }

    // ============================================================
    // Realtime setters (alle echten Parameter)
    // ============================================================
    void set_purePhase(bool b)     { m_purePhase = b; }
    void set_gain(double g)        { m_gain = g; if (m_stretch) m_stretch->gain = g; }
    void set_timeFactor(double f)  { m_timeFactor = f; if (m_stretch) m_stretch->setTimeFactor(f); }
    void set_freqFactor(double f)  { m_freqFactor = f; if (m_stretch) m_stretch->setFreqFactor(f); }
    void set_blockMs(double ms)    { m_blockMs = ms; }
    void set_overlap(double o)     { m_overlap = o; }
    void set_zeroPadding(double z) { m_zeroPadding = z; }

private:
    std::unique_ptr<PhaseVocoderStretch> m_stretch;

    u32 m_channels = 0;
    u32 m_sampleRate = 0;

    std::vector<std::vector<double>> m_inPlanar;
    std::vector<std::vector<double>> m_outPlanar;

    IDspChainElement* m_inputs[1] = { nullptr };

    bool   m_purePhase;
    double m_gain;
    double m_timeFactor;
    double m_freqFactor;
    double m_blockMs;
    double m_overlap;
    double m_zeroPadding;
};


} // end namespace audio.
} // end namespace de.
