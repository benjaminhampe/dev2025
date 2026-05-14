#pragma once
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

struct AudioFifo {
    std::vector<float> L, R;
    size_t readPos = 0, writePos = 0, size = 0;

    AudioFifo(size_t capacity) {
        L.resize(capacity);
        R.resize(capacity);
        size = capacity;
    }

    size_t available() const {
        return (writePos + size - readPos) % size;
    }

    size_t freeSpace() const {
        return size - 1 - available();
    }

    void push(const float* inL, const float* inR, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            L[writePos] = inL[i];
            R[writePos] = inR[i];
            writePos = (writePos + 1) % size;
        }
    }

    void pop(float* outL, float* outR, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            outL[i] = L[readPos];
            outR[i] = R[readPos];
            readPos = (readPos + 1) % size;
        }
    }
};


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

    AudioFifo m_fifo;
public:
    DspResampler();
    ~DspResampler() override;

    void setSampleRateIn(u32 sampleRate);

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
