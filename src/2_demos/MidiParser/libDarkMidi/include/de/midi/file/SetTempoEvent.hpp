#pragma once
#include <cstdint>
#include <cmath>        // std::round()
//#include <algorithm>
#include <de/Logger.hpp>

namespace de {
namespace midi {
namespace file {

// Item struct of the TempoMap:
// Stores an interval where m_bpm is active.
// Consequent intervals of same m_bpm are combined into one to ease our life.
//
// Some formulas:
//
// I:
// + int microsecondsPerQuarterNote = int( std::round( double(60000000.0) / double(bpm) ) );
// + double bpm = double(60000000.0) / double(microsecondsPerQuarterNote);
//
// II:
// + double secondsPerTick = double(m_microsecondsPerQuarterNote) /(1000000.0 * ticksPerQuarterNote);
// + double timeInSeconds = double(m_duration) * secondsPerTick;
//
// III: older stuff
//
// + double beatsPerMinute = (60 * 1000 * 1000) / microsPerQuarterNote;
//
// + double microsPerQuarterNote = (60 * 1000 * 1000) / bpm;
//
// + double microsPerTick = microsPerQuarterNote / ticksPerQuarterNote;
//
// + double secondsPerTick = microsPerQuarterNote / 1000000.0 / ticksPerQuarterNote
//                      = microsPerQuarterNote /(1000000.0 * ticksPerQuarterNote)
// + double ticks = resolution * (1 / tempo) * 1000 * elapsed_time
//
// getTempoBPM -- Returns the tempo in terms of beats per minute.
//                return 60000000.0 / (double)microseconds;
// getTempoTPS -- Returns the tempo in terms of ticks per seconds.
//                return tpq * 1000000.0 / (double)microseconds;
// getTempoSPT -- Returns the tempo in terms of seconds per tick.
//                return (double)microseconds / 1000000.0 / tpq;

// =======================================================================
struct SetTempoEvent
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.file.SetTempoEvent")

   uint64_t m_tickStart;               // Given by SetTempo event
   uint64_t m_tickDuration;            // Computed using following SetTempo events.
   int m_microsecondsPerQuarterNote;   // in [us/beat] Given by SetTempo event
   float m_bpm;                        // in [beat/s]  Given by SetTempo event
   double m_timeStart;                 // in [s] Computed using previous SetTempo events.
   double m_timeDuration;              // in [s] Computed using following SetTempo/NoteOff events.

   SetTempoEvent()
      : m_tickStart(0)
      , m_tickDuration(0)
      , m_microsecondsPerQuarterNote(0)
      , m_bpm(0.0f)
      , m_timeStart(0.0)
      , m_timeDuration(0.0)
   {}

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }

   static SetTempoEvent
   create( uint64_t tick, float bpm, int microsecondsPerQuarterNote = 0 )
   {
      SetTempoEvent e;
      e.m_tickStart = tick;
      e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
      e.m_bpm = bpm;

      // Compute missing second input...
      if ( microsecondsPerQuarterNote < 1 )
      {
         e.m_microsecondsPerQuarterNote = int( std::round( double(60000000.0) / double(bpm) ) );
      }
      else
      {
         // ... or validate given second input
         int const check = int( std::round( double(60000000.0) / double(bpm) ) );
         if ( check != e.m_microsecondsPerQuarterNote )
         {
            DE_ERROR("check(",check,") != e.m_microsecondsPerQuarterNote(",e.m_microsecondsPerQuarterNote,")")
         }
      }

      return e;
   }

   bool
   isValid() const { return m_microsecondsPerQuarterNote > 0; }

   std::string
   toString() const
   {
      std::ostringstream o; o <<
      "TickStart(" << m_tickStart << "), TickDuration("<< m_tickDuration<<"), "
      "bpm(" << m_bpm << "), ysPerBeat(" << m_microsecondsPerQuarterNote << "), "
      "TimeStart(" << m_timeStart << "), TimeDuration(" << m_timeDuration << ")";
      return o.str();
   }

   /*
   double
   computeDurationInSec( int ticksPerQuarterNote, float speed = 1.0f ) const
   {
      return MP_Cfg::computeDurationInSec( m_tickDuration, m_microsecondsPerQuarterNote );
   }

   double
   computeDurationInSeconds( int ticksPerQuarterNote ) const
   {
      if ( ticksPerQuarterNote < 1 ) return 0.0;
      if ( m_microsecondsPerQuarterNote < 1 ) return 0.0;
      if ( m_duration < 1 ) return 0.0;
      double secondsPerTick = double(m_microsecondsPerQuarterNote) /(1000000.0 * ticksPerQuarterNote);
      double elapsedSeconds = double(m_duration) * secondsPerTick;
      return elapsedSeconds;
   }


   */
};

} // end namespace file
} // end namespace midi
} // end namespace de
