#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

struct OSC_Saw
{
    float m_freq = 220.0f;
    int m_sampleRate = 44100;
    float m_sampleRateInv = 1.0f / 44100.0f;
    float m_volume = 100;
    int m_framePos = 0;
    // bool m_bBypassed = false;

    void noteOn(float freq, int sampleRate, int volume = 100)
    {
        m_freq = freq;
        m_sampleRate = sampleRate;
        m_sampleRateInv = 1.0f / float(m_sampleRate); // 2 is for saw wave period
        m_volume = 0.0001f * float(volume*volume);
        m_framePos = 0; // reset();
    }

    void reset()
    {
        m_framePos = 0;
    }

    float computeSample()
    {
        // Compute phase from integer framePos
        float phase = (m_freq * m_framePos) * m_sampleRateInv;

        // Extract fractional part (phase in [0,1))
        phase -= floorf(phase);

        // Increment integer framePos
        m_framePos++;
        m_framePos -= (m_framePos >= m_sampleRate) * m_sampleRate;

        // if (m_framePos >= m_sampleRate)
        //     m_framePos = 0;

        // Convert to saw [-1, +1]
        //float sample = (2.0f * phase) - 1.0f;
        float sample = sinf(float(2.0 * M_PI) * phase);
        return sample * m_volume;
    }

    //   |  /|   | ma = m_amp / (pulseWidth * m_period)
    //   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
    //  _|/  |   |
    //   | a | b/| pulseWidth = a / (a + b), T = 1 / f
    //   |   | / |
    //   |   |/  |   Like two mirrored ADSR without release phase,
};

} // end namespace audio.
} // end namespace de.
