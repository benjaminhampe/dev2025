#pragma once
#include <SineVoice.h>

class Synth
{

    SynthCfg m_cfg;

    std::vector<Voice> m_voices;

    //int64_t m_framePos = 0;
    // int32_t m_blockSize;
    // int32_t m_sampleRate;
    // std::vector<Partial> m_partials;
    // float m_sampleRate;
    // float m_masterAmplitude;
    // float m_baseFrequency = 55.f;
    //int32_t m_baseOctave;

public:

    const SynthCfg& getConfig() const { return m_cfg; }
    SynthCfg& getConfig() { return m_cfg; }


    void init()
    {
        m_cfg.init();

        m_voices.resize(m_cfg.m_maxVoices); // polyphony

        DE_OK("Created ",m_voices.size()," voices.")
    }

    void setSampleRate(int sampleRate)
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

    void setBlockSize(int blockSize)
    {
        if (blockSize == m_cfg.m_blockSize)
        {
            return;
        }
        m_cfg.m_blockSize = blockSize;
        DE_BENNI("setBlockSize(",blockSize,")")
    }

    int findIdleVoice() const
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

    void noteOn(int channel, int midiNote, int velocity)
    {
        DE_OK("NoteOn: ", midiNote)
        int voice = findIdleVoice();
        if (voice < 0)
        {
            return; // Discard, Information loss!
        }

        DE_OK("IdleVoice = ",voice)
        m_voices[voice].noteOn(channel, midiNote, velocity);
        /*
        m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
        // Optionally: trigger envelopes, voices, etc.
        calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
        */
    }

    void noteOff(int channel, int midiNote, int velocity)
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

        DE_OK("NoteOff: ",midiNote, " for nVoices = ",nVoices)
    }

    void allNotesOff()
    {
        DE_OK("allNotesOff()")
        for (size_t i = 0; i < m_voices.size(); i++)
        {
            Voice & voice = m_voices[i];
            voice.allNotesOff();
        }

    }

    void controlChange(int channel, int controller, int value)
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

    void pitchBend(int channel, int bendValue)
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

    void process( float* __restrict__ L, float* __restrict__ R )
    {
        size_t nNotes = 0;

        const int32_t blockSize = m_cfg.m_blockSize;
        //DE_OK("blockSize = ",blockSize)

        //DE_OK("m_synth.m_notes.size() = ", m_synth.m_notes.size(), ", "
        //        "sampleFrames = ", sampleFrames)

        std::memset(L,0,blockSize * sizeof(float));
        std::memset(R,0,blockSize * sizeof(float));

        for (Voice & voice : m_voices)
        {
            if (voice.isPlaying())
            {
                voice.computeSamples(blockSize);

                const float* __restrict__ srcL = voice.m_L.data();

                DE_ASSUME_NO_OVERLAP(srcL,L,blockSize * sizeof(float));

                for (int32_t i = 0; i < blockSize; i++)
                {
                    L[i] += srcL[i];
                }

                const float* __restrict__ srcR = voice.m_R.data();

                DE_ASSUME_NO_OVERLAP(srcR,R,blockSize * sizeof(float));

                for (int32_t i = 0; i < blockSize; i++)
                {
                    R[i] += srcR[i];
                }
            }
        }
    }

    // void setPartial(int index, float amplitude, double centDetune = 0.0);
    // void setPartialsToRect();
    // void setPartialsToSaw();
    // void setPartialsToSawRev();
    // void setPartialsToTriangle();

    // static void calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate );


};



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
*/



#if 0

#include <immintrin.h>
#include <cstddef>

// Fast sine approximation for __m256 using range reduction to [-pi, pi]
// sin(x) ≈ x + c3*x^3 + c5*x^5 + c7*x^7 (Taylor, decent for audio use)
static inline __m256 sin_ps(__m256 x) {
    // Constants
    const __m256 two_pi = _mm256_set1_ps(6.28318530717958647692f);
    const __m256 inv_two_pi = _mm256_set1_ps(1.0f / 6.28318530717958647692f);
    const __m256 pi = _mm256_set1_ps(3.14159265358979323846f);
    const __m256 c3 = _mm256_set1_ps(-1.0f / 6.0f);          // -0.16666667
    const __m256 c5 = _mm256_set1_ps(1.0f / 120.0f);         //  0.0083333337
    const __m256 c7 = _mm256_set1_ps(-1.0f / 5040.0f);       // -0.0001984127

    // Reduce x to y in [-pi, pi]: y = x - round(x / (2*pi)) * (2*pi)
    __m256 k = _mm256_round_ps(_mm256_mul_ps(x, inv_two_pi), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    __m256 y = _mm256_fnmadd_ps(k, two_pi, x); // y = x - k*2pi

    // Polynomial via Estrin
    __m256 y2 = _mm256_mul_ps(y, y);
    __m256 y3 = _mm256_mul_ps(y2, y);
    __m256 p = _mm256_fmadd_ps(c7, y2, c5);    // c5 + c7*y^2
    p = _mm256_fmadd_ps(p, y2, c3);            // c3 + y^2*(c5 + c7*y^2)
    p = _mm256_fmadd_ps(p, y3, y);             // y + y^3*(c3 + y^2*(c5 + c7*y^2))

    // Optional: improve accuracy near pi via sign flip (sine symmetry)
    // Map y to [-pi/2, pi/2] using quadrant, but omitted for speed.

    return p;
}

// Compute sample and Asum using AVX2
// m_amplitudes, m_phases, m_phaseIncrements are float arrays of size N
// de::TWO_PI = 2*pi
void process_block_avx2(const float* amplitudes,
                        float* phases,
                        const float* phaseIncrements,
                        size_t N,
                        float& outSample,
                        float& outAsum,
                        float two_pi /* de::TWO_PI */)
{
    const size_t vecWidth = 8;
    size_t i = 0;

    __m256 sample_acc = _mm256_set1_ps(0.0f);
    __m256 Asum_acc   = _mm256_set1_ps(0.0f);
    const __m256 TWO_PI = _mm256_set1_ps(two_pi);

    for (; i + vecWidth <= N; i += vecWidth) {
        // Load
        __m256 A   = _mm256_loadu_ps(amplitudes + i);
        __m256 P   = _mm256_loadu_ps(phases + i);
        __m256 dP  = _mm256_loadu_ps(phaseIncrements + i);

        // sample += A * sin(P)
        __m256 S = sin_ps(P);
        sample_acc = _mm256_fmadd_ps(A, S, sample_acc);

        // Asum += A
        Asum_acc = _mm256_add_ps(Asum_acc, A);

        // phases += dP
        P = _mm256_add_ps(P, dP);

        // if (P > TWO_PI) P -= TWO_PI;
        __m256 gt = _mm256_cmp_ps(P, TWO_PI, _CMP_GT_OQ);
        // subtract TWO_PI where gt is true
        __m256 adj = _mm256_and_ps(gt, TWO_PI);
        P = _mm256_sub_ps(P, adj);

        // Store back
        _mm256_storeu_ps(phases + i, P);
    }

    // Horizontal sum of accumulators
    auto hsum256 = [](const __m256 v) {
        __m128 low  = _mm256_castps256_ps128(v);
        __m128 high = _mm256_extractf128_ps(v, 1);
        __m128 sum128 = _mm_add_ps(low, high);
        // Horizontal sum 4 floats
        __m128 shuf = _mm_movehdup_ps(sum128);         // (b,d)
        __m128 sums = _mm_add_ps(sum128, shuf);        // (a+b, c+d)
        shuf = _mm_movehl_ps(shuf, sums);              // (c+d)
        sums = _mm_add_ss(sums, shuf);                 // (a+b+c+d, ...)
        return _mm_cvtss_f32(sums);
    };

    float sample_vec = hsum256(sample_acc);
    float Asum_vec   = hsum256(Asum_acc);

    // Scalar tail
    float sample_tail = 0.0f;
    float Asum_tail   = 0.0f;
    for (; i < N; ++i) {
        float A = amplitudes[i];
        float P = phases[i];

        sample_tail += A * sinf(P);
        Asum_tail   += A;

        P += phaseIncrements[i];
        if (P > two_pi) {
            P -= two_pi;
        }
        phases[i] = P;
    }

    outSample = sample_vec + sample_tail;
    outAsum   = Asum_vec + Asum_tail;
}

#endif
