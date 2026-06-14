// MyProcessor.cpp
#include "MyProcessor.h"
#include "MyEditor.h"

MyProcessor::MyProcessor()
    : AudioProcessor(BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    DE_DEBUG("MyProcessor()")

    smoothedGain.reset (48000.0, 0.05); // 50ms smoothing
    smoothedGain.setCurrentAndTargetValue (1.0f);
}

MyProcessor::~MyProcessor()
{
    DE_OK()
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
    if (m_editor)
    {
        m_editor->getCanvas()->getCollector().dsp_init(samplesPerBlock,2,sampleRate);
    }
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

    const int nInputs  = getTotalNumInputChannels();
    const int nOutputs = getTotalNumOutputChannels();
    const uint64_t nFrames  = buffer.getNumSamples();

    // For every output channel…
    for (int ch = 0; ch < nOutputs; ++ch)
    {
        float* __restrict__ dst = buffer.getWritePointer(ch);

        if (ch < nInputs)
        {
            // Relay input → output
            const float* __restrict__ src = buffer.getReadPointer(ch);

            DE_ASSUME_NO_OVERLAP(dst,src,nFrames * sizeof(float));

            memcpy(dst, src, nFrames * sizeof(float));
        }
        else
        {
            // No input for this output channel → silence
            memset(dst, 0, nFrames * sizeof(float));
        }
    }

    m_sumVector.resize( nFrames );

    for (int ch = 0; ch < nInputs; ++ch)
    {
        const float* __restrict__ src = buffer.getReadPointer(ch);
        float* __restrict__ dst = m_sumVector.data();
        for (uint64_t i = 0; i < nFrames; ++i)
        {
            dst[i] += src[i];
        }
    }

    const float nInputsInv = 1.0f / float(nInputs);

    float* __restrict__ dst = m_sumVector.data();

    for (uint64_t i = 0; i < nFrames; ++i)
    {
        dst[i] *= nInputsInv;
    }

    if (m_editor)
        m_editor->getCanvas()->pushSamples(m_sumVector);

    // if (bypass)
    // {
    //     // Nur Visualisierung füttern
    //     pushAudioForVisualization (buffer);
    //     return;
    // }

    // float gainDb = gainParam->load();
    // float targetGainLinear = juce::Decibels::decibelsToGain(gainDb);
    // smoothedGain.setTargetValue (targetGainLinear);

    // for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    // {
    //     auto g = smoothedGain.getNextValue();
    //     for (int ch = 0; ch < totalNumInputChannels; ++ch)
    //     {
    //         auto* data = buffer.getWritePointer (ch);
    //         data[sample] *= g;
    //     }
    // }

    // pushAudioForVisualization (buffer);
}

/*
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
*/

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

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyProcessor();
}
