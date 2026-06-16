#pragma once
#include <Config.h>
#include <common/Envelope.h>
#include <common/DspUtil.h>
#include <de/audio/OSC_Saw.h>
#include <de/audio/OSC_Additive.h>
#include <de/audio/OSC_AnalogDrift.h>
#include <de/audio/OSC_BlepHybrid.h>

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

    void process(float & L, float & R)
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
    de::audio::OSC_Additive m_oscAdditive;
    de::audio::OSC_AnalogDrift m_oscAnalogDrift;
    de::audio::OSC_BlepHybrid m_oscBlepHybrid;

    int m_channel = -1;
    int m_midiNote = -1;
    float m_detuneCent = 0.0f;
    float m_frequency = 220.f;

    de::TAlignedVector<float> m_L;
    de::TAlignedVector<float> m_R;

    double m_t = 0.0;
    double m_tStep = 0.0001;

    // SinOsc
    float m_phase = 0.0f;
    float m_phaseInc = 0.0001;

    void init(SynthCfg* cfg)
    {
        m_cfg = cfg;
        m_envelope.init(m_cfg->m_envelope);
        m_oscPartials.init(m_cfg->m_partials, cfg->m_sampleRate);

        DSP_RESIZE(m_L, 1024);
        DSP_RESIZE(m_R, 1024);
    }

    bool isPlaying() const { return m_envelope.isPlaying(); }

    void allNotesOff()
    {
        m_envelope.resetIdle();
        m_midiNote = -1;
    }

    // Phase aus Sample‑Index:
    //     float phase = 2.0f * M_PI * freq * (n / sampleRate);

    // Phase aus Zeit t:
    //     float phase = 2.0f * M_PI * freq * t;

    bool noteOn(int midiNote, int velocity, float detuneCent = 0.0f )
    {
        if (m_envelope.isPlaying())
        {
            //DE_ERROR("Env still playing")
            return false;
        }

        // if (m_midiNote > -1)
        // {
        //     DE_ERROR("Already playing midiNote(",m_midiNote,")")
        //     return false;
        // }

        const int m_sampleRate = m_cfg->m_sampleRate;

        m_midiNote = midiNote;
        m_frequency = de::calc_frequencyFromMidi(midiNote, detuneCent);
        float fSubOsc = de::calc_frequencyFromMidi(midiNote-12, detuneCent);
        //DE_TRACE("midiNote = ",m_midiNote,", frequency = ",m_frequency," Hz")

        m_envelope.init(m_cfg->m_envelope);
        m_envelope.triggerNoteOn( float(velocity) / 127.0f );

        m_oscPartials.noteOn(m_cfg->m_partials, m_frequency, velocity);
        m_oscSaw.noteOn(m_frequency, m_sampleRate, 100);

        int nPartials = 64;
        m_oscAdditive.noteOn(m_frequency, m_sampleRate, 64, de::audio::OSC_Additive::Saw);
        m_oscAnalogDrift.noteOn(m_frequency, m_sampleRate, 64, de::audio::OSC_AnalogDrift::Saw);
        m_oscBlepHybrid.noteOn(fSubOsc, m_sampleRate, de::audio::OSC_BlepHybrid::Saw);

        return true;
    }

    void noteOff(int velocity )
    {
        m_envelope.triggerNoteOff( float(velocity) / 127.0f );
        m_midiNote = -1;
    }

    struct StereoSampleF32
    {
        float L;
        float R;
    };

    StereoSampleF32 nextSampleF32()
    {
    // <Envelope>
        if (!m_envelope.isPlaying())
        {
            //DE_WARN("Envelope ended.")
            m_midiNote = -1;
            return { 0.0f, 0.0f };
        }
    // </Envelope>

        float L = 0.0f;
        float R = 0.0f;

        float Lp, Rp;
        m_oscPartials.process(Lp, Rp);
        L += Lp;
        R += Rp;

    // <OSC>
    /*
        float sampleL = m_oscAdditive.process();
        float sampleR = m_oscAnalogDrift.process();
        L += sampleL;
        R += sampleR;

        float sample = m_oscBlepHybrid.process();
        L += sample;
        R += sample;
    */
    // </OSC>

    // <Envelope>
        const float env = m_envelope.process();
        L *= env;
        R *= env;
    // </Envelope>

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
