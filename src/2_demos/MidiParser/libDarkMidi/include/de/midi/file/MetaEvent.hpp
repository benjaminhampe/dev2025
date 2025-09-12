#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// For all meta text events
// =======================================================================
struct MetaEvent
// =======================================================================
{
   uint64_t m_tick;
   int32_t m_meta;   // metaType
   std::string m_text;

   MetaEvent() : m_tick(0), m_meta(-1) {}

   std::string toString() const
   {
      std::ostringstream o;
      o << "tick(" << m_tick << "), metaType(" << m_meta << "), text(" << m_text << ")";
      return o.str();
   }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_text.capacity() * sizeof( char );
      return n;
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de
