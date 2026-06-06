#pragma once
#include "Common.h"

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


