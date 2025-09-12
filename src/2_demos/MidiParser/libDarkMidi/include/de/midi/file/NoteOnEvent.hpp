#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct NoteOnEvent
// =======================================================================
{
   uint64_t m_tick;
   uint8_t m_channel;
   uint8_t m_midiNote;
   uint8_t m_velocity;
   uint8_t m_dummy;

   NoteOnEvent()
      : m_tick(0)
      , m_channel(0)
      , m_midiNote(0)
      , m_velocity(0)
   {}

   NoteOnEvent( uint64_t tick, int channel, int midiNote, int velocity )
      : m_tick(tick)
      , m_channel(channel)
      , m_midiNote(midiNote)
      , m_velocity(velocity)
   {}

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }

   std::string
   toString() const
   {
      std::ostringstream o; o << 
      "tick(" << m_tick << "), channel(" << m_channel << "), "
      "note(" << m_midiNote << "), velocity(" << m_velocity << ")";
      return o.str();
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de