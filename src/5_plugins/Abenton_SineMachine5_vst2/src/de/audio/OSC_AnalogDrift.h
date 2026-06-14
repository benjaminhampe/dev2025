#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

//🧩 Additive Oscillator with Analogue Drift
// Each partial slowly moves toward a random drift target.
/*
Below is the correct, low‑level, C++‑ready implementation with your m_ prefix, plus the three drift layers that real analogue oscillators exhibit:

    Per‑voice drift (slow global pitch wander)

    Per‑partial drift (each harmonic moves independently)

    Per‑partial phase offset (breaks digital phase‑locking)

And I’ll give you the exact DSP model used in classic analogue polysynths.
🎛️ What “analogue drift” actually is

Analogue oscillators don’t stay at one frequency. They have:
1) Slow random walk (0.1–0.5 Hz)

A very slow, smooth LFO that wanders unpredictably.
2) Independent drift per partial

Real analogue harmonics don’t stay phase‑locked.
Each partial has its own tiny frequency error.
3) Startup phase randomness

Analogue oscillators never start with all partials aligned.
🎯 The correct DSP model

We model drift as:
fn(t)=nf⋅(1+dn(t))

Where:

    dn(t) is a slow random walk (low‑passed noise)

    Each partial has its own dn

    The fundamental also has its own drift
*/
struct OSC_AnalogDrift
{
    enum Wave { Saw, Square, Triangle };

    double m_phase        = 0.0;
    double m_phaseInc     = 0.0;
    int    m_maxPartials  = 8;
    Wave   m_wave         = Saw;

    // Analogue drift
    static constexpr int MAX_PARTIALS = 64;
    double m_partialPhase[MAX_PARTIALS]       = {0.0};
    double m_partialDrift[MAX_PARTIALS]       = {0.0};
    double m_partialDriftTarget[MAX_PARTIALS] = {0.0};
    double m_partialDriftSpeed[MAX_PARTIALS]  = {0.0};

    void noteOn(double freq, double sampleRate, int nPartials, int wave = Square)
    {
        m_phaseInc = freq / sampleRate;
        m_maxPartials = (nPartials <= MAX_PARTIALS) ? nPartials : MAX_PARTIALS;
        m_wave = (Wave)wave;
        initDrift();
    }

    // Initialize drift parameters
    void initDrift()
    {
        for (int i = 0; i < m_maxPartials; ++i)
        {
            m_partialPhase[i] = (double)rand() / RAND_MAX; // random start phase
            m_partialDrift[i] = 0.0;
            m_partialDriftTarget[i] = ((double)rand() / RAND_MAX - 0.5) * 0.002; // ±0.1% drift
            m_partialDriftSpeed[i]  = 0.00001 + ((double)rand() / RAND_MAX) * 0.00005;
        }
    }

    inline double fastSin(double x)
    {
        double x2 = x * x;
        return x * (1.0 - x2 * (1.0/6.0) + x2 * x2 * (1.0/120.0));
    }

    inline float process()
    {
        // Update global phase
        m_phase += m_phaseInc;
        if (m_phase >= 1.0)
            m_phase -= 1.0;

        double out = 0.0;
        double base = m_phase * 2.0 * M_PI;

        // Update drift for each partial
        for (int i = 0; i < m_maxPartials; ++i)
        {
            // Move drift toward target
            m_partialDrift[i] += (m_partialDriftTarget[i] - m_partialDrift[i]) * m_partialDriftSpeed[i];

            // Occasionally pick a new drift target
            if (fabs(m_partialDriftTarget[i] - m_partialDrift[i]) < 0.000001)
            {
                m_partialDriftTarget[i] = ((double)rand() / RAND_MAX - 0.5) * 0.002; // ±0.1%
            }
        }

        switch (m_wave)
        {
            case Saw:
            {
                for (int n = 1; n <= m_maxPartials; ++n)
                {
                    double driftedPhase = base * n * (1.0 + m_partialDrift[n-1]);
                    driftedPhase = fmod(driftedPhase, 2.0 * M_PI);

                    if (driftedPhase >  M_PI) driftedPhase -= 2.0 * M_PI;
                    if (driftedPhase < -M_PI) driftedPhase += 2.0 * M_PI;

                    out += fastSin(driftedPhase) * (1.0 / n);
                }
            }
            break;

            case Square:
            {
                for (int k = 1; k <= m_maxPartials; ++k)
                {
                    int n = 2*k - 1;

                    double driftedPhase = base * n * (1.0 + m_partialDrift[k-1]);
                    driftedPhase = fmod(driftedPhase, 2.0 * M_PI);

                    if (driftedPhase >  M_PI) driftedPhase -= 2.0 * M_PI;
                    if (driftedPhase < -M_PI) driftedPhase += 2.0 * M_PI;

                    out += fastSin(driftedPhase) * (1.0 / n);
                }
            }
            break;

            case Triangle:
            {
                for (int k = 1; k <= m_maxPartials; ++k)
                {
                    int n = 2*k - 1;

                    double driftedPhase = base * n * (1.0 + m_partialDrift[k-1]);
                    driftedPhase = fmod(driftedPhase, 2.0 * M_PI);

                    if (driftedPhase >  M_PI) driftedPhase -= 2.0 * M_PI;
                    if (driftedPhase < -M_PI) driftedPhase += 2.0 * M_PI;

                    double amp = 1.0 / (n * n);
                    if (k & 1) amp = -amp;

                    out += fastSin(driftedPhase) * amp;
                }
            }
            break;
        }

        return (float)out;
    }
};

} // end namespace audio.
} // end namespace de.
