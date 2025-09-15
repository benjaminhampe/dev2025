#pragma once
#include <de/midi/MidiFile.h>
#include <assert.h>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct Listener : public IParserListener
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.file.Listener")

   File* m_file;
   Track m_track; // current Track
   int m_trackNumber;

   Listener() : m_file(nullptr), m_trackNumber(0) {}
   ~Listener() override {}

   void setFile( File* file )
   {
      m_file = file;
   }

// I.:

   void mpStart( uint8_t const* beg, uint8_t const* end, std::string const & fileName ) override
   {
      (void)beg;
      (void)end;
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      m_file->reset();
      m_file->m_fileName = fileName;
   }
   void mpEnd() override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      m_file->finalize();
   }

// II.:

   void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote ) override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      m_file->m_fileType = fileType;
      m_file->m_trackCount = trackCount;
      m_file->m_ticksPerQuarterNote = ticksPerQuarterNote;
      m_track = Track( m_trackNumber );

      m_file->m_tempoMap.mpFileHeader( fileType, trackCount, ticksPerQuarterNote );
   }
   void mpTrackStart( int trackNumber ) override
   {

   }
   void mpTrackEnd() override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      m_file->m_tracks.emplace_back( std::move( m_track ) );
      ++m_trackNumber;
      m_track = Track( m_trackNumber );
   }

// III.:

   void mpSetTempo( uint64_t tick, float bpm, int microsecondsPerQuarterNote ) override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      m_file->m_tempoMap.mpSetTempo( tick, bpm, microsecondsPerQuarterNote );

      SetTempoEvent e = SetTempoEvent::create( tick, bpm, microsecondsPerQuarterNote );
      m_track.m_setTempoEvents.emplace_back( std::move(e) );
   }
   void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity ) override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      NoteOnEvent noteOn;
      noteOn.m_tick = tick;
      noteOn.m_channel = uint8_t( channel ) & 0x0F;
      noteOn.m_midiNote = uint8_t( midiNote ) & 0x7F;
      noteOn.m_velocity = uint8_t( velocity ) & 0x7F;
      m_track.getChannel(channel).m_noteOnEvents.emplace_back( std::move( noteOn ) );

      NoteEvent note;
      note.m_channel = channel;
      note.m_midiNote = midiNote;
      note.m_attack = velocity;
      note.m_attackMs = tick;
      note.m_release = velocity;
      note.m_releaseMs = tick;
      m_track.getChannel(channel).addNote( note );

      m_file->m_tempoMap.mpNoteOn( tick, channel, midiNote, velocity );
   }
   void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity ) override
   {
      if ( !m_file ) { std::runtime_error("No File pointer"); }
      NoteOffEvent noteOff;
      noteOff.m_tick = tick;
      noteOff.m_channel = uint8_t( channel ) & 0x0F;
      noteOff.m_midiNote = uint8_t( midiNote ) & 0x7F;
      noteOff.m_velocity = uint8_t( velocity ) & 0x7F;
      m_track.getChannel(channel).m_noteOffEvents.emplace_back( std::move( noteOff ) );

      NoteEvent* note = m_track.getChannel(channel).getLastNote( midiNote );
      if ( !note )
      {
         DE_ERROR("Last note not found.")
         //DE_ERROR("No last note to finish ch(",select,"), midiNote(",midiNote,"), release(",velocity,")" )
         return;
      }
      note->m_releaseMs = tick;
      note->m_release = velocity;

      m_file->m_tempoMap.mpNoteOff( tick, channel, midiNote, velocity );
   }
   void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat ) override
   {
      TimeSignatureEvent e;
      e.m_tick = tick;
      e.m_top = top;
      e.m_bottom = bottom;
      e.m_clocksPerBeat = clocksPerBeat;
      e.m_n32rdPerBar = n32rd_per_beat;
      m_track.m_timeSignatureEvents.emplace_back( std::move(e) );
   }
   void mpKeySignature( uint64_t tick, int tonart_c_offset, int minor ) override
   {
      KeySignatureEvent e;
      e.m_tick = tick;
      e.m_cdur_offset = tonart_c_offset;
      e.m_minor = minor;
      m_track.m_keySignatureEvents.emplace_back( std::move(e) );
   }
/*
   void mpSMPTEOffset( uint64_t tick, int hh, int mm, int ss, int fc, int sf ) override
   {
      (void)tick;
      (void)hh;
      (void)mm;
      (void)ss;
      (void)fc;
      (void)sf;

   }
*/
   void mpProgramChange( uint64_t tick, int channel, int program ) override
   {
      (void)tick;
      m_track.getChannel(channel).m_instrument = program;
   }
   void mpControlChange( uint64_t tick, int channel, int cc, int value ) override
   {
      ControlChangeEvent e;
      e.m_tick = tick;
      //e.m_channel = channel;
      e.m_cc = cc;
      e.m_value = value;
      m_track.getChannel(channel).getController(cc).addEvent( e );
   }
   void mpPolyphonicAftertouch( uint64_t tick, int value ) override
   {
      ControlChangeEvent e;
      e.m_tick = tick;
      e.m_value = value;
      m_track.m_polyphonicAftertouch.m_events.emplace_back( std::move(e) );
   }
   void mpChannelAftertouch( uint64_t tick, int channel, int value ) override
   {
      ControlChangeEvent e;
      e.m_tick = tick;
      e.m_value = value;
      m_track.getChannel(channel).m_channelAftertouch.m_events.emplace_back( std::move(e) );
   }
   void mpPitchBend( uint64_t tick, int channel, int value ) override
   {
      ControlChangeEvent e;
      e.m_tick = tick;
      e.m_value = value;
      m_track.getChannel(channel).m_pitchBend.m_events.emplace_back( std::move(e) );
   }
/*
   void mpSequenceNumber( uint64_t tick, int sequenceNumber ) override
   {
      (void)tick;
      (void)sequenceNumber;
   }
   void mpChannelPrefix( uint64_t tick, int channelPrefix ) override
   {
      (void)tick;
      (void)channelPrefix;
   }
   void mpPortDisplay( uint64_t tick, int port ) override
   {
      (void)tick;
      (void)port;
   }
*/
   void mpMetaText( uint64_t tick, std::string const & metaText ) override
   {
      if ( m_track.m_trackName.empty() )
      {
         m_track.m_trackName = metaText;
      }

      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaCopyright( uint64_t tick, std::string const & metaText ) override
   {
      if ( m_track.m_trackName.empty() )
      {
         m_track.m_trackName = metaText;
      }

      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaTrackName( uint64_t tick, std::string const & metaText ) override
   {
      if ( m_track.m_trackName.empty() )
      {
         m_track.m_trackName = metaText;
      }

      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaInstrumentName( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaLyric( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_lyricEvents.emplace_back( std::move(e) );
   }
   void mpMetaMarker( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaCuePoint( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaProgramName( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
   void mpMetaDeviceName( uint64_t tick, std::string const & metaText ) override
   {
      MetaEvent e;
      e.m_tick = tick;
      e.m_meta = 0;
      e.m_text = metaText;
      m_track.m_textEvents.emplace_back( std::move(e) );
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de
