#include "NeonMonoProcessor.h"
#include "NeonMonoEditor.h"
#include <cstdlib>

using namespace juce;

//==============================================================================
// Oscillator
void Oscillator::prepare (double sr)
{
    sampleRate = sr;
    phase = 0.0f;
}

void Oscillator::setWave (int type)
{
    waveType = type;
}

float Oscillator::process()
{
    float inc = (float) (2.0 * MathConstants<double>::pi * freq / sampleRate);
    phase += inc;
    if (phase > MathConstants<float>::twoPi)
        phase -= MathConstants<float>::twoPi;

    float x = 0.0f;
    switch (waveType)
    {
        case 0: x = std::sin (phase); break; // sine
        case 1: x = (phase / MathConstants<float>::pi) - 1.0f; break; // saw
        case 2: x = phase < MathConstants<float>::pi ? 1.0f : -1.0f; break; // square
        case 3: // triangle
        {
            float t = phase / MathConstants<float>::pi;
            x = t < 1.0f ? (t * 2.0f - 1.0f) : ((2.0f - t) * 2.0f - 1.0f);
            break;
        }
        case 4: // pwm square
        {
            float duty = 0.5f + (pwmAmount - 0.5f) * 0.4f;
            x = (phase / MathConstants<float>::twoPi) < duty ? 1.0f : -1.0f;
            break;
        }
        default: x = std::sin (phase); break;
    }

    return x * level;
}

//==============================================================================
// Envelope
void Envelope::prepare (double sr)
{
    sampleRate = sr;
    value = 0.0f;
    gate  = false;
}

void Envelope::noteOn()
{
    gate = true;
}

void Envelope::noteOff()
{
    gate = false;
}

void Envelope::setParams (float a, float d, float s, float r)
{
    attack  = a;
    decay   = d;
    sustain = s;
    release = r;
}

float Envelope::process()
{
    float dt = 1.0f / (float) sampleRate;

    if (gate)
    {
        if (value < 1.0f)
            value += dt / jmax (attack, 0.0001f);
        else if (value > sustain)
            value -= dt / jmax (decay, 0.0001f);
    }
    else
    {
        if (value > 0.0f)
            value -= dt / jmax (release, 0.0001f);
    }

    value = jlimit (0.0f, 1.0f, value);
    return value;
}

//==============================================================================
// Filter
void NeonFilter::prepare (double sr)
{
    sampleRate = sr;
    z1 = z2 = 0.0f;
}

void NeonFilter::setParams (float cutoff, float reso, int type, float drv)
{
    drive = drv;

    float fc = cutoff;
    fc = jlimit (20.0f, 20000.0f, fc);

    float w0 = 2.0f * MathConstants<float>::pi * fc / (float) sampleRate;
    float cosw0 = std::cos (w0);
    float sinw0 = std::sin (w0);
    float Q = jmap (reso, 0.1f, 10.0f);

    float alpha = sinw0 / (2.0f * Q);

    float b0, b1_, b2_, a0_, a1_, a2_;

    switch (type)
    {
        case 0: // LP
        case 1: // LP24 (same biquad)
        {
            b0 = (1.0f - cosw0) * 0.5f;
            b1_ = 1.0f - cosw0;
            b2_ = (1.0f - cosw0) * 0.5f;
            a0_ = 1.0f + alpha;
            a1_ = -2.0f * cosw0;
            a2_ = 1.0f - alpha;
            break;
        }
        case 2: // BP
        {
            b0 = sinw0 * 0.5f;
            b1_ = 0.0f;
            b2_ = -sinw0 * 0.5f;
            a0_ = 1.0f + alpha;
            a1_ = -2.0f * cosw0;
            a2_ = 1.0f - alpha;
            break;
        }
        case 3: // HP
        {
            b0 = (1.0f + cosw0) * 0.5f;
            b1_ = -(1.0f + cosw0);
            b2_ = (1.0f + cosw0) * 0.5f;
            a0_ = 1.0f + alpha;
            a1_ = -2.0f * cosw0;
            a2_ = 1.0f - alpha;
            break;
        }
        case 4: // Notch
        default:
        {
            b0 = 1.0f;
            b1_ = -2.0f * cosw0;
            b2_ = 1.0f;
            a0_ = 1.0f + alpha;
            a1_ = -2.0f * cosw0;
            a2_ = 1.0f - alpha;
            break;
        }
    }

    a0 = b0 / a0_;
    a1 = b1_ / a0_;
    a2 = b2_ / a0_;
    b1 = a1_;
    b2 = a2_;
}

float NeonFilter::process (float x)
{
    float y = a0 * x + a1 * z1 + a2 * z2 - b1 * z1 - b2 * z2;
    z2 = z1;
    z1 = y;

    y = std::tanh (y * (1.0f + drive * 4.0f));
    return y;
}

//==============================================================================
// Bitcrusher
void Bitcrusher::setParams (float bits, float downsample, float asym,
                            float drv, float toneFreq, float toneRes_, float mix_)
{
    bitDepth = jlimit (2.0f, 16.0f, bits);
    dsFactor = jlimit (1, 32, (int) std::round (downsample));
    asymmetry = asym;
    drive = drv;
    toneCut = jlimit (200.0f, 20000.0f, toneFreq);
    toneRes = toneRes_;
    mix = mix_;

    toneZ1 = 0.0f;
    counter = 0;
    held = 0.0f;
}

float Bitcrusher::process (float x)
{
    if (++counter >= dsFactor)
    {
        counter = 0;
        held = x;
    }

    float crushed = held;

    float maxVal = std::pow (2.0f, bitDepth - 1.0f) - 1.0f;
    crushed = std::round (crushed * maxVal) / maxVal;

    float asymGain = 1.0f + asymmetry * 2.0f;
    if (crushed > 0.0f)
        crushed *= asymGain;

    crushed = std::tanh (crushed * (1.0f + drive * 6.0f));

    float alpha = toneCut / (toneCut + (float) sampleRate);
    toneZ1 = alpha * crushed + (1.0f - alpha) * toneZ1;
    float toned = toneZ1;

    return x + mix * (toned - x);
}

//==============================================================================
// FXChain
void FXChain::prepare (double sr, int blockSize)
{
    crush.setSampleRate (sr);

    dsp::ProcessSpec spec { sr, (uint32) blockSize, 2 };
    chorus.prepare(spec);
    reverb.prepare(spec);
    delay.prepare(spec);
}

void FXChain::updateFromParams (AudioProcessorValueTreeState& apvts)
{
    crush.setParams (
        apvts.getRawParameterValue (PID::crushBits)->load(),
        apvts.getRawParameterValue (PID::crushDownsample)->load(),
        apvts.getRawParameterValue (PID::crushAsym)->load(),
        apvts.getRawParameterValue (PID::crushDrive)->load(),
        apvts.getRawParameterValue (PID::crushToneFreq)->load(),
        apvts.getRawParameterValue (PID::crushToneRes)->load(),
        apvts.getRawParameterValue (PID::crushMix)->load());

    distDrive = apvts.getRawParameterValue (PID::distDrive)->load();
    distMix   = apvts.getRawParameterValue (PID::distMix)->load();

    float chRate  = apvts.getRawParameterValue (PID::chorusRate)->load();
    float chDepth = apvts.getRawParameterValue (PID::chorusDepth)->load();
    float chMix   = apvts.getRawParameterValue (PID::chorusMix)->load();
    chorus.setRate (chRate);
    chorus.setDepth (chDepth);
    chorus.setMix (chMix);

    float delMs   = apvts.getRawParameterValue (PID::delayTimeMs)->load();
    float delFb   = apvts.getRawParameterValue (PID::delayFeedback)->load();
    float delMix  = apvts.getRawParameterValue (PID::delayMix)->load();
    delay.setDelay (delMs * 0.001f * 48000.0f);
    delayFeedback = delFb;
    delayMix      = delMix;

    dsp::Reverb::Parameters rp;
    rp.roomSize = apvts.getRawParameterValue (PID::reverbSize)->load();
    rp.damping  = apvts.getRawParameterValue (PID::reverbDecay)->load();
    rp.wetLevel = apvts.getRawParameterValue (PID::reverbMix)->load();
    rp.dryLevel = 1.0f - rp.wetLevel;
    rp.width    = 1.0f;
    reverb.setParameters (rp);
}

void FXChain::process (AudioBuffer<float>& buffer, AudioProcessorValueTreeState& apvts)
{
    updateFromParams (apvts);

    auto numSamples  = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float x = data[i];

            x = crush.process (x);

            float d = std::tanh (x * (1.0f + distDrive * 8.0f));
            x = x + distMix * (d - x);

            float delayed = delay.popSample (ch);
            delay.pushSample (ch, x + delayed * delayFeedback);
            x = x + delayMix * delayed;

            data[i] = x;
        }
    }

    dsp::AudioBlock<float> block (buffer);
    dsp::ProcessContextReplacing<float> ctx (block);
    chorus.process (ctx);
    reverb.process (ctx);

    bool panic = apvts.getRawParameterValue (PID::cathedralPanic)->load() > 0.5f;
    if (panic)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
                data[i] = jlimit (-1.0f, 1.0f, data[i] * 1.8f);
        }
    }
}

//==============================================================================
// Voice
NeonMonoVoice::NeonMonoVoice (NeonMonoAudioProcessor& proc, AudioProcessorValueTreeState& s)
    : processor (proc), apvts (s)
{
}

bool NeonMonoVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<NeonSound*> (sound) != nullptr;
}

void NeonMonoVoice::prepare (double sampleRate)
{
    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    sub.prepare (sampleRate);
    ampEnv.prepare (sampleRate);
    filtEnv.prepare (sampleRate);
    filter.prepare (sampleRate);
}

void NeonMonoVoice::startNote (int midiNoteNumber, float velocity,
                               SynthesiserSound*, int)
{
    currentNote = midiNoteNumber;
    baseFreq = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    active = true;

    double sr = getSampleRate();
    prepare (sr);

    int osc1Wave = (int) apvts.getRawParameterValue (PID::osc1Wave)->load();
    int osc2Wave = (int) apvts.getRawParameterValue (PID::osc2Wave)->load();
    float osc1Level = apvts.getRawParameterValue (PID::osc1Level)->load();
    float osc2Level = apvts.getRawParameterValue (PID::osc2Level)->load();
    float subLevel  = apvts.getRawParameterValue (PID::subLevel)->load();
    noiseLevel      = apvts.getRawParameterValue (PID::noiseLevel)->load();

    osc1.setWave (osc1Wave);
    osc2.setWave (osc2Wave);
    osc1.setLevel (osc1Level);
    osc2.setLevel (osc2Level);
    sub.setWave (0);
    sub.setLevel (subLevel);

    osc1.setFreq (baseFreq);
    osc2.setFreq (baseFreq);
    sub.setFreq (baseFreq * 0.5f);

    ampEnv.setParams (
        apvts.getRawParameterValue (PID::ampAttack)->load(),
        apvts.getRawParameterValue (PID::ampDecay)->load(),
        apvts.getRawParameterValue (PID::ampSustain)->load(),
        apvts.getRawParameterValue (PID::ampRelease)->load());

    filtEnv.setParams (
        apvts.getRawParameterValue (PID::filtAttack)->load(),
        apvts.getRawParameterValue (PID::filtDecay)->load(),
        apvts.getRawParameterValue (PID::filtSustain)->load(),
        apvts.getRawParameterValue (PID::filtRelease)->load());

    ampEnv.noteOn();
    filtEnv.noteOn();

    float filterCut = apvts.getRawParameterValue (PID::filterCutoff)->load();
    float filterRes = apvts.getRawParameterValue (PID::filterReso)->load();
    float filterDrv = apvts.getRawParameterValue (PID::filterDrive)->load();
    int filterType  = (int) apvts.getRawParameterValue (PID::filterType)->load();

    filter.setParams (filterCut, filterRes, filterType, filterDrv);
}

void NeonMonoVoice::stopNote (float, bool allowTailOff)
{
    ampEnv.noteOff();
    filtEnv.noteOff();

    if (! allowTailOff)
        clearCurrentNote();

    active = false;
}

void NeonMonoVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! active)
        return;

    float filterCut = apvts.getRawParameterValue (PID::filterCutoff)->load();
    float filterRes = apvts.getRawParameterValue (PID::filterReso)->load();
    float filterDrv = apvts.getRawParameterValue (PID::filterDrive)->load();
    int filterType  = (int) apvts.getRawParameterValue (PID::filterType)->load();
    float envAmt    = apvts.getRawParameterValue (PID::filtEnvAmount)->load();

    for (int i = 0; i < numSamples; ++i)
    {
        float envA = ampEnv.process();
        float envF = filtEnv.process();

        float sig = osc1.process() + osc2.process() + sub.process();
        float n = ((float) std::rand() / (float) RAND_MAX) * 2.0f - 1.0f;
        sig += noiseLevel * n;

        float cutoffMod = filterCut * jmap (envF * envAmt, 0.0f, 2.0f);
        filter.setParams (cutoffMod, filterRes, filterType, filterDrv);

        sig = filter.process (sig);
        sig *= envA;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample + i, sig);
    }
}

//==============================================================================
// Parameter layout
AudioProcessorValueTreeState::ParameterLayout NeonMonoAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    using FloatParam = AudioParameterFloat;
    using ChoiceParam = AudioParameterChoice;
    using BoolParam = AudioParameterBool;

    // Voice
    params.push_back (std::make_unique<ChoiceParam> (PID::voiceMode, "Voice Mode",
        StringArray { "Mono", "Legato", "Poly" }, 0));
    params.push_back (std::make_unique<FloatParam> (PID::glideTimeMs, "Glide Time",
        NormalisableRange<float> (0.0f, 200.0f), 40.0f));
    params.push_back (std::make_unique<ChoiceParam> (PID::glideMode, "Glide Mode",
        StringArray { "Off", "Always", "Legato" }, 1));

    // Osc
    params.push_back (std::make_unique<ChoiceParam> (PID::osc1Wave, "Osc1 Wave",
        StringArray { "Sine", "Saw", "Square", "Tri", "PWM" }, 1));
    params.push_back (std::make_unique<FloatParam> (PID::osc1Level, "Osc1 Level",
        NormalisableRange<float> (0.0f, 1.0f), 0.8f));
    params.push_back (std::make_unique<ChoiceParam> (PID::osc2Wave, "Osc2 Wave",
        StringArray { "Sine", "Saw", "Square", "Tri", "PWM" }, 2));
    params.push_back (std::make_unique<FloatParam> (PID::osc2Level, "Osc2 Level",
        NormalisableRange<float> (0.0f, 1.0f), 0.6f));
    params.push_back (std::make_unique<FloatParam> (PID::subLevel, "Sub Level",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::noiseLevel, "Noise Level",
        NormalisableRange<float> (0.0f, 1.0f), 0.1f));

    // Filter
    params.push_back (std::make_unique<ChoiceParam> (PID::filterType, "Filter Type",
        StringArray { "LP12", "LP24", "BP12", "HP12", "Notch12" }, 0));
    params.push_back (std::make_unique<FloatParam> (PID::filterCutoff, "Filter Cutoff",
        NormalisableRange<float> (50.0f, 18000.0f), 800.0f));
    params.push_back (std::make_unique<FloatParam> (PID::filterReso, "Filter Reso",
        NormalisableRange<float> (0.1f, 10.0f), 0.7f));
    params.push_back (std::make_unique<FloatParam> (PID::filterDrive, "Filter Drive",
        NormalisableRange<float> (0.0f, 1.0f), 0.2f));

    // Amp env
    params.push_back (std::make_unique<FloatParam> (PID::ampAttack, "Amp Attack",
        NormalisableRange<float> (0.001f, 2.0f), 0.01f));
    params.push_back (std::make_unique<FloatParam> (PID::ampDecay, "Amp Decay",
        NormalisableRange<float> (0.001f, 2.0f), 0.2f));
    params.push_back (std::make_unique<FloatParam> (PID::ampSustain, "Amp Sustain",
        NormalisableRange<float> (0.0f, 1.0f), 0.8f));
    params.push_back (std::make_unique<FloatParam> (PID::ampRelease, "Amp Release",
        NormalisableRange<float> (0.001f, 4.0f), 0.4f));

    // Filter env
    params.push_back (std::make_unique<FloatParam> (PID::filtAttack, "Filt Attack",
        NormalisableRange<float> (0.001f, 2.0f), 0.02f));
    params.push_back (std::make_unique<FloatParam> (PID::filtDecay, "Filt Decay",
        NormalisableRange<float> (0.001f, 2.0f), 0.3f));
    params.push_back (std::make_unique<FloatParam> (PID::filtSustain, "Filt Sustain",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::filtRelease, "Filt Release",
        NormalisableRange<float> (0.001f, 4.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::filtEnvAmount, "Filt Env Amount",
        NormalisableRange<float> (0.0f, 2.0f), 1.0f));

    // LFOs
    params.push_back (std::make_unique<FloatParam> (PID::lfo1Rate, "LFO1 Rate",
        NormalisableRange<float> (0.1f, 20.0f), 2.0f));
    params.push_back (std::make_unique<FloatParam> (PID::lfo1Amount, "LFO1 Amount",
        NormalisableRange<float> (0.0f, 1.0f), 0.3f));
    params.push_back (std::make_unique<FloatParam> (PID::lfo2Rate, "LFO2 Rate",
        NormalisableRange<float> (0.1f, 20.0f), 4.0f));
    params.push_back (std::make_unique<FloatParam> (PID::lfo2Amount, "LFO2 Amount",
        NormalisableRange<float> (0.0f, 1.0f), 0.3f));
    params.push_back (std::make_unique<FloatParam> (PID::lfo3Rate, "LFO3 Rate",
        NormalisableRange<float> (0.1f, 20.0f), 6.0f));
    params.push_back (std::make_unique<FloatParam> (PID::lfo3Amount, "LFO3 Amount",
        NormalisableRange<float> (0.0f, 1.0f), 0.3f));

    // Bitcrusher
    params.push_back (std::make_unique<FloatParam> (PID::crushBits, "Crush Bits",
        NormalisableRange<float> (2.0f, 16.0f), 8.0f));
    params.push_back (std::make_unique<FloatParam> (PID::crushDownsample, "Crush Downsample",
        NormalisableRange<float> (1.0f, 32.0f), 4.0f));
    params.push_back (std::make_unique<FloatParam> (PID::crushAsym, "Crush Asym",
        NormalisableRange<float> (0.0f, 1.0f), 0.3f));
    params.push_back (std::make_unique<FloatParam> (PID::crushDrive, "Crush Drive",
        NormalisableRange<float> (0.0f, 1.0f), 0.4f));
    params.push_back (std::make_unique<FloatParam> (PID::crushToneFreq, "Crush ToneFreq",
        NormalisableRange<float> (200.0f, 20000.0f), 6000.0f));
    params.push_back (std::make_unique<FloatParam> (PID::crushToneRes, "Crush ToneRes",
        NormalisableRange<float> (0.1f, 1.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::crushMix, "Crush Mix",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));

    // Distortion
    params.push_back (std::make_unique<FloatParam> (PID::distDrive, "Dist Drive",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::distMix, "Dist Mix",
        NormalisableRange<float> (0.0f, 1.0f), 0.6f));

    // Chorus
    params.push_back (std::make_unique<FloatParam> (PID::chorusRate, "Chorus Rate",
        NormalisableRange<float> (0.1f, 5.0f), 1.2f));
    params.push_back (std::make_unique<FloatParam> (PID::chorusDepth, "Chorus Depth",
        NormalisableRange<float> (0.0f, 1.0f), 0.4f));
    params.push_back (std::make_unique<FloatParam> (PID::chorusMix, "Chorus Mix",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));

    // Delay
    params.push_back (std::make_unique<FloatParam> (PID::delayTimeMs, "Delay Time",
        NormalisableRange<float> (50.0f, 1000.0f), 350.0f));
    params.push_back (std::make_unique<FloatParam> (PID::delayFeedback, "Delay Feedback",
        NormalisableRange<float> (0.0f, 0.95f), 0.35f));
    params.push_back (std::make_unique<FloatParam> (PID::delayMix, "Delay Mix",
        NormalisableRange<float> (0.0f, 1.0f), 0.3f));

    // Reverb
    params.push_back (std::make_unique<FloatParam> (PID::reverbSize, "Reverb Size",
        NormalisableRange<float> (0.0f, 1.0f), 0.6f));
    params.push_back (std::make_unique<FloatParam> (PID::reverbDecay, "Reverb Decay",
        NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    params.push_back (std::make_unique<FloatParam> (PID::reverbMix, "Reverb Mix",
        NormalisableRange<float> (0.0f, 1.0f), 0.4f));

    // Fun feature
    params.push_back (std::make_unique<BoolParam> (PID::cathedralPanic, "Cathedral Panic", false));

    return { params.begin(), params.end() };
}

//==============================================================================
// Presets
void NeonMonoAudioProcessor::initPresets()
{
    presets.clear();

    auto addPreset = [this] (const String& name, std::initializer_list<std::pair<String, float>> vals)
    {
        Preset p;
        p.name = name;
        for (auto& v : vals)
            p.values[v.first] = v.second;
        presets.push_back (std::move (p));
    };

    // 5 leads
    addPreset ("Lead NeonSaw", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 1 },
        { PID::osc1Level, 0.9f }, { PID::osc2Level, 0.7f },
        { PID::subLevel, 0.3f }, { PID::noiseLevel, 0.1f },
        { PID::filterCutoff, 4000.0f }, { PID::filterReso, 0.8f },
        { PID::ampAttack, 0.005f }, { PID::ampDecay, 0.15f },
        { PID::ampSustain, 0.7f }, { PID::ampRelease, 0.2f },
        { PID::crushMix, 0.2f }, { PID::distDrive, 0.6f }, { PID::distMix, 0.7f }
    });

    addPreset ("Lead SquareBite", {
        { PID::osc1Wave, 2 }, { PID::osc2Wave, 2 },
        { PID::osc1Level, 0.8f }, { PID::osc2Level, 0.6f },
        { PID::subLevel, 0.4f }, { PID::noiseLevel, 0.05f },
        { PID::filterCutoff, 2500.0f }, { PID::filterReso, 1.2f },
        { PID::crushBits, 6.0f }, { PID::crushDownsample, 6.0f },
        { PID::crushMix, 0.5f }, { PID::distDrive, 0.7f }, { PID::distMix, 0.8f }
    });

    addPreset ("Lead PWM Glow", {
        { PID::osc1Wave, 4 }, { PID::osc2Wave, 4 },
        { PID::osc1Level, 0.7f }, { PID::osc2Level, 0.7f },
        { PID::subLevel, 0.2f }, { PID::noiseLevel, 0.1f },
        { PID::filterCutoff, 3500.0f }, { PID::filterReso, 0.9f },
        { PID::chorusMix, 0.5f }, { PID::delayMix, 0.25f }, { PID::reverbMix, 0.3f }
    });

    addPreset ("Lead AcidRay", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 2 },
        { PID::osc1Level, 0.9f }, { PID::osc2Level, 0.5f },
        { PID::filterCutoff, 1800.0f }, { PID::filterReso, 2.0f },
        { PID::filtEnvAmount, 1.5f }, { PID::filtAttack, 0.01f },
        { PID::filtDecay, 0.25f }, { PID::filtRelease, 0.3f },
        { PID::distDrive, 0.8f }, { PID::distMix, 0.9f }
    });

    addPreset ("Lead GlassKeys", {
        { PID::osc1Wave, 3 }, { PID::osc2Wave, 0 },
        { PID::osc1Level, 0.6f }, { PID::osc2Level, 0.4f },
        { PID::filterCutoff, 5000.0f }, { PID::filterReso, 0.6f },
        { PID::ampAttack, 0.01f }, { PID::ampDecay, 0.2f },
        { PID::ampSustain, 0.5f }, { PID::ampRelease, 0.4f },
        { PID::reverbMix, 0.5f }
    });

    // 5 basses
    addPreset ("Bass SubPunch", {
        { PID::osc1Wave, 2 }, { PID::osc2Wave, 0 },
        { PID::osc1Level, 0.7f }, { PID::osc2Level, 0.3f },
        { PID::subLevel, 0.9f }, { PID::noiseLevel, 0.02f },
        { PID::filterCutoff, 400.0f }, { PID::filterReso, 0.7f },
        { PID::ampAttack, 0.005f }, { PID::ampDecay, 0.1f },
        { PID::ampSustain, 0.9f }, { PID::ampRelease, 0.2f },
        { PID::distDrive, 0.5f }, { PID::distMix, 0.6f }
    });

    addPreset ("Bass DirtSaw", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 1 },
        { PID::osc1Level, 0.8f }, { PID::osc2Level, 0.8f },
        { PID::subLevel, 0.6f }, { PID::noiseLevel, 0.1f },
        { PID::filterCutoff, 300.0f }, { PID::filterReso, 1.5f },
        { PID::crushBits, 5.0f }, { PID::crushDownsample, 8.0f },
        { PID::crushMix, 0.6f }, { PID::distDrive, 0.7f }, { PID::distMix, 0.8f }
    });

    addPreset ("Bass NotchWob", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 2 },
        { PID::osc1Level, 0.7f }, { PID::osc2Level, 0.5f },
        { PID::filterType, 4 }, { PID::filterCutoff, 600.0f },
        { PID::filterReso, 1.0f }, { PID::lfo1Rate, 1.0f },
        { PID::lfo1Amount, 0.6f }, { PID::distDrive, 0.6f }
    });

    addPreset ("Bass SquareThump", {
        { PID::osc1Wave, 2 }, { PID::osc2Wave, 2 },
        { PID::osc1Level, 0.9f }, { PID::osc2Level, 0.5f },
        { PID::subLevel, 0.8f }, { PID::filterCutoff, 250.0f },
        { PID::filterReso, 0.9f }, { PID::ampAttack, 0.01f },
        { PID::ampDecay, 0.15f }, { PID::ampRelease, 0.25f }
    });

    addPreset ("Bass PWMGrind", {
        { PID::osc1Wave, 4 }, { PID::osc2Wave, 4 },
        { PID::osc1Level, 0.8f }, { PID::osc2Level, 0.8f },
        { PID::subLevel, 0.5f }, { PID::filterCutoff, 350.0f },
        { PID::filterReso, 1.3f }, { PID::crushMix, 0.4f },
        { PID::distDrive, 0.8f }, { PID::distMix, 0.9f }
    });

    // 3 keys/plucks
    addPreset ("Pluck NeonBell", {
        { PID::osc1Wave, 3 }, { PID::osc2Wave, 0 },
        { PID::osc1Level, 0.6f }, { PID::osc2Level, 0.4f },
        { PID::ampAttack, 0.005f }, { PID::ampDecay, 0.25f },
        { PID::ampSustain, 0.2f }, { PID::ampRelease, 0.3f },
        { PID::filterCutoff, 4500.0f }, { PID::filterReso, 0.7f },
        { PID::reverbMix, 0.6f }
    });

    addPreset ("Pluck GlassDrop", {
        { PID::osc1Wave, 0 }, { PID::osc2Wave, 3 },
        { PID::osc1Level, 0.5f }, { PID::osc2Level, 0.5f },
        { PID::ampAttack, 0.002f }, { PID::ampDecay, 0.2f },
        { PID::ampSustain, 0.1f }, { PID::ampRelease, 0.25f },
        { PID::filterCutoff, 5000.0f }, { PID::filterReso, 0.8f },
        { PID::delayMix, 0.35f }
    });

    addPreset ("Pluck BitKeys", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 1 },
        { PID::osc1Level, 0.6f }, { PID::osc2Level, 0.6f },
        { PID::ampAttack, 0.003f }, { PID::ampDecay, 0.18f },
        { PID::ampSustain, 0.15f }, { PID::ampRelease, 0.3f },
        { PID::crushBits, 4.0f }, { PID::crushDownsample, 10.0f },
        { PID::crushMix, 0.7f }, { PID::reverbMix, 0.4f }
    });

    // 3 pads
    addPreset ("Pad NeonCloud", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 3 },
        { PID::osc1Level, 0.7f }, { PID::osc2Level, 0.7f },
        { PID::ampAttack, 0.5f }, { PID::ampDecay, 0.8f },
        { PID::ampSustain, 0.9f }, { PID::ampRelease, 1.5f },
        { PID::filterCutoff, 2500.0f }, { PID::filterReso, 0.7f },
        { PID::chorusMix, 0.6f }, { PID::reverbMix, 0.7f }
    });

    addPreset ("Pad GlassHalo", {
        { PID::osc1Wave, 0 }, { PID::osc2Wave, 3 },
        { PID::osc1Level, 0.6f }, { PID::osc2Level, 0.6f },
        { PID::ampAttack, 0.4f }, { PID::ampDecay, 0.7f },
        { PID::ampSustain, 0.8f }, { PID::ampRelease, 1.2f },
        { PID::filterCutoff, 3000.0f }, { PID::filterReso, 0.8f },
        { PID::reverbMix, 0.8f }
    });

    addPreset ("Pad BitCathedral", {
        { PID::osc1Wave, 1 }, { PID::osc2Wave, 1 },
        { PID::osc1Level, 0.7f }, { PID::osc2Level, 0.7f },
        { PID::ampAttack, 0.6f }, { PID::ampDecay, 0.9f },
        { PID::ampSustain, 0.9f }, { PID::ampRelease, 1.8f },
        { PID::filterCutoff, 2000.0f }, { PID::filterReso, 1.0f },
        { PID::crushBits, 7.0f }, { PID::crushDownsample, 5.0f },
        { PID::crushMix, 0.4f }, { PID::reverbMix, 0.9f }
    });
}

//==============================================================================
// Processor ctor/dtor
NeonMonoAudioProcessor::NeonMonoAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    synth.clearVoices();
    synth.addVoice (new NeonMonoVoice (*this, apvts));
    synth.clearSounds();
    synth.addSound (new NeonSound());

    initPresets();
}

NeonMonoAudioProcessor::~NeonMonoAudioProcessor() {}

//==============================================================================
// Programs
int NeonMonoAudioProcessor::getNumPrograms() { return (int) presets.size(); }
int NeonMonoAudioProcessor::getCurrentProgram() { return currentPreset; }

void NeonMonoAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= (int) presets.size())
        return;

    currentPreset = index;
    auto& p = presets[(size_t) index];

    for (auto& kv : p.values)
    {
        if (auto* param = apvts.getParameter (kv.first))
            param->setValueNotifyingHost (param->getNormalisableRange().convertTo0to1 (kv.second));
    }
}

const String NeonMonoAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= (int) presets.size())
        return {};
    return presets[(size_t) index].name;
}

//==============================================================================
// Prepare / process
bool NeonMonoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    return true;
}

void NeonMonoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    fx.prepare (sampleRate, samplesPerBlock);
}

void NeonMonoAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
    fx.process (buffer, apvts);
}

//==============================================================================
// State
void NeonMonoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void NeonMonoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
}

//==============================================================================
// Editor
AudioProcessorEditor* NeonMonoAudioProcessor::createEditor()
{
    return new NeonMonoEditor (*this);
}

//==============================================================================
// Factory
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeonMonoAudioProcessor();
}
