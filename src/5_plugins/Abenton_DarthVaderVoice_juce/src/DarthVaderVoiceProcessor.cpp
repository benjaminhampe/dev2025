#include "DarthVaderVoiceProcessor.h"
#include "DarthVaderVoiceEditor.h"
#include <DarkImage.h>

//====================== SoundTouchPitchShifter ======================

void SoundTouchPitchShifter::prepare(double sampleRate, int blockSize)
{
    m_BlockSize = blockSize;

    m_St.setSampleRate((uint) sampleRate);
    m_St.setChannels(1);
    m_St.setTempo(1.0f);

    m_OutBuffer.resize(blockSize);
}

void SoundTouchPitchShifter::setSemitones(float semitones)
{
    m_St.setPitchSemiTones(semitones);
}

void SoundTouchPitchShifter::processBlock(float* samples, int numSamples)
{
    m_St.putSamples(samples, numSamples);

    int available = m_St.numSamples();
    if (available >= numSamples)
    {
        m_St.receiveSamples(m_OutBuffer.data(), numSamples);
        std::copy(m_OutBuffer.begin(), m_OutBuffer.begin() + numSamples, samples);
    }
    // else: leave dry until buffer fills
}

//====================== RubberBandPitchShifter ======================

void RubberBandPitchShifter::prepare(double sampleRate, int blockSize)
{
    m_BlockSize = blockSize;

    m_Stretcher = std::make_unique<RubberBand::RubberBandStretcher>(
        (size_t) sampleRate, 1,
        RubberBand::RubberBandStretcher::OptionProcessRealTime |
        RubberBand::RubberBandStretcher::OptionPitchHighQuality);

    m_OutBuffer.resize(blockSize);
}

void RubberBandPitchShifter::setSemitones(float semitones)
{
    float ratio = std::pow(2.0f, semitones / 12.0f);
    if (m_Stretcher)
        m_Stretcher->setPitchScale(ratio);
}

void RubberBandPitchShifter::processBlock(float* samples, int numSamples)
{
    if (!m_Stretcher)
        return;

    const float* in[1] = { samples };
    m_Stretcher->process(in, numSamples, false);

    int available = m_Stretcher->available();
    if (available >= numSamples)
    {
        float* out[1] = { m_OutBuffer.data() };
        m_Stretcher->retrieve(out, numSamples);
        std::copy(m_OutBuffer.begin(), m_OutBuffer.begin() + numSamples, samples);
    }
}

//====================== PhaseVocoderPitchShifter ======================

PhaseVocoderPitchShifter::~PhaseVocoderPitchShifter()
{
    if (m_Pffft)
        pffft_destroy_setup(m_Pffft);
}

void PhaseVocoderPitchShifter::prepare(double sampleRate, int blockSize)
{
    m_SampleRate = sampleRate;
    m_BlockSize  = blockSize;
    m_FftSize    = blockSize * 2;

    if (m_Pffft)
        pffft_destroy_setup(m_Pffft);
    m_Pffft = pffft_new_setup(m_FftSize, PFFFT_REAL);

    m_Window.resize(m_FftSize);
    for (int i = 0; i < m_FftSize; ++i)
        m_Window[i] = 0.5f - 0.5f * std::cos(2.0 * juce::MathConstants<double>::pi * i / (m_FftSize - 1));

    m_In.assign(m_FftSize, 0.0f);
    m_Out.assign(m_FftSize, 0.0f);
    m_PrevPhase.assign(m_FftSize / 2, 0.0f);
    m_PhaseAcc.assign(m_FftSize / 2, 0.0f);
}

void PhaseVocoderPitchShifter::setSemitones(float semitones)
{
    m_Ratio = std::pow(2.0f, semitones / 12.0f);
}

void PhaseVocoderPitchShifter::processBlock(float* samples, int numSamples)
{
    if (!m_Pffft || numSamples != m_BlockSize)
        return;

    const int N = m_FftSize;
    const int H = m_BlockSize;

    std::fill(m_In.begin(), m_In.end(), 0.0f);
    for (int i = 0; i < H; ++i)
        m_In[i] = samples[i] * m_Window[i];

    pffft_transform_ordered(m_Pffft, m_In.data(), m_Out.data(), nullptr, PFFFT_FORWARD);

    int bins = N / 2;
    float binHz = (float) m_SampleRate / (float) N;

    std::vector<float> newReal(N, 0.0f);

    for (int k = 1; k < bins; ++k)
    {
        float re = m_Out[k];
        float mag = std::abs(re);
        float phase = std::atan2(0.0f, re);

        float delta = phase - m_PrevPhase[k];
        m_PrevPhase[k] = phase;

        float expected = 2.0f * juce::MathConstants<float>::pi * (float) H * (float) k / (float) N;
        float diff = delta - expected;
        diff = std::fmod(diff + juce::MathConstants<float>::pi,
                         2.0f * juce::MathConstants<float>::pi)
             - juce::MathConstants<float>::pi;

        float trueFreq = (float) k * binHz +
                         diff * (float) m_SampleRate /
                         (2.0f * juce::MathConstants<float>::pi * (float) H);

        float newBin = (float) k * m_Ratio;
        int k2 = (int) newBin;

        if (k2 >= 1 && k2 < bins)
        {
            m_PhaseAcc[k2] += 2.0f * juce::MathConstants<float>::pi *
                              trueFreq * (float) H / (float) m_SampleRate;

            newReal[k2] = mag * std::cos(m_PhaseAcc[k2]);
        }
    }

    std::copy(newReal.begin(), newReal.end(), m_Out.begin());

    pffft_transform_ordered(m_Pffft, m_Out.data(), m_In.data(), nullptr, PFFFT_BACKWARD);

    for (int i = 0; i < H; ++i)
        samples[i] = (m_In[i] / (float) N) * m_Window[i];
}

//====================== Processor ======================

DarthVaderVoiceAudioProcessor::DarthVaderVoiceAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      m_APVTS(*this, nullptr, "PARAMS", createParameterLayout())
{
    DE_BENNI("=======================================")
    DE_BENNI("DarthVaderVoiceAudioProcessor")
    DE_BENNI("=======================================")
    m_ParamPitchSemitones = m_APVTS.getRawParameterValue("pitch");
    m_ParamPitchMode      = m_APVTS.getRawParameterValue("pitchMode");
    m_ParamFormantRatio   = m_APVTS.getRawParameterValue("formant");
    m_ParamLowMidGainDB   = m_APVTS.getRawParameterValue("lowmid");
    m_ParamLowpassHz      = m_APVTS.getRawParameterValue("lowpass");
    m_ParamSaturation     = m_APVTS.getRawParameterValue("saturation");
    m_ParamCombDepthMs    = m_APVTS.getRawParameterValue("combDepth");
    m_ParamCombRateHz     = m_APVTS.getRawParameterValue("combRate");
    m_ParamMix            = m_APVTS.getRawParameterValue("mix");

    setPlayConfigDetails(2, 2, 48000.0, 256);
}

DarthVaderVoiceAudioProcessor::~DarthVaderVoiceAudioProcessor()
{
    if (m_PffftSetup)
        pffft_destroy_setup(m_PffftSetup);
}

juce::AudioProcessorValueTreeState::ParameterLayout
DarthVaderVoiceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitch", "Pitch Shift",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        -6.0f,
        "Pitch Shift [semitones]"));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "pitchMode", "Pitch Mode",
        juce::StringArray{ "SoundTouch", "RubberBand", "PhaseVocoder" },
        0,
        "Pitch Shifter Algorithm"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "formant", "Formant Ratio",
        juce::NormalisableRange<float>(0.5f, 1.2f, 0.001f),
        0.8f,
        "Formant Ratio [×]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lowmid", "Low-Mid Boost",
        juce::NormalisableRange<float>(0.0f, 18.0f, 0.1f),
        9.0f,
        "Low-Mid Boost [dB @ 220 Hz]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lowpass", "Lowpass Cutoff",
        juce::NormalisableRange<float>(1500.0f, 6000.0f, 1.0f, 0.4f),
        3800.0f,
        "Lowpass Cutoff [Hz]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "saturation", "Saturation",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f),
        0.8f,
        "Saturation Amount [tanh drive]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "combDepth", "Mask Depth",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        8.0f,
        "Mask Resonance Delay [ms]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "combRate", "Mask Rate",
        juce::NormalisableRange<float>(0.1f, 3.0f, 0.01f),
        0.6f,
        "Mask Modulation Rate [Hz]"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        "Wet/Dry Mix [0–1]"));

    return { params.begin(), params.end() };
}

/*
bool DarthVaderVoiceAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}
*/

bool DarthVaderVoiceAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet()  != juce::AudioChannelSet::stereo()) return false;
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    return true;
}


void DarthVaderVoiceAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DE_DEBUG("prepareToPlay(",sampleRate,",",samplesPerBlock,")")
    m_SampleRate = sampleRate;
    m_BlockSize  = samplesPerBlock;

    // pffft for formant
    m_FftSize = samplesPerBlock * 2;
    if (m_PffftSetup)
        pffft_destroy_setup(m_PffftSetup);
    m_PffftSetup = pffft_new_setup(m_FftSize, PFFFT_REAL);

    m_FftIn.assign(m_FftSize, 0.0f);
    m_FftOut.assign(m_FftSize, 0.0f);
    m_Window.assign(m_FftSize, 0.0f);
    for (int i = 0; i < m_FftSize; ++i)
        m_Window[i] = 0.5f - 0.5f * std::cos(2.0 * juce::MathConstants<double>::pi * i / (m_FftSize - 1));

    updateCombSize();
    updateFilters();
    updatePitchImpl();
}

void DarthVaderVoiceAudioProcessor::updateCombSize()
{
    float maxDepthMs = 20.0f;
    int maxSamples = (int) std::ceil(maxDepthMs * 0.001f * (float) m_SampleRate) + 4;
    m_CombBuffer.assign(maxSamples, 0.0f);
    m_CombWritePos = 0;
}

void DarthVaderVoiceAudioProcessor::updateFilters()
{
    float fs = (float) m_SampleRate;
    float lowMidGain = m_ParamLowMidGainDB ? m_ParamLowMidGainDB->load() : 9.0f;
    float lowpassHz  = m_ParamLowpassHz    ? m_ParamLowpassHz->load()    : 3800.0f;

    m_LowMidEQ = makePeak(fs, 220.0f, 0.7f, lowMidGain);
    m_Lowpass  = makeLowpass(fs, lowpassHz, 0.707f);
}

void DarthVaderVoiceAudioProcessor::updatePitchImpl()
{
    int mode = m_ParamPitchMode ? (int) m_ParamPitchMode->load() : 0;

    switch (mode)
    {
        case 0: m_PitchShifter = std::make_unique<SoundTouchPitchShifter>(); break;
        case 1: m_PitchShifter = std::make_unique<RubberBandPitchShifter>(); break;
        case 2: m_PitchShifter = std::make_unique<PhaseVocoderPitchShifter>(); break;
        default: m_PitchShifter = std::make_unique<SoundTouchPitchShifter>(); break;
    }

    if (m_PitchShifter)
        m_PitchShifter->prepare(m_SampleRate, m_BlockSize);
}

DarthVaderVoiceAudioProcessor::Biquad
DarthVaderVoiceAudioProcessor::makePeak(float fs, float f0, float Q, float gainDB)
{
    Biquad b;
    float A  = std::pow(10.0f, gainDB / 40.0f);
    float w0 = 2.0f * juce::MathConstants<float>::pi * f0 / fs;
    float alpha = std::sin(w0) / (2.0f * Q);
    float cosw0 = std::cos(w0);

    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha / A;

    b.b0 = b0 / a0;
    b.b1 = b1 / a0;
    b.b2 = b2 / a0;
    b.a1 = a1 / a0;
    b.a2 = a2 / a0;

    return b;
}

DarthVaderVoiceAudioProcessor::Biquad
DarthVaderVoiceAudioProcessor::makeLowpass(float fs, float f0, float Q)
{
    Biquad b;
    float w0 = 2.0f * juce::MathConstants<float>::pi * f0 / fs;
    float alpha = std::sin(w0) / (2.0f * Q);
    float cosw0 = std::cos(w0);

    float b0 = (1.0f - cosw0) * 0.5f;
    float b1 = 1.0f - cosw0;
    float b2 = (1.0f - cosw0) * 0.5f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;

    b.b0 = b0 / a0;
    b.b1 = b1 / a0;
    b.b2 = b2 / a0;
    b.a1 = a1 / a0;
    b.a2 = a2 / a0;

    return b;
}

void DarthVaderVoiceAudioProcessor::formantShiftInPlace(float* data, int numSamples)
{
    float ratio = m_ParamFormantRatio ? m_ParamFormantRatio->load() : 0.8f;
    if (ratio == 1.0f || !m_PffftSetup)
        return;

    if (numSamples > m_FftSize)
        numSamples = m_FftSize;

    applyCepstralFormantShift(data, numSamples, ratio);
}

void DarthVaderVoiceAudioProcessor::applyCepstralFormantShift(float* frame, int numSamples, float ratio)
{
    int N = m_FftSize;

    std::fill(m_FftIn.begin(), m_FftIn.end(), 0.0f);
    for (int i = 0; i < numSamples; ++i)
        m_FftIn[i] = frame[i] * m_Window[i];

    pffft_transform_ordered(m_PffftSetup, m_FftIn.data(), m_FftOut.data(), nullptr, PFFFT_FORWARD);

    std::vector<float> mag(N), logMag(N);
    for (int k = 0; k < N; ++k)
    {
        float re = m_FftOut[k];
        float m = std::abs(re);
        mag[k] = m;
        logMag[k] = std::log(m + 1e-6f);
    }

    std::vector<float> cep(N), cepWarp(N);

    std::copy(logMag.begin(), logMag.end(), m_FftIn.begin());
    pffft_transform_ordered(m_PffftSetup, m_FftIn.data(), m_FftOut.data(), nullptr, PFFFT_BACKWARD);

    for (int n = 0; n < N; ++n)
        cep[n] = m_FftOut[n] / (float) N;

    int lifterN = 40;
    for (int n = lifterN; n < N; ++n)
        cep[n] = 0.0f;

    for (int n = 0; n < N; ++n)
    {
        float idx = (float) n / ratio;
        if (idx >= (float) (N - 1))
            idx = (float) (N - 1);

        int i0 = (int) idx;
        int i1 = juce::jmin(i0 + 1, N - 1);
        float frac = idx - (float) i0;
        cepWarp[n] = cep[i0] * (1.0f - frac) + cep[i1] * frac;
    }

    std::copy(cepWarp.begin(), cepWarp.end(), m_FftIn.begin());
    pffft_transform_ordered(m_PffftSetup, m_FftIn.data(), m_FftOut.data(), nullptr, PFFFT_FORWARD);

    for (int k = 0; k < N; ++k)
        logMag[k] = m_FftOut[k];

    for (int k = 0; k < N; ++k)
    {
        float env = std::exp(logMag[k]);
        float scale = env / (mag[k] + 1e-6f);
        m_FftOut[k] *= scale;
    }

    pffft_transform_ordered(m_PffftSetup, m_FftOut.data(), m_FftIn.data(), nullptr, PFFFT_BACKWARD);

    for (int i = 0; i < numSamples; ++i)
        frame[i] = (m_FftIn[i] / (float) N) * m_Window[i];
}

float DarthVaderVoiceAudioProcessor::processVaderChain(float x, int sampleIndex)
{
    float s = x;

    s = m_LowMidEQ.process(s);
    s = m_Lowpass.process(s);

    float drive = m_ParamSaturation ? m_ParamSaturation->load() : 0.8f;
    s = std::tanh(drive * s);

    float depthMs = m_ParamCombDepthMs ? m_ParamCombDepthMs->load() : 8.0f;
    float rateHz  = m_ParamCombRateHz ? m_ParamCombRateHz->load() : 0.6f;

    int combSize = (int) m_CombBuffer.size();
    float t = (float) sampleIndex / (float) m_SampleRate;
    float mod = 0.5f * (1.0f + std::sin(2.0f * juce::MathConstants<float>::pi * rateHz * t));
    float delaySamples = depthMs * 0.001f * (float) m_SampleRate * (0.5f + 0.5f * mod);

    int dInt = (int) delaySamples;
    int readPos = m_CombWritePos - dInt;
    while (readPos < 0) readPos += combSize;

    float yDelay = m_CombBuffer[readPos];
    float y = 0.7f * yDelay + 0.3f * s;

    m_CombBuffer[m_CombWritePos] = s + 0.3f * yDelay;
    m_CombWritePos = (m_CombWritePos + 1) % combSize;

    float mix = m_ParamMix ? m_ParamMix->load() : 1.0f;
    return x * (1.0f - mix) + y * mix;
}

/*
void DarthVaderVoiceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer&)
{
    DE_BENNI("processBlock")
    juce::ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    int numCh      = buffer.getNumChannels();

    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        for (int n = 0; n < numSamples; ++n)
            data[n] *= 0.1f; // -20 dB
    }
}
*/
void DarthVaderVoiceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    int numCh      = buffer.getNumChannels();

    auto* left  = buffer.getWritePointer(0);
    auto* right = numCh > 1 ? buffer.getWritePointer(1) : nullptr;

    updateFilters();

    std::vector<float> mono(numSamples);
    for (int n = 0; n < numSamples; ++n)
        mono[n] = left[n];

    if (m_PitchShifter)
    {
        float semitones = m_ParamPitchSemitones ? m_ParamPitchSemitones->load() : -6.0f;
        m_PitchShifter->setSemitones(semitones);
        m_PitchShifter->processBlock(mono.data(), numSamples);
    }

    formantShiftInPlace(mono.data(), numSamples);

    for (int n = 0; n < numSamples; ++n)
    {
        float out = processVaderChain(mono[n], n);
        left[n] = out;
        if (right) right[n] = out;
    }
}

void DarthVaderVoiceAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = m_APVTS.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DarthVaderVoiceAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(m_APVTS.state.getType()))
        m_APVTS.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* DarthVaderVoiceAudioProcessor::createEditor()
{
    return new DarthVaderVoiceAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DarthVaderVoiceAudioProcessor();
}
