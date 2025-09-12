#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct NoteEvent
// =======================================================================
{
   int m_channel;
   int m_midiNote;
   int m_attack;
   uint64_t m_attackMs;
   int m_release;
   uint64_t m_releaseMs;

   NoteEvent()
      : m_channel(0), m_midiNote(0)
      , m_attack(0), m_attackMs(0)
      , m_release(0), m_releaseMs(0)
   {}

   NoteEvent(int channel, int midiNote,
            int attack, uint64_t attackMs,
            int release, uint64_t releaseMs)
      : m_channel(0), m_midiNote(midiNote)
      , m_attack(attack), m_attackMs(attackMs)
      , m_release(release), m_releaseMs(releaseMs)
   {}


   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }

   uint64_t
   duration() const { return m_releaseMs - m_attackMs; }

   std::string
   toString() const
   {
      std::ostringstream o; o <<
      "channel(" << m_channel << "), "
      "midiNote(" << m_midiNote << "), "
      "velocity(" << m_attack << "," << m_release << "), "
      "duration(" << m_attackMs << "," << m_releaseMs << ")";
      return o.str();
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de