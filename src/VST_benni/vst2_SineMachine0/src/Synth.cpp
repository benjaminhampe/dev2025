#include "Synth.h"
#include "Editor.h"

Synth::Synth(audioMasterCallback audioMaster)
    : AudioEffectX(audioMaster, 1, NUM_PARTIALS)
    , m_sampleRate(44100.0f)
    , m_masterAmplitude(0.8f)
{
    setNumInputs(0);
    setNumOutputs(2);
    setUniqueID('SMhb');
    canProcessReplacing();

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

    m_editor = new Editor(this);
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

void Synth::calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate )
{
    const float fScale = de::TWO_PI * baseFrequency / sampleRate;

    for (Partial & partial : partials)
    {
        partial.phaseIncrement = fScale * partial.partial;
    }
}

VstInt32 Synth::canDo(char* text)
{
    // 🧠 Example Implementation
    //     1 → Yes, I can do this
    //     0 → I’m not sure
    //     -1 → No, I can’t do this

    // 🎧 Audio & Processing
    //     "receiveVstEvents" — receive VST events (e.g. MIDI)
    //     "receiveVstMidiEvent" — receive MIDI events
    //     "sendVstEvents" — send VST events
    //     "sendVstMidiEvent" — send MIDI events
    //     "receiveVstTimeInfo" — receive time info (tempo, position, etc.)
    //     "offline" — supports offline processing
    //     "plugAsChannelInsert" — can be used as a channel insert effect
    //     "plugAsSend" — can be used as a send effect
    //     "bypass" — supports bypass processing

    // 🎹 Instrument & Synth
    //     "isSynth" — plugin is a synth (no audio input, generates sound)
    //     "noRealTime" — plugin doesn’t require real-time processing

    // 🖥️ GUI & Automation
    //     "hasEditor" — plugin has a custom GUI editor
    //     "canMono" — supports mono processing
    //     "canStereo" — supports stereo processing
    //     "midiProgramNames" — supports MIDI program name queries
    //     "conformsToWindowRules" — GUI follows host windowing rules

    // 🧪 Host-Specific or Rare
    //     "supportsBypass" — supports host bypass
    //     "supportsOpen" — supports opening plugin window
    //     "supportsClose" — supports closing plugin window
    //     "supportsRewire" — supports ReWire
    //     "supportsMultiChannel" — supports more than stereo
    //     "supportsShell" — part of a shell plugin (multiple plugins in one binary)

    if (!strcmp(text, "receiveVstEvents")) return 1;
    if (!strcmp(text, "receiveVstMidiEvent")) return 1;
    if (!strcmp(text, "hasEditor")) return 1;
    if (!strcmp(text, "isSynth")) return 1;
    return 0;
}

VstInt32 Synth::processEvents(VstEvents* events)
{
    for (VstInt32 i = 0; i < events->numEvents; ++i)
    {
        if (events->events[i]->type == kVstMidiType)
        {
            VstMidiEvent* midi = (VstMidiEvent*)events->events[i];
            handleMidi(reinterpret_cast<unsigned char*>(midi->midiData));
        }
    }
    return 1;
}

void Synth::handleMidi(unsigned char* data)
{
    int status = data[0] & 0xF0;
    int channel = data[0] & 0x0F;
    int data1 = data[1] & 0x7F;
    int data2 = data[2] & 0x7F;

    switch (status) {
    case 0x90:  // Note On
        if (data2 > 0)
            noteOn(channel, data1, data2);
        else
            noteOff(channel, data1, data2);  // velocity 0 = note off
        break;
    case 0x80:  // Note Off
        noteOff(channel, data1, data2);
        break;
    case 0xB0:  // Control Change
        controlChange(channel, data1, data2);
        break;
    case 0xE0:  // Pitch Bend
        pitchBend(channel, ((data2 << 7) | data1) - 8192);
        break;
        // Add more cases as needed
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
void Synth::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Synth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; ++i)
    {
        float sample = 0.0f;
        float Asum = 0.0f;

        for (int p = 0; p < m_partials.size(); ++p)
        {
            float A = m_partials[p].A();
            //float omega = TWO_PI * m_baseFrequency * m_partials[p].partial;
            Asum += A;

            sample += A * sinf(m_partials[p].phase);

            m_partials[p].phase += m_partials[p].phaseIncrement;

            if (m_partials[p].phase > de::TWO_PI)
            {
                m_partials[p].phase -= de::TWO_PI;
            }
        }

        sample /= float(Asum);

        outL[i] = outR[i] = sample;
    }
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
