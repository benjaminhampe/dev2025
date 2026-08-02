#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <signalsmith-dsp/delay.h>
#include <signalsmith-dsp/windows.h>
#include <signalsmith-dsp/fft.h>
#include <vector>
#include <complex>
#include <cmath>
#include <cstring>

namespace de {
namespace audio {

// ============================================================
class PhaseVocoderDspElement final : public IDspChainElement
// ============================================================
{
public:
    using Sample  = float;
    using Complex = std::complex<float>;

    // ============================================================
    // Parameter (alle dynamisch veränderbar)
    // ============================================================

    // Zeit-Stretch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    float timeFactor = 1.0f;

    // Pitch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    float freqFactor = 1.0f;

    // Blocklänge in ms (Default=120.0, Min=10.0, Max=500.0, Step=1.0)
    float blockMs = 120.0f;

    // Overlap-Faktor (Default=4.0, Min=1.0, Max=8.0, Step=0.1)
    float overlapFactor = 4.0f;

    // Zero-Padding-Faktor (Default=2.0, Min=1.0, Max=8.0, Step=0.5)
    float zeroPadding = 2.0f;

    // Reiner Phase-Vocoder (Default=false)
    bool purePhase = false;

    // Gain (Default=1.0, Min=0.0, Max=4.0, Step=0.01)
    float gain = 1.0f;

    // ============================================================
    // Konstruktor
    // ============================================================
    PhaseVocoderDspElement() {}

    std::string dsp_name() const override { return "PhaseVocoderDspElement"; }

    // ============================================================
    // dsp_init — wird EINMAL aufgerufen
    // ============================================================
    void dsp_init(uint64_t frames, uint32_t channels, uint32_t sampleRate) override
    {
        m_channels   = int(channels);
        m_sampleRate = int(sampleRate);

        // Initiale Blockgrößen berechnen
        updateBlockParameters();

        // MultiBuffer initialisieren (darf später wachsen)
        m_inputHistory = signalsmith::delay::MultiBuffer<Sample>(m_channels, m_blockSamples + 4096);
        m_summedOutput = signalsmith::delay::MultiBuffer<Sample>(m_channels, m_blockSamples);

        // Fenster initial berechnen
        updateWindow();

        // FFT initial konfigurieren
        updateFFT();

        // Spektral-Buffer initial anlegen
        allocateSpectralBuffers();

        // Planar-IO-Buffers initial anlegen
        m_inPlanar.resize(m_channels);
        m_outPlanar.resize(m_channels);
        for (int c = 0; c < m_channels; ++c) {
            m_inPlanar[c].resize(m_blockSamples * 4);
            m_outPlanar[c].resize(m_blockSamples * 4);
        }
    }

    // ============================================================
    // dsp_read — HIER PASSIERT ALLES
    // ============================================================
    void dsp_read(double pts, uint32_t frames, uint32_t sampleRate,
                  float* __restrict__ L,
                  float* __restrict__ R) override
    {
        IDspChainElement* input = m_inputs[0];
        if (!input) {
            std::memset(L, 0, sizeof(float)*frames);
            if (R) std::memset(R, 0, sizeof(float)*frames);
            return;
        }

        // ============================================================
        // 1) Dynamische Parameter prüfen (BlockSize, Overlap, ZeroPadding)
        // ============================================================
        bool blockChanged = updateBlockParameters();
        bool fftChanged   = updateFFT();
        bool windowChanged = updateWindow();

        if (blockChanged || fftChanged || windowChanged) {
            // ============================================================
            // WICHTIG:
            // Hier müsste man theoretisch History/Phasen löschen,
            // weil sich die Blockgröße geändert hat.
            //
            // ABER DU WILLST ES NICHT.
            //
            // Ich dokumentiere nur:
            //
            // - prevInputSpectra müsste neu initialisiert werden
            // - prevOutputSpectra müsste neu initialisiert werden
            // - outputRotations müsste neu initialisiert werden
            // - summedOutput müsste geleert werden
            // - inputHistory müsste geleert werden
            //
            // Du entscheidest später selbst, was du davon tust.
            // ============================================================

            allocateSpectralBuffers(); // wächst dynamisch
        }

        // ============================================================
        // 2) Input-Samples berechnen
        // ============================================================
        const int outputSamples = int(frames);
        const float invTime = 1.0f / (timeFactor * freqFactor);
        const int inputSamples = int(std::ceil(outputSamples * invTime - m_surplusInputSamples));

        // Planar-Buffergröße sicherstellen
        for (int c = 0; c < m_channels; ++c) {
            if (int(m_inPlanar[c].size()) < inputSamples)
                m_inPlanar[c].resize(inputSamples);
            if (int(m_outPlanar[c].size()) < outputSamples)
                m_outPlanar[c].resize(outputSamples);
        }

        // ============================================================
        // 3) Upstream lesen
        // ============================================================
        input->dsp_read(pts, inputSamples, sampleRate,
                        m_inPlanar[0].data(),
                        m_channels > 1 ? m_inPlanar[1].data() : nullptr);

        Sample* inputs[2]  = { m_inPlanar[0].data(),
                               m_channels > 1 ? m_inPlanar[1].data() : m_inPlanar[0].data() };
        Sample* outputs[2] = { m_outPlanar[0].data(),
                               m_channels > 1 ? m_outPlanar[1].data() : m_outPlanar[0].data() };

        int inputFilledTo = 0;

        // ============================================================
        // 4) Hauptschleife: Output-Samples erzeugen
        // ============================================================
        for (int o = 0; o < outputSamples; ++o) {

            // ------------------------------------------------------------
            // Block-Trigger
            // ------------------------------------------------------------
            if (++m_intervalCounter >= m_intervalSamples) {
                m_intervalCounter = 0;

                // ------------------------------------------------------------
                // Block-Start im Input
                // ------------------------------------------------------------
                int inputStart = int(std::round(o * invTime
                                                - m_surplusInputSamples
                                                - m_blockSamples));

                inputStart = std::max(
                    std::min(inputStart, inputSamples - m_blockSamples),
                    -4096 - m_blockSamples
                );

                // ------------------------------------------------------------
                // Input-History auffüllen + Fenster anwenden
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    auto inputBuf  = inputs[c];
                    auto history   = m_inputHistory[c];

                    for (int i = inputFilledTo; i < inputStart + m_blockSamples; ++i)
                        history[i] = inputBuf[i];

                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    for (int i = 0; i < m_blockSamples; ++i)
                        block[i] = history[inputStart + i] * m_window[i];
                }

                // ------------------------------------------------------------
                // FFT pro Kanal
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block   = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    for (int i = 0; i < m_blockSamples; ++i)
                        m_fftBuffer[i] = block[i];
                    for (int i = m_blockSamples; i < int(m_fftBuffer.size()); ++i)
                        m_fftBuffer[i] = Sample(0);

                    m_fft.fft(m_fftBuffer.data(), spectrum);
                }

                // ------------------------------------------------------------
                // Phase-Vocoder
                // ------------------------------------------------------------
                int inputIntervalSamples = inputStart - m_prevInputIndex;
                m_prevInputIndex = inputStart;

                float timeFactorLocal = inputIntervalSamples > 0
                    ? float(m_intervalSamples) / float(inputIntervalSamples)
                    : 0.0f;

                timeShiftPhases(-inputIntervalSamples, m_prevInputRotations.data());

                for (int c = 0; c < m_channels; ++c) {
                    Complex* prevIn  = m_prevInputSpectra.data()  + c*m_bandCount;
                    Complex* prevOut = m_prevOutputSpectra.data() + c*m_bandCount;

                    for (int b = 0; b < m_bandCount; ++b) {
                        prevIn[b]  *= m_prevInputRotations[b];
                        prevOut[b] *= m_prevOutputRotations[b];
                    }
                }

                for (int c = 0; c < m_channels; ++c) {
                    Complex* curr    = m_channelSpectra.data() + c*m_bandCount;
                    Complex* prevIn  = m_prevInputSpectra.data() + c*m_bandCount;
                    Complex* prevOut = m_prevOutputSpectra.data() + c*m_bandCount;

                    for (int b = 0; b < m_bandCount; ++b) {

                        if (inputIntervalSamples > 0) {
                            Complex rotation = curr[b] * std::conj(prevIn[b]);
                            float rotationAbs = std::abs(rotation);
                            float phase = std::arg(rotation) * timeFactorLocal;

                            m_outputRotations[b] = {
                                rotationAbs * std::cos(phase),
                                rotationAbs * std::sin(phase)
                            };

                            prevIn[b] = curr[b];
                        }

                        float outputEnergy = std::norm(curr[b]);
                        Complex complexPhase = prevOut[b] * m_outputRotations[b];

                        if (!purePhase) {
                            float existingEnergy = std::min(std::norm(prevOut[b]), outputEnergy);
                            float newEnergy      = outputEnergy - existingEnergy;
                            complexPhase = existingEnergy * complexPhase + newEnergy * curr[b];
                        }

                        curr[b] = generateComplex(outputEnergy, complexPhase);
                        curr[b] *= gain;
                        prevOut[b] = curr[b];
                    }
                }

                // ------------------------------------------------------------
                // IFFT pro Kanal
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block   = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    m_fft.ifft(spectrum, m_fftBuffer.data());
                    for (int i = 0; i < m_blockSamples; ++i)
                        block[i] = m_fftBuffer[i] * m_scalingFactor;
                }

                // ------------------------------------------------------------
                // Overlap-Add Summation
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    auto outBuf   = m_summedOutput[c];
                    for (int i = 0; i < m_blockSamples; ++i)
                        outBuf[i] += block[i] * m_window[i];
                }
            }

            // ------------------------------------------------------------
            // Ein Sample aus Summen-Output ausgeben
            // ------------------------------------------------------------
            for (int c = 0; c < m_channels; ++c) {
                outputs[c][o] = m_summedOutput[c][0];
                m_summedOutput[c][0] = Sample(0);
            }
            ++m_summedOutput;
        }

        // ============================================================
        // 5) Restliches Input in History kopieren
        // ============================================================
        for (int c = 0; c < m_channels; ++c) {
            auto inputBuf  = inputs[c];
            auto history   = m_inputHistory[c];
            for (int i = inputFilledTo; i < inputSamples; ++i)
                history[i] = inputBuf[i];
        }
        m_inputHistory += inputSamples;
        m_prevInputIndex -= inputSamples;
        m_surplusInputSamples += inputSamples - outputSamples * invTime;

        // ============================================================
        // 6) Output nach L/R kopieren
        // ============================================================
        for (int i = 0; i < outputSamples; ++i) {
            L[i] = m_outPlanar[0][i];
            if (m_channels > 1 && R)
                R[i] = m_outPlanar[1][i];
        }
    }

    // ============================================================
    // Input handling
    // ============================================================
    uint32_t dsp_getInputSignalCount() const override { return 1; }
    IDspChainElement* dsp_getInputSignal(int i = 0) override { return m_inputs[i]; }
    void dsp_setInputSignal(IDspChainElement* in, int i = 0) override { m_inputs[i] = in; }
    void dsp_clearInputSignals() override { m_inputs[0] = nullptr; }

private:
    // ============================================================
    // Hilfsfunktionen (NICHT im Hotpath)
    // ============================================================

    bool updateBlockParameters()
    {
        int newBlockSamples = int(blockMs * 0.001f * m_sampleRate + 0.5f);
        int newIntervalSamples = int(newBlockSamples / overlapFactor);

        bool changed = (newBlockSamples != m_blockSamples ||
                        newIntervalSamples != m_intervalSamples);

        m_blockSamples    = newBlockSamples;
        m_intervalSamples = newIntervalSamples;

        // BlockBuffers wachsen dynamisch
        m_blockBuffers.resize(m_blockSamples * m_channels);

        return changed;
    }

    bool updateWindow()
    {
        m_window.resize(m_blockSamples);

        auto kaiser = signalsmith::windows::Kaiser::withBandwidth(
            m_blockSamples * 1.0 / m_intervalSamples, true);
        kaiser.fill(m_window.data(), m_blockSamples);
        signalsmith::windows::forcePerfectReconstruction(
            m_window.data(), m_blockSamples, m_intervalSamples);

        return true;
    }

    bool updateFFT()
    {
        int newFFTSize = int(m_blockSamples * zeroPadding);
        m_fft.setFastSizeAbove(newFFTSize);

        m_fftBuffer.resize(m_fft.size());
        m_bandCount = int(m_fft.size() / 2);
        m_scalingFactor = 1.0f / float(m_fft.size());

        return true;
    }

    void allocateSpectralBuffers()
    {
        m_channelSpectra.resize(m_bandCount * m_channels);
        m_prevInputSpectra.resize(m_bandCount * m_channels);
        m_prevOutputSpectra.resize(m_bandCount * m_channels);
        m_outputRotations.resize(m_bandCount * m_channels);
        m_prevInputRotations.resize(m_bandCount);
        m_prevOutputRotations.resize(m_bandCount);
    }

    void timeShiftPhases(float shiftSamples, Complex* output) const
    {
        for (int b = 0; b < m_bandCount; ++b) {
            float freq  = (float(b) + 0.5f) / float(m_fft.size());
            float phase = freq * shiftSamples * (-2.0f * float(M_PI));
            output[b] = { std::cos(phase), std::sin(phase) };
        }
    }

    static Complex generateComplex(float energy, Complex complexPhase)
    {
        float norm = std::norm(complexPhase);
        if (norm > 0.0f) {
            return complexPhase * std::sqrt(energy / norm);
        } else {
            float phase = float(2.0f * M_PI) * float(std::rand()) / float(RAND_MAX);
            Complex cp{ std::cos(phase), std::sin(phase) };
            return std::sqrt(energy) * cp;
        }
    }

private:
    IDspChainElement* m_inputs[1] = { nullptr };

    int m_channels      = 0;
    int m_sampleRate    = 0;
    int m_blockSamples  = 0;
    int m_intervalSamples = 0;
    int m_bandCount     = 0;

    float m_surplusInputSamples = 0.0f;
    int   m_prevInputIndex      = 0;
    int   m_intervalCounter     = 0;

    signalsmith::delay::MultiBuffer<Sample> m_inputHistory;
    signalsmith::delay::MultiBuffer<Sample> m_summedOutput;

    std::vector<Sample>  m_blockBuffers;
    std::vector<Sample>  m_window;

    signalsmith::fft::ModifiedRealFFT<Sample> m_fft{1};
    float m_scalingFactor = 1.0f;
    std::vector<Sample>  m_fftBuffer;

    std::vector<Complex> m_channelSpectra;
    std::vector<Complex> m_prevInputSpectra;
    std::vector<Complex> m_prevOutputSpectra;
    std::vector<Complex> m_outputRotations;
    std::vector<Complex> m_prevInputRotations;
    std::vector<Complex> m_prevOutputRotations;

    std::vector<std::vector<Sample>> m_inPlanar;
    std::vector<std::vector<Sample>> m_outPlanar;
};


} // end namespace audio.
} // end namespace de.

