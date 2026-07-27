#include <de/sound/SampleTypeConverter.h>

namespace de {

namespace {

    inline float my_clampf(float x, float lo, float hi)
    {
        return fminf(fmaxf(x, lo), hi);
    }

    inline double my_clampd(double x, double lo, double hi)
    {
        return fmin(fmax(x, lo), hi);
    }

    inline float my_absf(float x)
    {
        uint32_t bits;
        memcpy(&bits, &x, sizeof(bits));
        bits &= 0x7fffffff;
        memcpy(&x, &bits, sizeof(x));
        return x;
    }

} // namespace.

// static
SampleTypeConverter::Converter_t
SampleTypeConverter::getConverter( const SampleType& src, const SampleType& dst )
{
    if ( src == SampleType::U8 )
    {
             if ( dst == SampleType::U8 )  { return convert_U8_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_U8_to_S8; }
     // else if ( dst == SampleType::S16 ) { return convert_U8_to_S16; }
     // else if ( dst == SampleType::S24 ) { return convert_U8_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_U8_to_S32; }
     // else if ( dst == SampleType::F32 ) { return convert_U8_to_F32; }
     // else if ( dst == SampleType::F64 ) { return convert_U8_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::S8 )
    {
             if ( dst == SampleType::U8 )  { return convert_S8_to_U8; }
        else if ( dst == SampleType::S8 )  { return convert_S8_to_S8; }
     // else if ( dst == SampleType::S16 ) { return convert_S8_to_S16; }
     // else if ( dst == SampleType::S24 ) { return convert_S8_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_S8_to_S32; }
     // else if ( dst == SampleType::F32 ) { return convert_S8_to_F32; }
     // else if ( dst == SampleType::F64 ) { return convert_S8_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::S16 )
    {
     // else if ( dst == SampleType::U8 )  { return convert_S16_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_S16_to_S8; }
             if ( dst == SampleType::S16 ) { return convert_S16_to_S16; }
     // else if ( dst == SampleType::S24 ) { return convert_S16_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_S16_to_S32; }
        else if ( dst == SampleType::F32 ) { return convert_S16_to_F32; }
     // else if ( dst == SampleType::F64 ) { return convert_S16_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::S24 )
    {
     // else if ( dst == SampleType::U8 )  { return convert_S24_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_S24_to_S8; }
     // else if ( dst == SampleType::S16 ) { return convert_S24_to_S16; }
             if ( dst == SampleType::S24 ) { return convert_S24_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_S24_to_S32; }
        else if ( dst == SampleType::F32 ) { return convert_S24_to_F32; }
     // else if ( dst == SampleType::F64 ) { return convert_S24_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::S32 )
    {
     // else if ( dst == SampleType::U8 )  { return convert_S32_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_S32_to_S8; }
     // else if ( dst == SampleType::S16 ) { return convert_S32_to_S16; }
     // else if ( dst == SampleType::S24 ) { return convert_S32_to_S24; }
             if ( dst == SampleType::S32 ) { return convert_S32_to_S32; }
     // else if ( dst == SampleType::F32 ) { return convert_S32_to_F32; }
     // else if ( dst == SampleType::F64 ) { return convert_S32_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::F32 )
    {
     // else if ( dst == SampleType::U8 )  { return convert_F32_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_F32_to_S8; }
             if ( dst == SampleType::S16 ) { return convert_F32_to_S16; }
        else if ( dst == SampleType::S24 ) { return convert_F32_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_F32_to_S32; }
        else if ( dst == SampleType::F32 ) { return convert_F32_to_F32; }
        else if ( dst == SampleType::F64 ) { return convert_F32_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else if ( src == SampleType::F64 )
    {
     // else if ( dst == SampleType::U8 )  { return convert_F64_to_U8; }
     // else if ( dst == SampleType::S8 )  { return convert_F64_to_S8; }
     // else if ( dst == SampleType::S16 ) { return convert_F64_to_S16; }
     // else if ( dst == SampleType::S24 ) { return convert_F64_to_S24; }
     // else if ( dst == SampleType::S32 ) { return convert_F64_to_S32; }
             if ( dst == SampleType::F32 ) { return convert_F64_to_F32; }
        else if ( dst == SampleType::F64 ) { return convert_F64_to_F64; }
        else
        {
            auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
            DE_ERROR(s)
            return nullptr;
        }
    }
    else
    {
        auto s = dbStr("No converter from(",src.str(),") _to_ (",dst.str(),")");
        DE_ERROR(s)
        return nullptr;
    }
}

// Dummy converters:

// static
void SampleTypeConverter::convert_U8_to_U8( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(uint8_t) );
}

// static
void SampleTypeConverter::convert_S8_to_S8( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(int8_t) );
}

// static
void SampleTypeConverter::convert_S16_to_S16( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(int16_t) );
}

// static
void SampleTypeConverter::convert_S24_to_S24( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * 3 );
}

// static
void SampleTypeConverter::convert_S32_to_S32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(int32_t) );
}

// static
void SampleTypeConverter::convert_F32_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(float) );
}

// static
void SampleTypeConverter::convert_F64_to_F64( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    std::memcpy( dst, src, nSamples * sizeof(double) );
}

// F32 <=====> F64

// static
void SampleTypeConverter::convert_F32_to_F64( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const float* __restrict__ pSrc = (const float*)src;
         double* __restrict__ pDst = (double*)dst;

    for (int64_t i = 0; i < nSamples; ++i)
    {
        pDst[i] = static_cast<double>(pSrc[i]);
    }
}

// static
void SampleTypeConverter::convert_F64_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const double* __restrict__ pSrc = (const double*)src;
           float* __restrict__ pDst = (float*)dst;

    for (int64_t i = 0; i < nSamples; ++i)
    {
        pDst[i] = static_cast<float>(pSrc[i]);
    }
}


// F32 <=====> S16

// static
void SampleTypeConverter::convert_F32_to_S16( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const float* __restrict__ in = reinterpret_cast<const float*>(src);
    int16_t*    __restrict__ out = reinterpret_cast<int16_t*>(dst);

    constexpr static const float SCALE_F32_TO_S16 = 32767.0f;

    for (int64_t i = 0; i < nSamples; ++i)
    {
        float x = my_clampf(in[i], -1.0f, 1.0f);
        out[i] = int16_t(x * SCALE_F32_TO_S16);
    }
#if 0
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Broadcast constants into 8-wide SIMD registers
    const __m256 scale = _mm256_set1_ps(32767.0f); // multiply float [-1,1] → int16 range
    const __m256 minv  = _mm256_set1_ps(-1.0f);    // clamp lower bound
    const __m256 maxv  = _mm256_set1_ps(1.0f);     // clamp upper bound

    size_t i = 0;
    for (; i + 8 <= nSamples; i += 8)
    {
        // Load 8 floats: x0 x1 x2 x3 x4 x5 x6 x7
        __m256 x = _mm256_loadu_ps(in + i);

        // x = min(x, 1.0f)
        // SIMD min: elementwise min(x[j], maxv[j])
        x = _mm256_min_ps(x, maxv);

        // x = max(x, -1.0f)
        // SIMD max: elementwise max(x[j], minv[j])
        x = _mm256_max_ps(x, minv);

        // x = x * 32767.0f
        // SIMD multiply: elementwise scaling into int16 range
        x = _mm256_mul_ps(x, scale);

        // Convert 8 floats → 8 int32 using truncation
        // cvttps2dq: float → int32, truncates toward zero
        __m256i xi = _mm256_cvtps_epi32(x);

        // Now xi contains 8 int32 values:
        // [i0 i1 i2 i3 i4 i5 i6 i7]

        // Split 256-bit register into two 128-bit halves
        __m128i lo = _mm256_castsi256_si128(xi);        // i0 i1 i2 i3
        __m128i hi = _mm256_extracti128_si256(xi, 1);   // i4 i5 i6 i7

        // Pack 8 int32 → 8 int16 with saturation
        // packssdw:
        //   - takes two 4×int32 vectors
        //   - converts each int32 → int16
        //   - clamps to [-32768, 32767]
        //   - produces 8×int16
        __m128i packed = _mm_packs_epi32(lo, hi);

        // Store 8 int16 values
        _mm_storeu_si128((__m128i*)(out + i), packed);
    }

    // Scalar tail
    for (; i < nSamples; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(32767.0f);
    const __m128 minv  = _mm_set1_ps(-1.0f);
    const __m128 maxv  = _mm_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 4 <= nSamples; i += 4)
    {
        // Load 4 floats
        __m128 x = _mm_loadu_ps(in + i);

        // Clamp to [-1,1]
        x = _mm_min_ps(x, maxv);
        x = _mm_max_ps(x, minv);

        // Scale into int16 range
        x = _mm_mul_ps(x, scale);

        // Convert 4 floats → 4 int32
        __m128i xi = _mm_cvtps_epi32(x);

        // Pack 4 int32 → 4 int16 (upper half ignored)
        __m128i packed = _mm_packs_epi32(xi, xi);

        // Store lower 64 bits (4×int16)
        _mm_storel_epi64((__m128i*)(out + i), packed);
    }

    // Scalar tail
    for (; i < nSamples; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < nSamples; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }
#endif
#endif
}

// static
void SampleTypeConverter::convert_S16_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const int16_t* __restrict__ in = reinterpret_cast<const int16_t*>(src);
    float*        __restrict__ out = reinterpret_cast<float*>(dst);

    constexpr static const float SCALE_S16_TO_F32 = (1.0f / 32767.0f);

    for (int64_t i = 0; i < nSamples; ++i)
    {
        out[i] = SCALE_S16_TO_F32 * float(in[i]);
    }

#if 0
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 16 samples per loop
    // ============================================================

    // Scale factor: 1 / 32767.0f
    // Converts int16 [-32768, 32767] → float [-1,1]
    const __m256 scale = _mm256_set1_ps(1.0f / 32767.0f);

    size_t i = 0;
    for (; i + 16 <= nSamples; i += 16)
    {
        // Load 16 int16 samples (256 bits)
        __m256i raw16 = _mm256_loadu_si256((__m256i*)(in + i));

        // Unpack 16×int16 → 16×int32
        // unpack low 8 samples
        __m256i lo = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(raw16));
        // unpack high 8 samples
        __m256i hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(raw16, 1));

        // Convert int32 → float32
        __m256 flo = _mm256_cvtepi32_ps(lo);
        __m256 fhi = _mm256_cvtepi32_ps(hi);

        // Scale into [-1,1]
        flo = _mm256_mul_ps(flo, scale);
        fhi = _mm256_mul_ps(fhi, scale);

        // Store 16 floats
        _mm256_storeu_ps(out + i, flo);
        _mm256_storeu_ps(out + i + 8, fhi);
    }

    // Scalar tail
    for (; i < nSamples; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 8 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(1.0f / 32767.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8 int16 samples (128 bits)
        __m128i raw16 = _mm_loadu_si128((__m128i*)(in + i));

        // Unpack 8×int16 → 8×int32
        __m128i lo = _mm_unpacklo_epi16(raw16, _mm_setzero_si128());
        __m128i hi = _mm_unpackhi_epi16(raw16, _mm_setzero_si128());

        // Sign-extend int16 → int32
        lo = _mm_srai_epi32(lo, 16);
        hi = _mm_srai_epi32(hi, 16);

        // Convert int32 → float32
        __m128 flo = _mm_cvtepi32_ps(lo);
        __m128 fhi = _mm_cvtepi32_ps(hi);

        // Scale into [-1,1]
        flo = _mm_mul_ps(flo, scale);
        fhi = _mm_mul_ps(fhi, scale);

        // Store 8 floats
        _mm_storeu_ps(out + i, flo);
        _mm_storeu_ps(out + i + 4, fhi);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }
#endif
#endif
}



// F32 <=====> S24

// static
void SampleTypeConverter::convert_F32_to_S24( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const float* __restrict__ in = reinterpret_cast<const float*>(src);
    uint8_t*    __restrict__ out = reinterpret_cast<uint8_t*>(dst);

    constexpr static const float SCALE_F32_TO_S24 = 8388607.0f;

    for (int64_t i = 0; i < nSamples; ++i)
    {
        float x = my_clampf(in[i], -1.0f, 1.0f);
        int32_t v = int32_t(x * SCALE_F32_TO_S24);

        out[0] = (uint8_t)(v & 0xFF);
        out[1] = (uint8_t)((v >> 8) & 0xFF);
        out[2] = (uint8_t)((v >> 16) & 0xFF);
        out += 3;
    }

#if 0
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Broadcast constants:
    // 8388607.0f = max positive int24 value (0x7FFFFF)
    const __m256 scale = _mm256_set1_ps(8388607.0f);
    const __m256 minv  = _mm256_set1_ps(-1.0f);
    const __m256 maxv  = _mm256_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8 floats
        __m256 x = _mm256_loadu_ps(in + i);

        // Clamp to [-1,1]
        x = _mm256_min_ps(x, maxv);   // x[j] = min(x[j], 1.0)
        x = _mm256_max_ps(x, minv);   // x[j] = max(x[j], -1.0)

        // Scale into int24 range
        // x[j] = x[j] * 8388607.0f
        x = _mm256_mul_ps(x, scale);

        // Convert float → int32 (truncate toward zero)
        // cvttps2dq: elementwise float → int32
        __m256i xi = _mm256_cvtps_epi32(x);

        // Split 8×int32 into two 128-bit halves
        __m128i lo = _mm256_castsi256_si128(xi);        // i0 i1 i2 i3
        __m128i hi = _mm256_extracti128_si256(xi, 1);   // i4 i5 i6 i7

        // Store each int32 as 3 bytes (little-endian)
        int32_t tmp[8];
        _mm_storeu_si128((__m128i*)tmp, lo);
        _mm_storeu_si128((__m128i*)(tmp + 4), hi);

        uint8_t* dst = out + i * 3;

        // Manual pack: int32 → 3 bytes
        // tmp[k] & 0xFFFFFF gives 24-bit signed PCM
        for (int k = 0; k < 8; ++k)
        {
            int32_t v = tmp[k];

            // Little-endian 24-bit:
            // byte0 = LSB
            // byte1 = mid
            // byte2 = MSB
            dst[k*3 + 0] = (uint8_t)(v & 0xFF);
            dst[k*3 + 1] = (uint8_t)((v >> 8) & 0xFF);
            dst[k*3 + 2] = (uint8_t)((v >> 16) & 0xFF);
        }
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(8388607.0f);
    const __m128 minv  = _mm_set1_ps(-1.0f);
    const __m128 maxv  = _mm_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        __m128 x = _mm_loadu_ps(in + i);

        x = _mm_min_ps(x, maxv);
        x = _mm_max_ps(x, minv);

        x = _mm_mul_ps(x, scale);

        __m128i xi = _mm_cvtps_epi32(x);

        int32_t tmp[4];
        _mm_storeu_si128((__m128i*)tmp, xi);

        uint8_t* dst = out + i * 3;

        for (int k = 0; k < 4; ++k)
        {
            int32_t v = tmp[k];
            dst[k*3 + 0] = (uint8_t)(v & 0xFF);
            dst[k*3 + 1] = (uint8_t)((v >> 8) & 0xFF);
            dst[k*3 + 2] = (uint8_t)((v >> 16) & 0xFF);
        }
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }
#endif
#endif
}

// static
void SampleTypeConverter::convert_S24_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const uint8_t* __restrict__ in = reinterpret_cast<const uint8_t*>(src);
    float*        __restrict__ out = reinterpret_cast<float*>(dst);

    constexpr static const float SCALE_S24_TO_F32 = (1.0f / 8388607.0f);

    // ============================================================
    // Scalar fallback
    // ============================================================
    for (int64_t i = 0; i < nSamples; ++i)
    {
        int32_t v =
            (int32_t(in[0])      ) |
            (int32_t(in[1]) << 8 ) |
            (int32_t(in[2]) << 16);

        if (v & 0x800000) { v |= ~0xFFFFFF; }

        in += 3;
        out[i] = float(v) * SCALE_S24_TO_F32;
    }
#if 0
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Scale factor: 1 / 8388607.0f
    // Converts int24 [-8388608, 8388607] → float [-1,1]
    const __m256 scale = _mm256_set1_ps(1.0f / 8388607.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8×24-bit samples → unpack into 8×int32
        // We must manually unpack because AVX2 has no 24-bit load.

        int32_t tmp[8];

        const uint8_t* src = in + i * 3;

        for (int k = 0; k < 8; ++k)
        {
            // Little-endian 24-bit:
            // byte0 = LSB
            // byte1 = mid
            // byte2 = MSB
            int32_t v =
                (int32_t(src[k*3 + 0])      ) |
                (int32_t(src[k*3 + 1]) << 8 ) |
                (int32_t(src[k*3 + 2]) << 16);

            // Sign-extend from 24-bit to 32-bit:
            // If MSB (bit 23) is set → negative number
            if (v & 0x800000)
                v |= ~0xFFFFFF;  // fill upper bits with 1s

            tmp[k] = v;
        }

        // Load 8 int32 values into AVX2 register
        __m256i xi = _mm256_loadu_si256((__m256i*)tmp);

        // Convert int32 → float32
        // cvtdq2ps: elementwise int32 → float32
        __m256 xf = _mm256_cvtepi32_ps(xi);

        // Scale into [-1,1]
        xf = _mm256_mul_ps(xf, scale);

        // Store 8 floats
        _mm256_storeu_ps(out + i, xf);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(1.0f / 8388607.0f);

    size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        int32_t tmp[4];

        const uint8_t* src = in + i * 3;

        for (int k = 0; k < 4; ++k)
        {
            int32_t v =
                (int32_t(src[k*3 + 0])      ) |
                (int32_t(src[k*3 + 1]) << 8 ) |
                (int32_t(src[k*3 + 2]) << 16);

            if (v & 0x800000)
                v |= ~0xFFFFFF;

            tmp[k] = v;
        }

        // Load 4 int32 values
        __m128i xi = _mm_loadu_si128((__m128i*)tmp);

        // Convert int32 → float32
        __m128 xf = _mm_cvtepi32_ps(xi);

        // Scale into [-1,1]
        xf = _mm_mul_ps(xf, scale);

        // Store 4 floats
        _mm_storeu_ps(out + i, xf);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }
#endif
#endif
}

// int8_t range: −128…127

// FLAC PCM_U8 range: 0…255

// Silence in PCM_U8 = 128

// void convert_int8_to_uint8(const int8_t* in, uint8_t* out, size_t n)
// {
//     for (size_t i = 0; i < n; ++i) out[i] = uint8_t(int(in[i]) + 128);
// }

// static
void SampleTypeConverter::convert_S8_to_U8( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const int8_t* __restrict__ in = reinterpret_cast<const int8_t*>(src);
    uint8_t*     __restrict__ out = reinterpret_cast<uint8_t*>(dst);

    // ============================================================
    // Scalar fallback
    // ============================================================
    for (int64_t i = 0; i < nSamples; ++i)
    {
        out[i] = uint8_t(int_fast16_t(in[i]) + 128);
    }
}

// #include <emmintrin.h>

// static
void SampleTypeConverter::convert_S8_to_U8_sse( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const int8_t* __restrict__ in = reinterpret_cast<const int8_t*>(src);
    uint8_t*     __restrict__ out = reinterpret_cast<uint8_t*>(dst);

    const __m128i add128 = _mm_set1_epi8(char(128));

    int64_t i = 0;
    for (; i + 16 <= nSamples; i += 16)
    {
        __m128i v = _mm_loadu_si128((const __m128i*)(in + i));
        v = _mm_add_epi8(v, add128);
        _mm_storeu_si128((__m128i*)(out + i), v);
    }

    // tail
    for (; i < nSamples; ++i)
    {
        out[i] = uint8_t(int_fast16_t(in[i]) + 128);
    }
}

// #include <immintrin.h>

// static
void SampleTypeConverter::convert_S8_to_U8_avx( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples )
{
    const int8_t* __restrict__ in = reinterpret_cast<const int8_t*>(src);
    uint8_t*     __restrict__ out = reinterpret_cast<uint8_t*>(dst);

    const __m256i add128 = _mm256_set1_epi8(char(128));

    size_t i = 0;
    for (; i + 32 <= nSamples; i += 32)
    {
        __m256i v = _mm256_loadu_si256((const __m256i*)(in + i));
        v = _mm256_add_epi8(v, add128);
        _mm256_storeu_si256((__m256i*)(out + i), v);
    }

    for (; i < nSamples; ++i)
    {
        out[i] = uint8_t(int_fast16_t(in[i]) + 128);
    }
}

} // end namespace de.



#if 0

// F32 <=====> S16

void convertFloatToInt16(const float* in, int16_t* out, size_t n) noexcept
{
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Broadcast constants into 8-wide SIMD registers
    const __m256 scale = _mm256_set1_ps(32767.0f); // multiply float [-1,1] → int16 range
    const __m256 minv  = _mm256_set1_ps(-1.0f);    // clamp lower bound
    const __m256 maxv  = _mm256_set1_ps(1.0f);     // clamp upper bound

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8 floats: x0 x1 x2 x3 x4 x5 x6 x7
        __m256 x = _mm256_loadu_ps(in + i);

        // x = min(x, 1.0f)
        // SIMD min: elementwise min(x[j], maxv[j])
        x = _mm256_min_ps(x, maxv);

        // x = max(x, -1.0f)
        // SIMD max: elementwise max(x[j], minv[j])
        x = _mm256_max_ps(x, minv);

        // x = x * 32767.0f
        // SIMD multiply: elementwise scaling into int16 range
        x = _mm256_mul_ps(x, scale);

        // Convert 8 floats → 8 int32 using truncation
        // cvttps2dq: float → int32, truncates toward zero
        __m256i xi = _mm256_cvtps_epi32(x);

        // Now xi contains 8 int32 values:
        // [i0 i1 i2 i3 i4 i5 i6 i7]

        // Split 256-bit register into two 128-bit halves
        __m128i lo = _mm256_castsi256_si128(xi);        // i0 i1 i2 i3
        __m128i hi = _mm256_extracti128_si256(xi, 1);   // i4 i5 i6 i7

        // Pack 8 int32 → 8 int16 with saturation
        // packssdw:
        //   - takes two 4×int32 vectors
        //   - converts each int32 → int16
        //   - clamps to [-32768, 32767]
        //   - produces 8×int16
        __m128i packed = _mm_packs_epi32(lo, hi);

        // Store 8 int16 values
        _mm_storeu_si128((__m128i*)(out + i), packed);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(32767.0f);
    const __m128 minv  = _mm_set1_ps(-1.0f);
    const __m128 maxv  = _mm_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        // Load 4 floats
        __m128 x = _mm_loadu_ps(in + i);

        // Clamp to [-1,1]
        x = _mm_min_ps(x, maxv);
        x = _mm_max_ps(x, minv);

        // Scale into int16 range
        x = _mm_mul_ps(x, scale);

        // Convert 4 floats → 4 int32
        __m128i xi = _mm_cvtps_epi32(x);

        // Pack 4 int32 → 4 int16 (upper half ignored)
        __m128i packed = _mm_packs_epi32(xi, xi);

        // Store lower 64 bits (4×int16)
        _mm_storel_epi64((__m128i*)(out + i), packed);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        out[i] = int16_t(x * 32767.0f);
    }
#endif
}


void convertInt16ToFloat(const int16_t* in, float* out, size_t n) noexcept
{
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 16 samples per loop
    // ============================================================

    // Scale factor: 1 / 32767.0f
    // Converts int16 [-32768, 32767] → float [-1,1]
    const __m256 scale = _mm256_set1_ps(1.0f / 32767.0f);

    size_t i = 0;
    for (; i + 16 <= n; i += 16)
    {
        // Load 16 int16 samples (256 bits)
        __m256i raw16 = _mm256_loadu_si256((__m256i*)(in + i));

        // Unpack 16×int16 → 16×int32
        // unpack low 8 samples
        __m256i lo = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(raw16));
        // unpack high 8 samples
        __m256i hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(raw16, 1));

        // Convert int32 → float32
        __m256 flo = _mm256_cvtepi32_ps(lo);
        __m256 fhi = _mm256_cvtepi32_ps(hi);

        // Scale into [-1,1]
        flo = _mm256_mul_ps(flo, scale);
        fhi = _mm256_mul_ps(fhi, scale);

        // Store 16 floats
        _mm256_storeu_ps(out + i, flo);
        _mm256_storeu_ps(out + i + 8, fhi);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 8 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(1.0f / 32767.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8 int16 samples (128 bits)
        __m128i raw16 = _mm_loadu_si128((__m128i*)(in + i));

        // Unpack 8×int16 → 8×int32
        __m128i lo = _mm_unpacklo_epi16(raw16, _mm_setzero_si128());
        __m128i hi = _mm_unpackhi_epi16(raw16, _mm_setzero_si128());

        // Sign-extend int16 → int32
        lo = _mm_srai_epi32(lo, 16);
        hi = _mm_srai_epi32(hi, 16);

        // Convert int32 → float32
        __m128 flo = _mm_cvtepi32_ps(lo);
        __m128 fhi = _mm_cvtepi32_ps(hi);

        // Scale into [-1,1]
        flo = _mm_mul_ps(flo, scale);
        fhi = _mm_mul_ps(fhi, scale);

        // Store 8 floats
        _mm_storeu_ps(out + i, flo);
        _mm_storeu_ps(out + i + 4, fhi);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        out[i] = float(in[i]) * (1.0f / 32767.0f);
    }
#endif
}


// F32 <=====> S24

void convertFloatToInt24(const float* in, uint8_t* out, size_t n) noexcept
{
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Broadcast constants:
    // 8388607.0f = max positive int24 value (0x7FFFFF)
    const __m256 scale = _mm256_set1_ps(8388607.0f);
    const __m256 minv  = _mm256_set1_ps(-1.0f);
    const __m256 maxv  = _mm256_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8 floats
        __m256 x = _mm256_loadu_ps(in + i);

        // Clamp to [-1,1]
        x = _mm256_min_ps(x, maxv);   // x[j] = min(x[j], 1.0)
        x = _mm256_max_ps(x, minv);   // x[j] = max(x[j], -1.0)

        // Scale into int24 range
        // x[j] = x[j] * 8388607.0f
        x = _mm256_mul_ps(x, scale);

        // Convert float → int32 (truncate toward zero)
        // cvttps2dq: elementwise float → int32
        __m256i xi = _mm256_cvtps_epi32(x);

        // Split 8×int32 into two 128-bit halves
        __m128i lo = _mm256_castsi256_si128(xi);        // i0 i1 i2 i3
        __m128i hi = _mm256_extracti128_si256(xi, 1);   // i4 i5 i6 i7

        // Store each int32 as 3 bytes (little-endian)
        int32_t tmp[8];
        _mm_storeu_si128((__m128i*)tmp, lo);
        _mm_storeu_si128((__m128i*)(tmp + 4), hi);

        uint8_t* dst = out + i * 3;

        // Manual pack: int32 → 3 bytes
        // tmp[k] & 0xFFFFFF gives 24-bit signed PCM
        for (int k = 0; k < 8; ++k)
        {
            int32_t v = tmp[k];

            // Little-endian 24-bit:
            // byte0 = LSB
            // byte1 = mid
            // byte2 = MSB
            dst[k*3 + 0] = (uint8_t)(v & 0xFF);
            dst[k*3 + 1] = (uint8_t)((v >> 8) & 0xFF);
            dst[k*3 + 2] = (uint8_t)((v >> 16) & 0xFF);
        }
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(8388607.0f);
    const __m128 minv  = _mm_set1_ps(-1.0f);
    const __m128 maxv  = _mm_set1_ps(1.0f);

    size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        __m128 x = _mm_loadu_ps(in + i);

        x = _mm_min_ps(x, maxv);
        x = _mm_max_ps(x, minv);

        x = _mm_mul_ps(x, scale);

        __m128i xi = _mm_cvtps_epi32(x);

        int32_t tmp[4];
        _mm_storeu_si128((__m128i*)tmp, xi);

        uint8_t* dst = out + i * 3;

        for (int k = 0; k < 4; ++k)
        {
            int32_t v = tmp[k];
            dst[k*3 + 0] = (uint8_t)(v & 0xFF);
            dst[k*3 + 1] = (uint8_t)((v >> 8) & 0xFF);
            dst[k*3 + 2] = (uint8_t)((v >> 16) & 0xFF);
        }
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        float x = in[i];
        x = std::clamp(x, -1.0f, 1.0f);
        int32_t v = int32_t(x * 8388607.0f);

        uint8_t* dst = out + i * 3;
        dst[0] = (uint8_t)(v & 0xFF);
        dst[1] = (uint8_t)((v >> 8) & 0xFF);
        dst[2] = (uint8_t)((v >> 16) & 0xFF);
    }
#endif
}


void convertInt24ToFloat(const uint8_t* in, float* out, size_t n) noexcept
{
#if defined(__AVX2__)
    // ============================================================
    // AVX2: process 8 samples per loop
    // ============================================================

    // Scale factor: 1 / 8388607.0f
    // Converts int24 [-8388608, 8388607] → float [-1,1]
    const __m256 scale = _mm256_set1_ps(1.0f / 8388607.0f);

    size_t i = 0;
    for (; i + 8 <= n; i += 8)
    {
        // Load 8×24-bit samples → unpack into 8×int32
        // We must manually unpack because AVX2 has no 24-bit load.

        int32_t tmp[8];

        const uint8_t* src = in + i * 3;

        for (int k = 0; k < 8; ++k)
        {
            // Little-endian 24-bit:
            // byte0 = LSB
            // byte1 = mid
            // byte2 = MSB
            int32_t v =
                (int32_t(src[k*3 + 0])      ) |
                (int32_t(src[k*3 + 1]) << 8 ) |
                (int32_t(src[k*3 + 2]) << 16);

            // Sign-extend from 24-bit to 32-bit:
            // If MSB (bit 23) is set → negative number
            if (v & 0x800000)
                v |= ~0xFFFFFF;  // fill upper bits with 1s

            tmp[k] = v;
        }

        // Load 8 int32 values into AVX2 register
        __m256i xi = _mm256_loadu_si256((__m256i*)tmp);

        // Convert int32 → float32
        // cvtdq2ps: elementwise int32 → float32
        __m256 xf = _mm256_cvtepi32_ps(xi);

        // Scale into [-1,1]
        xf = _mm256_mul_ps(xf, scale);

        // Store 8 floats
        _mm256_storeu_ps(out + i, xf);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }

#elif defined(__SSE2__)
    // ============================================================
    // SSE2: process 4 samples per loop
    // ============================================================

    const __m128 scale = _mm_set1_ps(1.0f / 8388607.0f);

    size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        int32_t tmp[4];

        const uint8_t* src = in + i * 3;

        for (int k = 0; k < 4; ++k)
        {
            int32_t v =
                (int32_t(src[k*3 + 0])      ) |
                (int32_t(src[k*3 + 1]) << 8 ) |
                (int32_t(src[k*3 + 2]) << 16);

            if (v & 0x800000)
                v |= ~0xFFFFFF;

            tmp[k] = v;
        }

        // Load 4 int32 values
        __m128i xi = _mm_loadu_si128((__m128i*)tmp);

        // Convert int32 → float32
        __m128 xf = _mm_cvtepi32_ps(xi);

        // Scale into [-1,1]
        xf = _mm_mul_ps(xf, scale);

        // Store 4 floats
        _mm_storeu_ps(out + i, xf);
    }

    // Scalar tail
    for (; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }

#else
    // ============================================================
    // Scalar fallback
    // ============================================================
    for (size_t i = 0; i < n; ++i)
    {
        const uint8_t* src = in + i * 3;

        int32_t v =
            (int32_t(src[0])      ) |
            (int32_t(src[1]) << 8 ) |
            (int32_t(src[2]) << 16);

        if (v & 0x800000)
            v |= ~0xFFFFFF;

        out[i] = float(v) * (1.0f / 8388607.0f);
    }
#endif
}

#endif
