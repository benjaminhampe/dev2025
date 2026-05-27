#include "PluginProcessor.h"
#include "PluginEditor.h"

RubberbandProcessor::RubberbandProcessor()
    : AudioProcessor (BusesProperties()
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    apvts (*this, nullptr, "PARAMS", createLayout())
{
}

bool RubberbandProcessor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
    && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void RubberbandProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rb = std::make_unique<RubberBand::RubberBandStretcher>(
        sampleRate,
        2,
        RubberBand::RubberBandStretcher::OptionProcessRealTime
            | RubberBand::RubberBandStretcher::OptionPitchHighQuality
            | RubberBand::RubberBandStretcher::OptionStretchElastic);

    tempIn.resize(samplesPerBlock * 2);
    tempOut.resize(samplesPerBlock * 2);
}

void RubberbandProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer&)
{
    const int numSamples  = buffer.getNumSamples();

    // === APPLY PARAMETERS ===
    float tempo = *apvts.getRawParameterValue("tempo");
    float pitch = *apvts.getRawParameterValue("pitch");

    rb->setTimeRatio(tempo);
    rb->setPitchScale(pitch);

    // === INPUT POINTERS ===
    const float* inputPtrs[2];
    inputPtrs[0] = buffer.getReadPointer(0);
    inputPtrs[1] = buffer.getReadPointer(1);

    rb->process(inputPtrs, numSamples, false);

    int available = rb->available();
    if (available > numSamples)
        available = numSamples;

    // === OUTPUT POINTERS ===
    float* outputPtrs[2];
    outputPtrs[0] = buffer.getWritePointer(0);
    outputPtrs[1] = buffer.getWritePointer(1);

    rb->retrieve(outputPtrs, available);

    // Zero-pad if RubberBand returned fewer samples
    for (int ch = 0; ch < 2; ++ch)
        for (int i = available; i < numSamples; ++i)
            outputPtrs[ch][i] = 0.0f;
}

void RubberbandProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto xml = apvts.copyState().createXml();
    copyXmlToBinary(*xml, destData);
}

void RubberbandProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ============================================================================
// PARAMETER LAYOUT
// ============================================================================
RubberbandProcessor::APVTS::ParameterLayout RubberbandProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tempo", "Tempo",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitch", "Pitch",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.001f), 1.0f));

    return { p.begin(), p.end() };
}

// ============================================================================
// ENTRY POINT
// ============================================================================

juce::AudioProcessorEditor* RubberbandProcessor::createEditor()
{
    return new RubberbandEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RubberbandProcessor();
}


#if 0

RubberBandProcessor::RubberBandProcessor()
    : parameters(*this, nullptr, "PARAMS", createLayout())
{
}

RubberBandProcessor::APVTS::ParameterLayout RubberBandProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    // Sliders (range parameters)
    p.push_back(std::make_unique<juce::AudioParameterFloat>("time", "Time Ratio",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch Scale",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.001f), 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>("transients", "Transient Smoothing",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>("phase", "Phase Smoothing",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>("formant", "Formant Shift",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.001f), 1.0f));

    // Checkboxes (boolean parameters)
    p.push_back(std::make_unique<juce::AudioParameterBool>("realtime", "Real-Time Mode", true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("highq", "High Quality Pitch", true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("formantPres", "Preserve Formants", false));

    return { p.begin(), p.end() };
}

void RubberBandProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    int channels = getTotalNumInputChannels();

    int options = 0;
    if (parameters.getRawParameterValue("realtime")->load())
        options |= RubberBand::RubberBandStretcher::OptionProcessRealTime;
    if (parameters.getRawParameterValue("highq")->load())
        options |= RubberBand::RubberBandStretcher::OptionPitchHighQuality;
    if (parameters.getRawParameterValue("formantPres")->load())
        options |= RubberBand::RubberBandStretcher::OptionFormantPreserved;

    stretcher = std::make_unique<RubberBand::RubberBandStretcher>(
        sampleRate, channels, options);

    tempBuffer.setSize(channels, samplesPerBlock * 4);
}

void RubberBandProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (!stretcher)
        return;

    const int numSamples = buffer.getNumSamples();
    const int channels = buffer.getNumChannels();

    float timeRatio = parameters.getRawParameterValue("time")->load();
    float pitchScale = parameters.getRawParameterValue("pitch")->load();
    float transient = parameters.getRawParameterValue("transients")->load();
    float phase = parameters.getRawParameterValue("phase")->load();
    float formant = parameters.getRawParameterValue("formant")->load();

    stretcher->setTimeRatio(timeRatio);
    stretcher->setPitchScale(pitchScale);
    stretcher->setTransientsOption(transient);
    stretcher->setPhaseOption(phase);
    stretcher->setFormantOption(formant);

    // Feed input
    std::vector<const float*> inputs(channels);
    for (int ch = 0; ch < channels; ++ch)
        inputs[ch] = buffer.getReadPointer(ch);

    stretcher->process(inputs.data(), numSamples, false);

    // Retrieve output
    int available = stretcher->available();
    if (available > 0)
    {
        tempBuffer.setSize(channels, available, false, false, true);

        std::vector<float*> outputs(channels);
        for (int ch = 0; ch < channels; ++ch)
            outputs[ch] = tempBuffer.getWritePointer(ch);

        stretcher->retrieve(outputs.data(), available);

        // Copy back (truncate or pad)
        for (int ch = 0; ch < channels; ++ch)
            buffer.copyFrom(ch, 0, tempBuffer, ch, 0, juce::jmin(available, numSamples));
    }
}

juce::AudioProcessorEditor* RubberBandProcessor::createEditor()
{
    return new RubberBandEditor(*this);
}

#endif