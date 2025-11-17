#include <de_HamFFT.h>

#include <immintrin.h>
#include <cmath>
#include <vector>
// #include <cstdint>
// #include <iostream>
#include <cassert>

namespace de {

#if 0
std::vector<complexf> generate_twiddle_table(size_t N)
{
    std::vector<complexf> twiddles(N / 2);
    constexpr float PI = 3.14159265358979323846f;
    for (size_t k = 0; k < N / 2; ++k) {
        float angle = -2.0f * PI * k / N;
        twiddles[k].re = std::cos(angle);
        twiddles[k].im = std::sin(angle);
    }
    return twiddles;
}
#endif

bool HamFFT::init(uint32_t fftsize)
{
    if ( !isPowerOf2( fftsize ) )
    {
        DE_ERROR("Not a power of 2 fft size ", fftsize)
        return false;
    }

    m_lut.clear();
    m_lut.resize(fftsize);

    for (size_t i = 0; i < fftsize; ++i)
    {
        float angle = -float(TWO_PI) * float(i) / float(fftsize);
        m_lut[i].re = std::cosf(angle);
        m_lut[i].im = std::sinf(angle);
    }

    DE_BENNI("Init fft size = ", m_lut.size())
    return true;
}

// Multiply two __m256 registers containing 4 complexf values each
inline __m256 complexf_mul_avx2(__m256 a, __m256 b)
{
    // Extract real and imaginary parts from both operands
    // AVX2 stores complexf as [re0 im0 re1 im1 re2 im2 re3 im3]
    __m256 a_re = _mm256_shuffle_ps(a, a, 0xA0); // [re0 re1 re2 re3]
    __m256 a_im = _mm256_shuffle_ps(a, a, 0xF5); // [im0 im1 im2 im3]
    __m256 b_re = _mm256_shuffle_ps(b, b, 0xA0);
    __m256 b_im = _mm256_shuffle_ps(b, b, 0xF5);

    // Complex multiplication:
    // real = a.re * b.re - a.im * b.im
    // imag = a.re * b.im + a.im * b.re
    __m256 re = _mm256_sub_ps(_mm256_mul_ps(a_re, b_re), _mm256_mul_ps(a_im, b_im));
    __m256 im = _mm256_add_ps(_mm256_mul_ps(a_re, b_im), _mm256_mul_ps(a_im, b_re));

    // Interleave real and imaginary parts back into complexf layout
    return _mm256_unpacklo_ps(re, im);
}

// Perform radix-2 FFT using AVX2 intrinsics
bool
HamFFT::fft()
{
    size_t N = m_tmp.size();

    if ((N < 4) || !isPowerOf2(N))
    {
        throw std::invalid_argument("FFT size must be >= 4 and a power of 2.");
    }

    // if (m_out.size() != m_in.size())
    // {
    //     m_out.resize(m_in.size()); // Allocates only if needed
    // }

    // // Copy input to output buffer (in-place FFT)
    // std::memcpy(m_out.data(), m_in.data(), m_in.size() * sizeof(complexf)); // Fast copy, no reallocation

    // Bit reversal permutation: reorders input indices to prepare for in-place FFT
    // This is required for Cooley-Tukey algorithm to work correctly

    size_t logN = static_cast<size_t>(std::log2(N));
    for (size_t i = 0; i < N; ++i)
    {
        size_t rev = 0;
        for (size_t j = 0; j < logN; ++j)
        {
            // This line is part of a bit-reversal permutation,
            // which is a critical preprocessing step in radix-2 FFT.
            // It rearranges the input indices so that the FFT can be computed in-place efficiently.
            // Step-by-step:
            // 1.) (i >> j) & 1
            //      Extracts the j-th bit of i.
            //      Shifts i right by j bits, then masks with 1 to isolate that bit.
            // 2.)  << (logN - 1 - j)
            //      Places that bit in the reversed position.
            //      If the original bit was at position j, it now goes to position logN - 1 - j.
            // 3.)  rev |= ...
            //      Accumulates the reversed bits into rev using bitwise OR.
            rev |= ((i >> j) & 1) << (logN - 1 - j);
        }
        if (i < rev)
            std::swap(m_tmp[i], m_tmp[rev]);
    }

    // Cooley-Tukey FFT: iterate over log2(N) stages
    for (size_t s = 1; s <= logN; ++s)
    {
        size_t m = 1 << s;          // FFT block size at this stage
        size_t half_m = m >> 1;     // Half block size for butterfly
        size_t tw_stride = N / m;   // Twiddle factor stride for this stage

        // Process each FFT block of size m
        for (size_t k = 0; k < N; k += m)
        {
            // Process butterflies in groups of 4 complex numbers using AVX2
            for (size_t j = 0; j < half_m; j += 4)
            {
                size_t idx0 = k + j;           // Index of top half of butterfly
                size_t idx1 = idx0 + half_m;   // Index of bottom half of butterfly

                // Load 4 complex numbers from top and bottom halves
                // Each __m256 holds 4 complexf values (8 floats)
                __m256 u = _mm256_load_ps(reinterpret_cast<const float*>(&m_tmp[idx0]));
                __m256 v = _mm256_load_ps(reinterpret_cast<const float*>(&m_tmp[idx1]));

                // Load corresponding twiddle factors for this butterfly group
                // Twiddle factor: w = e^(-2πi * j / m)
                __m256 w = _mm256_load_ps(reinterpret_cast<const float*>(&m_lut[tw_stride * j]));

                // Compute t = v * w (twiddle multiplication)
                __m256 t = complexf_mul_avx2(v, w);

                // Butterfly operation:
                // out[idx0] = u + t
                // out[idx1] = u - t
                __m256 sum = _mm256_add_ps(u, t);
                __m256 diff = _mm256_sub_ps(u, t);

                // Store results back to output vector
                _mm256_store_ps(reinterpret_cast<float*>(&m_tmp[idx0]), sum);
                _mm256_store_ps(reinterpret_cast<float*>(&m_tmp[idx1]), diff);
            }
        }
    }

    // Final output: 'out' contains the FFT result in bit-reversed order
    // If needed, reorder again or use as-is depending on downstream usage

    return true;
}

} // end namespace de.

#if 0

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

// Returns bit-reversed index of i for log2(N) bits
size_t bit_reverse(size_t i, size_t logN) {
    size_t rev = 0;
    for (size_t j = 0; j < logN; ++j) {
        // Extract bit j from i and place it at position (logN - 1 - j)
        rev |= ((i >> j) & 1) << (logN - 1 - j);
    }
    return rev;
}

// Unit test for bit-reversal
void test_bit_reverse() {
    size_t N = 8;
    size_t logN = static_cast<size_t>(std::log2(N));

    std::vector<size_t> expected = {
        0, // 000 → 000
        4, // 001 → 100
        2, // 010 → 010
        6, // 011 → 110
        1, // 100 → 001
        5, // 101 → 101
        3, // 110 → 011
        7  // 111 → 111
    };

    for (size_t i = 0; i < N; ++i) {
        size_t rev = bit_reverse(i, logN);
        std::cout << "i = " << i << ", reversed = " << rev << std::endl;
        assert(rev == expected[i]);
    }

    std::cout << "Bit-reversal test passed for N = " << N << std::endl;
}



bool
HamFFT::fft(std::vector<complexf> & out, const std::vector<complexf>& in)
{
    if (in.size() != m_lut.size())
    {
        DE_ERROR("Mismatch in size ", in.size())
        return false;
    }

    int N = static_cast<int>(m_lut.size());
    int logN = static_cast<int>(std::log2(N));

    for (int s = 1; s <= logN; ++s)
    {
        int m = 1 << s;
        int m2 = m >> 1;
        int lut_stride = m_lut.size() / m;

        for (int k = 0; k < N; k += m)
        {
            for (int j = 0; j < m2; j += 8)
            {
                int idx1 = k + j;
                int idx2 = idx1 + m2;

                // Load A and B
                __m256 a_re = _mm256_loadu_ps(&in[idx1].re);
                __m256 a_im = _mm256_loadu_ps(&in[idx1].im);
                __m256 b_re = _mm256_loadu_ps(&in[idx2].re);
                __m256 b_im = _mm256_loadu_ps(&in[idx2].im);

                // Load twiddle factors from LUTs
                __m256 tw_re = _mm256_set_ps(
                    m_lut[j * lut_stride + 7].re,
                    m_lut[j * lut_stride + 6].re,
                    m_lut[j * lut_stride + 5].re,
                    m_lut[j * lut_stride + 4].re,
                    m_lut[j * lut_stride + 3].re,
                    m_lut[j * lut_stride + 2].re,
                    m_lut[j * lut_stride + 1].re,
                    m_lut[j * lut_stride + 0].re
                );
                __m256 tw_im = _mm256_set_ps(
                    m_lut[j * lut_stride + 7].im,
                    m_lut[j * lut_stride + 6].im,
                    m_lut[j * lut_stride + 5].im,
                    m_lut[j * lut_stride + 4].im,
                    m_lut[j * lut_stride + 3].im,
                    m_lut[j * lut_stride + 2].im,
                    m_lut[j * lut_stride + 1].im,
                    m_lut[j * lut_stride + 0].im
                );

                // Complex multiply: t = B * W
                __m256 t_re = _mm256_sub_ps(_mm256_mul_ps(b_re, tw_re), _mm256_mul_ps(b_im, tw_im));
                __m256 t_im = _mm256_add_ps(_mm256_mul_ps(b_re, tw_im), _mm256_mul_ps(b_im, tw_re));

                // Butterfly: A' = A + t, B' = A - t
                __m256 out1_re = _mm256_add_ps(a_re, t_re);
                __m256 out1_im = _mm256_add_ps(a_im, t_im);
                __m256 out2_re = _mm256_sub_ps(a_re, t_re);
                __m256 out2_im = _mm256_sub_ps(a_im, t_im);

                // Store results
                _mm256_storeu_ps(&out[idx1].re, out1_re);
                _mm256_storeu_ps(&out[idx1].im, out1_im);
                _mm256_storeu_ps(&out[idx2].re, out2_re);
                _mm256_storeu_ps(&out[idx2].im, out2_im);
            }
        }
    }
}


bool
FFT_Computer_AVX2_LUT::fft(FFT_Result & out, const std::vector<float>& waveform)
{
    if (waveform.size() != m_lut.size())
    {
        return false;
    }

    int N = static_cast<int>(waveform.size());
    int logN = static_cast<int>(std::log2(N));

    for (int s = 1; s <= logN; ++s) {
        int m = 1 << s;
        int m2 = m >> 1;
        int lut_stride = m_lut.size() / m;

        for (int k = 0; k < N; k += m) {
            for (int j = 0; j < m2; j += 8) {
                int idx1 = k + j;
                int idx2 = idx1 + m2;

                // Load A and B
                __m256 a_re = _mm256_loadu_ps(&data[idx1].re);
                __m256 a_im = _mm256_loadu_ps(&data[idx1].im);
                __m256 b_re = _mm256_loadu_ps(&data[idx2].re);
                __m256 b_im = _mm256_loadu_ps(&data[idx2].im);

                // Load twiddle factors from LUTs
                __m256 tw_re = _mm256_set_ps(
                    cos_lut[j * lut_stride + 7],
                    cos_lut[j * lut_stride + 6],
                    cos_lut[j * lut_stride + 5],
                    cos_lut[j * lut_stride + 4],
                    cos_lut[j * lut_stride + 3],
                    cos_lut[j * lut_stride + 2],
                    cos_lut[j * lut_stride + 1],
                    cos_lut[j * lut_stride + 0]
                    );
                __m256 tw_im = _mm256_set_ps(
                    sin_lut[j * lut_stride + 7],
                    sin_lut[j * lut_stride + 6],
                    sin_lut[j * lut_stride + 5],
                    sin_lut[j * lut_stride + 4],
                    sin_lut[j * lut_stride + 3],
                    sin_lut[j * lut_stride + 2],
                    sin_lut[j * lut_stride + 1],
                    sin_lut[j * lut_stride + 0]
                    );

                // Complex multiply: t = B * W
                __m256 t_re = _mm256_sub_ps(_mm256_mul_ps(b_re, tw_re), _mm256_mul_ps(b_im, tw_im));
                __m256 t_im = _mm256_add_ps(_mm256_mul_ps(b_re, tw_im), _mm256_mul_ps(b_im, tw_re));

                // Butterfly: A' = A + t, B' = A - t
                __m256 out1_re = _mm256_add_ps(a_re, t_re);
                __m256 out1_im = _mm256_add_ps(a_im, t_im);
                __m256 out2_re = _mm256_sub_ps(a_re, t_re);
                __m256 out2_im = _mm256_sub_ps(a_im, t_im);

                // Store results
                _mm256_storeu_ps(&data[idx1].re, out1_re);
                _mm256_storeu_ps(&data[idx1].im, out1_im);
                _mm256_storeu_ps(&data[idx2].re, out2_re);
                _mm256_storeu_ps(&data[idx2].im, out2_im);
            }
        }
    }
}

#endif
