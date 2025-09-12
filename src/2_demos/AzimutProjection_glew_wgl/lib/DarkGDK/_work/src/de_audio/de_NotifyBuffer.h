#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <de_core/de_Logger.h>
#include <de_core/de_RuntimeError.h>
#include <de_core/de_DebugUtils.h>

#include <de_audio/de_SampleChecker.h>

namespace de {

// Collects (mono channel) data until full.
// Notifies user when buffer is full using callback onFull( T* data, size_t n ).

// This callback can e.g. be used to build (2d/3d) geometry.

// After the notification the class switches front and back buffer,
// resets counter to zero and starts collecting data/samples again.

// Collecting is double buffered to reduce contention = concurrent access
// Maybe tripple or quad buffered will be added later.

// If we got more data than space avail (overflow), it triggers the onFull()
// callback, resets counter to the remaining samples.

// Best is to use an exact multiple of your audio callback size.

// The input can be small, e.g. 256 samples per channel for realtime audio.
// So with 1024 shift buffer size we collect 4 audio callbacks
// then we trigger a new mesh from collection and reset m_frameIndex.

/////////////////////////////////////////////////////////////////////////////
template < typename T >
struct NotifyBuffer
/////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.NotifyBuffer")

   uint32_t m_fillCounter;

   // Atleast double buffered, impl. this way to possibly add more buffers...
   uint32_t m_bufferIndex;
   std::vector< T* > m_pointers;

   // TODO: use aligned memory?!
   std::vector< T > m_front;
   std::vector< T > m_back;
   std::function< void( T*, size_t ) > m_notifyFullCallback;

   void notifyFull()
   {
      if ( !m_notifyFullCallback ) { DE_ERROR("No callback") return; }
      if ( !m_front.data() )       { DE_ERROR("!m_front.data()") return; }
      if ( !m_back.data() )        { DE_ERROR("!m_back.data()") return; }
      if ( m_pointers.size() != 2 ){ DE_ERROR("m_pointers.size() != 2") return; }
      if ( m_bufferIndex >= m_pointers.size() ) {
         DE_ERROR("m_bufferIndex >= m_pointers.size()") return;
      }

      T* p = m_pointers[ m_bufferIndex ];
      if ( p != m_front.data() && p != m_back.data() ) {
         DE_ERROR("p != m_front.data() && p != m_back.data()")
         return;
      }

      //de::SampleChecker::check(p,m_front.size(),"notifyFull");

      m_notifyFullCallback( p, m_front.size() );
   }

   uint8_t const* get_dst_beg() const
   {
      return reinterpret_cast< uint8_t const* >( m_pointers[ m_bufferIndex ] );
   }

   uint8_t const* get_dst_end() const
   {
      return get_dst_beg() + (m_front.size() * sizeof(T));
   }

   NotifyBuffer()
      : m_fillCounter( 0 )
      , m_bufferIndex( 0 )
      , m_notifyFullCallback( [] ( T*, size_t ) {} ) // empty lambda
   {
      m_pointers.resize( 2 );
      m_pointers[ 0 ] = nullptr;
      m_pointers[ 1 ] = nullptr;
//      m_front.resize( 1024, T(0) );
//      m_back.resize( 1024, T(0) );
//      m_pointers[ 0 ] = m_front.data();
//      m_pointers[ 1 ] = m_back.data();

   }

   // Set user callback
   void setNotifyFullCallback( std::function< void( T*, size_t ) > const & onFull )
   {
      m_notifyFullCallback = onFull;
   }

   // Behave like std::vector< T >
   //uint64_t size() const { return m_data.size(); }
   // Behave like std::vector< T >
   //T const* data() const { return m_data.data(); }
   // Behave like std::vector< T >
   //T*       data() { return m_data.data(); }
   // Behave like std::vector< T >
   //T const& operator[] ( size_t i ) const { return m_data[ i ]; }
   // Behave like std::vector< T >
   //T&       operator[] ( size_t i ) { return m_data[ i ]; }
   // Behave like std::vector< T >
   //void clear() { m_fillCounter = 0; }

   // Behave like std::vector< T >
   void resize( size_t n )
   {
      DE_DEBUG("elemCount = ",n)
      if ( n != m_front.size() )
      {
         m_pointers[ 0 ] = nullptr;
         m_pointers[ 1 ] = nullptr;
         m_front.resize( n, T(0) );
         m_back.resize( n, T(0) );
         m_pointers[ 0 ] = m_front.data();
         m_pointers[ 1 ] = m_back.data();
         m_bufferIndex = 0;
         //DE_DEBUG("size() = ",m_front.size())
         //m_copy.resize( elemCount );
         m_fillCounter = 0;
         //zeroMemory();
      }
   }

   // push samples at back of vector, destroy n older elements by shifting
   void
   push( T const* const src, size_t n )
   {
      size_t const s = m_front.size();
/*
      if ( m_pointers.size() < 2 )
      {
         DE_RUNTIME_ERROR( "", "m_pointers.size() < 2" )
      }

      // Nothing todo. ( Can happen on recursion )
      if (!pSrc || n < 1)
      {
         DE_ERROR("[recursion=",recursion,"] caller(",caller,") -> "
            "Nothing todo, s(",s,"), n(",n,"), fill(",m_fillCounter,"), buf(",m_bufferIndex,")")
         return;
      }

      // Validate input...
      if ( n > s )
      {
         DE_ERROR("[recursion=",recursion,"]caller(",caller,") -> "
            "NotifyBuffer too small, s(",s,"), n(",n,"), fill(",m_fillCounter,"), buf(",m_bufferIndex,")")
         //resize( srcCount );
         return;
      }

      // Validate buffer
      if ( m_fillCounter > s )
      {
         DE_RUNTIME_ERROR( "m_fillCounter > s", "Unexpected" )
      }
*/

      // Complicated case: Splitt into 2 calls n = nA + nB.
      // We can push some samples nA, then notify, then push remaining nB.
      if ( n + m_fillCounter > s )
      {
         // First bucket
         auto nA = s - m_fillCounter; // Compute nA samples until buffer is full...
         // Second bucket (remaining)
         auto nB = n - nA;

/*
         if ( nA > n )
         {
            DE_RUNTIME_ERROR("nA > n","")
         }

         if ( nB > n )
         {
            DE_RUNTIME_ERROR("nB > n","")
         }

         if ( nA + nB != n )
         {
            DE_RUNTIME_ERROR("nA + nB != n","")
         }
*/
         // Copy first bucket...
         T const* pSrc = src;
         T* pDst = m_pointers[ m_bufferIndex ];
         memcpy( pDst + m_fillCounter, pSrc, nA * sizeof(T) );

         // Notify full
         notifyFull();

         // Switch buffers
         ++m_bufferIndex;
         if ( m_bufferIndex >= m_pointers.size() ) m_bufferIndex = 0;

         // Reset
         m_fillCounter = 0;

         pSrc += nA;
         pDst = m_pointers[ m_bufferIndex ];
         memcpy( pDst + m_fillCounter, pSrc, nB * sizeof(T) );
         m_fillCounter += nB;
      }
      // Normal case: Add n samples, then check if full.
      // If full then notify caller, switch buffer and reset fillcounter.
      else
      {
         // Copy bucket...
         T const* pSrc = src;
         T* pDst = m_pointers[ m_bufferIndex ];
         memcpy( pDst + m_fillCounter, pSrc, n * sizeof(T) );
         m_fillCounter += n;

         // Check if full
         if ( m_fillCounter >= s )
         {
            if ( m_fillCounter > s )
            {
               DE_RUNTIME_ERROR("m_fillCounter > s","")
            }

            // Notify full
            notifyFull();

            // Switch buffers
            ++m_bufferIndex;
            if ( m_bufferIndex >= m_pointers.size() ) m_bufferIndex = 0;

            // Reset
            m_fillCounter = 0;
         }
      }
   }

/*
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
*/

   static void
   test()
   {
      NotifyBuffer testObj;
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
NotifyBuffer::push( std::string caller, float const* src, size_t srcCount, bool debug = false,
   std::function< void() > const & onFull = [] () {} )
{
   auto n = m_data.size();
   if ( n < srcCount )
   {
      DE_ERROR("caller(",caller,") -> NotifyBuffer to small n(",n,"), srcCount(",srcCount,"), fillCounter(",m_fillCounter,")")
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
