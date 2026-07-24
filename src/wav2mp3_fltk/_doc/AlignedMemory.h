#pragma once
#include <de/Core.h>
#include <functional>

#if 0
    #include <malloc.h>
    #include <new>
    #include <cstddef>

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

namespace de {

    // Uses 64byte cache-line. AVX512 ready.

    template <typename T, std::size_t Alignment>
    struct TAlignedVectorAllocator
    {
        static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be power of two");
        static_assert(Alignment >= alignof(T), "Alignment must be >= alignof(T)");

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        //using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_swap = std::true_type;
        using is_always_equal = std::true_type;

        // template <typename U>
        // struct rebind {
        //     using other = TAlignedVectorAllocator<U, Alignment>;
        // };

        // Rebind support
        template <typename U> struct rebind
        {
            using other = TAlignedVectorAllocator<U, Alignment>;
        };

        T* allocate(std::size_t n)
        {
            if (n > max_size())
            {
                throw std::bad_alloc();
            }

            size_t nBytes = n * sizeof(T);
            if (nBytes % Alignment != 0)
            {
                //DE_WARN(nBytes," not a multiple of Alignment(",Alignment,")")
                size_t nMofA = nBytes / Alignment;
                nBytes = (nMofA+1) * Alignment;
                //DE_WARN(nBytes," adapted to a multiple of Alignment(",Alignment,")")
                size_type padded = (nBytes + (Alignment - 1)) & ~(Alignment - 1);
                //DE_WARN(padded," padded to a multiple of Alignment(",Alignment,")")
            }
            void* ptr = _aligned_malloc(nBytes, Alignment);
            //std::aligned_alloc(Alignment, n * sizeof(T));
            if (!ptr) throw std::bad_alloc();
            return static_cast<T*>(ptr);
        }

        void deallocate(T* p, std::size_t) noexcept
        {
            if (p)
            {
                _aligned_free(p);
            }
        }

        std::size_t max_size() const noexcept
        {
            return std::numeric_limits<std::size_t>::max() / sizeof(T);
        }

        //bool operator==(const TAlignedVectorAllocator&) const noexcept { return true; }
        //bool operator!=(const TAlignedVectorAllocator&) const noexcept { return false; }
    };

    // AVX2 32-byte aligned vector
    template <typename T>
    using TAlignedVector = std::vector<T, TAlignedVectorAllocator<T, 32>>;

    // AVX512 64-byte aligned vector
    template <typename T>
    using TAlignedVector64 = std::vector<T, TAlignedVectorAllocator<T, 64>>;

    typedef TAlignedVector<f32> AlignedFloatVector;
    typedef TAlignedVector<u8>  AlignedByteVector;

#if 0
    inline BBox1f
    computeMinMax(AlignedFloatVector const & v)
    {
        f32 lMin = std::numeric_limits< f32 >::max();
        f32 lMax = std::numeric_limits< f32 >::lowest();

        for ( const f32& f : v )
        {
            lMin = std::min( lMin, f );
            lMax = std::max( lMax, f );
        }

        return BBox1f(lMin,lMax);
    }
#endif

} // end namespace de.
