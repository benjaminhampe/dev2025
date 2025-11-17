#pragma once
#include <cstdint>
#include <cmath>
#include <limits>
#include <algorithm>

namespace de {

    // π
    constexpr float TWO_PI = float( 2.0 * M_PI );

    constexpr double TWO_PI64 = 2.0 * M_PI;

    inline bool
    isPowerOf2(uint32_t x)
    {
        return x != 0 && (x & (x - 1)) == 0;
    }

    template <typename T>
    bool
    isPositiveInfinity(const T t)
    {
        return std::isinf(t) && !std::signbit(t);
    }

    template <typename T>
    bool
    isNegativeInfinity(const T t)
    {
        return std::isinf(t) && std::signbit(t);
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

    // Simple window functions:
    // TODO: Replace with stateful classes that contain precomputed lookup tables.

    inline void
    apply_hann_window(float* data, int32_t N)
    {
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.5f * (1.0f - cosf(2.0f * M_PI * n / (N - 1)));
            data[n] *= std::clamp(w, 0.0f, 1.0f);
        }
    }

    inline void
    apply_hamming_window(float* data, int32_t N)
    {
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.54f
                      - 0.46f * cosf(2.0f * M_PI * n / (N - 1));
            data[n] *= std::clamp(w, 0.0f, 1.0f);
        }
    }

    inline void
    apply_blackman_window(float* data, int32_t N)
    {
        for (int32_t n = 0; n < N; ++n)
        {
            float w = 0.42f
                      - 0.5f * cosf(2.0f * M_PI * n / (N - 1))
                      + 0.08f *cosf(4.0f * M_PI * n / (N - 1));
            data[n] *= std::clamp(w, 0.0f, 1.0f);
        }
    }

} // end namespace de.
