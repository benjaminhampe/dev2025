#pragma once
#include <DarkImage.h>
#include <cmath>

#include <immintrin.h>

class Float8 {
public:
    __m256 v;

    DE_FORCEINLINE Float8()                 : v(_mm256_setzero_ps()) {}
    DE_FORCEINLINE Float8(float f)          : v(_mm256_set1_ps(f)) {}
    DE_FORCEINLINE Float8(__m256 vec)       : v(vec) {}
    DE_FORCEINLINE Float8(const float* ptr) : v(_mm256_loadu_ps(ptr)) {}

    // Store
    DE_FORCEINLINE void store(float* ptr) const { _mm256_storeu_ps(ptr, v); }

    // Basic arithmetic
    DE_FORCEINLINE Float8 operator+(const Float8& rhs) const { return _mm256_add_ps(v, rhs.v); }
    DE_FORCEINLINE Float8 operator-(const Float8& rhs) const { return _mm256_sub_ps(v, rhs.v); }
    DE_FORCEINLINE Float8 operator*(const Float8& rhs) const { return _mm256_mul_ps(v, rhs.v); }
    DE_FORCEINLINE Float8 operator/(const Float8& rhs) const { return _mm256_div_ps(v, rhs.v); }

    // Compound assignment
    DE_FORCEINLINE Float8& operator+=(const Float8& rhs) { v = _mm256_add_ps(v, rhs.v); return *this; }
    DE_FORCEINLINE Float8& operator-=(const Float8& rhs) { v = _mm256_sub_ps(v, rhs.v); return *this; }
    DE_FORCEINLINE Float8& operator*=(const Float8& rhs) { v = _mm256_mul_ps(v, rhs.v); return *this; }
    DE_FORCEINLINE Float8& operator/=(const Float8& rhs) { v = _mm256_div_ps(v, rhs.v); return *this; }

    // Comparison
    DE_FORCEINLINE Float8 min(const Float8& rhs) const { return _mm256_min_ps(v, rhs.v); }
    DE_FORCEINLINE Float8 max(const Float8& rhs) const { return _mm256_max_ps(v, rhs.v); }
    DE_FORCEINLINE Float8 clamp(const Float8& lo, const Float8& hi) const { return max(lo).min(hi); }

    // Math functions
    DE_FORCEINLINE Float8 sqrt() const { return _mm256_sqrt_ps(v); }
    DE_FORCEINLINE Float8 abs() const { return _mm256_andnot_ps(_mm256_set1_ps(-0.f), v); }
    DE_FORCEINLINE Float8 floor() const { return _mm256_floor_ps(v); }
    DE_FORCEINLINE Float8 ceil() const { return _mm256_ceil_ps(v); }

    // Trig (requires external lib or approximation)
    DE_FORCEINLINE Float8 sin() const { return Float8(sin_approx3(v)); }
    DE_FORCEINLINE Float8 cos() const { return Float8(cos_approx3(v)); }

    // Conversion
    DE_FORCEINLINE operator __m256() const { return v; }

private:
    // Taylor polynome
    DE_FORCEINLINE static __m256 sin_approx3(__m256 x)
    {
        // Example: x - x^3/6 + x^5/120 (valid for small x)
        __m256 x2 = _mm256_mul_ps(x, x);
        __m256 x3 = _mm256_mul_ps(x2, x);
        __m256 x5 = _mm256_mul_ps(x3, x2);
        return _mm256_sub_ps(x, _mm256_div_ps(x3, _mm256_set1_ps(6.f))) +
               _mm256_div_ps(x5, _mm256_set1_ps(120.f));
    }

    // Taylor polynome
    DE_FORCEINLINE static __m256 cos_approx3(__m256 x)
    {
        // Example: 1 - x^2/2 + x^4/24 (valid for small x)
        __m256 x2 = _mm256_mul_ps(x, x);
        __m256 x4 = _mm256_mul_ps(x2, x2);
        return _mm256_sub_ps(_mm256_set1_ps(1.f), _mm256_div_ps(x2, _mm256_set1_ps(2.f))) +
               _mm256_div_ps(x4, _mm256_set1_ps(24.f));
    }
};
