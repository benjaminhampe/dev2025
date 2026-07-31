#include "PluckStringProcessor.h"
#include "PluckStringEditor.h"

PluckStringProcessor::PluckStringProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new WavetableVoice());

    synth.clearSounds();
    synth.addSound (new WavetableSound());

    wavetable.setSize (1, 2048);
    initString();
}

juce::AudioProcessorValueTreeState::ParameterLayout PluckStringProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("c", "WaveSpeed",
        juce::NormalisableRange<float> (0.1f, 10.0f), 1.0f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("d", "Damping",
        juce::NormalisableRange<float> (0.0f, 0.05f), 0.002f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("pluck", "PluckPos",
        juce::NormalisableRange<float> (0.05f, 0.95f), 0.30f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("timespeed", "TimeSpeed",
        juce::NormalisableRange<float> (0.1f, 4.0f), 1.0f));

    p.push_back (std::make_unique<juce::AudioParameterBool> ("limBypass", "LimiterBypass", false));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("limit", "Limit",
        juce::NormalisableRange<float> (0.2f, 5.0f), 1.0f));

    p.push_back (std::make_unique<juce::AudioParameterBool> ("lpBypass", "LPBypass", false));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("lpCut", "LPCut",
        juce::NormalisableRange<float> (1.0f, 500.0f), 50.0f));

    p.push_back (std::make_unique<juce::AudioParameterBool> ("adsrBypass", "ADSRBypass", false));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("atk", "Attack",
        juce::NormalisableRange<float> (0.001f, 1.0f), 0.01f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("dec", "Decay",
        juce::NormalisableRange<float> (0.001f, 2.0f), 0.10f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("sus", "Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f), 0.70f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("rel", "Release",
        juce::NormalisableRange<float> (0.001f, 3.0f), 0.30f));

    return { p.begin(), p.end() };
}

void PluckStringProcessor::initString()
{
    uPrev.resize (N);
    u.resize (N);
    uNext.resize (N);
    y_lp.resize (N);

    float pluckPos = apvts.getRawParameterValue ("pluck")->load();
    for (int i = 0; i < N; ++i)
    {
        float x = (float) i / (float) (N - 1);
        auto& ru = u[i];
        if (x < pluckPos)
            u[i] = x / pluckPos;
        else
            u[i] = (1.0f - x) / (1.0f - pluckPos);
    }
    uPrev = u;
    y_lp = u;

    for (int i = 0; i < N; ++i)
        uPrev.set (i, u[i] - 0.0005f);
}

void PluckStringProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    dt = 1.0 / sampleRate; // simple: one step per sample
    dx = 1.0 / (double) (N - 1);

    rebuildWavetable();
}

bool PluckStringProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PluckStringProcessor::stepString (int iterations, double sampleRate)
{
    auto c   = apvts.getRawParameterValue ("c")->load();
    auto d   = apvts.getRawParameterValue ("d")->load();
    auto ts  = apvts.getRawParameterValue ("timespeed")->load();
    auto L   = apvts.getRawParameterValue ("limit")->load();
    auto fc  = apvts.getRawParameterValue ("lpCut")->load();
    auto limBypass = apvts.getRawParameterValue ("limBypass")->load() > 0.5f;
    auto lpBypass  = apvts.getRawParameterValue ("lpBypass")->load() > 0.5f;

    double c2 = (double) c * (double) c;
    double alpha = 1.0 - std::exp (-2.0 * juce::MathConstants<double>::pi * (double) fc * dt);

    for (int s = 0; s < iterations; ++s)
    {
        for (int i = 1; i < N - 1; ++i)
        {
            double lap = (u[i - 1] - 2.0f * u[i] + u[i + 1]) / (dx * dx);
            uNext[i] = (float) ((2.0 - d) * u[i] - (1.0 - d) * uPrev[i] + c2 * dt * dt * lap);
        }
        uNext[0]     = 0.0f;
        uNext[N - 1] = 0.0f;

        if (! limBypass)
        {
            for (int i = 0; i < N; ++i)
            {
                if (uNext[i] > L)  uNext[i] = L;
                if (uNext[i] < -L) uNext[i] = -L;
            }
        }

        uPrev = u;
        u     = uNext;

        if (! lpBypass)
        {
            for (int i = 0; i < N; ++i)
                y_lp[i] = (float) (y_lp[i] + alpha * (u[i] - y_lp[i]));
        }
        else
        {
            y_lp = u;
        }
    }
}

void PluckStringProcessor::rebuildWavetable()
{
    auto* wtData = wavetable.getWritePointer (0);
    auto M = wavetable.getNumSamples();

    for (int n = 0; n < M; ++n)
    {
        double x = (double) n / (double) (M - 1);
        int idx = (int) std::floor (x * (double) (N - 1));
        idx = juce::jlimit (0, N - 1, idx);
        wtData[n] = y_lp[idx];
    }

    double sum = 0.0;
    for (int i = 0; i < M; ++i)
        sum += wtData[i];
    float dc = (float) (sum / (double) M);
    for (int i = 0; i < M; ++i)
        wtData[i] -= dc;

    float peak = 0.0f;
    for (int i = 0; i < M; ++i)
        peak = std::max (peak, std::abs (wtData[i]));
    if (peak > 0.0f)
        for (int i = 0; i < M; ++i)
            wtData[i] /= peak;

    for (int v = 0; v < synth.getNumVoices(); ++v)
        if (auto* voice = dynamic_cast<WavetableVoice*> (synth.getVoice (v)))
            voice->setWavetable (&wavetable);

    auto atk = apvts.getRawParameterValue ("atk")->load();
    auto dec = apvts.getRawParameterValue ("dec")->load();
    auto sus = apvts.getRawParameterValue ("sus")->load();
    auto rel = apvts.getRawParameterValue ("rel")->load();
    auto adsrBypass = apvts.getRawParameterValue ("adsrBypass")->load() > 0.5f;

    for (int v = 0; v < synth.getNumVoices(); ++v)
        if (auto* voice = dynamic_cast<WavetableVoice*> (synth.getVoice (v)))
        {
            if (adsrBypass)
                voice->setADSR (0.001f, 0.001f, 1.0f, 0.001f);
            else
                voice->setADSR (atk, dec, sus, rel);
        }
}

void PluckStringProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    auto sampleRate = getSampleRate();
    auto numSamples = buffer.getNumSamples();

    int iterations = juce::jmax (1, numSamples);
    stepString (iterations, sampleRate);
    rebuildWavetable();

    juce::MidiBuffer mergedMidi;
    mergedMidi.addEvents (midi, 0, numSamples, 0);
    mergedMidi.addEvents (editorMidi, 0, numSamples, 0);
    editorMidi.clear();

    synth.renderNextBlock (buffer, mergedMidi, 0, numSamples);
}

void PluckStringProcessor::triggerNoteFromEditor (int midiNote)
{
    editorMidi.addEvent (juce::MidiMessage::noteOn (1, midiNote, (juce::uint8) 100), 0);
}

juce::AudioProcessorEditor* PluckStringProcessor::createEditor()
{
    return new StringWaveAudioProcessorEditor (*this);
}

void PluckStringProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    juce::MemoryOutputStream mos (destData, false);
    state.writeToStream (mos);
}

void PluckStringProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState (tree);
}
