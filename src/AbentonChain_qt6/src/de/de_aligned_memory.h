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
                DE_ERROR(nBytes," not a multiple of Alignment(",Alignment,")")
                size_t nMofA = nBytes / Alignment;
                nBytes = (nMofA+1) * Alignment;
                DE_ERROR(nBytes," adapted to a multiple of Alignment(",Alignment,")")
                size_type padded = (nBytes + (Alignment - 1)) & ~(Alignment - 1);
                DE_ERROR(padded," padded to a multiple of Alignment(",Alignment,")")
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

#if 1
    // AVX2 32-byte aligned vector
    template <typename T>
    using TAlignedVector = std::vector<T, TAlignedVectorAllocator<T, 32>>;

    // AVX512 64-byte aligned vector
    template <typename T>
    using TAlignedVector64 = std::vector<T, TAlignedVectorAllocator<T, 64>>;

    typedef TAlignedVector<f32> AlignedFloatVector;
    typedef TAlignedVector<u8>  AlignedByteVector;

#else
    template <typename T>
    using TAlignedVector = std::vector<T>;

    typedef TAlignedVector<f32> AlignedFloatVector;
    typedef TAlignedVector<u8>  AlignedByteVector;

#endif

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

    // ----------------------------------------------------
    //
    // ----------------------------------------------------
    template < typename T >
    struct TAlignedShiftVector
    {
        using Vector = TAlignedVector<T>;
        using FN_onFullVector = std::function< void(Vector const &)>;
        u64 m_free;
        u64 m_used;
        TAlignedVector< T > m_data;
        FN_onFullVector m_onFullVector;

        TAlignedShiftVector( size_t n = 0 )
            : m_free(0)
            , m_used(0)
            , m_onFullVector([] (Vector const &) {})
        {
            if (n>0)
            {
                m_data.resize(n);
            }
        }

        void setCallback_onFullVector( FN_onFullVector const & onFullVector ) { m_onFullVector = onFullVector; }

        BBox1f getMinMax() const { return computeMinMax(m_data); }

        void resize( size_t desired )
        {
            if (m_data.size() != desired)
            {
                m_data.resize( desired );
                m_used = 0;
                m_free = m_data.size();
            }
        }

        // ShiftVector<float>(8)
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // ShiftVector<float>(8).push([0,1,2,3,4,5,6])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   ?   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // ShiftVector<float>(8).push([7,8])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // ShiftVector<float>(8).push([A,B,C,D])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   5   |   6   |   7   |   8   |   A   |   B   |   C   |   D   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;
        // ShiftVector<float>(8).push([E,F,B])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   8   |   A   |   B   |   C   |   D   |   E   |   F   |   B   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;

        void push(const T* __restrict__ pSamples, u32 nSamples)
        {
            if (nSamples > size())
            {
                // resize(nSamples);
                DE_ERROR("Split logic is not recursive. nSamples too large for single iteration.")
                nSamples = size();
            }

            if (size() == nSamples)
            {
                std::memcpy(m_data.data(), pSamples, nSamples * sizeof(T));

                m_onFullVector(m_data);

                m_used = 0;

                return;
            }

            // TAlignedShiftVector<float>(8) v8: nAvail=8
            // ++---+---+---+---++---+---+---+---++
            // ||   |   |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            // 0.) push([0,1,2,3,4])
            // - Before: nAvail=8, nSamples=5       (nAvail >= nSamples)
            // - After:  nAvail=3
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | ? | ? | ? ||
            // ++---+---+---+---++---+---+---+---++

            // 1.) push([5,6,7,8,9])                (nAvail >= nSamples)
            // - Before: nAvail=3, nSamples=5

            // 2.) push([5,6,7])
            // - After:  nAvail=0, nSamples=2         ---> Notify onFullRow()
            // - After:  nAvail=8, nSamples=2       (nAvail > nSamples)
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||
            // ++---+---+---+---++---+---+---+---++

            // 3.) push([8,9])
            // - After:  nAvail=6, nSamples=0
            // ++---+---+---+---++---+---+---+---++
            // || 8 | 9 |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            if (nSamples + used() > size())
            {
                // push([5,6,7,8,9]) :: nSamples = 5, nAvail = size(8)-used(5);
                u64 nAvail = avail();
                // I. push([5,6,7]) :: nLeft = min(nSamples,nAvail) = min(nSamples,3) = 3;
                u64 n1 = std::min<u64>(nSamples, nAvail);
                // II. push([8,9]) :: nRight = nSamples - nLeft = 2;
                u64 n2 = nSamples - n1;

                // I. Copy until end of row ... ( push([5,6,7]) )
                // ++---+---+---+---++---+---+---+---++
                // || 0 | 1 | 2 | 3 || 4 |[5]|[6]|[7]|| :: nLeft = 3 = nSamples - (m_size - m_used);
                // ++---+---+---+---++---+---+---+---++
                if (n1 > 0)
                {
                    const T* __restrict__ src = pSamples;  // src=[5,6,7|8,9]
                    T* __restrict__ dst = data() + used(); // dst=[0,1,2,3,4|?,?,?]
                    std::memcpy(dst, src, n1 * sizeof(T));
                }

                // II. Notify row ...
                // ++---+---+---+---++---+---+---+---++
                // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
                // ++---+---+---+---++---+---+---+---++
                // III. Reset row ...
                // ++---+---+---+---++---+---+---+---++
                // ||   |   |   |   ||   |   |   |   ||
                // ++---+---+---+---++---+---+---+---++
                // m_used = 0, size() = 8;
                m_onFullVector(m_data);
                m_used = 0;

                // DE_OK("[",caller,"] onFullVector(", m_data.size(),"), used(",m_used,") :: push(",n1,") :: END")
                // DE_OK("[",caller,"] onFullVector(", m_data.size(),"), used(",m_used,") :: push(",n2,") :: END")
                //

                // IV. Final push([8,9]) :: nRight = 2 = nSamples - Left
                // ++---+---+---+---++---+---+---+---++
                // || 8 | 9 |   |   ||   |   |   |   ||
                // ++---+---+---+---++---+---+---+---++
                // m_used = 2;
                if (n2 > 0)
                {
                    const T* __restrict__ src = pSamples + n1; // Read remain input token.
                    T* __restrict__ dst = data(); // Write to begin() of shiftbuffer.
                    std::memcpy( dst, src, n2 * sizeof(T) );
                    m_used = n2;
                }
            }
            else
            {
                // DE_OK("MID")
                const T* __restrict__ src = pSamples;
                T* __restrict__ dst = data() + used();
                std::memcpy( dst, src, nSamples * sizeof(T));
                m_used += nSamples;
            }
        }

        T const * data() const { return m_data.data(); }
        T * data() { return m_data.data(); }

        T const & at( size_t i ) const { return m_data.at( i ); }
        T & at( size_t i ) { return m_data.at( i ); }

        T const & operator[] ( size_t i ) const { return m_data.at( i ); }
        T & operator[] ( size_t i ) { return m_data.at( i ); }

        u64 avail() const
        {
            if (m_data.size() < m_used)
            {
                throw std::runtime_error("m_data.size() < m_used");
            }
            return m_data.size() - m_used;
        }

        u64 used() const { return m_used; }
        u64 size() const { return m_data.size(); }
        u64 capacity() const { return m_data.capacity(); }
        void clear() { m_used = 0; m_free = m_data.size(); }
        void fill( T const & value ) { for ( T & f : m_data) f = value; }
        void fillZero() { for ( auto & f : m_data) f = 0.0f; }

        static bool compare( TAlignedVector<T> const & a, TAlignedVector<T> const & b )
        {
            if (a.size() != b.size())
            {
                DE_ERROR("a.size() != b.size()")
                return false;
            }

            for (size_t i = 0; i < a.size(); i++)
            {
                if (a[i] != b[i])
                {
                    DE_ERROR("a[i] != b[i]")
                    return false;
                }
            }
            return true;
        }

        bool operator==( TAlignedVector<T> const & other ) const
        {
            return compare(*this,other);
        }

        bool operator!=( TAlignedVector<T> const & other ) const
        {
            return !compare(*this,other);
        }



        std::string str() const
        {
            std::stringstream o;

            o << "n = " << m_data.size() << "\n";
            for ( u64 i = 0; i < m_data.size(); ++i )
            {
                o << "[" << i << "] " << m_data[ i ] << "\n";
            }
            return o.str();
        }
    };

    struct ShiftVectorTest
    {
        static void
        test()
        {
            typedef float T;

            // TAlignedShiftVector<float>(8) v8: m_used = 0;
            // ++---+---+---+---++---+---+---+---++
            // ||   |   |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            TAlignedShiftVector<T> testObj; // (8);
            testObj.resize(8);

            if (testObj.size() != 8)
            {
                DE_ERROR("testObj.size() != 8")
                return;
            }

            // [n=8] Push([0,1,2,3,4]) :: nSamples = 5, testObj.m_used = 0;
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | ? | ? | ? ||
            // ++---+---+---+---++---+---+---+---++
            TAlignedVector<T> a{0,1,2,3,4};
            testObj.push(a.data(),a.size());
            TAlignedVector<T> b{0,1,2,3,4,0,0,0};
            if (testObj.m_data != b)
            {
                DE_ERROR("Test(1)[n=8] != {0,1,2,3,4,5,6}")
                return;
            }

            // [n=8] Push([5,6,7,8,9]) :: nSamples = 5, testObj.m_used = 5, testObj.m_size = 8;

            // = Push([5,6,7]) :: nLeft = 3 = nSamples - (testObj.m_size - testObj.m_used);
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
            // ++---+---+---+---++---+---+---+---++

            // + Push([8,9]) :: nRight = 2 = nSamples - Left, testObj.m_used = 2;
            // ++---+---+---+---++---+---+---+---++
            // || 8 | 9 |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            // push([5,6,7]) :: n1 = 3 = nSamples - (m_size - m_used);
            // push([8,9]) :: n2 = 2 = nSamples - n1;

            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
            // ++---+---+---+---++---+---+---+---++

            // +-------+-------+-------+-------+-------+-------+-------+-------+
            // |       |       |       |       |       |       |       |       |
            // |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   ?   |
            // |       |       |       |       |       |       |       |       |
            // +-------+-------+-------+-------+-------+-------+-------+-------+

            // [n=8] Push([7,8]):
            // +-------+-------+-------+-------+-------+-------+-------+-------+
            // |       |       |       |       |       |       |       |       |
            // |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
            // |       |       |       |       |       |       |       |       |
            // +-------+-------+-------+-------+-------+-------+-------+-------+
            bool bTestFull1 = false;
            testObj.setCallback_onFullVector([&](TAlignedVector<T> const &v)
                                { bTestFull1 = true; });
            TAlignedVector<T> c{7,8};
            testObj.push(c.data(),c.size());
            if (!bTestFull1)
            {
                DE_ERROR("!bTestFull1")
                return;
            }

            if (testObj.m_data != TAlignedVector<T>{1,2,3,4,5,6,7,8})
            {
                DE_ERROR("Test(2)[n=8]  != {1,2,3,4,5,6,7,8}")
                return;
            }
            // [n=8] Push([4,4,6,0]):
            // +-------+-------+-------+-------+-------+-------+-------+-------+
            // |       |       |       |       |       |       |       |       |
            // |   5   |   6   |   7   |   8   |   4   |   4   |   6   |   0   |
            // |       |       |       |       |       |       |       |       |
            // +-------+-------+-------+-------+-------+-------+-------+-------+;
            bool bTestFull2 = false;
            testObj.setCallback_onFullVector([&](TAlignedVector<T> const &v){ bTestFull2 = true; });
            TAlignedVector<T> d{4,4,6,0};
            testObj.push(d.data(),d.size());

            if (!bTestFull2)
            {
                DE_ERROR("!bTestFull2")
                //return;
            }

            if (testObj.m_data != TAlignedVector<T>{5,6,7,8,4,4,6,0})
            {
                DE_ERROR("Test(3)[n=8]  != {5,6,7,8,4,4,6,0}")
                return;
            }
            // [n=8] Push([1,1,2]):
            // +-------+-------+-------+-------+-------+-------+-------+-------+
            // |       |       |       |       |       |       |       |       |
            // |   8   |   4   |   4   |   6   |   0   |   1   |   1   |   2   |
            // |       |       |       |       |       |       |       |       |
            // +-------+-------+-------+-------+-------+-------+-------+-------+;
            TAlignedVector<T> e{1,1,2};
            testObj.push(e.data(),e.size());
            if (testObj.m_data != TAlignedVector<T>{8,4,4,6,0,1,1,2})
            {
                DE_ERROR("Test(4)[n=8]  != {8,4,4,6,0,1,1,2}")
                return;
            }

            /*
            TAlignedVector< T > a{ 0,1,2,3,4,5,6,7,8,9};
            TAlignedVector< T > b{ 10,11,12,13,14,15,16,17,18,19};
            TAlignedVector< T > c{ 20,21,22,23,24,25,26,27,28,29};

            TAlignedShiftVector<T> testObj2;

            testObj.resize( 20 );
            DE_DEBUG( "TestResult[0] :: Resize(20): ", testObj.str() )

            testObj.push( "test1", a.data(), 10, true );
            DE_DEBUG( "TestResult[1] :: Push(a): ", testObj.str() )

            testObj.push( "test2", b.data(), 10, true );
            DE_DEBUG( "TestResult[2] :: Push(b): ", testObj.str() )

            testObj.push( "test3", c.data(), 10, true );
            DE_DEBUG( "TestResult[3] :: Push(c): ", testObj.str() )
            */
        }
    };

    typedef TAlignedShiftVector<f32>    AlignedFloatShiftVector;
    typedef TAlignedShiftVector<u8>     AlignedByteShiftVector;

    // Manages a rows that are automaticly shifted when new data arrives.
    //
    // 3D meshes are created out of this data to render with OpenGLES.
    //
    // Can hold AudioWaveform ( 1 float per sample )
    //       or AudioWaveformAmp in dB ( 1 float ), 1 dB = 20 * log10( amp*amp );
    //       or AudioSpektrumAmplitude in dB ( 1 float )
    //       or AudioSpektrumPhase in radians? or cents? ( 1 float )
    //
    // does only swap pointers and does not move memory!
    // push() manages matrix shift and always refills the 0-th row
    // if m_shiftBuffer collected enough samples to fill a new row.
    // A ShiftBuffer is used to decouple different audio callback size and matrix column size
    // But only meaningful if m_dacFrames <= m_shiftBuffer.size()
    //
    // To have a more quadratic matrix it uses m_dacFrames = m_shiftBuffer.size()
    // so the shiftbuffer probably collects only once untils its already full.
    // With colCount == m_dacFrames each row represents exactly one audio callback.
    //
    // Since push() is called constantly we delay expensive postfx like log10f() until render().
    // render() collects data from the matrix using m_samples, not m_orig or m_copy.
    // =======================================================================
    struct AlignedFloatShiftMatrix
    // =======================================================================
    {
        typedef float T;
        typedef TAlignedVector< T > TData;
        typedef std::vector< T* > TRowVector;

        u32 m_colCount;  // Count matrix cols = m_shiftBuffer.size()
        u32 m_rowCount;  // Count matrix rows
        TRowVector m_rows; // Row Viewer ( original rows )
        TRowVector m_temp; // Row Viewer ( shuffled rows )
        TData m_data;

        AlignedFloatShiftMatrix();
        ~AlignedFloatShiftMatrix();

        void resize( u32 colCount, u32 rowCount );
        void push( T const* __restrict__ src, u32 srcFrames );
        u32 rowCount() const;
        u32 columnCount() const;
        BBox1f getMinMax() const;
        const T* getRow(int32_t row) const;
        T getPixel(int32_t col, int32_t row, float defaultValue = 0.0f) const;

        static void
        shiftVectorLeft(TRowVector & orig, TRowVector & temp);

        static void
        shiftVectorRight(TRowVector & orig, TRowVector & temp);


        static void test();
        static void testShiftLeft();
        static void testShiftRight();
    };

} // end namespace de.

typedef de::AlignedFloatVector      DE_AlignedFloatVector;
typedef de::AlignedFloatShiftVector DE_AlignedFloatShiftVector;
typedef de::AlignedFloatShiftMatrix DE_AlignedFloatShiftMatrix;

struct DE_GuardedBuffer
{
    uint32_t pre = 0xDEADBEEF;
    DE_AlignedFloatVector data;
    uint32_t post = 0xCAFEBABE;
    char m_name[64];
    DE_GuardedBuffer(const char* name);
    ~DE_GuardedBuffer();
};
