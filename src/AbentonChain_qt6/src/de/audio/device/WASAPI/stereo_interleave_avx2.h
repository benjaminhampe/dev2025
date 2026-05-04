#include <immintrin.h>
#include <cstdint>

static inline void interleave_stereo_avx2(
    const float* __restrict L,
    const float* __restrict R,
    float* __restrict out,
    uint32_t frames)
{
    uint32_t i = 0;

    // Process 8 frames (16 floats) per iteration
    for (; i + 8 <= frames; i += 8) {
        __m256 l = _mm256_loadu_ps(L + i);
        __m256 r = _mm256_loadu_ps(R + i);

        // Interleave low halves
        __m256 lo = _mm256_unpacklo_ps(l, r);
        // Interleave high halves
        __m256 hi = _mm256_unpackhi_ps(l, r);

        // Permute into LRLRLRLR order
        __m256 out0 = _mm256_permute2f128_ps(lo, hi, 0x20);
        __m256 out1 = _mm256_permute2f128_ps(lo, hi, 0x31);

        _mm256_storeu_ps(out + (i * 2) + 0, out0);
        _mm256_storeu_ps(out + (i * 2) + 8, out1);
    }

    // Tail (scalar)
    for (; i < frames; i++) {
        out[i * 2 + 0] = L[i];
        out[i * 2 + 1] = R[i];
    }
}
