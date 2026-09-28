#include "VectorSynth.h"

//=======================================================
VectorOsc::VectorOsc()
//=======================================================
{
}

void VectorOsc::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void VectorOsc::reset()
{
    phase = phaseOffset;
}

void VectorOsc::setFrequency(float newFrequency)
{
    frequency = newFrequency;

    if (sampleRate <= 0.0f)
    {
        DE_ERROR("No sampleRate ",sampleRate)
        sampleRate = 48000.0f;
    }

    constexpr float twoPi = juce::MathConstants<float>::twoPi;
    phaseIncrement = twoPi * frequency / sampleRate;
}

void VectorOsc::setAmplitude(float newAmplitude)
{
    amplitude = newAmplitude;
}

void VectorOsc::setPhaseOffset(float radians)
{
    phaseOffset = radians;
}

void VectorOsc::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
}

float VectorOsc::nextSample()
{
    if (sampleRate <= 0.0)
    {
        DE_ERROR("No sampleRate ",sampleRate)
        return 0.0f;
    }

    constexpr float twoPi = juce::MathConstants<float>::twoPi;
    constexpr float twoPiInv = 1.0f / juce::MathConstants<float>::twoPi;
    constexpr float onePi = juce::MathConstants<float>::pi;

    float sample = 0.0f;

    switch (waveform)
    {
        case Waveform::Sine:
        {
            sample = std::sin(phase);
            break;
        }

        case Waveform::Saw:
        {
            const float t = phase * twoPiInv;
            sample = (2.0f * t) - 1.0f;
            break;
        }

        case Waveform::Square:
        {
            sample = phase < onePi ? 1.0f : -1.0f;
            break;
        }

        case Waveform::Triangle:
        {
            const float t = phase * twoPiInv;
            if (t < 0.25f) return t * 4.0f;
            if (t < 0.75f) return 2.0f - (t * 4.0f);
            sample = (t * 4.0f) - 4.0f;
            break;
        }
        default:
            break;
    }

    phase += phaseIncrement;
    while (phase >= twoPi) phase -= twoPi;
    while (phase < 0.0f) phase += twoPi;

    return sample * amplitude;
}

//=======================================================
VectorSynth::VectorSynth()
//=======================================================
{

}

void VectorSynth::prepare(double sampleRate)
{
    topLeft.prepare(sampleRate);
    topRight.prepare(sampleRate);
    bottomLeft.prepare(sampleRate);
    bottomRight.prepare(sampleRate);

    topLeft.setWaveform(VectorOsc::Waveform::Saw);
    topRight.setWaveform(VectorOsc::Waveform::Square);
    bottomLeft.setWaveform(VectorOsc::Waveform::Triangle);
    bottomRight.setWaveform(VectorOsc::Waveform::Sine);

    topLeft.setAmplitude(0.5f);
    topRight.setAmplitude(0.5f);
    bottomLeft.setAmplitude(1.0f);
    bottomRight.setAmplitude(1.0f);
}

void VectorSynth::reset()
{
    topLeft.reset();
    topRight.reset();
    bottomLeft.reset();
    bottomRight.reset();
}

void VectorSynth::setFrequency(float frequency)
{
    topLeft.setFrequency(frequency);
    topRight.setFrequency(frequency);
    bottomLeft.setFrequency(frequency);
    bottomRight.setFrequency(frequency);
}

float VectorSynth::process(float morphX, float morphY)
{
    morphX = juce::jlimit(0.0f, 1.0f, morphX);
    morphY = juce::jlimit(0.0f, 1.0f, morphY);

    const float tl = topLeft.nextSample();
    const float tr = topRight.nextSample();
    const float bl = bottomLeft.nextSample();
    const float br = bottomRight.nextSample();

    const float wTL = (1.0f - morphX) * (1.0f - morphY);
    const float wTR = morphX * (1.0f - morphY);
    const float wBL = (1.0f - morphX) * morphY;
    const float wBR = morphX * morphY;

    return tl * wTL +
        tr * wTR +
        bl * wBL +
        br * wBR;
}


//=======================================================
MorphVoice::MorphVoice()
//=======================================================
{
#if 0
    adsrParameters.attack  = 0.05f;
    adsrParameters.decay   = 0.25f;
    adsrParameters.sustain = 0.80f;
    adsrParameters.release = 0.50f;

    adsr.setParameters(adsrParameters);
#endif

    adsrParameters.sampleRate = 48000.0f;
    adsrParameters.attackTimeInSec = 0.01f;
    adsrParameters.decayTimeInSec = 0.25f;
    adsrParameters.sustainLevel = 0.80f;
    adsrParameters.releaseTimeInSec = 0.50f;
    adsr.init(adsrParameters);

    adsr.onProgress =
        [&] (de::vec::Env::ePhase phase, float t)
        {
            visualState.stage = phase;
            visualState.stageProgress = t;
        };
}

MorphVoice::~MorphVoice()
{

}

bool MorphVoice::isVoiceActive() const
{
    return adsr.isPlaying();
}

bool MorphVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return true; // dynamic_cast<MorphSound*>(sound) != nullptr;
}

void MorphVoice::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    DE_DEBUG("sr(",sampleRate,"), "
    "fc(",samplesPerBlock,"), "
    "ch(",numChannels,")")

    currentSampleRate = sampleRate;
    synth.prepare(sampleRate);
    adsrParameters.sampleRate = sampleRate;
    adsr.init(adsrParameters);
}

void MorphVoice::startNote(int midiNote,float velocity,
    juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    currentFrequency = (float)juce::MidiMessage::getMidiNoteInHertz(midiNote);
    synth.setFrequency(currentFrequency);
    adsr.noteOn(velocity);
    visualState.reset();
    visualState.bPlaying = adsr.isPlaying();
    visualState.bNoteReleased = false;
    visualState.midiNote = midiNote;
    visualState.morphX = morphX;
    visualState.morphY = morphY;
}

void MorphVoice::stopNote(float velocity, bool allowTailOff)
{
    DE_DEBUG("Voice[",getVoiceIndex(),"].noteOff(",currentFrequency," Hz)")
    adsr.noteOff(velocity);

    visualState.bPlaying = adsr.isPlaying();
    visualState.bNoteReleased = true;

    if (!allowTailOff)
    {
        clearCurrentNote();
        visualState.reset();
    }
}

void MorphVoice::renderNextBlock(
    juce::AudioBuffer<float>& outputBuffer,
    int startSample,
    int numSamples)
{
    if (!isVoiceActive())
    {
        return;
    }

    if (outputBuffer.getNumChannels() != 2)
    {
        DE_DEBUG("Buffer must be stereo, no sound produced!")
        return;
    }

    auto L = outputBuffer.getWritePointer(0);
    auto R = outputBuffer.getWritePointer(1);

    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
    {
        const float env = adsr.getNextSample();

        float sample = synth.process(morphX, morphY);

        sample *= env;

        L[startSample + sampleIndex] += sample;
        R[startSample + sampleIndex] += sample;

#if 0
        visualState.stageProgress += 1.0f / (float(currentSampleRate) * 2.0f);

        updateVisualState();

        if (!adsr.isActive())
        {
            visualState.active = false;
            visualState.stage = VisualADSRStage::Finished;
            clearCurrentNote();
            break;
        }
#endif
        if (!visualState.bPlaying)
        {
            clearCurrentNote();
            break;
        }
    }
}


void MorphVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void MorphVoice::controllerMoved(int ccNumber, int newCCValue)
{
}

void MorphVoice::setVoiceIndex(int index)
{
    voiceIndex = index;
}

int MorphVoice::getVoiceIndex() const
{
    return voiceIndex;
}

void MorphVoice::setMorphPosition(float x,float y)
{
    morphX = juce::jlimit(0.0f,1.0f,x);
    morphY = juce::jlimit(0.0f,1.0f,y);

    visualState.morphX = morphX;
    visualState.morphY = morphY;
}

/*
void MorphVoice::updateVisualState()
{
    visualState.orbiterAngle += visualState.orbiterAngleIncrement;

    while (visualState.orbiterAngle >= juce::MathConstants<float>::twoPi)
    {
        visualState.orbiterAngle -= juce::MathConstants<float>::twoPi;
    }
    while (visualState.orbiterAngle < 0.0f)
    {
        visualState.orbiterAngle += juce::MathConstants<float>::twoPi;
    }

    if (!noteReleased)
    {
        if (visualState.stageProgress < 0.33f)
        {
            visualState.stage = VisualADSRStage::Attack;
        }
        else if (visualState.stageProgress < 0.66f)
        {
            visualState.stage = VisualADSRStage::Decay;
        }
        else
        {
            visualState.stage = VisualADSRStage::Sustain;
        }
    }
}
*/
