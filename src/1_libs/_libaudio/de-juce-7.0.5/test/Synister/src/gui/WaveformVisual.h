#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"

class WaveformVisual : public Component
{

public:

    WaveformVisual(eOscWaves waveformKey, float pulseWidth, float trngAmount)
        : m_iWaveformKey(waveformKey)
        , m_fPulseWidth(pulseWidth)
        , m_fTrngAmount(trngAmount)
    {
    }

    void setWaveformKey(eOscWaves waveformKey) { m_iWaveformKey = waveformKey; needNewNoise = true; repaint(); }
    void setPulseWidth(float pulseWidth) { m_fPulseWidth = pulseWidth; repaint(); }
    void setTrngAmount(float trngAmount) { m_fTrngAmount = trngAmount; repaint(); }

protected:
    void paint(Graphics &g);

private:
    eOscWaves m_iWaveformKey;
    float m_fPulseWidth;
    float m_fTrngAmount;

    Path noise;
    bool needNewNoise = true;
};
