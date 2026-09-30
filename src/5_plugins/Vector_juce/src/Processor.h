#pragma once
#include <JuceHeader.h>
#include <de/Vector_ADSR.h>

class VectorPluginProcessor;

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

class VectorSynthesiser;

class VectorVoice : public juce::SynthesiserVoice
{
public:
    VectorVoice(VectorSynthesiser& synth,
        juce::AudioProcessorValueTreeState& apvts);

    ~VectorVoice() override;

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

    // void setMorphPosition(float x, float y);

    juce::Point<float>
    getCursorPosition() const
    {
        return m_cursorPos;
    }

    juce::Point<float>
    getOrbiterPosition() const
    {
        return m_orbiterPos;
    }

    float getOrbPhase() const { return m_orbPhase; }

public:
    // VectorPluginProcessor& processor;
    juce::AudioProcessorValueTreeState& m_apvts;
    VectorSynthesiser& m_synth;

    int m_voiceIndex = -1;
    int m_midiNote = -1;


    bool m_bPlaying = false;
    bool m_bNoteReleased = false;

    double m_sampleRate = 48000.0;

    // ADSR
    de::vec::Env m_adsr;
    de::vec::Env::Cfg m_adsrParameters;
    de::vec::Env::ePhase m_adsrStage = de::vec::Env::ePhase::Idle;
    float m_adsrStageProgress = 0.0f;

    VectorOsc m_topLeft;        // Oscillator A
    VectorOsc m_topRight;       // Oscillator B
    VectorOsc m_bottomLeft;     // Oscillator C
    VectorOsc m_bottomRight;    // Oscillator D

    float m_frequency = 440.0f;

    // Future morph position
    // float m_morphX = 0.5f;
    // float m_morphY = 0.5f;

    // Current cursor position inside display
    juce::Point<float> m_cursorPos{ 0.0f, 0.0f };
    juce::Point<float> m_cursorPosRelease{ 0.0f, 0.0f };
    juce::Point<float> m_orbiterPos{ 0.0f, 0.0f };

    // Orbiter state
    // float orbitRadius = .01f; // in range [0,1], will be scaled to actual screen size when drawing
    // float orbitSpeed = 0.05f;
    float m_orbPhase = 0.0f;
    float m_orbPhaseIncrement = 0.05f;

    // Voice lifetime
    double m_noteOnTimeSeconds = 0.0;
    double m_noteOffTimeSeconds = 0.0;

    void reset();
    void updateCursorPosition();
    void updateOrbiterPosition();

private:
    float interpolateSample(float morphX, float morphY);
};

class VectorSynthesiser : public juce::Synthesiser
{
public:
    VectorSynthesiser(juce::AudioProcessorValueTreeState& apvts);

    juce::AudioProcessorValueTreeState& m_apvts;

    std::vector<VectorVoice*> m_voices;

    std::vector<VectorVoice*>& getVoices() { return m_voices; }
    const std::vector<VectorVoice*>& getVoices() const { return m_voices; }

    juce::Point<float> m_attackPoint;
    juce::Point<float> m_decayPoint;
    juce::Point<float> m_sustainPoint;
    juce::Point<float> m_releasePoint;

    float m_orbRadius = 0.0f;
    float m_orbSpeed = 0.0f;


    void setOrbiterSpeed(float speed_in_Hz);
};

class VectorPluginProcessor : public juce::AudioProcessor
{
public:
    VectorPluginProcessor();
    ~VectorPluginProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& name) override;

    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    //VisualStateManager& getVisualStateManager() { return visualStateManager; }

    // std::vector<NoteVisualState*>& getVisualStates() { return visualStates; }

    VectorSynthesiser& getSynth() { return m_synth; }

    juce::AudioProcessorEditor* createEditor() override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return m_apvts; }
private:
    juce::AudioProcessorValueTreeState m_apvts;

    VectorSynthesiser m_synth;

    struct Preset
    {
        juce::String name;
        std::map<juce::String, float> values;
    };

    std::vector<Preset> m_presets;
    int m_preset{ 0 };


    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initPresets();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorPluginProcessor)
};
