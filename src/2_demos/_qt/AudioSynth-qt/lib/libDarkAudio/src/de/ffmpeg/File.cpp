#include <de/ffmpeg/File.hpp>

namespace de {
namespace ffmpeg {

// =====================================================================================
// class FileStream
// =====================================================================================

std::string
FileStream::getDetailName() const
{
   if ( !m_avCodec ) { return "Unknown"; }
   std::stringstream s;

   if ( getMediaType() == AVMEDIA_TYPE_AUDIO )
   {
      s << "ch:" << m_avCodecParams->channels << ", ";
      s << "fmt:" << Utils::toString( m_avCodecCtx->sample_fmt ) << ", ";
   }
   else if ( getMediaType() == AVMEDIA_TYPE_VIDEO )
   {
      s << "w:" << m_avCodecParams->width << ", ";
      s << "h:" << m_avCodecParams->height << ", ";
   }

   s << m_avCodec->long_name << ", ";
   s << "CodecId(" << int( m_avCodecParams->codec_id ) << ")";
   return s.str();
}

// Owned and used only by the File, so its actually a FileStream
FileStream::FileStream( AVFormatContext* avFormatCtx, int avStreamIndex )
   : m_avFormatCtx( avFormatCtx )
   , m_avStream( nullptr )
   , m_avCodecParams( nullptr )
   , m_avCodec( nullptr )
   , m_avCodecCtx( nullptr )
   , m_swResampler( nullptr )
   , m_swScaler( nullptr )
   , m_IsEnabled( false )
   , m_IsSeekable( false )
   , m_avStreamIndex( avStreamIndex )
   , m_Position( 0.0 )
   , m_Duration( 0.0 )
{
   if ( m_avFormatCtx )
   {
      m_avStream = m_avFormatCtx->streams[ m_avStreamIndex ];
      if ( m_avStream )
      {
         if ( m_avStream->duration == AV_NOPTS_VALUE )
         {
            m_avTimeBase.num = 0;
            m_avTimeBase.den = 1;
            m_avDuration = 0;
            m_IsSeekable = false;
            m_Duration = 0.0;
         }
         else
         {
            m_avTimeBase = m_avStream->time_base;
            m_avDuration = m_avStream->duration;
            m_IsSeekable = true;
            m_Duration = Utils::toDouble( m_avTimeBase ) * m_Duration;
         }

         m_avCodecParams = m_avStream->codecpar;
         m_avCodecId = m_avCodecParams->codec_id;
         m_avCodec = avcodec_find_decoder( m_avCodecParams->codec_id );
         if ( m_avCodec )
         {
            m_avCodecCtx = avcodec_alloc_context3( m_avCodec );
            if ( m_avCodecCtx )
            {
               int e = avcodec_parameters_to_context( m_avCodecCtx, m_avCodecParams );
               if (e < 0)
               {
                  DE_ERROR("No avcodec_parameters_to_context, e = ", e)
               }

               // int e = avcodec_parameters_to_context( m_avCodecCtx, m_avCodecParams );
               // Explicitly request interleaved ( non planar ) data.
               if ( m_avCodecCtx->codec_type == AVMEDIA_TYPE_AUDIO )
               {
                  AVSampleFormat fmt = m_avCodecCtx->sample_fmt;
                  m_avCodecCtx->request_sample_fmt = av_get_alt_sample_fmt( fmt, 0 ); // 0 = interleaved, 1 = planar
               }

               e = avcodec_open2( m_avCodecCtx, m_avCodec, nullptr );
               if (e >= 0)
               {

                  m_Duration = Utils::getDuration( m_avFormatCtx, m_avStreamIndex );

               }
               else { DE_ERROR("No avcodec_open2()") }
            }
            else { DE_ERROR("No avcodec_alloc_context3()") }
         }
         else { DE_ERROR("No avcodec_find_decoder(), streamIndex(",avStreamIndex,")") }
      }
      else { DE_ERROR("No m_avStream") }
   }
   else { DE_ERROR("No m_avFormatCtx") }
}

FileStream::~FileStream()
{
   //drainDecoder(m_avCodecCtx, m_avFrame);
   avcodec_close( m_avCodecCtx );
   avcodec_free_context( &m_avCodecCtx );
   swr_free( &m_swResampler );
   sws_freeContext( m_swScaler );
}


bool
FileStream::isSeekable() const { return m_IsSeekable; }

bool
FileStream::seek( double pts )
{
   if ( m_IsEnabled )
   {
      m_Position = pts;
      return Utils::seek( m_avFormatCtx, m_avStreamIndex, pts );
   }
   else
   {
      return false;
   }
}

double
FileStream::tell() const
{
   return m_Position;
}

//bool
//FileStream::hasDuration() const { return m_HasDuration; }
//double
//FileStream::getDuration() const { return m_Duration; }
//double
//FileStream::getPosition() const { return m_Position; }

//bool
//FileStream::setPosition( double pts )
//{
//   return seek( pts );
//}

// =====================================================================================
// class File
// =====================================================================================

File::File()
   : m_IsOpen( false )
   , m_IsSeekable( false )
   , m_Position( 0.0 )
   , m_Duration( 0.0 )
   , m_avFormatCtx( nullptr )
   , m_avPacket( nullptr )
   , m_avFrame( nullptr )
   , m_AudioOutputType( audio::ST_F32I ) //
//   , m_VideoPTS( 0.0 )
//   , m_AudioPTS( 0.0 )
{}

File::~File()
{
   close();
}

bool
File::isSeekable() const
{
   DE_DEBUG("m_IsSeekable = ",m_IsSeekable)
   return m_IsSeekable;
}

void
File::seek( double pts )
{
   DE_DEBUG("seek = ",pts)
   DE_DEBUG("m_IsSeekable = ",m_IsSeekable)

   for ( size_t i = 0; i < m_Streams.size(); ++i )
   {
      auto stream = m_Streams[ i ];
      if ( stream )
      {
         stream->seek( pts );
      }
   }
   m_Position = pts;
}

double
File::tell() const
{
   return m_Position;
}

bool
File::hasDuration() const
{
   return m_IsSeekable;
}

double
File::getDuration() const
{
   return m_Duration;
}

bool
File::is_open() const
{
   //std::lock_guard< std::mutex > guard( m_Mutex );
   return m_IsOpen;
}

bool
File::open( std::string uri, bool debug )
{
   m_IsDebug = debug;

   PerformanceTimer timer;
   timer.start();

   if ( is_open() )
   {
      close();
   }

   m_Uri = uri;

   //std::lock_guard< std::mutex > guard( m_Mutex );

   m_avFrame = av_frame_alloc();
   m_avPacket = av_packet_alloc();
   m_avFormatCtx = avformat_alloc_context();
   if ( !m_avFormatCtx ) { DE_ERROR("No context") return false; }
   int e = avformat_open_input( &m_avFormatCtx, uri.c_str(), nullptr, nullptr );
   if (e < 0) { DE_ERROR("Cant open uri ", uri ) return false; }
   e = avformat_find_stream_info( m_avFormatCtx, nullptr );
   if (e < 0) { DE_ERROR("Cant find stream info in uri ", uri ) }
   // Enumerate streams
   for ( int32_t i = 0; i < m_avFormatCtx->nb_streams; ++i )
   {
      m_Streams.push_back( new FileStream( m_avFormatCtx, i ) );
   }

   if ( m_IsDebug )
   {
      Utils::dumpStreamInfos( m_avFormatCtx, false );
   }

   m_IsOpen = true;
   m_IsSeekable = true;
   m_Position = 0.0;
   m_Duration = 0.0;
   for ( auto & stream : m_Streams )
   {
      m_IsSeekable &= stream->isSeekable();
      m_Duration = std::max( m_Duration, stream->getDuration() );
   }

   if ( m_IsDebug )
   {
      DE_DEBUG("File.IsSeekable = ", m_IsSeekable )
      DE_DEBUG("File.Duration = ", dbStrSeconds( m_Duration ) )
      timer.stop();
      DE_DEBUG("File.LoadTime = ", timer.ms(), " ms.")
   }
//   fillCacheUnguarded();
   return m_IsOpen;
}

void
File::close()
{
   //std::lock_guard< std::mutex > guard( m_Mutex );

   for ( size_t i = 0; i < m_Streams.size(); ++i )
   {
      FileStream* p = m_Streams[ i ];
      if ( !p ) continue;
      delete p;
   }
   m_Streams.clear();
   if ( m_avFormatCtx ) avformat_close_input( &m_avFormatCtx );
   if ( m_avFormatCtx ) avformat_free_context( m_avFormatCtx );
   if ( m_avPacket ) av_packet_free( &m_avPacket );
   if ( m_avFrame ) av_frame_free( &m_avFrame );

//   m_AudioQueue.clear();
//   m_ImageQueue.clear();
}

// receiver needs to delete payload after use.
int32_t
File::readFrame( File::ON_GET_DATA const & onGetData )
{
   if ( !m_avFormatCtx ) { DE_ERROR("No context") return AVERROR_EOF; }

   //
   // readPacket() Usually one image or multiple audio frames
   //
   int e = av_read_frame( m_avFormatCtx, m_avPacket );
   if (e < 0)
   {
      DE_DEBUG("[Fail] av_read_frame(), e = ", e, " = ", Utils::getErrorStr(e) )
      return e;
   }

   //
   // filter packet: to its corresponding stream->m_av_decoder_context
   //
   FileStream* stream = m_Streams[ m_avPacket->stream_index ];
   if ( !stream ) { DE_ERROR("No stream") return 0; /* skip */ }
   if ( !stream->isEnabled() ) { return 0; /* skip */ }
   //AVCodecContext* m_avCodecCtx = stream->m_avCodecCtx;
   //if ( !m_avCodecCtx ) { DE_ERROR("No codec ctx") return AVERROR_EOF; }

   //
   // sendPacket()
   //
   e = avcodec_send_packet( stream->m_avCodecCtx, m_avPacket );
   if (e < 0)
   {
      DE_ERROR("[Fail] avcodec_send_packet(), e = ", e, " = ", Utils::getErrorStr(e) )
      return e;
   }

   // NOTE: Each packet may generate more than one frame, depending on the codec.
   // receive and handle frame, rereceive until no EAGAIN
   // DE_DEBUG("e = ", e, " = ", Utils::getErrorStr(e) )
   while (e >= 0)
   {
      e = avcodec_receive_frame( stream->m_avCodecCtx, m_avFrame );
      if (e >= 0)
      {
         if ( stream->isEnabled() )
         {
            std::unique_ptr< FilePayload > payload( new FilePayload() );
            payload->time_base = stream->m_avStream->time_base;
            //payload->time_base = m_avFrame->time_base;
            //payload->pts = m_avFrame->pts;
            payload->pts = m_avPacket->pts;
            payload->dts = m_avPacket->dts;
            payload->duration = m_avPacket->duration;
            payload->streamIndex = stream->m_avStreamIndex;
            //m_Position += payload->getDuration();
            double tb = Utils::toDouble( payload->time_base );

            bool didDuration = false;

            if ( stream->isAudio() )
            {
               payload->typ = AVMEDIA_TYPE_AUDIO;
               payload->buf.reset( new audio::SampleBuffer() );
               Utils::extractAudio( m_avFrame, *payload->buf, m_AudioOutputType );

               payload->benni_pts = m_Position;

               if ( !didDuration )
               {
                  m_Position += tb * double( payload->duration );
                  didDuration = true;
               }

            }
            else if ( stream->isVideo() )
            {
//               DE_DEBUG("VideoFrame bts(", payload->benni_pts, "), "
//                                   "pts(", payload->pts, "), "
//                                   "dts(", payload->dts, "), "
//                                   "duration(", payload->duration, "), "
//                                   "tb(",Utils::toString(payload->time_base),")")
               payload->typ = AVMEDIA_TYPE_VIDEO;
               payload->img.reset( new Image );
               Utils::extractImage( m_avFrame, *payload->img );
               payload->benni_pts = tb * payload->pts;

               if ( !didDuration )
               {
                  m_Position += tb * double( payload->duration );
                  didDuration = true;
               }

            }
            onGetData( std::move( payload ) );
         }
         av_frame_unref( m_avFrame );
      }
      else
      {
         //DE_ERROR("[Fail] avcodec_receive_frame(), e = ", e, " = ", Utils::getErrorStr(e) )
         break;
      }
   }
   av_packet_unref( m_avPacket );
   return e;
}

void
File::setAudioOutFormat( de::audio::ESampleType sampleType )
{
   m_AudioOutputType = sampleType;
}


bool
File::isStream( int i ) const
{
   if ( i < 0 )
   {
      return false;
   }
   if ( i >= int( m_Streams.size() ) )
   {
      DE_ERROR("Bad")
      return false;
   }
   return true;
}

uint32_t
File::getStreamCount() const
{
   return m_Streams.size();
}


FileStream*
File::getStream( int i )
{
   if ( !isStream( i ) ) { DE_ERROR("No stream i = ",i) return nullptr; }
   return m_Streams[ i ];
}

FileStream const*
File::getStream( int i ) const
{
   if ( !isStream( i ) ) { DE_ERROR("No stream i = ",i) return nullptr; }
   return m_Streams[ i ];
}


void
File::enableStream( int i )
{
   FileStream* stream = getStream( i );
   if ( !stream ) { DE_ERROR("No stream i = ",i) return; }
   stream->enable();
}

void
File::disableStream( int i )
{
   FileStream* stream = getStream( i );
   if ( !stream ) { DE_ERROR("No stream i = ",i) return; }
   stream->disable();
}

void
File::enableStreams()
{
   for ( size_t i = 0; i < m_Streams.size(); ++i )
   {
      enableStream( i );
   }
}

void
File::disableStreams()
{
   for ( size_t i = 0; i < m_Streams.size(); ++i )
   {
      disableStream( i );
   }
}


#if 0


double
File::getPosition() const
{
   std::lock_guard< std::mutex > guard( m_Mutex );

   return m_Position;
}

void
File::setPosition( double seconds )
{
   std::lock_guard< std::mutex > guard( m_Mutex );

   m_Position = seconds;

   if ( m_Position < 0.0 )
   {
      m_Position = 0.0;
   }

   if ( m_Position > m_Duration )
   {
      m_Position = m_Duration - 1.0;
   }

   for ( auto & stream : m_Streams )
   {
      stream->setPosition( seconds );
   }

   m_AudioQueue.clear();
   m_ImageQueue.clear();
   fillCacheUnguarded();
}

//bool
//File::hasDuration() const
//{
//   bool has_dur = Utils::hasDuration( m_avFormatCtx, m_AudioStreamIndex );
//   has_dur |= Utils::hasDuration( m_avFormatCtx, m_VideoStreamIndex );
//   return has_dur;
//}

double
File::getDuration() const
{
   // Utils::getDuration( m_avFormatCtx )
   return m_Duration;
}


uint32_t
File::getAudioStreamCount() const
{
   size_t n = 0;
   for ( auto p : m_Streams ) { if ( p && p->isAudio() ) n++; }
   return n;
}

FileStream*
File::getVideoStream( int i ) const
{
   int n = getVideoStreamCount();
   if ( i >= n ) return nullptr;
   int k = 0;
   for ( auto p : m_Streams )
   {
      if ( p && p->isVideo() )
      {
         if ( k == i )
         {
            return p;
         }
         k++;
      }
   }
   return nullptr;
}

FileStream*
File::getAudioStream( int i ) const
{
   int n = getAudioStreamCount();
   if ( i >= n ) return nullptr;
   int k = 0;
   for ( auto p : m_Streams )
   {
      if ( p && p->isAudio() )
      {
         if ( k == i )
         {
            return p;
         }
         k++;
      }
   }
   return nullptr;
}

uint32_t
File::getVideoStreamCount() const
{
   size_t n = 0;
   for ( auto p : m_Streams ) { if ( p && p->isVideo() ) n++; }
   return n;
}


bool
File::getCoverArt( de::Image & img )
{
   int best = Utils::findBestVideo( m_avFormatCtx );
   if ( !Utils::isStream( m_avFormatCtx, best ) ) { DE_ERROR("No video") return false; }

   FileStream* stream = getStream( best );
   if ( !stream ) { DE_ERROR("Bad ptr") return false; }
   if ( !stream->isAudio() ) { DE_ERROR("Bad type") return false; }

   stream->setPosition( 0.0 );

   int loop_count = 0;
   int loop_count_max = 20;
   bool img_found = false;

   auto readAnyImagePayload =
      [&](std::shared_ptr< FilePayload > payload )
      {
         if ( payload->img )
         {
            img = *payload->img;
            img_found = true;
         }
      };

   while ( !img_found &&
           loop_count < loop_count_max &&
           readFrame( readAnyImagePayload ) != AVERROR_EOF )
   {
      loop_count++;
   };

   return img_found;
}


// The audio engine calls this, so it should be threadsafe
uint64_t
File::fillAudioBuffer( audio::Buffer & dst, double pts )
{
   std::lock_guard< std::mutex > guard( m_Mutex );

   if ( m_AudioFrameIndex + dst.getFrameCount() > m_AudioFrameCount )
   {
      DE_DEBUG("[BAD] Emergency cache audio frames ", dst.getFrameCount() )
      fillCacheUnguarded();
   }

   uint64_t k = m_AudioQueue.pop_front( dst, pts );
   if ( k < dst.getFrameCount() )
   {
      DE_DEBUG("Reached eof? k(",k,") < dst.getFrameCount(",dst.getFrameCount(),")")
   }

   m_AudioFrameIndex += k;
   m_Position += dst.getDuration();

   fillCacheUnguarded();
   return k;
}

bool
File::fillImageBuffer( Image & dst, double pts )
{
   std::lock_guard< std::mutex > guard( m_Mutex );
   fillCacheUnguarded();

   bool ok = m_ImageQueue.pop_front( dst, pts );
   if ( !ok )
   {
      DE_DEBUG("Reached eof? pts(",pts,")")
   }

   return ok;
}

void
File::fillCache()
{
   std::lock_guard< std::mutex > guard( m_Mutex );
   fillCacheUnguarded();
}

void
File::fillCacheUnguarded()
{
   if ( !m_IsOpen )
   {
      DE_WARN("Not open")
      return;
   }

   if ( m_AudioFrameIndex + 30000 < m_AudioFrameCount )
   {
      DE_DEBUG( "Cache is full")
      DE_DEBUG("m_AudioFrameIndex = ", m_AudioFrameIndex)
      DE_DEBUG("m_AudioFrameCount = ", m_AudioFrameCount)
      DE_DEBUG("m_AudioQueue.size() = ", m_AudioQueue.size())
      DE_DEBUG("m_ImageQueue.size() = ", m_ImageQueue.size())

      return; // Cache is full
   }

   //m_Position = m_Duration;

   auto pushToQueue =
      [&](std::shared_ptr< FilePayload > payload)
      {
         if ( payload->streamIndex == m_AudioStreamIndex &&
              payload->typ == AVMEDIA_TYPE_AUDIO &&
              payload->buf )
         {
            m_AudioFrameCount += payload->buf->getFrameCount();
            m_Duration += payload->buf->getDuration();
            m_AudioQueue.push_back( payload->buf, payload->getPTS() );
         }

         if ( payload->streamIndex == m_VideoStreamIndex &&
              payload->typ == AVMEDIA_TYPE_VIDEO &&
              payload->img )
         {
            m_VideoFrameCount += 1; // payload->img->getFrameCount();
            m_ImageQueue.push_back( payload->img, payload->getPTS() );
         }
      };

   while ( 1 )
   {
      int e = readFrame( pushToQueue );
      if ( e == AVERROR_EOF )
      {
         break;
      }

      if ( m_AudioFrameIndex + 30000 < m_AudioFrameCount )
      {
         return; // Cache is full
      }

//      // while loop body
//      if ( m_Duration - lastDuration >= m_PrecacheDuration )
//      {
//         DE_DEBUG("[Cache] Filled another 300ms" )
//         break;
//      }

//      // while loop body
//      if ( m_AudioQueue.size() > m_AudioQueueThresholdLoadStop )
//      {
//         //DE_DEBUG("[QueueFilled] ",m_AudioQueue.size() )
//         break;
//      }
   };

   //m_FrameCount = m_AudioQueue.size();
}

#endif

} // end namespace ffmpeg
} // end namespace de

