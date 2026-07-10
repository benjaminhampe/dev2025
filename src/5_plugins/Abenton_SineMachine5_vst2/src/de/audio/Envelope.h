#pragma once
#include <DarkImage.h>

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
    void dump() const
    {
        DE_DEBUG("EnvelopeCfg:")
        DE_DEBUG("AttackTimeMs = ",AttackTimeMs)
        DE_DEBUG("DecayTimeMs = ",DecayTimeMs)
        DE_DEBUG("ReleaseTimeMs = ",ReleaseTimeMs)
        DE_DEBUG("SampleRate = ",SampleRate)
        DE_DEBUG("SustainLevel = ",SustainLevel)
        DE_DEBUG("bDampedSustain = ",bDampedSustain)
        DE_DEBUG("bSingleShot = ",bSingleShot)
        DE_DEBUG("bAltReleaseFunc = ",bAltReleaseFunc)
        DE_DEBUG("bVeloSquaredGain = ",bVeloSquaredGain)
        DE_DEBUG("bVeloAffectsGain = ",bVeloAffectsGain)
        DE_DEBUG("bVeloAffectsAttack = ",bVeloAffectsAttack)
        DE_DEBUG("bVeloAffectsDecay = ",bVeloAffectsDecay)
        DE_DEBUG("bVeloAffectsRelease = ",bVeloAffectsRelease)
    }
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

    std::string phaseStr() const
    {
        switch (m_phase)
        {
            case Idle: return "Idle";
            case Attack: return "Attack";
            case Decay: return "Decay";
            case Sustain: return "Sustain";
            case Release: return "Release";
            default: return "Unknown";
        }
    }

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

    int32_t m_baseAttackFrames;  // real value (depends on params like velocity)
    int32_t m_baseDecayFrames;   // real value (depends on params like velocity)
    int32_t m_baseReleaseFrames; // real value (depends on params like velocity)
    float m_baseSustainLevel;
    int32_t m_attackFrames;  // real value (depends on params like velocity)
    int32_t m_decayFrames;   // real value (depends on params like velocity)
    int32_t m_releaseFrames; // real value (depends on params like velocity)
    float m_sustainLevel;

    int32_t m_currentFrame = 0;
    int32_t m_frameCounter = 0;

    uint8_t m_phase = Idle;
    bool m_bOK = false;
    bool m_bSustainPedal = 0;
    bool m_bTriggeredNoteOff = false;
    bool m_bReserved1 = false;

    float m_noteOnVelocity = 0.5f;
    float m_noteOffVelocity = 0.5f;

    float m_mAttack = .01f; // m = (dy/dx)
    float m_mDecay = -.01f; // m = (dy/dx)
    float m_mRelease = -.001f; // m = (dy/dx)
    float m_lastOutput = 0.0f;
    float m_releaseStart = 0.0f;

public:
    bool isPlaying() const { return m_phase != Idle; }

    void init(const EnvelopeCfg& cfg)
    {
        m_cfg = cfg;
        if (m_cfg.SampleRate > 1.0f)
        {
            m_baseAttackFrames = std::lround(0.001f * m_cfg.AttackTimeMs * m_cfg.SampleRate);
            m_baseDecayFrames = std::lround(0.001f * m_cfg.DecayTimeMs * m_cfg.SampleRate);
            m_baseReleaseFrames = std::lround(0.001f * m_cfg.ReleaseTimeMs * m_cfg.SampleRate);
            m_baseSustainLevel = std::clamp(m_cfg.SustainLevel,SustainMin,SustainMax);
            m_baseAttackFrames = std::clamp(m_baseAttackFrames,AttackMin,AttackMax);
            m_baseDecayFrames = std::clamp(m_baseDecayFrames,DecayMin,DecayMax);
            m_baseReleaseFrames = std::clamp(m_baseReleaseFrames,ReleaseMin,ReleaseMax);
            m_bOK = true;
        }
        else
        {
            DE_ERROR("Failed with invalid sampleRate ", cfg.SampleRate)
            m_bOK = false;
        }

        // cfg.dump();
        // DE_OK("m_bOK = ",m_bOK)
        // DE_OK("m_baseAttackFrames = ",m_baseAttackFrames)
        // DE_OK("m_baseDecayFrames = ",m_baseDecayFrames)
        // DE_OK("m_baseReleaseFrames = ",m_baseReleaseFrames)
        // DE_OK("m_baseSustainLevel = ",m_baseSustainLevel)

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
        if (!m_bOK)
        {
            DE_ERROR("Not OK, abort.")
            return;
        }

        if (m_phase != Idle)
        {
            DE_ERROR("Illegal retrigger attempt, abort.")
            return;
        }

        m_bTriggeredNoteOff = false;
        m_phase = Attack;
        m_currentFrame = 0;
        m_frameCounter = 0;
        m_noteOnVelocity = std::clamp(velocity, VelocityMin, VelocityMax);

        float attackFrames = m_baseAttackFrames;
        if (m_cfg.bVeloAffectsAttack)
        {
            attackFrames *= (1.0f - m_noteOnVelocity);
        }
        m_attackFrames = std::max(long(1),std::lroundf(attackFrames));
        m_mAttack = float(1.0f) / float(m_attackFrames);

        float sustainLevel = m_baseSustainLevel;
        if (m_cfg.bVeloAffectsGain)
        {
            sustainLevel *= m_noteOnVelocity;
        }
        m_sustainLevel = sustainLevel;

        m_decayFrames = std::max(1,m_baseDecayFrames);
        m_mDecay = float(m_sustainLevel - 1.0f) / float(m_decayFrames);

        // DE_BENNI("m_noteOnVelocity = ",m_noteOnVelocity)
        // DE_BENNI("m_attackFrames = ",m_attackFrames)
        // DE_BENNI("m_decayFrames = ",m_decayFrames)
        // DE_BENNI("m_sustainLevel = ",m_sustainLevel)
        // DE_BENNI("m_mAttack = ",m_mAttack)
        // DE_BENNI("m_mDecay = ",m_mDecay)
    }

    void triggerNoteOff(float velocity = 0.5f )
    {
        if (!m_bOK) { DE_ERROR("Not OK") return; }
        if (m_phase == Idle) { DE_ERROR("Not Idle") return; }
        if (m_bTriggeredNoteOff) { DE_ERROR("Triggered") return; }

        m_noteOffVelocity = std::clamp(velocity, VelocityMin, VelocityMax);
        m_bTriggeredNoteOff = true;
        // m_phase = Release;
        //m_currentFrame = 0;

        m_releaseStart = m_lastOutput;
        m_releaseFrames = std::max(1,m_baseReleaseFrames);
        m_mRelease = -m_sustainLevel / float(m_releaseFrames);
/*
        // float v = velocity / 127.0f;
        // float R = baseRelease * (1.5f - v * v); // expressive
        // float R = baseRelease * std::exp(-3.0f * v); // physical
        if (m_cfg.bVeloAffectsRelease)
        {
            if (m_cfg.bAltReleaseFunc)
            {
                // Physical piano dampening:
                m_releaseFrames = m_baseReleaseFrames * std::exp(-3.0f * m_noteOffVelocity);
            }
            else
            {
                // Synth expressive dampening:
                m_releaseFrames = m_baseReleaseFrames * (1.5f - m_noteOffVelocity * m_noteOffVelocity);
            }
        }
        else
        {
            m_releaseFrames = m_baseReleaseFrames;
        }
*/

        // DE_WARN("m_noteOffVelocity = ",m_noteOffVelocity)
        // DE_WARN("m_releaseFrames = ",m_releaseFrames)
        // DE_WARN("m_mRelease = ",m_mRelease)
        // DE_WARN("m_phase = ",phaseStr())
        // DE_WARN("m_currentFrame = ",m_currentFrame)
    }

    float process();

    static void test();
    static void test1();
    static void test2();

    static void draw(Envelope & env,
                     int nCalls, de::Image & img,
                     const de::Recti& pos,
                     uint32_t color);
};


