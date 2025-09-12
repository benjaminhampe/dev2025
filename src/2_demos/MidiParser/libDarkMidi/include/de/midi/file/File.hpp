#pragma once
#include <de/midi/file/Track.hpp>
#include <de/midi/file/TempoMap.hpp>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct File
// =======================================================================
{
   std::string m_fileName;
   int m_fileType;
   int m_trackCount;
   int m_ticksPerQuarterNote;
   float m_speed;
   TempoMap m_tempoMap;
   std::vector< Track > m_tracks;  // Up to 65536 tracks possible?!

   File()
   {
      reset();
   }

   double getDurationInSec() const
   {
      return m_tempoMap.getDurationInSec();
   }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      n += m_fileName.capacity() * sizeof( char );
      n += m_tempoMap.computeMemoryConsumption();
      for ( size_t i = 0; i < m_tracks.size(); ++i )
      {
         n += m_tracks[ i ].computeMemoryConsumption();
      }
      n += (m_tracks.capacity() - m_tracks.size())* sizeof( Track );

      return n;
   }

   void
   reset()
   {
      m_fileName = "";
      m_fileType = -1;
      m_trackCount = -1;
      m_ticksPerQuarterNote = -1;
      m_speed = 1.0f;
      m_tempoMap.reset();
      m_tracks.clear();
   }

   void finalize()
   {
      m_tempoMap.finalizeTempoMap();

      // finalizeNotes();
   }

   uint64_t getTickFromNanosecond( int64_t nanoseconds ) const
   {
      uint64_t tick = 0;

      return tick;
   }

   Track*
   addTrack( Track const & track )
   {
      m_tracks.emplace_back( track );
      return &m_tracks.back();
   }

   Track &
   getTrack( int trackIndex ) // We guarantee the track exist -> returns reference, not pointer.
   {
      auto it = std::find_if( m_tracks.begin(), m_tracks.end(),
                        [&]( Track const & cached )
                        {
                           return cached.m_trackIndex == trackIndex;
                        });

      if ( it == m_tracks.end() )
      {
         m_tracks.emplace_back( trackIndex ); // Value ctr
         return m_tracks.back();
      }
      else
      {
         return (*it);
      }
   }

   std::string
   toString() const
   {
      std::ostringstream o;
      o << "FileName = "<<m_fileName<<"\n";
      o << "FileType = "<<m_fileType<<"\n";
      o << "TicksPerQuarterNote = "<<m_ticksPerQuarterNote<<"\n";
      o << "TrackCount = "<<m_tracks.size()<<" ("<<m_trackCount<<")\n";
      for ( auto const & track : m_tracks )
      {
         o << track.toString( true );
      }

      o << "TempoMap = " << m_tempoMap.toString() << "\n";

      o << "MemoryConsumption = " << StringUtil::byteCount( computeMemoryConsumption() ) << "\n";
      return o.str();
   }

};

} // end namespace file
} // end namespace midi
} // end namespace de
