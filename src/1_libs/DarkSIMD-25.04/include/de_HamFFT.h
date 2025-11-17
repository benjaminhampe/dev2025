#pragma once
#include <de/Core.h>
#include <de_AlignedVector.h>
#include <de_ForceInline.h>
#include <de_Approx_Math.h>

namespace de {

struct complexf // z = a + bi = r * e^(iφ) = r * (cos(φ) + i*sin(φ))
{
    float re;   // a = r * cos(φ)
    float im;   // b = r*i*sin(φ)
};

// Radix-2 FFT
struct HamFFT
{
    typedef TAlignedVector<complexf> ComplexVector;

    ComplexVector m_lut; // re = cos, im = sin;
    ComplexVector m_tmp; // FFT works inplace.

    bool init(uint32_t fftsize);


    // Let’s say:
    // samplerate = 48000 Hz
    // N = 1024 (your FFT size)

    // Then each bin represents:
    //  Δf=480001024≈46.875 Hz
    // So:
    //  out[0] → 0 Hz (DC)
    //  out[1] → 46.875 Hz
    //  out[2] → 93.75 Hz
    // ...
    //  out[512] → 24000 Hz (Nyquist)
    //  out[513] → -23953.125 Hz
    // ...
    //  out[1023] → -46.875 Hz

    bool fft();


    // Real valued fft
    bool fft(TAlignedVector<float> & out, const TAlignedVector<float>& in)
    {
        // std::vector<complexf> data(waveform.size());
        // for (size_t i = 0; i < waveform.size(); ++i) {
        //     data[i].re = waveform[i];  // real part from waveform
        //     data[i].im = 0.0f;         // imaginary part initialized to zero
        // }
        return true;
    }
};

#if 0
#include <immintrin.h>
#include <cmath>
#include <vector>
#include <cstdint>

struct complexf {
    float re;
    float im;
};

// Generate sine and cosine LUTs
void generate_twiddle_luts(std::vector<float>& sin_lut, std::vector<float>& cos_lut, int N) {
    sin_lut.resize(N / 2);
    cos_lut.resize(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        float angle = -2.0f * M_PI * i / N;
        sin_lut[i] = std::sinf(angle);
        cos_lut[i] = std::cosf(angle);
    }
}

// Convert real input to complex format
void real_to_complex(const std::vector<float>& input, std::vector<complexf>& output) {
    int N = static_cast<int>(input.size());
    output.resize(N);
    for (int i = 0; i < N; ++i) {
        output[i].re = input[i];
        output[i].im = 0.0f;
    }
}

// In-place radix-2 FFT using AVX2, FMA3, and LUTs
void fft_avx2_fma(std::vector<complexf>& data, const std::vector<float>& sin_lut, const std::vector<float>& cos_lut) {
    int N = static_cast<int>(data.size());
    int logN = static_cast<int>(std::log2(N));

    for (int s = 1; s <= logN; ++s) {
        int m = 1 << s;
        int m2 = m >> 1;
        int stride = N / m;

        for (int k = 0; k < N; k += m) {
            for (int j = 0; j < m2; j += 8) {
                int idx1 = k + j;
                int idx2 = idx1 + m2;

                __m256 a_re = _mm256_loadu_ps(&data[idx1].re);
                __m256 a_im = _mm256_loadu_ps(&data[idx1].im);
                __m256 b_re = _mm256_loadu_ps(&data[idx2].re);
                __m256 b_im = _mm256_loadu_ps(&data[idx2].im);

                __m256 tw_re = _mm256_set_ps(
                    cos_lut[j * stride + 7],
                    cos_lut[j * stride + 6],
                    cos_lut[j * stride + 5],
                    cos_lut[j * stride + 4],
                    cos_lut[j * stride + 3],
                    cos_lut[j * stride + 2],
                    cos_lut[j * stride + 1],
                    cos_lut[j * stride + 0]
                );
                __m256 tw_im = _mm256_set_ps(
                    sin_lut[j * stride + 7],
                    sin_lut[j * stride + 6],
                    sin_lut[j * stride + 5],
                    sin_lut[j * stride + 4],
                    sin_lut[j * stride + 3],
                    sin_lut[j * stride + 2],
                    sin_lut[j * stride + 1],
                    sin_lut[j * stride + 0]
                );

                // FMA3 complex multiply: t = B * W
                __m256 t_re = _mm256_fmsub_ps(b_re, tw_re, _mm256_mul_ps(b_im, tw_im)); // b_re * tw_re - b_im * tw_im
                __m256 t_im = _mm256_fmadd_ps(b_re, tw_im, _mm256_mul_ps(b_im, tw_re)); // b_re * tw_im + b_im * tw_re

                // Butterfly
                __m256 out1_re = _mm256_add_ps(a_re, t_re);
                __m256 out1_im = _mm256_add_ps(a_im, t_im);
                __m256 out2_re = _mm256_sub_ps(a_re, t_re);
                __m256 out2_im = _mm256_sub_ps(a_im, t_im);

                _mm256_storeu_ps(&data[idx1].re, out1_re);
                _mm256_storeu_ps(&data[idx1].im, out1_im);
                _mm256_storeu_ps(&data[idx2].re, out2_re);
                _mm256_storeu_ps(&data[idx2].im, out2_im);
            }
        }
    }
}

void ifft_avx2(std::vector<complexf>& data, const std::vector<float>& sin_lut, const std::vector<float>& cos_lut) {
    int N = static_cast<int>(data.size());
    int logN = static_cast<int>(std::log2(N));

    for (int s = 1; s <= logN; ++s) {
        int m = 1 << s;
        int m2 = m >> 1;
        int stride = N / m;

        for (int k = 0; k < N; k += m) {
            for (int j = 0; j < m2; j += 8) {
                int idx1 = k + j;
                int idx2 = idx1 + m2;

                __m256 a_re = _mm256_loadu_ps(&data[idx1].re);
                __m256 a_im = _mm256_loadu_ps(&data[idx1].im);
                __m256 b_re = _mm256_loadu_ps(&data[idx2].re);
                __m256 b_im = _mm256_loadu_ps(&data[idx2].im);

                // Flip sign of twiddle angle for inverse FFT
                __m256 tw_re = _mm256_set_ps(
                    cos_lut[j * stride + 7],
                    cos_lut[j * stride + 6],
                    cos_lut[j * stride + 5],
                    cos_lut[j * stride + 4],
                    cos_lut[j * stride + 3],
                    cos_lut[j * stride + 2],
                    cos_lut[j * stride + 1],
                    cos_lut[j * stride + 0]
                );
                __m256 tw_im = _mm256_set_ps(
                    -sin_lut[j * stride + 7],
                    -sin_lut[j * stride + 6],
                    -sin_lut[j * stride + 5],
                    -sin_lut[j * stride + 4],
                    -sin_lut[j * stride + 3],
                    -sin_lut[j * stride + 2],
                    -sin_lut[j * stride + 1],
                    -sin_lut[j * stride + 0]
                );

                // Complex multiply: t = B * W
                __m256 t_re = _mm256_sub_ps(_mm256_mul_ps(b_re, tw_re), _mm256_mul_ps(b_im, tw_im));
                __m256 t_im = _mm256_add_ps(_mm256_mul_ps(b_re, tw_im), _mm256_mul_ps(b_im, tw_re));

                // Butterfly
                __m256 out1_re = _mm256_add_ps(a_re, t_re);
                __m256 out1_im = _mm256_add_ps(a_im, t_im);
                __m256 out2_re = _mm256_sub_ps(a_re, t_re);
                __m256 out2_im = _mm256_sub_ps(a_im, t_im);

                _mm256_storeu_ps(&data[idx1].re, out1_re);
                _mm256_storeu_ps(&data[idx1].im, out1_im);
                _mm256_storeu_ps(&data[idx2].re, out2_re);
                _mm256_storeu_ps(&data[idx2].im, out2_im);
            }
        }
    }

    // Normalize by dividing all values by N
    __m256 scale = _mm256_set1_ps(1.0f / N);
    for (int i = 0; i < N; i += 8) {
        __m256 re = _mm256_loadu_ps(&data[i].re);
        __m256 im = _mm256_loadu_ps(&data[i].im);
        _mm256_storeu_ps(&data[i].re, _mm256_mul_ps(re, scale));
        _mm256_storeu_ps(&data[i].im, _mm256_mul_ps(im, scale));
    }
}

#endif

} // end namespace bhampe
