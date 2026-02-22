#ifndef DE_AUDIO_BUFFERQUEUE_HPP
#define DE_AUDIO_BUFFERQUEUE_HPP

#include <de/audio/buffer/Buffer.hpp>
#include <vector>
#include <list>
#include <deque>
#include <mutex>

namespace de {
namespace audio {

struct BufferQueue
{
   DE_CREATE_LOGGER("de.audio.BufferQueue")
   uint64_t m_FrameCount;
   uint64_t m_FrameIndex;
   uint64_t m_ChunkIndex;
   uint64_t m_FrameOffset;
   mutable std::mutex m_Mutex;
   std::deque< Buffer::Ptr > m_Queue;
   Buffer m_WorkBuffer;

   BufferQueue();
   ~BufferQueue();

   void
   push_back( Buffer::Ptr && src, double pts );

   uint64_t
   pop_front( Buffer & dst, double pts );

   void clear();
   void dump();
   std::string toString() const;
   uint64_t getChunkIndex() const { return m_ChunkIndex; }
   uint64_t getChunkOffset() const { return m_FrameOffset; }
   uint64_t getChunkCount() const { return m_Queue.size(); }
   uint64_t getFrameIndex() const { return m_FrameIndex; }
   uint64_t getFrameCount() const { return m_FrameCount; }
   uint64_t getAvailCount() const { return m_FrameCount - (m_FrameIndex + m_FrameOffset ); }
};

} // end namespace audio
} // end namespace de

#endif










