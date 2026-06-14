#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

struct OSC_Additive
{
    enum Wave { Saw, Square, Triangle };

    double m_phase      = 0.0;
    double m_phaseInc   = 0.0;
    int    m_maxPartials = 8;
    Wave   m_wave        = Saw;

    void noteOn(double freq, double sampleRate, int nPartials, int wave = Saw)
    {
        m_phaseInc = freq / sampleRate;
        m_maxPartials = nPartials;
        m_wave = (Wave)wave;
    }

    inline double fastSin(double x)
    {
        // 5th-order polynomial
        double x2 = x * x;
        return x * (1.0 - x2 * (1.0/6.0) + x2 * x2 * (1.0/120.0));
    }

    inline float process()
    {
        m_phase += m_phaseInc;
        if (m_phase >= 1.0)
            m_phase -= 1.0;

        double out = 0.0;
        double base = m_phase * 2.0 * M_PI;

        switch (m_wave)
        {
            case Saw:
            {
                for (int n = 1; n <= m_maxPartials; ++n)
                {
                    double x = base * n;
                    x = fmod(x, 2.0 * M_PI);
                    if (x >  M_PI) x -= 2.0 * M_PI;
                    if (x < -M_PI) x += 2.0 * M_PI;

                    out += fastSin(x) * (1.0 / n);
                }
            }
            break;

            case Square:
            {
                for (int k = 1; k <= m_maxPartials; ++k)
                {
                    int n = 2*k - 1; // odd harmonics
                    double x = base * n;
                    x = fmod(x, 2.0 * M_PI);
                    if (x >  M_PI) x -= 2.0 * M_PI;
                    if (x < -M_PI) x += 2.0 * M_PI;

                    out += fastSin(x) * (1.0 / n);
                }
            }
            break;

            case Triangle:
            {
                for (int k = 1; k <= m_maxPartials; ++k)
                {
                    int n = 2*k - 1; // odd harmonics
                    double x = base * n;
                    x = fmod(x, 2.0 * M_PI);
                    if (x >  M_PI) x -= 2.0 * M_PI;
                    if (x < -M_PI) x += 2.0 * M_PI;

                    double amp = 1.0 / (n * n);
                    if (k & 1) amp = -amp; // alternating sign

                    out += fastSin(x) * amp;
                }
            }
            break;
        }

        return (float)out;
    }
};


} // end namespace audio.
} // end namespace de.


