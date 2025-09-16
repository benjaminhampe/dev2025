#include <de/audio/decoder/DecoderFFMPEG.hpp>

namespace de {
namespace audio {
namespace decoder {

double
DecoderTools::toDouble( AVRational const & r )
{
   if ( r.num == 0 ) { return 0.0; }
   if ( r.den == 0 ) { return 0.0; }
   return double(r.num) / double(r.den);
}

std::string
DecoderTools::getErrorStr( int errnum )
{
   std::string tmp;
   tmp.resize( 1024 );
   int res = av_strerror( errnum, tmp.data(), tmp.size() );
   if ( res < 0 )
   {
      tmp = std::string( "Unknown ErrorNumber " ) + std::to_string( errnum );
   }
   return tmp;
}

float
DecoderTools::getSample( AVSampleFormat const & fmt, uint8_t const* src, uint32_t srcBPS )
{
   int64_t val = 0;
   float ret = 0;
   switch( srcBPS )
   {
   case 1:
      // 8bit samples are always unsigned, make signed.
      val = int( *reinterpret_cast< uint8_t const* >( src ) ) - 127; break;
   case 2:
      val = *reinterpret_cast< int16_t const* >( src ); break;
   case 4:
      val = *reinterpret_cast< int32_t const* >( src ); break;
   case 8:
      val = *reinterpret_cast< int64_t const* >( src ); break;
   default:
      DE_ERROR("Invalid srcBPS ", srcBPS) return 0.f;
   }

   // Check which data type is in the sample.
   switch ( fmt )
   {
   case AV_SAMPLE_FMT_U8:
   case AV_SAMPLE_FMT_S16:
   case AV_SAMPLE_FMT_S32:
   case AV_SAMPLE_FMT_U8P:
   case AV_SAMPLE_FMT_S16P:
   case AV_SAMPLE_FMT_S32P:
      // integer => Scale to [-1, 1] and convert to float.
      ret = val / float( ((1 << (srcBPS*8-1))-1) ); break;
   case AV_SAMPLE_FMT_FLT:
   case AV_SAMPLE_FMT_FLTP:
      // float => reinterpret
      ret = *reinterpret_cast< float* >( &val ); break;
   case AV_SAMPLE_FMT_DBL:
   case AV_SAMPLE_FMT_DBLP:
      ret = float( *reinterpret_cast< double* >( &val ) ); break;
   default:
      DE_ERROR("Invalid sample format ", av_get_sample_fmt_name( fmt )) return 0;
   }

   return ret;
}

bool
DecoderTools::isInterleaved( AVSampleFormat fmt )
{
   switch ( fmt )
   {
      case AV_SAMPLE_FMT_U8: return true;
      case AV_SAMPLE_FMT_S16: return true;
      case AV_SAMPLE_FMT_S32: return true;
      case AV_SAMPLE_FMT_FLT: return true;
      case AV_SAMPLE_FMT_DBL: return true;
      case AV_SAMPLE_FMT_U8P: return false;
      case AV_SAMPLE_FMT_S16P: return false;
      case AV_SAMPLE_FMT_S32P: return false;
      case AV_SAMPLE_FMT_FLTP: return false;
      case AV_SAMPLE_FMT_DBLP: return false;
      default:                 return false;
   }
}

uint64_t
DecoderTools::extractAudio( AVFrame* avFrame, AudioPayload & payload, bool debug )
{
   std::vector< float > & dst = payload.samples;
   dst.clear();

   payload.channelCount = uint32_t( avFrame->channels ); // in [ch]
   payload.frameCount = uint64_t( avFrame->nb_samples );
   payload.sampleRate = avFrame->sample_rate;

   uint64_t sampleCount = payload.frameCount * payload.channelCount;
   if ( sampleCount < 1 )
   {
      DE_ERROR("No samples")
      return 0;
   }
   dst.resize( sampleCount );

   auto srcFormat = AVSampleFormat( avFrame->format );
   auto srcBPS = av_get_bytes_per_sample( srcFormat );

   auto interleaved = isInterleaved( srcFormat );
   if ( interleaved )
   {
      if ( debug )
      {
         std::string fmtStr = av_get_sample_fmt_name( srcFormat );
         DE_DEBUG("INTERLEAVED srcFormat(",fmtStr,"), srcBPS(",srcBPS,"), payload(",payload.toString(),")")
      }

      uint8_t const* pSrc = avFrame->extended_data[0];
      float* pDst = dst.data();

      for ( size_t i = 0; i < sampleCount; ++i )
      {
         float sample = getSample( srcFormat, pSrc, srcBPS );
         pSrc += srcBPS;
         *pDst = sample;
         pDst++;
      }
   }
   else
   {
      if ( debug )
      {
         std::string fmtStr = av_get_sample_fmt_name( srcFormat );
         DE_DEBUG("PLANAR srcFormat(",fmtStr,"), srcBPS(",srcBPS,"), payload(",payload.toString(),")")
      }

      for ( size_t c = 0; c < payload.channelCount; ++c )
      {
         uint8_t const* pSrc = reinterpret_cast< uint8_t const* >( avFrame->extended_data[ c ] );
         float* pDst = dst.data() + c;
         for ( size_t i = 0; i < payload.frameCount; ++i )
         {
            float sample = getSample( srcFormat, pSrc, srcBPS );
            pSrc += srcBPS;
            *pDst = sample;
            pDst += payload.channelCount;
         }
      }
   }

   // DE_DEBUG("CHUNK ", dst.toString( 10 ) )

   return sampleCount;
}

int32_t
DecoderTools::readFrame( DecoderCtx & ds, ON_AUDIO_PAYLOAD const & onPayload )
{
   if ( !ds.isOpen ) { DE_ERROR("Not open, ", ds.uri) return AVERROR_EOF; }

   //
   // readPacket() Usually one image or multiple audio frames
   //
   int e = av_read_frame( ds.formatCtx, ds.packet );
   if (e < 0)
   {
      DE_DEBUG("[Fail] No av_read_frame(",e,"), ", getErrorStr(e) )
      return e;
   }

   //
   // sendPacket()
   //
   e = avcodec_send_packet( ds.codecCtx, ds.packet );
   if (e < 0)
   {
      //DE_ERROR("[Fail] No avcodec_send_packet(",e,"), ", getErrorStr(e) )
      return e;
   }

   // NOTE: Each packet may generate more than one frame, depending on the codec.
   // receive and handle frame, rereceive until no EAGAIN
   // DE_DEBUG("e = ", e, " = ", Utils::getErrorStr(e) )
   while (e >= 0)
   {
      e = avcodec_receive_frame( ds.codecCtx, ds.frame );
      if (e >= 0)
      {
         AudioPayload payload;
         if ( extractAudio( ds.frame, payload ) > 0 )
         {
            auto tb = ds.stream->time_base;
            double pts = toDouble( tb ) * ds.packet->pts;
            //double dts = toDouble( tb ) * ds.packet->dts;
            payload.pts = pts;
            onPayload( std::move( payload ) );
         }
      }
      av_frame_unref( ds.frame );
   }
   av_packet_unref( ds.packet );
   return e;
}

void
DecoderTools::initDecoder( DecoderCtx & ds )
{
   // File
   ds.formatCtx = nullptr;
   ds.packet = nullptr;
   ds.frame = nullptr;
   ds.isOpen = false;
   ds.isDebug = false;
   ds.isSeekable = false;
   ds.hasFrameCount = false;
   ds.hasDuration = false;

   // AudioFileStream
   ds.streamIndex = -1;
   ds.durationTicks = 0;
   ds.stream = nullptr;
   ds.codecParams = nullptr;
   ds.codec = nullptr;
   ds.codecCtx = nullptr;
   ds.position = 0.0;
   ds.duration = 0.0;

   ds.channelCount = 0;
   ds.sampleRate = 0;
   ds.frameCount = 0;

   ds.swResampler = nullptr;
}

void
DecoderTools::closeDecoder( DecoderCtx & ds )
{
   //drainDecoder(ds.codecCtx, m_avFrame);
   if ( ds.codecCtx )
   {
      avcodec_close( ds.codecCtx );
      avcodec_free_context( &ds.codecCtx );
      ds.codecCtx = nullptr;
   }

   if ( ds.swResampler )
   {
      swr_free( &ds.swResampler );
      ds.swResampler = nullptr;
   }
   //sws_freeContext( m_swScaler );

   if ( ds.formatCtx )
   {
      avformat_close_input( &ds.formatCtx );
      avformat_free_context( ds.formatCtx );
      ds.formatCtx = nullptr;
   }
   if ( ds.packet )
   {
      av_packet_free( &ds.packet );
      ds.packet = nullptr;
   }
   if ( ds.frame )
   {
      av_frame_free( &ds.frame );
      ds.frame = nullptr;
   }
}

bool
DecoderTools::openDecoder( DecoderCtx & ds, std::string uri, int streamIndex, bool debug )
{
   if ( ds.isOpen )
   {
      DE_ERROR("File already open, ", ds.uri )
      return false;
   }

   ds.isDebug = debug;
   PerformanceTimer perf;
   perf.start();

   ds.uri = uri;
   ds.formatCtx = avformat_alloc_context();
   if ( !ds.formatCtx )
   {
      DE_ERROR("No format context")
      return false;
   }

   int e = avformat_open_input( &ds.formatCtx, uri.c_str(), nullptr, nullptr );
   if (e < 0)
   {
      DE_ERROR("Cant open uri ", uri )
      return false;
   }

   e = avformat_find_stream_info( ds.formatCtx, nullptr );
   if (e < 0)
   {
      DE_WARN("Cant find stream info in uri ", uri )
   }

   if ( streamIndex < 0 )
   {
      streamIndex = av_find_best_stream( ds.formatCtx,
                        AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0 );
   }

   if ( streamIndex < 0 )
   {
      DE_ERROR("No audio stream in file ", uri )
      closeDecoder( ds );
      return false;
   }


   ds.streamIndex = streamIndex;
   ds.stream = ds.formatCtx->streams[ ds.streamIndex ];
   if ( !ds.stream )
   {
      DE_ERROR("No stream ", uri )
      closeDecoder( ds );
      return false;
   }

   ds.codecParams = ds.stream->codecpar;
   ds.codecId = ds.codecParams->codec_id;
   ds.codec = avcodec_find_decoder( ds.codecParams->codec_id );
   if ( !ds.codec )
   {
      DE_ERROR("No decoder found, streamIndex(",ds.streamIndex,")")
      closeDecoder( ds );
      return false;
   }

   ds.codecCtx = avcodec_alloc_context3( ds.codec );
   if ( !ds.codecCtx )
   {
      DE_ERROR("No avcodec_alloc_context3()")
      closeDecoder( ds );
      return false;
   }

   e = avcodec_parameters_to_context( ds.codecCtx, ds.codecParams );
   if (e < 0)
   {
      DE_ERROR("No avcodec_parameters_to_context, e = ", e)
   }

   // Request interleaved ( non planar ) data....
   if ( ds.codecCtx->codec_type != AVMEDIA_TYPE_AUDIO )
   {
      DE_ERROR("ds.codecCtx->codec_type != AVMEDIA_TYPE_AUDIO, e = ", e)
   }

   AVSampleFormat fmt = ds.codecCtx->sample_fmt;
   ds.codecCtx->request_sample_fmt = av_get_alt_sample_fmt( fmt, 0 ); // 0 = interleaved, 1 = planar

   // Open
   e = avcodec_open2( ds.codecCtx, ds.codec, nullptr );
   if (e < 0)
   {
      DE_ERROR("No avcodec_open2()")
      closeDecoder( ds );
      return false;
   }

   if ( !ds.frame )
   {
      ds.frame = av_frame_alloc();
   }

   if ( !ds.packet )
   {
      ds.packet = av_packet_alloc();
   }

   ds.timeBase.num = 0;
   ds.timeBase.den = 1;
   ds.durationTicks = 0;
   ds.duration = 0.0;
   ds.position = 0.0;
   ds.isSeekable = false;
   ds.hasDuration = false;
   ds.hasFrameCount = false;
   ds.frameCount = 0;
   ds.channelCount = ds.stream->codecpar->channels;
   ds.sampleRate = ds.stream->codecpar->sample_rate;

   if ( ds.stream->duration != AV_NOPTS_VALUE )
   {
      ds.hasDuration = true;
      ds.timeBase = ds.stream->time_base;
      ds.durationTicks = ds.stream->duration;
      ds.duration = toDouble( ds.timeBase ) * ds.durationTicks;
   }

   if ( ds.stream->nb_frames != AV_NOPTS_VALUE )
   {
      ds.frameCount = ds.stream->nb_frames;
      if ( ds.frameCount > 0 )
      {
         ds.hasFrameCount = true;
      }
   }

   if ( ds.hasDuration )
   {
      uint64_t estFrames = std::ceil( ds.duration * double( ds.sampleRate ) );
      if ( ds.hasFrameCount )
      {
         if ( ((3*estFrames) / 4) > ds.frameCount )
         {
            DE_WARN("Bad frameCount(",ds.frameCount,") estimate(", estFrames,")")
            ds.frameCount = estFrames;
         }
      }
      else
      {
         ds.frameCount = estFrames;
         //ds.hasFrameCount = true;
         DE_WARN("Estimate frameCount = ", ds.frameCount," from duration ")
      }
   }

//   if ( !ds.hasDuration && ds.hasFrameCount )
//   {
//      ds.duration = 256 + uint64_t( std::ceil( ds.duration * double( ds.sampleRate ) ) );
//      ds.hasFrameCount = true;
//      DE_WARN("Estimate frameCount = ", ds.frameCount," from duration ")
//   }

   if ( ds.hasFrameCount > 0 )
   {
      ds.isSeekable = true;
   }

   ds.isOpen = true;


   perf.stop();

   // fillCacheUnguarded();

   if ( ds.isDebug )
   {
      DE_DEBUG("Uri = ", ds.uri)
      DE_DEBUG("LoadTime = ", perf.ms(), " ms")
      DE_DEBUG("IsSeekable = ", ds.isSeekable )
      DE_DEBUG("HasDuration = ", ds.hasDuration )
      DE_DEBUG("HasFrameCount = ", ds.hasFrameCount )
      DE_DEBUG("Duration = ", dbStrSeconds( ds.duration ) )
      DE_DEBUG("ChannelCount = ", ds.channelCount )
      DE_DEBUG("FrameCount = ", ds.frameCount )
      DE_DEBUG("SampleRate = ", ds.sampleRate )

   }

   return true;
}


} // end namespace decoder
} // end namespace audio
} // end namespace de


