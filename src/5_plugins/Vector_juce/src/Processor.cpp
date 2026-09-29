#include "Processor.h"
#include "Editor.h"

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
VectorVoice::VectorVoice(VectorSynthesiser& synth,
                         juce::AudioProcessorValueTreeState& apvts)
//=======================================================
    : m_apvts(apvts)
    , m_synth(synth)
{
    m_sampleRate = 48000.0f;
    m_adsrParameters.sampleRate = 48000.0f;
    m_adsrParameters.attackTimeInSec = 0.01f;
    m_adsrParameters.decayTimeInSec = 0.25f;
    m_adsrParameters.sustainLevel = 0.80f;
    m_adsrParameters.releaseTimeInSec = 0.50f;
    m_adsr.init(m_adsrParameters);

    m_adsr.onProgress =
        [&] (de::vec::Env::ePhase phase, float t)
        {
            m_adsrStage = phase;
            m_adsrStageProgress = t;
            updateCursorPosition();
            updateOrbiterPosition();
        };

    m_topLeft.setWaveform(VectorOsc::Waveform::Saw);
    m_topRight.setWaveform(VectorOsc::Waveform::Square);
    m_bottomLeft.setWaveform(VectorOsc::Waveform::Triangle);
    m_bottomRight.setWaveform(VectorOsc::Waveform::Sine);

    m_topLeft.setAmplitude(0.5f);
    m_topRight.setAmplitude(0.5f);
    m_bottomLeft.setAmplitude(1.0f);
    m_bottomRight.setAmplitude(1.0f);

    m_orbPhaseIncrement = 2.0 * 3.141592 * m_synth.m_orbSpeed / m_sampleRate;
}

VectorVoice::~VectorVoice()
{

}

void VectorVoice::reset()
{
    m_topLeft.reset();
    m_topRight.reset();
    m_bottomLeft.reset();
    m_bottomRight.reset();

    m_bPlaying = false;
    m_bNoteReleased = false;
    m_midiNote = -1;

    m_adsrStage = de::vec::Env::ePhase::Idle;
    m_adsrStageProgress = 0.0f;

    m_orbPhase = 0.0f;

    // m_morphX = 0.5f;
    // m_morphY = 0.5f;

    m_cursorPos = { 0.0f, 0.0f };

    m_noteOnTimeSeconds = 0.0;
    m_noteOffTimeSeconds = 0.0;
}

float VectorVoice::interpolateSample(float morphX, float morphY)
{
    morphX = juce::jlimit(0.0f, 1.0f, morphX);
    morphY = juce::jlimit(0.0f, 1.0f, morphY);

    const float tl = m_topLeft.nextSample();
    const float tr = m_topRight.nextSample();
    const float bl = m_bottomLeft.nextSample();
    const float br = m_bottomRight.nextSample();

    const float wTL = (1.0f - morphX) * (1.0f - morphY);
    const float wTR = morphX * (1.0f - morphY);
    const float wBL = (1.0f - morphX) * morphY;
    const float wBR = morphX * morphY;

    return tl * wTL +
        tr * wTR +
        bl * wBL +
        br * wBR;
}

void VectorVoice::updateCursorPosition()
{
    switch (m_adsrStage)
    {
        case de::vec::Env::Attack:
        {
            const auto& A = m_synth.m_attackPoint;
            const auto& D = m_synth.m_decayPoint;
            m_cursorPos = A + (D - A) * m_adsrStageProgress;
            break;
        }
        case de::vec::Env::Decay:
        {
            const auto& D = m_synth.m_decayPoint;
            const auto& S = m_synth.m_sustainPoint;
            m_cursorPos = D + (S - D) * m_adsrStageProgress;
            break;
        }
        case de::vec::Env::Sustain:
        {
            m_cursorPos = m_synth.m_sustainPoint;
            break;
        }
        case de::vec::Env::Release:
        {
            const auto& S = m_synth.m_sustainPoint;
            const auto& R = m_synth.m_releasePoint;
            m_cursorPos = S + (R - S) * m_adsrStageProgress;
            break;
        }
        default:
        {
            m_cursorPos = m_synth.m_releasePoint;
            break;
        }
    }
}

void VectorVoice::updateOrbiterPosition()
{
    float cur_x = de::clampf(m_cursorPos.x, 0.0f, 1.0f);
    float cur_y = de::clampf(m_cursorPos.y, 0.0f, 1.0f);
    float orb_x = std::cos(m_orbPhase) * m_synth.m_orbRadius;
    float orb_y = std::sin(m_orbPhase) * m_synth.m_orbRadius;

    m_orbiterPos.x = de::clampf(cur_x + orb_x, 0.0f, 1.0f);
    m_orbiterPos.y = de::clampf(cur_y + orb_y, 0.0f, 1.0f);
}

bool VectorVoice::isVoiceActive() const
{
    return m_adsr.isPlaying();
}

bool VectorVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return true; // dynamic_cast<MorphSound*>(sound) != nullptr;
}

void VectorVoice::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    DE_DEBUG("sr(",sampleRate,"), "
    "fc(",samplesPerBlock,"), "
    "ch(",numChannels,")")

    m_sampleRate = sampleRate;

    m_adsrParameters.sampleRate = sampleRate;
    m_adsr.init(m_adsrParameters);

    m_topLeft.prepare(sampleRate);
    m_topRight.prepare(sampleRate);
    m_bottomLeft.prepare(sampleRate);
    m_bottomRight.prepare(sampleRate);

}

void VectorVoice::startNote(int midiNote,float velocity,
    juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    m_frequency = (float)juce::MidiMessage::getMidiNoteInHertz(midiNote);

    m_topLeft.setFrequency(m_frequency);
    m_topRight.setFrequency(m_frequency);
    m_bottomLeft.setFrequency(m_frequency);
    m_bottomRight.setFrequency(m_frequency);

    m_adsr.noteOn(velocity);
    reset();
    m_bPlaying = m_adsr.isPlaying();
    m_bNoteReleased = false;
    midiNote = midiNote;
    // visualState.morphX = morphX;
    // visualState.morphY = morphY;

    // int osc1Wave = (int) apvts.getRawParameterValue(PID::osc1Wave)->load();
    // int osc2Wave = (int) apvts.getRawParameterValue(PID::osc2Wave)->load();
    // float osc1Level = apvts.getRawParameterValue (PID::osc1Level)->load();
    // float osc2Level = apvts.getRawParameterValue (PID::osc2Level)->load();
    // float subLevel  = apvts.getRawParameterValue (PID::subLevel)->load();
}

void VectorVoice::stopNote(float velocity, bool allowTailOff)
{
    DE_DEBUG("Voice[",getVoiceIndex(),"].noteOff(",m_frequency," Hz)")
    m_adsr.noteOff(velocity);
    m_bPlaying = m_adsr.isPlaying();
    m_bNoteReleased = true;

    if (!allowTailOff)
    {
        clearCurrentNote();
        reset();
    }
}

void VectorVoice::renderNextBlock(
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

    for (int f = 0; f < numSamples; ++f)
    {
        const float env = m_adsr.getNextSample();

        float sample = interpolateSample(m_orbiterPos.x,
                                         m_orbiterPos.y);

        sample *= env;

        L[startSample + f] += sample;
        R[startSample + f] += sample;

        m_orbPhase += m_orbPhaseIncrement;

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
        if (!m_bPlaying)
        {
            clearCurrentNote();
            break;
        }
    }
}


void VectorVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void VectorVoice::controllerMoved(int ccNumber, int newCCValue)
{
}

void VectorVoice::setVoiceIndex(int index)
{
    m_voiceIndex = index;
}

int VectorVoice::getVoiceIndex() const
{
    return m_voiceIndex;
}

/*
void VectorVoice::setMorphPosition(float x,float y)
{
    m_morphX = juce::jlimit(0.0f,1.0f,x);
    m_morphY = juce::jlimit(0.0f,1.0f,y);
}

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

//=======================================================
VectorSynthesiser::VectorSynthesiser(
    juce::AudioProcessorValueTreeState& apvts)
//=======================================================
    : juce::Synthesiser()
    , m_apvts(apvts)
    , m_attackPoint { 0.2f, 0.2f }
    , m_decayPoint  { 0.8f, 0.3f }
    , m_sustainPoint{ 0.7f, 0.8f }
    , m_releasePoint{ 0.2f, 0.9f }
{
    m_orbRadius = m_apvts.getRawParameterValue(PID::orbitRadius)->load();
    m_orbSpeed = m_apvts.getRawParameterValue(PID::orbitSpeed)->load();
}

void VectorSynthesiser::setOrbiterSpeed(float speed_in_Hz)
{
    m_orbSpeed = speed_in_Hz;
    for (auto& voice : m_voices)
    {
        voice->m_orbPhaseIncrement = 2.0 * 3.141592 * m_orbSpeed / voice->m_sampleRate;
    }
}

//=======================================================
VectorPluginProcessor::VectorPluginProcessor()
//=======================================================
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(),true))
    , m_apvts(*this, nullptr, "PARAMS", createParameterLayout())
    , m_synth(m_apvts)
{
    constexpr int numVoices = 16;

    m_synth.m_voices.clear();
    m_synth.m_voices.reserve( numVoices );

    m_synth.clearVoices();
    for (int i = 0; i < numVoices; ++i)
    {
        auto voice = new VectorVoice(m_synth, m_apvts);
        voice->setVoiceIndex(i);
        m_synth.addVoice(voice);
        m_synth.m_voices.emplace_back(voice); // For editor
    }

    m_synth.clearSounds();
    m_synth.addSound(new MorphSound());

    initPresets();
}

void VectorPluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DE_BENNI("sampleRate(",sampleRate,"), "
             "blockSize(",samplesPerBlock,"), "
             "getTotalNumOutputChannels(",getTotalNumOutputChannels(),")")

    m_synth.setCurrentPlaybackSampleRate(sampleRate);

    DE_BENNI("synth.getNumVoices() = ",m_synth.getNumVoices())

    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        auto voice = dynamic_cast<VectorVoice*>(m_synth.getVoice(i));
        if (voice)
        {
            voice->prepare(sampleRate,samplesPerBlock,
                        getTotalNumOutputChannels());
        }
    }
}

void VectorPluginProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    // if (!midi.isEmpty())
    // {
    //     DE_DEBUG("midi")
    // }

    // DE_DEBUG("blockSize(",buffer.getNumSamples(),"), "
    //          "channel(",buffer.getNumChannels(),"), ")
    m_synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

/*
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        auto voice = dynamic_cast<MorphVoice*>(synth.getVoice(i));
        if (voice == nullptr)
            continue;

        visualStateManager.updateVoiceState(i, voice->getVisualState());
    }
*/
}

void VectorPluginProcessor::releaseResources()
{
    // juce::AudioProcessor::releaseResources();
}

bool VectorPluginProcessor::hasEditor() const
{
    return true;
}

const juce::String VectorPluginProcessor::getName() const
{
    return "VectorSynth";
}

bool VectorPluginProcessor::acceptsMidi() const { return true;}
bool VectorPluginProcessor::producesMidi() const { return false;}
bool VectorPluginProcessor::isMidiEffect() const { return false;}
double VectorPluginProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
// Parameter layout
AudioProcessorValueTreeState::ParameterLayout
VectorPluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    using FloatParam = AudioParameterFloat;
    using ChoiceParam = AudioParameterChoice;
    using BoolParam = AudioParameterBool;

    // Voice
    params.push_back(std::make_unique<FloatParam>(PID::orbitRadius, "Orbiter Radius",
        NormalisableRange<float> (0.0f, 1.0f), 0.1f));
    params.push_back(std::make_unique<FloatParam>(PID::orbitSpeed, "Orbiter Speed Hz",
        NormalisableRange<float> (0.001f, 1000.0f), .5f));

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
void VectorPluginProcessor::initPresets()
{
    m_presets.clear();

    auto addPreset = [this] (const String& name,
        std::initializer_list<std::pair<String, float>> vals)
    {
        Preset p;
        p.name = name;
        for (auto& v : vals) p.values[v.first] = v.second;
        m_presets.push_back (std::move (p));
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

    // 3 keys/plucks
    addPreset ("Pluck NeonBell", {
        { PID::osc1Wave, 3 }, { PID::osc2Wave, 0 },
        { PID::osc1Level, 0.6f }, { PID::osc2Level, 0.4f },
        { PID::ampAttack, 0.005f }, { PID::ampDecay, 0.25f },
        { PID::ampSustain, 0.2f }, { PID::ampRelease, 0.3f },
        { PID::filterCutoff, 4500.0f }, { PID::filterReso, 0.7f },
        { PID::reverbMix, 0.6f }
    });

}

int VectorPluginProcessor::getNumPrograms() { return (int) m_presets.size(); }
int VectorPluginProcessor::getCurrentProgram() { return m_preset; }
void VectorPluginProcessor::setCurrentProgram(int index)
{
    if (index < 0 || index >= (int)m_presets.size())
        return;

    m_preset = index;
    auto& p = m_presets[(size_t) index];

    for (auto& kv : p.values)
    {
        if (auto* param = m_apvts.getParameter(kv.first))
            param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1 (kv.second));
    }
}

const juce::String VectorPluginProcessor::getProgramName(int index)
{
    if (index < 0 || index >= (int)m_presets.size())
        return {};
    return m_presets[(size_t)index].name;
}

void VectorPluginProcessor::changeProgramName(int index, const juce::String& name)
{
    // juce::AudioProcessor::changeProgramName(index,name);
}


void VectorPluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = m_apvts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VectorPluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
        if (xml->hasTagName (m_apvts.state.getType()))
            m_apvts.replaceState(ValueTree::fromXml (*xml));
}

//==============================================================================
// Editor
juce::AudioProcessorEditor* VectorPluginProcessor::createEditor()
{
    return new VectorPluginEditor(*this);
}

//==============================================================================
// Factory
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VectorPluginProcessor();
}
