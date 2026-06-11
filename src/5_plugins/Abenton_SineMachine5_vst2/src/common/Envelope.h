#pragma once
#include "Common.h"

// NoteOnVelocity can influence global gain (linearly or quadratic)
// NoteOnVelocity can influence attack time (linearly or quadratic)
// NoteOnVelocity can influence decay time (linearly or quadratic)
// NoteOffVelocity can influence release time (linearly or quadratic)
// SustainPedal influences staying in Sustain Phase. (yet only)
/*
🧠 How to implement smoothing (simple + effective)

    When pedal is released:

    float smoothingTime = 0.02f; // 20 ms
    adsrParams.release = std::max(adsrParams.release, smoothingTime);

    Or a more piano‑like curve:

    float pedalReleaseFactor = 1.2f; // 20% longer release
    adsrParams.release *= pedalReleaseFactor;

    Or a micro‑fade:

    float fade = 1.0f - exp(-t * 80.0f); // 10–20 ms fade
*/

struct EnvelopeCfg
{
    float AttackTimeMs = 10.0f;   // init value
    float DecayTimeMs = 60.0f;    // init value
    float ReleaseTimeMs = 500.0f;
    float SampleRate = 0;
    float SustainLevel = 0.80f;   // init value
    int32_t AttackFrames = 480;   // init value
    int32_t DecayFrames = 480;    // init value
    int32_t ReleaseFrames = 48000;// init value
    bool bDampedSustain = { false }; // false = Synth Hold (no energy loss), true = Piano like energy loss while holding sustain pedal.
    bool bSingleShot = { false };
    bool bAltReleaseFunc = { false };
    bool bVeloSquaredGain = { false };
    bool bVeloAffectsGain = { false };   // Env(t) = ADSR(t) * envelope/127.0f;
    bool bVeloAffectsAttack = { false };
    bool bVeloAffectsDecay = { false };
    bool bVeloAffectsRelease = { false };
    //bool bQuadraticCurve = { false };
    //bool bQuadraticGain = { false }; // Linear = false, Squared = true
    //bool bQuadraticAttack = { false }; // Linear = false, Squared = true
    //bool bQuadraticDecay = { false }; // Linear = false, Squared = true
    //bool bQuadraticRelease = { false }; // Linear = false, Squared = true
};

class Envelope
{
public:
    enum ePhase : uint8_t
    {
        Idle = 0, // -> Not playing, accepts triggers
        Attack,
        Decay,
        Sustain,
        Release
    };
    const int32_t AttackMin = 8; // in [frames] prevent div by zero
    const int32_t AttackMax = 48000; // in [frames] 1 second
    const int32_t DecayMin = 8; // in [frames] prevent div by zero
    const int32_t DecayMax = 48000; // in [frames] 1 second
    const float SustainMin = 0.1f; // in [%]
    const float SustainMax = 1.0f; // in [%]
    const int32_t ReleaseMin = 8; // in [frames] prevent div by zero
    const int32_t ReleaseMax = 10*48000; // in [frames] 10 seconds
    const float VelocityMin = 1.0f / 127.0f; // Min (non-zero) value from MIDI
    const float VelocityMax = 1.0f;          // Max value from MIDI
    constexpr static float kMaxAmplitude = 1.0f;

//private:
    EnvelopeCfg m_cfg;

    int32_t m_attackFrames;  // real value (depends on params like velocity)
    int32_t m_decayFrames;   // real value (depends on params like velocity)
    int32_t m_releaseFrames; // real value (depends on params like velocity)
    int32_t m_frameCounter = 0;

    int32_t m_currentFrame = 0;
    uint8_t m_phase = Idle;
    bool m_bSustainPedal = 0;
    bool m_bTriggeredNoteOff = false;
    bool m_bReserved1 = false;

    float m_noteOnVelocity = 0.5f;
    float m_noteOffVelocity = 0.5f;

    float m_mAttack = .01f; // m = (dy/dx)
    float m_mDecay = -.01f; // m = (dy/dx)
    float m_mRelease = -.001f; // m = (dy/dx)

public:
    bool isPlaying() const { return m_phase != Idle; }

    void init(const EnvelopeCfg& cfg)
    {
        m_cfg = cfg;
        if (m_cfg.SampleRate > 1.0f)
        {
            m_cfg.AttackFrames = std::lround(0.001f * m_cfg.AttackTimeMs * m_cfg.SampleRate);
            m_cfg.DecayFrames = std::lround(0.001f * m_cfg.DecayTimeMs * m_cfg.SampleRate);
            m_cfg.ReleaseFrames = std::lround(0.001f * m_cfg.ReleaseTimeMs * m_cfg.SampleRate);
        }
        m_cfg.SustainLevel = std::clamp(m_cfg.SustainLevel,SustainMin,SustainMax);
        m_cfg.AttackFrames = std::clamp(m_cfg.AttackFrames,AttackMin,AttackMax);
        m_cfg.DecayFrames = std::clamp(m_cfg.DecayFrames,DecayMin,DecayMax);
        m_cfg.ReleaseFrames = std::clamp(m_cfg.ReleaseFrames,ReleaseMin,ReleaseMax);
        resetIdle();
    }

    void resetIdle()
    {
        m_phase = Idle;
        m_frameCounter = 0;
        m_currentFrame = 0;
    }

    void triggerNoteOn(float velocity = 0.5f )
    {
        if (m_phase != Idle)
        {
            DE_ERROR("Illegal retrigger attempt, abort.")
            return;
        }
        m_phase = Attack;
        m_frameCounter = 0;
        m_currentFrame = 0;

        // Amplitude = m_sustainLevel * m_noteOnVelocity;
        m_noteOnVelocity = std::clamp(velocity, VelocityMin, VelocityMax);

        if (m_cfg.bVeloAffectsAttack)
        {
            m_attackFrames = m_cfg.AttackFrames * m_noteOnVelocity;
        }
        else
        {
            m_attackFrames = m_cfg.AttackFrames;
        }

        if (m_cfg.bVeloAffectsDecay)
        {
            m_decayFrames = m_cfg.DecayFrames * m_noteOnVelocity;
        }
        else
        {
            m_decayFrames = m_cfg.DecayFrames;
        }

        m_mAttack = float(1.0f) / float(m_attackFrames);
        m_mDecay = float(m_cfg.SustainLevel - 1.0f) / float(m_decayFrames);
    }

    void triggerNoteOff(float velocity = 0.5f )
    {
        if (m_phase == Idle)
        {
            DE_ERROR("NoteOff before NoteOn, abort.")
            return;
        }

        if (m_bTriggeredNoteOff)
        {
            DE_ERROR("Illegal NoteOff trigger = ",m_bTriggeredNoteOff)
            return;
        }

        m_bTriggeredNoteOff = true;
        m_phase = Release;
        m_currentFrame = 0;
        m_noteOffVelocity = std::clamp(velocity, VelocityMin, VelocityMax);

        // float v = velocity / 127.0f;
        // float R = baseRelease * (1.5f - v * v); // expressive
        // float R = baseRelease * std::exp(-3.0f * v); // physical
        if (m_cfg.bVeloAffectsRelease)
        {
            if (m_cfg.bAltReleaseFunc)
            {
                // Physical piano dampening:
                m_releaseFrames = m_cfg.ReleaseFrames * std::exp(-3.0f * m_noteOffVelocity);
            }
            else
            {
                // Synth expressive dampening:
                m_releaseFrames = m_cfg.ReleaseFrames * (1.5f - m_noteOffVelocity * m_noteOffVelocity);
            }
        }
        else
        {
            m_releaseFrames = m_cfg.ReleaseFrames;
        }

        m_mRelease = -float(m_cfg.SustainLevel) / float(m_releaseFrames);
    }

    float nextSample();

    static void test();
    static void test1();
    static void test2();

    static void draw(Envelope & env,
                     int nCalls, de::Image & img,
                     const de::Recti& pos,
                     uint32_t color);
};


