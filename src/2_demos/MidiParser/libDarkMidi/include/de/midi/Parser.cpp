#include <de/midi/Parser.hpp>

namespace de {
namespace midi {

Parser::Parser()
{

}

size_t
Parser::parse( uint8_t const* const beg, uint8_t const* const end, std::string const & uri )
{
   mpStart( beg, end, uri );

   m_fileType = 0;
   m_trackCount = 0;
   m_resolution = 0;

   m_trackIndex = 0;
   m_runStatus = 0;  // per track reset running status

   m_trackDataSize = 0;
   m_trackDataStart = nullptr;

   m_currTick = 0;   // per track reset tick counter

   uint8_t const* src = beg;

   //DE_DEBUG("MIDI.fileSize = ", end - beg )
   //DE_DEBUG("MIDI.data = \n", hexStr( beg, end, 16 ) )

   size_t n = parseFileHeader( src, end );
   if ( n != 14 )
   {
      DE_ERROR("[",(void*)0,"] No Midi header ",uri )
      return 0;
   }
   src += n;

   size_t i = 0;                 // reset event counter
   size_t loopMax = 10000000000; // Prevent dead lock 10^10 events.

   while ( src < end ) //  && m_trackIndex < m_trackCount
   {
      n = parseMessage(src,end);
      if ( n < 1 )
      {
         DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex+1,"/",m_trackCount,") :: "
                  "No track header ",uri )
         break;
      }
      src += n;

      i++;
      if ( i >= loopMax )
      {
         DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex+1,"/",m_trackCount,") :: "
                  "Reached loop max ",uri )
         break;
      }
   }

   // Check outer parser state
   if ( src != end )
   {
      DE_WARN("Did not parse(",src-beg,") all bytes(",end-beg,"), ",uri)
   }

   //DE_WARN("\n",m_uri)

   mpEnd();

   return static_cast< size_t >(src - beg);
}

size_t
Parser::parseMessage( uint8_t const* const beg, uint8_t const* const end )
{
   uint8_t const* src = beg;

   size_t n = 0;

   // Parse Track header -> extract 'trackSize' in bytes.
   uint32_t dataSize = 0;
   n = parseTrackHeader( src, end, dataSize );
   src += n;

   n = parseTrackEnd( src, end );
   src += n;
   if ( n > 0 )
   {
      n = parseTrackHeader( src, end, dataSize );
      src += n;
   }

   n = parseEvent( src, end );
   src += n;

   return static_cast< size_t >(src - beg);
}

size_t
Parser::parseFileHeader( uint8_t const* const beg, uint8_t const* const end )
{
   // Not a 14 byte MIDI file header.
   if ((end - beg) < 14)
   {
      DE_ERROR("No enough bytes for file header" )
      return 0; // Means parser cant process this or advance(0).
   }

   // Dont touch 'beg', so introduce src/ptr/cur.
   uint8_t const* src = beg;

   // Read 8 bytes - must be same for all MIDI filestreams.
   if ( *src++ != 'M' ) { /*DE_ERROR("No M" )*/ return 0; }
   if ( *src++ != 'T' ) { /*DE_ERROR("No T" )*/ return 0; }
   if ( *src++ != 'h' ) { /*DE_ERROR("No h" )*/ return 0; }
   if ( *src++ != 'd' ) { /*DE_ERROR("No d" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 6 )   { /*DE_ERROR("No 6" )*/ return 0; }

   // For better printing we emit 2 tokens for long midi header. 8+6 byte tokens.
   //mpToken( beg, src, "MIDI header signature, 8 bytes, has size 6." );

   // We touch 'src', so introduce 'dat' to const store begin of 2nd token.
   //uint8_t const* dat = src;

   // Read last 6 bytes with 3x infos:
   // Make no additional error checks, because the 1st if(<14) covers them all.
   auto
   // Read 2 bytes for FileFormat
   n = MidiUtil::readU16_be( src, end, m_fileType );
   src += n;
   // Read 2 bytes for TrackCount
   n = MidiUtil::readU16_be( src, end, m_trackCount );
   src += n;
   // Read 2 bytes for TicksPerQuarterNote ( no 15th bit set ) or else.
   n = MidiUtil::readU16_be( src, end, m_resolution );
   src += n;

   // DEBUG
   std::string s = StringUtil::join( "MIDI file header fileType(", m_fileType, "), trackCount(", m_trackCount,"), " );

   // Parse MIDI clock resolution ( ticks ) and validate.
   int ticksPerQuarterNote = 96; // Give a default value ( kinda crude resolution )
   if ( m_resolution & 0x8000 ) // is bit 15 set in time resolution?
   {
      // Nobody explained to me what a frame is in the MIDI context. Never encountered yet.
      // DEBUG
      s += StringUtil::join( "ERROR clock(SMPTE,", m_resolution,",[ticks per frame])" );
   }
   else
   {
      // DEBUG
      s += StringUtil::join( "clock(normal,",m_resolution,"[ticks per quarter note])" );
      // Parse ticksPerQuarterNote:
      // kinda the most important information of MIDI file specific clock.
      ticksPerQuarterNote = int(m_resolution & 0x7FFF);
      if ( ticksPerQuarterNote < 1 )
      {
         // MORE DEBUG
         s += StringUtil::join( ", WARN no ticksPerQuarterNote, fallback to 96." );
         ticksPerQuarterNote = 96;
      }
   }

   // Validate FileType + TrackCount
   if ( m_fileType == 0 && m_trackCount != 1 )
   {
      s += StringUtil::join( ", WARN malformed format" );
      m_trackCount = 1;
   }

   mpToken( beg, src, s );
   mpFileHeader( int(m_fileType), int(m_trackCount), ticksPerQuarterNote );

   DE_DEBUG(s)

   return static_cast< size_t >(src - beg); // Must be 14 (bytes) for a valid header.
}

size_t
Parser::parseTrackHeader( uint8_t const* const beg, uint8_t const* const end, uint32_t & dataSize )
{
   // Should be atleast 8 bytes ( a guess ).
   if ((end - beg) < 8)
   {
      //DE_ERROR("Not enough bytes for track header" )
      return 0;
   }

   uint8_t const* src = beg;
   if ( *src != 'M' ) { /* DE_ERROR("No M of MTrk") */ return 0; }
   else { src++; } // advance
   if ( *src != 'T' ) { /* DE_ERROR("No T of MTrk" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 'r' ) { /* DE_ERROR("No r of MTrk" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 'k' ) { /* DE_ERROR("No k of MTrk" ) */ return 0; }
   else { src++; } // advance

   // We still expect a size...
   src += MidiUtil::readU32_be( src, end, m_trackDataSize );
   if ( m_trackDataSize < 1 )
   {
      DE_ERROR("No track dataSize found" )
   }

   m_trackDataStart = src;

   auto s = StringUtil::join( "NewTrack(",m_trackIndex+1,"/",m_trackCount,") header, dataSize = ", m_trackDataSize );

   mpToken( beg, src, s );
   mpTrackStart( m_trackIndex );

   //DE_DEBUG(s)

   return static_cast< size_t >(src - beg);
}

size_t
Parser::parseTrackEnd( uint8_t const* const beg, uint8_t const* const end )
{
   // Should be exactly 4 bytes ( 00 FF 2F 00 ) - EndOfTrack
   if ((end - beg) < 4)
   {
      //DE_ERROR("Not enough bytes for track end" )
      return 0;
   }

   uint8_t const* src = beg;
   if ( *src != 0x00 ) { /* DE_ERROR("No 0x00 of TrackEnd") */ return 0; }
   else { src++; } // advance
   if ( *src != 0xFF ) { /* DE_ERROR("No 0xFF of TrackEnd" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 0x2F ) { /* DE_ERROR("No 0x2F of TrackEnd" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 0x00 ) { /* DE_ERROR("No 0x00 of TrackEnd" ) */ return 0; }
   else { src++; } // advance

   auto s = StringUtil::join( "TrackEnd(",m_trackIndex+1,"/",m_trackCount,")" );

   mpToken( beg, src, s );
   mpTrackEnd();

   //DE_DEBUG(s)

   m_trackIndex++;   // advance track index
   m_currTick = 0;   // reset tick counter
   //m_runStatus = 0;  // reset running status

   return static_cast< size_t >(src - beg);
}


/*
size_t
Parser::parseTrackData( uint8_t const* const beg, uint8_t const* const end )
{
   mpTrack( beg, end, m_trackIndex ); // Tell listeners.

   uint8_t const* src = beg;

   m_currTick = 0;   // reset tick counter
   m_runStatus = 0;  // reset running status
   size_t i = 0;     // reset event counter

   size_t loopMax = 10000000000; // Prevent dead lock 10^10 events.

   while ( src < end && i < loopMax )
   {
      // Parse entire event, be able to skip it.
      size_t n = parseEvent( src, end );
      if ( n < 1 )
      {
         auto dataSize = size_t(end - beg);
         DE_DEBUG("[",(void*)(src-beg),"] "
                  "Track(",m_trackIndex,"/",m_trackCount,").Event[",i,"] :: "
                  "ParseError 0, n(",n,"), dataSize(",dataSize,")" )
         break;
      }

      // Skip entire event.
      src += n;
      i++;
      //m_curTrack.m_nEvents++;
   }
   return static_cast< size_t >(src - beg);
}
*/

size_t
Parser::parseEvent( uint8_t const* const beg, uint8_t const* const end )
{
   if ( end - beg < 1 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const* src = beg;

   std::string s; // comment
   std::string e; // error comment

   // [1.] Read delta-time
   uint32_t dt = 0;
   auto n = MidiUtil::parseVLQ( src, end, dt );
   if ( n < 1 )
   {
      DE_ERROR("No DeltaTime found") // Always emitted at EOF
      return 0;
   }
   src += n;

   //uint8_t const* dataBeg = src;

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
         e = StringUtil::join(  "- BAD runningStatus ", StringUtil::hex(m_runStatus) );
      }
      else
      {
         e = StringUtil::join(  "- OK runningStatus ", StringUtil::hex(m_runStatus) );
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
      d2 = *src++;         // Fetch param2
      int note = d1;       // Convert param1
      int velocity = d2;   // Convert param2
      bool isOn = command == 9 && velocity > 0; // Tatu.mid uses vel=0 for noteOff
      if ( isOn )
      {
         auto s = StringUtil::join( "dt(",dt,"), ch(",ch,") NOTE_ON(",MidiUtil::getNoteStr(note),
            "), velocity(", velocity,"), runstat(0x", StringUtil::hex(m_runStatus),")" );
         mpToken( beg, src, s );
         mpNoteOn( m_currTick, ch, note, velocity );
      }
      else
      {
         auto s = StringUtil::join( "dt(",dt,"), ch(",ch,") NOTE_OFF(",MidiUtil::getNoteStr(note),
            "), velocity(", velocity,"), runstat(0x", StringUtil::hex(m_runStatus),")" );
         mpToken( beg, src, s );
         mpNoteOff( m_currTick, ch, note, velocity );
      }
   }
   // [Ax] Handle PAT - Polyphonic Aftertouch, +2 data bytes.
   else if ( command == 10 )
   {
      d2 = *src++;         // Fetch param2
      int value = ( (d1 & 0x7F) << 7 ) | (d2 & 0x7F);
      s = StringUtil::join( "dt(",dt,"), ch(",ch,") AfterTouch(",value,"), "
            "d1(0x",StringUtil::hex(d1),"), "
            "d2(0x",StringUtil::hex(d2),"), "
            "runstat(0x", StringUtil::hex(m_runStatus),")" );
      mpToken( beg, src, s );
      mpPolyphonicAftertouch( m_currTick, d2 ); // 16383 not 127 range
   }
   // [Bx] Handle CC - Continuous controller, +2 data bytes.
   else if ( command == 11 )
   {
      d2 = *src++;         // Fetch param2
      int cc = d1;
      int value = d2;
      auto ccStr = CC_toString( EMidiCC(d1) );
      s = StringUtil::join( "dt(",dt,"), ch(",ch,") ",ccStr,"(0x",StringUtil::hex(d2),"), "
                             "runstat(0x", StringUtil::hex(m_runStatus),")" );
      mpToken( beg, src, s );
      mpControlChange( m_currTick, ch, cc, value );
   }
   // [Cx] ProgramChange, +1 data byte.
   else if ( command == 12 )
   {
      s = StringUtil::join( "dt(",dt,"), ch(",ch,") ",
                             GM_toString( EMidiGM(d1) ), "(0x",StringUtil::hex(d1),"), "
                             "runstat(0x", StringUtil::hex(m_runStatus),")" );
      mpToken( beg, src, s );
      mpProgramChange( m_currTick, ch, d1 );
   }
   // [CP] ChannelPressure, TODO: +1 data byte.? Or 2 ?
   else if ( command == 13 )
   {
      int value = d1;
      s = StringUtil::join( "dt(",dt,"), ch(",ch,") ChannelPressure(",value,"), "
                             "d1(0x",StringUtil::hex(d1),"), "
                             "runstat(0x", StringUtil::hex(m_runStatus),")" );
      mpToken( beg, src, s );
      mpChannelAftertouch( m_currTick, ch, value );
   }
   // [PB] PitchBend, +2 data bytes.
   // Status byte : 1110 CCCC
   // Data byte 1 : 0LLL LLLL (d1=LSB)
   // Data byte 2 : 0MMM MMMM (d2=MSB) -> value = (( d2 & 0x7F ) << 7) | ( d1 & 0x7F )
   else if ( command == 14 )
   {
      d2 = *src++;
      int bend = (( d2 & 0x7F ) << 7) | ( d1 & 0x7F );
      s = StringUtil::join( "dt(",dt,"), ch(",ch,") PitchBend(",bend,"), "
                             "d1(0x",StringUtil::hex(d1),"), "
                             "d2(0x",StringUtil::hex(d2),"), "
                             "runstat(0x", StringUtil::hex(m_runStatus),")" );
      mpToken( beg, src, s );
      mpPitchBend( m_currTick, ch, bend );
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
            s = StringUtil::join( "dt(",dt,") F0 - Sysex begin, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")" );
            mpToken( beg, src, s );
          }
            break;
         case 1: // F1 - Time Code QuarterNote Frame
            d2 = *src++;
            s = StringUtil::join( "dt(",dt,") F1 - TimeCode Quarter Frame(",int(d2),"), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF1_TimeCodeQuarterNoteFrame( m_currTick, int(d2) );
            break;
         case 2: // F2 - Song Position Pointer
            d2 = *src++;
            s = StringUtil::join( "dt(",dt,") F2 - Song Position Pointer(",int(d2),"), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF2_SongPositionPointer( m_currTick, int(d2) );
            break;
         case 3: // F3 - Song Select (Song #) 1byte (0-127)
            d2 = *src++;
            s = StringUtil::join( "dt(",dt,") F3 - Song Select(",int(d2),"), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF3_SongSelect( m_currTick, int(d2) );
            break;
         case 4: // F4 - Undefined (Reserved)
            s = StringUtil::join( "dt(",dt,") F4 - Undefined (Reserved), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF4( m_currTick,  );
            break;
         case 5: // F5 - Undefined (Reserved)
            s = StringUtil::join( "dt(",dt,") F5 - Undefined (Reserved), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF5( m_currTick, );
            break;
         case 6: // F6 - TuneRequest
            s = StringUtil::join( "dt(",dt,") NOIMPL F6 - TuneRequest, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF6_TuneRequest( m_currTick );
            break;
         case 7: // F7 - End System Exclusive
          {
            DE_ERROR("Should not run")
            uint8_t k = *src++;
            while ( k != 0xF0 )
            {
               k = *src++;
            }
            s = StringUtil::join( "dt(",dt,") F7 - SysEx end., "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF7_SystemExclusive( m_currTick );
          }
            break;
         case 8: // F8 - Timing clock
            s = StringUtil::join( "dt(",dt,") NOIMPL F8 - Timing clock, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF8_TimingClock( m_currTick );
            break;
         case 9: // F9 - Undefined (Reserved)
            s = StringUtil::join( "dt(",dt,") NOIMPL F9 - Undefined (Reserved), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpF9( m_currTick );
            break;
         case 10: // FA - MidiClock Start
            s = StringUtil::join( "dt(",dt,") NOIMPL FA - MidiClock Start, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpFA_MidiClockStart( m_currTick );
            break;
         case 11: // FB - MidiClock Continue
            s = StringUtil::join( "dt(",dt,") NOIMPL FB - MidiClock Continue, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpFB_MidiClockContinue( m_currTick );
            break;
         case 12: // FC - MidiClock Stop
            s = StringUtil::join( "dt(",dt,") NOIMPL FC - MidiClock Stop, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpFC_MidiClockStop( m_currTick );
            break;
         case 13: // FD - Undefined (Reserved)
            s = StringUtil::join( "dt(",dt,") NOIMPL FD - Undefined (Reserved), "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpFD( m_currTick );
            break;
         case 14: // FE - Active Sensing
            s = StringUtil::join( "dt(",dt,") NOIMPL FE - Active Sensing, "
                                   "runstat(0x", StringUtil::hex(m_runStatus),")");
            mpToken( beg, src, s );
            //mpFE_ActiveSensing( m_currTick );
            break;
         case 15: // FF - MetaEvent
            src += parseMetaEvent( beg, src, end, dt, d1 );
            break;
      }
      //m_curTrack.m_nMetaEvents++;
   }
   else
   {
      DE_ERROR("[",(void*)src,"] ",StringUtil::hex(beg,src)," dt(",dt,") "
         "NOIMPL Unknown command(0x", StringUtil::hex( event ),"), "
         "runstat(0x", StringUtil::hex(m_runStatus),")" )
   }

   return static_cast< size_t >(src - beg);
}


std::string
Parser::parseMetaText( uint8_t const* const beg, uint8_t const* const end )
{
   std::stringstream ss;
   uint8_t const* src = beg;
   while ( src < end )
   {
      uint8_t kk = *src++;
      ss << char( kk );
   }

   std::string dat = ss.str();
   return dat;
}

size_t
Parser::parseMetaEvent(uint8_t const* const ori,
                           uint8_t const* const beg,
                           uint8_t const* const end,
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
      int sequenceNumber = int(c4);
      s = StringUtil::join(  "dt(",dt,") FF 00 02 - SequenceNumber(",sequenceNumber,")" );
      if ( c3 != 0x02 )
      {
         s += " ERROR c3(" + StringUtil::hex(c3) + ") != 0x02";
      }
      mpToken( ori, src, s );
      mpSequenceNumber( m_currTick, sequenceNumber );
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
      n = MidiUtil::parseVLQ( src, end, dataSize );
      if ( n < 1 )
      {
         //DE_ERROR("FF ",StringUtil::hex(metatype)," :: ParseError :: Expected VLQ, but not found")
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
         if (metatype == 1) // text
         {
            s = StringUtil::join( "dt(",dt,") FF 01 - Text(",dat,"), dataSize = ", dataSize );
            mpMetaText(m_currTick,dat);
         }
         else if (metatype == 2) // copyright
         {
            s = StringUtil::join( "dt(",dt,") FF 02 - Copyright(",dat,"), dataSize = ", dataSize );
            mpMetaCopyright(m_currTick,dat);
         }
         else if (metatype == 3) // track name
         {
            s = StringUtil::join( "dt(",dt,") FF 03 - TrackName(",dat,"), dataSize = ", dataSize );
            //m_curTrack.m_name = dat;
            mpMetaTrackName(m_currTick,dat);
         }
         else if (metatype == 4) // instrument name
         {
            s = StringUtil::join( "dt(",dt,") FF 04 - InstrumentName(",dat,"), dataSize = ", dataSize );
            mpMetaInstrumentName(m_currTick,dat);
         }
         else if (metatype == 5) // lyric
         {
            s = StringUtil::join( "dt(",dt,") FF 05 - LyricText(",dat,"), dataSize = ", dataSize );
            mpMetaLyric(m_currTick,dat);
         }
         else if (metatype == 6) // marker
         {
            s = StringUtil::join( "dt(",dt,") FF 06 - MarkerText(",dat,"), dataSize = ", dataSize );
            mpMetaMarker(m_currTick,dat);
         }
         else if (metatype == 7) // cue point
         {
            s = StringUtil::join( "dt(",dt,") FF 07 - CuePoint(",dat,"), dataSize = ", dataSize );
            mpMetaCuePoint(m_currTick,dat);
         }
         else if (metatype == 8) // program name
         {
            s = StringUtil::join( "dt(",dt,") FF 08 - ProgramName(",dat,"), dataSize = ", dataSize );
            mpMetaProgramName(m_currTick,dat);
         }
         else if (metatype == 9) // device name
         {
            s = StringUtil::join( "dt(",dt,") FF 09 - DeviceDame(",dat,"), dataSize = ", dataSize );
            mpMetaDeviceName(m_currTick,dat);
         }

         mpToken( ori, src, s );
      }
   }
   // FF 20 01 - MIDI channel prefix
   else if (metatype == 0x20)
   {
      uint8_t c3 = *src++; // Must be 0x01
      uint8_t c4 = *src++;
      s = StringUtil::join( "dt(",dt,") FF 20 01 - Channel prefix ", int(c4) );
      if ( c3 != 0x01 )
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x02";
      }
      mpToken( ori, src, s );
      mpChannelPrefix(m_currTick, int(c4) );
   }
   // FF 21 (01) - MIDI port display single-byte decimal number
   else if (metatype == 0x21)
   {
      uint8_t c3 = *src++;
      int port = *src++;
      s = StringUtil::join( "dt(",dt,") FF 21 (01)? - MIDI port display = ", port );
      if (c3 != 0x01)
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x01";
      }
      mpToken( ori, src, s );
      mpPortDisplay(m_currTick, port );
   }
   // FF 2F 00 - MIDI EndOfTrack
   else if (metatype == 0x2F)
   {
      uint8_t c3 = *src++;
      s = StringUtil::join( "dt(",dt,") FF 2F 00 - EndOfTrack" );
      if (c3 != 0x00)
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x00";
      }

      mpToken( ori, src, s );
      mpTrackEnd();

      m_trackIndex++;   // advance track index
      m_currTick = 0;   // reset tick counter
      //m_runStatus = 0;  // reset running status

      //throw std::runtime_error("Got EndOfTrack that should already be catched by parseTrackEnd() ");

      DE_WARN("Got EndOfTrack with VLQ != 0, TODO: add VLQ parsing to parseTrackEnd()?!?!")
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
      auto bpm = double(60 * 1000 * 1000) / double(us);
      s = StringUtil::join( "dt(",dt,") FF 51 03 - SetTempo(", bpm, "), microseconds(",us, ")" );
      if ( c3 != 0x03 )
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x03";
      }
      mpToken( ori, src, s );
      mpSetTempo( m_currTick, float(bpm), us );
      //m_curTrack.addTempo( m_currTick, tempo );
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
      s = StringUtil::join( "dt(",dt,") FF 54 05 - SMPTE(",hh,":",mm,":",ss,"), fc(",fc,"), subfc(",sf,")");
      if ( c3 != 0x05 )
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x05";
      }
      mpToken( ori, src, s );
      mpSMPTEOffset( m_currTick, hh, mm, ss, fc, sf );
   }
   // FF 58 04 nn dd cc bb time signature
   // FF 58 04 06 03 24 08
   // That is, 6/8 time (8 is 2 to the 3rd power, so this is 06 03),
   // 36 MIDI clocks per dotted-quarter (24 hex!),
   // and eight notated 32nd-notes per quarter-note.
   else if (metatype == 0x58)
   {
      uint8_t c3 = *src++; // must be 0x04
      int top = *src++; // numerator
      int bottom = MidiUtil::powi( 2, int(*src++) ); // denominator = 2^value, value = 0 -> 1
      int iClocksPerBeat = *src++; // clocks per beat
      int i32ndNotesPerBeat = *src++; // 32nd notes per beat
      s = StringUtil::join( "dt(",dt,") FF 58 04 - TimeSignature(",top,"/",bottom,"), "
                             "iClocksPerBeat(",iClocksPerBeat,"), 32nd_notes_per_beat(",i32ndNotesPerBeat,")" );
      if ( c3 != 0x04 )
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x04";
      }
      mpToken( ori, src, s );
      mpTimeSignature( m_currTick, top, bottom, iClocksPerBeat, i32ndNotesPerBeat );
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
      uint8_t c4 = *src++; // <tonart-offset> -7...+7
      int tonart = *reinterpret_cast< int8_t* >( &c4 ); // interpret as signed int.
      int minor = int(*src++); // <mode> major = 0, minor = 1
      s = StringUtil::join( "dt(",dt,") FF 59 02 - KeySignature(Cdur + ",tonart,"), minor(",minor,")" );
      if ( c3 != 0x02 )
      {
         s += " ERROR c3(0x" + StringUtil::hex(c3) + ") != 0x02";
      }
      mpToken( ori, src, s );
      mpKeySignature( m_currTick, tonart, minor );
   }
   // Unknown FF 7F but hopefully understood, next byte is a size, skips these bytes.
   else if (metatype == 0x7F)
   {
      uint8_t c3 = *src++;
      s = StringUtil::join( "dt(",dt,") FF 7F ", StringUtil::hex(c3), " - UNKNOWN, try skip" );
      if ( c3 < 1 )
      {
         s += " ERROR invalid size";
      }
      src += c3;  // Skip bytes
      mpToken( ori, src, s );
   }
   else
   {
      uint32_t dataSize = 0;
      n = MidiUtil::parseVLQ( src,end, dataSize );
      if (n < 1 )
      {
         //DE_ERROR("NO VLQ")
      }

      src += n;

      s = StringUtil::join( "dt(",dt,") FF ",StringUtil::hex(metatype),
                             " - UNKNOWN META TYPE :: Probably FATAL." );
      //DE_DEBUG(name, hexStr(ori,src), s )
      mpToken( ori, src, s );
   }

   return static_cast< size_t >(src - beg); // not ori, dt and 2 bytes were already parsed
}

} // end namespace midi
} // end namespace de
