#include <de/de_aligned_memory.h>
#include <de/Core.h>

namespace de {

// static
void AlignedFloatShiftMatrix::test()
{
    testShiftRight();
}
// static
void AlignedFloatShiftMatrix::testShiftLeft()
{
    auto dump = []( std::string msg, const TRowVector& v )
    {
        auto n = v.size();
        DE_DEBUG(msg, " :: n = ",n)
        for (size_t i = 0; i < n; ++i)
        {
            DE_DEBUG(msg,"[",i,"] ", dbHex(reinterpret_cast<uint64_t>(v[i])))
        }
    };

    TRowVector ori{ (T*)0x05, (T*)0x04, (T*)0x03, (T*)0x02, (T*)0x01 };
    TRowVector tmp{ (T*)0x05, (T*)0x04, (T*)0x03, (T*)0x02, (T*)0x01 };

    DE_OK("[Test] ShiftLeft:")
    dump("Before-Ori",ori);
    dump("Before-Tmp",tmp);
    shiftVectorRight(ori,tmp);
    dump("After-Ori",ori);
    dump("After-Tmp",tmp);
}
// static
void AlignedFloatShiftMatrix::shiftVectorLeft(TRowVector & orig, TRowVector & temp)
{
    const auto n = orig.size();
    if (n < 2)
    {
        DE_WARN("Vector is tiny ", n)
        return;
    }

    //   ori |5|4|3|2|1|0|
    //   tmp |5|4|3|2|1|0|
    // = tmp |4|3|2|1|0|x|
    std::memcpy(temp.data(), orig.data() + 1, sizeof(T*) * (n-1));

    // = tmp |4|3|2|1|0|5|
    temp[n-1] = orig[0]; // last elem swaps around and becomes first elem.

    // Make 'temp' the new 'orig'...
    std::swap(orig,temp);
}

// static
void AlignedFloatShiftMatrix::testShiftRight()
{
    auto dump = []( std::string msg, const TRowVector& v )
    {
        auto n = v.size();
        DE_DEBUG(msg, " :: n = ",n)
        for (size_t i = 0; i < n; ++i)
        {
            DE_DEBUG(msg,"[",i,"] ", dbHex(reinterpret_cast<uint64_t>(v[i])))
        }
    };

    TRowVector ori{ (T*)0x01, (T*)0x02, (T*)0x03, (T*)0x04, (T*)0x05 };
    TRowVector tmp{ (T*)0x01, (T*)0x02, (T*)0x03, (T*)0x04, (T*)0x05 };

    DE_OK("[Test] ShiftRight:")
    dump("Before-Ori",ori);
    dump("Before-Tmp",tmp);
    shiftVectorRight(ori,tmp);
    dump("After-Ori",ori);
    dump("After-Tmp",tmp);
}

// static
void AlignedFloatShiftMatrix::shiftVectorRight(TRowVector & orig, TRowVector & temp)
{
    const auto n = orig.size();
    if (n < 2)
    {
        DE_WARN("Vector is tiny ", n)
        return;
    }

    //   ori |0|1|2|3|4|5|
    //   tmp |0|1|2|3|4|5|
    // = tmp |x|0|1|2|3|4|
    std::memcpy(temp.data() + 1, orig.data(), sizeof(T*) * (n-1));

    // = tmp |5|0|1|2|3|4|
    temp[0] = orig[n-1]; // last elem swaps around and becomes first elem.

    // Make 'temp' the new 'orig'...
    std::swap(orig,temp);
}


// =======================================================================
AlignedFloatShiftMatrix::AlignedFloatShiftMatrix()
    : m_colCount( 1024 )
    , m_rowCount( 64 )
    //, m_pushCount( 0 )
    //, m_dummy( 0 )
{
    test();

    m_data.resize( m_rowCount * m_colCount );
    m_rows.resize( m_rowCount );
    m_temp.resize( m_rowCount );
    for (size_t i = 0; i < m_rowCount; i++)
    {
        auto rowPtr = &m_data[m_colCount*i];
        m_rows[ i ] = rowPtr;
        m_temp[ i ] = rowPtr;
    }
}

u32 AlignedFloatShiftMatrix::rowCount() const { return m_rowCount; }
u32 AlignedFloatShiftMatrix::columnCount() const { return m_colCount; }

BBox1f AlignedFloatShiftMatrix::getMinMax() const
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

const AlignedFloatShiftMatrix::T*
AlignedFloatShiftMatrix::getRow(int32_t row) const
{
    if (row < 0 || row >= m_rowCount )
    {
        //DE_WARN("row(",row,") >= rowCount(",m_rowCount,")")
        return nullptr;
    }
    return m_rows[row]; // m_data.data() + row * m_colCount;
}

AlignedFloatShiftMatrix::T
AlignedFloatShiftMatrix::getPixel(int32_t col, int32_t row, float defaultValue ) const
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

void AlignedFloatShiftMatrix::resize( u32 colCount, u32 rowCount )
{
    if ( colCount < 1 || rowCount < 1 )
    {
        DE_WARN("colCount < 1 || rowCount < 1")
        return;
    }

    if ((m_colCount != colCount) || (m_rowCount != rowCount))
    {
        m_data.resize( rowCount * colCount );
        m_rows.resize( rowCount );
        m_temp.resize( rowCount );
        for ( size_t i = 0; i < rowCount; i++ )
        {
            auto rowPtr = &m_data[colCount*i];
            m_rows[ i ] = rowPtr;
            m_temp[ i ] = rowPtr;
        }
        m_colCount = colCount;
        m_rowCount = rowCount;
        DE_WARN("cols(",m_colCount,"), rows(",m_rowCount,")")
    }
}


// Only mono channel data is allowed.
void AlignedFloatShiftMatrix::push( T const* __restrict__ src, u32 srcFrames )
{
    if (srcFrames < 1) { DE_WARN("srcFrames < 1") return; }
    resize( srcFrames, m_rowCount );

    // Shift 'orig' and store in 'view'...
    shiftVectorRight(m_rows,m_temp);

#if 0
    // <shift-right>
    auto firstRow = m_view[ 0 ];
    auto lastRow = m_view[ m_rowCount-1 ];

    for ( size_t i = 0; i < m_rowCount; ++i )
    {
        std::swap(m_view[ m_rowCount-2-i ],
                  m_view[ m_rowCount-1-i ]);
    }

    // New front:
    m_view[ 0 ] = lastRow;
    // </shift-right>
#endif

    // New front: fill data from push()
    f32* dst = m_rows.front();
    memcpy( dst, src, srcFrames * sizeof( f32 ));


}

} // end namespace de.




#if 0


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
        TRowVector m_temp; // Row Viewer ( shuffled rows )

        u32 m_colCount;  // Count matrix cols = m_shiftBuffer.size()
        u32 m_rowCount;  // Count matrix rows
        // u32 m_pushCount; // Debug collection of new rows of samples
        // u32 m_dummy;     // Count m_shiftBuffer fill status to control shift tigger.

        TAlignedShiftMatrix()
            : m_colCount( 1024 )
            , m_rowCount( 64 )
            //, m_pushCount( 0 )
            //, m_dummy( 0 )
        {
            m_data.resize( m_rowCount * m_colCount );
            m_rows.resize( m_rowCount );
            m_temp.resize( m_rowCount );
            for (size_t i = 0; i < m_rowCount; i++)
            {
                auto rowPtr = &m_data[m_colCount*i];
                m_rows[ i ] = rowPtr;
                m_temp[ i ] = rowPtr;
            }
        }

        u32 rowCount() const { return m_rowCount; }
        u32 columnCount() const { return m_colCount; }

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
            return m_rows[row]; // m_data.data() + row * m_colCount;
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

        void resize( u32 colCount, u32 rowCount )
        {
            if ( colCount < 1 || rowCount < 1 )
            {
                DE_WARN("colCount < 1 || rowCount < 1")
                return;
            }

            if ((m_colCount != colCount) || (m_rowCount != rowCount))
            {
                m_data.resize( rowCount * colCount );
                m_rows.resize( rowCount );
                m_temp.resize( rowCount );
                for ( size_t i = 0; i < rowCount; i++ )
                {
                    auto rowPtr = &m_data[colCount*i];
                    m_rows[ i ] = rowPtr;
                    m_temp[ i ] = rowPtr;
                }
                m_colCount = colCount;
                m_rowCount = rowCount;
                DE_WARN("cols(",m_colCount,"), rows(",m_rowCount,")")
            }
        }

        // Only mono channel data is allowed.
        void push( TAlignedVector<T> const& src )
        {
            push( src.data(), src.size() );
        }



#if 0
        static void shiftVectorRight(std::vector<T> & orig, std::vector<T> & view)
        {
            const auto n = v.size();
            if (n < 2)
            {
                DE_WARN("Vector is tiny ", n)
                return;
            }

            auto firstRow = v[ 0 ];
            auto lastRow = v[ n-1 ];

            for ( size_t i = 0; i < n-1; ++i )
            {
                // i=0, a=n-2, b=n-1; -> replace last with last-1
                std::swap(v[ n-2-i ], v[ n-1-i ]);
            }

            // New front:
            v[ 0 ] = lastRow;
            // </shift-right>
        }
#endif

        static void shiftVectorRight(TRowVector & orig, TRowVector & temp)
        {
            const auto n = orig.size();
            if (n < 2)
            {
                DE_WARN("Vector is tiny ", n)
                return;
            }

            std::memcpy(temp.data(), orig.data() + 1, sizeof(T) * (n-1));
            temp[0] = orig[n-1]; // last elem swaps around and becomes first elem.

            // Make 'temp' the new 'orig'...
            std::swap(orig,temp);
        }

        // Only mono channel data is allowed.
        void push( T const* __restrict__ src, u32 srcFrames )
        {
            if (srcFrames < 1) { DE_WARN("srcFrames < 1") return; }
            resize( srcFrames, m_rowCount );

            // Shift 'orig' and store in 'view'...
            shiftVectorRight(m_rows,m_temp);

#if 0
            // <shift-right>
            auto firstRow = m_view[ 0 ];
            auto lastRow = m_view[ m_rowCount-1 ];

            for ( size_t i = 0; i < m_rowCount; ++i )
            {
                std::swap(m_view[ m_rowCount-2-i ],
                          m_view[ m_rowCount-1-i ]);
            }

            // New front:
            m_view[ 0 ] = lastRow;
            // </shift-right>
#endif

            // New front: fill data from push()
            f32* dst = m_rows.front();
            memcpy( dst, src, srcFrames * sizeof( f32 ));


        }
    };

    typedef TAlignedShiftMatrix<f32> AlignedFloatShiftMatrix;

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

        u32 m_colCount;  // Count matrix cols = m_shiftBuffer.size()
        u32 m_rowCount;  // Count matrix rows
        // u32 m_pushCount; // Debug collection of new rows of samples
        // u32 m_dummy;     // Count m_shiftBuffer fill status to control shift tigger.

        TAlignedShiftMatrix()
            : m_colCount( 1024 )
            , m_rowCount( 64 )
            //, m_pushCount( 0 )
            //, m_dummy( 0 )
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

        u32 rowCount() const { return m_rowCount; }
        u32 columnCount() const { return m_colCount; }

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

            if ((m_colCount != colCount) || (m_rowCount != rowCount))
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
                DE_WARN("cols(",m_colCount,"), rows(",m_rowCount,")")
            }
        }

        // Only mono channel data is allowed.
        void push( TAlignedVector<T> const& src )
        {
            push( src.data(), src.size() );
        }



#if 0
        static void shiftVectorRight(std::vector<T> & orig, std::vector<T> & view)
        {
            const auto n = v.size();
            if (n < 2)
            {
                DE_WARN("Vector is tiny ", n)
                return;
            }

            auto firstRow = v[ 0 ];
            auto lastRow = v[ n-1 ];

            for ( size_t i = 0; i < n-1; ++i )
            {
                // i=0, a=n-2, b=n-1; -> replace last with last-1
                std::swap(v[ n-2-i ], v[ n-1-i ]);
            }

            // New front:
            v[ 0 ] = lastRow;
            // </shift-right>
        }
#endif

        static void shiftVectorRight(TRowVector & orig, TRowVector & view)
        {
            const auto n = orig.size();
            if (n < 2)
            {
                DE_WARN("Vector is tiny ", n)
                return;
            }

            std::memcpy(view.data() + 1, orig.data(), sizeof(T) * (n-1));
            view[0] = orig[n-1]; // last elem swaps around and becomes first elem.
        }

        // Only mono channel data is allowed.
        void push( T const* __restrict__ src, u32 srcFrames )
        {
            if (srcFrames < 1) { DE_WARN("srcFrames < 1") return; }
            resize( srcFrames, m_rowCount );

            // Make 'view' the new 'orig'...
            std::swap(m_rows,m_view);

            // Shift 'orig' and store in 'view'...
            shiftVectorRight(m_rows,m_view);

#if 0
            // <shift-right>
            auto firstRow = m_view[ 0 ];
            auto lastRow = m_view[ m_rowCount-1 ];

            for ( size_t i = 0; i < m_rowCount; ++i )
            {
                std::swap(m_view[ m_rowCount-2-i ],
                          m_view[ m_rowCount-1-i ]);
            }

            // New front:
            m_view[ 0 ] = lastRow;
            // </shift-right>
#endif

            // New front: fill data from push()
            f32* dst = m_view.front();
            memcpy( dst, src, srcFrames * sizeof( f32 ));


        }
    };

#endif