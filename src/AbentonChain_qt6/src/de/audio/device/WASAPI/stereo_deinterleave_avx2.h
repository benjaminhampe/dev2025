static inline void deinterleave_stereo_avx2(
    const float* __restrict in,
    float* __restrict L,
    float* __restrict R,
    uint32_t frames)
{
    uint32_t i = 0;

    for (; i + 8 <= frames; i += 8) {
        __m256 a = _mm256_loadu_ps(in + (i * 2) + 0);
        __m256 b = _mm256_loadu_ps(in + (i * 2) + 8);

        // Shuffle to separate L and R
        __m256 l = _mm256_shuffle_ps(a, b, _MM_SHUFFLE(2, 0, 2, 0));
        __m256 r = _mm256_shuffle_ps(a, b, _MM_SHUFFLE(3, 1, 3, 1));

        // Permute into contiguous vectors
        __m256 l2 = _mm256_permute2f128_ps(l, l, 0x20);
        __m256 r2 = _mm256_permute2f128_ps(r, r, 0x20);

        _mm256_storeu_ps(L + i, l2);
        _mm256_storeu_ps(R + i, r2);
    }

    // Tail
    for (; i < frames; i++) {
        L[i] = in[i * 2 + 0];
        R[i] = in[i * 2 + 1];
    }
}
