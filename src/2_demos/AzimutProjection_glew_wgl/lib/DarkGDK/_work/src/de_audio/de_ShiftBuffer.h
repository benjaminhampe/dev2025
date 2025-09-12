#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <de_core/de_Logger.h>

namespace de {

// Collects (mono channel) data until full (m_shiftBuffer.size())
// If full the callback onFull( T* data, size_t n ) is triggered/called, if any.

// This callback is e.g. used to build a (2d/3d) geometry out of the data.
// After the callback the internal full counter index is reset to zero
// and collecting starts again.

// If we got more data than space avail (overflow), it triggers the onFull()
// callback, resets counter to the remaining samples.
// So no information loss, but possible performance loss due to synchronicity.
// Best is to use an exact multiple of your audio callback size.

// The input can be small, e.g. 256 samples per channel for realtime audio.
// So with 1024 shift buffer size we collect 4 audio callbacks
// then we trigger a new mesh from collection and reset m_frameIndex.

/////////////////////////////////////////////////////////////////////////////
template < typename T >
struct ShiftBuffer
/////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.ShiftBuffer")

   uint64_t m_fillCounter;
   std::vector< T > m_data; // TODO: use aligned memory.
   std::function< void( T*, size_t ) > m_callback_onFull;

   ShiftBuffer()
      : m_fillCounter( 0 )
      , m_callback_onFull( [] ( T*, size_t ) {} ) // empty lambda
   {}

   // Set user callback
   void setCallback( std::function< void( T*, size_t ) > onFull )
   {
      m_callback_onFull = std::move( onFull );
   }

   // Behave like std::vector< T >
   uint64_t size() const { return m_data.size(); }
   // Behave like std::vector< T >
   T const* data() const { return m_data.data(); }
   // Behave like std::vector< T >
   T*       data() { return m_data.data(); }
   // Behave like std::vector< T >
   T const& operator[] ( size_t i ) const { return m_data[ i ]; }
   // Behave like std::vector< T >
   T&       operator[] ( size_t i ) { return m_data[ i ]; }
   // Behave like std::vector< T >
   void resize( size_t elemCount )
   {
      DE_DEBUG("elemCount = ",elemCount)
      if ( elemCount != size() )
      {
         m_data.resize( elemCount );

         DE_DEBUG("m_data.size() = ",m_data.size())
         //m_copy.resize( elemCount );
         m_fillCounter = 0;
         //zeroMemory();
      }
   }
   // Behave like std::vector< T >
   void clear()
   {
      m_fillCounter = 0;
   }

   void fill( T const & fillValue )
   {
      for ( auto & f : m_data ) f = fillValue;
   }

   void pushZero( size_t srcCount )
   {
      if ( m_fillCounter > m_data.size() )
      {
         //DE_ERROR("m_fillCounter > m_data.size()")
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

      //DE_ERROR("srcCount(",srcCount,"), m_fillCounter(",m_fillCounter,")")
   }

   void shiftLeft( uint64_t shifts, bool debug = false )
   {
      //DE_ERROR("Shift(",shifts,"), m_fillCounter(",m_fillCounter,")")
      if ( shifts >= m_data.size() )
      {
         // if ( debug )
         // {
            // DE_ERROR("Nothing to shift(",shifts,"), m_data.size(",m_data.size(),")")
         // }
         return; // Nothing todo, we can only overwrite entirely
      }

      // if ( debug )
      // {
         // DE_DEBUG("Shift(",shifts,"), m_data.size(",m_data.size(),")")
      // }

      for ( uint64_t i = shifts; i < m_data.size(); ++i )
      {
         m_data[ i-shifts ] = m_data[ i ];
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

   // push samples at back of vector, destroy n older elements by shifting
   void push( std::string caller, float const* src, size_t srcCount, bool debug = false )
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
         memcpy( m_data.data() + m_fillCounter, src, srcCount );
         m_fillCounter = m_data.size();
         m_callback_onFull( m_data.data(), m_data.size() );
      }
      else
      {
         memcpy( m_data.data() + m_fillCounter, src, srcCount );
         m_fillCounter += srcCount;
      }
   }


   std::string
   toString() const
   {
      size_t n = m_data.size();
      std::stringstream s;
      s << "n = " << n << "\n";
      for ( uint64_t i = 0; i < n; ++i )
      {
         s << "[" << i << "] " << m_data[ i ] << "\n";
      }
      return s.str();
   }

   static void
   test()
   {
      ShiftBuffer testObj;
      testObj.resize( 20 );

      std::vector< T > a{ 0,1,2,3,4,5,6,7,8,9};
      std::vector< T > b{ 10,11,12,13,14,15,16,17,18,19};
      std::vector< T > c{ 20,21,22,23,24,25,26,27,28,29};
      //DE_DEBUG( "Test{0}.Resize(20): ", testObj.toString() )
      testObj.push( "test1", a.data(), 10, true );
      //DE_DEBUG( "Test{1}.Push(a): ", testObj.toString() )
      testObj.push( "test2", b.data(), 10, true );
      //DE_DEBUG( "Test{2}.Push(b): ", testObj.toString() )
      testObj.push( "test3", c.data(), 10, true );
      //DE_DEBUG( "Test{3}.Push(c): ", testObj.toString() )
   }
};

/* backup
// push samples at back of vector, destroy n older elements by shifting
void
ShiftBuffer::push( std::string caller, float const* src, size_t srcCount, bool debug = false,
   std::function< void() > const & onFull = [] () {} )
{
   auto n = m_data.size();
   if ( n < srcCount )
   {
      DE_ERROR("caller(",caller,") -> ShiftBuffer to small n(",n,"), srcCount(",srcCount,"), fillCounter(",m_fillCounter,")")
      //resize( srcCount );
      return;
   }

   if ( m_fillCounter < n ) // we can still push_back()/emplace_back()
   {
      auto freeSlots = std::min( n - m_fillCounter, srcCount );
      if ( freeSlots > 0 )
      {
         if ( debug )
         {
            DE_DEBUG("FreeSlots(",freeSlots,"), n(",n,"), srcCount(",srcCount,"), fillCounter(",m_fillCounter,")")
         }
         auto pDst = m_data.data() + m_fillCounter;
         for ( size_t i = 0; i < freeSlots; ++i )
         {
            *pDst = *src;
            pDst++;
            src++;
         }
         srcCount -= freeSlots;        // Update srcCount for check later
         m_fillCounter += freeSlots;
      }
   }
   else
   {
      onFull();
   }

   if ( srcCount > 0 )
   {
      if ( debug )
      {
         DE_DEBUG("ShiftLeft(",srcCount,"), n(",n,"), fillCounter(",m_fillCounter,")")
      }
      shiftLeft( srcCount, debug );
      DSP_COPY( src, m_data.data() + n - srcCount, srcCount );
   }
}
*/

} // end namespace de
