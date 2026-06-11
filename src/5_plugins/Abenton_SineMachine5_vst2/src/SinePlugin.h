#pragma once
#include "SineEditor.h"
#include "SineSynth.h"
#include <set>

enum
{
    kParamA = 0,
    kNumParams = 1
};
const int kNumPrograms = 0;
const int kNumInputs = 2;
const int kNumOutputs = 2;
const unsigned long kUniqueId = 'hSM4';    // Hampe SineMachine4

AudioEffect* createEffectInstance(audioMasterCallback audioMaster);

class Plugin : public AudioEffectX
{
private:
    char m_programName[kVstMaxProgNameLen + 1];

    std::set< std::string > m_canDo;

    // int32_t m_sampleRate;
    // int32_t m_blockSize;

    SynthCfg m_cfg;

    Synth m_synth;

    Editor m_editor;

public:
    Plugin(audioMasterCallback audioMaster);
    ~Plugin();

    VstInt32 getVendorVersion() override {return 1004;}            // Version number
    VstPlugCategory getPlugCategory() override {return kPlugCategEffect;} // The general category for the plug-in
    bool getEffectName(char* name) override;                       // The plug-in name
    bool getProductString(char* text) override;                    // This is a unique plug-in string provided by Steinberg
    bool getVendorString(char* text) override;                     // Vendor info

    void getProgramName(char *name) override;                      // read the name from the host
    void setProgramName(char *name) override;                      // changes the name of the preset displayed in the host

    void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames) override;
    void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames) override;

    VstInt32 getChunk (void** data, bool isPreset) override;
    VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset) override;
    float getParameter(VstInt32 index) override;                    // get the parameter value at the specified index
    void setParameter(VstInt32 index, float value) override;        // set the parameter at index to value
    void getParameterLabel(VstInt32 index, char *text) override;    // label for the parameter (eg dB)
    void getParameterName(VstInt32 index, char *text) override;     // name of the parameter
    void getParameterDisplay(VstInt32 index, char *text) override;  // text description of the current value
    VstInt32 canDo(char* text) override;

    VstIntPtr dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value, void* ptr, float opt) override;
    void open() override;
    void close() override;

    SynthCfg& getConfig() { return m_cfg; }
    SynthCfg const& getConfig() const { return m_cfg; }

    VstInt32 processEvents(VstEvents* events) override;
    void handleShortMidi(char bytes[4]);

    // void changeSampleRate( int32_t sampleRate );
    // void changeFrameCount( int32_t sampleRate );





/*
    VstInt32 processEvents(VstEvents* events) override;
    void handleMidi(unsigned char* data);
    int currentNote = -1;
    float modDepth = 0.0f;
    float masterGain = 1.0f;
    float filterCutoff = 1.0f;
    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note, int velocity);
    void controlChange(int channel, int controller, int value);
    void pitchBend(int channel, int bendValue);
*/
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

VstInt32 Plugin::processEvents(VstEvents* events)
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

void Plugin::handleMidi(unsigned char* data)
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
