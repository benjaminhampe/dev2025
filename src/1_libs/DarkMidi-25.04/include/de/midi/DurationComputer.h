#pragma once
#include <de/midi/Parser.h>
#include <assert.h>

namespace de {
namespace midi {

//------------------------------------------------------------------------
// MIDI Clock Information:
//------------------------------------------------------------------------
//
// + microseconds per tick = microseconds per quarter note / ticks per quarter note
//
// + ticks = resolution * (1 / tempo) * 1000 * elapsed_time
//
// - resolution in ticks/beat (or equivalently ticks/Quarter note).
//   This fixes the smallest time interval to be generated.
// - tempo in microseconds per beat,
//   which determines how many ticks are generated in a set time interval.
// - elapsed_time which provides the fixed timebase for playing the midi events.
//
//    ticks   ticks   beat   1000 us    ms
//    ----- = ----- * ---- * ------- * ----
//    time    beat     us       ms     time
//
// - time is the elapsed time between calls to the tick generator. This must be calculated by the tick generator based on the history of the previous call to the tick generator.
// - tempo is the tempo determined by the Set Tempo MIDI event. Note this event only deals in Quarter Notes.
// - resolution is held as TicksPerQuarterNote.
//

// =======================================================================
struct DurationComputer_SetTempoEvent
// =======================================================================
{
   uint64_t m_tick;
   int64_t m_duration; // in raw ticks
   int m_microsecondsPerQuarterNote;

   DurationComputer_SetTempoEvent()
      : m_tick(0)
      , m_duration(-1)
      , m_microsecondsPerQuarterNote(-1)
   {}

   static DurationComputer_SetTempoEvent
   fromBPM( uint64_t tick, double bpm )
   {
      DurationComputer_SetTempoEvent e;
      e.m_tick = tick;
      e.m_microsecondsPerQuarterNote = int( std::round( double(60000000.0) / bpm ) );
      return e;
   }

   bool
   isValid() const
   {
      return m_microsecondsPerQuarterNote > 0;
   }

   double
   computeBPM() const
   {
      if ( !isValid() ) return 0.0;
      return double(60000000.0) / double(m_microsecondsPerQuarterNote);
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

   std::string
   toString() const
   {
      std::ostringstream o; o <<
      "tick(" << m_tick << "), "
      "duration("<<m_duration<<"), "
      "bpm(" << computeBPM() << "), "
      "microsecondsPerQuarterNote(" << m_microsecondsPerQuarterNote << ")";
      return o.str();
   }
};


// =======================================================================
struct DurationComputer : public IParserListener
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.DurationComputer")
   int m_ticksPerQuarterNote = 96; // = ticks per beat
   int m_top = 4;
   int m_bottom = 4;
   int m_clocksPerBeat = 24;
   int m_n32rd_per_beat = 8;
   int m_microsecondsPerQuarterNote = 646000; // = microseconds per beat

   std::vector< DurationComputer_SetTempoEvent > m_setTempoEvents;

   uint64_t m_tickMin = std::numeric_limits< uint64_t >::max();
   uint64_t m_tickMax = std::numeric_limits< uint64_t >::lowest();

   DurationComputer() {}
   ~DurationComputer() override {}

   void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote ) override
   {
      (void)fileType;
      (void)trackCount;
      m_ticksPerQuarterNote = ticksPerQuarterNote;
   }

   // Finalize duration of last SetTempo() event
   void mpEnd() override
   {
      size_t const eventCount = m_setTempoEvents.size();

      // Insert atleast one SetTempoEvent that spans entire song.
      if ( eventCount == 0 )
      {
         DurationComputer_SetTempoEvent e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default MIDI bpm value.
         e.m_duration = int64_t( m_tickMax );
         m_setTempoEvents.emplace_back( std::move(e) );
      }
      // or finalize last SetTempo event's duration.
      else if ( eventCount == 1 )
      {
         DurationComputer_SetTempoEvent & e = m_setTempoEvents.back();
         e.m_duration = int64_t( m_tickMax );
      }
      // or finalize last SetTempo event's duration using its predecessor event.
      else
      {
         DurationComputer_SetTempoEvent & e = m_setTempoEvents.back();
         e.m_duration = int64_t( m_tickMax ) - int64_t( e.m_tick );
      }

      DE_DEBUG("SetTempoEvents.Count = ",eventCount)
   }

   void mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerQuarterNote ) override
   {
      (void)beatsPerMinute;

      // Insert a new first tempo event, if tick > 0
      /* Makes the duration incorrect longer
      if ( m_setTempoEvents.size() == 0 && tick > 0 )
      {
         DurationComputer_SetTempoEvent e;
         e.m_tick = 0;
         e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
         m_setTempoEvents.emplace_back( std::move(e) );
      }
      */

      // Finalize duration of last event ( before pushing a new event )
      if ( m_setTempoEvents.size() > 0 )
      {
         DurationComputer_SetTempoEvent & back = m_setTempoEvents.back();
         back.m_duration = int64_t(tick) - int64_t(back.m_tick);
      }

      // Push new event to back.
      DurationComputer_SetTempoEvent e;
      e.m_tick = tick;
      e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
      m_setTempoEvents.emplace_back( std::move(e) );
   }

   void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat ) override
   {
      (void)tick;
      m_top = top;
      m_bottom = bottom;
      m_clocksPerBeat = clocksPerBeat;
      m_n32rd_per_beat = n32rd_per_beat;
   }

   void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity ) override
   {
      m_tickMin = std::min( m_tickMin, tick );
      m_tickMax = std::max( m_tickMax, tick );
      (void)channel;
      (void)midiNote;
      (void)velocity;
   }

   void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity ) override
   {
      m_tickMin = std::min( m_tickMin, tick );
      m_tickMax = std::max( m_tickMax, tick );
      (void)channel;
      (void)midiNote;
      (void)velocity;
   }

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
/*
   void fillDurations()
   {
      std::cout << "Orignal.SetTempoEvents = " << m_setTempoEvents.size() << std::endl;
      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         std::cout << "Orignal.SetTempoEvent["<<i<<"] " << m_setTempoEvents[i].toString() << std::endl;
      }

      if ( m_tickMax < 1 )
      {
         std::cout << "ERROR m_tickMax < 1" << std::endl;
         return;
      }

      if ( m_setTempoEvents.size() == 0 )
      {
         std::cout << "WARN: Push atleast one SetTempo event" << std::endl;
         DurationComputer_SetTempoEvent e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default bpm, starts at 0.
         m_setTempoEvents.emplace_back( std::move( e ) );
      }

      if ( m_setTempoEvents[0].m_tick > 0 )
      {
         std::cout << "WARN: Insert additional SetTempo event (tickStart > 0)" << std::endl;
         DurationComputer_SetTempoEvent e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default bpm, starts at 0.
         m_setTempoEvents.insert( m_setTempoEvents.begin(), e );
      }

      // Now insert durations:

      if ( m_setTempoEvents.size() == 1 )
      {
         m_setTempoEvents[0].m_duration = int64_t(m_tickMax);
      }
      else
      {
         for ( size_t i = 1; i < m_setTempoEvents.size(); ++i )
         {
            int64_t tickBeg = int64_t(m_setTempoEvents[i-1].m_tick);
            int64_t tickEnd = int64_t(m_setTempoEvents[i].m_tick);
            int64_t duration = tickEnd - tickBeg;

            if ( duration > 0 )
            {
               m_setTempoEvents[i-1].m_duration = duration;
            }
            else
            {
               std::cout << "ERROR: ["<<i<<"] Got bad duration " << duration << ", "
                  "we should delete this entry, since it has no influence, "
                  "but makes our life more complicated!" << std::endl;
               m_setTempoEvents[i-1].m_duration = 0;
            }
         }

         int64_t tickBeg = int64_t(m_setTempoEvents.back().m_tick);
         int64_t tickEnd = int64_t(m_tickMax);
         int64_t duration = tickEnd - tickBeg;
         if ( duration > 0 )
         {
            m_setTempoEvents.back().m_duration = duration;
         }
         else
         {
            std::cout << "ERROR: [back] Got bad duration " << duration << std::endl;
            m_setTempoEvents.back().m_duration = 0;
         }
      }
   }
*/
   double computeDurationInSeconds()
   {
      // fillDurations();

      // Debug SetTempo events:

      DE_DEBUG("TicksPerQuarterNote = ", m_ticksPerQuarterNote )
      DE_DEBUG("TickMax = ", m_tickMax)
      DE_DEBUG("TickMin = ", m_tickMin)

      DE_DEBUG("SetTempoEvents = ", m_setTempoEvents.size())
      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         DE_DEBUG("SetTempoEvent[", i, "] ", m_setTempoEvents[i].toString())
      }

      DE_DEBUG("TimeSignature = ", m_top, "/", m_bottom,
               ", ClocksPerBeat = ", m_clocksPerBeat,
               ", Num32rdPerBeat = ", m_n32rd_per_beat )

      if ( m_tickMax == 0 )
      {
         DE_ERROR("Song has duration 0")
         return 0.0;
      }

      if ( m_setTempoEvents.size() == 0 )
      {
         DE_ERROR("No SetTempo events")
         return 0.0;
      }

      double durationInSeconds = 0.0;

      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         durationInSeconds += m_setTempoEvents[i].computeDurationInSeconds( m_ticksPerQuarterNote );
      }

      DE_DEBUG("Duration = ", durationInSeconds, "sec")
      DE_DEBUG("Duration = ", StringUtil::seconds(durationInSeconds) )
      return durationInSeconds;
   }

};

} // end namespace midi
} // end namespace de
