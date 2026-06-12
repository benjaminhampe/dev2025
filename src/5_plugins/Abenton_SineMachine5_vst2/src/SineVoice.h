#pragma once
#include <Config.h>
#include <common/Envelope.h>
#include <common/DspUtil.h>
#include <de/audio/OSC_Saw.h>

// 📊
struct OSC_Partials
{
    PartialsCfg m_cfg;
    int m_sampleRate = 44100;
    float m_baseFrequency = 220.f;
    float m_detuneCent = 0.0f;

    // int32_t m_frameIndex = 0;
    // int32_t m_frameCount = 0;
    int m_channel = 0;
    int m_midiNote = 0;
    float m_noteOnVelocity = 0.5f;
    float m_noteOffVelocity = 0.5f;
    float m_amplitudeSum;
    float m_amplitudeSumInv;


    de::TAlignedVector<float> m_amplitudes;
    //de::TAlignedVector<float> m_frequencies;
    de::TAlignedVector<float> m_phases;
    de::TAlignedVector<float> m_phaseIncrements;

    void init(const PartialsCfg& cfg, int sampleRate)
    {
        m_cfg = cfg;
        m_sampleRate = sampleRate;
        DSP_RESIZE(m_amplitudes, m_cfg.numPartials());
        DSP_RESIZE(m_phases, m_cfg.numPartials());
        DSP_RESIZE(m_phaseIncrements, m_cfg.numPartials());
    }

    // calcPhaseIncrements
    void noteOn(const PartialsCfg& cfg, float freq, int velocity)
    {
        m_cfg = cfg;
        m_baseFrequency = freq;

        DSP_RESIZE(m_amplitudes, m_cfg.numPartials());
        DSP_RESIZE(m_phases, m_cfg.numPartials());
        DSP_RESIZE(m_phaseIncrements, m_cfg.numPartials());

        //m_frequencies.resize(cfg.getNumPartials());

        const float fScale = de::TWO_PI * m_baseFrequency / float(m_sampleRate);

        float amplitudeSum = 0.0f;

        for (size_t i = 0; i < m_cfg.numPartials(); i++)
        {
            amplitudeSum += m_cfg.m_partials[i].fAmplitude;
            m_amplitudes[i] = m_cfg.m_partials[i].fAmplitude;
            //m_frequencies[i] = m_cfg.m_partials[i].fAmplitude;
            m_phases[i] = 0.0f;
            m_phaseIncrements[i] = fScale * (i+1);
        }

        m_amplitudeSum = amplitudeSum;
        if (amplitudeSum > 1e-12)
        {
            m_amplitudeSumInv = 1.0f / amplitudeSum;
        }
        else
        {
            m_amplitudeSumInv = 0.0f;
        }
    }

    void noteOff( int velocity )
    {

    }

    void nextSample(float & L, float & R)
    {
        float Asum = 0.0f;
        float sample = 0.0f;

        for (size_t k = 0; k < m_amplitudes.size(); k++)
        {
            const float A = m_amplitudes.at( k );
            float & phase = m_phases.at( k );
            float phaseInc = m_phaseIncrements.at( k );

            Asum += A;
            sample += A * sinf(phase);

            phase += phaseInc;
            if (phase > de::TWO_PI)
            {
                phase -= de::TWO_PI;
            }
        }

        sample /= float(Asum);
        L = sample;
        R = sample;
    }

};

// 📊
struct Voice
{
    SynthCfg* m_cfg = nullptr;
    Envelope m_envelope;
    OSC_Partials m_oscPartials;
    de::audio::OSC_Saw m_oscSaw;

    int m_channel = -1;
    int m_midiNote = -1;
    float m_detuneCent = 0.0f;
    float m_frequency = 220.f;

    de::TAlignedVector<float> m_L;
    de::TAlignedVector<float> m_R;

    void init(SynthCfg* cfg)
    {
        m_cfg = cfg;
        m_envelope.init(m_cfg->m_envelope);
        m_oscPartials.init(m_cfg->m_partials, cfg->m_sampleRate);

        DSP_RESIZE(m_L, 1024);
        DSP_RESIZE(m_R, 1024);
    }


    bool isPlaying() const
    {
        return m_midiNote > -1;
    }

    void allNotesOff()
    {
        m_envelope.resetIdle();
        m_midiNote = -1;
    }

    // calcPhaseIncrements
    void noteOn( int midiNote, int velocity, float detuneCent = 0.0f )
    {
        if (m_midiNote > -1)
        {
            DE_ERROR("Already playing midiNote(",m_midiNote,")")
            return;
        }

        m_midiNote = midiNote;
        m_envelope.init(m_cfg->m_envelope);

        m_frequency = de::calc_frequencyFromMidi(midiNote, detuneCent);

        m_oscPartials.noteOn(m_cfg->m_partials, m_frequency, velocity);
        m_oscSaw.noteOn(m_frequency, m_cfg->m_sampleRate, 100);
        const auto & m_cfgEnvelope = m_envelope.m_cfg;

        m_envelope.triggerNoteOn( float(velocity) / 127.0f );
    }

    void noteOff( int velocity )
    {
        m_envelope.triggerNoteOff( float(velocity) / 127.0f );
    }

    struct StereoSampleF32
    {
        float L;
        float R;
    };

    StereoSampleF32 nextSampleF32()
    {
        if (!m_envelope.isPlaying())
        {
            DE_WARN("Envelope ended.")
            m_midiNote = -1;
            return { 0.0f, 0.0f };
        }

        const float env = m_envelope.nextSample();

        float sample = m_oscSaw.computeSample();
        float L = sample;
        float R = sample;

        // L *= env;
        // R *= env;
        return { L, R };
    }

    void computeSamples(int32_t blockSize)
    {
        DSP_RESIZE(m_L, blockSize);
        DSP_RESIZE(m_R, blockSize);

        for (int i = 0; i < blockSize; ++i)
        {
            StereoSampleF32 sample = nextSampleF32();
            m_L[i] = sample.L;
            m_R[i] = sample.R;
        }
    }

};
