#include "PluginProcessor.h"
#include "PluginEditor.h"

MorphSynthAudioProcessor::MorphSynthAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true))
{
    constexpr int numVoices = 16;

    for (int i = 0; i < numVoices; ++i)
    {
        auto* voice = new MorphVoice();

        voice->setVoiceIndex(i);

        synth.addVoice(voice);
    }

    synth.addSound(
        new MorphSound());
}

void MorphSynthAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(
        sampleRate);

    for (int i = 0;
         i < synth.getNumVoices();
         ++i)
    {
        if (auto* voice =
                dynamic_cast<MorphVoice*>(
                    synth.getVoice(i)))
        {
            voice->prepare(
                sampleRate,
                samplesPerBlock,
                getTotalNumOutputChannels());
        }
    }
}

void MorphSynthAudioProcessor::releaseResources()
{
}

void MorphSynthAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    synth.renderNextBlock(
        buffer,
        midiMessages,
        0,
        buffer.getNumSamples());

    for (int i = 0;
         i < synth.getNumVoices();
         ++i)
    {
        auto* voice =
            dynamic_cast<MorphVoice*>(
                synth.getVoice(i));

        if (voice == nullptr)
            continue;

        visualStateManager.updateVoiceState(
            i,
            voice->getVisualState());
    }
}

bool MorphSynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
MorphSynthAudioProcessor::createEditor()
{
    return new MorphSynthAudioProcessorEditor(
        *this);
}

const juce::String
MorphSynthAudioProcessor::getName() const
{
    return "MorphSynth";
}

bool MorphSynthAudioProcessor::acceptsMidi() const
{
    return true;
}

bool MorphSynthAudioProcessor::producesMidi() const
{
    return false;
}

bool MorphSynthAudioProcessor::isMidiEffect() const
{
    return false;
}

double MorphSynthAudioProcessor::
getTailLengthSeconds() const
{
    return 0.0;
}

int MorphSynthAudioProcessor::
getNumPrograms()
{
    return 1;
}

int MorphSynthAudioProcessor::
getCurrentProgram()
{
    return 0;
}

void MorphSynthAudioProcessor::
setCurrentProgram(
    int /*index*/)
{
}

const juce::String
MorphSynthAudioProcessor::
getProgramName(
    int /*index*/)
{
    return {};
}

void MorphSynthAudioProcessor::
changeProgramName(
    int /*index*/,
    const juce::String& /*name*/)
{
}

void MorphSynthAudioProcessor::
getStateInformation(
    juce::MemoryBlock& /*destData*/)
{
}

void MorphSynthAudioProcessor::
setStateInformation(
    const void* /*data*/,
    int /*sizeInBytes*/)
{
}

VisualStateManager&
MorphSynthAudioProcessor::
getVisualStateManager()
{
    return visualStateManager;
}

juce::Synthesiser&
MorphSynthAudioProcessor::
getSynth()
{
    return synth;
}