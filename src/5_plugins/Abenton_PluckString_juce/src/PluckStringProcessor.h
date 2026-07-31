#pragma once

#include <JuceHeader.h>

class WavetableSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice() {}

    void setWavetable (const juce::AudioBuffer<float>* wt)
    {
        wavetable = wt;
    }

    void setADSR (float attack, float decay, float sustain, float release)
    {
        adsrParams.attack  = attack;
        adsrParams.decay   = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
        adsr.setParameters (adsrParams);
    }

    bool canPlaySound (juce::SynthesiserSound* s) override
    {
        return dynamic_cast<WavetableSound*> (s) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int) override
    {
        if (wavetable == nullptr)
            return;

        auto sr = getSampleRate();
        auto f  = 440.0 * std::pow (2.0, (midiNoteNumber - 69) / 12.0);
        auto baseFreq = sr / (double) wavetable->getNumSamples();
        phaseDelta = (float) (f / baseFreq);
        phase = 0.0f;

        adsr.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
            adsr.noteOff();
        else
        {
            clearCurrentNote();
            adsr.reset();
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (wavetable == nullptr)
            return;

        auto* wtData = wavetable->getReadPointer (0);
        auto wtSize  = wavetable->getNumSamples();

        adsr.setSampleRate (getSampleRate());

        for (int i = 0; i < numSamples; ++i)
        {
            auto env = adsr.getNextSample();
            if (! adsr.isActive())
            {
                clearCurrentNote();
                break;
            }

            auto idx = (int) phase;
            auto frac = phase - (float) idx;
            auto s0 = wtData[idx % wtSize];
            auto s1 = wtData[(idx + 1) % wtSize];
            auto sample = (s0 + frac * (s1 - s0)) * env;

            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
                outputBuffer.addSample (ch, startSample + i, sample);

            phase += phaseDelta;
            if (phase >= (float) wtSize)
                phase -= (float) wtSize;
        }
    }

private:
    const juce::AudioBuffer<float>* wavetable = nullptr;
    float phase = 0.0f, phaseDelta = 0.0f;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
};

class PluckStringProcessor : public juce::AudioProcessor
{
public:
    PluckStringProcessor();
    ~PluckStringProcessor() override = default;

    // AudioProcessor
    const juce::String getName() const override { return "StringWave"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    const juce::AudioBuffer<float>& getWavetable() const { return wavetable; }
    const juce::Array<float>& getStringState() const { return y_lp; }

    void triggerNoteFromEditor (int midiNote);

private:
    juce::AudioProcessorValueTreeState apvts;

    juce::Synthesiser synth;
    juce::AudioBuffer<float> wavetable;

    // string model
    int N = 300;
    double dt = 0.0008;
    double dx = 1.0 / (N - 1);
    juce::Array<float> uPrev, u, uNext, y_lp;

    void initString();
    void stepString (int iterations, double sampleRate);
    void rebuildWavetable();

    juce::MidiBuffer editorMidi;

    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluckStringProcessor)
};
