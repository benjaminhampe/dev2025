#pragma once
#include "Globals.h"
#include "data/ADSR.h"
#include "data/LFO.h"

// 📊
struct Partial
{
    float amplitude; // in [0,1]
    float detune; // max. detune in cent
    float phase = 0.0f;
    float phaseIncrement = 1.0f;
    int16_t partial; // 1,2,N x Even and Odd multiples of BaseFrequency = Harmonics/Partials
    bool audible = true;
    bool editable = true;
    NVGcolor color;

    std::string str() const
    {
        return std::to_string(partial);
    }

    float A() const
    {
        return amplitude;
    }
};

// 📊
struct Voice
{
    float baseFreq;
    float sampleRate;
    float amplitudeSum;
    float amplitudeSumInv;
    alignas(32) float partials[NUM_PARTIALS];
    alignas(32) float phase[NUM_PARTIALS];
    alignas(32) float phaseIncrement[NUM_PARTIALS];
    alignas(32) LFO<float> amplitude[NUM_PARTIALS];
    alignas(32) LFO<float> detune[NUM_PARTIALS];
    ADSR env;
    float A() const
    {
        return 1.0f;
    }
};

struct Voices
{

};

class Synth
{
public:
    Synth();
    void setSampleRate(float sampleRate);
    void setPartial(int index, float amplitude, double centDetune = 0.0);
    void setPartialsToRect();
    void setPartialsToSaw();
    void setPartialsToSawRev();
    void setPartialsToTriangle();

    static void calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate );

    std::vector<Partial> m_partials;
    float m_sampleRate;
    float m_masterAmplitude;
    float m_baseFrequency = 55.f;
    //int32_t m_baseOctave;
};
