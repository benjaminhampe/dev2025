#pragma once
#include <de/midi/IParserListener.hpp> // -> "MidiUtil.hpp" -> "GeneralMidi.hpp"
#include <de/PerformanceTimer.hpp>

namespace de {
namespace midi {

// More bug free version v3, didn't find any (valid) MIDI file.
// that does not parse correctly, but i dont have all MIDI files in the world.
// Much better than v2 in regard of missing track headers.
// Recognizes NoteOn events with velocity = 0 as NoteOff events.
// Running status is running like the stupid duracell rabbit.
// ============================================================================
struct Parser // More bug free ver 3, no (valid) .mid found not parsing ok
// ============================================================================
{
   DE_CREATE_LOGGER("de.midi.Parser")

   //<header>
   // m_fileType has always 1 for a format 0 file, else number of tracks.
   // Can be removed, is validated once in parseHeader() and emitted to listeners
   // m_fileType = 0 - File contains 1 (multi-channel) track
   // m_fileType = 1 - File contains 1+ 'm_tackCount' (simultaneous) tracks (or MIDI outputs) of a sequence
   // m_fileType = 2 - File contains 1+ 'm_tackCount' sequentially independent single-track patterns
   uint16_t m_fileType;   // Can be removed, is emitted to listener
   uint16_t m_trackCount; // Can be removed, is emitted to listener
   uint16_t m_resolution; // Can be removed, is emitted to listener, ticks per quarter-note,
                          // most important info for MIDI clock. Tells how the VLQ delta time ticks are to be interpreted.
   //</header>
   //<statemachine>
   uint8_t m_runStatus;          // Most important part of MIDI statemachine. Never remove.
   uint16_t m_trackIndex;        // <optional> Keep track of tracks (incremented on TrackEnd event)
   uint32_t m_trackDataSize = 0; // <optional> We cant trust the datasize inside TrackHeader
   uint8_t const* m_trackDataStart = nullptr; // <optional> We store pointer to validate datasize

   uint64_t m_currTick = 0; // Absolute tick (sum). Helps skipping events you dont need, which is not possible with deltas.
                            // Replacing with float64 time in seconds is outsourced to higher level listener classes.
                            // I actually like the ticks, because they dont change, even when changing BPM dynamically
                            // so they are really absolute. And integers are fast and exact to compare.
                            // @See std::chrono which uses int64_t nanosecond ticks which is enough precision the next 10 Mio. years.
                            // Maybe nanoseconds are not precise enough in 100 years but its definetly enough for 150y old MIDI.

   //MidiClock m_clock;     // No clock in this low level_0 parser stage (yet).
                            // Could be possible i guess, but that task is done more easily
                            // in higher level IParserListener classes.
                            // Easiest (imho) is to parse the midi file atleast once completly.
                            // Then build an index of time signatures and offer a getter that
                            // computes the time in seconds from the given tick stamp
                            // SetTempo events are mostly combined into the first track ( so called MasterTrack )
                            // but that is not guaranteed. SetTempo events can happen any time in any track.
                            // So it could be necessary to build one index for every track.
                            // I didnt wanted to pollute the Parser class with it yet and i would have
                            // to change all function's first argument of the listener class from uint64_t tick to double seconds.
                            // @See DurationComputer class which is a ParserListener computing durations of midi files e.g. 04:32.
   //</statemachine>

   Parser();
   //~Parser();

   bool parse( std::string const & uri )
   {
      // (c) 2014 by <benjaminhampe@gmx.de>
      de::PerformanceTimer perf;
      perf.start();

      std::vector< uint8_t > byteVector;
      FileSystem::loadByteVector( byteVector, uri );

      if ( byteVector.size() < 14 ) // No space for a midi header
      {
         return false;
      }

      size_t expectBytes = byteVector.size();
      uint8_t const* beg = byteVector.data();
      uint8_t const* end = beg + expectBytes;
      size_t resultBytes = parse( beg, end, uri );

      perf.stop();

      // Validate parser result.
      if ( expectBytes == resultBytes )
      {
         DG_DEBUG("Needed ", perf.ms(), " ms, parsed all expectBytes(",expectBytes,"), loadUri = ", uri )
      }
      else
      {
         if ( expectBytes > resultBytes )
         {
            DG_ERROR("Needed ", perf.ms(), " ms, Got fewer resultBytes(",resultBytes,") than expectBytes(",expectBytes,"), loadUri = ", uri )
         }
         else
         {
            DG_WARN("Needed ", perf.ms(), " ms, Got more resultBytes(",resultBytes,") than expectBytes(",expectBytes,"), loadUri = ", uri )
         }
      }
      return true;
   }

   size_t
   parse( uint8_t const* const beg, uint8_t const* const end, std::string const & uri );

   // Must always return 14 (bytes).
   size_t
   parseFileHeader( uint8_t const* const beg, uint8_t const* const end );

   size_t
   parseMessage( uint8_t const* const beg, uint8_t const* const end );

   // Must always return 8 (bytes).
   // Some files dont have TrackHeaders
   size_t
   parseTrackHeader( uint8_t const* const beg, uint8_t const* const end, uint32_t & dataSize );

   size_t
   parseTrackEnd( uint8_t const* const beg, uint8_t const* const end );

   //size_t
   //parseTrackData( uint8_t const* const beg, uint8_t const* const end );

   size_t
   parseEvent( uint8_t const* const beg, uint8_t const* const end );

   size_t
   parseMetaEvent(
      uint8_t const* const vt, // Event start ( time ), needed for full token extract
      uint8_t const* const beg, // Current pos (after reading byte metatype).
      uint8_t const* const end, // End of string
      uint32_t deltaTime, // Already parsed dt injected here
      uint8_t metatype    // Already parsed metatype injected here
   );

   std::string
   parseMetaText( uint8_t const* const beg, uint8_t const* const end );

   // =======================================================================
   //struct MidiParserListenerRegistry
   // =======================================================================

   std::vector< IParserListener* > m_listeners; // One and only member

   // ~MidiParserListenerRegistry() {}

   // Removes all listeners at once.
   // Simpler than having a removeListener() function.
   void clearListeners()
   {
      m_listeners.clear();
   }

   // Adds/registers a listener, rejects null pointers.
   void addListener( IParserListener* listener )
   {
      if ( !listener ) return;
      m_listeners.emplace_back( listener );
   }

// I.:
   // void mpMessage( uint8_t const* beg, uint8_t const* end, std::string const & comment )
   // {
      // for (auto l : m_listeners) { if (l) l->mpToken( beg, end, comment ); }
   // }
   void mpToken( uint8_t const* beg, uint8_t const* end, std::string const & comment )
   {
      for (auto l : m_listeners) { if (l) l->mpToken( beg, end, comment ); }
   }


   void mpStart( uint8_t const* beg, uint8_t const* end, std::string const & fileName )
   {
      for (auto l : m_listeners) { if (l) l->mpStart( beg, end, fileName ); }
   }
   void mpEnd()
   {
      for (auto l : m_listeners) { if (l) l->mpEnd(); }
   }

   void mpTrack( uint8_t const* beg, uint8_t const* end, int trackNumber )
   {
      for (auto l : m_listeners) { if (l) l->mpTrack( beg, end, trackNumber ); }
   }

// II.:

   void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
   {
      for (auto l : m_listeners) { if (l) l->mpFileHeader( fileType, trackCount, ticksPerQuarterNote ); }
   }
   void mpTrackStart( int trackIndex )
   {
      for (auto l : m_listeners) { if (l) l->mpTrackStart( trackIndex ); }
   }
   void mpTrackEnd()
   {
      for (auto l : m_listeners) { if (l) l->mpTrackEnd(); }
   }

// III.:

   void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
   {
      for (auto l : m_listeners) { if (l) l->mpNoteOn( tick, channel, midiNote, velocity ); }
   }
   void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
   {
      for (auto l : m_listeners) { if (l) l->mpNoteOff( tick, channel, midiNote, velocity ); }
   }
   void mpPolyphonicAftertouch( uint64_t tick, int value )
   {
      for (auto l : m_listeners) { if (l) l->mpPolyphonicAftertouch( tick, value ); }
   }
   void mpChannelAftertouch( uint64_t tick, int channel, int value )
   {
      for (auto l : m_listeners) { if (l) l->mpChannelAftertouch( tick, channel, value ); }
   }
   void mpPitchBend( uint64_t tick, int channel, int value )
   {
      for (auto l : m_listeners) { if (l) l->mpPitchBend( tick, channel, value ); }
   }
   void mpProgramChange( uint64_t tick, int channel, int program )
   {
      for (auto l : m_listeners) { if (l) l->mpProgramChange( tick, channel, program ); }
   }
   void mpControlChange( uint64_t tick, int channel, int cc, int value )
   {
      for (auto l : m_listeners) { if (l) l->mpControlChange( tick, channel, cc, value ); }
   }
   void mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerTick )
   {
      for (auto l : m_listeners) { if (l) l->mpSetTempo( tick, beatsPerMinute, microsecondsPerTick ); }
   }
   void mpSMPTEOffset( uint64_t tick, int hh, int mm, int ss, int fc, int sf )
   {
      for (auto l : m_listeners) { if (l) l->mpSMPTEOffset( tick, hh, mm, ss, fc, sf ); }
   }
   void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat )
   {
      for (auto l : m_listeners) { if (l) l->mpTimeSignature( tick, top, bottom, clocksPerBeat, n32rd_per_beat ); }
   }
   void mpKeySignature( uint64_t tick, int tonart_c_offset, int minor )
   {
      for (auto l : m_listeners) { if (l) l->mpKeySignature( tick, tonart_c_offset, minor ); }
   }
   void mpSequenceNumber( uint64_t tick, int sequenceNumber )
   {
      for (auto l : m_listeners) { if (l) l->mpSequenceNumber( tick, sequenceNumber ); }
   }
   void mpChannelPrefix( uint64_t tick, int channelPrefix )
   {
      for (auto l : m_listeners) { if (l) l->mpChannelPrefix( tick, channelPrefix ); }
   }
   void mpPortDisplay( uint64_t tick, int port )
   {
      for (auto l : m_listeners) { if (l) l->mpPortDisplay( tick, port ); }
   }
   /*
   void mpMeta( uint64_t tick, int metaType, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMeta( tick, metaType, metaText ); }
   }
   */
   void mpMetaText( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaText( tick, metaText ); }
   }
   void mpMetaCopyright( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaCopyright( tick, metaText ); }
   }
   void mpMetaTrackName( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaTrackName( tick, metaText ); }
   }
   void mpMetaInstrumentName( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaInstrumentName( tick, metaText ); }
   }
   void mpMetaLyric( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaLyric( tick, metaText ); }
   }
   void mpMetaMarker( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaMarker( tick, metaText ); }
   }
   void mpMetaCuePoint( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaCuePoint( tick, metaText ); }
   }
   void mpMetaProgramName( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaProgramName( tick, metaText ); }
   }
   void mpMetaDeviceName( uint64_t tick, std::string const & metaText )
   {
      for (auto l : m_listeners) { if (l) l->mpMetaDeviceName( tick, metaText ); }
   }
};

} // end namespace midi
} // end namespace de
