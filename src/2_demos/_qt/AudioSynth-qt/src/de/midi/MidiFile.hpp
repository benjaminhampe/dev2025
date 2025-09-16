#pragma once
#include <de/midi/MidiTools.hpp>

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
struct MidiTempo
// =======================================================================
{
   uint32_t m_ptsBeg = 0;
   uint32_t m_ptsEnd = 0;
   float m_bpm = 0;     // bar tempo
   double m_timeBeg = 0;
   double m_timeEnd = 0;
};

// =======================================================================
struct MidiTimeSignature
// =======================================================================
{
   uint32_t m_ptsBeg = 0;
   uint32_t m_ptsEnd = 0;
   int m_clockNum = 4;  // bar tempo - numerator
   int m_clockDen = 4;  // bar tempo - denominator
   int m_clocksPerBeat = 24;         // bar tempo - clocks per beat
   int m_32n = 8;          // bar tempo - 32nd per bar = 8ths per beat
   int m_upb = 646000;     // bar tempo - microseconds per beat
};

// =======================================================================
struct MidiTempoMap
// =======================================================================
{
   std::vector< MidiTempo > m_tempos;
   std::vector< MidiTimeSignature > m_timesigs;

   void addTimeSignature( uint32_t pts, int num, int den, int cpb, int n32rd_per_qn )
   {
      if ( m_timesigs.size() > 0 )
      {
         m_timesigs.back().m_ptsEnd = pts;
      }

      m_timesigs.emplace_back();
      auto & t = m_timesigs.back();
      t.m_ptsBeg = pts;
      t.m_ptsEnd = pts;
      t.m_clockNum = num;
      t.m_clockDen = den;
      t.m_clocksPerBeat = cpb;
      t.m_32n = n32rd_per_qn;
   }

   void addTempo( uint32_t pts, float bpm )
   {
      if ( m_tempos.size() > 0 )
      {
         m_tempos.back().m_ptsEnd = pts;
      }

      m_tempos.emplace_back();
      auto & t = m_tempos.back();
      t.m_ptsBeg = pts;
      t.m_ptsEnd = pts;
      t.m_bpm = bpm;
   }

};

// =======================================================================
struct MidiNote
// =======================================================================
{
   int m_channel = 0;
   int m_midinote = 0;
   int m_velocity = 0;
   uint32_t m_ppqBeg = 0;
   uint32_t m_ppqEnd = 0;
   uint32_t m_ticksPerQuarterNote = 96;
   uint32_t m_microsPerQuarterNote = 606000;
//   uint32_t m_noteNum = 1; // For a
//   uint32_t m_noteDen = 4; // quarter note in 4/4
//   int m_clockNum = 4;  // bar tempo - numerator
//   int m_clockDen = 4;  // bar tempo - denominator
//   float m_bpm = 0;     // bar tempo
//   int m_cpb = 24;         // bar tempo - clocks per beat
//   int m_32n = 8;          // bar tempo - 32nd per bar = 8ths per beat
//   int m_upb = 646000;     // bar tempo - microseconds per beat

   uint32_t duration() const { return m_ppqEnd - m_ppqBeg; }


   std::string
   toString() const
   {
      std::stringstream s;
      s << MidiTools::noteStr( m_midinote ) << ", "
            "ch(" << m_channel << "), "
            "velocity(" << m_velocity << "), "
            "ppqBeg(" << m_ppqBeg << "), "
            "ppqEnd(" << m_ppqEnd << "), "
            "duration(" << duration() << "), "
            "ticksPerQuarterNote(" << m_ticksPerQuarterNote << ")";

      return s.str();
   }

};

// =======================================================================
struct MidiStats
// =======================================================================
{
   int m_trackIndex;
   int m_channel;
   uint32_t m_ppqBeg;
   uint32_t m_ppqEnd;
   uint32_t m_tickCount;
   uint32_t m_eventCount;
   uint32_t m_metaCount;
   uint32_t m_noteOnCount;
   uint32_t m_noteOffCount;
   uint32_t m_tempoCount;
   uint32_t m_instrumentCount;
   uint32_t m_controlCount;
   uint32_t m_pitchBendCount;
   uint32_t m_sustainPedalCount;
   uint32_t m_softPedalCount;

   MidiStats()
   {
      reset();
   }

   void reset()
   {
      m_trackIndex = 0;
      m_channel = 0;
      m_ppqBeg = 0;
      m_ppqEnd = 0;
      m_tickCount = 0;
      m_eventCount = 0;
      m_metaCount = 0;
      m_noteOnCount = 0;
      m_noteOffCount = 0;
      m_tempoCount = 0;
      m_instrumentCount = 0;
      m_controlCount = 0;
      m_pitchBendCount = 0;
      m_sustainPedalCount = 0;
      m_softPedalCount = 0;
   }

   uint32_t duration() const { return m_ppqEnd - m_ppqBeg; }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "Channel(" << m_channel << "), "
            "ppqBeg(" << m_ppqBeg << "), "
            "ppqEnd(" << m_ppqEnd << "), "
            "instrumentCount(" << m_instrumentCount << "), "
            "duration(" << duration() << ")";

      return s.str();
   }
};

// =======================================================================
struct MidiTrackChannel
// =======================================================================
{
   int m_id;
   int m_channel;
   std::vector< MidiNote > m_notes;
   MidiStats m_stats;

   uint32_t duration() const { return m_stats.duration(); }

   MidiTrackChannel()
   {
      reset();
   }

   ~MidiTrackChannel()
   {
      reset();
   }

   void reset()
   {
      m_id = 0;
      m_channel = 0;
      m_stats.reset();
      m_notes.clear();
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "MidiTrackChannel[" << m_channel << "] :: \n"
         << m_stats.toString() << "\n"
         << "Note.Count = " << m_notes.size() << "\n";

      for ( size_t i = 0; i < m_notes.size(); ++i )
      {
         s << "MidiTrackChannel[" << m_channel << "] :: Note[" << i << "] " << m_notes[i].toString() << "\n";
      }

      return s.str();
   }
};

// =======================================================================
struct MidiParam
// =======================================================================
{

};

// =======================================================================
struct MidiTrack
// =======================================================================
{
   int m_id = 0;
   int m_trackIndex = 0;
   int m_sequenceNumber = 0;
   int m_ticksPerQuarterNote = 0;
   std::string m_uri;
   std::string m_name;
   MidiTempoMap m_tempoMap;
   std::vector< MidiTrackChannel > m_channels;
   std::vector< MidiParam > m_params;

   MidiStats m_stats;

   void addNote( uint32_t ppq, int channel, int midinote, int velocity )
   {
      // New note
      if ( velocity > 0 )
      {
         m_stats.m_noteOnCount++;

         // Add new note
         MidiTrackChannel & c = getChannel( ppq, channel );
         c.m_notes.emplace_back();
         MidiNote & note = c.m_notes.back();
         note.m_midinote = midinote;
         note.m_velocity = velocity;
         note.m_ppqBeg = ppq;
         note.m_ppqEnd = ppq;
      }
      // Stop note
      else
      {
         m_stats.m_noteOffCount++;

         // Find last note and stop it.
         MidiNote* note = getLastNote( ppq, channel, midinote );
         if ( note )
         {
            note->m_ppqEnd = ppq;
         }
      }
   }


   void addPolyAftertouch( uint32_t pts, int channel, int value )
   {

   }

   void addPitchWheel( uint32_t pts, int channel, int value )
   {

   }

   void addBendWheel( uint32_t pts, int channel, int value )
   {

   }

   void addProgramChange( uint32_t pts, int channel, int program )
   {

   }


   void addControlChange( uint32_t pts, int channel, int cc, int value )
   {

   }

   void addTimeSignature( uint32_t pts, int num, int den, int cpb, int n32rd_per_qn )
   {
      m_tempoMap.addTimeSignature( pts, num, den, cpb, n32rd_per_qn );
   }

   void addTempo( uint32_t pts, float bpm, uint32_t usPerQN = 606000 )
   {
      m_tempoMap.addTempo( pts, bpm );
   }

   int
   findChannel( int channel ) const
   {
      for ( size_t i = 0; i < m_channels.size(); ++i )
      {
         if ( m_channels[ i ].m_channel == channel )
         {
            return int( i );
         }
      }
      return -1;
   }

   MidiTrackChannel &
   getChannel( uint32_t ppq, int channel )
   {
      int found = findChannel( channel );
      if ( found > -1 )
      {
         return m_channels[ found ];
      }
      m_channels.emplace_back();
      auto & item = m_channels.back();
      item.m_channel = channel;
      item.m_stats.m_ppqBeg = ppq;
      item.m_stats.m_ppqEnd = ppq;
      return item;
   }

   int
   findLastNote( uint32_t ppq, int channel, int midiNote )
   {
      MidiTrackChannel const & c = getChannel( ppq, channel );
      for ( size_t i = 0; i < c.m_notes.size(); ++i )
      {
         size_t k = c.m_notes.size() - 1 - i;
         if ( c.m_notes[ k ].m_midinote == midiNote )
         {
            return k;
         }
      }
      return -1;
   }

   MidiNote*
   getLastNote( uint32_t ppq, int channel, int midiNote )
   {
      int found = findLastNote( ppq, channel, midiNote );
      if ( found > -1 )
      {
         return &getChannel( ppq, channel ).m_notes[ found ];
      }
      return nullptr;
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "Track(" << m_trackIndex << "), "
           "#(" << m_sequenceNumber << ")"
            "ticksPerQuarterNote(" << m_ticksPerQuarterNote << "), "
            "m_uri(" << m_uri << "), "
            "m_name(" << m_name << "), "
            "m_tempoMap(" << m_tempoMap.m_tempos.size() << "), "
            "m_channels(" << m_channels.size() << ")";

      for ( size_t i = 0; i < m_channels.size(); ++i )
      {
         s << m_channels[i].toString() << "\n";
      }

      //   // Track Stats
      //   uint32_t m_nTicks = 0;
      //   uint64_t m_nEvents = 0;
      //   uint64_t m_nMetaEvents = 0;
      //   uint64_t m_nNoteOn = 0;
      //   uint64_t m_nNoteOff = 0;
      //   uint64_t m_nSetTempo = 0;
      //   uint64_t m_nProgramChanges = 0;
      //   uint64_t m_nControlChanges = 0;
      //   uint64_t m_nSustainPedalCount = 0;
      //   uint64_t m_nSoftPedalCount = 0;


      return s.str();
   }
};

// =======================================================================
struct MidiFile
// =======================================================================
{
   int m_id = 0;
   int m_fmt = 0;
   int m_trackCount = 0;
   int m_ticksPerQuarterNote = 0;
   std::string m_uri;
   std::string m_name;
   MidiTempoMap m_tempoMap;
   std::vector< MidiTrack > m_tracks;
   MidiStats m_stats;

   std::string
   toString() const
   {
      std::stringstream s;
      s << "MidiFile(" << m_uri << "), "
           "trackCount(" << m_trackCount << "), "
            "ticksPerQuarterNote(" << m_ticksPerQuarterNote << ")\n";

      for ( size_t i = 0; i < m_tracks.size(); ++i )
      {
         s << m_tracks[i].toString() << "\n";
      }

      s << "MidiFile.Stats :: \n" << m_stats.toString() << "\n";

      return s.str();
   }
};

} // end namespace midi
} // end namespace de


/*
// =======================================================================
struct MidiMessage
// =======================================================================
{
   uint32_t m_ppqBeg;
   uint32_t m_ppqLen;
   std::vector< uint8_t > m_data;

   MidiMessage()
         : m_ppqBeg( 0 )
         , m_ppqLen( 0 )
   {}

   MidiMessage( uint32_t ppqBeg, uint32_t ppqLen, uint8_t const* beg, uint8_t const* end )
         : m_ppqBeg( ppqBeg )
         , m_ppqLen( ppqLen )
   {
      m_data.assign( beg, end );
   }

   size_t
   size() const { return m_data.size(); }

   int
   getCommand() const { m_data.empty() ? 0 : m_data[ 0 ] & 0xF0; }

   int
   getChannel() const { m_data.empty() ? 0 : m_data[ 0 ] & 0x0F; }

   int
   isCC() const { return getCommand() == 0xB0; }

   int
   isNoteOn() const
   {
      return getCommand() == 0x90 && m_data.
   }
};

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
