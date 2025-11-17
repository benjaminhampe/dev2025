#pragma once
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
// #include <memory>
// #include <stdexcept>

template <typename T, std::size_t Alignment>
struct AlignedAllocator
{
    using value_type = T;

    T* allocate(std::size_t n)
    {
        void* ptr = _aligned_malloc(n * sizeof(T), Alignment); // std::aligned_alloc(Alignment, n * sizeof(T));
        if (!ptr) throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        _aligned_free(p); // std::free(p);
    }
};

// Use this type for all AudioProcessing!
// AVX512 ready. Uses 64byte cache-line.
typedef std::vector<float, AlignedAllocator<float, 64>>
    AlignedFloatVector;

// Simple window functions:
// TODO: Replace with stateful classes that contain precomputed lookup tables.

inline void apply_hann_window(float* data, int32_t N)
{
    for (int32_t n = 0; n < N; ++n)
    {
        float w = 0.5f * (1.0f - cosf(2.0f * M_PI * n / (N - 1)));
        data[n] *= std::clamp(w, 0.0f, 1.0f);
    }
}

inline void apply_hamming_window(float* data, int32_t N)
{
    for (int32_t n = 0; n < N; ++n)
    {
        float w = 0.54f
                  - 0.46f * cosf(2.0f * M_PI * n / (N - 1));
        data[n] *= std::clamp(w, 0.0f, 1.0f);
    }
}

inline void apply_blackman_window(float* data, int32_t N)
{
    for (int32_t n = 0; n < N; ++n)
    {
        float w = 0.42f
                  - 0.5f * cosf(2.0f * M_PI * n / (N - 1))
                  + 0.08f *cosf(4.0f * M_PI * n / (N - 1));
        data[n] *= std::clamp(w, 0.0f, 1.0f);
    }
}


#if 0
void* operator new(std::size_t size)
{
    void* ptr = _aligned_malloc(size, 64);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    _aligned_free(ptr);
}

#include <malloc.h>
#include <new>
#include <cstddef>

class Aligned64
{
public:
    static constexpr std::size_t Alignment = 64;

    void* operator new(std::size_t size)
    {
        void* ptr = _aligned_malloc(size, Alignment);
        if (!ptr) throw std::bad_alloc();
        return ptr;
    }

    void operator delete(void* ptr) noexcept
    {
        _aligned_free(ptr);
    }
};

#endif

