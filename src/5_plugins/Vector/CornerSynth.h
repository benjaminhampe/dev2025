#pragma once

#include <JuceHeader.h>

class CornerSynth
{
public:

    enum class Waveform
    {
        Sine = 0,
        Saw,
        Square,
        Triangle
    };

    CornerSynth() = default;

    void prepare(double newSampleRate);

    void reset();

    void setFrequency(float newFrequency);

    void setAmplitude(float newAmplitude);

    void setPhaseOffset(float radians);

    void setWaveform(Waveform newWaveform);

    float process();

private:

    float renderOscillator() const;

private:

    double sampleRate = 44100.0;

    float frequency = 440.0f;

    float amplitude = 0.25f;

    float phase = 0.0f;

    float phaseOffset = 0.0f;

    Waveform waveform =
        Waveform::Sine;
};