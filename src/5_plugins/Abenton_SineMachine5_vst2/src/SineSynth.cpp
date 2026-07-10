#include "SineSynth.h"

const SynthCfg&
Synth::getConfig() const { return m_cfg; }

SynthCfg&
Synth::getConfig() { return m_cfg; }

// void
// Synth::setBypass(bool bBypassed)
// {
//     m_bBypass = bBypassed;
//     DE_BENNI("m_bBypass(",m_bBypass,")")
// }

void
Synth::init()
{
    // m_bBypass = false;

    m_cfg.init();

    m_voices.resize(m_cfg.m_maxVoices); // polyphony

    DE_OK("Created ",m_voices.size()," voices.")
}

void
Synth::setSampleRate(int sampleRate)
{
    if (sampleRate == m_cfg.m_sampleRate)
    {
        return;
    }
    m_cfg.m_sampleRate = sampleRate;
    m_cfg.m_envelope.SampleRate = sampleRate;
    for (auto & voice : m_voices)
    {
        voice.init(&m_cfg);
    }
    DE_BENNI("setSampleRate(",sampleRate,")")
}

void
Synth::setBlockSize(int blockSize)
{
    if (blockSize == m_cfg.m_blockSize)
    {
        return;
    }
    m_cfg.m_blockSize = blockSize;
    m_L.resize(blockSize);
    m_R.resize(blockSize);
    DE_BENNI("setBlockSize(",blockSize,")")
}

void
Synth::process(int blockSize)
{
    setBlockSize(blockSize);

    size_t nNotes = 0;

    //DE_OK("m_synth.m_notes.size() = ", m_synth.m_notes.size(), ", "
    //        "sampleFrames = ", sampleFrames)

    const uint32_t nChannelBytes = static_cast<uint32_t>(blockSize) * sizeof(float);

    float* __restrict__ const Lout = m_L.data();
    float* __restrict__ const Rout = m_R.data();

    // Clear L+R:
    std::memset(Lout,0,nChannelBytes);
    std::memset(Rout,0,nChannelBytes);

    // Fill L+R:
    for (Voice & voice : m_voices)
    {
        if (voice.isPlaying())
        {
            // Produce L+R:
            voice.computeSamples(blockSize);

            // Add L:
            const float* __restrict__ const Lin = voice.m_L.data();
            DE_ASSUME_NO_OVERLAP(Lin,Lout,nChannelBytes);
            for (long i = 0; i < blockSize; ++i) { Lout[i] += Lin[i]; }

            // Add R:
            const float* __restrict__ const Rin = voice.m_R.data();
            DE_ASSUME_NO_OVERLAP(Rin,Rout,nChannelBytes);
            for (long i = 0; i < blockSize; ++i) { Rout[i] += Rin[i]; }
        }
    }
}

int
Synth::findIdleVoice() const
{
    for (size_t i = 0; i < m_voices.size(); i++)
    {
        if (!m_voices[i].isPlaying())
        {
            return i;
        }
    }
    return -1;
}

void
Synth::noteOn(int channel, int midiNote, int velocity)
{
    int voice = findIdleVoice();
    if (voice < 0)
    {
        return; // Discard, Information loss!
    }

    //DE_OK("NoteOn: ", midiNote, ", IdleVoice = ",voice)
    m_voices[voice].noteOn(midiNote, velocity);
}

void
Synth::noteOff(int channel, int midiNote, int velocity)
{
    int nVoices = 0;

    for (size_t i = 0; i < m_voices.size(); i++)
    {
        Voice & voice = m_voices[i];
        if (voice.m_midiNote == midiNote)
        {
            m_voices[i].noteOff(velocity);
            nVoices++;

            // if (!voice.m_cfg->m_envelope.bSingleShot)
            // {
            //     m_voices[i].noteOff(velocity);
            //     nVoices++;
            // }
        }
    }

    //DE_OK("NoteOff: ",midiNote, " for nVoices = ",nVoices)
}

void
Synth::allNotesOff()
{
    DE_OK("allNotesOff()")
    for (size_t i = 0; i < m_voices.size(); i++)
    {
        Voice & voice = m_voices[i];
        voice.allNotesOff();
    }

}

void
Synth::controlChange(int channel, int controller, int value)
{
    switch (controller)
    {
    // case 1:  // Mod Wheel
    //     modDepth = value / 127.0f;
    //     break;
    // case 7:  // Volume
    //     masterGain = value / 127.0f;
    //     break;
    // case 74: // Filter cutoff (common mapping)
    //     filterCutoff = value / 127.0f;
    //     break;
    case 123: // de::midi::CC_123_AllNotesOff
        allNotesOff();
        break;
    default:
        // Handle other CCs or ignore
        break;
    }
}

void
Synth::pitchBend(int channel, int bendValue)
{
    /*
    // Convert to semitone offset: ±2 semitones range
    float bendSemis = (bendValue / 8192.0f) * 2.0f;

    // Apply bend to current note
    if (currentNote >= 0)
    {
        m_baseFrequency = 440.0f * pow(2.0, (currentNote - 69 + bendSemis) / 12.0);
        calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
    }
    */
}

// void setPartial(int index, float amplitude, double centDetune = 0.0);
// void setPartialsToRect();
// void setPartialsToSaw();
// void setPartialsToSawRev();
// void setPartialsToTriangle();

// static void calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate );


/*


void
Plugin::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; i++)
    {
        float sample = 0.0f;
        float Asum = 0.0f;

        for (Partial & partial : m_synth.m_partials)
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

void Plugin::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}
void Plugin::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Plugin::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Plugin::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote)
    {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}

void Plugin::controlChange(int channel, int controller, int value)
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

void Plugin::pitchBend(int channel, int bendValue)
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

// ------------------ Synth Implementation ------------------

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
