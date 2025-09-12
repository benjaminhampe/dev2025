#include <de/audio/music/MusicFileStream.hpp>

namespace de {
namespace audio {

MusicFileStream::MusicFileStream()
   : m_Uri("")
   , m_ThreadPtr( nullptr )
   , m_IsRunning( false )
   , m_DidRun( false )
   , m_StreamIndex( -1 )
{
   DE_DEBUG("Constructor")
}

MusicFileStream::~MusicFileStream()
{
   DE_DEBUG("Destructor")
   close();
}

uint64_t
MusicFileStream::getFrameCount() const { return m_BufferQueue.getFrameCount(); }
uint64_t
MusicFileStream::getFrameIndex() const { return m_BufferQueue.getFrameIndex(); }
bool
MusicFileStream::isPlaying() const { return getFrameCount() > getFrameIndex() + 1023; }
bool
MusicFileStream::isRunning() const { return m_IsRunning; }

void
MusicFileStream::close()
{
   m_IsRunning = false;
   DE_DEBUG("Close")

   if ( m_ThreadPtr )
   {
      m_ThreadPtr->join();
      delete m_ThreadPtr;
      m_ThreadPtr = nullptr;
   }

   m_File.reset();
}

bool
MusicFileStream::open( std::string uri, int streamIndex )
{
   close();
   m_Uri = uri;
   m_StreamIndex = streamIndex;
   m_ThreadPtr = new std::thread( &MusicFileStream::run, this );

   std::mutex m;
   std::unique_lock< std::mutex > guard( m );
   m_RunFence.wait( guard, [&] () { return m_DidRun; } );

   if ( m_IsRunning )
   {
      DE_DEBUG("Loader running ...")
   }
   else
   {
      DE_ERROR("Loader finished")
   }

   return m_IsRunning;
}

void
MusicFileStream::run()
{
   m_DidRun = true;
   m_File.reset( new decoder::DecoderFFMPEG() );
   m_IsRunning = m_File->open( m_Uri, -1, true );

   if ( !m_IsRunning )
   {
      DE_ERROR("Cant open uri ", m_Uri )
      m_RunFence.notify_all();      // Abort for videos only
      return;
   }

/*
   if ( !m_File->isStream( m_StreamIndex ) )
   {
      m_StreamIndex = m_File->getBestAudioStreamIndex();
   }

   if ( !m_File->isStream( m_StreamIndex ) )
   {
      DE_ERROR("No audio Stream in ", m_Uri )
      m_IsRunning = false;
      m_RunFence.notify_all();      // Abort for videos only
      return;
   }

   m_File->disableStreams();
   m_File->enableStream( m_StreamIndex );

   size_t packedCount = 0;
   size_t frameCount = 0;

   auto appendToBufferList =
      [&](std::unique_ptr< ffmpeg::FilePayload > && payload )
      {
         if ( payload->streamIndex == m_StreamIndex &&
              payload->typ == AVMEDIA_TYPE_AUDIO &&
              payload->buf )
         {
            //DE_ERROR("Packet[",packedCount,"] FrameCount = ", frameCount)
            frameCount += payload->buf->getFrameCount();
            //m_BufferList.append( std::move( payload->buf ) );
            //m_Duration += payload->buf->getDuration();
            //m_AudioQueue.push_back( payload->buf, payload->getPTS() );
            m_BufferQueue.push_back(
                        reinterpret_cast< float const* >( payload->buf->data() ),
                        payload->getPTS(),
                        payload->buf->getFrameCount(),
                        payload->buf->getChannelCount(),
                        uint32_t( payload->buf->getSampleRate() ) );
         }

//            if ( payload->streamIndex == m_VideoStreamIndex &&
//                 payload->typ == AVMEDIA_TYPE_VIDEO &&
//                 payload->img )
//            {
//               DE_ERROR("Got frame")
//            }

         packedCount++;
      };


   DE_DEBUG("Opened uri ", m_Uri )
   DE_DEBUG("Opened audio stream ", m_StreamIndex )
   DE_DEBUG("Start caching ...")

   while ( m_IsRunning )
   {
      if ( m_BufferQueue.getFrameCount() - m_BufferQueue.getFrameIndex() >= 32*1024 )
      {
         break;
      }

      int e = m_File->readFrame( appendToBufferList );
      if ( e == AVERROR_EOF )
      {
         break;
      }
   }

   DE_DEBUG("Caching done.")
   m_RunFence.notify_all();

   DE_DEBUG("Enter loop.")
   while ( m_IsRunning )
   {
      if ( m_BufferQueue.getFrameCount() - m_BufferQueue.getFrameIndex() < 32*1024 )
      {
         int e = m_File->readFrame( appendToBufferList );
         if ( e == AVERROR_EOF )
         {
            break;
         }
      }
      else
      {
         std::this_thread::sleep_for( std::chrono::milliseconds(300) );
      }

   }
*/

   DE_DEBUG("[END] Audio Loading ...")
   m_IsRunning = false;
}

} // end namespace audio
} // end namespace de

