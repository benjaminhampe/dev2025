#pragma once
#include <cmath>

/*
✔ Why this avoids clicks

This is a one‑pole exponential smoother:
    y[n]=a y[n−1]+(1−a) x[n]

Where:
    x[n] = target gain
    y[n] = smoothed gain
    a = e^(−1/(T⋅* fs))

This ensures:
    no discontinuities
    no sudden jumps
    no aliasing
    no zipper noise
    no clicks even with instant target changes

✔ What smoothing time should you use?

For gain changes:
    1–3 ms = extremely fast, no clicks
    5–10 ms = very smooth
    20+ ms = “fade‑y”

    2 ms is perfect for avoiding clicks
    5 ms is perfect for UI knob changes

Example:

    VolumeSmoother smoother(48000.0f, 2.0f); // 2 ms smoothing

✔ How to use it (per sample)

    float g = smoother.nextValue();
    out[i] = in[i] * g;

✔ How to use it (block)

    smoother.setTarget(newGain);
    smoother.processBlock(buffer, numSamples);
*/

class VolumeSmoother
{
public:
    VolumeSmoother(float sampleRate, float timeMs)
    {
        setTime(sampleRate, timeMs);
        m_current = 1.0f;
        m_target  = 1.0f;
    }

    // Set smoothing time in milliseconds
    void setTime(float sampleRate, float timeMs)
    {
        const float t = timeMs * 0.001f;
        const float coeff = std::exp(-1.0f / (t * sampleRate));
        m_coeff = coeff;
    }

    // Set new target gain (absolute, not normalized)
    inline void setTarget(float g)
    {
        m_target = g;
    }

    // Per-sample smoothing
    inline float nextValue()
    {
        // One-pole smoothing: y[n] = a*y[n-1] + (1-a)*target
        m_current = m_current * m_coeff + m_target * (1.0f - m_coeff);
        return m_current;
    }

    // Block smoothing: process N samples into out[]
    inline void processBlock(float* out, int n)
    {
        float c = m_coeff;
        float t = m_target;
        float y = m_current;

        for (int i = 0; i < n; ++i)
        {
            y = y * c + t * (1.0f - c);
            out[i] *= y;
        }

        m_current = y;
    }

    inline float current() const { return m_current; }

private:
    float m_current;
    float m_target;
    float m_coeff;
};
