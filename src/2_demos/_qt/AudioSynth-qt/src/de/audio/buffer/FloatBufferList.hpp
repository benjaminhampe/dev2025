#pragma once
#include <de/audio/buffer/FloatBuffer.hpp>

namespace de {
namespace audio {

// ===========================================================================
struct FloatBufferList
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.FloatBufferList")

   typedef float T; // introduce T to write less and have faster template conversion ability.
   typedef std::unique_ptr< FloatBuffer > ChunkPtr;   

   // Waveformat header ST_F32I = sampletype float 32bit interleaved.
   uint32_t m_channelCount;   // 2B, num channels [0..65535]
   uint32_t m_sampleRate;     // 4B, framerate in [Hz]
   uint64_t m_totalFrames;     // 8B, num samples per channel. Extra var for speed reasons = m_Samples.size() / m_channelCount;
   uint64_t m_totalBytes;
   double m_totalDuration;

   std::vector< ChunkPtr > m_chunks; 
   std::vector< uint64_t > m_frameStarts;
   std::vector< double > m_durationStarts;

public:
   FloatBufferList()
      : m_channelCount( 0 ), m_sampleRate( 0 ), m_totalFrames( 0 ), m_totalBytes( 0 ), m_totalDuration( 0.0 )
   {}

   FloatBufferList( uint32_t channels, uint32_t sampleRate )
      , m_channelCount( channels ), m_sampleRate( sampleRate ), m_totalFrames( 0 ), m_totalBytes( 0 ), m_totalDuration( 0.0 )
   {

   }

   void clear()
   {
      m_chunks.clear();
      m_frameStarts.clear();
      m_durationStarts.clear();
      m_totalBytes = 0;
      m_totalFrames = 0;
      m_totalDuration = 0.0;
   }
   uint32_t size() const { return m_chunks.size(); }
   uint64_t totalFrames() const { return m_totalFrames; }
   uint64_t totalBytes() const { return m_totalBytes; }
   double totalDuration() const { return m_totalDuration; }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "chunks:" << m_chunks.size();
      s << ", dur:" << dbSecondsToString( totalDuration() );
      s << ", frames:" << totalFrames();
      s << ", bytes:" << dbStrByteCount( totalBytes() );
      return s.str();
   }

   void
   append( std::unique_ptr< FloatBuffer > && chunk )
   {
      if ( !chunk ) { return; }
      if ( chunk->getFrameCount() < 1 ) { return; }

      auto cc = chunk->getChannelCount();
      auto sr = chunk->getSampleRate();
      auto fc = chunk->getFrameCount();
      auto ds = chunk->getDuration();
      auto bc = chunk->getByteCount();

      if ( m_channelCount < 1 ) m_channelCount = cc;
      if ( m_sampleRate < 1 ) m_sampleRate = sr;
      
      if ( m_channelCount != cc )
      {
         DE_ERROR("Different channelCount")
      }
      if ( m_sampleRate != sr )
      {
         DE_ERROR("Different channelCount")
      }

      m_totalFrames += fc;
      m_totalBytes += bc;
      m_totalDuration += ds;
      m_chunks.emplace_back( std::move( chunk ) );

      // Update frame start toc ( Table Of Contents )
      if ( m_frameStarts.empty() )
      {
         m_frameStarts.push_back( 0 );
      }
      else
      {
         m_frameStarts.push_back( m_frameStarts.back() + fc );
      }

      // Build duration start toc ( Table Of Contents )
      if ( m_durationStarts.empty() )
      {
         m_durationStarts.push_back( 0.0 );
      }
      else
      {
         m_durationStarts.push_back( m_durationStarts.back() + ds );
      }
   }

   // All conversion depends on first buffer format pushed to list.
   bool
   join( FloatBuffer & dst ) const
   {
      if ( m_chunks.size() < 1 ) { return false; }
      if ( m_chunks.size() < 2 ) { dst = m_chunks.front(); return true; } // Copy first and only element
      auto dstChannels = m_chunks.front().getChannelCount();
      auto dstRate = m_chunks.front().getSampleRate();

      try
      {
         dst.clear();
         dst.shrink_to_fit();
         dst.setChannelCount( dstChannels );
         dst.setSampleRate( dstRate );
         dst.resize( m_totalFrames );
      }
      catch( ... )
      {
         DE_ERROR("OOM for ST_F32I, frames(", m_totalFrames, "), dstChannels(", dstChannels, "), dstRate(", dstRate, ")")
         return false;
      }

      auto pDst = dst.data();
      for ( size_t i = 0; i < m_chunks.size(); ++i )
      {
         auto const & chunk = m_chunks[ i ];
         if ( !chunk ) continue;
         auto writtenSamples = chunk->write( 0, pDst, chunk->getFrameCount(), dstChannels, false );
         pDst += writtenSamples;
      }

      DE_DEBUG("frameIndex(", frameIndex, "), JOINED ", dst.toString() )
      return true;
   }

};

} // end namespace audio
} // end namespace de

*/

