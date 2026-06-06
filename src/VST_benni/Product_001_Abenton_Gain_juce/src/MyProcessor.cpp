// MyProcessor.cpp
#include "MyProcessor.h"
#include "MyEditor.h"

MyProcessor::MyProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout()),
      fifoBuffer (2, fifoSize)
{
    smoothedGain.reset (44100.0, 0.05); // 50ms smoothing
    smoothedGain.setCurrentAndTargetValue (1.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MyProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f),
        0.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

void MyProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    smoothedGain.reset (sampleRate, 0.05);
}

bool MyProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;
    return true;
}

void MyProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    auto* gainParam = apvts.getRawParameterValue ("gain");
    auto* bypassParam = apvts.getRawParameterValue ("bypass");

    bypass = (*bypassParam > 0.5f);

    if (bypass)
    {
        // Nur Visualisierung füttern
        pushAudioForVisualization (buffer);
        return;
    }

    float targetGainLinear = juce::Decibels::decibelsToGain (*gainParam);
    smoothedGain.setTargetValue (targetGainLinear);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        auto g = smoothedGain.getNextValue();
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            data[sample] *= g;
        }
    }

    pushAudioForVisualization (buffer);
}

void MyProcessor::pushAudioForVisualization (const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    int start1, size1, start2, size2;
    fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

    if (size1 > 0)
        for (int ch = 0; ch < fifoBuffer.getNumChannels(); ++ch)
            fifoBuffer.copyFrom (ch, start1, buffer, juce::jmin (ch, buffer.getNumChannels()-1), 0, size1);

    if (size2 > 0)
        for (int ch = 0; ch < fifoBuffer.getNumChannels(); ++ch)
            fifoBuffer.copyFrom (ch, start2, buffer, juce::jmin (ch, buffer.getNumChannels()-1), size1, size2);

    fifo.finishedWrite (size1 + size2);
}

void MyProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    juce::MemoryOutputStream stream (destData, false);
    state.writeToStream (stream);
}

void MyProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState (tree);
}

juce::AudioProcessorEditor* MyProcessor::createEditor()
{
    return new MyEditor(*this);
}
