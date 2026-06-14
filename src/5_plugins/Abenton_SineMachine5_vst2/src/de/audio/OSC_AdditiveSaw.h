#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {
    
struct OSC_AdditiveSaw
{
    double m_phase      = 0.0;   // [0,1)
    double m_phaseInc   = 0.0;   // phase increment per sample
    int    m_maxPartials = 8;    // number of harmonics to sum

    void noteOn(double freq, double sampleRate, int nPartials)
    {
        m_maxPartials = n;
        m_phaseInc = freq / sampleRate; // phase in [0,1)
    }

    inline float process()
    {
        m_phase += m_phaseInc;
        if (m_phase >= 1.0)
            m_phase -= 1.0;

        double out = 0.0;
        double base = m_phase * 2.0 * M_PI;

        for (int n = 1; n <= m_maxPartials; ++n)
        {
            double x = base * n;

            // range reduce to [-pi, pi]
            x = fmod(x, 2.0 * M_PI);
            if (x >  M_PI) x -= 2.0 * M_PI;
            if (x < -M_PI) x += 2.0 * M_PI;

            // 5th-order sine approx (minimax-ish / Taylor-ish)
            double x2 = x * x;
            double s  = x * (1.0 - x2 * (1.0/6.0) + x2 * x2 * (1.0/120.0));

            // saw: 1/n amplitude
            out += s * (1.0 / static_cast<double>(n));
        }

        return static_cast<float>(out);
    }
};


} // end namespace audio.
} // end namespace de.


