#pragma once
#include <de/fft/de_FFT_Windows.h>

#include <vector>
#include <cmath>
#include <stdexcept>

class HannWindowGeneric {
public:
    HannWindowGeneric() : m_N(0) {}

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

    // Apply window to input/output vector
    void apply(std::vector<float>& data) const {
        if (data.size() != m_N)
            throw std::runtime_error("Input size must match initialized Hann window size");

        for (size_t i = 0; i < m_N; ++i) {
            data[i] *= m_window[i];
        }
    }

    const std::vector<float>& window() const {
        return m_window;
    }

private:
    size_t m_N;
    std::vector<float> m_window;
};
