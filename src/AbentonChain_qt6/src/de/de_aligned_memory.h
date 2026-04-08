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
        using value_type = T;

        // Rebind support
        template <typename U> struct rebind
        {
            using other = TAlignedVectorAllocator<U, Alignment>;
        };

        T* allocate(std::size_t n)
        {
            void* ptr = _aligned_malloc(n * sizeof(T), Alignment); // std::aligned_alloc(Alignment, n * sizeof(T));
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

        // Optional: equality operators
        bool operator==(const TAlignedVectorAllocator&) const noexcept { return true; }
        bool operator!=(const TAlignedVectorAllocator&) const noexcept { return false; }
    };

    //template <typename T>
    //class TAlignedVector : public std::vector<T, TAlignedVectorAllocator<T, 64>>
    //{};

    // AVX2 32-byte aligned vector
    template <typename T>
    using TAlignedVector = std::vector<T, TAlignedVectorAllocator<T, 32>>;

    // AVX512 64-byte aligned vector
    template <typename T>
    using TAlignedVector64 = std::vector<T, TAlignedVectorAllocator<T, 64>>;

    typedef TAlignedVector<f32> AlignedFloatVector;
    typedef TAlignedVector<u8>  AlignedByteVector;
    //typedef TAlignedVector<s16> AlignedShortVector;
    //typedef TAlignedVector<u32> AlignedImageVector;

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

        BBox1f getMinMax() const { return computeMinMax(m_data); }

        void resize( size_t desired )
        {
            if (m_data.size() != desired)
            {
                m_data.resize( desired );
                m_used = 0;
                m_free = m_data.size();

                // DE_WARN("desired = ",m_data.size())
            }
        }

        // push samples until vector full, emit signal to process old buffer, write to new buffer
        void push(const std::string & caller, const Vector & v, bool debug = false )
        {
            push(caller, v.data(), v.size(), debug);
        }

        // push samples at back of vector, destroy n older elements by shifting

        // TAlignedShiftVector<float>(8)
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // TAlignedShiftVector<float>(8).push([0,1,2,3,4,5,6])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   ?   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // TAlignedShiftVector<float>(8).push([7,8])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // TAlignedShiftVector<float>(8).push([A,B,C,D])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   5   |   6   |   7   |   8   |   A   |   B   |   C   |   D   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;
        // TAlignedShiftVector<float>(8).push([E,F,B])
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |   8   |   A   |   B   |   C   |   D   |   E   |   F   |   B   |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;

        void push(const std::string & caller,
                  const T* __restrict__ pSamples,
                  size_t nSamples,
                  bool debug = false )
        {
            //DE_OK("[",caller,"] size(", m_collect.size(),"), m_used(",m_used,")"
            //       " :: push(",nSamples,")")

            if (size() < nSamples)
            {
                resize(nSamples);
            }

            // if ( nSamples > m_collect.size() )
            // {
            //     DE_ERROR("caller(",caller,") -> ShiftBuffer too small "
            //                                 "m_data(", m_collect.size(),") < "
            //              "nSamples(",nSamples,"), "
            //              "used(",m_used,")")
            //     return;
            // }

            if (size() == nSamples)
            {
                std::memcpy(m_data.data(), pSamples, nSamples * sizeof(T));

                m_onFullVector(m_data);

                m_used = 0;

                return;
            }

            // TAlignedShiftVector<float>(8) v8: m_used = 0;
            // ++---+---+---+---++---+---+---+---++
            // ||   |   |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            // v8.push([0,1,2,3,4]) :: nSamples = 5, m_used = 5;
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | ? | ? | ? ||
            // ++---+---+---+---++---+---+---+---++

            // v8.push([5,6,7,8,9]) :: nSamples = 5, m_used = 8;

            // = v8.push([5,6,7]) :: nLeft = 3 = nSamples - (m_size - m_used);
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
            // ++---+---+---+---++---+---+---+---++

            // + v8.push([8,9]) :: nRight = 2 = nSamples - Left, m_used = 2;
            // ++---+---+---+---++---+---+---+---++
            // || 8 | 9 |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++

            // push([5,6,7]) :: n1 = 3 = nSamples - (m_size - m_used);
            // push([8,9]) :: n2 = 2 = nSamples - n1;

            if (nSamples + used() > size())
            {
                // DE_OK("END")
                // push([5,6,7,8,9]) :: nSamples = 5, nAvail = size(8)-used(5);
                int64_t nAvail = avail();
                // I. push([5,6,7]) :: nLeft = n - nAvail = 3;
                int64_t n1 = int64_t(nSamples) - nAvail;
                // II. push([8,9]) :: nRight = n - nLeft = 2;
                int64_t n2 = int64_t(nSamples) - n1;

                // I. Copy until end of row ... ( push([5,6,7]) )
                // ++---+---+---+---++---+---+---+---++
                // || 0 | 1 | 2 | 3 || 4 |[5]|[6]|[7]|| :: nLeft = 3 = nSamples - (m_size - m_used);
                // ++---+---+---+---++---+---+---+---++
                if (n1 > 0)
                {
                    const T* src = pSamples;  // src=[5,6,7|8,9]
                    T* dst = data() + used(); // dst=[0,1,2,3,4|?,?,?]
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
                    const T* src = pSamples + n1; // Read remain input token.
                    T* dst = data(); // Write to begin() of shiftbuffer.
                    std::memcpy( dst, src, n2 * sizeof(T) );
                    m_used = n2;
                }
            }
            else
            {
                // DE_OK("MID")
                const T* src = pSamples;
                T* dst = data() + used();
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

        /*
        void shiftLeft( u64 shifts, bool debug = false )
        {
            if ( shifts >= m_write->size() )
            {
                if ( debug )
                {
                    DE_ERROR("Nothing to shift(",shifts,"), m_write(",m_write->size(),")")
                }
                return; // Nothing todo, we can only overwrite entirely
            }

            if ( debug )
            {
                DE_DEBUG("Shift(",shifts,"), m_write(",m_write->size(),")")
            }

            for ( u64 i = shifts; i < m_write->size(); ++i )
            {
                m_collect[ i - shifts ] = m_write->m_data[ i ];
            }
        }

        void pushZero( size_t srcCount )
        {
            if ( m_index > m_data.size() )
            {
                DE_ERROR("m_fillCounter > m_dat.size()")
                return;
            }

            if ( srcCount + m_index >= m_data.size() )
            {
                srcCount = m_data.size() - m_index;
            }

            for ( u64 i = 0; i < srcCount; ++i )
            {
                m_data[ i + m_index ] = 0.0f;
            }

            m_index += srcCount;

            DE_ERROR("srcCount(",srcCount,"), m_index(",m_index,")")
        }
        */

        // push samples at back of vector, destroy n older elements by shifting
        void setCallback_onFullVector( FN_onFullVector const & onFullVector )
        {
            m_onFullVector = onFullVector;
        }

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
            testObj.push("Test(1)", TAlignedVector<T>{0,1,2,3,4});
            if (testObj.m_data != TAlignedVector<T>{0,1,2,3,4,0,0,0})
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
            testObj.push("Test(2)[n=8]_7_8",TAlignedVector<T>{7,8});
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
            testObj.push("Test(3)[n=8]_4_4_6_0",TAlignedVector<T>{4,4,6,0});

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
            testObj.push("Test(4)[n=8]_1_1_2",TAlignedVector<T>{1,1,2});
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
    template < typename T >
    // =======================================================================
    struct TAlignedShiftMatrix
    // =======================================================================
    {
        typedef TAlignedVector< T > TData;
        typedef std::vector< T* > TRowVector;

        TData m_data;
        TRowVector m_rows; // Row Viewer ( original rows )
        TRowVector m_view; // Row Viewer ( shuffled rows )

        u32 m_colCount;         // Count matrix cols = m_shiftBuffer.size()
        u32 m_rowCount;        // Count matrix rows
        u32 m_pushCount;        // Debug collection of new rows of samples
        u32 m_dummy;        // Count m_shiftBuffer fill status to control shift tigger.

        TAlignedShiftMatrix()
            : m_colCount( 1024 )
            , m_rowCount( 64 )
            , m_pushCount( 0 )
            , m_dummy( 0 )
        {
            m_data.resize( m_rowCount * m_colCount );
            m_rows.resize( m_rowCount );
            m_view.resize( m_rowCount );
            for (size_t i = 0; i < m_rowCount; i++)
            {
                auto rowPtr = &m_data[m_colCount*i];
                m_rows[ i ] = rowPtr;
                m_view[ i ] = rowPtr;
            }
        }

        BBox1f getMinMax() const
        {
            float lMin = std::numeric_limits< float >::max();
            float lMax = std::numeric_limits< float >::lowest();

            for ( const auto & f : m_data )
            {
                lMin = std::min( lMin, f );
                lMax = std::max( lMax, f );
            }

            return BBox1f(lMin,lMax);
        }

        const T* getRow(int32_t row) const
        {
            if (row < 0 || row >= m_rowCount )
            {
                //DE_WARN("row(",row,") >= rowCount(",m_rowCount,")")
                return nullptr;
            }
            return m_view[row]; // m_data.data() + row * m_colCount;
        }

        T getPixel(int32_t col, int32_t row, float defaultValue = 0.0f) const
        {
            if (col < 0 || col >= m_colCount )
            {
                //DE_WARN("col(",col,") >= colCount(",m_colCount,")")
                return defaultValue;
            }
            if (row < 0 || row >= m_rowCount )
            {
                //DE_WARN("row(",row,") >= rowCount(",m_rowCount,")")
                return defaultValue;
            }
            const T* pRow = getRow(row);
            return pRow[ col ];
        }

        bool getFrontVector( TData & oFront ) const
        {
            oFront.resize( columnCount() );
            for (size_t i = 0; i < columnCount(); i++ )
            {
                oFront[i] = getPixel(i,0);
            }
            return true;
        }

        bool getBackVector( TData & oBack ) const
        {
            oBack.resize( columnCount() );
            for (size_t i = 0; i < columnCount(); i++ )
            {
                oBack[i] = getPixel(columnCount() - 1 - i, rowCount() - 1);
            }
            return true;
        }

        bool getLeftVector( TData & oLeft ) const
        {
            oLeft.resize( rowCount() );
            for (size_t i = 0; i < rowCount(); i++ )
            {
                oLeft[i] = getPixel(0,rowCount() - 1 - i);
            }
            return true;
        }

        bool getRightVector( TData & oRight ) const
        {
            oRight.resize( rowCount() );
            for (size_t i = 0; i < rowCount(); i++ )
            {
                oRight[i] = getPixel(columnCount() - 1,i);
            }
            return true;
        }

        void resize( u32 colCount, u32 rowCount )
        {
            if ( colCount < 1 || rowCount < 1 )
            {
                DE_WARN("colCount < 1 || rowCount < 1")
                return;
            }

            if ( (m_rowCount != rowCount) || (m_colCount != colCount) )
            {
                m_data.resize( rowCount * colCount );
                m_rows.resize( rowCount );
                m_view.resize( rowCount );
                for ( size_t i = 0; i < rowCount; i++ )
                {
                    auto rowPtr = &m_data[colCount*i];
                    m_rows[ i ] = rowPtr;
                    m_view[ i ] = rowPtr;
                }
                m_colCount = colCount;
                m_rowCount = rowCount;
            }
        }

        u32 rowCount() const { return m_rowCount; }
        u32 columnCount() const { return m_colCount; }

        // Only mono channel data is allowed.
        void push( TAlignedVector<T> const& src )
        {
            push( src.data(), src.size() );
        }

        // Only mono channel data is allowed.
        void push( T const* src, u32 srcFrames )
        {
            if (!src) { DE_WARN("!src") return; }
            if (srcFrames < 1) { DE_WARN("srcFrames < 1") return; }
            resize( srcFrames, m_rowCount );
            m_pushCount++;

            auto firstRow = m_view[ 0 ];
            auto lastRow = m_view[ m_rowCount-1 ];

            for ( size_t i = 0; i < m_rowCount; ++i )
            {
                std::swap(m_view[ m_rowCount-2-i ],
                          m_view[ m_rowCount-1-i ]);
            }

            // New front:
            m_view[ 0 ] = lastRow;

            // New front: fill data from push()
            f32* dst = m_view.front();
            memcpy( dst, src, srcFrames * sizeof( f32 ));
        }

    };

    typedef TAlignedShiftMatrix<f32> AlignedFloatShiftMatrix;
    // typedef TAlignedShiftMatrix<u8>  AlignedByteShiftMatrix;

} // end namespace de.

typedef de::AlignedFloatVector      DE_AlignedFloatVector;
typedef de::AlignedFloatShiftVector DE_AlignedFloatShiftVector;
typedef de::AlignedFloatShiftMatrix DE_AlignedFloatShiftMatrix;


