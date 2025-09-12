#pragma once
#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {

// ===========================================================================
struct BufferList
// ===========================================================================
{
   typedef std::unique_ptr< BufferList > UniquePtr;
   typedef std::shared_ptr< BufferList > SharedPtr;

   DE_CREATE_LOGGER("de.audio.BufferList")
   std::vector< Buffer > m_Chunks;
   std::vector< uint64_t > m_FrameStarts;
   std::vector< double > m_DurationStarts;
   uint64_t m_TotalBytes;
   uint64_t m_TotalFrames;
   double m_TotalDuration;

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
      s << ", dur:" << dbStrSeconds( totalDuration() );
      s << ", frames:" << totalFrames();
      s << ", bytes:" << dbStrBytes( totalBytes() );
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


// ===========================================================================
struct UniqueBufferList
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.UniqueBufferList")

   std::vector< Buffer::UniquePtr > m_Chunks;
   uint64_t m_TotalBytes;
   uint64_t m_TotalFrames;
   double m_TotalDuration;

   uint64_t m_FrameIndex;

   std::vector< uint64_t > m_FrameStarts;
   std::vector< double > m_DurationStarts;

   UniqueBufferList()
      : m_TotalBytes( 0 )
      , m_TotalFrames( 0 )
      , m_TotalDuration( 0.0 )
      , m_FrameIndex( 0 )
   {
      DE_DEBUG("Constructor")
   }

   ~UniqueBufferList()
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
      s << ", dur:" << dbStrSeconds( totalDuration() );
      s << ", frames:" << totalFrames();
      s << ", bytes:" << dbStrBytes( totalBytes() );
      return s.str();
   }

   uint64_t
   read( Buffer & dst )
   {
      uint64_t dstFrames = dst.getFrameCount();

//      DE_DEBUG("m_FrameIndex = ", m_FrameIndex )
//      DE_DEBUG("m_TotalFrames = ", m_TotalFrames )
//      DE_DEBUG("dstFrames = ", dstFrames )

      if ( m_FrameIndex + dstFrames > m_TotalFrames )
      {
         DE_ERROR("Data too low")
         DE_ERROR("m_FrameIndex = ", m_FrameIndex)
         DE_DEBUG("m_TotalFrames = ", m_TotalFrames )
         DE_DEBUG("dstFrames = ", dstFrames )
         //dst.fill_zero();
         return 0;
      }

      uint64_t frameCounter = 0;

      // find start chunk
      int32_t chunkIndex = -1;
      uint64_t frameIndex = 0;

      for ( size_t i = 0; i < m_Chunks.size(); ++i )
      {
         Buffer const * const chunkPtr = m_Chunks[ i ].get();
         if ( !chunkPtr ) continue;
         Buffer const & chunk = *chunkPtr;

         uint64_t chunkFrames = chunk.getFrameCount();

         if ( m_FrameIndex >= frameCounter &&
              m_FrameIndex < frameCounter + chunkFrames )
         {
            chunkIndex = int32_t( i );
            frameIndex = m_FrameIndex - frameCounter;
            break;
         }

         frameCounter += chunkFrames;
      }

      if ( chunkIndex < 0 )
      {
         DE_ERROR("No chunk found")
         DE_ERROR("m_FrameIndex = ", m_FrameIndex)
         DE_DEBUG("m_TotalFrames = ", m_TotalFrames )
         DE_DEBUG("dstFrames = ", dstFrames )
         //dst.fill_zero();
         return 0;
      }
      uint64_t writeCounter = 0;
      uint64_t writeIndex = 0;

      while ( dstFrames > 0 )
      {
         Buffer const * const chunkPtr = m_Chunks[ chunkIndex ].get();
         if ( !chunkPtr ) continue;
         Buffer const & chunk = *chunkPtr;
         uint64_t availFrames = chunk.getFrameCount() - frameIndex;
         if ( availFrames > dstFrames )
         {
            availFrames = dstFrames;
         }
         uint64_t writeCount = dst.write( writeIndex, chunk, frameIndex, availFrames );
         frameIndex = 0;
         dstFrames -= writeCount;
         chunkIndex++;
         writeIndex += writeCount;
         writeCounter += writeCount;
      }

      m_FrameIndex += writeCounter;

      //DE_DEBUG("Read ",dst.getFrameCount()," frames, got ", writeCounter)
      return writeCounter;
   }

   void
   append( Buffer::UniquePtr && chunk )
   {
      if ( !chunk || chunk->getFrameCount() < 1 )
      {
         DE_ERROR("Does not add empty")
         return;
      }

      //DE_DEBUG("Append chunk [",m_Chunks.size(),"] ", chunk->toString())
      auto duration = chunk->getDuration();
      auto bytes = chunk->getByteCount();
      auto frames = chunk->getFrameCount();
      m_TotalFrames += frames;
      m_TotalDuration += duration;
      m_TotalBytes += bytes;
      m_Chunks.emplace_back( std::move( chunk ) );

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
         dst = *m_Chunks.back(); // Copy last element ( = first and only element )
         return true;
      }

      ESampleType dstType = m_Chunks.front()->getSampleType();
      int dstChannels = m_Chunks.front()->getChannelCount();
      float dstRate = m_Chunks.front()->getSampleRate();

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
         if ( !m_Chunks[ i ] ) continue;
         frameIndex += dst.write( frameIndex, *m_Chunks[ i ] );
      }

      DE_DEBUG("frameIndex(", frameIndex, "), JOINED ", dst.toString() )
      return true;
   }

};

} // end namespace audio
} // end namespace de

