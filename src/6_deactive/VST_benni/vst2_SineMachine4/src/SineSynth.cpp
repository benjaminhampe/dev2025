#include "SineSynth.h"

// ------------------ Synth Implementation ------------------

/*
void Synth::calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate )
{
    const float fScale = de::TWO_PI * baseFrequency / sampleRate;

    for (Partial & partial : partials)
    {
        partial.phaseIncrement = fScale * partial.partial;
    }
}
void Synth::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}
*/


/*

void Synth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; i++)
    {
        float sample = 0.0f;
        float Asum = 0.0f;

        for (Partial & partial : m_partials)
        {
            float A = partial.A();
            sample += A * sinf(partial.phase);
            partial.phase += partial.phaseIncrement;
            if (partial.phase > de::TWO_PI)
            {
                partial.phase -= de::TWO_PI;
            }
            Asum += A;
        }

        sample /= float(Asum);

        outL[i] = outR[i] = sample;
    }
}

void Synth::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Synth::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote)
    {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}


*/

// ------------------ Plugin Implementation ------------------



// constexpr int NUM_PARTIALS = 48;
