#include "MorphVoice.h"
#include "MorphSound.h"

MorphVoice::MorphVoice()
{
    adsrParameters.attack  = 0.05f;
    adsrParameters.decay   = 0.25f;
    adsrParameters.sustain = 0.80f;
    adsrParameters.release = 0.50f;

    adsr.setParameters(adsrParameters);

    synth.topLeft.setWaveform(
        CornerSynth::Waveform::Sine);

    synth.topRight.setWaveform(
        CornerSynth::Waveform::Saw);

    synth.bottomLeft.setWaveform(
        CornerSynth::Waveform::Square);

    synth.bottomRight.setWaveform(
        CornerSynth::Waveform::Triangle);

    synth.topLeft.setAmplitude(0.25f);
    synth.topRight.setAmplitude(0.25f);
    synth.bottomLeft.setAmplitude(0.25f);
    synth.bottomRight.setAmplitude(0.25f);
}

bool MorphVoice::canPlaySound(
    juce::SynthesiserSound* sound)
{
    return dynamic_cast<MorphSound*>(sound) != nullptr;
}

void MorphVoice::prepare(
    double sampleRate,
    int /*samplesPerBlock*/,
    int /*numChannels*/)
{
    currentSampleRate = sampleRate;

    synth.prepare(sampleRate);

    adsr.setSampleRate(sampleRate);
}

void MorphVoice::startNote(
    int midiNoteNumber,
    float velocity,
    juce::SynthesiserSound*,
    int)
{
    juce::ignoreUnused(velocity);

    currentFrequency =
        (float)juce::MidiMessage::
            getMidiNoteInHertz(
                midiNoteNumber);

    synth.setFrequency(
        currentFrequency);

    adsr.noteOn();

    noteReleased = false;

    visualState.reset();

    visualState.active = true;
    visualState.midiNote = midiNoteNumber;
    visualState.stage =
        VisualADSRStage::Attack;

    visualState.morphX = morphX;
    visualState.morphY = morphY;
}

void MorphVoice::stopNote(
    float /*velocity*/,
    bool allowTailOff)
{
    noteReleased = true;

    visualState.stage =
        VisualADSRStage::Release;

    adsr.noteOff();

    if (!allowTailOff)
    {
        clearCurrentNote();

        visualState.reset();
    }
}

void MorphVoice::pitchWheelMoved(
    int /*newPitchWheelValue*/)
{
}

void MorphVoice::controllerMoved(
    int /*controllerNumber*/,
    int /*newControllerValue*/)
{
}

void MorphVoice::setVoiceIndex(
    int index)
{
    voiceIndex = index;
}

int MorphVoice::getVoiceIndex() const
{
    return voiceIndex;
}

void MorphVoice::setMorphPosition(
    float x,
    float y)
{
    morphX = juce::jlimit(
        0.0f,
        1.0f,
        x);

    morphY = juce::jlimit(
        0.0f,
        1.0f,
        y);

    visualState.morphX = morphX;
    visualState.morphY = morphY;
}

NoteVisualState&
MorphVoice::getVisualState()
{
    return visualState;
}

void MorphVoice::updateVisualState()
{
    visualState.orbiterAngle += 0.05f;

    if (visualState.orbiterAngle >
        juce::MathConstants<float>::twoPi)
    {
        visualState.orbiterAngle -=
            juce::MathConstants<float>::twoPi;
    }

    if (!noteReleased)
    {
        if (visualState.stageProgress < 0.33f)
        {
            visualState.stage =
                VisualADSRStage::Attack;
        }
        else if (visualState.stageProgress < 0.66f)
        {
            visualState.stage =
                VisualADSRStage::Decay;
        }
        else
        {
            visualState.stage =
                VisualADSRStage::Sustain;
        }
    }
}

void MorphVoice::renderNextBlock(
    juce::AudioBuffer<float>& outputBuffer,
    int startSample,
    int numSamples)
{
    if (!isVoiceActive())
        return;

    auto* left =
        outputBuffer.getWritePointer(0);

    auto* right =
        outputBuffer.getNumChannels() > 1
        ? outputBuffer.getWritePointer(1)
        : nullptr;

    for (int sampleIndex = 0;
         sampleIndex < numSamples;
         ++sampleIndex)
    {
        const float env =
            adsr.getNextSample();

        float sample =
            synth.process(
                morphX,
                morphY);

        sample *= env;

        left[startSample + sampleIndex]
            += sample;

        if (right != nullptr)
        {
            right[startSample + sampleIndex]
                += sample;
        }

        visualState.stageProgress +=
            1.0f /
            (float)(
                currentSampleRate * 2.0);

        updateVisualState();

        if (!adsr.isActive())
        {
            visualState.active = false;

            visualState.stage =
                VisualADSRStage::Finished;

            clearCurrentNote();

            break;
        }
    }
}