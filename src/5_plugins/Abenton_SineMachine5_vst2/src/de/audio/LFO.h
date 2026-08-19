#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

/*
int main()
{
    LFO<float> lfo;
    lfo.m_frequency = 5.0f;
    lfo.m_amplitude = 0.8f;
    lfo.m_sampleRate = 48000.0f;
    lfo.m_function = &LFO::sine;
    lfo.updatePhaseInc();

    for (int i = 0; i < 10; ++i)
    {
        std::cout << "Sample " << i << ": " << lfo.process() << std::endl;
    }

    return 0;
}
*/

enum class eLFOFunction
{
    Disabled = 0,
    Sine,
    Rect,
    Saw,
    Triangle
};

template <typename T>
class LFO
{
public:
    // Member function pointer
    using LFOFunction = T (LFO::*)(T);
    // Member function pointer member:
    // (hopefully faster than std::function or a switch statement)
    LFOFunction m_function = &LFO::constant;
    T m_phase = T(0);           // Current phase in radians
    T m_phaseInc = T(1);
    T m_amplitude = T(1);       // Peak amplitude
    T m_frequency = T(1);       // Hz

    void init( T sampleRate )
    {
        m_phaseInc = T(de::TWO_PI64) * m_frequency / sampleRate;
    }

    void updatePhase() // Advance phase and wrap to [0, 2π]
    {
        m_phase += m_phaseInc;
        if (m_phase >= T(de::TWO_PI64))
            m_phase -= T(de::TWO_PI64);
    }

    // LFO shape impl - Constant wave
    T constant(T t)
    {
        updatePhase();
        return m_amplitude;
    }

    // LFO shape impl - Sine wave
    T sine(T t)
    {
        updatePhase();
        return m_amplitude * std::sin(m_phase);
    }

    // LFO shape impl - Square wave
    T square(T t)
    {
        updatePhase();
        return m_amplitude * (m_phase < M_PI ? 1.0f : -1.0f);
    }

    // LFO shape impl - Triangle wave
    T triangle(T t)
    {
        updatePhase();
        return m_amplitude * (2.0f * std::abs(2.0f * (m_phase / (2.0f * M_PI) - std::floor(m_phase / (2.0f * M_PI) + 0.5f))) - 1.0f);
    }

    // Process one sample
    float process()
    {
        return (this->*m_function)();
    }
};

} // end namespace audio.
} // end namespace de.
