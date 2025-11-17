#pragma once
#include <de/fft/de_FFT_Windows.h>

#if 0

#include <immintrin.h>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <cstddef>

#pragma pack(push, 1)
struct Twiddle {
    float m_cos;
    float m_sin;
};
#pragma pack(pop)

class FFT_AVX512 {
public:
    FFT_AVX512() : m_N(0), m_real(nullptr), m_imag(nullptr) {}

    void transform(std::vector<float>& data, bool inverse) {
        init(data.size());

        // Step 1: Prepare input
        if (!inverse) {
            std::copy(data.begin(), data.end(), m_real.get());
            std::fill(m_imag.get(), m_imag.get() + m_N, 0.0f);
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

        // Step 2: Bit reversal
        for (size_t i = 1, j = 0; i < m_N; ++i) {
            size_t bit = m_N >> 1;
            while (j & bit) { j ^= bit; bit >>= 1; }
            j ^= bit;
            if (i < j) {
                std::swap(m_real[i], m_real[j]);
                std::swap(m_imag[i], m_imag[j]);
            }
        }

        // Step 3: FFT stages
        size_t twiddleOffset = 0;
        size_t stages = static_cast<size_t>(std::log2(m_N));

        for (size_t s = 0; s < stages; ++s) {
            size_t len = 1 << (s + 1);
            size_t halfLen = len / 2;

            for (size_t i = 0; i < m_N; i += len) {
                for (size_t j = 0; j < halfLen; j += 16) {
                    Twiddle* tw = &m_twiddles[twiddleOffset + j];

                    // AVX512F: Load 16 twiddle cos/sin values
                    __m512 wr = _mm512_loadu_ps(&tw[0].m_cos);
                    __m512 wi = _mm512_loadu_ps(&tw[0].m_sin);
                    if (inverse) wi = _mm512_sub_ps(_mm512_setzero_ps(), wi);

                    // AVX512F: Load 16 complex values from top and bottom halves
                    __m512 ur = aligned_or_unaligned_load(&m_real[i + j]);
                    __m512 ui = aligned_or_unaligned_load(&m_imag[i + j]);
                    __m512 vr = aligned_or_unaligned_load(&m_real[i + j + halfLen]);
                    __m512 vi = aligned_or_unaligned_load(&m_imag[i + j + halfLen]);

                    // FMA: Complex multiplication
                    __m512 tr = _mm512_fmsub_ps(vr, wr, _mm512_mul_ps(vi, wi));
                    __m512 ti = _mm512_fmadd_ps(vr, wi, _mm512_mul_ps(vi, wr));

                    // AVX512F: Butterfly combination
                    aligned_or_unaligned_store(&m_real[i + j], _mm512_add_ps(ur, tr));
                    aligned_or_unaligned_store(&m_imag[i + j], _mm512_add_ps(ui, ti));
                    aligned_or_unaligned_store(&m_real[i + j + halfLen], _mm512_sub_ps(ur, tr));
                    aligned_or_unaligned_store(&m_imag[i + j + halfLen], _mm512_sub_ps(ui, ti));
                }
            }

            twiddleOffset += halfLen;
        }

        // Step 4: Output formatting
        if (!inverse) {
            data.resize(m_N);
            data[0] = m_real[0];
            data[1] = m_real[m_N / 2];
            for (size_t k = 1; k < m_N / 2; ++k) {
                data[2 * k]     = m_real[k];
                data[2 * k + 1] = m_imag[k];
            }
        } else {
            __m512 scale = _mm512_set1_ps(1.0f / m_N);
            for (size_t i = 0; i < m_N; i += 16) {
                __m512 r = aligned_or_unaligned_load(&m_real[i]);
                aligned_or_unaligned_store(&data[i], _mm512_mul_ps(r, scale));
            }
        }
    }

private:
    void init(size_t fftSize) {
        if ((fftSize & (fftSize - 1)) != 0 || fftSize < 16)
            throw std::runtime_error("FFT size must be power of 2 and ≥ 16");

        if (fftSize == m_N) return;

        m_N = fftSize;

        // Allocate aligned memory (64-byte for AVX-512)
        m_real = std::unique_ptr<float[], deleter>(_mm_malloc(m_N * sizeof(float), 64));
        m_imag = std::unique_ptr<float[], deleter>(_mm_malloc(m_N * sizeof(float), 64));

        m_twiddles.clear();
        for (size_t len = 2; len <= m_N; len <<= 1) {
            size_t halfLen = len / 2;
            for (size_t j = 0; j < halfLen; ++j) {
                float angle = -2.0f * M_PI * j / len;
                m_twiddles.push_back({ cosf(angle), sinf(angle) });
            }
        }
    }

    // Helper: aligned load if pointer is 64-byte aligned
    static __m512 aligned_or_unaligned_load(const float* ptr) {
        return (reinterpret_cast<uintptr_t>(ptr) % 64 == 0)
            ? _mm512_load_ps(ptr)       // AVX512F: aligned load
            : _mm512_loadu_ps(ptr);     // AVX512F: unaligned load
    }

    // Helper: aligned store if pointer is 64-byte aligned
    static void aligned_or_unaligned_store(float* ptr, __m512 value) {
        if (reinterpret_cast<uintptr_t>(ptr) % 64 == 0)
            _mm512_store_ps(ptr, value);    // AVX512F: aligned store
        else
            _mm512_storeu_ps(ptr, value);   // AVX512F: unaligned store
    }

    // Custom deleter for aligned memory
    static void deleter(float* p) { _mm_free(p); }

    size_t m_N;
    std::unique_ptr<float[], decltype(&deleter)> m_real;
    std::unique_ptr<float[], decltype(&deleter)> m_imag;
    std::vector<Twiddle> m_twiddles;
};

#endif
