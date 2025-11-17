#include "de_FFT_Controller_AVX2.h"

#if 0
#include <immintrin.h>
#include <vector>
#include <cmath>
#include <stdexcept>

void fft_real_avx2(std::vector<float>& data, bool inverse) {
    size_t N = data.size();
    if ((N & (N - 1)) != 0 || N < 8) throw std::runtime_error("Size must be power of 2 and ≥ 8");

    std::vector<float> real(N, 0.0f), imag(N, 0.0f);

    if (!inverse) {
        // Forward FFT: real input
        for (size_t i = 0; i < N; ++i) real[i] = data[i];
    } else {
        // IFFT: unpack PFFFT-style spectrum
        real[0] = data[0];
        real[N / 2] = data[1];
        for (size_t k = 1; k < N / 2; ++k) {
            real[k] = data[2 * k];
            imag[k] = data[2 * k + 1];
            real[N - k] = data[2 * k];
            imag[N - k] = -data[2 * k + 1];
        }
    }

    // Bit reversal
    for (size_t i = 1, j = 0; i < N; ++i) {
        size_t bit = N >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }

    // Cooley-Tukey with AVX2
    for (size_t len = 2; len <= N; len <<= 1) {
        float angle = (inverse ? 2.0f : -2.0f) * M_PI / len;
        float wlen_r = cosf(angle);
        float wlen_i = sinf(angle);
        for (size_t i = 0; i < N; i += len) {
            float wr = 1.0f, wi = 0.0f;
            for (size_t j = 0; j < len / 2; j += 8) {
                __m256 wr_v = _mm256_set1_ps(wr);
                __m256 wi_v = _mm256_set1_ps(wi);

                __m256 ur = _mm256_loadu_ps(&real[i + j]);
                __m256 ui = _mm256_loadu_ps(&imag[i + j]);
                __m256 vr = _mm256_loadu_ps(&real[i + j + len / 2]);
                __m256 vi = _mm256_loadu_ps(&imag[i + j + len / 2]);

                __m256 tr = _mm256_fmsub_ps(vr, wr_v, _mm256_mul_ps(vi, wi_v));
                __m256 ti = _mm256_fmadd_ps(vr, wi_v, _mm256_mul_ps(vi, wr_v));

                _mm256_storeu_ps(&real[i + j], _mm256_add_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j], _mm256_add_ps(ui, ti));
                _mm256_storeu_ps(&real[i + j + len / 2], _mm256_sub_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j + len / 2], _mm256_sub_ps(ui, ti));

                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    if (!inverse) {
        // Pack output: [DC, Nyquist, Re[1], Im[1], ..., Re[N/2-1], Im[N/2-1]]
        data.resize(N);
        data[0] = real[0];
        data[1] = real[N / 2];
        for (size_t k = 1; k < N / 2; ++k) {
            data[2 * k]     = real[k];
            data[2 * k + 1] = imag[k];
        }
    } else {
        // Normalize and return real part
        __m256 scale = _mm256_set1_ps(1.0f / N);
        for (size_t i = 0; i < N; i += 8) {
            __m256 r = _mm256_loadu_ps(&real[i]);
            _mm256_storeu_ps(&data[i], _mm256_mul_ps(r, scale));
        }
    }
}


#include <immintrin.h>
#include <vector>
#include <cmath>

void fft_real_avx2(const std::vector<float>& input, std::vector<float>& output) {
    size_t N = input.size();
    if ((N & (N - 1)) != 0 || N < 8) throw std::runtime_error("Size must be power of 2 and ≥ 8");

    std::vector<float> real = input;
    std::vector<float> imag(N, 0.0f);

    // Bit reversal
    for (size_t i = 1, j = 0; i < N; ++i) {
        size_t bit = N >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }

    // Cooley-Tukey with AVX2
    for (size_t len = 2; len <= N; len <<= 1) {
        float angle = -2.0f * M_PI / len;
        float wlen_r = cosf(angle);
        float wlen_i = sinf(angle);
        for (size_t i = 0; i < N; i += len) {
            float wr = 1.0f, wi = 0.0f;
            for (size_t j = 0; j < len / 2; j += 8) {
                __m256 wr_v = _mm256_set1_ps(wr);
                __m256 wi_v = _mm256_set1_ps(wi);

                __m256 ur = _mm256_loadu_ps(&real[i + j]);
                __m256 ui = _mm256_loadu_ps(&imag[i + j]);
                __m256 vr = _mm256_loadu_ps(&real[i + j + len / 2]);
                __m256 vi = _mm256_loadu_ps(&imag[i + j + len / 2]);

                __m256 tr = _mm256_sub_ps(_mm256_mul_ps(vr, wr_v), _mm256_mul_ps(vi, wi_v));
                __m256 ti = _mm256_add_ps(_mm256_mul_ps(vr, wi_v), _mm256_mul_ps(vi, wr_v));

                _mm256_storeu_ps(&real[i + j], _mm256_add_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j], _mm256_add_ps(ui, ti));
                _mm256_storeu_ps(&real[i + j + len / 2], _mm256_sub_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j + len / 2], _mm256_sub_ps(ui, ti));

                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    // Pack output: [DC, Nyquist, Re[1], Im[1], ..., Re[N/2-1], Im[N/2-1]]
    output.resize(N);
    output[0] = real[0];
    output[1] = real[N / 2];
    for (size_t k = 1; k < N / 2; ++k) {
        output[2 * k]     = real[k];
        output[2 * k + 1] = imag[k];
    }
}

void ifft_real_avx2(const std::vector<float>& spectrum, std::vector<float>& output) {
    size_t N = spectrum.size();
    if ((N & (N - 1)) != 0 || N < 8) throw std::runtime_error("Size must be power of 2 and ≥ 8");

    std::vector<float> real(N, 0.0f), imag(N, 0.0f);

    real[0] = spectrum[0];
    real[N / 2] = spectrum[1];
    for (size_t k = 1; k < N / 2; ++k) {
        real[k] = spectrum[2 * k];
        imag[k] = spectrum[2 * k + 1];
        real[N - k] = spectrum[2 * k];
        imag[N - k] = -spectrum[2 * k + 1];
    }

    // Bit reversal
    for (size_t i = 1, j = 0; i < N; ++i) {
        size_t bit = N >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }

    // Cooley-Tukey with AVX2
    for (size_t len = 2; len <= N; len <<= 1) {
        float angle = 2.0f * M_PI / len;
        float wlen_r = cosf(angle);
        float wlen_i = sinf(angle);
        for (size_t i = 0; i < N; i += len) {
            float wr = 1.0f, wi = 0.0f;
            for (size_t j = 0; j < len / 2; j += 8) {
                __m256 wr_v = _mm256_set1_ps(wr);
                __m256 wi_v = _mm256_set1_ps(wi);

                __m256 ur = _mm256_loadu_ps(&real[i + j]);
                __m256 ui = _mm256_loadu_ps(&imag[i + j]);
                __m256 vr = _mm256_loadu_ps(&real[i + j + len / 2]);
                __m256 vi = _mm256_loadu_ps(&imag[i + j + len / 2]);

                __m256 tr = _mm256_fmsub_ps(vr, wr_v, _mm256_mul_ps(vi, wi_v));
                __m256 ti = _mm256_fmadd_ps(vr, wi_v, _mm256_mul_ps(vi, wr_v));

                _mm256_storeu_ps(&real[i + j], _mm256_add_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j], _mm256_add_ps(ui, ti));
                _mm256_storeu_ps(&real[i + j + len / 2], _mm256_sub_ps(ur, tr));
                _mm256_storeu_ps(&imag[i + j + len / 2], _mm256_sub_ps(ui, ti));

                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    output.resize(N);
    __m256 scale = _mm256_set1_ps(1.0f / N);
    for (size_t i = 0; i < N; i += 8) {
        __m256 r = _mm256_loadu_ps(&real[i]);
        _mm256_storeu_ps(&output[i], _mm256_mul_ps(r, scale));
    }
}

#endif
