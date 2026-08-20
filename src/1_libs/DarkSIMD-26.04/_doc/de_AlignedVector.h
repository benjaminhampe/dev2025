#pragma once
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
// #include <memory>
// #include <stdexcept>

namespace de {

template <typename T, std::size_t Alignment>
struct AlignedAllocator {
    using value_type = T;

    // Rebind support
    template <typename U>
    struct rebind
    {
        using other = AlignedAllocator<U, Alignment>;
    };

    T* allocate(std::size_t n)
    {
        void* ptr = _aligned_malloc(n * sizeof(T), Alignment); // std::aligned_alloc(Alignment, n * sizeof(T));
        if (!ptr) throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        _aligned_free(p);
    }

    // Optional: equality operators
    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};

// AVX512 ready. Uses 64byte cache-line.

#ifdef BENNI_DONT_USE_MODERN_CPP
    template <typename T>
    class TAlignedVector : public std::vector<T, AlignedAllocator<T, 64>>
    {};
#else
    template <typename T>
    using TAlignedVector = std::vector<T, AlignedAllocator<T, 64>>;
#endif

typedef TAlignedVector<float> AlignedFloatVector;
typedef TAlignedVector<uint8_t> AlignedByteVector;
typedef TAlignedVector<uint32_t> AlignedColorVector;


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

} // end namespace de.
