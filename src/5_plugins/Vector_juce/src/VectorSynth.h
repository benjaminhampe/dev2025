#pragma once
#include <JuceHeader.h>
#include <de/Vector_ADSR.h>

class VectorOsc
{
public:
    VectorOsc();

    void prepare(double newSampleRate);

    void reset();

    void setFrequency(float newFrequency);

    void setAmplitude(float newAmplitude);

    void setPhaseOffset(float radians);

    enum class Waveform
    {
        Sine = 0, Saw, Square, Triangle
    };

    void setWaveform(Waveform newWaveform);

    float nextSample();

private:
    float sampleRate = 48000.0f;
    float frequency = 440.0f;
    float amplitude = 0.25f;
    float phase = 0.0f;
    float phaseOffset = 0.0f;
    float phaseIncrement = 6.282f * frequency / static_cast<float>(sampleRate);
    Waveform waveform = Waveform::Sine;
};

class VectorSynth
{
public:
    VectorSynth();

    void prepare(double sampleRate);

    void reset();

    void setFrequency(float frequency);

    float process(float morphX,
                  float morphY);

public:

    VectorOsc topLeft;
    VectorOsc topRight;
    VectorOsc bottomLeft;
    VectorOsc bottomRight;
};


class MorphSound : public juce::SynthesiserSound
{
public:

    bool appliesToNote (int /*midiNoteNumber*/) override
    {
        return true;
    }

    bool appliesToChannel (int /*midiChannel*/) override
    {
        return true;
    }
};

struct NoteVisualState
{
    bool bPlaying = false;

    bool bNoteReleased = false;

    juce::Point<float> beforeReleasePosition{ 0.0f, 0.0f };

    int midiNote = -1;

    // ADSR visualization

    de::vec::Env::ePhase stage = de::vec::Env::ePhase::Idle;

    float stageProgress = 0.0f;

    // Current cursor position inside display

    juce::Point<float> cursorPosition{ 0.0f, 0.0f };

    // Orbiter state

    float orbiterAngle = 0.0f;

    float orbiterAngleIncrement = 0.05f;

    // Future morph position

    float morphX = 0.5f;

    float morphY = 0.5f;

    // Voice lifetime

    double noteOnTimeSeconds = 0.0;

    double noteOffTimeSeconds = 0.0;

    void reset()
    {
        bPlaying = false;
        bNoteReleased = false;
        midiNote = -1;

        stage = de::vec::Env::ePhase::Idle;
        stageProgress = 0.0f;

        orbiterAngle = 0.0f;

        morphX = 0.5f;
        morphY = 0.5f;

        cursorPosition = {};

        noteOnTimeSeconds = 0.0;
        noteOffTimeSeconds = 0.0;
    }
};

/*
class VisualStateManager
{
public:
    static constexpr int maxVoices = 32;

    void updateVoiceState(int voiceIndex, const NoteVisualState& state)
    {
        jassert(voiceIndex >= 0);
        jassert(voiceIndex < maxVoices);
        states[(size_t) voiceIndex] = state;
    }

    NoteVisualState getVoiceState(int voiceIndex) const
    {
        jassert(voiceIndex >= 0);
        jassert(voiceIndex < maxVoices);

        return states[(size_t) voiceIndex];
    }

    const std::array<NoteVisualState, maxVoices>&
    getStates() const { return states; }

    void clear()
    {
        for (auto& state : states) state = {};
    }

private:
    std::array<NoteVisualState, maxVoices> states {};
};
*/

class MorphVoice : public juce::SynthesiserVoice
{
public:
    MorphVoice();

    ~MorphVoice() override;

    bool isVoiceActive() const override;

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    void startNote(
        int midiNoteNumber,
        float velocity,
        juce::SynthesiserSound* sound,
        int currentPitchWheelPosition) override;

    void stopNote(float velocity,bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int ccNumber,int ccValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
        int startSample, int numSamples) override;

    void prepare(double sampleRate,int samplesPerBlock,int numChannels);

    void setVoiceIndex(int index);

    int getVoiceIndex() const;

    NoteVisualState& getVisualState() { return visualState; }

    void setMorphPosition(float x, float y);

private:
    int voiceIndex = -1;

    double currentSampleRate = 48000.0;

    VectorSynth synth;

    // juce::ADSR adsr;
    // juce::ADSR::Parameters adsrParameters;

    de::vec::Env adsr;
    de::vec::Env::Cfg adsrParameters;

    NoteVisualState visualState;

    float currentFrequency = 440.0f;

    float morphX = 0.5f;
    float morphY = 0.5f;
};
