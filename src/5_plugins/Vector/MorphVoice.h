#pragma once

#include <JuceHeader.h>

#include "CompositionSynth.h"
#include "NoteVisualState.h"

class MorphVoice : public juce::SynthesiserVoice
{
public:

    MorphVoice();

    ~MorphVoice() override = default;

    bool canPlaySound(
        juce::SynthesiserSound* sound) override;

    void startNote(
        int midiNoteNumber,
        float velocity,
        juce::SynthesiserSound* sound,
        int currentPitchWheelPosition) override;

    void stopNote(
        float velocity,
        bool allowTailOff) override;

    void pitchWheelMoved(
        int newPitchWheelValue) override;

    void controllerMoved(
        int controllerNumber,
        int newControllerValue) override;

    void renderNextBlock(
        juce::AudioBuffer<float>& outputBuffer,
        int startSample,
        int numSamples) override;

    void prepare(
        double sampleRate,
        int samplesPerBlock,
        int numChannels);

    void setVoiceIndex(int index);

    int getVoiceIndex() const;

    NoteVisualState& getVisualState();

    void setMorphPosition(
        float x,
        float y);

private:

    void updateVisualState();

private:

    int voiceIndex = -1;

    double currentSampleRate = 44100.0;

    CompositionSynth synth;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParameters;

    NoteVisualState visualState;

    float currentFrequency = 440.0f;

    float morphX = 0.5f;
    float morphY = 0.5f;

    bool noteReleased = false;
};