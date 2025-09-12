#pragma once
#include <de/midi/file/Channel.hpp>
#include <de/midi/file/SetTempoEvent.hpp>
#include <de/midi/file/TimeSignatureEvent.hpp>
#include <de/midi/file/KeySignatureEvent.hpp>
#include <de/midi/file/MetaEvent.hpp>

namespace de {
namespace midi {
namespace file {

// Midi can have u16 (65536) tracks with each 16 channels. Channel 9 is reserved for drums.
// That should be enough for any composer.
// =======================================================================
struct Track
// =======================================================================
{
   bool m_enabled;      // Controlled by player, not file.
   int m_trackIndex;
   std::string m_trackName;  // trackName given by meta events
   std::vector< Channel > m_channels;
   std::vector< SetTempoEvent > m_setTempoEvents;   // for debug, @see TempoMap integrated in SM_File.
   std::vector< TimeSignatureEvent > m_timeSignatureEvents;
   std::vector< KeySignatureEvent > m_keySignatureEvents;
   ControlChangeEventMap m_polyphonicAftertouch;
   std::vector< MetaEvent > m_textEvents;
   std::vector< MetaEvent > m_lyricEvents;

   explicit Track( int trackIndex = -1 )
      : m_enabled(true)
      , m_trackIndex(trackIndex)
   {}

   std::string const & name() const { return m_trackName; }

   size_t channelCount() const { return m_channels.size(); }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_trackName.capacity() * sizeof( char );
      for ( auto const & c : m_channels ) { n += c.computeMemoryConsumption(); }
      n += (m_channels.capacity() - m_channels.size()) * sizeof( Channel );
      n += m_setTempoEvents.capacity() * sizeof( SetTempoEvent );
      n += m_timeSignatureEvents.capacity() * sizeof( TimeSignatureEvent );
      n += m_keySignatureEvents.capacity() * sizeof( KeySignatureEvent );
      n += m_polyphonicAftertouch.computeMemoryConsumption();
      for ( auto const & e : m_textEvents ) { n += e.computeMemoryConsumption(); }
      for ( auto const & e : m_lyricEvents ){ n += e.computeMemoryConsumption(); }
      return n;
   }

   Channel &
   getChannel( int channel ) // We guarantee the channel exist or exit program immediatly.
   {
      if ( channel < 0 || channel > 15 )
      {
         throw std::runtime_error("Invalid channel");
      }

      auto it = std::find_if( m_channels.begin(), m_channels.end(),
         [&]( Channel const & cached ) { return cached.m_channelIndex == channel; });

      if ( it == m_channels.end() )
      {
         m_channels.emplace_back( channel );
         return m_channels.back();
      }
      else
      {
         return (*it);
      }
   }

   std::string
   toString( bool withNotes ) const
   {
      std::ostringstream o;
      //if ( !m_touched ) o << "UNTOUCHED ";
      o << "Track["<<m_trackIndex<<"] (" << m_trackName << ") "
            "channels(" << m_channels.size() << ")\n";
      if (withNotes)
      {
         for ( size_t i = 0; i < m_channels.size(); ++i )
         {
            o << "Track["<<m_trackIndex<<"].Channel["<<i<<"] " << m_channels[i].toString() << "\n";
         }
      }
      return o.str();
   }

};

} // end namespace file
} // end namespace midi
} // end namespace de
