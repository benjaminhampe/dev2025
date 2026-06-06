#pragma once
#include <Config.h>
#include <common/Envelope.h>
#include <common/DspUtil.h>


// 📊
struct Note
{
    int32_t m_frameIndex = 0;
    int32_t m_frameCount = 0;
    int m_channel = 0;
    //int m_midiNote = 0;
    float m_velocity = 1.0f;
    //float m_detuneCent = 0;
    float m_amplitudeSum;
    float m_amplitudeSumInv;
    de::TAlignedVector<float> m_amplitudes;
    //de::TAlignedVector<float> m_frequencies;
    de::TAlignedVector<float> m_phases;
    de::TAlignedVector<float> m_phaseIncrements;

    de::TAlignedVector<float> m_L;
    de::TAlignedVector<float> m_R;

    bool m_singleShot = true;

    ADSR m_envelope;

    // calcPhaseIncrements
    void init( const Cfg& cfg, int channel, int midiNote, int velocity, float detuneCent = 0.0f )
    {
        float baseFrequency = de::calc_frequencyFromMidi(midiNote, detuneCent);

        m_velocity = float(velocity) / 127.0f;

        DSP_RESIZE(m_amplitudes, cfg.getNumPartials());

        DSP_RESIZE(m_phases, cfg.getNumPartials());

        DSP_RESIZE(m_phaseIncrements, cfg.getNumPartials());

        //m_frequencies.resize(cfg.getNumPartials());

        //m_envelope = cfg.m_adsr;
        m_envelope.init(cfg.m_envAttackTimeInMs * 0.001f,
                        cfg.m_envDecayTimeInMs * 0.001f,
                        cfg.m_envSustainLevel,
                        cfg.m_envReleaseTimeInMs * 0.001f,
                        cfg.m_sampleRate );

        const float fScale = de::TWO_PI * baseFrequency / cfg.m_sampleRate;

        float amplitudeSum = 0.0f;

        for (size_t i = 0; i < cfg.getNumPartials(); i++)
        {
            amplitudeSum += cfg.m_partials.at(i).fAmplitude;
            m_amplitudes[i] = cfg.m_partials[i].fAmplitude;
            //m_frequencies[i] = cfg.m_partials[i].fAmplitude;
            m_phases[i] = 0.0f;
            m_phaseIncrements[i] = fScale * (i+1);
        }

        m_amplitudeSum = amplitudeSum;
        if (amplitudeSum > 1e-12)
        {
            m_amplitudeSumInv = 1.0f / amplitudeSum;
            m_frameCount = m_envelope.getFrameCount();
        }
        else
        {
            m_amplitudeSumInv = 0.0f;
            m_frameCount = 0; // -> Skip note, mark slot as free.
        }
    }

    void
    computeSamples(VstInt32 sampleFrames)
    {
        const auto N = static_cast<size_t>(sampleFrames);
        DSP_RESIZE(m_L, N);
        DSP_RESIZE(m_R, N);
        DSP_ZERO(m_L);
        DSP_ZERO(m_R);

        const int32_t availFrames = m_frameCount - m_frameIndex;
        if (availFrames > 0)
        {
            for (int i = 0; i < availFrames; i++)
            {
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

                m_L.at(i) = sample;
                m_R.at(i) = sample;
            }
        }

        m_frameIndex += sampleFrames;

        if (m_frameIndex >= m_frameCount)
        {
            m_frameCount = 0;
        }
    }

};