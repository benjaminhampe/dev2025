#pragma once
#include <DarkImage.h>
#include <DarkGPU.h>
// #include <cstdint>
// #include <string>
// #include <cmath>
// #include <algorithm>
// #include <thread>

#include <pluginterfaces/vst2.x/audioeffectx.h>

#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include <nanovg.h>
#include <nanovg_gl.h>

// #include <GLFW/glfw3.h> // Bad: GLFW uses TopLevelWindows, not well managable by Hosts.
// #include <de_Approx_Math.h>
// #include <de/approx_math.h>
// #include <de/de_aligned_memory.h>

DE_FORCE_INLINE void
DSP_RESIZE(de::TAlignedVector<float> & out, size_t n)
{
    if (n > 0 && out.size() != n)
    {
        out.resize(n);
    }
}

DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out)
{
    std::fill(out.begin(),out.end(),0.0f);
}

/*
DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out, size_t n, size_t start = 0)
{

    for (size_t i = start; i < n-start; i++)
    {
        out.at(i) = 0.0f;
    }
}
*/

DE_FORCE_INLINE void
DSP_ADD(float* __restrict__ pOut, const float* __restrict__ pIn, size_t n)
{
    DE_ASSUME_NO_OVERLAP(pOut,pIn,n*sizeof(float));

    for (size_t i = 0; i < n; i++)
    {
        pOut[i] += pIn[i];
    }
}


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


class ADSR
{
public:
    enum ePhase { Idle = 0, Attack, Decay, Sustain, Release };

    constexpr static float kMaxAmplitude = 1.0f;

//private:
    int32_t m_frameIndex;
    int32_t m_attackSamples;
    int32_t m_decaySamples;
    float   m_sustainLevel;
    int32_t m_releaseSamples;
    ePhase  m_phase;
    int32_t m_currentSample;
    float m_noteOnVelocity;
    float m_noteOffVelocity;
    float m_amplitude;
    bool m_noteOffTriggered;
    bool m_singleShot;

public:


    ADSR();

    uint32_t getFrameIndex() const
    {
        return m_currentSample;
    }

    uint32_t getFrameCount() const
    {
        return m_attackSamples +
               m_decaySamples +
               m_releaseSamples;
    }

    void init(  float attackTimeInSec,
                float decayTimeInSec,
                float sustainLevel,
                float releaseTimeInSec,
                float sampleRate)
    {
        set(std::roundl(sampleRate * attackTimeInSec),
            std::roundl(sampleRate * decayTimeInSec), sustainLevel,
            std::roundl(sampleRate * releaseTimeInSec) );
    }

    void set(   int32_t attackSamples,
                int32_t decaySamples, float sustainLevel,
                int32_t releaseSamples);

    void reset();
    void noteOn(float velocity = kMaxAmplitude);
    void noteOff(float velocity = kMaxAmplitude);
    void setSingleShot(bool bSingleShot);
    float nextSample();
    bool isActive() const;
};



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

enum class eFunction
{
    Const = 0,
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
        m_phaseInc = T(de::TWO_PI) * m_frequency / sampleRate;
    }

    void updatePhase() // Advance phase and wrap to [0, 2π]
    {
        m_phase += m_phaseInc;
        if (m_phase >= T(de::TWO_PI))
            m_phase -= T(de::TWO_PI);
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

// 📊
struct Partial
{
    enum eFlags
    {
        eF_None = 0,
        eF_Audible = 1, // For playing, probably redundant to fAmplitude = 0.
        eF_Editable = 2, // For editing. Used to quick draw only odd/even/oct partials.
        eF_SingleShot = 4,
        eF_Default = eF_Audible | eF_Editable | eF_SingleShot
    };

    int16_t iPartial; // partial number: 1,2,N x Even and Odd multiples of BaseFrequency = Harmonics/Partials
    uint16_t uFlags = eF_Default;
    float fAmplitude;       // For playing and editing.
    float fFrequency;       // For playing, for preview it's f = 1Hz * iPartial
    float fDetuneCent;      // For playing and editing. Max. (+-100) detune in cent
    float fPhase;           // For playing
    float fPhaseIncrement;  // For playing
    float fAmplitudeSum;    // For normalization of SUM(partial[i].A,i,0,iPartial)
    float fAmplitudeSumInv; // For normalization of SUM(partial[i].A,i,0,iPartial), mul is faster.
    NVGcolor color;         // For editing

    float lfoRate = 1.0f;
    float lfoMin = -1.0f;
    float lfoMax =  1.0f;
    eFunction lfoFunc = eFunction::Const; // == Deactive

    std::string str() const
    {
        return std::to_string(iPartial);
    }

    int32_t N() const { return iPartial; }

    float A() const { return fAmplitude; }
};

// 🎼
struct Cfg
{
    std::vector<Partial> m_partials;
    int32_t m_masterVolume = 100; // in [%]
    int32_t m_unisono = 4;
    float m_partialVolumeSum = 1.0f;
    float m_partialVolumeSumInv = 1.0f;

    bool m_singleShot = true;
    // ADSR m_adsr;

    float m_envAttackTimeInMs = 30.0f;
    float m_envDecayTimeInMs = 60.0f;
    float m_envSustainLevel = 0.9f;
    float m_envReleaseTimeInMs = 500.0f;
    float m_sampleRate = 48000.f;

    Cfg();
    ~Cfg();

    size_t getNumPartials() const;
    void setNumPartials(int numPartials);
    void setPartial(int index, float amplitude, double centDetune = 0.0);
    void setDefaultColors();
    void setDefaultPartialsToRect();
    void setDefaultPartialsToSaw();
    void setDefaultPartialsToSawRev();
    void setDefaultPartialsToTriangle();
};

