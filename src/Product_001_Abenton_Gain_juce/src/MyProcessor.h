// PluginProcessor.h
#pragma once
#include <JuceHeader.h>

class MyProcessor : public juce::AudioProcessor
{
public:
    MyProcessor();
    ~MyProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "OpenGLGain"; }
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
    void pushAudioForVisualization (const juce::AudioBuffer<float>& buffer);

    bool getBypass() const { return bypass; }
    void setBypass (bool b) { bypass = b; }

    // Thread‑safe FIFO
    juce::AbstractFifo& getFifo() { return fifo; }
    juce::AudioBuffer<float>& getFifoBuffer() { return fifoBuffer; }

private:
    juce::SmoothedValue<float> smoothedGain;
    bool bypass = false;

    static constexpr int fifoSize = 2048;
    juce::AbstractFifo fifo { fifoSize };
    juce::AudioBuffer<float> fifoBuffer;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
