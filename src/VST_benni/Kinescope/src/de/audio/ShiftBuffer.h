#pragma once

#include <cstring>
#include <de/Core.h>

#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <functional>

namespace de {

// ----------------------------------------------------
//
// ----------------------------------------------------
template < typename T >
struct ShiftBuffer
{
    uint64_t m_fillCounter;
    std::vector< T > m_data;

    ShiftBuffer() : m_fillCounter(0) {}

    uint64_t size() const { return m_data.size(); }
    T const * data() const { return m_data.data(); }
    T * data() { return m_data.data(); }
    T const & operator[] ( size_t i ) const { return m_data[ i ]; }
    T & operator[] ( size_t i ) { return m_data[ i ]; }
    void resize( size_t elemCount )
    {
        if ( elemCount != size() )
        {
            m_data.resize( elemCount );
            //m_copy.resize( elemCount );
            m_fillCounter = 0;
            //zeroMemory();
        }
    }
    
    void clear()
    {
        m_fillCounter = 0;
    }
    
    void zeroMemory()
    {
        //for ( auto & f : m_copy ) f = 0.0f;
        for ( auto & f : m_data ) f = 0.0f;
    }

    void shiftLeft( uint64_t shifts, bool debug = false )
    {
        //DE_ERROR("Shift(",shifts,"), m_fillCounter(",m_fillCounter,")")
        if ( shifts >= m_data.size() )
        {
            if ( debug )
            {
                DE_ERROR("Nothing to shift(",shifts,"), m_data.size(",m_data.size(),")")
            }
            return; // Nothing todo, we can only overwrite entirely
        }

        if ( debug )
        {
            DE_DEBUG("Shift(",shifts,"), m_data.size(",m_data.size(),")")
        }

        for ( uint64_t i = shifts; i < m_data.size(); ++i )
        {
            m_data[ i - shifts ] = m_data[ i ];
        }

        //      if ( shifts > m_fillCounter )
        //      {
        //         m_fillCounter = 0;
        //      }
        //      else
        //      {
        //         m_fillCounter -= shifts;
        //      }
    }
    
    void pushZero( size_t srcCount )
    {
        if ( m_fillCounter > m_data.size() )
        {
            DE_ERROR("m_fillCounter > m_data.size()")
            return;
        }

        if ( srcCount + m_fillCounter >= m_data.size() )
        {
            srcCount = m_data.size() - m_fillCounter;
        }

        for ( uint64_t i = 0; i < srcCount; ++i )
        {
            m_data[ i + m_fillCounter ] = 0.0f;
        }

        m_fillCounter += srcCount;

        DE_ERROR("srcCount(",srcCount,"), m_fillCounter(",m_fillCounter,")")
    }

    // push samples at back of vector, destroy n older elements by shifting
    void push(  const std::string & caller, 
                const T* __restrict pSrc,
                size_t srcCount, 
                bool debug = false,
                std::function< void() > const & onFull = [] () {} )
    {
        if ( srcCount > m_data.size() )
        {
            DE_ERROR("caller(",caller,") -> ShiftBuffer to small n(", m_data.size(),"), srcCount(",srcCount,"), fillCounter(",m_fillCounter,")")
            //resize( srcCount );
            return;
        }

        if ( m_fillCounter > m_data.size() ) // we can still push_back()/emplace_back()
        {
            DE_ERROR("m_fillCounter(",m_fillCounter,") > m_data.size(",m_data.size(),")")
            m_fillCounter = m_data.size();
        }

        if ( srcCount + m_fillCounter > m_data.size() ) // we can still push_back()/emplace_back()
        {
            auto shifts = (m_fillCounter + srcCount) - m_data.size();
            shiftLeft( shifts, debug );
            m_fillCounter -= shifts;
            //DSP_COPY( src, m_data.data() + m_fillCounter, srcCount );
            std::memcpy( m_data.data() + m_fillCounter, pSrc, srcCount * sizeof(T));
            m_fillCounter = m_data.size();
            onFull();
        }
        else
        {
            //DSP_COPY( src, m_data.data() + m_fillCounter, srcCount );
            std::memcpy( m_data.data() + m_fillCounter, pSrc, srcCount * sizeof(T));
            m_fillCounter += srcCount;
        }
    }

    std::string str() const
    {
        std::stringstream o;
        
        o << "n = " << m_data.size() << "\n";
        for ( uint64_t i = 0; i < m_data.size(); ++i )
        {
            o << "[" << i << "] " << m_data[ i ] << "\n";
        }
        return o.str();
    }
};

struct ShiftBufferTest
{
    static void
    test()
    {
        typedef float T;

        ShiftBuffer<T> testObj;
        testObj.resize( 20 );

        std::vector< T > a{ 0,1,2,3,4,5,6,7,8,9};
        std::vector< T > b{ 10,11,12,13,14,15,16,17,18,19};
        std::vector< T > c{ 20,21,22,23,24,25,26,27,28,29};
        DE_DEBUG( "Test{0}.Resize(20): ", testObj.str() )
        testObj.push( "test1", a.data(), 10, true );
        DE_DEBUG( "Test{1}.Push(a): ", testObj.str() )
        testObj.push( "test2", b.data(), 10, true );
        DE_DEBUG( "Test{2}.Push(b): ", testObj.str() )
        testObj.push( "test3", c.data(), 10, true );
        DE_DEBUG( "Test{3}.Push(c): ", testObj.str() )
    }
};

} // end namespace de.
