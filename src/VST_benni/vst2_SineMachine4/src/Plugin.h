#pragma once
#include "Synth.h"
#include "Editor.h"

#include <set>
#include <string>
#include <cmath>

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
    //void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames) override;

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

    Synth* getSynth() { return &m_synth; }

    void changeSampleRate( int32_t sampleRate );
    void changeFrameCount( int32_t sampleRate );

private:
    char m_programName[kVstMaxProgNameLen + 1];

    std::set< std::string > m_canDo;

    int32_t m_sampleRate;
    int32_t m_frameCount; // BlockSize
    int32_t m_masterAmplitude;

    Editor m_editor;

    Synth m_synth;
/*
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

/*

void Plugin::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}
void Plugin::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

VstInt32 Plugin::processEvents(VstEvents* events)
{
    for (VstInt32 i = 0; i < events->numEvents; ++i)
    {
        if (events->events[i]->type == kVstMidiType)
        {
            VstMidiEvent* midi = (VstMidiEvent*)events->events[i];
            handleMidi(reinterpret_cast<unsigned char*>(midi->midiData));
        }
    }
    return 1;
}

void Plugin::handleMidi(unsigned char* data)
{
    int status = data[0] & 0xF0;
    int channel = data[0] & 0x0F;
    int data1 = data[1] & 0x7F;
    int data2 = data[2] & 0x7F;

    switch (status) {
    case 0x90:  // Note On
        if (data2 > 0)
            noteOn(channel, data1, data2);
        else
            noteOff(channel, data1, data2);  // velocity 0 = note off
        break;
    case 0x80:  // Note Off
        noteOff(channel, data1, data2);
        break;
    case 0xB0:  // Control Change
        controlChange(channel, data1, data2);
        break;
    case 0xE0:  // Pitch Bend
        pitchBend(channel, ((data2 << 7) | data1) - 8192);
        break;
        // Add more cases as needed
    }
}

void Plugin::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Plugin::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote)
    {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}

void Plugin::controlChange(int channel, int controller, int value)
{
    switch (controller) {
    case 1:  // Mod Wheel
        modDepth = value / 127.0f;
        break;
    case 7:  // Volume
        masterGain = value / 127.0f;
        break;
    case 74: // Filter cutoff (common mapping)
        filterCutoff = value / 127.0f;
        break;
    default:
        // Handle other CCs or ignore
        break;
    }
}

void Plugin::pitchBend(int channel, int bendValue)
{
    // Convert to semitone offset: ±2 semitones range
    float bendSemis = (bendValue / 8192.0f) * 2.0f;

    // Apply bend to current note
    if (currentNote >= 0)
    {
        m_baseFrequency = 440.0f * pow(2.0, (currentNote - 69 + bendSemis) / 12.0);
        calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
    }
}
*/
