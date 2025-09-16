#include <de/video/VideoDecoderFFMPEG.hpp>

#if 0

double
VideoDecoderTools::toDouble( AVRational const & r )
{
   if ( r.num == 0 ) { return 0.0; }
   if ( r.den == 0 ) { return 0.0; }
   return double(r.num) / double(r.den);
}

std::string
VideoDecoderTools::getErrorStr( int errnum )
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

int32_t
VideoDecoderTools::readFrame( VideoDecoderCtx & ds, ON_IMAGE_PAYLOAD const & onPayload, bool debug )
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
         ImagePayload payload;
         if ( extractImage( ds.frame, ds.streamIndex, payload, debug ) )
         {
            auto tb = ds.stream->time_base;
            double pts = toDouble( tb ) * ds.packet->pts;
            //double dts = toDouble( tb ) * ds.packet->dts;
            payload.pts = pts;
            onPayload( payload );
         }
      }
      av_frame_unref( ds.frame );
   }
   av_packet_unref( ds.packet );
   return e;
}


//int dst_w,  // -1 for auto src_w, else scale to user_w
//int dst_h,  // -1 for auto src_h, else scale to user_h
//int scale_quality = SWS_BILINEAR
bool
VideoDecoderTools::extractImage( AVFrame* avFrame, int streamIndex, ImagePayload & payload, bool debug )
{
#if 1

   // Get source image width,height
   if ( !avFrame ) { DE_ERROR("No avFrame" ) return false; }
   int w = avFrame->width;
   int h = avFrame->height;

   if ( w < 1 ) { DE_ERROR("w(",w,") <= 0" ) return false; }
   if ( h < 1 ) { DE_ERROR("h(",h,") <= 0" ) return false; }

   AVPixelFormat srcFormat = AVPixelFormat( avFrame->format );
//   AVPixelFormat dstFormat = AV_PIX_FMT_RGB24; // i dont see any reason for an alpha channel
//   if ( img.getFormat() == de::ColorFormat::RGBA8888 )
//   {
//      dstFormat = AV_PIX_FMT_RGBA;
//   }

   payload.img.reset( new de::Image( w,h, de::ColorFormat::RGBA8888 ) );
   de::Image* dst = payload.img.get();

   if ( srcFormat == AV_PIX_FMT_RGB0 )
   {
      DE_ERROR("Direct store, coverArt image( ",w,",",h," ), srcFormat(",int(srcFormat),")")
      DE_FLUSH
      size_t stride = avFrame->linesize[ streamIndex ];

      for ( int y = 0; y < h; ++y )
      {
         uint8_t const* row = avFrame->data[ streamIndex ] + (stride * uint32_t( y ));
         for ( int x = 0; x < w; ++x )
         {
            uint8_t r = *row++;
            uint8_t g = *row++;
            uint8_t b = *row++;
            uint32_t color = de::RGBA( r,g,b );
            dst->setPixel( x,y, color );
         }
      }
   }
   else // if ( m_avCodecCtx->pix_fmt != AV_PIX_FMT_RGB0 )
   {
      DE_ERROR("Create coverArt image( ",w,",",h," ), srcFormat(",int(srcFormat),")" )
      DE_FLUSH
      // Init swScaler:
      //   SwsContext* m_swScaler = sws_alloc_context();

      //   int e = sws_init_context( m_swScaler, nullptr, nullptr );

      //   if ( m_avCodecCtx->pix_fmt == AV_PIX_FMT_RGB0 )
      //   {

//      if ( !pswScaler )
//      {
         auto pswScaler = sws_getContext(
               w, h, srcFormat,  // srcFormat
               w, h, AV_PIX_FMT_RGB0,  // dstFormat
               SWS_BILINEAR,       // flags e.g. scale_quality
               nullptr,                // srcFilter
               nullptr,                // dstFilter
               nullptr   );            // double* param
      //}

      if ( !pswScaler ) { DE_ERROR("No m_swScaler" ) return false; }


      DE_ERROR("OK sws_getContext()" )
      uint32_t stride = uint32_t( w ) * 4;
      uint8_t* dst_pointers[4] = { payload.img->data(), nullptr, nullptr, nullptr };
      int dst_strides[4] = { int(stride), 0, 0, 0 };
      int e = sws_scale( pswScaler,        // ctx
                 avFrame->data,     // srcSlice  - uint8_t[]
                 avFrame->linesize, // srcStride - int[]
                 0,                 // srcSliceY - int
                 h-1,   // srcSliceH - int
                 dst_pointers,      // dstSlice  - uint8_t[]
                 dst_strides );     // dstStride - int[]

      DE_ERROR("OK sws_scale()" )

   //   sws_freeContext( m_swScaler );
   //   m_swScaler = nullptr;

      DE_ERROR("OK sws_freeContext()" )

//      if ( e != 0 )
//      {
//         DE_ERROR("Got swScaler error e = ",e)
//      }
   }


    /// the picture is allocated by the decoder. no need to free it
   // snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
   // pgm_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf);
   //payload.img = img;

   DE_DEBUG("OK img(", payload.img->toString(),")" )
   DE_FLUSH
   return true;
#else
   return true;
#endif
}


void
VideoDecoderTools::initDecoder( VideoDecoderCtx & ds )
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

   ds.swScaler = nullptr;
}

void
VideoDecoderTools::closeDecoder( VideoDecoderCtx & ds )
{
   DE_ERROR("uri = ", ds.uri)

   //drainDecoder(ds.codecCtx, m_avFrame);
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
   if ( ds.swScaler )
   {
      //sws_freeContext( ds.swScaler );
      ds.swScaler = nullptr;
   }
   if ( ds.codecCtx )
   {
      avcodec_close( ds.codecCtx );
      avcodec_free_context( &ds.codecCtx );
      ds.codecCtx = nullptr;
   }
   if ( ds.formatCtx )
   {
      avformat_close_input( &ds.formatCtx );
      avformat_free_context( ds.formatCtx );
      ds.formatCtx = nullptr;
   }
   DE_ERROR("OK")
   DE_FLUSH
}


bool
VideoDecoderTools::openDecoderW( VideoDecoderCtx & ds, std::wstring uri, int streamIndex, bool debug )
{
   ds.wri = uri;
   return openDecoder( ds, dbStr(ds.wri), streamIndex, debug );
}


bool
VideoDecoderTools::openDecoder( VideoDecoderCtx & ds, std::string uri, int streamIndex, bool debug )
{
   //DE_ERROR("uri = ", ds.uri)

   closeDecoder( ds );

   ds.isDebug = debug;
   de::PerformanceTimer perf;
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
      streamIndex = av_find_best_stream( ds.formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0 );
   }
   if ( streamIndex < 0 )
   {
      DE_ERROR("No video stream found in uri ", uri )
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
   if ( !ds.codecParams )
   {
      DE_ERROR("No codecParams, streamIndex(",ds.streamIndex,")")
      closeDecoder( ds );
      return false;
   }

   ds.codecId = ds.codecParams->codec_id;
   ds.codec = avcodec_find_decoder( ds.codecId );
   if ( !ds.codec )
   {
      DE_ERROR("No coderId(",ds.codecId,") found, streamIndex(",ds.streamIndex,")")
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

   // Request image data....
   if ( ds.codecCtx->codec_type != AVMEDIA_TYPE_VIDEO )
   {
      DE_ERROR("No Video ds.codecCtx->codec_type != AVMEDIA_TYPE_VIDEO, e = ", e)
      closeDecoder( ds );
      return false;
   }

//   e = avcodec_parameters_to_context( ds.codecCtx, ds.codecParams );
//   if (e < 0)
//   {
//      DE_ERROR("No avcodec_parameters_to_context, e = ", e)
//   }



//   AVSampleFormat fmt = ds.codecCtx->sample_fmt;
//   ds.codecCtx->request_sample_fmt = av_get_alt_sample_fmt( fmt, 0 ); // 0 = interleaved, 1 = planar

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

   int w = ds.codecParams->width;
   int h = ds.codecParams->height;

   ds.timeBase.num = 0;
   ds.timeBase.den = 1;
//   ds.durationTicks = 0;
//   ds.duration = 0.0;
//   ds.position = 0.0;
//   ds.isSeekable = false;
//   ds.hasDuration = false;
//   ds.hasFrameCount = false;
   ds.frameCount = ds.stream->nb_frames;
//   ds.channelCount = ds.stream->codecpar->channels;
//   ds.sampleRate = ds.stream->codecpar->sample_rate;

//   if ( ds.stream->duration != AV_NOPTS_VALUE )
//   {
//      ds.hasDuration = true;
//      ds.timeBase = ds.stream->time_base;
//      ds.durationTicks = ds.stream->duration;
//      ds.duration = toDouble( ds.timeBase ) * ds.durationTicks;
//   }

//   if ( ds.stream->nb_frames != AV_NOPTS_VALUE )
//   {
//      ds.frameCount = ds.stream->nb_frames;
//      if ( ds.frameCount > 0 )
//      {
//         ds.hasFrameCount = true;
//      }
//   }

//   if ( ds.hasDuration )
//   {
//      uint64_t estFrames = std::ceil( ds.duration * double( ds.sampleRate ) );
//      if ( ds.hasFrameCount )
//      {
//         if ( ((3*estFrames) / 4) > ds.frameCount )
//         {
//            DE_WARN("Bad frameCount(",ds.frameCount,") estimate(", estFrames,")")
//            ds.frameCount = estFrames;
//         }
//      }
//      else
//      {
//         ds.frameCount = estFrames;
//         //ds.hasFrameCount = true;
//         DE_WARN("Estimate frameCount = ", ds.frameCount," from duration ")
//      }
//   }

//   if ( !ds.hasDuration && ds.hasFrameCount )
//   {
//      ds.duration = 256 + uint64_t( std::ceil( ds.duration * double( ds.sampleRate ) ) );
//      ds.hasFrameCount = true;
//      DE_WARN("Estimate frameCount = ", ds.frameCount," from duration ")
//   }

//   if ( ds.hasFrameCount > 0 )
//   {
//      ds.isSeekable = true;
//   }

   ds.isOpen = true;


   perf.stop();

   // fillCacheUnguarded();

   if ( ds.isDebug )
   {
      DE_DEBUG("Uri = ", ds.uri)
      DE_DEBUG("LoadTime = ", perf.ms(), " ms")
      DE_DEBUG("Width = ", w )
      DE_DEBUG("Height = ", h )
//      DE_DEBUG("FrameCount = ", ds.frameCount )
//      DE_DEBUG("SampleRate = ", ds.sampleRate )

//      DE_DEBUG("IsSeekable = ", ds.isSeekable )

//      DE_DEBUG("HasDuration = ", ds.hasDuration )
//      DE_DEBUG("HasFrameCount = ", ds.hasFrameCount )
//      DE_DEBUG("Duration = ", dbStrSeconds( ds.duration ) )
      //DE_DEBUG("ChannelCount = ", ds.channelCount )
      DE_FLUSH
   }

   return true;
}


#endif // 0
