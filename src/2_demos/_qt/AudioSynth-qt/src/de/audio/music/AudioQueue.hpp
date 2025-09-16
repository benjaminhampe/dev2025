#ifndef DE_AUDIO_QUEUE_HPP
#define DE_AUDIO_QUEUE_HPP

#include <de/Core.h>
#include <vector>
#include <list>
#include <deque>
#include <mutex>
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

struct AudioQueue
{
   typedef std::vector< float > Buffer;
   typedef std::unique_ptr< Buffer > BufferPtr;
   typedef std::deque< BufferPtr > BufferQueue;
   DE_CREATE_LOGGER("de.audio.AudioQueue")
   uint32_t m_channels;
   uint32_t m_sampleRate;
   uint64_t m_SampleCount;
   uint64_t m_SampleIndex;
   uint64_t m_ChunkIndex;
   uint64_t m_SampleOffset;
   mutable std::mutex m_Mutex;
   BufferQueue m_Queue;
   Buffer m_WorkBuffer;

   AudioQueue()
      : m_channels( 0 )
      , m_sampleRate( 0 )
      , m_SampleCount( 0 )
      , m_SampleIndex( 0 )
      , m_ChunkIndex( 0 )
      , m_SampleOffset( 0 )
   {
      DE_DEBUG("Constructor")
   }

   //~AudioQueue() { clear(); }
   //void dump();
   //std::string toString() const;
   // uint64_t getChunkIndex() const { return m_ChunkIndex; }
   // uint64_t getChunkOffset() const { return m_SampleOffset; }
   // uint64_t getChunkCount() const { return m_Queue.size(); }
   uint64_t getFrameIndex() const { return m_SampleIndex; }
   uint64_t getFrameCount() const { return m_SampleCount; }
   // uint64_t getAvailCount() const { return m_SampleCount - (m_SampleIndex + m_SampleOffset ); }

   void clear()
   {
      std::lock_guard< std::mutex > guard( m_Mutex );
      m_Queue.clear();
      m_SampleIndex = 0;
      m_SampleCount = 0;
      m_ChunkIndex = 0;
      m_SampleOffset = 0;
   }

   void
   push_back( float const* src, double pts, uint64_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
   {
      if ( !src ) { DE_ERROR("No chunk") return; }

      size_t dstSamples = dstFrames * dstChannels;    // Count
      if ( dstSamples < 1 ) { DE_ERROR("No dstSamples") return; }

      std::lock_guard< std::mutex > guard( m_Mutex ); // Lock
      m_channels = dstChannels;                       // Update channels
      m_sampleRate = dstRate;                         // Update rate
      m_SampleCount += dstSamples;                    // Update samples
      BufferPtr chunk = std::unique_ptr< Buffer >( new Buffer() );
      chunk->resize( dstSamples );
      DSP_COPY( src, chunk->data(), dstSamples );      // Copy source into queue buffer
      m_Queue.emplace_back( std::move( chunk ) );     // Copy source into queue buffer
   }

   uint64_t
   pop_front( float* dst, double pts, uint64_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
   {
      size_t dstSamples = dstFrames * dstChannels;    // Count
      if ( dstSamples < 1 ) { DE_ERROR("No dstSamples") return dstSamples; }
      DSP_FILLZERO( dst, dstSamples );                  // Setup

      std::lock_guard< std::mutex > guard( m_Mutex ); // Lock

      uint64_t desiredSamples = dstSamples;           // CountDown ( makes life easier ).
      if ( m_SampleIndex + dstSamples > m_SampleCount )// Rescue
      {
         DE_ERROR("m_SampleIndex + dstSamples > m_SampleCount")
         desiredSamples = m_SampleCount - m_SampleIndex; // Count again
      }

      if ( desiredSamples < 1 )                       // Abort
      {
         DE_ERROR("No desiredSamples")
         return 0;
      }

      uint64_t totalSamples = 0;                      // Process Counter
      while ( desiredSamples > 0 )                    // Process Condition
      {
         if ( m_Queue.empty() ) { DE_ERROR("Queue is empty") break; }

         BufferPtr & chunk = m_Queue.front();         // Pop
         // Chunk is bad, drop it.
         if ( !chunk )
         {
            DE_ERROR("No chunk?")
            m_Queue.pop_front();
            continue;
         }

         // How many samples can i get from this chunk still?
         uint64_t availSamples = chunk->size() - m_SampleOffset;

         // Chunk is fully consumed, drop it.
         if ( availSamples < 1 )
         {
            DE_ERROR("No availSamples in chunk")
            m_SampleIndex += chunk->size();
            m_SampleOffset = 0;
            m_ChunkIndex++;
            m_Queue.pop_front();
            continue;
         }

         // Not the last chunk...
         if ( desiredSamples >= availSamples )
         {
            // Consume full chunk or rest of it...
            float* pSrc = chunk->data() + m_SampleOffset;
            float* pDst = dst + totalSamples;
            memcpy( pDst, pSrc, availSamples );
            desiredSamples -= availSamples;
            totalSamples += availSamples;
            m_SampleIndex += chunk->size();
            m_SampleOffset = 0;
            m_ChunkIndex++;
            m_Queue.pop_front();
         }
         // Last chunk, only need part of it...
         else
         {
            float* pSrc = chunk->data();
            float* pDst = dst + totalSamples;
            memcpy( pDst, pSrc, desiredSamples );
            desiredSamples = 0;
            totalSamples += desiredSamples;
            m_SampleOffset += desiredSamples;
         }
      }

      return totalSamples;
   }




};

} // end namespace audio
} // end namespace de

#endif










