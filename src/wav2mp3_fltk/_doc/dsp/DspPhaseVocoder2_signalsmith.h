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

class PhaseVocoderDspElement final : public IDspChainElement {
public:
    using Sample  = float;
    using Complex = std::complex<Sample>;

    PhaseVocoderDspElement()
    {
        // timeFactor: Zeit-Stretch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
        m_timeFactor = 1.0f;
        // freqFactor: Pitch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
        m_freqFactor = 1.0f;
        // blockMs: Blocklänge in ms (Default=120.0, Min=10.0, Max=500.0, Step=1.0)
        m_blockMs = 120.0f;
        // overlapFactor: Block/Schritt-Verhältnis (Default=4.0, Min=1.0, Max=8.0, Step=0.1)
        m_overlapFactor = 4.0f;
        // zeroPadding: FFT-Zero-Padding-Faktor (Default=2.0, Min=1.0, Max=8.0, Step=0.5)
        m_zeroPadding = 2.0f;
        // purePhase: true = reiner Phase-Vocoder, false = Energie-Mix (Default=false)
        m_purePhase = false;
        // gain: Ausgangs-Gain (Default=1.0, Min=0.0, Max=4.0, Step=0.01)
        m_gain = 1.0f;
    }

    std::string dsp_name() const override { return "PhaseVocoderDspElement"; }

    void dsp_init(uint64_t frames, uint32_t channels, uint32_t sampleRate) override
    {
        m_channels   = int(channels);
        m_sampleRate = int(sampleRate);

        m_blockSamples    = int(m_blockMs * 0.001f * m_sampleRate + 0.5f);
        m_intervalSamples = int(m_blockSamples / m_overlapFactor);

        m_invTimeFactor       = 1.0f / (m_timeFactor * m_freqFactor);
        m_surplusInputSamples = 0.0f;
        m_prevInputIndex      = 0;
        m_intervalCounter     = 0;

        // MultiBuffer: channels x samples
        m_inputHistory = signalsmith::delay::MultiBuffer<Sample>(m_channels,
                                                                 m_blockSamples + m_maxSurplusInputSamples);
        m_summedOutput = signalsmith::delay::MultiBuffer<Sample>(m_channels,
                                                                 m_blockSamples);

        m_blockBuffers.resize(m_blockSamples * m_channels);
        m_window.resize(m_blockSamples);

        // Fenster (Kaiser + Perfect Reconstruction)
        auto kaiser = signalsmith::windows::Kaiser::withBandwidth(
            m_blockSamples * 1.0 / m_intervalSamples, true);
        kaiser.fill(m_window.data(), m_blockSamples);
        signalsmith::windows::forcePerfectReconstruction(
            m_window.data(), m_blockSamples, m_intervalSamples);

        // FFT / Spektral-Setup
        m_fft.setFastSizeAbove(int(m_blockSamples * m_zeroPadding));
        m_fftBuffer.resize(m_fft.size());
        m_bandCount     = int(m_fft.size() / 2);
        m_scalingFactor = Sample(1.0f) / Sample(m_fft.size());

        m_channelSpectra.resize(m_bandCount * m_channels);
        m_prevInputSpectra.resize(m_bandCount * m_channels);
        m_prevOutputSpectra.resize(m_bandCount * m_channels);
        m_outputRotations.resize(m_bandCount * m_channels);
        m_prevInputRotations.resize(m_bandCount);
        m_prevOutputRotations.resize(m_bandCount);

        timeShiftPhases(-m_intervalSamples, m_prevOutputRotations.data());

        // Planar-IO-Buffers
        m_inPlanar.resize(m_channels);
        m_outPlanar.resize(m_channels);
        for (int c = 0; c < m_channels; ++c) {
            m_inPlanar[c].resize(m_blockSamples * 4);
            m_outPlanar[c].resize(m_blockSamples * 4);
        }
    }

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

        const int outputSamples = int(frames);
        const int inputSamples  = int(std::ceil(outputSamples * m_invTimeFactor - m_surplusInputSamples));

        // Planar-Buffergröße sicherstellen
        for (int c = 0; c < m_channels; ++c) {
            if (int(m_inPlanar[c].size()) < inputSamples)
                m_inPlanar[c].resize(inputSamples);
            if (int(m_outPlanar[c].size()) < outputSamples)
                m_outPlanar[c].resize(outputSamples);
        }

        // Upstream lesen (float planar)
        input->dsp_read(pts, inputSamples, sampleRate,
                        m_inPlanar[0].data(),
                        m_channels > 1 ? m_inPlanar[1].data() : nullptr);

        Sample* inputs[2]  = { m_inPlanar[0].data(),
                               m_channels > 1 ? m_inPlanar[1].data() : m_inPlanar[0].data() };
        Sample* outputs[2] = { m_outPlanar[0].data(),
                               m_channels > 1 ? m_outPlanar[1].data() : m_outPlanar[0].data() };

        int inputFilledTo = 0;

        // ============================
        // Hauptschleife: Output-Samples
        // ============================
        for (int o = 0; o < outputSamples; ++o) {

            // Block-Trigger
            if (++m_intervalCounter >= m_intervalSamples) {
                m_intervalCounter = 0;

                // Block-Start im Input
                int inputStart = int(std::round(o * m_invTimeFactor
                                                - m_surplusInputSamples
                                                - m_blockSamples));
                inputStart = std::max(
                    std::min(inputStart, inputSamples - m_blockSamples),
                    -m_maxSurplusInputSamples - m_blockSamples
                );

                // Input-History auffüllen + Fenster anwenden
                for (int c = 0; c < m_channels; ++c) {
                    auto inputBuf  = inputs[c];
                    auto history   = m_inputHistory[c];

                    for (int i = inputFilledTo; i < inputStart + m_blockSamples; ++i)
                        history[i] = inputBuf[i];

                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    for (int i = 0; i < m_blockSamples; ++i)
                        block[i] = history[inputStart + i] * m_window[i];
                }

                // FFT pro Kanal
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block   = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    for (int i = 0; i < m_blockSamples; ++i)
                        m_fftBuffer[i] = block[i];
                    for (int i = m_blockSamples; i < int(m_fftBuffer.size()); ++i)
                        m_fftBuffer[i] = Sample(0);

                    m_fft.fft(m_fftBuffer.data(), spectrum);
                }

                // Phase-Vocoder
                int inputIntervalSamples = inputStart - m_prevInputIndex;
                m_prevInputIndex = inputStart;

                Sample timeFactor = inputIntervalSamples > 0
                    ? Sample(m_intervalSamples) / Sample(inputIntervalSamples)
                    : Sample(0);

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
                            Sample rotationAbs = std::abs(rotation);
                            Sample phase = std::arg(rotation) * timeFactor;

                            m_outputRotations[b] = {
                                rotationAbs * std::cos(phase),
                                rotationAbs * std::sin(phase)
                            };
                            prevIn[b] = curr[b];
                        }

                        Sample outputEnergy = std::norm(curr[b]);
                        Complex complexPhase = prevOut[b] * m_outputRotations[b];

                        if (!m_purePhase) {
                            Sample existingEnergy = std::min(std::norm(prevOut[b]), outputEnergy);
                            Sample newEnergy      = outputEnergy - existingEnergy;
                            complexPhase = existingEnergy * complexPhase + newEnergy * curr[b];
                        }

                        curr[b] = generateComplex(outputEnergy, complexPhase);
                        curr[b] *= m_gain;
                        prevOut[b] = curr[b];
                    }
                }

                // IFFT pro Kanal
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block   = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    m_fft.ifft(spectrum, m_fftBuffer.data());
                    for (int i = 0; i < m_blockSamples; ++i)
                        block[i] = m_fftBuffer[i] * m_scalingFactor;
                }

                // Overlap-Add Summation
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    auto outBuf   = m_summedOutput[c];
                    for (int i = 0; i < m_blockSamples; ++i)
                        outBuf[i] += block[i] * m_window[i];
                }
            }

            // Ein Sample aus Summen-Output ausgeben
            for (int c = 0; c < m_channels; ++c) {
                outputs[c][o] = m_summedOutput[c][0];
                m_summedOutput[c][0] = Sample(0);
            }
            ++m_summedOutput;
        }

        // Restliches Input in History kopieren
        for (int c = 0; c < m_channels; ++c) {
            auto inputBuf  = inputs[c];
            auto history   = m_inputHistory[c];
            for (int i = inputFilledTo; i < inputSamples; ++i)
                history[i] = inputBuf[i];
        }
        m_inputHistory += inputSamples;
        m_prevInputIndex -= inputSamples;
        m_surplusInputSamples += inputSamples - outputSamples * m_invTimeFactor;

        // Output nach L/R kopieren
        for (int i = 0; i < outputSamples; ++i) {
            L[i] = m_outPlanar[0][i];
            if (m_channels > 1 && R)
                R[i] = m_outPlanar[1][i];
        }
    }

    uint32_t dsp_getInputSignalCount() const override { return 1; }
    IDspChainElement* dsp_getInputSignal(int i = 0) override { return m_inputs[i]; }
    void dsp_setInputSignal(IDspChainElement* in, int i = 0) override { m_inputs[i] = in; }
    void dsp_clearInputSignals() override { m_inputs[0] = nullptr; }

    // Zeit-Stretch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    void set_timeFactor(float f) { m_timeFactor = f; m_invTimeFactor = 1.0f / (m_timeFactor * m_freqFactor); }
    // Pitch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    void set_freqFactor(float f) { m_freqFactor = f; m_invTimeFactor = 1.0f / (m_timeFactor * m_freqFactor); }
    // Blocklänge in ms (Default=120.0, Min=10.0, Max=500.0, Step=1.0)
    void set_blockMs(float ms)   { m_blockMs = ms; }
    // Overlap-Faktor (Default=4.0, Min=1.0, Max=8.0, Step=0.1)
    void set_overlapFactor(float o) { m_overlapFactor = o; }
    // Zero-Padding-Faktor (Default=2.0, Min=1.0, Max=8.0, Step=0.5)
    void set_zeroPadding(float z)   { m_zeroPadding = z; }
    // Reiner Phase-Vocoder (Default=false)
    void set_purePhase(bool b)      { m_purePhase = b; }
    // Ausgangs-Gain (Default=1.0, Min=0.0, Max=4.0, Step=0.01)
    void set_gain(float g)          { m_gain = g; }

private:
    void timeShiftPhases(Sample shiftSamples, Complex* output) const {
        for (int b = 0; b < m_bandCount; ++b) {
            Sample freq  = (Sample(b) + Sample(0.5f)) / Sample(m_fft.size());
            Sample phase = freq * shiftSamples * Sample(-2.0f * M_PI);
            output[b] = { std::cos(phase), std::sin(phase) };
        }
    }

    static Complex generateComplex(Sample energy, Complex complexPhase) {
        Sample norm = std::norm(complexPhase);
        if (norm > Sample(0)) {
            return complexPhase * std::sqrt(energy / norm);
        } else {
            Sample phase = Sample(2.0f * M_PI) * Sample(std::rand()) / Sample(RAND_MAX);
            Complex cp{ std::cos(phase), std::sin(phase) };
            return std::sqrt(energy) * cp;
        }
    }

    IDspChainElement* m_inputs[1] = { nullptr };

    int   m_channels      = 0;
    int   m_sampleRate    = 0;
    int   m_blockSamples  = 0;
    int   m_intervalSamples = 0;
    int   m_bandCount     = 0;
    int   m_maxSurplusInputSamples = 0;

    Sample m_timeFactor;
    Sample m_freqFactor;
    Sample m_blockMs;
    Sample m_overlapFactor;
    Sample m_zeroPadding;
    bool   m_purePhase;
    Sample m_gain;

    Sample m_invTimeFactor       = 1.0f;
    Sample m_surplusInputSamples = 0.0f;
    int    m_prevInputIndex      = 0;
    int    m_intervalCounter     = 0;

    signalsmith::delay::MultiBuffer<Sample> m_inputHistory;
    signalsmith::delay::MultiBuffer<Sample> m_summedOutput;
    std::vector<Sample>  m_blockBuffers;
    std::vector<Sample>  m_window;

    signalsmith::fft::ModifiedRealFFT<Sample> m_fft{1};
    Sample m_scalingFactor = 1.0f;
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

/*
🧩 Beispiel: Verwendung der PhaseVocoderDspElement‑Klasse
cpp

#include "PhaseVocoderDspElement.h"

// Dummy-Soundquelle für das Beispiel
class DummyInput final : public IDspChainElement {
public:
    std::string dsp_name() const override { return "DummyInput"; }

    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override {
        m_channels = channels;
        m_sampleRate = sampleRate;
    }

    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                  float* __restrict__ L,
                  float* __restrict__ R) override
    {
        // Einfach ein Sinus als Testsignal
        double freq = 440.0;
        for (u32 i = 0; i < frames; ++i) {
            float s = float(std::sin(2.0 * M_PI * freq * (pts + i) / m_sampleRate));
            L[i] = s;
            if (m_channels > 1) R[i] = s;
        }
    }

    u32 dsp_getInputSignalCount() const override { return 0; }
    IDspChainElement* dsp_getInputSignal(int) override { return nullptr; }
    void dsp_setInputSignal(IDspChainElement*, int) override {}
    void dsp_clearInputSignals() override {}

private:
    u32 m_channels = 2;
    u32 m_sampleRate = 48000;
};

int main() {
    const u32 sampleRate = 48000;
    const u32 channels   = 2;
    const u32 frames     = 512;

    // --- Input-Signal ---
    DummyInput input;
    input.dsp_init(frames, channels, sampleRate);

    // --- PhaseVocoder DSP ---
    PhaseVocoderDspElement pv;

    // Parameter setzen (alle dokumentiert in der Klasse)
    pv.set_timeFactor(2.0);        // doppelt so langsam
    pv.set_freqFactor(1.0);        // Pitch unverändert
    pv.set_blockMs(120.0);         // Blockgröße 120ms
    pv.set_overlapFactor(4.0);     // Overlap 4x
    pv.set_zeroPadding(2.0);       // FFT Zero-Padding 2x
    pv.set_purePhase(false);       // Energie-Mix statt reinem Phase-Vocoder
    pv.set_gain(1.0);              // Normaler Gain

    // Input anschließen
    pv.dsp_setInputSignal(&input);

    // DSP initialisieren
    pv.dsp_init(frames, channels, sampleRate);

    // Output-Buffers
    std::vector<float> L(frames);
    std::vector<float> R(frames);

    // Rendern
    pv.dsp_read(
        0.0, // pts
        frames,
        sampleRate,
        L.data(),
        R.data());

    // Jetzt sind L/R die Phase-Vocoder-Ausgabe
    return 0;
}

🔥 Hier ist die korrekte, effiziente Lösung

Ich zeige dir nur den korrigierten Teil, damit du ihn direkt ersetzen kannst.
🧩 Schritt 1 — Maximalgrößen definieren
cpp

static constexpr int MAX_BLOCK_SAMPLES = 8192;     // frei wählbar
static constexpr int MAX_CHANNELS      = 2;        // stereo
static constexpr int MAX_FFT_SIZE      = 32768;    // frei wählbar
static constexpr int MAX_HISTORY       = 65536;    // frei wählbar

Diese Werte sind konstant, du kannst sie anpassen.
🧩 Schritt 2 — MultiBuffer einmalig erzeugen
cpp

m_inputHistory = MultiBuffer<Sample>(MAX_CHANNELS, MAX_HISTORY);
m_summedOutput = MultiBuffer<Sample>(MAX_CHANNELS, MAX_BLOCK_SAMPLES);

Nur einmal im Konstruktor oder dsp_init.
🧩 Schritt 3 — BlockSize ändern ohne Re‑Allokation
cpp

void set_blockMs(double ms) {
    m_blockMs = ms;

    m_blockSamples = int(m_blockMs * 0.001 * m_sampleRate + 0.5);
    if (m_blockSamples > MAX_BLOCK_SAMPLES)
        m_blockSamples = MAX_BLOCK_SAMPLES;

    m_intervalSamples = int(m_blockSamples / m_overlapFactor);

    // Fenster neu berechnen
    auto kaiser = signalsmith::windows::Kaiser::withBandwidth(
        m_blockSamples * 1.0 / m_intervalSamples, true);
    kaiser.fill(m_window.data(), m_blockSamples);

    signalsmith::windows::forcePerfectReconstruction(
        m_window.data(), m_blockSamples, m_intervalSamples);
}

Keine Allokation.
🧩 Schritt 4 — FFT‑Größe ändern ohne Re‑Allokation
cpp

void set_zeroPadding(double z) {
    m_zeroPadding = z;

    int newFFTSize = int(m_blockSamples * m_zeroPadding);
    if (newFFTSize > MAX_FFT_SIZE)
        newFFTSize = MAX_FFT_SIZE;

    m_fft.setFastSizeAbove(newFFTSize);

    // Buffer nur bis MAX_FFT_SIZE allokiert
    // wir nutzen nur die ersten m_fft.size() Samples
}

🧩 Schritt 5 — Planar‑Buffers einmalig allokieren
cpp

for (int c = 0; c < MAX_CHANNELS; ++c) {
    m_inPlanar[c].resize(MAX_HISTORY);
    m_outPlanar[c].resize(MAX_HISTORY);
}

🎯 Ergebnis

Damit erreichst du:
✔ BlockSize kann jederzeit geändert werden

ohne Allokation, ohne Re‑Init.
✔ FFT‑Größe kann jederzeit geändert werden

ohne Re‑Allokation.
✔ Overlap‑Add arbeitet immer mit maximalen Buffern

und nutzt nur die ersten m_blockSamples.
✔ Realtime‑safe

keine Allokationen im Audio‑Thread.
✔ Keine Indirektion

alles direkt sichtbar.








// ============================================================
// PhaseVocoderDspElement
// ============================================================
class PhaseVocoderDspElement final : public IDspChainElement {
public:
    using Sample  = float;
    using Complex = std::complex<Sample>;

    PhaseVocoderDspElement()
    {
        // --- Zeit-/Frequenz-Parameter ---
        // timeFactor: Zeit-Stretch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
        m_timeFactor = 1.0;
        // freqFactor: Pitch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
        m_freqFactor = 1.0;

        // --- Block-/Overlap-Parameter ---
        // blockMs: Blocklänge in Millisekunden (Default=120.0, Min=10.0, Max=500.0, Step=1.0)
        m_blockMs = 120.0;
        // overlapFactor: Verhältnis Block/Schritt (Default=4.0, Min=1.0, Max=8.0, Step=0.1)
        m_overlapFactor = 4.0;

        // --- Spektral-Parameter ---
        // zeroPadding: FFT-Zero-Padding-Faktor (Default=2.0, Min=1.0, Max=8.0, Step=0.5)
        m_zeroPadding = 2.0;

        // --- Phase-Vocoder-Parameter ---
        // purePhase: true = reiner Phase-Vocoder, false = Energie-Mix (Default=false)
        m_purePhase = false;
        // gain: Ausgangs-Gain (Default=1.0, Min=0.0, Max=4.0, Step=0.01)
        m_gain = 1.0;
    }

    std::string dsp_name() const override {
        return "PhaseVocoderDspElement";
    }

    // ============================================================
    // dsp_init
    // ============================================================
    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override
    {
        m_channels   = int(channels);
        m_sampleRate = int(sampleRate);

        // Block-/Intervallgrößen wie im Beispiel
        m_blockSamples    = int(m_blockMs * 0.001 * m_sampleRate + 0.5);
        m_intervalSamples = int(m_blockSamples / m_overlapFactor);

        // --- Overlap-Add / Zeit-Domain Setup ---
        // m_inputHistory.configure(m_channels, m_blockSamples + m_maxSurplusInputSamples);
        // m_summedOutput.configure(m_channels, m_blockSamples);

        // --- Overlap-Add / Zeit-Domain Setup ---
        m_inputHistory = signalsmith::delay::MultiBuffer<Sample>(
            m_channels,
            m_blockSamples + m_maxSurplusInputSamples
        );

        m_summedOutput = signalsmith::delay::MultiBuffer<Sample>(
            m_channels,
            m_blockSamples
        );


        m_blockBuffers.resize(m_blockSamples * m_channels);
        m_window.resize(m_blockSamples);

        // Fenster: Kaiser für Spektral, sonst Sinus/Hann
        auto kaiser = signalsmith::windows::Kaiser::withBandwidth(
            m_blockSamples * 1.0 / m_intervalSamples, true);
        kaiser.fill(m_window.data(), m_blockSamples);
        signalsmith::windows::forcePerfectReconstruction(
            m_window.data(), m_blockSamples, m_intervalSamples);

        m_intervalCounter     = 0;
        m_invTimeFactor       = 1.0 / (m_timeFactor * m_freqFactor);
        m_surplusInputSamples = 0.0;
        m_prevInputIndex      = 0;

        // --- FFT / Spektral-Setup ---
        m_fft.setFastSizeAbove(int(m_blockSamples * m_zeroPadding));
        m_fftBuffer.resize(m_fft.size());
        m_bandCount     = int(m_fft.size() / 2);
        m_scalingFactor = 1.0 / m_fft.size();

        m_channelSpectra.resize(m_bandCount * m_channels);

        // --- Phase-Vocoder-Setup ---
        m_prevInputSpectra.resize(m_bandCount * m_channels);
        m_prevOutputSpectra.resize(m_bandCount * m_channels);
        m_outputRotations.resize(m_bandCount * m_channels);

        m_prevInputRotations.resize(m_bandCount);
        m_prevOutputRotations.resize(m_bandCount);

        // Initiale Output-Rotation: Zeitverschiebung um -intervalSamples
        timeShiftPhases(-m_intervalSamples, m_prevOutputRotations.data());

        // --- Planar-IO-Buffers für DSP-Chain ---
        m_inPlanar.resize(m_channels);
        m_outPlanar.resize(m_channels);
        for (int c = 0; c < m_channels; ++c) {
            m_inPlanar[c].resize(m_blockSamples * 4);
            m_outPlanar[c].resize(m_blockSamples * 4);
        }
    }

    void process(Sample* const* inputs, int inputSamples,
                 Sample** outputs, int outputSamples)
    {
        // ============================================================
        // 1) Overlap-Add Scheduler: Sample für Sample durchlaufen
        // ============================================================
        int inputFilledTo = 0;

        for (int o = 0; o < outputSamples; ++o) {

            // ------------------------------------------------------------
            // Block-Trigger: Wenn genug Samples vergangen sind → neuen Block starten
            // ------------------------------------------------------------
            if (++m_intervalCounter >= m_intervalSamples) {
                m_intervalCounter = 0;

                // ------------------------------------------------------------
                // 2) Block-Start im Input bestimmen (Zeit-Stretch-Formel)
                // ------------------------------------------------------------
                int inputStart = int(std::round(o * m_invTimeFactor
                                                - m_surplusInputSamples
                                                - m_blockSamples));

                // Begrenzen, damit wir nicht außerhalb des Buffers lesen
                inputStart = std::max(
                    std::min(inputStart, inputSamples - m_blockSamples),
                    -m_maxSurplusInputSamples - m_blockSamples
                );

                // ------------------------------------------------------------
                // 3) Input-History auffüllen + Fenster anwenden
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    auto input   = inputs[c];
                    auto history = m_inputHistory[c];

                    // History auffüllen
                    for (int i = inputFilledTo; i < inputStart + m_blockSamples; ++i) {
                        history[i] = input[i];
                    }

                    // Block aus History + Fenster
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    for (int i = 0; i < m_blockSamples; ++i) {
                        block[i] = history[inputStart + i] * m_window[i];
                    }
                }

                // ------------------------------------------------------------
                // 4) FFT pro Kanal
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    // Block in FFT-Buffer kopieren
                    for (int i = 0; i < m_blockSamples; ++i)
                        m_fftBuffer[i] = block[i];

                    // Zero-Padding
                    for (int i = m_blockSamples; i < int(m_fftBuffer.size()); ++i)
                        m_fftBuffer[i] = 0;

                    // FFT
                    m_fft.fft(m_fftBuffer.data(), spectrum);
                }

                // ------------------------------------------------------------
                // 5) Phase-Vocoder: Spektren bearbeiten
                // ------------------------------------------------------------
                int inputIntervalSamples = inputStart - m_prevInputIndex;
                m_prevInputIndex = inputStart;

                // Zeit-Faktor für Phase-Rotation
                Sample timeFactor = inputIntervalSamples > 0
                    ? m_intervalSamples / Sample(inputIntervalSamples)
                    : 0;

                // Alte Spektren zeitlich verschieben
                timeShiftPhases(-inputIntervalSamples, m_prevInputRotations.data());
                for (int c = 0; c < m_channels; ++c) {
                    Complex* prevIn  = m_prevInputSpectra.data()  + c*m_bandCount;
                    Complex* prevOut = m_prevOutputSpectra.data() + c*m_bandCount;

                    for (int b = 0; b < m_bandCount; ++b) {
                        prevIn[b]  *= m_prevInputRotations[b];
                        prevOut[b] *= m_prevOutputRotations[b];
                    }
                }

                // Haupt-Phase-Vocoder-Schleife
                for (int c = 0; c < m_channels; ++c) {
                    Complex* curr    = m_channelSpectra.data() + c*m_bandCount;
                    Complex* prevIn  = m_prevInputSpectra.data() + c*m_bandCount;
                    Complex* prevOut = m_prevOutputSpectra.data() + c*m_bandCount;

                    for (int b = 0; b < m_bandCount; ++b) {

                        // Phase-Rotation aus Input-Historie
                        if (inputIntervalSamples > 0) {
                            Complex rotation = curr[b] * std::conj(prevIn[b]);
                            Sample rotationAbs = std::abs(rotation);
                            Sample phase = std::arg(rotation) * timeFactor;

                            m_outputRotations[b] = {
                                rotationAbs * std::cos(phase),
                                rotationAbs * std::sin(phase)
                            };

                            prevIn[b] = curr[b];
                        }

                        // Energie bestimmen
                        Sample outputEnergy = std::norm(curr[b]);

                        // Phase fortsetzen
                        Complex complexPhase = prevOut[b] * m_outputRotations[b];

                        // Energie-Mix (falls purePhase=false)
                        if (!m_purePhase) {
                            Sample existingEnergy = std::min(std::norm(prevOut[b]), outputEnergy);
                            Sample newEnergy      = outputEnergy - existingEnergy;
                            complexPhase = existingEnergy * complexPhase + newEnergy * curr[b];
                        }

                        // Rekonstruktion
                        curr[b] = generateComplex(outputEnergy, complexPhase);
                        curr[b] *= m_gain;

                        prevOut[b] = curr[b];
                    }
                }

                // ------------------------------------------------------------
                // 6) IFFT pro Kanal
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    Complex* spectrum = m_channelSpectra.data() + c*m_bandCount;

                    m_fft.ifft(spectrum, m_fftBuffer.data());

                    for (int i = 0; i < m_blockSamples; ++i)
                        block[i] = m_fftBuffer[i] * m_scalingFactor;
                }

                // ------------------------------------------------------------
                // 7) Overlap-Add Summation
                // ------------------------------------------------------------
                for (int c = 0; c < m_channels; ++c) {
                    Sample* block = m_blockBuffers.data() + c*m_blockSamples;
                    auto out = m_summedOutput[c];

                    for (int i = 0; i < m_blockSamples; ++i)
                        out[i] += block[i] * m_window[i];
                }
            }

            // ------------------------------------------------------------
            // 8) Ein Sample aus Summen-Output ausgeben
            // ------------------------------------------------------------
            for (int c = 0; c < m_channels; ++c) {
                outputs[c][o] = m_summedOutput[c][0];
                m_summedOutput[c][0] = 0;
            }

            ++m_summedOutput;
        }

        // ------------------------------------------------------------
        // 9) Restliches Input in History kopieren
        // ------------------------------------------------------------
        for (int c = 0; c < m_channels; ++c) {
            auto input  = inputs[c];
            auto history = m_inputHistory[c];
            for (int i = inputFilledTo; i < inputSamples; ++i)
                history[i] = input[i];
        }

        m_inputHistory += inputSamples;
        m_prevInputIndex -= inputSamples;
        m_surplusInputSamples += inputSamples - outputSamples * m_invTimeFactor;
    }

#if 0
    // ============================================================
    // dsp_read
    // ============================================================
    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R) override
    {
        IDspChainElement* input = m_inputs[0];
        if (!input) {
            std::memset(L, 0, sizeof(float)*frames);
            std::memset(R, 0, sizeof(float)*frames);
            return;
        }

        const int outputSamples = int(frames);
        const int inputSamples  = samplesForOutput(outputSamples);

        // Planar-Buffergröße sicherstellen
        for (int c = 0; c < m_channels; ++c) {
            if (int(m_inPlanar[c].size()) < inputSamples)
                m_inPlanar[c].resize(inputSamples);
            if (int(m_outPlanar[c].size()) < outputSamples)
                m_outPlanar[c].resize(outputSamples);
        }

        // Upstream lesen (planar L/R)
        input->dsp_read(pts, inputSamples, sampleRate,
                        m_inPlanar[0].data(),
                        m_channels > 1 ? m_inPlanar[1].data() : nullptr);

        // Pointer-Arrays für Overlap-Add/FFT/Phase-Vocoder
        std::vector<Sample*> inPtrs(m_channels);
        std::vector<Sample*> outPtrs(m_channels);
        for (int c = 0; c < m_channels; ++c) {
            inPtrs[c]  = m_inPlanar[c].data();
            outPtrs[c] = m_outPlanar[c].data();
        }

        // Hauptprozess: Overlap-Add + FFT + Phase-Vocoder + IFFT
        process(inPtrs.data(), inputSamples, outPtrs.data(), outputSamples);

        // Zurück in L/R
        for (int i = 0; i < outputSamples; ++i) {
            L[i] = float(m_outPlanar[0][i]);
            if (m_channels > 1)
                R[i] = float(m_outPlanar[1][i]);
        }
    }


#endif


    // ============================================================
    // Input handling
    // ============================================================
    u32 dsp_getInputSignalCount() const override { return 1; }
    IDspChainElement* dsp_getInputSignal(int i = 0) override { return m_inputs[i]; }
    void dsp_setInputSignal(IDspChainElement* in, int i = 0) override { m_inputs[i] = in; }
    void dsp_clearInputSignals() override { m_inputs[0] = nullptr; }

    // ============================================================
    // Realtime-Parameter-Setters (mit Doku)
    // ============================================================

    // Zeit-Stretch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    void set_timeFactor(double f) {
        m_timeFactor = f;
        m_invTimeFactor = 1.0 / (m_timeFactor * m_freqFactor);
    }

    // Pitch-Faktor (Default=1.0, Min=0.25, Max=4.0, Step=0.01)
    void set_freqFactor(double f) {
        m_freqFactor = f;
        m_invTimeFactor = 1.0 / (m_timeFactor * m_freqFactor);
    }

    // Blocklänge in ms (Default=120.0, Min=10.0, Max=500.0, Step=1.0)
    void set_blockMs(double ms) {
        m_blockMs = ms;
    }

    // Overlap-Faktor (Block/Schritt) (Default=4.0, Min=1.0, Max=8.0, Step=0.1)
    void set_overlapFactor(double o) {
        m_overlapFactor = o;
    }

    // Zero-Padding-Faktor für FFT (Default=2.0, Min=1.0, Max=8.0, Step=0.5)
    void set_zeroPadding(double z) {
        m_zeroPadding = z;
    }

    // Reiner Phase-Vocoder (true) oder Energie-Mix (false) (Default=false)
    void set_purePhase(bool b) {
        m_purePhase = b;
    }

    // Ausgangs-Gain (Default=1.0, Min=0.0, Max=4.0, Step=0.01)
    void set_gain(double g) {
        m_gain = g;
    }

private:
    // ============================================================
    // Overlap-Add / Zeit-Domain
    // ============================================================
    int samplesForOutput(int outputSamples) const {
        double inputSamples = outputSamples * m_invTimeFactor - m_surplusInputSamples;
        return int(std::ceil(inputSamples));
    }

    void process(Sample* const* inputs, int inputSamples,
                 Sample** outputs, int outputSamples)
    {
        int inputFilledTo = 0;

        for (int o = 0; o < outputSamples; ++o) {
            if (++m_intervalCounter >= m_intervalSamples) {
                m_intervalCounter = 0;

                // Block-Start im Input (wie OverlapAddStretch)
                int inputStart = int(std::round(o * m_invTimeFactor
                                                - m_surplusInputSamples
                                                - m_blockSamples));
                inputStart = std::max(
                    std::min(inputStart, inputSamples - m_blockSamples),
                    -m_maxSurplusInputSamples - m_blockSamples);

                // Input-History auffüllen
                for (int c = 0; c < m_channels; ++c) {
                    auto input  = inputs[c];
                    auto history = m_inputHistory[c];
                    for (int i = inputFilledTo; i < inputStart + m_blockSamples; ++i) {
                        history[i] = input[i];
                    }
                    // Block aus History + Fenster
                    Sample* blockBuffer = channelBlock(c);
                    for (int i = 0; i < m_blockSamples; ++i) {
                        blockBuffer[i] = history[inputStart + i] * m_window[i];
                    }
                }

                // Spektral-Processing (FFT + Phase-Vocoder + IFFT)
                processBlock(inputStart - m_prevInputIndex);
                m_prevInputIndex = inputStart;

                // Block in Summen-Output addieren (Overlap-Add)
                for (int c = 0; c < m_channels; ++c) {
                    Sample* blockBuffer = channelBlock(c);
                    auto output = m_summedOutput[c];
                    for (int i = 0; i < m_blockSamples; ++i) {
                        output[i] += blockBuffer[i] * m_window[i];
                    }
                }
            }

            // Ein Sample aus Summen-Output ausgeben
            for (int c = 0; c < m_channels; ++c) {
                outputs[c][o] = m_summedOutput[c][0];
                m_summedOutput[c][0] = 0;
            }
            ++m_summedOutput;
        }

        // Restliches Input in History kopieren
        for (int c = 0; c < m_channels; ++c) {
            auto input  = inputs[c];
            auto history = m_inputHistory[c];
            for (int i = inputFilledTo; i < inputSamples; ++i) {
                history[i] = input[i];
            }
        }
        m_inputHistory += inputSamples;
        m_prevInputIndex -= inputSamples;
        m_surplusInputSamples += inputSamples - outputSamples * m_invTimeFactor;
    }

    Sample* channelBlock(int channel) {
        return m_blockBuffers.data() + channel * m_blockSamples;
    }

    // ============================================================
    // FFT / Spektral-Block
    // ============================================================
    Complex* channelSpectrum(int channel) {
        return m_channelSpectra.data() + channel * m_bandCount;
    }

    void timeShiftPhases(Sample shiftSamples, Complex* output) const {
        for (int b = 0; b < m_bandCount; ++b) {
            Sample phase = bandToFreq(b) * shiftSamples * (-2 * M_PI);
            output[b] = { std::cos(phase), std::sin(phase) };
        }
    }

    Sample bandToFreq(Sample band) const {
        return (band + 0.5) / m_fft.size();
    }

    void processBlock(int inputIntervalSamples)
    {
        // FFT pro Kanal
        for (int c = 0; c < m_channels; ++c) {
            Sample* block   = channelBlock(c);
            Complex* spectrum = channelSpectrum(c);
            for (int i = 0; i < m_blockSamples; ++i) {
                m_fftBuffer[i] = block[i];
            }
            for (int i = m_blockSamples; i < int(m_fftBuffer.size()); ++i) {
                m_fftBuffer[i] = 0;
            }
            m_fft.fft(m_fftBuffer.data(), spectrum);
        }

        // Phase-Vocoder-Spektrum bearbeiten
        processSpectrum(inputIntervalSamples);

        // IFFT pro Kanal
        for (int c = 0; c < m_channels; ++c) {
            Sample* block   = channelBlock(c);
            Complex* spectrum = channelSpectrum(c);
            m_fft.ifft(spectrum, m_fftBuffer.data());
            for (int i = 0; i < m_blockSamples; ++i) {
                block[i] = m_fftBuffer[i] * m_scalingFactor;
            }
        }
    }

    // ============================================================
    // Phase-Vocoder-Spektrum
    // ============================================================
    Complex* prevInputSpectrum(int channel) {
        return m_prevInputSpectra.data() + channel * m_bandCount;
    }
    Complex* prevOutputSpectrum(int channel) {
        return m_prevOutputSpectra.data() + channel * m_bandCount;
    }

    static Complex generateComplex(Sample energy, Complex complexPhase) {
        Sample complexPhaseNorm = std::norm(complexPhase);
        if (complexPhaseNorm > 0) {
            return complexPhase * std::sqrt(energy / complexPhaseNorm);
        } else {
            Sample phase = Sample(2 * M_PI) * std::rand() / RAND_MAX;
            Complex cp = { std::cos(phase), std::sin(phase) };
            return std::sqrt(energy) * cp;
        }
    }

    void processSpectrum(int inputIntervalSamples)
    {
        Sample timeFactor = inputIntervalSamples > 0
            ? m_intervalSamples / Sample(inputIntervalSamples)
            : 0;

        // Vorherige Input/Output-Spektren zeitlich verschieben
        timeShiftPhases(-inputIntervalSamples, m_prevInputRotations.data());
        for (int c = 0; c < m_channels; ++c) {
            Complex* prevInputBands  = prevInputSpectrum(c);
            Complex* prevOutputBands = prevOutputSpectrum(c);
            for (int b = 0; b < m_bandCount; ++b) {
                prevInputBands[b]  *= m_prevInputRotations[b];
                prevOutputBands[b] *= m_prevOutputRotations[b];
            }
        }

        // Haupt-Phase-Vocoder-Loop
        for (int c = 0; c < m_channels; ++c) {
            Complex* currentBands   = channelSpectrum(c);
            Complex* prevInputBands = prevInputSpectrum(c);
            Complex* prevOutputBands = prevOutputSpectrum(c);

            for (int b = 0; b < m_bandCount; ++b) {
                if (inputIntervalSamples > 0) {
                    Complex rotation = currentBands[b] * std::conj(prevInputBands[b]);
                    Sample rotationAbs = std::abs(rotation);
                    Sample phase = std::arg(rotation) * timeFactor;
                    m_outputRotations[b] = { rotationAbs * std::cos(phase),
                                             rotationAbs * std::sin(phase) };
                    prevInputBands[b] = currentBands[b];
                }

                Sample outputEnergy = std::norm(currentBands[b]);
                Complex complexPhase = prevOutputBands[b] * m_outputRotations[b];

                if (!m_purePhase) {
                    Sample existingEnergy = std::min(std::norm(prevOutputBands[b]), outputEnergy);
                    Sample newEnergy      = outputEnergy - existingEnergy;
                    complexPhase = existingEnergy * complexPhase + newEnergy * currentBands[b];
                }

                currentBands[b] = generateComplex(outputEnergy, complexPhase);
                currentBands[b] *= m_gain;
                prevOutputBands[b] = currentBands[b];
            }
        }
    }

private:
    // Upstream
    IDspChainElement* m_inputs[1] = { nullptr };

    // Basis
    int m_channels      = 0;
    int m_sampleRate    = 0;
    int m_blockSamples  = 0;
    int m_intervalSamples = 0;

    // Overlap-Add-State
    signalsmith::delay::MultiBuffer<Sample> m_inputHistory;
    signalsmith::delay::MultiBuffer<Sample> m_summedOutput;
    std::vector<Sample> m_blockBuffers;
    std::vector<Sample> m_window;

    int    m_maxSurplusInputSamples = 0;
    double m_invTimeFactor          = 1.0;
    double m_surplusInputSamples    = 0.0;
    int    m_prevInputIndex         = 0;
    int    m_intervalCounter        = 0;

    // FFT/Spektral-State
    signalsmith::fft::ModifiedRealFFT<Sample> m_fft{1};
    int    m_bandCount     = 0;
    Sample m_scalingFactor = 1.0;
    std::vector<Sample>  m_fftBuffer;
    std::vector<Complex> m_channelSpectra;

    // Phase-Vocoder-State
    bool m_purePhase = false;
    double m_gain    = 1.0;

    std::vector<Complex> m_prevInputSpectra;
    std::vector<Complex> m_prevOutputSpectra;
    std::vector<Complex> m_outputRotations;
    std::vector<Complex> m_prevInputRotations;
    std::vector<Complex> m_prevOutputRotations;

    // Planar-IO für DSP-Chain
    std::vector<std::vector<Sample>> m_inPlanar;
    std::vector<std::vector<Sample>> m_outPlanar;

    // Parameter
    double m_timeFactor;
    double m_freqFactor;
    double m_blockMs;
    double m_overlapFactor;
    double m_zeroPadding;
};

*/
