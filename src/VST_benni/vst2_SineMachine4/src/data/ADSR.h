#pragma once
#include <cstdint>
#include <algorithm>

class ADSR
{
public:
    enum ePhase { Idle = 0, Attack, Decay, Sustain, Release };

//private:
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

public:

    static constexpr float kMaxAmplitude = 1.0f;

    ADSR()
        : m_attackSamples(480)
        , m_decaySamples(480)
        , m_sustainLevel(0.7f)
        , m_releaseSamples(4800)
        , m_phase(Idle)
        , m_currentSample(0)
        , m_noteOnVelocity(kMaxAmplitude)
        , m_noteOffVelocity(kMaxAmplitude)
        , m_amplitude(0.0f)
        , m_noteOffTriggered(false)
    {
    }

    void set(   int32_t attackSamples,
                int32_t decaySamples,
                float sustainLevel,
                int32_t releaseSamples)
    {
        m_attackSamples = attackSamples;
        m_decaySamples = decaySamples;
        m_sustainLevel = std::clamp(sustainLevel, 0.0f, 1.0f);
        m_releaseSamples = releaseSamples;
        reset();
    }

    void reset()
    {
        m_phase = Idle;
        m_currentSample = 0;
        m_noteOnVelocity = kMaxAmplitude;
        m_noteOffVelocity = kMaxAmplitude;
        m_amplitude = 0.0f;
        m_noteOffTriggered = false;
    }

    void noteOn(float velocity = kMaxAmplitude)
    {
        m_phase = Attack;
        m_currentSample = 0;
        m_noteOnVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
        m_noteOffTriggered = false;
    }

    void noteOff(float velocity = kMaxAmplitude)
    {
        m_noteOffVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
        m_noteOffTriggered = true;
        if (m_phase != Release)
        {
            m_phase = Release;
            m_currentSample = 0;
        }
    }

    void triggerSingleShot(float velocity = kMaxAmplitude)
    {
        noteOn(velocity);
        noteOff(velocity);
    }

    float nextSample()
    {
        switch (m_phase)
        {
            case Idle:
            {
                m_amplitude = 0.0f;
                break;
            }

            case Attack:
            {
                // Attack:      ____ 1
                //             /|        y = m * x + n
                //            / |        m = dy / dx
                //           /  |dy      n = 0
                //     0 ___/___|       dy = m_noteOnVelocity
                //            dx        dx = m_attackSamples
                //
                if (m_currentSample < m_attackSamples)
                {
                    const float dx = m_attackSamples;
                    const float dy = m_noteOnVelocity;
                    const float m = dy / dx;
                    const float n = 0.0f;
                    // Linear function: y = m * x + n
                    m_amplitude = m * m_currentSample + n;
                    ++m_currentSample;
                }
                else
                {
                    m_amplitude = m_noteOnVelocity;
                    m_phase = Decay;
                    m_currentSample = 0;
                }
                break;
            }

            case Decay:
            {
                // Decay:
                //    m_noteOnVel __  dx                    y = m * x + n
                //                  \                       m = dy / dx
                //                   \   dy                 n = m_noteOnVelocity;
                //                    \                     dy = -(m_noteOnVelocity - m_sustainLevel)
                //                     \___ m_sustainLevel  dx = m_decaySamples
                //
                if (m_currentSample < m_decaySamples)
                {
                    float dx = m_decaySamples;
                    float dy = (m_sustainLevel - m_noteOnVelocity);
                    float m = dy / dx;
                    float n = m_noteOnVelocity;
                    // Linear function: y = m * x + n
                    m_amplitude = m * m_currentSample + n;
                    ++m_currentSample;
                }
                else
                {
                    m_amplitude = m_sustainLevel;
                    m_phase = Sustain;
                    m_currentSample = 0;
                }
                break;
            }

            case Sustain:
            {
                // Constant function: y = m * x + n
                // m = 0
                // n = sustainLevel
                m_amplitude = m_sustainLevel;
                if (m_noteOffTriggered)
                {
                    m_phase = Release;
                    m_currentSample = 0;
                }
                break;
            }

            case Release:
            {
                // Release:
                //    m_sustainLevel ___            y = m * x + n
                //                     |\           m = dy / dx
                //                     | \          n = m_sustainLevel
                //                  dy |  \        dy = -m_sustainLevel
                //                     |___\___ 0  dx = m_releaseSamples
                //                      dx

                // Linear function: y = m * x + n
                // m = -sustainLevel * noteOffVelocity / releaseSamples
                // n = sustainLevel * noteOffVelocity
                if (m_currentSample < m_releaseSamples)
                {
                    float m = -m_sustainLevel / m_releaseSamples; //  * m_noteOffVelocity
                    float n = m_sustainLevel; //  * m_noteOffVelocity
                    m_amplitude = m * m_currentSample + n;
                    ++m_currentSample;
                }
                else
                {
                    m_phase = Idle;
                    m_amplitude = 0.0f;
                }
                break;
            }
        }

        return std::clamp(m_amplitude, 0.0f, 1.0f); // Limiter
    }

    bool isActive() const
    {
        return m_phase != Idle;
    }


};



#if 0
🔁 Looping Envelope Diff (Synth Concept)
✅ What’s Added
    A m_loopEnabled flag
    Logic to restart the envelope from Attack after Sustain if looping is active
    No while loop — still one sample per call

class ADSREnvelope {
public:
    static constexpr float kMaxAmplitude = 1.0f;

    ADSREnvelope(int32_t attackSamples,
                 int32_t decaySamples,
                 float sustainLevel,
                 int32_t releaseSamples)
        : m_attackSamples(attackSamples)
        , m_decaySamples(decaySamples)
        , m_sustainLevel(std::clamp(sustainLevel, 0.0f, kMaxAmplitude))
        , m_releaseSamples(releaseSamples)
        , m_phase(ADSRPhase::Idle)
        , m_currentSample(0)
        , m_noteOnVelocity(kMaxAmplitude)
        , m_noteOffVelocity(kMaxAmplitude)
        , m_amplitude(0.0f)
        , m_noteOffTriggered(false)
+       , m_loopEnabled(false)
    {
    }

+   void setLoopEnabled(bool enabled)
+   {
+       m_loopEnabled = enabled;
+   }

    float nextSample()
    {
        switch (m_phase)
        {
        ...
            case ADSRPhase::Sustain:
            {
                m_amplitude = m_sustainLevel;
-               if (m_noteOffTriggered) {
-                   m_phase = ADSRPhase::Release;
-                   m_currentSample = 0;
-               }
+               if (m_loopEnabled && !m_noteOffTriggered) {
+                   m_phase = ADSRPhase::Attack;
+                   m_currentSample = 0;
+               } else if (m_noteOffTriggered) {
+                   m_phase = ADSRPhase::Release;
+                   m_currentSample = 0;
+               }
                break;
            }
            ...
        }

        return m_amplitude;
    }

private:
    ...
+   bool m_loopEnabled;
};



#endif



#if 0




#include <cstdint>
#include <cmath>
#include <algorithm>

enum class ADSRPhase {
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
};

class ADSREnvelope {
public:
    static constexpr float kMaxAmplitude = 1.0f;
    static constexpr float kMinAmplitude = 0.0001f;

    ADSREnvelope(int32_t attackSamples,
                 int32_t decaySamples,
                 float sustainLevel,
                 int32_t releaseSamples)
        : m_attackSamples(attackSamples)
        , m_decaySamples(decaySamples)
        , m_sustainLevel(std::clamp(sustainLevel, 0.0f, kMaxAmplitude))
        , m_releaseSamples(releaseSamples)
        , m_phase(ADSRPhase::Idle)
        , m_currentSample(0)
        , m_noteOnVelocity(kMaxAmplitude)
        , m_noteOffVelocity(kMaxAmplitude)
        , m_amplitude(0.0f)
        , m_noteOffTriggered(false)
    {
    }

    void noteOn(float velocity = kMaxAmplitude)
    {
        m_phase = ADSRPhase::Attack;
        m_currentSample = 0;
        m_noteOnVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
        m_noteOffTriggered = false;
    }

    void noteOff(float velocity = kMaxAmplitude)
    {
        m_noteOffVelocity = std::clamp(velocity, 0.0f, kMaxAmplitude);
        m_noteOffTriggered = true;
        if (m_phase != ADSRPhase::Release) {
            m_phase = ADSRPhase::Release;
            m_currentSample = 0;
        }
    }

    void triggerSingleShot(float velocity = kMaxAmplitude)
    {
        noteOn(velocity);
        noteOff(velocity);
    }

    float nextSample()
    {
        switch (m_phase) {
        case ADSRPhase::Idle: {
            m_amplitude = 0.0f;
            break;
        }

        case ADSRPhase::Attack: {
            // Exponential rise: y = A * (1 - e^(-k * x))
            // A = noteOnVelocity
            // k = ln(100) / attackSamples
            if (m_currentSample < m_attackSamples) {
                float k = std::log(100.0f) / m_attackSamples;
                float x = static_cast<float>(m_currentSample);
                m_amplitude = m_noteOnVelocity * (1.0f - std::exp(-k * x));
                ++m_currentSample;
            } else {
                m_phase = ADSRPhase::Decay;
                m_currentSample = 0;
            }
            break;
        }

        case ADSRPhase::Decay: {
            // Exponential fall: y = (1 - sustainLevel) * e^(-k * x) + sustainLevel
            // k = ln(100) / decaySamples
            if (m_currentSample < m_decaySamples) {
                float k = std::log(100.0f) / m_decaySamples;
                float x = static_cast<float>(m_currentSample);
                m_amplitude = (kMaxAmplitude - m_sustainLevel) * std::exp(-k * x) + m_sustainLevel;
                ++m_currentSample;
            } else {
                m_phase = ADSRPhase::Sustain;
                m_currentSample = 0;
            }
            break;
        }

        case ADSRPhase::Sustain: {
            // Constant: y = sustainLevel
            m_amplitude = m_sustainLevel;
            if (m_noteOffTriggered) {
                m_phase = ADSRPhase::Release;
                m_currentSample = 0;
            }
            break;
        }

        case ADSRPhase::Release: {
            // Exponential fall: y = sustainLevel * e^(-k * x)
            // k = ln(100) / releaseSamples
            if (m_currentSample < m_releaseSamples) {
                float k = std::log(100.0f) / m_releaseSamples;
                float x = static_cast<float>(m_currentSample);
                m_amplitude = m_sustainLevel * std::exp(-k * x) * m_noteOffVelocity;
                m_amplitude = std::max(m_amplitude, kMinAmplitude);
                ++m_currentSample;
            } else {
                m_phase = ADSRPhase::Idle;
                m_amplitude = 0.0f;
            }
            break;
        }
        }

        return m_amplitude;
    }

    bool isActive() const
    {
        return m_phase != ADSRPhase::Idle;
    }

private:
    int32_t m_attackSamples;
    int32_t m_decaySamples;
    float   m_sustainLevel;
    int32_t m_releaseSamples;

    ADSRPhase m_phase;
    int32_t   m_currentSample;

    float m_noteOnVelocity;
    float m_noteOffVelocity;
    float m_amplitude;

    bool m_noteOffTriggered;
};













#endif
