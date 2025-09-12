#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"
#include "FxClipping.h"
#include "FxDelay.h"
#include <array>
#include "StepSequencer.h"
#include "FxChorus.h"
#include "LowFidelity.h"
#include <math.h>

class Sequencer;

//==============================================================================
class PluginAudioProcessor  : public AudioProcessor, public SynthParams
//==============================================================================
{
public:
    //==============================================================================
    PluginAudioProcessor();
    ~PluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


private:
    //==============================================================================
    class Synth : public Synthesiser {
    public:
        Synth(MidiState& mS) : midiState(mS) {}
        void handleController(int midiChannel, int controllerNumber, int newValue) override {
            switch (controllerNumber)
            {

            case 1: //Modwheel
                midiState.values[MidiState::eModwheel] = newValue;
                break;
            case 4: //Foot Controller
                midiState.values[MidiState::eFoot] = newValue;
                break;
            case 11: //Expression Control
                midiState.values[MidiState::eExpPedal] = newValue;
                break;
            default:
                break;
            }
            Synthesiser::handleController(midiChannel, controllerNumber, newValue);
        }
        void handleChannelPressure(int midiChannel, int channelPressureValue) override {
            midiState.values[MidiState::eAftertouch] = channelPressureValue;
            Synthesiser::handleChannelPressure(midiChannel, channelPressureValue);
        }
    private:
        MidiState& midiState;
    };

    Synth synth;

    // FX
    FxDelay delay;
    FxClipping clip;

    LowFidelity lowFi;

    StepSequencer stepSeq;
    FxChorus chorus;

    void updateHostInfo();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
