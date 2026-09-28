#include "CornerSynth.h"

void CornerSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void CornerSynth::reset()
{
    phase = 0.0f;
}

void CornerSynth::setFrequency(float newFrequency)
{
    frequency = newFrequency;
}

void CornerSynth::setAmplitude(float newAmplitude)
{
    amplitude = newAmplitude;
}

void CornerSynth::setPhaseOffset(float radians)
{
    phaseOffset = radians;
}

void CornerSynth::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
}

float CornerSynth::renderOscillator()
{
    const float twoPi =
        juce::MathConstants<float>::twoPi;

    float p = phase + phaseOffset;

    while (p >= twoPi)
        p -= twoPi;

    while (p < 0.0f)
        p += twoPi;

    switch (waveform)
    {
        case Waveform::Sine:
        {
            return std::sin(p);
        }

        case Waveform::Saw:
        {
            const float t = p / twoPi;
            return (2.0f * t) - 1.0f;
        }

        case Waveform::Square:
        {
            return p < juce::MathConstants<float>::pi
                ? 1.0f
                : -1.0f;
        }

        case Waveform::Triangle:
        {
            const float t = p / twoPi;

            if (t < 0.25f)
                return t * 4.0f;

            if (t < 0.75f)
                return 2.0f - (t * 4.0f);

            return (t * 4.0f) - 4.0f;
        }
    }

    return 0.0f;
}

float CornerSynth::process()
{
    if (sampleRate <= 0.0)
        return 0.0f;

    const float sample =
        renderOscillator();

    phase +=
        juce::MathConstants<float>::twoPi
        * frequency
        / static_cast<float>(sampleRate);

    while (phase >= juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;

    return sample * amplitude;
}