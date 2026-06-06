#pragma once
#include "Common.h"

class SmoothFloat
{
public:
    float m_current;
    float m_target;
    float m_step;
    int32_t m_remaining;
    int32_t m_smoothingSamples;

    SmoothFloat(float initial = 0.0f,
                int smoothingSamples = 0)
        : m_current(initial)
        , m_target(initial)
        , m_step(0.0f)
        , m_remaining(0)
        , m_smoothingSamples(smoothingSamples)
    {}

    // Set smoothing duration in samples (once)
    void setSmoothingSamples(int samples) { m_smoothingSamples = samples; }

    // Set target value directly
    void setTarget(float target)
    {
        m_target = target;
        m_remaining = m_smoothingSamples;
        m_step = (m_remaining > 0)
                     ? (m_target - m_current) / static_cast<float>(m_remaining)
                     : 0.0f;
    }

    // Get target value
    float getTarget() const { return m_target; }

    // Get current value (auto-interpolates)
    float getCurrent()
    {
        if (m_remaining > 0)
        {
            m_current += m_step;
            --m_remaining;
        }
        else
        {
            m_current = m_target;
        }
        return m_current;
    }


    // Implicit conversion to float
    operator float()
    {
        return getCurrent();
    }

};

/*
    // Set target in decibels
    void setTargetDb(float db)
    {
        setTarget(std::pow(10.0f, db / 20.0f));
    }
    // Get target in decibels
    float getTargetDb() const
    {
        return 20.0f * std::log10(m_target);
    }
    // Get current value in decibels
    float getCurrentDb()
    {
        return 20.0f * std::log10(getCurrent());
    }
*/

