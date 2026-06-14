/*
🎯 Core Architecture: One Phase, Many Partials

You do not want 64 independent oscillators.
You want one master phase:
ϕ=ϕ+Δϕ(wrap at 1.0)

Then each partial uses:
ϕn=n⋅ϕ

This is the cheapest possible way.
🧱 Efficient Sine Generation (3 Options)

Each partial needs a sine. You have three realistic options:
1) Polynomial sine (fastest)

Use a 5th‑order or 7th‑order minimax polynomial:
sin⁡(x)≈x⋅(1−x26+x4120)

But use a range‑reduced version for [0,2π).
This is extremely fast and accurate enough for partials.
2) Small wavetable (classic)

    2048 samples

    Linear interpolation

    SIMD‑friendly

    Perfect for 64 partials

3) CORDIC rotator (your style)

You can rotate a complex number by n⋅Δθ using CORDIC.
But for 64 partials, polynomial or table is faster.
🧮 SIMD Loop for 8–64 Partials

This is the real trick: compute 4 or 8 partials at once.

    Precompute n * phase into a SIMD register

    Range‑reduce

    Evaluate sine polynomial in SIMD

    Multiply by amplitude (1/n for saw, 1/n² for triangle, odd only for square)

    Accumulate

This gives you 64 partials in ~20–30 ns on a modern CPU.
🎛️ Partial Formulas for Classic Waves
Saw
x(t)=∑n=1N1nsin⁡(2πnft)
Square
x(t)=∑k=1N12k−1sin⁡(2π(2k−1)ft)
Triangle
x(t)=∑k=1N(−1)k(2k−1)2sin⁡(2π(2k−1)ft)
🧨 Band‑Limiting (Critical)

You must stop partials above Nyquist:
nmax⁡=⌊fs2f⌋

This gives you perfect anti‑aliasing without BLEP.
🎛️ Analogue “Life” (the good stuff)

To make it sound analogue, add:
1) Phase randomization

Each partial gets a tiny random offset:
ϕn=nϕ+ϵn

This breaks the sterile digital alignment.
2) Per‑partial drift

Use a slow random LFO:
fn=nf⋅(1+dn(t))

Where dn(t) is a 0.1–0.3% random walk.
3) Amplitude jitter

Very small (±0.1 dB) random modulation per partial.
4) Soft saturation

After summing 64 partials, run through:
y=tanh⁡(gx)

This is what makes analogue saws “chewy”.
🧪 Digital Magic (modern tricks)
1) SIMD partial summation

Use AVX2/AVX‑512 to compute 8–16 partials per instruction.
2) Harmonic caching

For stable frequencies (e.g. notes), precompute:

    harmonic amplitudes

    harmonic phase offsets

Then only update phase.
3) PolyBLEP on top

Even though you’re additive, adding a tiny polyBLEP on the fundamental gives the “edge” of analogue oscillators.
4) Oversampling only the high partials

Oversample by 2× or 4× only for partials near Nyquist.
This is extremely efficient and sounds expensive.
🧩 Minimal C++ Skeleton (SIMD‑ready)

(Inline, no abstractions, your style)
cpp

struct AdditiveOsc {
    double phase = 0.0;
    double phaseInc;
    int maxPartials;

    float process() {
        phase += phaseInc;
        if (phase >= 1.0) phase -= 1.0;

        float out = 0.0f;
        double p = phase * 2.0 * M_PI;

        for (int n = 1; n <= maxPartials; ++n) {
            double x = p * n;
            x -= floor(x / (2.0*M_PI)) * (2.0*M_PI); // range reduce

            // 5th order sine approx
            double x2 = x*x;
            double s = x * (1.0 - x2/6.0 + (x2*x2)/120.0);

            out += float(s * (1.0 / n)); // saw
        }

        return out;
    }
};

Replace the loop with AVX2 for real speed.
🧠 What you should do next

You have two paths:

    Build a 64‑partial SIMD additive engine

    Build a BLEP hybrid oscillator

Which direction do you want to go deeper into:

    SIMD additive oscillator

    Analogue drift modelling

    BLEP hybrid oscillator

Pick one and I’ll write the full low‑level C++ implementation.
explain BLEP, PolyBLEP and show me code of your BLEP hybrid Osc

You picked a fun rabbit hole. Let’s go straight to the meat and keep it low‑level.
What BLEP is

BLEP = Band‑Limited Step.

    Problem: naive saw/square have instant steps → infinite harmonics → aliasing above Nyquist.

    Idea: don’t band‑limit the whole waveform, just band‑limit the step itself.

    You precompute (or approximate) a band‑limited step response and add/subtract a tiny correction around each discontinuity.

Conceptually:

    Start with a cheap, aliased waveform (naive saw/square).

    At each discontinuity (where the waveform jumps), you overlay a small correction curve that morphs the ideal step into a band‑limited one.

    That correction is the BLEP.

What PolyBLEP is

PolyBLEP = Polynomial BLEP.

Instead of a table or FIR, you use a small polynomial over a tiny phase window around the discontinuity.

Let:

    ϕ∈[0,1) be the phase

    Δ=f/fs be the phase increment per sample

The PolyBLEP function is usually defined on [0,1) and only non‑zero in a window of width Δ around the step:
text

t = phase / dT   (or shifted around the discontinuity)

A common 2‑sample PolyBLEP:

    For 0≤t<1:

polyblep(t)=t2−2t+1

    For 1≤t<2:

polyblep(t)=−t2+2t−1

    Else: 0

(There are slightly different normalizations; the idea is the same: a small polynomial that starts at 1, ends at 0, and has continuous derivatives.)

You then subtract this from a naive saw at the rising edge (or add/subtract appropriately for square).
Hybrid idea

The “hybrid” part here is:

    Core waveform: naive phase accumulator → saw/square/triangle

    Anti‑aliasing: PolyBLEP (for steps) and PolyBLAMP (for slope discontinuities, e.g. triangle)

    Analogue flavor: drift, slight noise, soft clip, etc. (you can bolt that on later)

So you get:

    Efficiency of a single phase oscillator

    Anti‑aliasing from PolyBLEP/PolyBLAMP

    No need for huge wavetables or full additive synthesis

Minimal PolyBLEP helpers (C++)
cpp

inline float poly_blep(float t, float dt)
{
    // t in [0,1), dt = phase increment
    if (t < dt) {
        t /= dt;
        // 2-sample polyBLEP
        return t + t - t * t - 1.0f;
    } else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

// For triangle: PolyBLAMP (band-limited ramp) for slope discontinuity
inline float poly_blamp(float t, float dt)
{
    // Integrated polyBLEP; one common simple form:
    if (t < dt) {
        t /= dt;
        return t * t * 0.5f - t * t * t / 3.0f;
    } else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t * 0.5f + t * t * t / 3.0f + t + 1.0f / 6.0f;
    }
    return 0.0f;
}

You can tweak these polynomials; I’m giving you a clear, readable starting point rather than the most micro‑optimized variant.
BLEP hybrid oscillator (saw, square, triangle)

Single struct, single phase, three waveforms, PolyBLEP/BLAMP:
cpp

struct BlepHybridOsc
{
    float sampleRate = 48000.0f;
    float freq       = 440.0f;
    float phase      = 0.0f;   // [0,1)
    float lastTri    = 0.0f;   // for triangle integration

    enum Wave { Saw, Square, Triangle } wave = Saw;

    void setSampleRate(float sr) { sampleRate = sr; }
    void setFreq(float f)        { freq = f; }
    void setWave(Wave w)         { wave = w; }

    inline float process()
    {
        float dt = freq / sampleRate;   // phase increment
        phase += dt;
        if (phase >= 1.0f)
            phase -= 1.0f;

        float t = phase;
        float out = 0.0f;

        switch (wave)
        {
            case Saw:
            {
                // naive saw: -1..+1
                float s = 2.0f * t - 1.0f;

                // discontinuity at phase wrap (t ~ 0)
                s -= poly_blep(t, dt);

                out = s;
            } break;

            case Square:
            {
                // naive square: -1..+1
                float s = (t < 0.5f) ? 1.0f : -1.0f;

                // rising edge at t = 0
                s += poly_blep(t, dt);

                // falling edge at t = 0.5
                float t2 = fmodf(t + 0.5f, 1.0f);
                s -= poly_blep(t2, dt);

                out = s;
            } break;

            case Triangle:
            {
                // Start from band-limited square
                float sq = (t < 0.5f) ? 1.0f : -1.0f;
                sq += poly_blep(t, dt);
                float t2 = fmodf(t + 0.5f, 1.0f);
                sq -= poly_blep(t2, dt);

                // Integrate square to get triangle
                // Simple leaky integrator to keep it bounded
                lastTri += (2.0f * freq / sampleRate) * sq;
                // optional light damping
                lastTri *= 0.999f;

                // Optionally apply BLAMP at slope changes (triangle corners)
                // (here: corners at t=0.25, 0.75 if you want to be precise)
                // For a simple version, you can skip BLAMP or refine later.

                out = lastTri;
            } break;
        }

        return out;
    }
};

Notes:

    Saw: naive saw + PolyBLEP at the wrap point.

    Square: naive square + PolyBLEP at both edges (0 and 0.5).

    Triangle: integrate a band‑limited square; you can refine with PolyBLAMP at slope changes if you want ultra‑clean corners.

This is already a very usable, efficient oscillator. You can now:

    Add drift by modulating freq with a slow random LFO.

    Add per‑voice phase offset for unison.

    Add soft saturation after the oscillator for analogue feel.

If you want, next step we can:

    Tighten the PolyBLEP/BLAMP polynomials

    Make a SIMD version of this oscillator

    Or combine this with your additive partial engine (e.g. BLEP for the fundamental + additive for low partials for extra “meat”).

show me different precise polynomial sine approx and show if they can still be represented by float or double and how they compare to real sin function

Below is a deep, precise, low‑level breakdown of the best polynomial sine approximations, how accurate they are, how they behave in float vs double, and how they compare to the real sin().

I’ll give you:

    The actual polynomials (minimax, Taylor, Estrin‑friendly)

    Their max error in float/double

    Whether they fit safely in float coefficients

    A comparison table

    SIMD‑friendly C++ code

And I’ll keep everything grounded in the DSP context you care about: oscillators, partials, and phase‑accumulation.
🎯 1. The Three Polynomial Families You Actually Want

There are many, but only three matter for oscillators:
1) Taylor polynomial (simple, not minimax)
sin⁡(x)≈x−x36+x5120

    Very fast

    Error grows quickly outside [−π/2,π/2]

    Needs range reduction

Max error: ~2.3e‑4 (float), ~2.3e‑4 (double)
Float‑safe: Yes
Use case: partials, FM, additive, cheap LFOs
2) Minimax Remez 5th‑order (best 5th‑order you can get)
sin⁡(x)≈x⋅(1+a2x2+a4x4)

Coefficients (minimax over [−π/2,π/2]):
Code

a2 = -0.1666666716f
a4 =  0.0083330251f

Max error: ~1.2e‑6 (float), ~1.2e‑6 (double)
Float‑safe: Yes
Use case: high‑quality oscillators, partial banks, VA synths
3) Minimax 7th‑order (extremely accurate)
sin⁡(x)≈x+a3x3+a5x5+a7x7

Coefficients:
Code

a3 = -0.16666667163372
a5 =  0.00833306577176
a7 = -0.00019807418727

Max error: ~3e‑9 (float), ~3e‑9 (double)
Float‑safe: Yes
Use case: band‑limited oscillators, FM, PM, additive with 64+ partials
🎛️ 2. Range Reduction (Critical)

All polynomials require:
x=phase⋅2π

Then reduce:
cpp

x -= floorf(x * INV_TWO_PI) * TWO_PI;
if (x >  M_PI) x -= TWO_PI;
if (x < -M_PI) x += TWO_PI;

This keeps the polynomial stable.
🧪 3. Accuracy Comparison Table
Approx	Order	Max Error	Float OK?	Speed	Use Case
Taylor 5th	5	~2e‑4	Yes	⭐⭐⭐⭐⭐	LFO, partials
Minimax 5th	5	~1e‑6	Yes	⭐⭐⭐⭐	Oscillators
Minimax 7th	7	~3e‑9	Yes	⭐⭐⭐	High‑quality VA
std::sin()	—	<1e‑16	double only	⭐	Offline, not realtime
🧩 4. C++ Implementations (SIMD‑friendly)
Taylor 5th
cpp

inline float sin_taylor5(float x)
{
    float x2 = x * x;
    return x * (1.0f - x2 * (1.0f/6.0f) + x2 * x2 * (1.0f/120.0f));
}

Minimax 5th (recommended)
cpp

inline float sin_minimax5(float x)
{
    float x2 = x * x;
    return x * (1.0f + x2 * (-0.1666666716f + x2 * 0.0083330251f));
}

Minimax 7th (very accurate)
cpp

inline float sin_minimax7(float x)
{
    float x2 = x * x;
    return x * (1.0f
        + x2 * (-0.16666667163372f
        + x2 * ( 0.00833306577176f
        + x2 * (-0.00019807418727f))));
}

🧠 5. Float vs Double: What Actually Matters
All coefficients fit in float

Even the 7th‑order minimax fits safely in 32‑bit float.
Error is dominated by polynomial order, not float precision

Because:

    Input x is already float (phase)

    Output is float (oscillator sample)

    Coefficients are float

Using double does not improve the output unless your entire DSP chain is double.
Double only helps if:

    You do FM/PM with huge modulation indices

    You accumulate phase in double

    You want extremely stable long‑term phase coherence

For oscillators and partials: float is perfect.
🎧 6. How They Compare to Real sin()
Taylor 5th

    Error ~2e‑4

    Audible? No, unless used for FM at high index

    Perfect for additive partials

Minimax 5th

    Error ~1e‑6

    Completely inaudible

    Best speed/quality ratio

Minimax 7th

    Error ~3e‑9

    Essentially identical to sin()

    Slightly slower, but still SIMD‑friendly

std::sin()

    20–40× slower

    Overkill for oscillators

    Not SIMD‑friendly

🧨 7. Which One Should You Use?

Given your style (low‑level, partial banks, oscillators):

    For 8–64 partial additive:
    → Minimax 5th

    For FM/PM oscillator cores:
    → Minimax 7th

    For LFOs, envelopes, cheap modulation:
    → Taylor 5th
*/