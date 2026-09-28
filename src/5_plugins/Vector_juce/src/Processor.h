#pragma once
#include "VectorSynth.h"

class VectorPluginProcessor : public juce::AudioProcessor
{
public:
    VectorPluginProcessor();
    ~VectorPluginProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& name) override;

    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    //VisualStateManager& getVisualStateManager() { return visualStateManager; }

    std::vector<NoteVisualState*>& getVisualStates() { return visualStates; }

    juce::Synthesiser& getSynth();

    juce::AudioProcessorEditor* createEditor() override;

private:
    juce::Synthesiser synth;

    std::vector<NoteVisualState*> visualStates;

    // VisualStateManager visualStateManager;

    struct Preset
    {
        juce::String name;
        std::map<juce::String, float> values;
    };

    std::vector<Preset> presets;
    int currentPreset { 0 };

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initPresets();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorPluginProcessor)
};
