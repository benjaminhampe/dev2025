#pragma once
#include <de/midi/file/ControlChangeEvent.hpp>
#include <vector>

namespace de {
namespace midi {
namespace file {

// Standard Midi File ControlChangeEventMap - A list of CC events
// =======================================================================
struct ControlChangeEventMap
// =======================================================================
{
   int m_cc;

   std::vector< ControlChangeEvent > m_events;

   explicit ControlChangeEventMap( int cc = -1 ) : m_cc(cc)
   {}

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_events.capacity() * sizeof( ControlChangeEvent );
      return n;
   }

   ControlChangeEvent*
   addEvent( ControlChangeEvent const & mce )
   {
      m_events.emplace_back( mce );
      return &m_events.back();
   }

   std::string
   toString() const
   {
      std::ostringstream o;
      o << "cc(" << m_cc << "), events(" << m_events.size() << ")";
      return o.str();
   }

   Range<int> getValueRange() const
   {
      Range<int> range;
      for ( size_t i = 0; i < m_events.size(); ++i )
      {
         ControlChangeEvent const & e = m_events[ i ];
         range.min = std::min( e.m_value, range.min );
         range.max = std::max( e.m_value, range.max );
      }
      return range;
   }

   Range<uint64_t> getTickRange() const
   {
      Range<uint64_t> range;
      for ( size_t i = 0; i < m_events.size(); ++i )
      {
         ControlChangeEvent const & e = m_events[ i ];
         range.min = std::min( e.m_tick, range.min );
         range.max = std::max( e.m_tick, range.max );
      }
      return range;
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de
