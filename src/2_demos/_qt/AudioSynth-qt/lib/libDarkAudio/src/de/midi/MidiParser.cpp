#include <de/midi/MidiParser.hpp>

namespace de {
namespace midi {

MidiParser::MidiParser()
{
   reset();
}

MidiParser::~MidiParser()
{
   reset();
}

void
MidiParser::reset()
{
   m_trackIndex = 0;
   m_runStatus = 0;
   m_curFile = MidiFile();
}

uint32_t
MidiParser::parse( uint8_t const* beg, uint8_t const* end, std::string const & uri )
{
   m_uri = uri;
   emitDebugStart( beg, end, uri );

   auto src = beg;

   //DE_DEBUG("MIDI.fileSize = ", end - beg )
   //DE_DEBUG("MIDI.data = \n", hexStr( beg, end, 16 ) )

   auto n = parseFileHeader( src, end );
   if ( n != 14 )
   {
      DE_ERROR("[",(void*)0,"] No Midi header ",uri ) return 0;
   }
   src += n;

   m_trackIndex = 1;

   m_curFile = MidiFile();
   m_curFile.m_uri = uri;
   m_curFile.m_ticksPerQuarterNote = m_ticksPerQuarterNote;

   m_curTrack = MidiTrack();
   m_curTrack.m_uri = uri;
   m_curTrack.m_id = m_trackIndex;
   m_curTrack.m_ticksPerQuarterNote = m_ticksPerQuarterNote;

   while ( src < end ) //  && m_trackIndex < m_trackCount
   {
      // Parse Track header -> extract 'trackSize' in bytes.
      uint32_t dataSize = 0;
      n = parseTrackHeader( src, end, dataSize );
      if ( n != 8 )
      {
         DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex,"/",m_trackCount,") :: "
                  "No track header ",uri )
         break;
      }
      src += n;

      // Parse Track data -> extract all timed events <deltatime+command>
      // Dont cross file border with bad dataSize.
      auto dataBegin = src;
      auto dataEnd = std::min( src + dataSize, end );
      n = parseTrackData( dataBegin, dataEnd );

      // Jump more safely to next track using precomputed value from outer parser.
      // So we can overcome errors from inner parser
      if ( dataBegin + n != dataEnd )
      {
         DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex,"/",m_trackCount,") :: "
                  "TrackData parse error. " ,uri)
      }
      src = dataEnd;

      // Stats

      m_curFile.m_tracks.emplace_back( std::move( m_curTrack ) );

      m_trackIndex++;

      m_curTrack = MidiTrack();
      m_curTrack.m_uri = uri;
      m_curTrack.m_id = m_trackIndex;
      m_curTrack.m_ticksPerQuarterNote = m_ticksPerQuarterNote;
   }

   // Check outer parser state
   if ( src != end )
   {
      DE_WARN("Did not parse(",src-beg,") all bytes(",end-beg,"), ",uri)
   }

   DE_WARN("\n",m_curFile.toString())

   return src - beg;
}

uint32_t
MidiParser::parseFileHeader( uint8_t const* beg, uint8_t const* end )
{
   // Not a MIDI file header.
   if ((end - beg) < 14)
   {
      DE_ERROR("No enough bytes" ) return 0; // Means parser cant process this or advance(0).
   }

   // Dont touch 'beg', so introduce src.
   auto src = beg;

   // Read 8 bytes - must be same for all MIDI filestreams.
   if ( *src++ != 'M' ) { DE_ERROR("No M" ) return 0; }
   if ( *src++ != 'T' ) { DE_ERROR("No T" ) return 0; }
   if ( *src++ != 'h' ) { DE_ERROR("No h" ) return 0; }
   if ( *src++ != 'd' ) { DE_ERROR("No d" ) return 0; }
   if ( *src++ != 0 ) { DE_ERROR("No 0" ) return 0; }
   if ( *src++ != 0 ) { DE_ERROR("No 0" ) return 0; }
   if ( *src++ != 0 ) { DE_ERROR("No 0" ) return 0; }
   if ( *src++ != 6 ) { DE_ERROR("No 6" ) return 0; }

   // For better printing we emit 2 tokens for long midi header. 8+6 byte tokens.
   std::string s = dbStr( "MIDI header signature, 8 bytes, has size 6." );
   emitToken( beg, src, s );

   // We touch 'src', so introduce 'dat' to const store begin of 2nd token.
   auto dat = src;

   // Read last 6 bytes with 3x infos:
   // Make no additional error checks, because the 1st if(<14) covers them all.
   auto
   // Read 2 bytes for FileFormat
   n = MidiTools::readU16_be( src, end, m_fmt ); src += n;
   // Read 2 bytes for TrackCount
   n = MidiTools::readU16_be( src, end, m_trackCount ); src += n;
   // Read 2 bytes for TickPerQuarterNote ( no 15th bit set ) or else.
   n = MidiTools::readU16_be( src, end, m_resolution ); src += n;

   // Debug Text
   s = dbStr( "MIDI header filetype(", m_fmt, "), tracks(", m_trackCount,"), " );
   if ( m_resolution & 0x8000 ) // is bit 15 set in time resolution?
   {
      s += dbStr("clock(SMPTE,", m_resolution,",[ticks per frame])" );
   }
   else
   {
      s += dbStr("clock(normal,",m_resolution,"[ticks per quarter note])" );
   }

   // Debug Error Text
   if ( m_fmt == 0 && m_trackCount != 1 )
   {
      s += dbStr(", WARN malformed format" );
      m_trackCount = 1;
   }

   // Debug Error Text
   m_ticksPerQuarterNote = uint32_t(m_resolution & 0x7FFF);
   if ( m_ticksPerQuarterNote < 1 )
   {
      s += dbStr(", WARN no m_ticksPerQuarterNote, fallback to 96." );
      m_ticksPerQuarterNote = 96;
   }

   // Emit 2nd token with debug text ( for 3 infos )
   emitToken( dat, src, s ); // For better debug info printing - an extra token.
   return src - beg; // 8+6 = 14!
}

uint32_t
MidiParser::parseTrackHeader( uint8_t const* beg, uint8_t const* end, uint32_t & dataSize )
{
   // Must always return 8 bytes.
   if ((end - beg) < 8) { DE_ERROR("Not enough bytes" ) return 0; }

   auto src = beg;
   if ( *src++ != 'M' ) { DE_ERROR("No M" ) return 0; };
   if ( *src++ != 'T' ) { DE_ERROR("No T" ) return 0; };
   if ( *src++ != 'r' ) { DE_ERROR("No r" ) return 0; };
   if ( *src++ != 'k' ) { DE_ERROR("No k" ) return 0; };

   dataSize = 0;
   src += MidiTools::readU32_be( src, end, dataSize );
   if ( dataSize < 1 ) { DE_ERROR("No dataSize" ) }

   auto s = dbStr("NewTrack(",m_trackIndex,"/",m_trackCount,") header, dataSize = ", dataSize );

   emitToken( beg, src, s );

   return src - beg;
}

// in nanoseconds or MIDI ticks
uint64_t
MidiParser::parseTrackData( uint8_t const* const beg, uint8_t const* const end )
{
   emitTrack( beg, end ); // Tell listeners.

   auto dataSize = uint32_t(end - beg);

   uint8_t const* src = beg;

   m_runStatus = 0;  // reset running status
   size_t i = 0;     // reset event counter

   size_t loopMax = 10000000000; // Prevent dead lock 10^10 events.

   while ( src < end && i < loopMax )
   {
      // Parse entire event, be able to skip it.
      uint32_t n = parseEvent( src, end );
      if ( n < 1 )
      {
         DE_DEBUG("[",(void*)(src-beg),"] "
                  "Track(",m_trackIndex,"/",m_trackCount,").Event[",i,"] :: "
                  "ParseError 0, n(",n,"), dataSize(",dataSize,")" )
         break;
      }

      // Skip entire event.
      src += n;
      i++;
      m_curTrack.m_stats.m_eventCount++;
   }


   return src - beg;
}

uint32_t
MidiParser::parseEvent( uint8_t const* beg, uint8_t const* end )
{
   auto src = beg;

   std::string s; // comment
   std::string e; // error comment

   // [1.] Read delta-time
   uint32_t dt = 0;
   auto n = MidiTools::parseVLQ( src, end, dt );
   if ( n < 1 )
   {
      DE_ERROR("No DeltaTime found")
      return 0;
   }
   src += n;
   m_currTick += dt; // SetTempo dependent complex formula?

   //double pts = double( m_currTick ) / double( m_resolution );

   uint8_t event = 0;
   uint8_t d1 = 0;
   uint8_t d2 = 0;

   // [2.] Fetch 1st byte after VLQ deltaTime dt.
   uint8_t c1 = *src++; // ( Can be only a new command or arg1 for last command )

   // [3.] Not a command byte
   if ( c1 < 0x80 )
   {
      if ( m_runStatus < 8 )
      {
         e = dbStr( "- BAD runningStatus ", hexStr(m_runStatus) );
      }
      else
      {
         e = dbStr( "- OK runningStatus ", hexStr(m_runStatus) );
      }

      event = m_runStatus; // Repeat last command
      d1 = c1;             // Use value as param1 to last command.
   }
   // [4.] A command byte
   else
   {
      event = c1;    // Set new command
      d1 = *src++;   // Fetch one more byte as param1.
      if ( event < 0xF0 )  // SysCommon and SysEx dont influence running status ( yet ).
      {
         m_runStatus = event;
      }
   }

   // [5.] Decompose command byte into command[0,15] and channel [0,15].
   // This helps compiler to better optimize, No 256 combinations, more 32 or so.
   int command = (event & 0xF0) >> 4;// Get command high nibble
   int ch = event & 0x0F;       // Get channel low nibble

   // [8x+9x] Handle NoteOff and NoteOn both at once.
   if ( command == 8 || command == 9 )
   {
      //double pts = double( m_currTick ) / double( m_resolution );

      d2 = *src++;         // Fetch param2
      int note = d1;       // Convert param1
      int velocity = d2;   // Convert param2
      bool isOn = command == 9 && velocity > 0;
      if ( isOn )
      {
         auto s = dbStr("dt(",dt,"), ch(",ch,") NOTE_ON(",MidiTools::noteStr(note),
            "), velocity(", velocity,"), runstat(0x", hexStr(m_runStatus),")" );
         emitToken( beg, src, s );
         emitNote( m_currTick, ch, note, velocity );
      }
      else
      {
         auto s = dbStr("dt(",dt,"), ch(",ch,") NOTE_OFF(",MidiTools::noteStr(note),
            "), runstat(0x", hexStr(m_runStatus),")" );
         emitToken( beg, src, s );
         emitNote( m_currTick, ch, note, 0 );
      }
   }
   // [Ax] Handle PAT - Polyphonic Aftertouch, +2 data bytes.
   else if ( command == 10 )
   {
      d2 = *src++;         // Fetch param2
      int value = ( (d1 & 0x7F) << 7 ) | (d2 & 0x7F);
      s = dbStr("dt(",dt,"), ch(",ch,") AfterTouch(",value,"), "
            "d1(0x",hexStr(d1),"), d2(0x",hexStr(d2),"), runstat(0x", hexStr(m_runStatus),")" );
      emitToken( beg, src, s );
      emitPolyAftertouch( m_currTick, ch, d2 );
   }
   // [Bx] Handle CC - Continuous controller, +2 data bytes.
   else if ( command == 11 )
   {
      //double pts = double( m_currTick ) / double( m_resolution );
      d2 = *src++;         // Fetch param2
      int cc = d1;
      int value = d2;
      auto ccStr = EMidiCC_toString( EMidiCC(d1) );
      s = dbStr("dt(",dt,"), ch(",ch,") ",ccStr,"(0x",hexStr(d2),"), runstat(0x", hexStr(m_runStatus),")" );
      emitToken( beg, src, s );
      emitControlChange( m_currTick, ch, cc, value );
   }
   // [Cx] ProgramChange, +1 data byte.
   else if ( command == 12 )
   {
      s = dbStr("dt(",dt,"), ch(",ch,") ",EMidiGM_toString( EMidiGM(d1) ), "(0x",hexStr(d1),"), runstat(0x", hexStr(m_runStatus),")" );
      emitToken( beg, src, s );
      emitProgramChange( m_currTick, ch, d1 );
   }
   // [CP] ChannelPressure, +1 data byte.
   else if ( command == 13 )
   {
      s = dbStr("dt(",dt,"), ch(",ch,") ChannelPressure, d1(0x",hexStr(d1),"), runstat(0x", hexStr(m_runStatus),")" );
      emitToken( beg, src, s );
      //emitNoteOff( channel, midiNote, velocity );
   }
   // [PB] PitchBend, +2 data bytes.
   // Status byte : 1110 CCCC
   // Data byte 1 : 0LLL LLLL (d1=LSB)
   // Data byte 2 : 0MMM MMMM (d2=MSB) -> value = (( d2 & 0x7F ) << 7) | ( d1 & 0x7F )
   else if ( command == 14 )
   {
      d2 = *src++;
      int bend = (( d2 & 0x7F ) << 7) | ( d1 & 0x7F );
      s = dbStr("dt(",dt,"), ch(",ch,") PitchBend(",bend,"), d1(0x",hexStr(d1),"), d2(0x",hexStr(d2),"), runstat(0x", hexStr(m_runStatus),")" );
      emitToken( beg, src, s );
      emitPitchBend( m_currTick, ch, bend );
   }
   // [SysCommon] A system common event F0...FF (after DeltaTime)
   else if ( command == 15 )
   {
      switch ( ch )
      {
         case 0: // F0 - Begin System Exclusive [SysEx]
          {
            uint8_t k = *src++;
            while ( k != 0xF7 )
            {
               k = *src++;
            }
            s = dbStr("dt(",dt,") F0 - Sysex begin, runstat(0x", hexStr(m_runStatus),")" );
            emitToken( beg, src, s );
          }
            break;
         case 1: // F1 - Time Code QuarterNote Frame
            d2 = *src++;
            s = dbStr("dt(",dt,") F1 - TimeCode Quarter Frame(",int(d2),"), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 2: // F2 - Song Position Pointer
            d2 = *src++;
            s = dbStr("dt(",dt,") F2 - Song Position Pointer(",int(d2),"), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 3: // F3 - Song Select (Song #) 1byte (0-127)
            d2 = *src++;
            s = dbStr("dt(",dt,") F3 - Song Select(",int(d2),"), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 4: // F4 - Undefined (Reserved)
            s = dbStr("dt(",dt,") F4 - Undefined (Reserved), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 5: // F5 - Undefined (Reserved)
            s = dbStr("dt(",dt,") F5 - Undefined (Reserved), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 6: // F6 - TuneRequest
            s = dbStr("dt(",dt,") NOIMPL F6 - TuneRequest, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 7: // F7 - End System Exclusive
          {
            uint8_t k = *src++;
            while ( k != 0xF0 )
            {
               k = *src++;
            }
            s = dbStr("dt(",dt,") F7 - SysEx end., runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
          }
            break;
         case 8: // F8 - Timing clock
            s = dbStr("dt(",dt,") NOIMPL F8 - Timing clock, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 9: // F9 - Undefined (Reserved)
            s = dbStr("dt(",dt,") NOIMPL F9 - Undefined (Reserved), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 10: // FA - MidiClock Start
            s = dbStr("dt(",dt,") NOIMPL FA - MidiClock Start, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 11: // FB - MidiClock Continue
            s = dbStr("dt(",dt,") NOIMPL FB - MidiClock Continue, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 12: // FC - MidiClock Stop
            s = dbStr("dt(",dt,") NOIMPL FC - MidiClock Stop, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 13: // FD - Undefined (Reserved)
            s = dbStr( "dt(",dt,") NOIMPL FD - Undefined (Reserved), runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 14: // FE - Active Sensing
            s = dbStr( "dt(",dt,") NOIMPL FE - Active Sensing, runstat(0x", hexStr(m_runStatus),")");
            emitToken( beg, src, s );
            break;
         case 15: // FF - MetaEvent
            src += parseMetaEvent( beg, src, end, dt, d1 );
            break;
      }
      m_curTrack.m_stats.m_metaCount++;
   }
   else
   {
      DE_ERROR("[",(void*)src,"] ",hexStr(beg,src)," dt(",dt,") NOIMPL Unknown command(0x", hexStr( event ),"), runstat(0x", hexStr(m_runStatus),")" )
   }

   return src - beg;
}


std::string
MidiParser::parseMetaText( uint8_t const* beg, uint8_t const* end )
{
   std::stringstream ss;
   while ( beg < end )
   {
      uint8_t kk = *beg++;
      ss << char( kk );
   }

   std::string dat = ss.str();
   if ( !dat.empty() )
   {
      if ( dat.back() != 0x00 )
      {
         //DE_WARN("Last char not zero, add one")
         dat += char(0); // Add end of c-string, just to safe.
      }
   }

   return dat;
}


////////////////////////////
//
// Binasc::processMidiPitchBendWord -- convert a floating point number in
//   the range from +1.0 to -1.0 into a 14-point integer with -1.0 mapping
//   to 0 and +1.0 mapping to 2^15-1.  This integer will be packed into
//   two bytes, with the LSB coming first and containing the bottom
//   7-bits of the 14-bit value, then the MSB coming second and containing
//   the top 7-bits of the 14-bit value.
/*
int Binasc::processMidiPitchBendWord(std::ostream& out, const std::string& word,
      int lineNum) {
   if (word.size() < 2) {
      std::cerr << "Error on line: " << lineNum
           << ": 'p' needs to be followed immediately by "
           << "a floating-point number" << std::endl;
      return 0;
   }
   if (!(isdigit(word[1]) || word[1] == '.' || word[1] == '-'
         || word[1] == '+')) {
      std::cerr << "Error on line: " << lineNum
           << ": 'p' needs to be followed immediately by "
           << "a floating-point number" << std::endl;
      return 0;
   }
   double value = strtod(&word[1], NULL);

   if (value > 1.0) {
      value = 1.0;
   }
   if (value < -1.0) {
      value = -1.0;
   }

   int intval = (int)(((1 << 13)-0.5)  * (value + 1.0) + 0.5);
   uchar LSB = intval & 0x7f;
   uchar MSB = (intval >>  7) & 0x7f;
   out << LSB << MSB;
   return 1;
}
*/


namespace {

int powi( int base, int exponent )
{
   if ( exponent == 0 ) return 1;

   int k = base;
   for ( int i = 1; i < exponent; ++i )
   {
      k *= base;
   }
   return k;
}

} // end namespace


uint32_t
MidiParser::parseMetaEvent(uint8_t const* ori,
                           uint8_t const* beg,
                           uint8_t const* end,
                           uint32_t dt,
                           uint8_t metatype )
{
   // meta messages must have at least three bytes:
   //    0: 0xff == meta message marker
   //    1: meta message type
   //    2: meta message data bytes to follow
   auto src = beg;

   std::string s;

   size_t n = 0;

   // FF 00 02 Sequence number, 2bytes, big-endian decimal val.
   if ( metatype == 0 )
   {
      uint8_t c3 = *src++; // Must be 0x02 for FF 00 02
      uint8_t c4 = *src++;
      int number = c4;
      s = dbStr( "dt(",dt,") FF 00 02 - SequenceNumber(",number,")" );
      if ( c3 != 0x02 )
      {
         s += " ERROR c3(" + hexStr(c3) + ") != 0x02";
      }
      emitToken( ori, src, s );
   }
   // FF 01 - text
   // FF 02 - copyright
   // FF 03 - track name
   // FF 04 - instrument name
   // FF 05 - lyric text
   // FF 06 - marker text
   // FF 07 - cue point
   // FF 08 - program name
   // FF 09 - device name
   else if ( metatype > 0 && metatype < 10 )
   {
      uint32_t dataSize = 0;
      n = MidiTools::parseVLQ( src, end, dataSize );
      if ( n < 1 )
      {
         DE_ERROR("FF ",hexStr(metatype)," :: ParseError :: Expected VLQ, but not found")
      }
      else
      {
         // Advance to text begin.
         src += n;

         // Parse text.
         std::string dat = parseMetaText( src, src + dataSize );

         // Advance to byte after text = text end.
         src += dataSize;

         // Switch text meta type...
         if      (metatype == 1) // text
         {
            s = dbStr("dt(",dt,") FF 01 - Text(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 2) // copyright
         {
            s = dbStr("dt(",dt,") FF 02 - Copyright(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 3) // track name
         {
            s = dbStr("dt(",dt,") FF 03 - TrackName(",dat,"), dataSize = ", dataSize );
            m_curTrack.m_name = dat;
         }
         else if (metatype == 4) // instrument name
         {
            s = dbStr("dt(",dt,") FF 04 - InstrumentName(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 5) // lyric
         {
            s = dbStr("dt(",dt,") FF 05 - LyricText(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 6) // marker
         {
            s = dbStr("dt(",dt,") FF 06 - MarkerText(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 7) // cue point
         {
            s = dbStr("dt(",dt,") FF 07 - CuePoint(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 8) // program name
         {
            s = dbStr("dt(",dt,") FF 08 - ProgramName(",dat,"), dataSize = ", dataSize );
         }
         else if (metatype == 9) // device name
         {
            s = dbStr("dt(",dt,") FF 09 - DeviceDame(",dat,"), dataSize = ", dataSize );
         }

         emitToken( ori, src, s );
      }
   }
   // FF 20 01 - MIDI channel prefix
   else if (metatype == 0x20)
   {
      uint8_t c3 = *src++; // Must be 0x01
      uint8_t c4 = *src++;
      s = dbStr("dt(",dt,") FF 20 01 - Channel prefix ", int(c4) );
      if ( c3 != 0x01 )
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x02";
      }
      emitToken( ori, src, s );
   }
   // FF 21 (01) - MIDI port display single-byte decimal number
   else if (metatype == 0x21)
   {
      uint8_t c3 = *src++;
      int port = *src++;
      s = dbStr("dt(",dt,") FF 21 (01)? - MIDI port display = ", port );
      if (c3 != 0x01)
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x01";
      }
      emitToken( ori, src, s );
   }
   // FF 2F 00 - MIDI EndOfTrack
   else if (metatype == 0x2F)
   {
      uint8_t c3 = *src++;
      s = dbStr("dt(",dt,") FF 2F 00 - EndOfTrack" );
      if (c3 != 0x00)
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x00";
      }
      emitToken( ori, src, s );
   }
   // FF 51 03 tttttt - Set Tempo ( in 10^-6 sec per quarter-note )
   // getTempoBPM -- Returns the tempo in terms of beats per minute.
   //                return 60000000.0 / (double)microseconds;
   // getTempoTPS -- Returns the tempo in terms of ticks per seconds.
   //                return tpq * 1000000.0 / (double)microseconds;
   // getTempoSPT -- Returns the tempo in terms of seconds per tick.
   //                return (double)microseconds / 1000000.0 / tpq;
   else if (metatype == 0x51)
   {
      uint8_t c3 = *src++; // Must be 0x03
      uint8_t c4 = *src++;
      uint8_t c5 = *src++;
      uint8_t c6 = *src++;
      int us = (c4 << 8) | c5;   // 3 bytes encoding Microseconds us.
      us = (us << 8) | c6;
      auto tempo = double(60 * 1000 * 1000) / double(us);
      s = dbStr("dt(",dt,") FF 51 03 - SetTempo(", tempo, "), microseconds(",us, ")" );
      if ( c3 != 0x03 )
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x03";
      }
      emitToken( ori, src, s );
      m_curTrack.addTempo( m_currTick, tempo );
   }
   // FF 54 05 hh mm ss fc sf - SMPTE offset
   else if (metatype == 0x54)
   {
      uint8_t c3 = *src++; // must be 0x05
      int hh = *src++; // hours
      int mm = *src++; // minutes
      int ss = *src++; // seconds
      int fc = *src++; // frames
      int sf = *src++; // sub frames
      s = dbStr("dt(",dt,") FF 54 05 - SMPTE(",hh,":",mm,":",ss,"), fc(",fc,"), subfc(",sf,")");
      if ( c3 != 0x05 )
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x05";
      }
      emitToken( ori, src, s );
   }
   // FF 58 04 nn dd cc bb time signature
   // FF 58 04 06 03 24 08
   // That is, 6/8 time (8 is 2 to the 3rd power, so this is 06 03),
   // 36 MIDI clocks per dotted-quarter (24 hex!),
   // and eight notated 32nd-notes per quarter-note.
   else if (metatype == 0x58)
   {
      uint8_t c3 = *src++; // must be 0x04
      int nn = *src++; // numerator
      int dd = *src++; // denominator = 2^dd, dd=0 -> 1
      dd = powi( 2, dd );
      int cc = *src++; // clocks per beat
      int bb = *src++; // 32nd notes per beat
      s = dbStr("dt(",dt,") FF 58 04 - TimeSignature(",nn,"/",dd,"), cpb(",cc,"), 32nd_notes_per_beat(",bb,")" );
      if ( c3 != 0x04 )
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x04";
      }
      emitToken( ori, src, s );
      emitTimeSignature( m_currTick, nn, dd, cc, bb );

   }
   // FF 59 02 sf mi Key Signature
   // sf = -7: 7 flats
   // sf = -1: 1 flat
   // sf = 0: key of C
   // sf = 1: 1 sharp
   // sf = 7: 7 sharps
   // mi = 0: major key
   // mi = 1: minor key
   else if (metatype == 0x59) // key signature
   {
      uint8_t c3 = *src++; // <size> must be 0x02
      uint8_t c4 = *src++; // <accid> sf
      uint8_t c5 = *src++; // <mode> mi
      int tonart = *reinterpret_cast< int8_t* >( &c4 ); // interpret as signed int.
      int minor = c5; // major = 0, minor = 1
      s = dbStr("dt(",dt,") FF 59 02 - KeySignature(Cdur + ",tonart,")"
          ", minor(",minor,")" );
      if ( c3 != 0x02 )
      {
         s += " ERROR c3(0x" + hexStr(c3) + ") != 0x02";
      }
      emitToken( ori, src, s );
   }
   // Unknown FF 7F but hopefully understood, next byte is a size, skips these bytes.
   else if (metatype == 0x7F)
   {
      uint8_t c3 = *src++;
      s = dbStr("dt(",dt,") FF 7F ", hexStr(c3), " - UNKNOWN, try skip" );
      if ( c3 < 1 )
      {
         s += " ERROR invalid size";
      }
      src += c3;
      emitToken( ori, src, s );
   }
   else
   {
      uint32_t dataSize = 0;
      n = MidiTools::parseVLQ( src,end, dataSize );
      if (n < 1 )
      {
         DE_ERROR("NO VLQ")
      }

      src += n;

      s = dbStr("dt(",dt,") FF ",hexStr(metatype)," - UNKNOWN META TYPE"
                                                      " :: Probably FATAL." );
      //DE_DEBUG(name, hexStr(ori,src), s )
      emitToken( ori, src, s );
   }

   return src - beg; // not ori, dt and 2 bytes were already parsed
}



/*
//
// MidiMessage::setSpelling -- Encode a MidiPlus accidental state for a note.
//    For example, if a note's key number is 60, the enharmonic pitch name
//    could be any of these possibilities:
//        C, B-sharp, D-double-flat
//    MIDI note 60 is ambiguous as to which of these names are intended,
//    so MIDIPlus allows these mappings to be preserved for later recovery.
//    See Chapter 5 (pp. 99-104) of Beyond MIDI (1997).
//
//    The first parameter is the diatonic pitch number (or pitch class
//    if the octave is set to 0):
//       octave * 7 + 0 = C pitches
//       octave * 7 + 1 = D pitches
//       octave * 7 + 2 = E pitches
//       octave * 7 + 3 = F pitches
//       octave * 7 + 4 = G pitches
//       octave * 7 + 5 = A pitches
//       octave * 7 + 6 = B pitches
//
//    The second parameter is the semitone alteration (accidental).
//    0 = natural state, 1 = sharp, 2 = double sharp, -1 = flat,
//    -2 = double flat.
//
//    Only note-on messages can be processed (other messages will be
//    silently ignored).
//

void MidiMessage::setSpelling(int base7, int accidental)
{
   if (!isNoteOn()) {
      return;
   }
   // The bottom two bits of the attack velocity are used for the
   // spelling, so need to make sure the velocity will not accidentally
   // be set to zero (and make the note-on a note-off).
   if (getVelocity() < 4) {
      setVelocity(4);
   }
   int dpc = base7 % 7;
   uchar spelling = 0;

   // Table 5.1, page 101 in Beyond MIDI (1997)
   // http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
   switch (dpc) {

      case 0:
         switch (accidental) {
            case -2: spelling = 1; break; // Cbb
            case -1: spelling = 1; break; // Cb
            case  0: spelling = 2; break; // C
            case +1: spelling = 2; break; // C#
            case +2: spelling = 3; break; // C##
         }
         break;

      case 1:
         switch (accidental) {
            case -2: spelling = 1; break; // Dbb
            case -1: spelling = 1; break; // Db
            case  0: spelling = 2; break; // D
            case +1: spelling = 3; break; // D#
            case +2: spelling = 3; break; // D##
         }
         break;

      case 2:
         switch (accidental) {
            case -2: spelling = 1; break; // Ebb
            case -1: spelling = 2; break; // Eb
            case  0: spelling = 2; break; // E
            case +1: spelling = 3; break; // E#
            case +2: spelling = 3; break; // E##
         }
         break;

      case 3:
         switch (accidental) {
            case -2: spelling = 1; break; // Fbb
            case -1: spelling = 1; break; // Fb
            case  0: spelling = 2; break; // F
            case +1: spelling = 2; break; // F#
            case +2: spelling = 3; break; // F##
            case +3: spelling = 3; break; // F###
         }
         break;

      case 4:
         switch (accidental) {
            case -2: spelling = 1; break; // Gbb
            case -1: spelling = 1; break; // Gb
            case  0: spelling = 2; break; // G
            case +1: spelling = 2; break; // G#
            case +2: spelling = 3; break; // G##
         }
         break;

      case 5:
         switch (accidental) {
            case -2: spelling = 1; break; // Abb
            case -1: spelling = 1; break; // Ab
            case  0: spelling = 2; break; // A
            case +1: spelling = 3; break; // A#
            case +2: spelling = 3; break; // A##
         }
         break;

      case 6:
         switch (accidental) {
            case -2: spelling = 1; break; // Bbb
            case -1: spelling = 2; break; // Bb
            case  0: spelling = 2; break; // B
            case +1: spelling = 3; break; // B#
            case +2: spelling = 3; break; // B##
         }
         break;

   }

   uchar vel = static_cast<uchar>(getVelocity());
   // suppress any previous content in the first two bits:
   vel = vel & 0xFC;
   // insert the spelling code:
   vel = vel | spelling;
   setVelocity(vel);
}

//////////////////////////////
//
// MidiMessage::getSpelling -- Return the diatonic pitch class and accidental
//    for a note-on's key number.  The MIDI file must be encoded with MIDIPlus
//    pitch spelling codes for this function to return valid data; otherwise,
//    it will return a neutral fixed spelling for each MIDI key.
//
//    The first parameter will be filled in with the base-7 diatonic pitch:
//        pc + octave * 7
//     where pc is the numbers 0 through 6 representing the pitch classes
//     C through B, the octave is MIDI octave (not the scientific pitch
//     octave which is one less than the MIDI ocatave, such as C4 = middle C).
//     The second number is the accidental for the base-7 pitch.
//

void MidiMessage::getSpelling(int& base7, int& accidental)
{
   if (!isNoteOn()) {
      return;
   }
   base7 = -123456;
   accidental = 123456;
   int base12   = getKeyNumber();
   int octave   = base12 / 12;
   int base12pc = base12 - octave * 12;
   int base7pc  = 0;
   int spelling = 0x03 & getVelocity();

   // Table 5.1, page 101 in Beyond MIDI (1997)
   // http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
   switch (base12pc) {

      case 0:
         switch (spelling) {
                    case 1: base7pc = 1; accidental = -2; break;  // Dbb
            case 0: case 2: base7pc = 0; accidental =  0; break;  // C
                    case 3: base7pc = 6; accidental = +1; octave--; break;  // B#
         }
         break;

      case 1:
         switch (spelling) {
                    case 1: base7pc = 1; accidental = -1; break;  // Db
            case 0: case 2: base7pc = 0; accidental = +1; break;  // C#
                    case 3: base7pc = 6; accidental = +2; octave--; break;  // B##
         }
         break;

      case 2:
         switch (spelling) {
                    case 1: base7pc = 2; accidental = -2; break;  // Ebb
            case 0: case 2: base7pc = 1; accidental =  0; break;  // D
                    case 3: base7pc = 0; accidental = +2; break;  // C##
         }
         break;

      case 3:
         switch (spelling) {
                    case 1: base7pc = 3; accidental = -2; break;  // Fbb
            case 0: case 2: base7pc = 2; accidental = -1; break;  // Eb
                    case 3: base7pc = 1; accidental = +1; break;  // D#
         }
         break;

      case 4:
         switch (spelling) {
                    case 1: base7pc = 3; accidental = -1; break;  // Fb
            case 0: case 2: base7pc = 2; accidental =  0; break;  // E
                    case 3: base7pc = 1; accidental = +2; break;  // D##
         }
         break;

      case 5:
         switch (spelling) {
                    case 1: base7pc = 4; accidental = -2; break;  // Gbb
            case 0: case 2: base7pc = 3; accidental =  0; break;  // F
                    case 3: base7pc = 2; accidental = +1; break;  // E#
         }
         break;

      case 6:
         switch (spelling) {
                    case 1: base7pc = 4; accidental = -1; break;  // Gb
            case 0: case 2: base7pc = 3; accidental = +1; break;  // F#
                    case 3: base7pc = 2; accidental = +2; break;  // E##
         }
         break;

      case 7:
         switch (spelling) {
                    case 1: base7pc = 5; accidental = -2; break;  // Abb
            case 0: case 2: base7pc = 4; accidental =  0; break;  // G
                    case 3: base7pc = 3; accidental = +2; break;  // F##
         }
         break;

      case 8:
         switch (spelling) {
                    case 1: base7pc = 5; accidental = -1; break;  // Ab
            case 0: case 2: base7pc = 4; accidental = +1; break;  // G#
                    case 3: base7pc = 3; accidental = +3; break;  // F###
         }
         break;

      case 9:
         switch (spelling) {
                    case 1: base7pc = 6; accidental = -2; break;  // Bbb
            case 0: case 2: base7pc = 5; accidental =  0; break;  // A
                    case 3: base7pc = 4; accidental = +2; break;  // G##
         }
         break;

      case 10:
         switch (spelling) {
                    case 1: base7pc = 0; accidental = -2; octave++; break;  // Cbb
            case 0: case 2: base7pc = 6; accidental = -1; break;  // Bb
                    case 3: base7pc = 5; accidental = +1; break;  // A#
         }
         break;

      case 11:
         switch (spelling) {
                    case 1: base7pc = 0; accidental = -1; octave++; break;  // Cb
            case 0: case 2: base7pc = 6; accidental =  0; break;  // B
                    case 3: base7pc = 5; accidental = +2; break;  // A##
         }
         break;

   }

   base7 = base7pc + 7 * octave;
}

static
int
MidiParser::findActualEventLength( uint8_t const* data, int maxBytes )
{
   auto byte = uint32_t( *data );

   if (byte == 0xf0 || byte == 0xf7)
   {
      int i = 1;

      while ( i < maxBytes )
      {
         if ( data[ i++ ] == 0xf7 )
         {
            break;
         }
      }

      return i;
   }

   if (byte == 0xff)
   {
      if ( maxBytes == 1 ) return 1;

      int value = readVLQ( data + 1, maxBytes - 1, nullptr );
      return std::min( maxBytes, value + 2 );
   }

   if (byte >= 0x80)
      return std::min( maxBytes, getMessageLengthFromFirstByte ((uint8) byte));

   return 0;
}

*/


} // end namespace midi
} // end namespace de
