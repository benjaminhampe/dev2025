#pragma once
#include <de/fft/de_FFT_Windows.h>

#if 0

#include <immintrin.h>
#include <vector>
#include <cmath>
#include <stdexcept>

#pragma pack(push, 1)
struct Twiddle {
    float m_cos; // cos(θ)
    float m_sin; // sin(θ)
};
#pragma pack(pop)

class FFT_AVX512 {
public:
    FFT_AVX512() : m_N(0) {}

    void transform(std::vector<float>& data, bool inverse) {
        init(data.size());

        // Prepare input
        if (!inverse) {
            for (size_t i = 0; i < m_N; ++i) m_real[i] = data[i];
            std::fill(m_imag.begin(), m_imag.end(), 0.0f);
        } else {
            m_real[0] = data[0];
            m_real[m_N / 2] = data[1];
            for (size_t k = 1; k < m_N / 2; ++k) {
                m_real[k] = data[2 * k];
                m_imag[k] = data[2 * k + 1];
                m_real[m_N - k] = data[2 * k];
                m_imag[m_N - k] = -data[2 * k + 1];
            }
        }

        // Bit reversal
        for (size_t i = 1, j = 0; i < m_N; ++i) {
            size_t bit = m_N >> 1;
            while (j & bit) { j ^= bit; bit >>= 1; }
            j ^= bit;
            if (i < j) {
                std::swap(m_real[i], m_real[j]);
                std::swap(m_imag[i], m_imag[j]);
            }
        }

        // Cooley-Tukey FFT stages
        size_t twiddleOffset = 0;
        size_t stages = static_cast<size_t>(std::log2(m_N));

        for (size_t s = 0; s < stages; ++s) {
            size_t len = 1 << (s + 1);
            size_t halfLen = len / 2;

            for (size_t i = 0; i < m_N; i += len) {
                for (size_t j = 0; j < halfLen; j += 16) {
                    Twiddle* tw = &m_twiddles[twiddleOffset + j];

                    // AVX512F: Load 16 twiddle cos/sin values
                    __m512 wr = _mm512_loadu_ps(&tw[0].m_cos); // cos(θ)
                    __m512 wi = _mm512_loadu_ps(&tw[0].m_sin); // sin(θ)
                    if (inverse) wi = _mm512_sub_ps(_mm512_setzero_ps(), wi); // flip sign for IFFT

                    // AVX512F: Load 16 complex values from top and bottom halves of butterfly
                    __m512 ur = _mm512_loadu_ps(&m_real[i + j]);             // upper real
                    __m512 ui = _mm512_loadu_ps(&m_imag[i + j]);             // upper imag
                    __m512 vr = _mm512_loadu_ps(&m_real[i + j + halfLen]);   // lower real
                    __m512 vi = _mm512_loadu_ps(&m_imag[i + j + halfLen]);   // lower imag

                    // AVX512F + FMA: Complex multiplication of v * twiddle
                    __m512 tr = _mm512_fmsub_ps(vr, wr, _mm512_mul_ps(vi, wi)); // tr = vr * wr - vi * wi
                    __m512 ti = _mm512_fmadd_ps(vr, wi, _mm512_mul_ps(vi, wr)); // ti = vr * wi + vi * wr

                    // AVX512F: Butterfly combination
                    _mm512_storeu_ps(&m_real[i + j], _mm512_add_ps(ur, tr)); // upper real
                    _mm512_storeu_ps(&m_imag[i + j], _mm512_add_ps(ui, ti)); // upper imag
                    _mm512_storeu_ps(&m_real[i + j + halfLen], _mm512_sub_ps(ur, tr)); // lower real
                    _mm512_storeu_ps(&m_imag[i + j + halfLen], _mm512_sub_ps(ui, ti)); // lower imag
                }
            }

            twiddleOffset += halfLen;
        }

        // Output formatting
        if (!inverse) {
            data.resize(m_N);
            data[0] = m_real[0];
            data[1] = m_real[m_N / 2];
            for (size_t k = 1; k < m_N / 2; ++k) {
                data[2 * k]     = m_real[k];
                data[2 * k + 1] = m_imag[k];
            }
        } else {
            __m512 scale = _mm512_set1_ps(1.0f / m_N); // AVX512F: broadcast scale factor
            for (size_t i = 0; i < m_N; i += 16) {
                __m512 r = _mm512_loadu_ps(&m_real[i]); // AVX512F: load real part
                _mm512_storeu_ps(&data[i], _mm512_mul_ps(r, scale)); // AVX512F: scale and store
            }
        }
    }

private:
    void init(size_t fftSize) {
        if ((fftSize & (fftSize - 1)) != 0 || fftSize < 16)
            throw std::runtime_error("FFT size must be power of 2 and ≥ 16");

        if (fftSize == m_N) return;

        m_N = fftSize;
        m_real.resize(m_N);
        m_imag.resize(m_N);

        m_twiddles.clear();
        for (size_t len = 2; len <= m_N; len <<= 1) {
            size_t halfLen = len / 2;
            for (size_t j = 0; j < halfLen; ++j) {
                float angle = -2.0f * M_PI * j / len;
                m_twiddles.push_back({ cosf(angle), sinf(angle) });
            }
        }
    }

    size_t m_N;
    std::vector<float> m_real;
    std::vector<float> m_imag;
    std::vector<Twiddle> m_twiddles;
};

#endif
