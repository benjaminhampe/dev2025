#pragma once
#include <DarkImage.h>

DE_FORCE_INLINE void
DSP_RESIZE(de::TAlignedVector<float> & out, size_t n)
{
    if (n > 0 && out.size() != n)
    {
        out.resize(n);
    }
}

DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out)
{
    std::fill(out.begin(),out.end(),0.0f);
}

/*
DE_FORCE_INLINE void
DSP_ZERO(de::TAlignedVector<float> & out, size_t n, size_t start = 0)
{

    for (size_t i = start; i < n-start; i++)
    {
        out.at(i) = 0.0f;
    }
}
*/

DE_FORCE_INLINE void
DSP_ADD(float* __restrict__ pOut, const float* __restrict__ pIn, size_t n)
{
    DE_ASSUME_NO_OVERLAP(pOut,pIn,n*sizeof(float));

    for (size_t i = 0; i < n; i++)
    {
        pOut[i] += pIn[i];
    }
}




#if 0

#include <immintrin.h>
#include <cstddef>

// Fast sine approximation for __m256 using range reduction to [-pi, pi]
// sin(x) ≈ x + c3*x^3 + c5*x^5 + c7*x^7 (Taylor, decent for audio use)
static inline __m256 sin_ps(__m256 x) {
    // Constants
    const __m256 two_pi = _mm256_set1_ps(6.28318530717958647692f);
    const __m256 inv_two_pi = _mm256_set1_ps(1.0f / 6.28318530717958647692f);
    const __m256 pi = _mm256_set1_ps(3.14159265358979323846f);
    const __m256 c3 = _mm256_set1_ps(-1.0f / 6.0f);          // -0.16666667
    const __m256 c5 = _mm256_set1_ps(1.0f / 120.0f);         //  0.0083333337
    const __m256 c7 = _mm256_set1_ps(-1.0f / 5040.0f);       // -0.0001984127

    // Reduce x to y in [-pi, pi]: y = x - round(x / (2*pi)) * (2*pi)
    __m256 k = _mm256_round_ps(_mm256_mul_ps(x, inv_two_pi), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    __m256 y = _mm256_fnmadd_ps(k, two_pi, x); // y = x - k*2pi

    // Polynomial via Estrin
    __m256 y2 = _mm256_mul_ps(y, y);
    __m256 y3 = _mm256_mul_ps(y2, y);
    __m256 p = _mm256_fmadd_ps(c7, y2, c5);    // c5 + c7*y^2
    p = _mm256_fmadd_ps(p, y2, c3);            // c3 + y^2*(c5 + c7*y^2)
    p = _mm256_fmadd_ps(p, y3, y);             // y + y^3*(c3 + y^2*(c5 + c7*y^2))

    // Optional: improve accuracy near pi via sign flip (sine symmetry)
    // Map y to [-pi/2, pi/2] using quadrant, but omitted for speed.

    return p;
}

// Compute sample and Asum using AVX2
// m_amplitudes, m_phases, m_phaseIncrements are float arrays of size N
// de::TWO_PI = 2*pi
void process_block_avx2(const float* amplitudes,
                        float* phases,
                        const float* phaseIncrements,
                        size_t N,
                        float& outSample,
                        float& outAsum,
                        float two_pi /* de::TWO_PI */)
{
    const size_t vecWidth = 8;
    size_t i = 0;

    __m256 sample_acc = _mm256_set1_ps(0.0f);
    __m256 Asum_acc   = _mm256_set1_ps(0.0f);
    const __m256 TWO_PI = _mm256_set1_ps(two_pi);

    for (; i + vecWidth <= N; i += vecWidth) {
        // Load
        __m256 A   = _mm256_loadu_ps(amplitudes + i);
        __m256 P   = _mm256_loadu_ps(phases + i);
        __m256 dP  = _mm256_loadu_ps(phaseIncrements + i);

        // sample += A * sin(P)
        __m256 S = sin_ps(P);
        sample_acc = _mm256_fmadd_ps(A, S, sample_acc);

        // Asum += A
        Asum_acc = _mm256_add_ps(Asum_acc, A);

        // phases += dP
        P = _mm256_add_ps(P, dP);

        // if (P > TWO_PI) P -= TWO_PI;
        __m256 gt = _mm256_cmp_ps(P, TWO_PI, _CMP_GT_OQ);
        // subtract TWO_PI where gt is true
        __m256 adj = _mm256_and_ps(gt, TWO_PI);
        P = _mm256_sub_ps(P, adj);

        // Store back
        _mm256_storeu_ps(phases + i, P);
    }

    // Horizontal sum of accumulators
    auto hsum256 = [](const __m256 v) {
        __m128 low  = _mm256_castps256_ps128(v);
        __m128 high = _mm256_extractf128_ps(v, 1);
        __m128 sum128 = _mm_add_ps(low, high);
        // Horizontal sum 4 floats
        __m128 shuf = _mm_movehdup_ps(sum128);         // (b,d)
        __m128 sums = _mm_add_ps(sum128, shuf);        // (a+b, c+d)
        shuf = _mm_movehl_ps(shuf, sums);              // (c+d)
        sums = _mm_add_ss(sums, shuf);                 // (a+b+c+d, ...)
        return _mm_cvtss_f32(sums);
    };

    float sample_vec = hsum256(sample_acc);
    float Asum_vec   = hsum256(Asum_acc);

    // Scalar tail
    float sample_tail = 0.0f;
    float Asum_tail   = 0.0f;
    for (; i < N; ++i) {
        float A = amplitudes[i];
        float P = phases[i];

        sample_tail += A * sinf(P);
        Asum_tail   += A;

        P += phaseIncrements[i];
        if (P > two_pi) {
            P -= two_pi;
        }
        phases[i] = P;
    }

    outSample = sample_vec + sample_tail;
    outAsum   = Asum_vec + Asum_tail;
}

#endif



