#include "de_fast_sine.h"

#include <immintrin.h>

/*
    Approximates sine using a 5th-order Taylor series expansion:

        sin(x) ≈ x - x³/6 + x⁵/120

    This is accurate for small x (typically |x| < π/2).
    Each term is computed using AVX2 intrinsics for parallel processing of 8 floats.
*/

__m256 fast_sin_avx(__m256 x)
{
    // Compute x²
    __m256 x2 = _mm256_mul_ps(x, x);  // x²

    // Compute x³ = x * x²
    __m256 x3 = _mm256_mul_ps(x2, x);  // x³
    // Compute -x³ / 6
    __m256 term3 = _mm256_mul_ps(x3, _mm256_set1_ps(-1.0f / 6.0f));

    // Compute x⁵ = x³ * x²
    __m256 x5 = _mm256_mul_ps(x3, x2);  // x⁵
    // Compute x⁵ / 120
    __m256 term5 = _mm256_mul_ps(x5, _mm256_set1_ps(1.0f / 120.0f));

    // Combine terms: x + term3 + term5
    __m256 result = _mm256_add_ps(x, term3);
    result = _mm256_add_ps(result, term5);

    return result;
}

/*
    Approximates sine using a 5th-order Taylor series:

        sin(x) ≈ x - x³/6 + x⁵/120

    Optimized with AVX2 and FMA3.
    Accurate for small x (typically after range reduction to [-π, π]).
*/

__m256 fast_sin_fma_avx(__m256 x)
{
    // x²
    __m256 x2 = _mm256_mul_ps(x, x);

    // x³ = x² * x
    __m256 x3 = _mm256_mul_ps(x2, x);

    // x⁵ = x³ * x²
    __m256 x5 = _mm256_mul_ps(x3, x2);

    // Compute: result = x - x³/6 + x⁵/120
    // Step 1: -x³/6 + x⁵/120
    __m256 term = _mm256_fmadd_ps(x5, _mm256_set1_ps(1.0f / 120.0f),
                    _mm256_mul_ps(x3, _mm256_set1_ps(-1.0f / 6.0f)));

    // Step 2: x + term
    __m256 result = _mm256_add_ps(x, term);

    return result;
}

void fast_sin7_avx2(const float* __restrict input, float* __restrict output) {
    __m256 x  = _mm256_loadu_ps(input);
    __m256 x2 = _mm256_mul_ps(x, x);         // x²
    __m256 x3 = _mm256_mul_ps(x2, x);        // x³
    __m256 x5 = _mm256_mul_ps(x3, x2);       // x⁵
    __m256 x7 = _mm256_mul_ps(x5, x2);       // x⁷

    // term = -x³/6 + x⁵/120 - x⁷/5040
    __m256 term = _mm256_mul_ps(x3, _mm256_set1_ps(-1.0f / 6.0f));
    term = _mm256_fmadd_ps(x5, _mm256_set1_ps(1.0f / 120.0f), term);
    term = _mm256_fmadd_ps(x7, _mm256_set1_ps(-1.0f / 5040.0f), term);

    __m256 result = _mm256_add_ps(x, term);
    _mm256_storeu_ps(output, result);
}

void fast_sin9_avx2(const float* __restrict input, float* __restrict output) {
    __m256 x  = _mm256_loadu_ps(input);
    __m256 x2 = _mm256_mul_ps(x, x);         // x²
    __m256 x3 = _mm256_mul_ps(x2, x);        // x³
    __m256 x5 = _mm256_mul_ps(x3, x2);       // x⁵
    __m256 x7 = _mm256_mul_ps(x5, x2);       // x⁷
    __m256 x9 = _mm256_mul_ps(x7, x2);       // x⁹

    // term = -x³/6 + x⁵/120 - x⁷/5040 + x⁹/362880
    __m256 term = _mm256_mul_ps(x3, _mm256_set1_ps(-1.0f / 6.0f));
    term = _mm256_fmadd_ps(x5, _mm256_set1_ps(1.0f / 120.0f), term);
    term = _mm256_fmadd_ps(x7, _mm256_set1_ps(-1.0f / 5040.0f), term);
    term = _mm256_fmadd_ps(x9, _mm256_set1_ps(1.0f / 362880.0f), term);

    __m256 result = _mm256_add_ps(x, term);
    _mm256_storeu_ps(output, result);
}

#if defined(__AVX512F__)
/*
    Approximates sine using a 5th-order Taylor series:

        sin(x) ≈ x - x³/6 + x⁵/120

    Optimized for AVX-512F with FMA3.
    Processes 16 floats in parallel.
    Accurate for small x (typically after range reduction to [-π, π]).

__m512 fast_sin_fma_avx512(__m512 x)
{
    // x²
    __m512 x2 = _mm512_mul_ps(x, x);

    // x³ = x² * x
    __m512 x3 = _mm512_mul_ps(x2, x);

    // x⁵ = x³ * x²
    __m512 x5 = _mm512_mul_ps(x3, x2);

    // Compute: result = x - x³/6 + x⁵/120
    // Step 1: -x³/6 + x⁵/120 using FMA
    __m512 term = _mm512_fmadd_ps(x5, _mm512_set1_ps(1.0f / 120.0f),
                    _mm512_mul_ps(x3, _mm512_set1_ps(-1.0f / 6.0f)));

    // Step 2: x + term
    __m512 result = _mm512_add_ps(x, term);

    return result;
}
*/


void fast_sin_fma_avx512(const float* __restrict input, float* __restrict output)
{
    // Load 16 floats from input (aligned)
    __m512 x = _mm512_load_ps(input);

    // Compute powers
    __m512 x2 = _mm512_mul_ps(x, x);       // x²
    __m512 x3 = _mm512_mul_ps(x2, x);      // x³
    __m512 x5 = _mm512_mul_ps(x3, x2);     // x⁵

    // Compute: -x³/6 + x⁵/120
    __m512 term = _mm512_fmadd_ps(x5, _mm512_set1_ps(1.0f / 120.0f),
                    _mm512_mul_ps(x3, _mm512_set1_ps(-1.0f / 6.0f)));

    // Final result: x + term
    __m512 result = _mm512_add_ps(x, term);

    // Store result to output (aligned)
    _mm512_store_ps(output, result);
}

void fast_sin7_avx512_unaligned(const float* __restrict input, float* __restrict output)
{
    __m512 x  = _mm512_loadu_ps(input);
    __m512 x2 = _mm512_mul_ps(x, x);         // x²
    __m512 x3 = _mm512_mul_ps(x2, x);        // x³
    __m512 x5 = _mm512_mul_ps(x3, x2);       // x⁵
    __m512 x7 = _mm512_mul_ps(x5, x2);       // x⁷

    // term = -x³/6 + x⁵/120 - x⁷/5040
    __m512 term = _mm512_mul_ps(x3, _mm512_set1_ps(-1.0f / 6.0f));
    term = _mm512_fmadd_ps(x5, _mm512_set1_ps(1.0f / 120.0f), term);
    term = _mm512_fmadd_ps(x7, _mm512_set1_ps(-1.0f / 5040.0f), term);

    __m512 result = _mm512_add_ps(x, term);
    _mm512_storeu_ps(output, result);
}

void fast_sin9_avx512_unaligned(const float* __restrict input, float* __restrict output)
{
    __m512 x  = _mm512_loadu_ps(input);
    __m512 x2 = _mm512_mul_ps(x, x);         // x²
    __m512 x3 = _mm512_mul_ps(x2, x);        // x³
    __m512 x5 = _mm512_mul_ps(x3, x2);       // x⁵
    __m512 x7 = _mm512_mul_ps(x5, x2);       // x⁷
    __m512 x9 = _mm512_mul_ps(x7, x2);       // x⁹

    // term = -x³/6 + x⁵/120 - x⁷/5040 + x⁹/362880
    __m512 term = _mm512_mul_ps(x3, _mm512_set1_ps(-1.0f / 6.0f));
    term = _mm512_fmadd_ps(x5, _mm512_set1_ps(1.0f / 120.0f), term);
    term = _mm512_fmadd_ps(x7, _mm512_set1_ps(-1.0f / 5040.0f), term);
    term = _mm512_fmadd_ps(x9, _mm512_set1_ps(1.0f / 362880.0f), term);

    __m512 result = _mm512_add_ps(x, term);
    _mm512_storeu_ps(output, result);
}

#endif
