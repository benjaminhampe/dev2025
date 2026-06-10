#pragma once
#include <JuceHeader.h>

class MyComponent;

class MyProcessor : public juce::AudioProcessor
{
public:
    MyProcessor();
    ~MyProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "3D Spektrum"; }
    double getTailLengthSeconds() const override { return 0.0; }

    // State
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Zugriff für Editor
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // FIFO für Visualisierung
    //void pushAudioForVisualization (const juce::AudioBuffer<float>& buffer);

    // bool getBypass() const { return bypass; }
    // void setBypass (bool b) { bypass = b; }

    // Thread‑safe FIFO
    // juce::AbstractFifo& getFifo() { return fifo; }
    // juce::AudioBuffer<float>& getFifoBuffer() { return fifoBuffer; }

    bool acceptsMidi() const override      { return false; }
    bool producesMidi() const override     { return false; }
    bool isMidiEffect() const override     { return false; }

    //de::TAlignedVector<float>& getSumVector() { return m_sumVector; }

    void setCanvas( MyComponent* component ) { m_canvas = component; }
private:
    juce::SmoothedValue<float> smoothedGain;
    bool bypass = false;

    // static constexpr int fifoSize = 2048;
    // juce::AbstractFifo fifo { fifoSize };
    // juce::AudioBuffer<float> fifoBuffer;

    MyComponent* m_canvas;

    de::TAlignedVector<float> m_sumVector;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
