#pragma once
#include <SineVoice.h>

class Synth
{
    SynthCfg m_cfg;
    std::vector<Voice> m_voices;

    // bool m_bBypass = false;
    // int64_t m_framePos = 0;
    // int32_t m_blockSize;
    // int32_t m_sampleRate;
    // std::vector<Partial> m_partials;
    // float m_sampleRate;
    // float m_masterAmplitude;
    // float m_baseFrequency = 55.f;
    // int32_t m_baseOctave;

public:
    de::TAlignedVector<float> m_L;
    de::TAlignedVector<float> m_R;

    const SynthCfg& getConfig() const;

    SynthCfg& getConfig();

    // void setBypass(bool bBypassed);

    void init();

    void setSampleRate(int sampleRate);

    void setBlockSize(int blockSize);

    void process(int blockSize);

    int findIdleVoice() const;

    void noteOn(int channel, int midiNote, int velocity);

    void noteOff(int channel, int midiNote, int velocity);

    void allNotesOff();

    void controlChange(int channel, int controller, int value);

    void pitchBend(int channel, int bendValue);

    // void setPartial(int index, float amplitude, double centDetune = 0.0);
    // void setPartialsToRect();
    // void setPartialsToSaw();
    // void setPartialsToSawRev();
    // void setPartialsToTriangle();

    // static void calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate );
};
