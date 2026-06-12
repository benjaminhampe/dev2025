#pragma once
#include <common/Common.h>
#include <common/Envelope.h>
#include <common/LFO.h>

// 📊
struct PartialCfg
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
    float fDetuneCent;      // For playing and editing. Max. (+-100) detune in cent
    NVGcolor color;         // For editing

    float lfoRate = 1.0f;
    float lfoMin = -1.0f;
    float lfoMax =  1.0f;
    eFunction lfoFunc = eFunction::Disabled;

    // float fFrequency;       // For playing, for preview it's f = 1Hz * iPartial
    // float fPhase;           // For playing
    // float fPhaseIncrement;  // For playing

    std::string str() const
    {
        return std::to_string(iPartial);
    }

    int32_t N() const { return iPartial; }

    float A() const { return fAmplitude; }
};

constexpr int NUM_PARTIALS = 48;

// 📊
struct PartialsCfg
{
    std::array<PartialCfg,NUM_PARTIALS> m_partials;
    // float fAmplitudeSum;    // For normalization of SUM(partial[i].A,i,0,iPartial)
    // float fAmplitudeSumInv; // For normalization of SUM(partial[i].A,i,0,iPartial), mul is faster.
    // float m_partialVolumeSum = 1.0f;
    // float m_partialVolumeSumInv = 1.0f;

    void init();

    size_t numPartials() const;

    void setPartial(int index, float amplitude, double centDetune = 0.0);
    void setDefaultColors();
    void makeRect();
    void makeSaw();
    void makeSawRev();
    void makeTriangle();
};


// 🎼
struct SynthCfg
{
    int32_t m_sampleRate = 0;
    int32_t m_blockSize = 0;
    int32_t m_maxVoices = 20;
    int32_t m_masterVolume = 80; // in [%]
    int32_t m_unisono = 4;
    PartialsCfg m_partials;
    EnvelopeCfg m_envelope;

    void setSampleRate(int sampleRate)
    {
        m_sampleRate = sampleRate;
        m_envelope.SampleRate = sampleRate;
        DE_BENNI("setSampleRate(",m_sampleRate,")")
    }

    void setBlockSize(int blockSize)
    {
        m_blockSize = blockSize;
        DE_BENNI("setBlockSize(",m_blockSize,")")
    }

    void init()
    {
        m_partials.init();
    }

};

