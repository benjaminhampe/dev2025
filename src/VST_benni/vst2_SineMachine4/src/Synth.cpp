#include "Synth.h"
#include "Editor.h"

Synth::Synth()
    : m_sampleRate(44100.0f)
    , m_masterAmplitude(0.8f)
{
    m_partials.resize(NUM_PARTIALS);

    // 🌈 Rainbow coloring:
    float color_step = 0.7f / float(m_partials.size() - 1);

    for (size_t i = 0; i < m_partials.size(); i++)
    {
        m_partials[i].phase = 0.0f;

        auto color = de::RainbowColor::computeColor128(0.8f - color_step * i);
        m_partials[i].color.r = color.r;
        m_partials[i].color.g = color.g;
        m_partials[i].color.b = color.b;
        m_partials[i].color.a = 1.0f;
    }

    for (size_t i = 0; i < m_partials.size(); i++)
    {
        setPartial(i, 0.0f, 0);
    }
    setPartial(0, 1.0f, 0);

    calcPhaseIncrements(m_partials, m_baseFrequency, m_sampleRate);
}

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

void Synth::setPartial(int index, float amplitude, double cent )
{
    if (index < 0 || index >= NUM_PARTIALS)
    {
        return;
    }

    m_partials[index].partial = index + 1;
    m_partials[index].amplitude = amplitude;
    m_partials[index].detune = cent;
    m_partials[index].phase = 0.0f;
}

void Synth::setPartialsToRect()
{
    for (Partial & partial : m_partials)
    {
        partial.amplitude = de::calc_amplitude_rect<float>( partial.partial );
    }
}

void Synth::setPartialsToSaw()
{
    for (Partial & partial : m_partials)
    {
        partial.amplitude = de::calc_amplitude_saw<float>( partial.partial );
    }
}

void Synth::setPartialsToSawRev()
{
    for (Partial & partial : m_partials)
    {
        partial.amplitude = de::calc_amplitude_saw_rev<float>( partial.partial );
    }
}

void Synth::setPartialsToTriangle()
{
    for (Partial & partial : m_partials)
    {
        partial.amplitude = de::calc_amplitude_triangle<float>( partial.partial );
    }
}

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

void Synth::controlChange(int channel, int controller, int value)
{
    switch (controller) {
    case 1:  // Mod Wheel
        modDepth = value / 127.0f;
        break;
    case 7:  // Volume
        masterGain = value / 127.0f;
        break;
    case 74: // Filter cutoff (common mapping)
        filterCutoff = value / 127.0f;
        break;
    default:
        // Handle other CCs or ignore
        break;
    }
}

void Synth::pitchBend(int channel, int bendValue)
{
    // Convert to semitone offset: ±2 semitones range
    float bendSemis = (bendValue / 8192.0f) * 2.0f;

    // Apply bend to current note
    if (currentNote >= 0)
    {
        m_baseFrequency = 440.0f * pow(2.0, (currentNote - 69 + bendSemis) / 12.0);
        calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
    }
}

void Synth::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}


void Synth::setParameter(VstInt32 index, float value)
{
    if (index < 0 || index >= int(m_partials.size()))
    {
        return;
    }
    m_partials[index].amplitude = value;
}

float Synth::getParameter(VstInt32 index)
{
    if (index < 0 || index >= int(m_partials.size()))
    {
        return 0.0f;
    }
    return m_partials[index].amplitude;
}

void Synth::open()
{
    std::thread guiThread(
        [this] ()
        {
            m_editor->open();
            m_editor->run();
        }
    );
    guiThread.detach();
}

void Synth::close()
{
    m_editor->requestClose();
}

*/