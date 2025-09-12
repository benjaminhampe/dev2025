#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct TimeSignatureEvent
// =======================================================================
{
   uint64_t m_tick;
   int m_top;
   int m_bottom;
   int m_clocksPerBeat;
   int m_n32rdPerBar;

   TimeSignatureEvent()
      : m_tick(0)
      , m_top(-1)
      , m_bottom(-1)
      , m_clocksPerBeat(-1)
      , m_n32rdPerBar(-1)
   {}

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }

   void set( int top, int bottom, int clocksPerBeat, int n32rd_per_bar )
   {
      m_top = top;
      m_bottom = bottom;
      m_clocksPerBeat = clocksPerBeat;
      m_n32rdPerBar = n32rd_per_bar;
   }

   bool
   isValid() const
   {
      if (m_top < 1) return false;
      if (m_bottom < 1) return false;
      if (m_clocksPerBeat < 1) return false;
      if (m_n32rdPerBar < 1) return false;
      return true;
   }

   std::string
   toString() const
   {
      std::ostringstream o; o <<
      "tick(" << m_tick << "), "
      "takt(" << m_top << "," << m_bottom << "), "
      "clocksPerBeat(" << m_top << "), "
      "num32rdPerBar(" << m_n32rdPerBar << ")";
      return o.str();
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de