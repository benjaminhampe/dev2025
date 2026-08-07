#pragma once
#include <de/Core.h>
#include <cstdint>
#include <cmath>
#include <limits>
#include <algorithm>

namespace de {
// namespace audio {
// namespace math {

    // #include <xmmintrin.h>

    // inline float clampf_sse(float x, float lo, float hi)
    // {
    //     __m128 v = _mm_set_ss(x);
    //     __m128 l = _mm_set_ss(lo);
    //     __m128 h = _mm_set_ss(hi);
    //     v = _mm_max_ss(v, l);
    //     v = _mm_min_ss(v, h);
    //     return _mm_cvtss_f32(v);
    // }

    // ✅ 1. Frequency of each FFT bin (PFFFT)

    // f[ i ] = i * sampleRate / fftSize;

    inline float bin2freq(int i, float sampleRate, float fftSize)
    {
        return (sampleRate / fftSize) * i;
    }

    // ✅ 2. Linear mapping from frequency → x coordinate

    // x( f ) = width * f / nyquist; nyquist = sampleRate/2;

    inline float freq2lin(float f, float fMin, float fMax, float sampleRate)
    {
        fMin = std::fmaxf(fMin,0.0f);
        fMax = std::fminf(fMax,sampleRate * 0.5f); // nyquist
        return (f - fMin) / (fMax - fMin); // normalized in range [0,1]
    }

    inline float lin2freq(float x_norm, float fMin, float fMax, float sampleRate)
    {
        fMin = std::fmaxf(fMin,0.0f);
        fMax = std::fminf(fMax,sampleRate * 0.5f); // nyquist
        return x_norm * (fMax - fMin) + fMin;
    }

    // ✅ 3. Logarithmic mapping from frequency → x coordinate

    // x( f ) = width * (log(f) - log(fMin))/(log(fMax) - log(fMin));

    inline float freq2log(float f, float fMin, float fMax, float sampleRate)
    {
        fMin = std::fmaxf(fMin,1.0f);
        fMax = std::fminf(fMax,sampleRate * 0.5f); // nyquist
        const float lMin = std::logf(fMin);
        const float lRange = std::logf(fMax) - lMin;
        f = ::de::clampf(f, fMin, fMax);
        return (std::log(f) - lMin) / lRange; // normalized in range [0,1]
    }

    inline float log2freq(float x_norm, float fMin, float fMax, float sampleRate)
    {
        fMin = std::fmaxf(fMin,1.0f);
        fMax = std::fminf(fMax,sampleRate * 0.5f); // nyquist
        const float lMin = std::logf(fMin);
        const float lMax = std::logf(fMax);
        float logF = x_norm * (lMax - lMin) + lMin;
        return std::exp(logF);
    }


    template <typename T>
    bool isPositiveInfinity(const T t)
    {
        return std::isinf(t) && !std::signbit(t);
    }

    template <typename T>
    bool isNegativeInfinity(const T t)
    {
        return std::isinf(t) && std::signbit(t);
    }

    // Simple window functions:
    // TODO: Replace with stateful classes that contain precomputed lookup tables.

    inline void apply_hann(float* __restrict__ dst,
                            const float* __restrict__ src, int32_t N)
    {
        if (N < 2)
        {
            DE_ERROR("N = ",N)
            return;
        }
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.5f * (1.0f - cosf(2.0f * M_PI * n / (N - 1)));
            dst[n] = src[n] * std::clamp(w, 0.0f, 1.0f);
        }
    }

    inline void apply_hamming(float* __restrict__ dst,
                              const float* __restrict__ src, int32_t N)
    {
        if (N < 2)
        {
            DE_ERROR("N = ",N)
            return;
        }
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.54f - 0.46f * cosf(2.0f * M_PI * n / (N - 1));
            dst[n] = src[n] * std::clamp(w, 0.0f, 1.0f);
        }
    }

    inline void apply_blackman(float* __restrict__ dst,
                               const float* __restrict__ src, int32_t N)
    {
        if (N < 2)
        {
            DE_ERROR("N = ",N)
            return;
        }
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.42f
                      - 0.5f * cosf(2.0f * M_PI * n / (N - 1))
                      + 0.08f *cosf(4.0f * M_PI * n / (N - 1));
            dst[n] = src[n] * std::clamp(w, 0.0f, 1.0f);
        }
    }

    // Converts amplitude to decibels (dB)
    // ✅ Signed amplitude ↔ signed dB conversion
    // ✅ Proper handling of ±INFINITY
    // ✅ Round-trip compatibility for graphical control
    // ✅ Comments for clarity (finally!)
    // Converts a signed amplitude to a signed decibel value.
    // Positive amplitudes yield positive dB, negative amplitudes yield negative dB.
    // Zero amplitude returns -INFINITY to represent silence/mute.

    template <typename T>
    T convert_amplitude_to_dB(const T amplitude)
    {
        // Logarithm of zero is undefined; use -∞ to represent silence
        if (amplitude == T(0))
        {
            return -std::numeric_limits<T>::infinity();
        }

        // Compute magnitude in dB
        T magnitudeDB = T(20) * std::log10(std::abs(amplitude));

        // Preserve sign of original amplitude
        return (amplitude > T(0)) ? magnitudeDB : -magnitudeDB;
    }

    // double amplitudeToDb(double amplitude)
    // {
    //     if (amplitude == 0.0) return -INFINITY;
    //     double magnitude = std::abs(amplitude);
    //     double dB = 20.0 * std::log10(magnitude);
    //     return (amplitude > 0) ? dB : -dB;
    // }

    // double amplitudeToDb(double amplitude)
    // {
    //     if (amplitude <= 0.0) return -INFINITY; // log(0) is undefined
    //     return 20.0 * std::log10(amplitude);
    // }

    // Converts a signed decibel value back to signed amplitude.
    // ✅ Signed amplitude ↔ signed dB conversion
    // ✅ Proper handling of ±INFINITY
    // ✅ Round-trip compatibility for graphical control
    // ✅ Comments for clarity (finally!)
    // Negative dB returns negative amplitude, positive dB returns positive amplitude.
    // -INFINITY dB returns 0.0 amplitude (mute).
    template <typename T>
    T convert_dB_to_amplitude(T dB)
    {
        if (std::isinf(dB) && dB < T(0))
        {
            // -∞ dB means zero amplitude
            return T(0);
        }

        // Compute magnitude from dB
        T magnitude = std::pow(T(10), std::abs(dB) / T(20));

        // Restore sign from dB value
        return (dB >= T(0)) ? magnitude : -magnitude;
    }

    // Converts decibels (dB) back to amplitude
    // double decibelsToAmplitude(double dB)
    // {
    //     return std::pow(10.0, dB / 20.0);
    // }

    // double decibelsToAmplitude(double dB)
    // {
    //     if (std::isinf(dB) && dB < 0) return 0.0; // -∞ dB means zero amplitude
    //     return std::pow(10.0, dB / 20.0);
    // }


    // 📐 1. Rechteckfunktion (nur ungerade Frequenzen)
    template <typename T>
    T calc_amplitude_rect(const int partial)
    {
        return (partial % 2 == 1) ? T(4) / T(M_PI * partial) : T(0);
    }

    // 📉 2. Sägezahnfunktion (alle Frequenzen, fallend)
    template <typename T>
    T calc_amplitude_saw(const int partial)
    {
        return T(2) / T(M_PI * partial);
    }

    // 📈 3. Umgekehrte Sägezahnfunktion (alle Frequenzen, steigend)
    template <typename T>
    T calc_amplitude_saw_rev(const int partial)
    {
        return -T(2) / T(M_PI * partial);
    }

    // ▲ Returns amplitude of the nth partial of a triangle wave
    // Only odd harmonics are non-zero: n = 1, 3, 5, ...
    template <typename T>
    T calc_amplitude_triangle(const int partial)
    {
        if (partial % 2 == 0)
        {
            return 0.0; // Even harmonics are zero
        }

        // Amplitude formula: (8 / (π^2)) * (1 / n^2) * (-1)^((n - 1)/2)
        double amplitude = (8.0 / (M_PI * M_PI)) * (1.0 / (partial * partial));
        int sign = ((partial - 1) / 2) % 2 == 0 ? 1 : -1;
        return amplitude * sign;
    }


    // 🎹Frequency from MIDI Note and Cent Tuning, MIDI note 69 =
    //  - frequencyFromMidi(69);           // A4 = 440 Hz
    //  - frequencyFromMidi(60);           // C4 ≈ 261.63 Hz
    //  - frequencyFromMidi(60, 50.0);     // C4 + 50 cents ≈ 267.94 Hz
    template <typename T>
    T calc_frequencyFromMidi(const int midiNote, const T centOffset = T(0))
    {
        return T(440) * std::pow(T(2), (T(midiNote - 69) + T(0.01) * centOffset) / T(12));
    }

    // 🎹Frequency from Octave, Semitone, and Cent Tuning
    //  - frequencyFromPitch(4, 9);          // A4 = 440 Hz
    //  - frequencyFromPitch(3, 0);          // C3 ≈ 130.81 Hz
    //  - frequencyFromPitch(5, 7, -25.0);   // G5 - 25 cents ≈ 783.99 Hz
    template <typename T>
    T
    calc_frequencyFromPitch(const int octave, const int semitone, const T centOffset = T(0))
    {
        const int midiNote = (octave + 1) * 12 + semitone;
        return calc_frequencyFromMidi<T>(midiNote, centOffset);
    }

// } // end namespace math.
// } // end namespace audio.
} // end namespace de.
