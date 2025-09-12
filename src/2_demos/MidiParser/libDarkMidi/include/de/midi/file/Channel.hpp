#pragma once
#include <de/midi/GeneralMidi.hpp>
#include <de/midi/file/ControlChangeEventMap.hpp>
#include <de/midi/file/NoteEvent.hpp>
#include <de/midi/file/NoteOnEvent.hpp>
#include <de/midi/file/NoteOffEvent.hpp>
#include <algorithm>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct Channel
// =======================================================================
{
   int m_channelIndex;  // 0...15, 9 = always drums
   int m_instrument;    // GM instrument or drum number
   std::vector< NoteEvent > m_notes;
   std::vector< NoteOnEvent > m_noteOnEvents;
   std::vector< NoteOffEvent > m_noteOffEvents;
   std::vector< ControlChangeEventMap > m_controller;
   ControlChangeEventMap m_channelAftertouch;
   ControlChangeEventMap m_pitchBend;

   explicit Channel( int channelIndex = -1 )
      : m_channelIndex(channelIndex)
      , m_instrument(-1)
   {}

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_notes.capacity() * sizeof( NoteEvent );
      n += m_noteOnEvents.capacity() * sizeof( NoteOnEvent );
      n += m_noteOffEvents.capacity() * sizeof( NoteOffEvent );
      for ( size_t i = 0; i < m_controller.size(); ++i )
      {
         n += m_controller[ i ].computeMemoryConsumption();
      }
      n += (m_controller.capacity() - m_controller.size()) * sizeof( ControlChangeEvent );
      n += m_channelAftertouch.computeMemoryConsumption();
      n += m_pitchBend.computeMemoryConsumption();
      return n;
   }

   NoteEvent*
   addNote( NoteEvent const & note )
   {
      m_notes.emplace_back( note );
      return &m_notes.back();
   }

   NoteEvent*
   addNote( int channel, int midiNote,
            int attack, uint32_t attackMs,
            int release, uint32_t releaseMs)
   {
      NoteEvent note;
      note.m_channel = channel;
      note.m_midiNote = midiNote;
      note.m_attack = attack;
      note.m_attackMs = attackMs;
      note.m_release = release;
      note.m_releaseMs = releaseMs;
      return addNote( note );
   }

   NoteEvent*
   getLastNote( int midiNote )
   {
      auto it = std::find_if( m_notes.rbegin(), m_notes.rend(),
         [&]( NoteEvent const & cached ) { return cached.m_midiNote == midiNote; });
      if ( it == m_notes.rend() )
      {
         //DE_ERROR("No midi note (",midiNote,") found in track(", m_trackIndex,")")
         return nullptr;
      }

      //size_t index = std::distance( m_noten.rend(), it );
      return &(*it);
   }

   ControlChangeEventMap &
   getController( int cc ) // We guarantee that the controller exists.
   {
      auto it = std::find_if( m_controller.begin(), m_controller.end(),
         [&]( ControlChangeEventMap const & cached ) { return cached.m_cc == cc; });
      if ( it == m_controller.end() )
      {
         m_controller.emplace_back( cc ); // Value ctr
         return m_controller.back();
      }
      else
      {
         return (*it);
      }
   }

   std::string
   toString( bool withNotes = false ) const
   {
      std::ostringstream o;
      //if ( !m_touched ) o << "UNTOUCHED ";
      o << "Channel["<<m_channelIndex<<"]"
            " instrument(" << GM_toString( m_instrument ) << ")"
            ", controller(" << m_controller.size() << ")"
            //", lyrics(" << m_lyrics << ")"
            ", notes(" << m_notes.size() << ")"
            ", notesOn(" << m_noteOnEvents.size() << ")"
            ", notesOff(" << m_noteOffEvents.size() << ")"
            "\n";
      if (withNotes)
      {
         for ( size_t i = 0; i < m_notes.size(); ++i )
         {
            o <<"Channel["<<m_channelIndex<<"].Note["<<i<<"] " << m_notes[i].toString() << "\n";
         }
      }
      return o.str();
   }

   Range<int> getNoteRange() const
   {
      Range<int> range;
      for ( size_t i = 0; i < m_notes.size(); ++i )
      {
         auto const & note = m_notes[ i ];
         int midiNote = note.m_midiNote;
         range.min = std::min( midiNote, range.min );
         range.max = std::max( midiNote, range.max );
      }
      return range;
   }

   Range<uint64_t> getTickRange() const
   {
      Range<uint64_t> range;
      for ( size_t i = 0; i < m_notes.size(); ++i )
      {
         auto const & note = m_notes[ i ];
         uint64_t tickStart = note.m_attackMs;
         uint64_t tickEnd = note.m_releaseMs;
         range.min = std::min( tickStart, range.min );
         range.max = std::max( tickStart, range.max );
         range.min = std::min( tickEnd, range.min );
         range.max = std::max( tickEnd, range.max );
      }
      return range;
   }

};


} // end namespace file
} // end namespace midi
} // end namespace de
