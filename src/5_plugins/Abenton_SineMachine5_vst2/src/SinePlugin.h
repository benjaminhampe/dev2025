#pragma once
#include "SineEditor.h"
#include "SineSynth.h"
#include <set>

enum
{
    kParamA = 0,
    kNumParams = 1
};
const int kNumPrograms = 0;
const int kNumInputs = 2;
const int kNumOutputs = 2;
const unsigned long kUniqueId = 'hSM4';    // Hampe SineMachine4

AudioEffect* createEffectInstance(audioMasterCallback audioMaster);

class Plugin : public AudioEffectX
{
private:
    std::atomic_bool m_bBypassed{ false };
    std::atomic_bool m_bPluginOpen{ true };

    char m_programName[kVstMaxProgNameLen + 1];

    // int32_t m_sampleRate;
    // int32_t m_blockSize;

    Synth m_synth;

    Editor m_editor;

public:
    Plugin(audioMasterCallback audioMaster);
    ~Plugin();

    VstInt32 getVendorVersion() override {return 1004;}            // Version number
    VstPlugCategory getPlugCategory() override {return kPlugCategEffect;} // The general category for the plug-in
    bool getEffectName(char* name) override;                       // The plug-in name
    bool getProductString(char* text) override;                    // This is a unique plug-in string provided by Steinberg
    bool getVendorString(char* text) override;                     // Vendor info

    void getProgramName(char *name) override;                      // read the name from the host
    void setProgramName(char *name) override;                      // changes the name of the preset displayed in the host

    void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames) override;
    void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames) override;

    VstInt32 getChunk (void** data, bool isPreset) override;
    VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset) override;
    float getParameter(VstInt32 index) override;                    // get the parameter value at the specified index
    void setParameter(VstInt32 index, float value) override;        // set the parameter at index to value
    void getParameterLabel(VstInt32 index, char *text) override;    // label for the parameter (eg dB)
    void getParameterName(VstInt32 index, char *text) override;     // name of the parameter
    void getParameterDisplay(VstInt32 index, char *text) override;  // text description of the current value
    VstInt32 canDo(char* text) override;

    VstIntPtr dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value, void* ptr, float opt) override;
    void open() override;
    void close() override;

    const Synth& getSynth() const { return m_synth; }
    Synth& getSynth() { return m_synth; }

    VstInt32 processEvents(VstEvents* events) override;
    void handleShortMidi(char bytes[4]);

/*
    void changeSampleRate( int32_t sampleRate );
    void changeFrameCount( int32_t sampleRate );
    VstInt32 processEvents(VstEvents* events) override;
    void handleMidi(unsigned char* data);
    int currentNote = -1;
    float modDepth = 0.0f;
    float masterGain = 1.0f;
    float filterCutoff = 1.0f;
    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note, int velocity);
    void controlChange(int channel, int controller, int value);
    void pitchBend(int channel, int bendValue);
*/
};
