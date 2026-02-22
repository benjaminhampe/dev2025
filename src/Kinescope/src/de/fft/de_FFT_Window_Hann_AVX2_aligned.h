#pragma once
#include <de/fft/de_FFT_Windows.h>

#if 0

#include <vector>
#include <cmath>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <immintrin.h> // For AVX2

class HannWindowAVX2 {
public:
    HannWindowAVX2() : m_N(0) {}

    // Ensure window is computed for given size
    void init(size_t size) {
        if (size < 2)
            throw std::runtime_error("Hann window size must be ≥ 2");

        if (size == m_N) return;

        m_N = size;
        m_window.resize(m_N);

        // Compute Hann window: w[n] = 0.5 * (1 - cos(2πn / (N - 1)))
        for (size_t n = 0; n < m_N; ++n) {
            float phase = 2.0f * static_cast<float>(M_PI) * n / (m_N - 1);
            m_window[n] = 0.5f * (1.0f - std::cos(phase));
        }
    }

    // Apply window to aligned input/output vector
    void apply(std::vector<float, std::aligned_allocator<float, 32>>& data) const {
        if (data.size() != m_N)
            throw std::runtime_error("Input size must match initialized Hann window size");

        if (reinterpret_cast<uintptr_t>(data.data()) % 32 != 0)
            throw std::runtime_error("Input vector must be 32-byte aligned");

        const float* w = m_window.data();
        float* d = data.data();

        size_t i = 0;
        const size_t step = 8; // AVX2 processes 8 floats at a time

        // AVX2: Apply window in chunks of 8 floats
        for (; i + step <= m_N; i += step) {
            __m256 win = _mm256_load_ps(&w[i]);   // AVX2: aligned load
            __m256 val = _mm256_load_ps(&d[i]);   // AVX2: aligned load
            __m256 out = _mm256_mul_ps(val, win); // AVX2: multiply
            _mm256_store_ps(&d[i], out);          // AVX2: aligned store
        }

        // Handle remaining tail (if any)
        for (; i < m_N; ++i) {
            d[i] *= w[i];
        }
    }

    const std::vector<float, std::aligned_allocator<float, 32>>& window() const {
        return m_window;
    }

private:
    size_t m_N;
    std::vector<float, std::aligned_allocator<float, 32>> m_window;
};

#endif
