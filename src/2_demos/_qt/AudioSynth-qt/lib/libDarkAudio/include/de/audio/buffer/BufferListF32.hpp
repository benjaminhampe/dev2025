#pragma once
#include <de/audio/buffer/Buffer.hpp>

/*
namespace de {
namespace audio {

// ===========================================================================
struct BufferListF32
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.BufferList")
   std::vector< Buffer > m_Chunks;
   uint64_t m_TotalBytes;
   uint64_t m_TotalFrames;
   double m_TotalDuration;

   std::vector< uint64_t > m_FrameStarts;
   std::vector< double > m_DurationStarts;

   BufferList()
      : m_TotalBytes( 0 )
      , m_TotalFrames( 0 )
      , m_TotalDuration( 0.0 )
   {
      DE_DEBUG("Constructor")
   }

   ~BufferList()
   {
      DE_DEBUG("Destructor")
      clear();
   }

   void clear()
   {
      m_Chunks.clear();
      m_FrameStarts.clear();
      m_DurationStarts.clear();
      m_TotalBytes = 0;
      m_TotalFrames = 0;
      m_TotalDuration = 0.0;
   }
   uint32_t size() const { return m_Chunks.size(); }
   uint64_t totalFrames() const { return m_TotalFrames; }
   uint64_t totalBytes() const { return m_TotalBytes; }
   double totalDuration() const { return m_TotalDuration; }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "chunks:" << m_Chunks.size();
      s << ", dur:" << dbSecondsToString( totalDuration() );
      s << ", frames:" << totalFrames();
      s << ", bytes:" << dbStrByteCount( totalBytes() );
      return s.str();
   }

   void
   append( Buffer const & chunk )
   {
      if ( chunk.getFrameCount() < 1 )
      {
         //DE_ERROR("Does not add empty")
         return;
      }

      //DE_DEBUG("Append chunk [",m_Chunks.size(),"] ", chunk.toString())
      auto duration = chunk.getDuration();
      auto bytes = chunk.getByteCount();
      auto frames = chunk.getFrameCount();
      m_TotalFrames += frames;
      m_TotalDuration += duration;
      m_TotalBytes += bytes;
      m_Chunks.push_back( chunk );

      // Build frame index
      if ( m_FrameStarts.empty() )
      {
         m_FrameStarts.push_back( 0 );
      }
      else
      {
         m_FrameStarts.push_back( m_FrameStarts.back() + frames );
      }

      // Build duration index
      if ( m_DurationStarts.empty() )
      {
         m_DurationStarts.push_back( 0.0 );
      }
      else
      {
         m_DurationStarts.push_back( m_DurationStarts.back() + duration );
      }
   }

   // All conversion depends on first buffer format pushed to list.
   bool
   join( Buffer & dst ) const
   {
      if ( m_Chunks.size() < 1 )
      {
         // dst.reset(); // Make invalid
         return false; // Indicate nothing done.
      }

      if ( m_Chunks.size() < 2 )
      {
         dst = m_Chunks.back(); // Copy last element ( = first and only element )
         return true;
      }

      ESampleType dstType = m_Chunks.front().getSampleType();
      int dstChannels = m_Chunks.front().getChannelCount();
      float dstRate = m_Chunks.front().getSampleRate();

      DE_DEBUG("dstType(", ST_toString(dstType), "), "
               "dstChannels(", dstChannels, "), "
               "dstRate(", dstRate, ")")
      try
      {
         dst.clear();
         dst.setFormat( dstType, dstChannels, dstRate );
         dst.resize( m_TotalFrames );
      }
      catch( ... )
      {
         DE_ERROR("OOM for frames(", m_TotalFrames, "), "
                        "dstType(", ST_toString(dstType), "), "
                        "dstChannels(", dstChannels, "), "
                        "dstRate(", dstRate, ")")
         return false;
      }

      size_t frameIndex = 0;

      for ( size_t i = 0; i < m_Chunks.size(); ++i )
      {
         frameIndex += dst.write( frameIndex, m_Chunks[ i ] );
      }

      DE_DEBUG("frameIndex(", frameIndex, "), JOINED ", dst.toString() )
      return true;
   }

};

} // end namespace audio
} // end namespace de

*/
