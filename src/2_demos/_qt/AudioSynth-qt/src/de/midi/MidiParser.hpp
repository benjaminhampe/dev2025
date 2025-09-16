#pragma once
#include <de/midi/MidiFile.hpp>

// SMF Syntax - Standard Midi file syntax. ( Aufs Byte genau leider )
// <descriptor:length> means 'length' bytes, MSB first
// <descriptor:v> means variable length argument (special format)
// SMF := <header_chunk> + <track_chunk> [ + <track_chunk> ... ]
// header chunk := "MThd" + <header_length:4> + <format:2> + <num_tracks:2> + <time_division:2>
// track_chunk := "MTrk" + <length:4> + <track_event> [ + <track_event> ... ]
// track_event := <time:v> + [ <midi_event> | <meta_event> | <sysex_event> ]
// midi_event := any MIDI channel message, including running status
// meta_event := 0xFF + <meta_type:1> + <length:v> + <event_data_bytes>
// sysex_event := 0xF0 + <len:1> + <data_bytes> + 0xF7
// sysex_event := 0xF7 + <len:1> + <data_bytes> + 0xF7

namespace de {
namespace midi {

// =======================================================================
struct IMidiParserListener
// =======================================================================
{
   virtual ~IMidiParserListener() = default;

// I.:

   // Used for debugging ( diff analysis ), etc...
   virtual void
   mpDebugStart( uint8_t const* beg, uint8_t const* end, std::string const & uri ) {}

   // Used for advanced parsers, debug, diff analysis, etc...
   virtual void
   mpDebugToken( uint8_t const* beg, uint8_t const* end, std::string const & comment ) {}

// II.:

   virtual void
   mpTrack( uint8_t const* beg, uint8_t const* end ) {}

   virtual void
   mpNote( uint32_t pts, int channel, int midinote, int velocity ) {}

   virtual void
   mpProgramChange( uint32_t pts, int channel, int program ) {}

   virtual void
   mpPolyAftertouch( uint32_t pts, int channel, int value ) {}

   virtual void
   mpPitchBend( uint32_t pts, int channel, int value ) {}

   virtual void
   mpControlChange( uint32_t pts, int channel, int cc, int value ) {}

//   virtual void
//   mpBendWheel( uint32_t pts, int channel, int value ) {}

// III.: Even more Simple API

   virtual void
   mpSetTempo( uint32_t pts, float bpm ) {}

   // Always sent before a track event.
//   virtual void
//   mpClock( int deltaTime ) {}

//   virtual void
//   mpNoteOn( double pts, int channel, int midinote, int velocity ) {}

//   virtual void
//   mpNoteOff( double pts, int channel, int midinote, int velocity ) {}

   virtual void
   mpTimeSignature( uint32_t pts, int num, int den, int bpc, int n32rd_per_qn ) {}

/*
   // Get full tokens, Midi header, Track header, TrackEvent
   virtual void
   mpToken( uint8_t const* beg, uint8_t const* end ) {}

   virtual void
   mpTextEvent( int typ, std::string const & txt ) {}


   virtual void
   mpKeySignature( int accidental, int minor ) {}

   // Get all NoteOn 9x and NoteOff 8x events
   //
   virtual void
   mpNoteEvent( int trackIndex = 0,
                int channel = 0,
                int program = 0,
                int dt = 100,
                int midiNote = 60,
                bool noteOn = false,
                uint8_t velocity = 0 ) {}

   // Get all ProgramChange 8x events
   //
*/

//   virtual void
//   mpClockStart( int deltaTime ) {}


//   virtual void
//   mpChannelAfterTouch( int channel, int program ) {}
};

// =======================================================================
struct MidiParser
// =======================================================================
{
   DE_CREATE_LOGGER("de.MidiParser")
   std::vector< IMidiParserListener* > m_listeners;

   void addListener( IMidiParserListener* listener )
   {
      if ( !listener ) return;
      m_listeners.emplace_back( listener );
   }

   void emitDebugStart( uint8_t const* beg, uint8_t const* end, std::string const & uri )
   {
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpDebugStart( beg, end, uri );
      }
   }

   void emitTrack( uint8_t const* beg, uint8_t const* end )
   {
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpTrack( beg, end );
      }
   }

   void emitToken( uint8_t const* beg, uint8_t const* end, std::string const & comment )
   {
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpDebugToken( beg, end, comment );
      }
   }

   void emitNote( uint32_t pts, int channel, int midinote, int velocity )
   {
      m_curTrack.addNote( m_currTick, channel, midinote, velocity );
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpNote( pts, channel, midinote, velocity );
      }
   }

   void emitPolyAftertouch( uint32_t pts, int channel, int value )
   {
      m_curTrack.addPolyAftertouch( m_currTick, channel, value );
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpPolyAftertouch( pts, channel, value );
      }
   }

   void emitPitchBend( uint32_t pts, int channel, int value )
   {
      m_curTrack.addPitchWheel( m_currTick, channel, value );
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpPitchBend( pts, channel, value );
      }
   }

//   void emitBendWheel( uint32_t pts, int channel, int value )
//   {
//      m_curTrack.addBendWheel( m_currTick, channel, value );
//      for ( auto listener : m_listeners )
//      {
//         if ( listener ) listener->mpBendWheel( pts, channel, value );
//      }
//   }

   void emitProgramChange( uint32_t pts, int channel, int program )
   {
      m_curTrack.addProgramChange( pts, channel, program );
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpProgramChange( pts, channel, program );
      }
   }

   void emitControlChange( uint32_t pts, int channel, int cc, int value )
   {
      m_curTrack.addControlChange( pts, channel, cc, value );
      if ( cc == CC_64_SustainPedal )
      {
         m_curTrack.m_stats.m_sustainPedalCount++;
      }
      if ( cc == CC_67_SoftPedal )
      {
         m_curTrack.m_stats.m_softPedalCount++;
      }

      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpControlChange( pts, channel, cc, value );
      }
   }

   void emitTimeSignature( uint32_t pts, int num, int den, int bpc, int n32rd_per_qn )
   {
      m_curTrack.addTimeSignature( pts, num, den, bpc, n32rd_per_qn );
      for ( auto listener : m_listeners )
      {
         if ( listener ) listener->mpTimeSignature( pts, num, den, bpc, n32rd_per_qn );
      }
   }
   //0-the file contains a single multi-channel track
   //1-the file contains one or more simultaneous tracks (or MIDI outputs) of a sequence
   //2-the file contains one or more sequentially independent single-track patterns
   uint16_t m_fmt;
   // <ntrks> the number of tracks. Always 1 for a format 0 file.
   uint16_t m_trackCount;
   //15| bits 14 thru 8            | bits 7 thru 0
   //F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   //0 | ticks per quarter-note bits 14 - 0 = 0...2^14 = 16 * 1024 |
   //1 | negative SMPTE format     | ticks per frame               |
   uint16_t m_resolution = 0;// ticks per quarter-note

   uint16_t m_trackIndex = 0;

   uint8_t m_runStatus = 0;

   uint32_t m_ticksPerQuarterNote = 96;

   uint64_t m_currTick = 0;
   uint32_t m_currChannel = 0;
   uint32_t m_currProgram = 0;
   float m_currBpm = 0;  // bar tempo
   int m_currTempoNum = 4;            // bar tempo - numerator
   int m_currTempoDen = 4;            // bar tempo - denominator
   int m_currTempoCPB = 24;         // bar tempo - clocks per beat
   int m_currTempo32n = 8;          // bar tempo - 32nd per bar = 8ths per beat
   int m_currTempoUPB = 646000;     // bar tempo - microseconds per beat


   MidiFile m_curFile;
   MidiTrack m_curTrack;


   std::string m_uri;

   MidiParser();
   ~MidiParser();

   void
   reset();

   uint32_t
   parse( uint8_t const* beg, uint8_t const* end, std::string const & uri );

   // Must always return 14 bytes.
   uint32_t
   parseFileHeader( uint8_t const* beg, uint8_t const* end );

   // Must always return 8 bytes.
   uint32_t
   parseTrackHeader( uint8_t const* beg, uint8_t const* end, uint32_t & dataSize );

   uint64_t
   parseTrackData( uint8_t const* const beg, uint8_t const* const end );

   uint32_t
   parseEvent( uint8_t const* beg, uint8_t const* end );

   uint32_t
   parseMetaEvent(
      uint8_t const* evt, // Event start ( time ), needed for full token extract
      uint8_t const* beg, // Current pos (after reading byte metatype).
      uint8_t const* end, // End of string
      uint32_t deltaTime, // Already parsed dt injected here
      uint8_t metatype    // Already parsed metatype injected here
   );

   std::string
   parseMetaText( uint8_t const* beg, uint8_t const* end );

//   bool
//   loadFromFile( std::string uri )
//   {
//      return m_file.loadFromFile( uri );
//   }

//   bool
//   loadFromData( uint8_t const* src, uint64_t nBytes )
//   {
//      return m_file.loadFromData( src, nBytes );
//   }

//   static MidiParser
//   createFromFile( std::string uri )
//   {
//      MidiParser m;
//      m.loadFromFile( uri );
//      m.parse();
//      return m;
//   }

//   static MidiParser
//   createFromData( uint8_t const* src, uint64_t nBytes )
//   {
//      MidiParser m;
//      m.loadFromData( src, nBytes );
//      m.parse();
//      return m;
//   }

};


} // end namespace midi
} // end namespace de


/*
// =======================================================================
struct MidiTokenListener : public IMidiParserListener
// =======================================================================
{
   DE_CREATE_LOGGER("de.MidiTokenListener")

   MidiTokenListener() {}

   void
   clear() { m_tokens.clear(); }

   size_t
   countTokens() const { return m_tokens.size(); }

   size_t
   countBytes() const
   {
      size_t n = 0;
      for ( auto const & t : m_tokens )
      {
         n += t.size();
      }
      return n;
   }

   size_t
   maxDataString() const
   {
      size_t n = 0;
      for ( auto const & t : m_tokens )
      {
         n = std::max( n, t.toString().size() );
      }
      return n;
   }

   size_t
   maxCommentString() const
   {
      size_t n = 0;
      for ( auto const & t : m_tokens )
      {
         n = std::max( n, t.comment.size() );
      }
      return n;
   }


   std::string
   spaces( size_t n ) const
   {
      std::stringstream s;
      for ( size_t i = 0; i < n; ++i )
      {
         s << " ";
      }
      return s.str();
   }

   virtual void
   onParseStart( uint8_t const* beg, uint8_t const* end )
   {

   }

   virtual void
   onParsedToken( uint8_t const* beg, uint8_t const* end,
                    std::string const & comment = "" ) override
   {
      if ( beg >= end ) return;
      m_tokens.emplace_back();
      MidiToken & token = m_tokens.back();
      token.m_address = uint64_t( beg );
      token.m_data.assign( beg, end );
      token.comment = comment;
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "TokenCount = " << countTokens() << " with " << countBytes() << " byte(s)\n";
      //auto m = maxDataString();
      for ( uint32_t i = 0; i < m_tokens.size(); ++i )
      {
         auto const & t = m_tokens[ i ];
         auto const d = t.toString();
         auto const c = d.size();
         auto const p = " \t=>\t "; // spaces( 1 + m - c );
         s << dbStrJoin("Token[",i,"] ",d,p,t.comment,'\n');
      }

      return s.str();
   }

   inline void
   dumpTokens()
   {
      DE_DEBUG( toString() )
   }

   std::vector< MidiToken > m_tokens;
};
*/
