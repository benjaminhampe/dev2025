#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"

SoundTouchProcessor::SoundTouchProcessor()
    : AudioProcessor (BusesProperties()
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    apvts (*this, nullptr, "PARAMS", createLayout())
{
    st.setChannels(2);
    st.setSampleRate(44100);
}

bool SoundTouchProcessor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
    && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SoundTouchProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    st.setSampleRate((uint32_t)sampleRate);
    st.clear();

    tempIn.resize(samplesPerBlock * 2);
    tempOut.resize(samplesPerBlock * 2);
}

void SoundTouchProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer&)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // === APPLY PARAMETERS ===
    st.setTempo(*apvts.getRawParameterValue("tempo"));
    st.setRate(*apvts.getRawParameterValue("rate"));
    st.setPitchSemiTones(*apvts.getRawParameterValue("pitch"));

    st.setSetting(SETTING_SEQUENCE_MS, (int)*apvts.getRawParameterValue("sequence"));
    st.setSetting(SETTING_SEEKWINDOW_MS, (int)*apvts.getRawParameterValue("seek"));
    st.setSetting(SETTING_OVERLAP_MS, (int)*apvts.getRawParameterValue("overlap"));
    st.setSetting(SETTING_USE_AA_FILTER, (int)*apvts.getRawParameterValue("aa"));

    // === INTERLEAVE ===
    for (int i = 0; i < numSamples; ++i)
        for (int ch = 0; ch < numChannels; ++ch)
            tempIn[i * numChannels + ch] = buffer.getReadPointer(ch)[i];

    st.putSamples(tempIn.data(), numSamples);

    uint32_t got = st.receiveSamples(tempOut.data(), numSamples);

    // === DEINTERLEAVE ===
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* out = buffer.getWritePointer(ch);

        for (uint32_t i = 0; i < got; ++i)
            out[i] = tempOut[i * numChannels + ch];

        for (uint32_t i = got; i < (uint32_t)numSamples; ++i)
            out[i] = 0.0f;
    }
}

void SoundTouchProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto xml = apvts.copyState().createXml();
    copyXmlToBinary(*xml, destData);
}

void SoundTouchProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ============================================================================
// PARAMETER LAYOUT
// ============================================================================
SoundTouchProcessor::APVTS::ParameterLayout SoundTouchProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tempo", "Tempo",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "rate", "Rate",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitch", "Pitch (semitones)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sequence", "Sequence (ms)",
        juce::NormalisableRange<float>(10.0f, 100.0f, 1.0f), 40.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "seek", "Seek (ms)",
        juce::NormalisableRange<float>(10.0f, 100.0f, 1.0f), 30.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "overlap", "Overlap (ms)",
        juce::NormalisableRange<float>(5.0f, 50.0f, 1.0f), 8.0f));

    p.push_back(std::make_unique<juce::AudioParameterBool>(
        "aa", "Anti-Alias Filter", true));

    return { p.begin(), p.end() };
}

juce::AudioProcessorEditor* SoundTouchProcessor::createEditor()
{
    return new SoundTouchEditor(*this);
}

// ============================================================================
// ENTRY POINT
// ============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoundTouchProcessor();
}


#if 0
/*
#include "PluginProcessor.h"
#include "PluginEditor.h"

SoundTouchProcessor::SoundTouchProcessor()
    : AudioProcessor (BusesProperties()
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    st.setChannels(2);
    st.setSampleRate(44100);
    st.setTempo(1.0f);
    st.setPitchSemiTones(0.0f);
    st.setRate(1.0f);
}

bool SoundTouchProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
    && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SoundTouchProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    st.setSampleRate((uint32_t)sampleRate);
    st.clear();

    tempIn.resize(samplesPerBlock * 2);
    tempOut.resize(samplesPerBlock * 2);
}

void SoundTouchProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer&)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Interleave
    for (int i = 0; i < numSamples; ++i)
        for (int ch = 0; ch < numChannels; ++ch)
            tempIn[i * numChannels + ch] = buffer.getReadPointer(ch)[i];

    st.putSamples(tempIn.data(), numSamples);

    uint32_t got = st.receiveSamples(tempOut.data(), numSamples);

    // Deinterleave
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* out = buffer.getWritePointer(ch);

        for (uint32_t i = 0; i < got; ++i)
            out[i] = tempOut[i * numChannels + ch];

        for (uint32_t i = got; i < (uint32_t)numSamples; ++i)
            out[i] = 0.0f;
    }
}

void SoundTouchProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
}

void SoundTouchProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}


juce::AudioProcessorEditor* SoundTouchProcessor::createEditor()
{
    return new SoundTouchProcessorEditor (*this);
}

// ENTRY POINT — REQUIRED IN YOUR PROJECT TYPE
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoundTouchProcessor();
}
*/

SoundTouchProcessor::SoundTouchProcessor()
    : parameters(*this, nullptr, "PARAMS", createLayout())
{
}

SoundTouchProcessor::APVTS::ParameterLayout
SoundTouchProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    // Tempo, Rate, Pitch as factors (0.5x–2.0x)
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tempo", "Tempo",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "rate", "Rate",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitch", "Pitch",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    // Sequence / seek / overlap in ms (SoundTouch expects ints)
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sequence", "Sequence (ms)",
        juce::NormalisableRange<float>(10.0f, 100.0f, 1.0f), 40.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "seek", "Seek (ms)",
        juce::NormalisableRange<float>(10.0f, 100.0f, 1.0f), 30.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "overlap", "Overlap (ms)",
        juce::NormalisableRange<float>(5.0f, 50.0f, 1.0f), 8.0f));

    // Boolean: anti-alias filter
    p.push_back(std::make_unique<juce::AudioParameterBool>(
        "aa", "Anti-Alias Filter", true));

    return { p.begin(), p.end() };
}

void SoundTouchProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    st.clear();
    st.setSampleRate(static_cast<uint>(sampleRate));
    st.setChannels(static_cast<uint>(getTotalNumInputChannels()));

    tempBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock * 4);
}

void SoundTouchProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer&)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Update parameters
    const float tempo  = parameters.getRawParameterValue("tempo")->load();
    const float rate   = parameters.getRawParameterValue("rate")->load();
    const float pitch  = parameters.getRawParameterValue("pitch")->load();
    const float seqMs  = parameters.getRawParameterValue("sequence")->load();
    const float seekMs = parameters.getRawParameterValue("seek")->load();
    const float ovlMs  = parameters.getRawParameterValue("overlap")->load();
    const bool useAA   = parameters.getRawParameterValue("aa")->load() > 0.5f;

    st.setTempo(tempo);
    st.setRate(rate);
    st.setPitch(pitch);

    st.setSetting(SETTING_SEQUENCE_MS,  static_cast<int>(seqMs));
    st.setSetting(SETTING_SEEKWINDOW_MS, static_cast<int>(seekMs));
    st.setSetting(SETTING_OVERLAP_MS,   static_cast<int>(ovlMs));
    st.setSetting(SETTING_USE_AA_FILTER, useAA ? 1 : 0);

    // Interleave input for SoundTouch
    tempBuffer.setSize(numChannels, numSamples, false, false, true);

    // SoundTouch works on interleaved samples
    juce::AudioBuffer<float> interleaved(1, numSamples * numChannels);
    float* interPtr = interleaved.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
        for (int ch = 0; ch < numChannels; ++ch)
            interPtr[i * numChannels + ch] = buffer.getReadPointer(ch)[i];

    st.putSamples(interPtr, numSamples);

    const int maxOut = st.numSamples();
    if (maxOut <= 0)
        return;

    juce::AudioBuffer<float> outInterleaved(1, maxOut * numChannels);
    int received = st.receiveSamples(outInterleaved.getWritePointer(0), maxOut);

    if (received <= 0)
        return;

    // De-interleave back into buffer (truncate/pad to numSamples)
    const float* outPtr = outInterleaved.getReadPointer(0);
    const int copySamples = juce::jmin(received, numSamples);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* dst = buffer.getWritePointer(ch);
        for (int i = 0; i < copySamples; ++i)
            dst[i] = outPtr[i * numChannels + ch];

        // zero tail if output shorter
        if (copySamples < numSamples)
            juce::FloatVectorOperations::clear(dst + copySamples, numSamples - copySamples);
    }
}

void SoundTouchProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    // auto state = apvts.copyState();
    // std::unique_ptr<juce::XmlElement> xml (state.createXml());
    // copyXmlToBinary (*xml, destData);
}

void SoundTouchProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    // if (xml && xml->hasTagName (apvts.state.getType()))
    //     apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* SoundTouchProcessor::createEditor()
{
    return new SoundTouchEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoundTouchProcessor();
}

#endif