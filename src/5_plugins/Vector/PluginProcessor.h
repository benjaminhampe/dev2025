#pragma once

#include <JuceHeader.h>

#include "MorphSound.h"
#include "MorphVoice.h"
#include "VisualStateManager.h"

class MorphSynthAudioProcessor
    : public juce::AudioProcessor
{
public:

    MorphSynthAudioProcessor();

    ~MorphSynthAudioProcessor() override = default;

    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock) override;

    void releaseResources() override;

    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&) override;

    juce::AudioProcessorEditor*
    createEditor() override;

    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(
        int index) override;

    const juce::String getProgramName(
        int index) override;

    void changeProgramName(
        int index,
        const juce::String& name) override;

    void getStateInformation(
        juce::MemoryBlock& destData) override;

    void setStateInformation(
        const void* data,
        int sizeInBytes) override;

    VisualStateManager&
    getVisualStateManager();

    juce::Synthesiser&
    getSynth();

private:

    juce::Synthesiser synth;

    VisualStateManager visualStateManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MorphSynthAudioProcessor)
};