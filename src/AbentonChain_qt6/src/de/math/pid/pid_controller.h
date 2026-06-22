#include <cmath>
#include <cfloat>

inline bool isBad(float x)
{
    return std::isnan(x) || std::isinf(x);
}

struct PID
{
    float Kp = 0.05f;
    float Ki = 0.0005f;
    float Kd = 0.02f;

    float target = 0.0f;
    float current = 0.0f;

    float integral = 0.0f;
    float prevError = 0.0f;

    float integralLimit = 100.0f;
    float outputLimit   = 100.0f;

    // Avoid denormals
    static inline float zap(float x)
    {
        return (std::fabs(x) < 1e-20f ? 0.0f : x);
    }

    // Set a new target
    inline void setTarget(float t)
    {
        if (!isBad(t))
            target = t;
    }

    // Hard reset (non-legato)
    inline void reset(float value)
    {
        if (isBad(value)) value = 0.0f;

        target = current = value;
        integral = 0.0f;
        prevError = 0.0f;
    }

    // One control update (call per block or per sample)
    inline float update()
    {
        float error = target - current;

        if (isBad(error)) error = 0.0f;

        // Integral term
        integral += error;
        integral = std::clamp(integral, -integralLimit, integralLimit);
        integral = zap(integral);

        // Derivative term
        float derivative = error - prevError;
        prevError = error;
        derivative = zap(derivative);

        // PID output
        float control = Kp * error + Ki * integral + Kd * derivative;

        if (isBad(control))
            control = 0.0f;

        control = std::clamp(control, -outputLimit, outputLimit);
        control = zap(control);

        // Apply to current value
        current += control;

        if (isBad(current))
            current = target; // fail-safe

        current = zap(current);

        return current;
    }
};

/*

// 🎼 Example A — Pitch glide (legato)

PID glide;
glide.reset(0.0f);       // C4
glide.setTarget(7.0f);   // G4

float semis = glide.update();
float freq = 440.0f * std::pow(2.0f, (semis - 69.0f) / 12.0f);


// 🎻 Example B — Bow‑like expressive transitions

glide.Kp = 0.03f;
glide.Ki = 0.0001f;
glide.Kd = 0.05f;   // more overshoot

glide.setTarget(12.0f); // octave jump

🎹 Example C — Velocity‑dependent glide

float vel = 0.8f; // MIDI velocity normalized

glide.Kp = 0.02f + vel * 0.03f;
glide.Kd = 0.01f + vel * 0.04f;

glide.setTarget(5.0f);

🥁 Example D — Rhythm‑reactive pitch modulation

float transient = detectTransient(input); // 0..1

glide.setTarget( basePitch + transient * 3.0f );


🎤 Example E — Formant‑like vowel sweeps

PID cutoffPID;
cutoffPID.reset(500.0f);
cutoffPID.setTarget(2000.0f);

float cutoff = cutoffPID.update();


#include <cmath>
#include <algorithm>

inline bool isBad(float x)
{
    return std::isnan(x) || std::isinf(x);
}

inline float zap(float x)
{
    return (std::fabs(x) < 1e-20f ? 0.0f : x);
}

struct PID
{
    float Kp = 0.05f;
    float Ki = 0.0005f;
    float Kd = 0.02f;

    float target = 0.0f;
    float current = 0.0f;

    float integral = 0.0f;
    float prevError = 0.0f;

    float integralLimit = 100.0f;
    float outputLimit   = 100.0f;

    void setTarget(float t)
    {
        if (!isBad(t))
            target = t;
    }

    void reset(float value)
    {
        if (isBad(value)) value = 0.0f;
        target = current = value;
        integral = 0.0f;
        prevError = 0.0f;
    }

    float update()
    {
        float error = target - current;
        if (isBad(error)) error = 0.0f;

        integral += error;
        integral = std::clamp(integral, -integralLimit, integralLimit);
        integral = zap(integral);

        float derivative = error - prevError;
        prevError = error;
        derivative = zap(derivative);

        float control = Kp * error + Ki * integral + Kd * derivative;
        if (isBad(control)) control = 0.0f;

        control = std::clamp(control, -outputLimit, outputLimit);
        control = zap(control);

        current += control;
        if (isBad(current)) current = target;

        current = zap(current);
        return current;
    }
};


inline float semitonesToHz(float semitones)
{
    return 440.0f * std::pow(2.0f, (semitones - 69.0f) / 12.0f);
}

struct SineOsc
{
    float phase = 0.0f;
    float sampleRate = 48000.0f;

    float process(float freq)
    {
        float inc = freq / sampleRate;
        phase += inc;
        if (phase >= 1.0f) phase -= 1.0f;

        return std::sin(phase * 2.0f * float(M_PI));
    }
};

struct SynthVoice
{
    PID glide;
    SineOsc osc;

    bool active = false;
    bool legatoMode = true;

    float currentNote = 0.0f; // MIDI note number

    void noteOn(float midiNote, bool legato)
    {
        float semis = midiNote;

        if (!active || !legato)
        {
            glide.reset(semis);
        }
        else
        {
            glide.setTarget(semis);
        }

        currentNote = midiNote;
        active = true;
    }

    void noteOff()
    {
        active = false;
    }

    float process()
    {
        if (!active)
            return 0.0f;

        float semis = glide.update();
        float freq = semitonesToHz(semis);

        return osc.process(freq);
    }
};

voice.noteOn(60, false); // C4
voice.noteOn(67, true);  // G4 (legato)


voice.noteOn(55, false); // G3
voice.noteOn(72, true);  // C5



voice.noteOn(60, false);
voice.noteOn(62, true);
voice.noteOn(64, true);
voice.noteOn(67, true);




struct TransientDetector
{
    float envFast  = 0.0f;   // fast envelope
    float envSlow  = 0.0f;   // slow envelope
    float sampleRate = 48000.0f;

    // attack/release times in ms
    float fastAttackMs  = 1.0f;
    float fastReleaseMs = 20.0f;

    float slowAttackMs  = 10.0f;
    float slowReleaseMs = 200.0f;

    inline float zap(float x)
    {
        return (std::fabs(x) < 1e-20f ? 0.0f : x);
    }

    inline bool bad(float x)
    {
        return std::isnan(x) || std::isinf(x);
    }

    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }

    // Process one sample, return transient strength 0..1
    float process(float x)
    {
        if (bad(x)) x = 0.0f;

        float rect = std::fabs(x);
        rect = zap(rect);

        // Convert ms to coefficients
        float fastA = std::exp(-1.0f / (0.001f * fastAttackMs  * sampleRate));
        float fastR = std::exp(-1.0f / (0.001f * fastReleaseMs * sampleRate));

        float slowA = std::exp(-1.0f / (0.001f * slowAttackMs  * sampleRate));
        float slowR = std::exp(-1.0f / (0.001f * slowReleaseMs * sampleRate));

        // Fast envelope
        if (rect > envFast)
            envFast = rect + fastA * (envFast - rect);
        else
            envFast = rect + fastR * (envFast - rect);

        // Slow envelope
        if (rect > envSlow)
            envSlow = rect + slowA * (envSlow - rect);
        else
            envSlow = rect + slowR * (envSlow - rect);

        envFast = zap(envFast);
        envSlow = zap(envSlow);

        if (bad(envFast)) envFast = 0.0f;
        if (bad(envSlow)) envSlow = 0.0f;

        // Transient = fast - slow
        float t = envFast - envSlow;

        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        return zap(t);
    }
};


TransientDetector transient;
float input = osc.process(freq);
float t = transient.process(input); // 0..1 transient strength

Now you can feed t into anything:

    PID glide target modulation

    filter cutoff

    FM index

    amplitude accents

    pitch scoops

    rhythmic modulation


🎹 3. Example: transient‑reactive glide

float t = transient.process(sample);

// Add up to 3 semitones on strong transients
float target = baseNote + t * 3.0f;

voice.glide.setTarget(target);

🥁 4. Example: transient‑driven rhythmic wobble

float t = transient.process(sample);
float wobble = 1.0f + t * 0.1f; // 10% pitch wobble on hits

float freq = baseFreq * wobble;

🎤 5. Example: transient‑controlled formant sweep

float t = transient.process(sample);
filterCutoff = 500.0f + t * 2500.0f;

*/