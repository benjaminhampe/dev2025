#pragma once
#include <de/midi/IParserListener.hpp>
#include <de/midi/file/SetTempoEvent.hpp>
#include <de/midi/file/Util.hpp>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct TempoMap : public IParserListener
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.file.TempoMap")

   int m_ticksPerQuarterNote; // = ticks per beat
   float m_speed;
   uint64_t m_tickMin;
   uint64_t m_tickMax;

   double m_timeDurationInSec;

   std::vector< SetTempoEvent > m_setTempoEvents;

   TempoMap()
      : m_ticksPerQuarterNote(96)
      , m_speed( 1.0f )
      , m_tickMin( std::numeric_limits< uint64_t >::max() )
      , m_tickMax( std::numeric_limits< uint64_t >::lowest() )
      , m_timeDurationInSec( 0.0 )
   {}

   ~TempoMap() override {}


   uint64_t getTickFromNanosecond( int64_t nanoseconds ) const
   {
      uint64_t ticks = 0;
      uint64_t tickStart = 0;

      int found = -1;
      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         SetTempoEvent const & e = m_setTempoEvents[ i ];

         if ( e.m_tickStart > tickStart )
         {
            break;
         }
         tickStart = computeTicks( e.m_tickDuration,
                                   e.m_microsecondsPerQuarterNote,
                                   m_ticksPerQuarterNote, m_speed );
      }

      return ticks;
   }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_setTempoEvents.capacity() * sizeof( SetTempoEvent );
      return n;
   }

   double getDurationInSec() const { return m_timeDurationInSec; }

   void reset()
   {
      m_tickMin = std::numeric_limits< uint64_t >::max();
      m_tickMax = std::numeric_limits< uint64_t >::lowest();
      m_timeDurationInSec = 0.0;
      m_setTempoEvents.clear();
   }

   void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote ) override
   {
      (void)fileType;
      (void)trackCount;
      m_ticksPerQuarterNote = ticksPerQuarterNote;
   }

   // Finalize TempoMap ( finalize duration of last SetTempo event )
   void mpEnd() override
   {
      finalizeTempoMap();
   }

   void finalizeTempoMap()
   {
      size_t const eventCount = m_setTempoEvents.size();

      // Insert atleast one SetTempoEvent that spans entire song with default midi bpm value...
      if ( eventCount == 0 )
      {
         auto e = SetTempoEvent::create( 0, 90.0f );
         e.m_tickDuration = m_tickMax;
         m_setTempoEvents.emplace_back( std::move(e) );
      }
         // ... or finalize last SetTempo event's duration ...
         //      else if ( eventCount == 1 )
         //      {
         //         SetTempoEvent & e = m_setTempoEvents.back();
         //         e.m_tickDuration = m_tickMax;
         //      }
      // ... or finalize last SetTempo event.
      else
      {
         SetTempoEvent & e = m_setTempoEvents.back();
         e.m_tickDuration = m_tickMax - e.m_tickStart;
      }


      m_timeDurationInSec = 0.0;

      if ( m_tickMax == 0 )
      {
         DE_ERROR("Song has duration 0")
         return;
      }

      if ( m_setTempoEvents.size() == 0 )
      {
         DE_ERROR("No SetTempo events")
         return;
      }

      for ( auto const & item : m_setTempoEvents )
      {
         m_timeDurationInSec += computeSeconds( item.m_tickDuration,
                                                item.m_microsecondsPerQuarterNote,
                                                m_ticksPerQuarterNote,
                                                double(m_speed) );
      }
   }

   void mpSetTempo( uint64_t tick, float bpm, int microsecondsPerQuarterNote ) override
   {
      // Finalize last event's duration ( before adding a new event )
      if ( m_setTempoEvents.size() > 0 )
      {
         SetTempoEvent & e = m_setTempoEvents.back();
         e.m_tickDuration = tick - e.m_tickStart;
      }

      // Add new event.
      auto e = SetTempoEvent::create( tick, bpm, microsecondsPerQuarterNote );
      m_setTempoEvents.emplace_back( std::move(e) );
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


   std::string
   toString() const
   {
      std::ostringstream o;
      o << "TicksPerQuarterNote = " << m_ticksPerQuarterNote << "\n";
      o << "TickMax = " << m_tickMax << "\n";
      o << "TickMin = " << m_tickMin << "\n";

      o << "SetTempoEvents = " << m_setTempoEvents.size() << "\n";

      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         o << "SetTempoEvent[" << i << "] " << m_setTempoEvents[i].toString() << "\n";
      }

      o << "Duration = " << m_timeDurationInSec << "sec\n";
      o << "Duration = " << StringUtil::seconds(m_timeDurationInSec) << "\n";
      return o.str();
   }

   void dump() const
   {
      DE_DEBUG("TicksPerQuarterNote = ",m_ticksPerQuarterNote)
      DE_DEBUG("TickMax = ",m_tickMax)
      DE_DEBUG("TickMin = ",m_tickMin)

      DE_DEBUG("SetTempoEvents = ",m_setTempoEvents.size())

      for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
      {
         DE_DEBUG("SetTempoEvent[",i,"] ",m_setTempoEvents[i].toString())
      }

      DE_DEBUG("Duration = ",m_timeDurationInSec,"sec\n")
      DE_DEBUG("Duration = ",StringUtil::seconds(m_timeDurationInSec))
   }


   // Math util III. Convert midi tick amount -> play time duration in nanoseconds
   static int64_t
   computeNanoseconds(  uint64_t const ticks,
                        int const microsPerQuarterNote,
                        int const ticksPerQuarterNote,
                        double const speed = 1.0 )
   {
      if ( ticks < 1 ) return 0;
      if ( microsPerQuarterNote < 1 ) return 0;
      if ( ticksPerQuarterNote < 1 ) return 0;

      double const microsProTick = double(microsPerQuarterNote) / double(ticksPerQuarterNote);
      int64_t const ns = int64_t( std::round( double(ticks) * microsProTick * 1e3 / speed ) );
      return ns;
   }

   // Math util I. Convert midi tick amount -> play time duration
   static double
   computeSeconds(      uint64_t const ticks,
                        int const microsPerQuarterNote,
                        int const ticksPerQuarterNote,
                        double const speed = 1.0 )
   {
      if ( ticks < 1 ) return 0.0;
      if ( microsPerQuarterNote < 1 ) return 0.0;
      if ( ticksPerQuarterNote < 1 ) return 0.0;

      double const microsProTick = double(microsPerQuarterNote) / double(ticksPerQuarterNote);
      double const elapsedSeconds = double(ticks) * microsProTick * 1e-6 / speed;
      return elapsedSeconds;
   }

   // Math util II. Convert play time duration -> midi tick amount
   static uint64_t
   computeTicks(        double const seconds,
                        int const microsPerQuarterNote,
                        int const ticksPerQuarterNote,
                        double const speed = 1.0 )
   {
      if ( seconds <= 0.0 ) return 0;
      if ( microsPerQuarterNote < 1 ) return 0;
      if ( ticksPerQuarterNote < 1 ) return 0;

      double const ticksPerMicros = double(ticksPerQuarterNote) / double(microsPerQuarterNote);
      uint64_t const ticks = uint64_t( std::round( seconds * ticksPerMicros * 1e6 * speed ) );
      return ticks;
   }

};

} // end namespace file
} // end namespace midi
} // end namespace de
