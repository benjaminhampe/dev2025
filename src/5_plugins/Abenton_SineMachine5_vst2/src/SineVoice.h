#pragma once
#include <Config.h>
#include <common/Envelope.h>
#include <common/DspUtil.h>

// 📊
struct Voice
{
    SynthCfg* m_cfg = nullptr;
    PartialsCfg m_cfgPartials;
    Envelope m_envelope;

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

    de::TAlignedVector<float> m_L;
    de::TAlignedVector<float> m_R;

    void init(SynthCfg* cfg)
    {
        m_cfg = cfg;
        m_envelope.init(m_cfg->m_envelope);
        m_cfgPartials = m_cfg->m_partials;

        DSP_RESIZE(m_amplitudes, cfg->m_partials.numPartials());

        DSP_RESIZE(m_phases, cfg->m_partials.numPartials());

        DSP_RESIZE(m_phaseIncrements, cfg->m_partials.numPartials());

        DSP_RESIZE(m_L, 1024);
        DSP_RESIZE(m_R, 1024);
    }


    bool isPlaying() const
    {
        return m_envelope.isPlaying();
    }

    // calcPhaseIncrements
    void noteOn( int midiNote, int velocity, float detuneCent = 0.0f )
    {
        m_envelope.init(m_cfg->m_envelope);
        m_cfgPartials = m_cfg->m_partials;
        const auto & m_cfgEnvelope = m_envelope.m_cfg;

        m_envelope.triggerNoteOn( float(velocity) / 127.0f );

        DSP_RESIZE(m_amplitudes, m_cfg->m_partials.numPartials());

        DSP_RESIZE(m_phases, m_cfg->m_partials.numPartials());

        DSP_RESIZE(m_phaseIncrements, m_cfg->m_partials.numPartials());

        //m_frequencies.resize(cfg.getNumPartials());

        m_baseFrequency = de::calc_frequencyFromMidi(midiNote, detuneCent);

        const float fScale = de::TWO_PI * m_baseFrequency / float(m_cfg->m_sampleRate);

        float amplitudeSum = 0.0f;

        for (size_t i = 0; i < m_cfgPartials.numPartials(); i++)
        {
            amplitudeSum += m_cfgPartials.m_partials[i].fAmplitude;
            m_amplitudes[i] = m_cfgPartials.m_partials[i].fAmplitude;
            //m_frequencies[i] = m_cfgPartials[i].fAmplitude;
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
            return { 0.0f, 0.0f };
        }

        const float env = m_envelope.nextSample();

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
        sample *= env;
        return { sample, sample };
    }

    void computeSamples(int32_t blockSize)
    {
        DSP_RESIZE(m_L, blockSize);
        DSP_RESIZE(m_R, blockSize);

        for (int i = 0; i < blockSize; i++)
        {
            StereoSampleF32 sample = nextSampleF32();
            m_L[i] = sample.L;
            m_R[i] = sample.R;
        }
    }

};
