#include "de_FFT_Controller_PFFFT.h"

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

class FFT_AVX2 {
public:
    FFT_AVX2() : m_N(0) {}

    // Main FFT/IFFT entry point
    void transform(std::vector<float>& data, bool inverse) {
        // Ensure internal buffers and twiddles are initialized for this size
        init(data.size());

        // Step 1: Prepare input
        if (!inverse) {
            // Forward FFT: real input, zero imaginary part
            for (size_t i = 0; i < m_N; ++i) m_real[i] = data[i];
            std::fill(m_imag.begin(), m_imag.end(), 0.0f);
        } else {
            // Inverse FFT: unpack interleaved spectrum into full complex array
            m_real[0] = data[0];               // DC component
            m_real[m_N / 2] = data[1];         // Nyquist frequency
            for (size_t k = 1; k < m_N / 2; ++k) {
                m_real[k] = data[2 * k];       // Real part of bin k
                m_imag[k] = data[2 * k + 1];   // Imag part of bin k
                m_real[m_N - k] = data[2 * k];       // Real part of bin N-k (conjugate symmetry)
                m_imag[m_N - k] = -data[2 * k + 1];  // Imag part of bin N-k (conjugate symmetry)
            }
        }

        // Step 2: Bit reversal permutation
        // Reorders input for in-place Cooley-Tukey FFT
        for (size_t i = 1, j = 0; i < m_N; ++i) {
            size_t bit = m_N >> 1;
            while (j & bit) { j ^= bit; bit >>= 1; }
            j ^= bit;
            if (i < j) {
                std::swap(m_real[i], m_real[j]);
                std::swap(m_imag[i], m_imag[j]);
            }
        }

        // Step 3: Cooley-Tukey FFT stages
        // Each stage doubles the size of the butterfly blocks
        size_t twiddleOffset = 0;
        size_t stages = static_cast<size_t>(std::log2(m_N));

        for (size_t s = 0; s < stages; ++s) {
            size_t len = 1 << (s + 1);       // FFT block size for this stage
            size_t halfLen = len / 2;        // Number of butterflies per block

            for (size_t i = 0; i < m_N; i += len) { // i = start index of each FFT block
                for (size_t j = 0; j < halfLen; j += 8) { // j = butterfly index within block
                    Twiddle* tw = &m_twiddles[twiddleOffset + j]; // twiddle index = stage offset + butterfly index

                    // AVX2: Load 8 twiddle cos/sin values
                    __m256 wr = _mm256_loadu_ps(&tw[0].m_cos); // cos(θ)
                    __m256 wi = _mm256_loadu_ps(&tw[0].m_sin); // sin(θ)
                    if (inverse) wi = _mm256_sub_ps(_mm256_setzero_ps(), wi); // flip sign for IFFT

                    // AVX2: Load 8 complex values from top and bottom halves of butterfly
                    __m256 ur = _mm256_loadu_ps(&m_real[i + j]);             // upper real
                    __m256 ui = _mm256_loadu_ps(&m_imag[i + j]);             // upper imag
                    __m256 vr = _mm256_loadu_ps(&m_real[i + j + halfLen]);   // lower real
                    __m256 vi = _mm256_loadu_ps(&m_imag[i + j + halfLen]);   // lower imag

                    // FMA3: Complex multiplication of v * twiddle
                    // tr = vr * wr - vi * wi
                    __m256 tr = _mm256_fmsub_ps(vr, wr, _mm256_mul_ps(vi, wi));
                    // ti = vr * wi + vi * wr
                    __m256 ti = _mm256_fmadd_ps(vr, wi, _mm256_mul_ps(vi, wr));

                    // AVX2: Butterfly combination
                    // upper = u + twiddled v
                    _mm256_storeu_ps(&m_real[i + j], _mm256_add_ps(ur, tr));
                    _mm256_storeu_ps(&m_imag[i + j], _mm256_add_ps(ui, ti));
                    // lower = u - twiddled v
                    _mm256_storeu_ps(&m_real[i + j + halfLen], _mm256_sub_ps(ur, tr));
                    _mm256_storeu_ps(&m_imag[i + j + halfLen], _mm256_sub_ps(ui, ti));
                }
            }

            twiddleOffset += halfLen; // Advance into flat twiddle table
        }

        // Step 4: Output formatting
        if (!inverse) {
            // Forward FFT: pack result into interleaved format [DC, Nyquist, Re[1], Im[1], ...]
            data.resize(m_N);
            data[0] = m_real[0];
            data[1] = m_real[m_N / 2];
            for (size_t k = 1; k < m_N / 2; ++k) {
                data[2 * k]     = m_real[k];
                data[2 * k + 1] = m_imag[k];
            }
        } else {
            // Inverse FFT: normalize and return real part
            __m256 scale = _mm256_set1_ps(1.0f / m_N); // AVX2: broadcast scale factor
            for (size_t i = 0; i < m_N; i += 8) {
                __m256 r = _mm256_loadu_ps(&m_real[i]); // AVX2: load real part
                _mm256_storeu_ps(&data[i], _mm256_mul_ps(r, scale)); // AVX2: scale and store
            }
        }
    }

private:
    // Initializes or reinitializes buffers and twiddle tables
    void init(size_t fftSize) {
        if ((fftSize & (fftSize - 1)) != 0 || fftSize < 8)
            throw std::runtime_error("FFT size must be power of 2 and ≥ 8");

        if (fftSize == m_N) return; // Already initialized — skip

        m_N = fftSize;
        m_real.resize(m_N);
        m_imag.resize(m_N);

        // Flatten twiddle table: one contiguous block
        m_twiddles.clear();
        for (size_t len = 2; len <= m_N; len <<= 1) {
            size_t halfLen = len / 2;
            for (size_t j = 0; j < halfLen; ++j) {
                float angle = -2.0f * M_PI * j / len;
                m_twiddles.push_back({ cosf(angle), sinf(angle) });
            }
        }
    }

    size_t m_N; // FFT size
    std::vector<float> m_real; // Real part buffer
    std::vector<float> m_imag; // Imaginary part buffer
    std::vector<Twiddle> m_twiddles; // Flat twiddle table: stage-major, butterfly-minor
};


#endif
