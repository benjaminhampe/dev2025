#pragma once

#include <JuceHeader.h>

// PF FFT
#include <pffft.h>

// SoundTouch
#include <SoundTouch_241/SoundTouch.h>

// Rubber Band
#include <rubberband_400/RubberBandStretcher.h>

//====================== Pitch shifter abstraction ======================

struct IPitchShifter
{
    virtual ~IPitchShifter() = default;
    virtual void prepare(double sampleRate, int blockSize) = 0;
    virtual void setSemitones(float semitones) = 0;
    virtual void processBlock(float* samples, int numSamples) = 0;
};

//====================== SoundTouch ======================

class SoundTouchPitchShifter : public IPitchShifter
{
public:
    void prepare(double sampleRate, int blockSize) override;
    void setSemitones(float semitones) override;
    void processBlock(float* samples, int numSamples) override;

private:
    soundtouch::SoundTouch m_St;
    int m_BlockSize = 0;
    std::vector<float> m_OutBuffer;
};

//====================== RubberBand ======================

class RubberBandPitchShifter : public IPitchShifter
{
public:
    void prepare(double sampleRate, int blockSize) override;
    void setSemitones(float semitones) override;
    void processBlock(float* samples, int numSamples) override;

private:
    int m_BlockSize = 0;
    std::unique_ptr<RubberBand::RubberBandStretcher> m_Stretcher;
    std::vector<float> m_OutBuffer;
};

//====================== Phase‑vocoder (pffft) ======================

class PhaseVocoderPitchShifter : public IPitchShifter
{
public:
    PhaseVocoderPitchShifter() = default;
    ~PhaseVocoderPitchShifter() override;

    void prepare(double sampleRate, int blockSize) override;
    void setSemitones(float semitones) override;
    void processBlock(float* samples, int numSamples) override;

private:
    double m_SampleRate = 44100.0;
    int    m_BlockSize  = 0;
    int    m_FftSize    = 0;

    PFFFT_Setup* m_Pffft = nullptr;
    std::vector<float> m_Window, m_In, m_Out;
    std::vector<float> m_PrevPhase, m_PhaseAcc;
    float m_Ratio = 1.0f;
};

//====================== Main processor ======================

class DarthVaderVoiceAudioProcessor : public juce::AudioProcessor
{
public:
    DarthVaderVoiceAudioProcessor();
    ~DarthVaderVoiceAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DarthVaderVoice"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return m_APVTS; }

private:
    juce::AudioProcessorValueTreeState m_APVTS;

    std::atomic<float>* m_ParamPitchSemitones = nullptr;
    std::atomic<float>* m_ParamPitchMode      = nullptr;
    std::atomic<float>* m_ParamFormantRatio   = nullptr;
    std::atomic<float>* m_ParamLowMidGainDB   = nullptr;
    std::atomic<float>* m_ParamLowpassHz      = nullptr;
    std::atomic<float>* m_ParamSaturation     = nullptr;
    std::atomic<float>* m_ParamCombDepthMs    = nullptr;
    std::atomic<float>* m_ParamCombRateHz     = nullptr;
    std::atomic<float>* m_ParamMix            = nullptr;

    double m_SampleRate = 44100.0;
    int    m_BlockSize  = 0;

    // pffft for formant
    int            m_FftSize    = 0;
    PFFFT_Setup*   m_PffftSetup = nullptr;
    std::vector<float> m_FftIn;
    std::vector<float> m_FftOut;
    std::vector<float> m_Window;

    std::unique_ptr<IPitchShifter> m_PitchShifter;

    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float z1 = 0.0f, z2 = 0.0f;

        float process(float x) noexcept
        {
            float y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            return y;
        }
    };

    Biquad m_LowMidEQ;
    Biquad m_Lowpass;

    std::vector<float> m_CombBuffer;
    int                m_CombWritePos = 0;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateFilters();
    void updateCombSize();
    void updatePitchImpl();

    static Biquad makePeak(float fs, float f0, float Q, float gainDB);
    static Biquad makeLowpass(float fs, float f0, float Q);

    void formantShiftInPlace(float* data, int numSamples);
    void applyCepstralFormantShift(float* frame, int numSamples, float ratio);

    float processVaderChain(float x, int sampleIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarthVaderVoiceAudioProcessor)
};


// juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
