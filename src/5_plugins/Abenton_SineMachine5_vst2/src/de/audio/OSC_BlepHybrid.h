#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

struct OSC_BlepHybrid
{
    float m_sampleRate = 48000.0f;
    float m_freq       = 440.0f;
    float m_phase      = 0.0f;   // [0,1)
    float m_lastTri    = 0.0f;

    enum Wave { Saw, Square, Triangle };
    Wave m_wave = Saw;

    void noteOn(float f, float sr, int wave = Triangle)
    {
        m_freq = f;
        m_sampleRate = sr;
        m_wave = (Wave)wave;
    }

    inline float polyBLEP(float t, float dt)
    {
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.0f;
        }
        else if (t > 1.0f - dt) {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        }
        return 0.0f;
    }

    inline float process()
    {
        float dt = m_freq / m_sampleRate;
        m_phase += dt;
        if (m_phase >= 1.0f)
            m_phase -= 1.0f;

        float t = m_phase;
        float out = 0.0f;

        switch (m_wave)
        {
            case Saw:
            {
                float s = 2.0f * t - 1.0f;
                s -= polyBLEP(t, dt);
                out = s;
            } break;

            case Square:
            {
                float s = (t < 0.5f) ? 1.0f : -1.0f;
                s += polyBLEP(t, dt);
                float t2 = fmodf(t + 0.5f, 1.0f);
                s -= polyBLEP(t2, dt);
                out = s;
            } break;

            case Triangle:
            {
                float sq = (t < 0.5f) ? 1.0f : -1.0f;
                sq += polyBLEP(t, dt);
                float t2 = fmodf(t + 0.5f, 1.0f);
                sq -= polyBLEP(t2, dt);

                m_lastTri += (2.0f * m_freq / m_sampleRate) * sq;
                m_lastTri *= 0.999f;

                out = m_lastTri;
            } break;
        }

        return out;
    }
};

} // end namespace audio.
} // end namespace de.


