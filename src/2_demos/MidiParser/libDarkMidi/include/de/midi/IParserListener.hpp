#pragma once
#include "MidiUtil.hpp"

namespace de {
namespace midi {

// The prefix 'mp' of the functions means 'MidiParser'.
//
// If you want your class to consume MidiData using the MidiParser,
// you have to derive your class from the listener and register your listener
// to the MidiParser.addListener( non_owning_ptr_to_your_class )
//
// Override all the methods you want to listen to ( change default behaviour ).
//
// Start parsing with MidiParser.parse( beg, end );

// =======================================================================
struct IParserListener
// =======================================================================
{
   virtual ~IParserListener() = default;

// Layer I.: Tokenizer

   /// @brief Used for debugging (like counting bytes) and pretty printing.
   /// The token includes the vlq time delta and following midi command.
   /// Used to count the bytes to verify if all bytes were parsed in the end.
   /// @param beg Listener receives the start byte address of parsed data ( for debugging )
   /// @param end Listener receives the end byte address of parsed data ( for debugging )
   /// @param comment Listener receives a debugString full of information by parser.
   virtual void mpToken( uint8_t const* beg, uint8_t const* end, std::string const & comment )
   {
      (void)beg;
      (void)end;
      (void)comment;
   }

   /// @brief Used for debugging (like counting bytes) and pretty printing.
   /// Relays the arguments given to MidiParser.parse(beg,end,fileName) call.
   /// @param beg Listener receives the start byte address of parsed data ( for debugging )
   /// @param end Listener receives the end byte address of parsed data ( for debugging )
   /// @param fileName Listener receives the fileName of the parsed midi file ( a title for html writer, etc...)
   virtual void mpStart( uint8_t const* beg, uint8_t const* end, std::string const & fileName )
   {
      (void)beg;
      (void)end;
      (void)fileName;
   }

   /// @brief Used for finalizing, post-processing, @see DurationComputer as example
   virtual void mpEnd()
   {

   }

   // MTrk <uint64_t dataSize>
   /// @brief Used for debugging (like counting bytes) and pretty printing.
   /// The token contains the entire track data including track-start and track-end markers.
   /// @param beg Listener receives the start byte address of parsed data ( for debugging )
   /// @param end Listener receives the end byte address of parsed data ( for debugging )
   /// @param trackNumber Current track number
   virtual void mpTrack( uint8_t const* beg, uint8_t const* end, int trackNumber )
   {
      (void)beg;
      (void)end;
      (void)trackNumber;
   }

// Layer II.:

   // MThd0006 <u16 fileType> <u16 trackCount> <u16 ticksPerBeat>
   virtual void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
   {
      (void)fileType;
      (void)trackCount;
      (void)ticksPerQuarterNote;
   }

   // MTrk <u32 dataSize>
   virtual void mpTrackStart( int trackNumber )
   {
      (void)trackNumber;
   }

   // FF 2F 00 - MIDI EndOfTrack
   // Not really importent for me. Just there because i debugged for it.
   virtual void mpTrackEnd()
   {
   }

// III.:

   // FF 51 03 tttttt - Set Tempo ( in 10^-6 sec per quarter-note )
   // Important event for clock, defines actual tempo in beats per minutes,
   // can occur anywhere, but mostly in the first track ( tempo map track ).
   virtual void mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerQuarterNote )
   {
      (void)tick;
      (void)beatsPerMinute;
      (void)microsecondsPerQuarterNote;
   }

   // FF 54 05 hh mm ss fc sf - SMPTE offset
   // Can occur anywhere, rather seldom and unsupported by my clock implementation (yet).
   // Dont know who needs it and why.
   virtual void mpSMPTEOffset( uint64_t tick, int hh, int mm, int ss, int fc, int sf )
   {
      (void)tick;
      (void)hh;
      (void)mm;
      (void)ss;
      (void)fc;
      (void)sf;
   }

   // FF 58 04 nn dd cc bb time signature
   // Important event for clock/metronom,
   // Importance of last two values arent fully understood by me (yet).
   virtual void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat )
   {
      (void)tick;
      (void)top;
      (void)bottom;
      (void)clocksPerBeat;
      (void)n32rd_per_beat;
   }

   // FF 59 02 sf mi Key Signature
   // Important for drawing sheetmusic if this is even possible with midi data alone.
   // Dont know (yet) if this influences the interpretation of note events.
   virtual void mpKeySignature( uint64_t tick, int tonart_c_offset, int minor )
   {
      (void)tick;
      (void)tonart_c_offset;
      (void)minor;
   }

   // This function is emitted very often, can bloat debuglog.
   virtual void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
   {
      (void)tick;
      (void)channel;
      (void)midiNote;
      (void)velocity;
   }

   // This function is emitted very often, can bloat debuglog.
   virtual void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
   {
      (void)tick;
      (void)channel;
      (void)midiNote;
      (void)velocity;
   }

   // Rather seldom event and mostly once in the begin of a track,
   // but strangely sometimes first encounter in the middle of a track.
   virtual void mpProgramChange( uint64_t tick, int channel, int program )
   {
      (void)tick;
      (void)channel;
      (void)program;
   }

   // Can bloat debuglog.
   virtual void mpControlChange( uint64_t tick, int channel, int cc, int value )
   {
      (void)tick;
      (void)channel;
      (void)cc;
      (void)value;
   }

   // Can bloat debuglog.
   virtual void mpPolyphonicAftertouch( uint64_t tick, int value )
   {
      (void)tick;
      (void)value;
   }

   // Can bloat debuglog.
   virtual void mpChannelAftertouch( uint64_t tick, int channel, int value )
   {
      (void)tick;
      (void)channel;
      (void)value;
   }

   // Can bloat debuglog.
   virtual void mpPitchBend( uint64_t tick, int channel, int value )
   {
      (void)tick;
      (void)channel;
      (void)value;
   }

   // Rather unimportant yet
   // FF 00 02 Sequence number, 2bytes, big-endian decimal val.
   virtual void mpSequenceNumber( uint64_t tick, int sequenceNumber )
   {
      (void)tick;
      (void)sequenceNumber;
   }

   // FF 20 01 <channelPrefix> - MIDI channel prefix
   virtual void mpChannelPrefix( uint64_t tick, int channelPrefix )
   {
      (void)tick;
      (void)channelPrefix;
   }

   // FF 21 01 <port> - MIDI port display single-byte decimal number
   virtual void mpPortDisplay( uint64_t tick, int port )
   {
      (void)tick;
      (void)port;
   }

   // This function collapses 9 text meta event functions in one function.
   // FF 01 - text
   // FF 02 - copyright
   // FF 03 - track name
   // FF 04 - instrument name
   // FF 05 - lyric text
   // FF 06 - marker text
   // FF 07 - cue point
   // FF 08 - program name
   // FF 09 - device name
   /*
   virtual void mpMeta( uint64_t tick, int metaType, std::string const & metaText )
   {
      std::cout << "mpMeta("<<tick<<","<<metaType<<","<<metaText<<")" << std::endl;
   }
   */
   // FF 01 - text
   virtual void mpMetaText( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 02 - copyright
   virtual void mpMetaCopyright( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 03 - track name
   virtual void mpMetaTrackName( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 04 - instrument name
   virtual void mpMetaInstrumentName( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 05 - lyric text
   virtual void mpMetaLyric( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 06 - marker text
   virtual void mpMetaMarker( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 07 - cue point
   virtual void mpMetaCuePoint( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 08 - program name
   virtual void mpMetaProgramName( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }

   // FF 09 - device name
   virtual void mpMetaDeviceName( uint64_t tick, std::string const & metaText )
   {
      (void)tick;
      (void)metaText;
   }
};

} // end namespace midi
} // end namespace de


