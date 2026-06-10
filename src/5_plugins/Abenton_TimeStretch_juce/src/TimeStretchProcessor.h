#pragma once
#include <JuceHeader.h>

#include "DspSoundTouch.h"

//#include <SoundTouch_241/SoundTouch.h>

class TimeStretchProcessor : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    TimeStretchProcessor();
    ~TimeStretchProcessor() override = default;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "SoundTouchPlugin"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static APVTS::ParameterLayout createLayout();

    APVTS apvts;

private:
#if 0
    soundtouch::SoundTouch st;

    std::vector<float> tempIn, tempOut;
#else
    de::audio::DspSoundTouch m_dsp;

    std::vector<float> m_dspIn, m_dspOut; // Interleaved
#endif
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeStretchProcessor)
};


#if 0
class TimeStretchProcessor : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    TimeStretchProcessor();
    ~TimeStretchProcessor() override = default;

    // Basic info
    const juce::String getName() const override { return "SoundTouchPlugin"; }
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;

    // Layout
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return true; }

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    // Processing
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    double getTailLengthSeconds() const override { return 0.0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // State
    APVTS::ParameterLayout createLayout();
    APVTS parameters;

    //juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    soundtouch::SoundTouch st;
    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeStretchProcessor)
};

#endif
