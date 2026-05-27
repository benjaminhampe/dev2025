#pragma once
#include <JuceHeader.h>
#include <rubberband_400/RubberBandStretcher.h>

class RubberbandProcessor : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    RubberbandProcessor();
    ~RubberbandProcessor() override = default;

    // JUCE overrides
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "RubberbandPlugin"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static APVTS::ParameterLayout createLayout();

    APVTS apvts;

private:
    std::unique_ptr<RubberBand::RubberBandStretcher> rb;

    std::vector<float> tempIn, tempOut;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RubberbandProcessor)
};


#if 0
class RubberBandProcessor : public juce::AudioProcessor
{
public:
    RubberBandProcessor();
    ~RubberBandProcessor() override = default;

    const juce::String getName() const override { return "RubberBandPlugin"; }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return true; }

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS::ParameterLayout createLayout();
    APVTS parameters;

private:
    std::unique_ptr<RubberBand::RubberBandStretcher> stretcher;
    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RubberBandProcessor)
};

#endif