#include "Processor.h"
#include "Editor.h"

VectorPluginProcessor::VectorPluginProcessor()
    : AudioProcessor(
        BusesProperties()
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true))
    , apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    constexpr int numVoices = 16;

    visualStates.clear();
    visualStates.reserve( numVoices );

    synth.clearVoices();
    for (int i = 0; i < numVoices; ++i)
    {
        auto voice = new MorphVoice();
        voice->setVoiceIndex(i);
        synth.addVoice(voice);

        // For editor
        visualStates.emplace_back(&voice->getVisualState());
    }

    synth.clearSounds();
    synth.addSound(new MorphSound());

    initPresets();
}

juce::Synthesiser&
VectorPluginProcessor::getSynth() { return synth; }

void VectorPluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DE_BENNI("sampleRate(",sampleRate,"), "
             "blockSize(",samplesPerBlock,"), "
             "getTotalNumOutputChannels(",getTotalNumOutputChannels(),")")

    synth.setCurrentPlaybackSampleRate(sampleRate);

    DE_BENNI("synth.getNumVoices() = ",synth.getNumVoices())

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        auto voice = dynamic_cast<MorphVoice*>(synth.getVoice(i));
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
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

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
    presets.clear();

    auto addPreset = [this] (const String& name,
        std::initializer_list<std::pair<String, float>> vals)
    {
        Preset p;
        p.name = name;
        for (auto& v : vals) p.values[v.first] = v.second;
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

int VectorPluginProcessor::getNumPrograms() { return (int) presets.size(); }
int VectorPluginProcessor::getCurrentProgram() { return currentPreset; }
void VectorPluginProcessor::setCurrentProgram(int index)
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

const juce::String VectorPluginProcessor::getProgramName(int index)
{
    if (index < 0 || index >= (int) presets.size())
        return {};
    return presets[(size_t) index].name;
}

void VectorPluginProcessor::changeProgramName(int index, const juce::String& name)
{
    // juce::AudioProcessor::changeProgramName(index,name);
}


void VectorPluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VectorPluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
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
