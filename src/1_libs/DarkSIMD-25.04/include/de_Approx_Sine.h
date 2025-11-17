#pragma once
#include <de_AlignedVector.h>

namespace de {

} // end namespace de.

/*

#include <immintrin.h>

__m256 k_vec = _mm256_set1_ps(k);
simd_fma_add_mul_kvec(c, a, b, k_vec, n);


void simd_fma_add_mul_kvec(float* c, const float* a, const float* b, __m256 k_vec, size_t n) {
    for (size_t i = 0; i + 7 < n; i += 8) {
        __m256 a_vec = _mm256_load_ps(a + i);
        __m256 b_vec = _mm256_load_ps(b + i);
        __m256 result = _mm256_fmadd_ps(b_vec, k_vec, a_vec);
        _mm256_store_ps(c + i, result);
    }
}

#include <immintrin.h>

void simd_add_mul(float* c, const float* a, const float* b, float k, size_t n) {
    __m256 k_vec = _mm256_set1_ps(k);
    size_t i = 0;

    for (; i + 7 < n; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(a + i);
        __m256 b_vec = _mm256_loadu_ps(b + i);
        __m256 result = _mm256_add_ps(a_vec, _mm256_mul_ps(b_vec, k_vec));
        _mm256_storeu_ps(c + i, result);
    }

    // Handle remaining elements
    for (; i < n; ++i) {
        c[i] = a[i] + b[i] * k;
    }
}



void simd_fma_add_mul(float* c, const float* a, const float* b, float k, size_t n) {
    __m256 k_vec = _mm256_set1_ps(k);
    size_t i = 0;

    for (; i + 7 < n; i += 8) {
        __m256 a_vec = _mm256_load_ps(a + i);   // aligned load
        __m256 b_vec = _mm256_load_ps(b + i);   // aligned load
        __m256 result = _mm256_fmadd_ps(b_vec, k_vec, a_vec); // FMA: b * k + a
        _mm256_store_ps(c + i, result);         // aligned store
    }

    // Handle remaining elements
    for (; i < n; ++i) {
        c[i] = a[i] + b[i] * k;
    }
}

*/
