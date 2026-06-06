#pragma once
#include <common/Common.h>
#include <common/LFO.h>

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

constexpr int NUM_PARTIALS = 48;

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

