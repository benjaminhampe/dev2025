#include <de/audio/dsp/ShiftBuffer.hpp>

namespace de {
namespace audio {

ShiftBuffer::ShiftBuffer()
   : m_fillCounter( 0 )
{}

uint64_t
ShiftBuffer::size() const { return m_data.size(); }

float const *
ShiftBuffer::data() const { return m_data.data(); }

float*
ShiftBuffer::data() { return m_data.data(); }

float const &
ShiftBuffer::operator[] ( size_t i ) const { return m_data[ i ]; }

float &
ShiftBuffer::operator[] ( size_t i ) { return m_data[ i ]; }

void
ShiftBuffer::resize( size_t elemCount )
{
   if ( elemCount != size() )
   {
      m_data.resize( elemCount );
      //m_copy.resize( elemCount );
      m_fillCounter = 0;
      //zeroMemory();
   }
}


void
ShiftBuffer::clear()
{
   m_fillCounter = 0;
}

void
ShiftBuffer::zeroMemory()
{
   //for ( auto & f : m_copy ) f = 0.0f;
   for ( auto & f : m_data ) f = 0.0f;
}

void
ShiftBuffer::shiftLeft( uint64_t shifts, bool debug )
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

void
ShiftBuffer::pushZero( size_t srcCount )
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
void
ShiftBuffer::push( std::string caller, float const* src, size_t srcCount, bool debug,
                std::function< void() > const & onFullRow)
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
      DSP_COPY( src, m_data.data() + m_fillCounter, srcCount );
      m_fillCounter = m_data.size();
      onFullRow();
   }
   else
   {
      DSP_COPY( src, m_data.data() + m_fillCounter, srcCount );
      m_fillCounter += srcCount;
   }
}

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


std::string
ShiftBuffer::toString() const
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

void
ShiftBuffer::test()
{
   ShiftBuffer testObj;
   testObj.resize( 20 );

   std::vector< T > a{ 0,1,2,3,4,5,6,7,8,9};
   std::vector< T > b{ 10,11,12,13,14,15,16,17,18,19};
   std::vector< T > c{ 20,21,22,23,24,25,26,27,28,29};
   DE_DEBUG( "Test{0}.Resize(20): ", testObj.toString() )
   testObj.push( "test1", a.data(), 10, true );
   DE_DEBUG( "Test{1}.Push(a): ", testObj.toString() )
   testObj.push( "test2", b.data(), 10, true );
   DE_DEBUG( "Test{2}.Push(b): ", testObj.toString() )
   testObj.push( "test3", c.data(), 10, true );
   DE_DEBUG( "Test{3}.Push(c): ", testObj.toString() )
}

} // end namespace audio
} // end namespace de
