#pragma once
#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <map>

//==============================================================================
// DSP helpers (global, so both processor and voice can use them)

struct Oscillator
{
    void prepare (double sr);
    void setWave (int type);
    void setLevel (float l) { level = l; }
    void setFreq (float f)  { freq = f; }
    float process();

    double sampleRate { 44100.0 };
    float phase       { 0.0f };
    float freq        { 440.0f };
    float level       { 0.5f };
    int   waveType    { 0 }; // 0=sine,1=saw,2=square,3=tri,4=pwm
    float pwmAmount   { 0.5f };
};

struct Envelope
{
    void prepare (double sr);
    void noteOn();
    void noteOff();
    void setParams (float a, float d, float s, float r);
    float process();

    double sampleRate { 44100.0 };
    float attack { 0.01f }, decay { 0.1f }, sustain { 0.8f }, release { 0.2f };
    float value { 0.0f };
    bool  gate  { false };
};

struct NeonFilter
{
    void prepare (double sr);
    void setParams (float cutoff, float reso, int type, float drive);
    float process (float x);

    double sampleRate { 44100.0 };
    float z1 { 0.0f }, z2 { 0.0f };
    float a0 { 1.0f }, a1 { 0.0f }, a2 { 0.0f }, b1 { 0.0f }, b2 { 0.0f };
    float drive { 0.0f };
};

struct Bitcrusher
{
    void setSampleRate (double sr) { sampleRate = sr; }
    void setParams (float bits, float downsample, float asym, float drive,
                    float toneFreq, float toneRes, float mix);
    float process (float x);

    double sampleRate { 44100.0 };
    float bitDepth    { 8.0f };
    int   dsFactor    { 1 };
    float asymmetry   { 0.0f };
    float drive       { 0.0f };
    float toneCut     { 8000.0f };
    float toneRes     { 0.5f };
    float mix         { 0.5f };

    float toneZ1 { 0.0f };
    int   counter { 0 };
    float held    { 0.0f };
};

struct FXChain
{
    void prepare (double sr, int blockSize);
    void updateFromParams (juce::AudioProcessorValueTreeState& apvts);
    void process (juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

    Bitcrusher crush;
    float distDrive { 0.6f };
    float distMix   { 0.5f };

    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float> delay { 48000 };
    float delayFeedback { 0.35f };
    float delayMix      { 0.25f };

    juce::dsp::Reverb reverb;
};

//==============================================================================
// Sound
class NeonSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override        { return true; }
    bool appliesToChannel (int) override     { return true; }
};

//==============================================================================
// Processor forward
class NeonMonoAudioProcessor;

//==============================================================================
// Voice
class NeonMonoVoice : public juce::SynthesiserVoice
{
public:
    NeonMonoVoice (NeonMonoAudioProcessor& proc, juce::AudioProcessorValueTreeState& s);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound,
                    int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void prepare (double sampleRate);

private:
    NeonMonoAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;

    int   currentNote { -1 };
    float baseFreq    { 0.0f };
    bool  active      { false };

    Oscillator osc1, osc2, sub;
    Envelope  ampEnv, filtEnv;
    NeonFilter filter;

    float noiseLevel { 0.0f };
};

//==============================================================================
// Processor
class NeonMonoAudioProcessor : public juce::AudioProcessor
{
public:
    NeonMonoAudioProcessor();
    ~NeonMonoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "Abenton NeonMono"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    friend class NeonMonoVoice;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;
    FXChain fx;

    struct Preset
    {
        juce::String name;
        std::map<juce::String, float> values;
    };

    std::vector<Preset> presets;
    int currentPreset { 0 };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initPresets();
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
