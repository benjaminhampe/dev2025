#pragma once
#include <de/midi/file/Range.hpp>
#include <vector>

namespace de {
namespace midi {
namespace file {

// Standard Midi Event CC - Controller Change
// =======================================================================
struct ControlChangeEvent
// =======================================================================
{
   uint64_t m_tick;
   int32_t m_cc;
   int32_t m_value;

   ControlChangeEvent() : m_tick(0), m_cc(-1), m_value(0.f)
   {

   }

   std::string
   toString() const
   {
      std::ostringstream o;
      o << "tick(" << m_tick << "), cc(" << m_cc << "), value(" << m_value << ")";
      return o.str();
   }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de