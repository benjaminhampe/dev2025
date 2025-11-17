#pragma once
#include "SineMachine_Globals.h"

class Editor;

class Synth : public AudioEffectX
{
public:
    Synth(audioMasterCallback audioMaster);
    void processReplacing (float** in, float** out, VstInt32 frames) override;
    void setParameter(VstInt32 index, float value) override;
    float getParameter(VstInt32 index) override;
    VstInt32 canDo(char* text) override;

    void open() override;
    void close() override;
    void setSampleRate(float sampleRate) override;
    void resume() override;
    VstInt32 processEvents(VstEvents* events) override;

    void setPartialsToRect();
    void setPartialsToSaw();
    void setPartialsToSawRev();

    // SineOvertone m_tones[NUM_OVERTONES];
    std::vector<Partial> m_partials;
    float m_sampleRate;
    float m_masterAmplitude;
    float m_baseFrequency = 55.f;
    //int32_t m_baseOctave;
    Editor* m_editor;

private:
    void setPartial(int index, float amplitude, double centDetune = 0.0);


    static void calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate );

    void handleMidi(unsigned char* data);
    int currentNote = -1;
    float modDepth = 0.0f;
    float masterGain = 1.0f;
    float filterCutoff = 1.0f;
    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note, int velocity);
    void controlChange(int channel, int controller, int value);
    void pitchBend(int channel, int bendValue);
};
