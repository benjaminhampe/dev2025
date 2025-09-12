#include <de/ffmpeg/Utils.hpp>

namespace de {
namespace ffmpeg {

bool
Utils::isStream( AVFormatContext const * formatCtx, int i )
{
   if ( !formatCtx ) return false;
   if ( i < 0 ) return false;
   if ( i >= int( formatCtx->nb_streams ) ) return false;
   return true;
}

// check that a given sample format is supported by the encoder
bool
Utils::hasAVSampleFormat( AVCodec const * codec, AVSampleFormat searchToken )
{
   AVSampleFormat const* p = codec->sample_fmts;

   size_t i = 0;
   p = codec->sample_fmts;
   while (*p != AV_SAMPLE_FMT_NONE)
   {
      DE_DEBUG("AVSampleFormat[",i++,"] ", toString( *p++ ) )
   }

   p = codec->sample_fmts;
   while (*p != AV_SAMPLE_FMT_NONE)
   {
      if (*p == searchToken){ return true; }
      p++;
   }
   return false;
}

// just pick the highest supported samplerate
int
Utils::getBestSampleRate( AVCodec const * codec )
{
   if (!codec->supported_samplerates) { return 44100; }

   int best = 0;
   int const* p = codec->supported_samplerates;
   while (*p)
   {
      if (!best || abs(44100 - *p) < abs(44100 - best))
      {
         best = *p;
      }
      p++;
   }
   return best;
}

// select layout with the highest channel count
int
Utils::getChannelCountFromLayout( uint64_t channel_layout )
{
   if (channel_layout < 1)
   {
      return 0;
   }
   return av_get_channel_layout_nb_channels( channel_layout );
}

// select layout with the highest channel count
int
Utils::getBestChannelLayout( AVCodec const * codec)
{
   if (!codec)
   {
      return AV_CH_LAYOUT_STEREO;
   }

   uint64_t best_layout = 0;
   int best_channelCount   = 0;

   if (!codec->channel_layouts)
   {
      return AV_CH_LAYOUT_STEREO;
   }

   uint64_t const* p = codec->channel_layouts;
   while (*p)
   {
      int channelCount = av_get_channel_layout_nb_channels( *p );
      if (channelCount > best_channelCount)
      {
         best_layout = *p;
         best_channelCount = channelCount;
      }
      p++;
   }

   return best_layout;
}

int32_t
Utils::findBestStream( AVFormatContext * formatCtx, AVMediaType mediaType )
{
   return av_find_best_stream( formatCtx, mediaType, -1, -1, nullptr, 0 );
}

int32_t
Utils::findBestAudio( AVFormatContext * formatCtx )
{
   return findBestStream( formatCtx, AVMEDIA_TYPE_AUDIO );
}

int32_t
Utils::findBestVideo( AVFormatContext * formatCtx )
{
   return findBestStream( formatCtx, AVMEDIA_TYPE_VIDEO );
}


double
Utils::toDouble( AVRational const & r )
{
   if ( r.num == 0 ) { return 0.0; }
   if ( r.den == 0 ) { return 0.0; }
   double aspect = double(r.num) / double(r.den);
   //DE_DEBUG( "AVRational(",r.num,",",r.den,") = ",aspect )
   return aspect;
}

std::string
Utils::toString( AVRational const & r )
{
   std::stringstream s;
   //s << "AVRational(";
   s << r.num << "," << r.den;
   // << ")"; // << " = " << toDouble( r );
   return s.str();
}

std::string
Utils::getErrorStr( int errnum )
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

de::audio::ESampleType
Utils::toESampleType( AVSampleFormat sample_fmt )
{
   switch( sample_fmt )
   {
   case AV_SAMPLE_FMT_U8:  return de::audio::ST_U8I;
   case AV_SAMPLE_FMT_U8P: return de::audio::ST_U8P;
   case AV_SAMPLE_FMT_S16: return de::audio::ST_S16I;
   case AV_SAMPLE_FMT_S16P: return de::audio::ST_S16P;
   case AV_SAMPLE_FMT_S32: return de::audio::ST_S32I;
   case AV_SAMPLE_FMT_S32P: return de::audio::ST_S32P;
      // integer => Scale to [-1, 1] and convert to float.
      //ret = val / DE_STATIC_CAST(float, ((1 << (sampleSize*8-1))-1));
      //break;
   case AV_SAMPLE_FMT_FLT: return de::audio::ST_F32I;
   case AV_SAMPLE_FMT_FLTP: return de::audio::ST_F32P;
   case AV_SAMPLE_FMT_DBL: return de::audio::ST_F64I;
   case AV_SAMPLE_FMT_DBLP: return de::audio::ST_F64P;
   default:
      DE_ERROR( "Invalid sample_fmt ",av_get_sample_fmt_name( sample_fmt ) )
      return de::audio::ST_Unknown;
   }
}

AVSampleFormat
Utils::toAVSampleFormat( de::audio::ESampleType sampleType )
{
   switch( sampleType )
   {
   case de::audio::ST_U8I:  return AV_SAMPLE_FMT_U8;
   case de::audio::ST_U8P: return AV_SAMPLE_FMT_U8P;
   case de::audio::ST_S16I: return AV_SAMPLE_FMT_S16;
   case de::audio::ST_S16P: return AV_SAMPLE_FMT_S16P;
   case de::audio::ST_S32I: return AV_SAMPLE_FMT_S32;
   case de::audio::ST_S32P: return AV_SAMPLE_FMT_S32P;
   case de::audio::ST_F32I: return AV_SAMPLE_FMT_FLT;
   case de::audio::ST_F32P: return AV_SAMPLE_FMT_FLTP;
   case de::audio::ST_F64I: return AV_SAMPLE_FMT_DBL;
   case de::audio::ST_F64P: return AV_SAMPLE_FMT_DBLP;
   default:
      DE_ERROR( "Invalid sample_fmt ", de::audio::ST_toString( sampleType ) )
      return AV_SAMPLE_FMT_NONE;
   }
}

std::string
Utils::toString( AVSampleFormat sampleFormat )
{
   return de::audio::ST_toString( toESampleType( sampleFormat ) );
}


de::PixelFormat
Utils::toEColorFormat( AVPixelFormat pixelFormat )
{
   switch( pixelFormat )
   {
   case AV_PIX_FMT_RGB24: return de::PixelFormat::RGB_888;
   case AV_PIX_FMT_RGBA:  return de::PixelFormat::RGBA_8888;
   case AV_PIX_FMT_BGR0:  return de::PixelFormat::BGRA_8888;
   case AV_PIX_FMT_BGR24:  return de::PixelFormat::BGR_888;
   default: DE_ERROR( "Invalid pixel format" ) return de::PixelFormat::Unknown;
   }
}

std::string
Utils::toString( AVMediaType mediaType )
{
   switch( mediaType )
   {
   case AVMEDIA_TYPE_ATTACHMENT: return "Attachment";
   case AVMEDIA_TYPE_AUDIO: return "Audio";
   case AVMEDIA_TYPE_VIDEO: return "Video";
   case AVMEDIA_TYPE_SUBTITLE: return "SubTitle";
   case AVMEDIA_TYPE_DATA: return "BinaryData";
   default: return "Unknown";
   }
}

std::string
Utils::toString( AVPixelFormat pixelFormat )
{
   switch( pixelFormat )
   {
   case AV_PIX_FMT_RGB24: return "RGB888";
   case AV_PIX_FMT_RGBA: return "RGBA8888";
   case AV_PIX_FMT_BGR0: return "BGRA8888";
   case AV_PIX_FMT_BGR24: return "BGR888";
   default: DE_ERROR( "Invalid pixel format" ) return "Unknown";
   }
}

// Logs infos to console and possibly a file, duration not guaranteed without a full scan
void
Utils::dumpStreamInfos( AVFormatContext* file, bool dumpInfoFilePerStream )
{
   if ( !file )
   {
      DE_DEBUG("NOT OPEN")
      return;
   }
   std::string uri = file->filename;
   std::string fileName = FileSystem::fileName( uri );

   DE_DEBUG("File.Uri = ", uri )
   DE_DEBUG("File.Name = ", fileName )

   int bestAudio = av_find_best_stream( file, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0 );
   int bestVideo = av_find_best_stream( file, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0 );
   int bestSubtitle = av_find_best_stream( file, AVMEDIA_TYPE_SUBTITLE, -1, -1, nullptr, 0 );
   DE_DEBUG("File.BestAudio = ", bestAudio )
   DE_DEBUG("File.BestVideo = ", bestVideo )
   DE_DEBUG("File.BestSubtitle = ", bestSubtitle )

   if ( file->duration == AV_NOPTS_VALUE )
   {
      DE_DEBUG("File.Duration = AV_NOPTS_VALUE" )
   }
   else
   {
      DE_DEBUG("File.Duration = ", file->duration )
   }

   if ( file->start_time == AV_NOPTS_VALUE )
   {
      DE_DEBUG("File.StartTime = AV_NOPTS_VALUE" )
   }
   else
   {
      DE_DEBUG("File.StartTime = ", file->start_time )
   }

   DE_DEBUG("File.max_chunk_size = ", file->max_chunk_size )
   DE_DEBUG("File.max_chunk_duration = ", file->max_chunk_duration )
   DE_DEBUG("File.max_delay = ", file->max_delay )
   DE_DEBUG("File.max_picture_buffer = ", file->max_picture_buffer )

   uint32_t streamCount = file->nb_streams;
   DE_DEBUG("File.StreamCount = ", streamCount )

   for ( uint32_t i = 0; i < streamCount; ++i )
   {
      AVStream* stream = file->streams[ i ];
      if ( !stream ) continue;
      AVCodecParameters* codecInfo = stream->codecpar;
      if ( !codecInfo )
      {
         DE_ERROR("No AVCodecParameters for stream ",i)
         continue;
      }
      AVCodecID codecId = codecInfo->codec_id;
      AVCodec* codec = avcodec_find_decoder( codecId );
      if ( !codec )
      {
         DE_ERROR("No AVCodec for stream ",i)
         continue;
      }
      std::string name;
      if ( codecInfo->codec_type == AVMEDIA_TYPE_VIDEO ) { name = "Video"; }
      else if ( codecInfo->codec_type == AVMEDIA_TYPE_AUDIO ) { name = "Audio"; }
      else if ( codecInfo->codec_type == AVMEDIA_TYPE_SUBTITLE ) { name = "Subtitle"; }
      else if ( codecInfo->codec_type == AVMEDIA_TYPE_DATA ) { name = "Data"; }
      else { name = "Unknown"; }
      DE_DEBUG(name,"Stream[",i,"].CodecName = ", codec->long_name )
      DE_DEBUG(name,"Stream[",i,"].CodecID = ", int( codecId ) )
      if ( codecInfo->codec_type == AVMEDIA_TYPE_VIDEO )
      {
         DE_DEBUG(name,"Stream[",i,"].Width = ", codecInfo->width )
         DE_DEBUG(name,"Stream[",i,"].Height = ", codecInfo->height )
         DE_DEBUG(name,"Stream[",i,"].SampleRate = ", codecInfo->sample_rate )
         DE_DEBUG(name,"Stream[",i,"].FrameSize = ", codecInfo->frame_size )
      }
      else if ( codecInfo->codec_type == AVMEDIA_TYPE_AUDIO )
      {
         DE_DEBUG(name,"Stream[",i,"].Channels = ", codecInfo->channels )
         DE_DEBUG(name,"Stream[",i,"].ChannelLayout = ", codecInfo->channel_layout )
         DE_DEBUG(name,"Stream[",i,"].SampleRate = ", codecInfo->sample_rate )
         DE_DEBUG(name,"Stream[",i,"].FrameSize = ", codecInfo->frame_size )
      }
      DE_DEBUG(name,"Stream[",i,"].FrameCount = ", stream->nb_frames )
      DE_DEBUG(name,"Stream[",i,"].BitRate = ", codecInfo->bit_rate )
      DE_DEBUG(name,"Stream[",i,"].Level = ", codecInfo->level )
      DE_DEBUG(name,"Stream[",i,"].Profile = ", codecInfo->profile )
      DE_DEBUG(name,"Stream[",i,"].TimeBase = ", toString( stream->time_base ) )
      double t = Utils::toDouble( stream->time_base );
      if ( stream->duration == AV_NOPTS_VALUE )
      {
         DE_DEBUG(name,"Stream[",i,"].Duration = AV_NOPTS_VALUE" )
      }
      else
      {
         DE_DEBUG(name,"Stream[",i,"].Duration = ", dbStrSeconds( t * stream->duration ) )
      }
      if ( stream->start_time == AV_NOPTS_VALUE )
      {
         DE_DEBUG(name,"Stream[",i,"].StartTime = AV_NOPTS_VALUE" )
      }
      else
      {
         DE_DEBUG(name,"Stream[",i,"].StartTime = ", dbStrSeconds( t * stream->start_time ) )
      }

      if ( stream->duration > 0 && stream->duration != AV_NOPTS_VALUE )
      {
         double fps = double( stream->nb_frames ) / stream->duration;
         DE_DEBUG(name,"Stream[",i,"].FPS = ", fps )
      }
      else
      {
         DE_DEBUG(name,"Stream[",i,"].FPS = AV_NOPTS_VALUE" )
      }
   }

   if ( dumpInfoFilePerStream )
   {
      for ( uint32_t i = 0; i < streamCount; ++i )
      {
         std::string dumpName = fileName + ".stream" + std::to_string( i ) + ".txt";
         av_dump_format( file, i, dumpName.c_str(), 0 );
         DE_DEBUG("Stream[",i,"].Dump = ", dumpName )
      }
   }

}

bool
Utils::hasDuration( AVFormatContext* formatCtx, int streamIndex )
{
   if ( !formatCtx ) { DE_ERROR( "No formatCtx" ) return false; }
   if ( !isStream( formatCtx, streamIndex ) ) { DE_ERROR( "No streamIndex" ) return false; }
   AVStream* stream = formatCtx->streams[ streamIndex ];
   if ( !stream ) { DE_ERROR( "No stream" ) return false; }
   return (stream->duration != AV_NOPTS_VALUE);
}

double
Utils::getDuration( AVFormatContext* formatCtx, int streamIndex )
{
   if ( !formatCtx ) { DE_ERROR( "No formatCtx" ) return 0.0; }
   if ( !isStream( formatCtx, streamIndex ) ) { DE_ERROR( "No streamIndex" ) return 0.0; }
   AVStream* stream = formatCtx->streams[ streamIndex ];
   if ( !stream ) { DE_ERROR( "No stream" ) return 0.0; }
   if (stream->duration == AV_NOPTS_VALUE)
   {
      return 0.0;
   }
   else
   {
      return toDouble( stream->time_base ) * stream->duration;
   }
}

double
Utils::getDuration( AVFormatContext* formatCtx )
{
   if ( !formatCtx ) { DE_ERROR( "No formatCtx" ) return 0.0; }

   double max_duration = 0.0;
   for ( size_t i = 0; i < formatCtx->nb_streams; ++i)
   {
      AVStream* stream = formatCtx->streams[ i ];
      if ( !stream ) { DE_ERROR( "No stream ", i ) continue; }
      if (stream->duration != AV_NOPTS_VALUE)
      {
         max_duration = std::max( max_duration, toDouble( stream->time_base ) * stream->duration );
      }
   }
   return max_duration;
}

bool
Utils::seek(
   AVFormatContext* formatCtx,
   int streamIndex,
   double timeInSec,
   bool backward )
{
   if ( !formatCtx ) { DE_ERROR( "No formatCtx" ) return false; }
   if ( !isStream( formatCtx, streamIndex ) ) { DE_ERROR( "No streamIndex" ) return false; }
   AVStream* stream = formatCtx->streams[ streamIndex ];
   if ( !stream ) { DE_ERROR( "No stream" ) return false; }

   if ( stream->duration == AV_NOPTS_VALUE )
   {
      DE_ERROR("SEEK_POS Stream[",streamIndex,"] has no duration" )
   }
   else
   {
      double time_base = Utils::toDouble( stream->time_base );
      int64_t ts = int64_t( timeInSec / time_base );
      DE_DEBUG("SEEK_POS Stream[",streamIndex,"], Position(",timeInSec," s), ts(", ts,")" )

      int flags = AVSEEK_FLAG_ANY;
      if ( backward ) flags |= AVSEEK_FLAG_BACKWARD;
      int e = av_seek_frame( formatCtx, streamIndex, ts, flags );
      if ( e < 0 )
      {
         DE_ERROR("SEEK_FAIL Stream[",streamIndex,"], Position(",timeInSec," s), ts(",ts,")")
      }
      else
      {
         DE_DEBUG("SEEK_POS Stream[",streamIndex,"], Position(",timeInSec," s), ts(",ts,")")
      }
   }

   return true;
}


bool
Utils::seekTimeStamp(
   AVFormatContext* formatCtx,
   int streamIndex,
   int64_t ts,
   bool backward )
{
   if ( !formatCtx ) { DE_ERROR( "No formatCtx" ) return false; }
   //int n = formatCtx->nb_streams;
   //if ( streamIndex ) { throw std::runtime_error( "No formatCtx" ); }
   AVStream* stream = formatCtx->streams[ streamIndex ];
   if ( !stream ) { DE_ERROR( "No stream" ) return false; }

   int flags = AVSEEK_FLAG_ANY;
   if ( backward ) flags |= AVSEEK_FLAG_BACKWARD;
   int e = av_seek_frame( formatCtx, streamIndex, ts, flags );
   if ( e < 0 )
   {
      DE_ERROR("Stream[",streamIndex,"] Seek failed for timestamp(",ts,")")
   }
   else
   {
      DE_DEBUG("Stream[",streamIndex,"] Seeked to timestamp(",ts,")")
   }
   return true;
}



bool
Utils::seekPercent(
   AVFormatContext* formatCtx,
   int streamIndex,
   double pc )
{
   if ( !isStream( formatCtx, streamIndex ) ) { DE_ERROR( "No streamIndex" ) return false; }
   AVStream* stream = formatCtx->streams[ streamIndex ];
   //if ( !stream ) { DE_ERROR( "No stream" ) return false; }

   if (stream->duration == AV_NOPTS_VALUE)
   {
      DE_DEBUG("No duration in stream ",streamIndex )
      return false;
   }

   int64_t ts = int64_t( std::clamp( pc, 0.0, 1.0 ) * stream->duration );

   int e = av_seek_frame( formatCtx, streamIndex, ts, AVSEEK_FLAG_ANY );
   if ( e < 0 )
   {
      DE_ERROR("Stream[",streamIndex,"] Seek failed, pc(",pc,"), ts(",ts,")")
   }
   else
   {
      DE_DEBUG("Stream[",streamIndex,"] SeekPos, pc(",pc,"), ts(",ts,")")
   }
   return true;
}


//bool
//Utils::extractFirstImage(
//   AVFormatContext* formatCtx,
//   AVFrame* avFrame,
//   AVPacket* m_avPacket,
//   AVCodecContext* m_avCodecCtx,
//   int streamIndex,
//   de::Image & img,
//   int dst_w,  // -1 for auto src_w, else scale to user_w
//   int dst_h,  // -1 for auto src_h, else scale to user_h
//   int scale_quality )
//{
//   //seek( formatCtx, streamIndex, 0.0 );

//   bool found_img = false;
//   int32_t c = 0;

//   int e = 0;
//   while ( !found_img && e != AVERROR_EOF )
//   {
//      e = readFrame( formatCtx, avFrame, m_avPacket, m_avCodecCtx, streamIndex,
//         [&] ( AVFrame* avFrame )
//         {
//            extractFrameImage( avFrame, m_avCodecCtx, img, dst_w, dst_h, scale_quality );
//            found_img = true;
//         } );

//      c++;
//   }

//   DE_DEBUG("Found CoverArt after ", c, " tries.")

//   return found_img;
//}

int32_t
Utils::decodeFrame(
   AVFormatContext* formatCtx,
   AVFrame* avFrame,
   AVPacket* m_avPacket,
   AVCodecContext* m_avCodecCtx,
   int streamIndex,
   std::function< void(AVFrame*) > const & handleFrame )
{
   // std::cout << "[Debug] " << __func__ << std::endl;
   if ( !formatCtx ) { throw std::runtime_error("No context"); }
   int e = av_read_frame( formatCtx, m_avPacket );
   if ( e == AVERROR_EOF )
   {
      DE_DEBUG("EOF")
      return e;
   }
   if ( e < 0 )
   {
      DE_DEBUG(Utils::getErrorStr(e))
      return e;
   }

   // Filter packets
   if ( streamIndex != m_avPacket->stream_index )
   {
      return 0; // Skip
   };

   if ( !m_avCodecCtx )
   {
      DE_ERROR("No codec ctx" )
      return -1;
   }

   e = avcodec_send_packet( m_avCodecCtx, m_avPacket );
   if ( e < 0 )
   {
      DE_DEBUG("avcodec_send_packet = ",Utils::getErrorStr(e))
      return e;
   }

   // NOTE: Each packet may generate more than one frame, depending on the codec.
   // receive and handle frame, rereceive until no EAGAIN

   while ( e >= 0 )
   {
      e = avcodec_receive_frame( m_avCodecCtx, avFrame );
      if ( e == AVERROR(EAGAIN) || e == AVERROR_EOF )
      {
         break;
      }
      handleFrame( avFrame );
      av_frame_unref( avFrame );
   }

   av_packet_unref( m_avPacket );
   return e;
}

int32_t
Utils::encodeFrame(
   //AVFormatContext* formatCtx,
   AVFrame* avFrame,
   AVPacket* m_avPacket,
   AVCodecContext* m_avCodecCtx,
   //int streamIndex,
   std::function< void(uint8_t const *, uint64_t) > const & writePacket )
{
   // avFrame -> avCodecCtx ( send the frame for encoding )
   int e = avcodec_send_frame( m_avCodecCtx, avFrame );
   if (e < 0)
   {
      DE_ERROR("sending the frame to the encoder")
      return e;
   }

   // read all the available output packets (in general there may be any number of them
   while (e >= 0)
   {
      // avCodecCtx -> avPacket
      e = avcodec_receive_packet( m_avCodecCtx, m_avPacket );
      if (e == AVERROR(EAGAIN) || e == AVERROR_EOF)
      {
         break;
      }
      else if (e < 0)
      {
         DE_ERROR("Error encoding audio frame")
         break;
      }

      // avPacket -> file
      // fwrite( m_avPacket->data, 1, m_avPacket->size, output );
      writePacket( m_avPacket->data, m_avPacket->size );

      av_packet_unref( m_avPacket );
   }

   return e;
}


bool
Utils::extractImage(
   //AVFormatContext* formatCtx,
   AVFrame* avFrame,
//   AVCodecContext* m_avCodecCtx,
   de::Image & img,
   int dst_w,  // -1 for auto src_w, else scale to user_w
   int dst_h,  // -1 for auto src_h, else scale to user_h
   int scale_quality )
{
   // Get source image width,height
   if ( !avFrame ) { DE_ERROR("No avFrame" ) return false; }
   int src_w = avFrame->width;
   int src_h = avFrame->height;
   if ( src_w <= 0 ) { DE_ERROR("src_w <= 0" ) return false; }
   if ( src_h <= 0 ) { DE_ERROR("src_h <= 0" ) return false; }

   // This function output depends on given image and its format.
   // Only 2 formats are supported yet. RGB24 und RGBA32 wobei A immer 255 ist.
   if ( img.pixelFormat() != de::PixelFormat::RGB_24 &&
        img.pixelFormat() != de::PixelFormat::RGBA_32 )
   {
      DE_ERROR("Image must be RGB888 or RGBA8888 format." )
      return false;
   }

   // Preconditions:
//   if ( !formatCtx ) { throw std::runtime_error( "No formatCtx" ); }
//   if ( !m_avCodecCtx ) { DE_ERROR("No m_avCodecCtx" ) return false; }
//   if ( m_avCodecCtx->codec_type != AVMEDIA_TYPE_VIDEO ) { DE_ERROR("Not an image stream" ) return false; }

   // Get destination image width,height
   if ( dst_w < 0 ) dst_w = src_w;
   if ( dst_h < 0 ) dst_h = src_h;

   // Resize destination image, if needed, clear not needed.
   if ( img.w() != dst_w || img.h() != dst_h )
   {
      img.clear(); // Tell that we dont keep any data while resize.
      img.resize( dst_w, dst_h ); // should make resize more efficient
      // Calling .clear() does not change the color format.
      // The .resize() always computes bytecount using the set color format.
   }

   // Get color formats

   AVPixelFormat srcFormat = AVPixelFormat( avFrame->format );
   AVPixelFormat dstFormat = AV_PIX_FMT_RGB24; // i dont see any reason for an alpha channel
   if ( img.pixelFormat() == de::PixelFormat::RGBA_32 )
   {
      dstFormat = AV_PIX_FMT_RGBA;
   }

   // Init swScaler:
   SwsContext* m_swScaler = sws_getContext(
         src_w, src_h, srcFormat,  // srcFormat
         dst_w, dst_h, dstFormat,  // dstFormat
         scale_quality,       // flags e.g. scale quality
         nullptr,                // srcFilter
         nullptr,                // dstFilter
         nullptr   );            // double* param

   if ( !m_swScaler ) { DE_ERROR("No m_swScaler" ) return false; }

   // Scale decoded image into given output image
   uint8_t* dst[4] = { img.data(), nullptr, nullptr, nullptr };
   int dst_linesize[4] = { int( img.stride() ), 0, 0, 0 };
   int e = sws_scale( m_swScaler,        // ctx
              avFrame->data,     // srcSlice  - uint8_t[]
              avFrame->linesize, // srcStride - int[]
              0,                 // srcSliceY - int
              avFrame->height,   // srcSliceH - int
              dst,               // dstSlice  - uint8_t[]
              dst_linesize );    // dstStride - int[]

   sws_freeContext( m_swScaler );

   if ( e != 0 )
   {
      // DE_ERROR("Got swScaler error e = ",e)
   }
//   if ( m_avCodecCtx->pix_fmt == AV_PIX_FMT_RGB0 )
//   {

//      DE_DEBUG("Direct store")

//      size_t stride = avFrame->linesize[ streamIndex ];

//      for ( int y = 0; y < h; ++y )
//      {
//         uint8_t const* src = avFrame->data[streamIndexVideo] + (stride * uint32_t( y ));
//         for ( int x = 0; x < w; ++x )
//         {
//            uint32_t color = RGBA(
//               src[ 3*x ], src[ 3*x+1 ], src[ 3*x+2 ] );
//            img.setPixel( x,y, color );
//         }
//      }
//   }
    /// the picture is allocated by the decoder. no need to free it
   // snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
   // pgm_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf);
   return true;
}


/*
// Processes the sound
uint64_t
resampleImpl( SampleBuffer const & srcBuffer, SampleBuffer & dstBuffer )
{
   ESampleType srcType = srcBuffer.getSampleType();
   ESampleType dstType = dstBuffer.getSampleType();
   if ( srcType != ST_F32I )
   {
      DE_ERROR("Invalid srcType ", ST_toString(srcType))
      return 0;
   }

   if ( dstType != ST_F32I )
   {
      DE_ERROR("Set dstType(", ST_toString(dstType),") to dstType(ST_F32I)")
      dstType = ST_F32I;
      dstBuffer.setSampleType( ST_F32I );
   }

   int srcRate = srcBuffer.getSampleRate();
   int dstRate = dstBuffer.getSampleRate();
   m_srcRate = srcRate;
   m_dstRate = dstRate;

   m_swResampler = swr_alloc_set_opts( m_swResampler,
         m_dstLayout, // out_ch_layout
         m_dstType,   // out_sample_fmt
         m_dstRate,   // out_sample_rate
         m_srcLayout, // in_ch_layout
         m_srcType,   // in_sample_fmt
         m_srcRate,   // in_sample_rate
         0, nullptr ); // log_offset + log_ctx

   if ( !m_swResampler )
   {
      DE_ERROR("No m_swResampler")
      return 0;
   }

   //swr_init( m_swResampler );

   int64_t srcFrames = srcBuffer.getFrameCount();
   int64_t swrDelay = swr_get_delay( m_swResampler, srcRate );
   int64_t srcFrameDelay = swrDelay + srcFrames;
   int64_t dstFrames = av_rescale_rnd( srcFrameDelay, dstRate, srcRate, AV_ROUND_UP );

   DE_DEBUG("srcRate(",srcRate,"), srcFrames(",srcFrames,")")
   DE_DEBUG("dstRate(",dstRate,"), dstFrames(",dstFrames,")")
   DE_DEBUG("swrDelay(",swrDelay,")")
   DE_DEBUG("srcFrameDelay(",srcFrameDelay,")")

   dstBuffer.resize( dstFrames );

   uint8_t* dst = dstBuffer.data();
   uint8_t const* src = srcBuffer.data();
   int64_t doneFrames = swr_convert( m_swResampler, &dst,dstFrames, &src,srcFrames );

   if ( doneFrames != dstFrames )
   {
      DE_DEBUG("doneFrames(",doneFrames,") != dstFrames(",dstFrames,"), srcFrames(",srcFrames,")")
   }

   //uint8_t* output;
   //int dstSampleCount = av_rescale_rnd( swr_get_delay( swr, srcRate ) + sampleCount, dstRate, srcRate, AV_ROUND_UP );
   //outputVector.resize(
   //av_samples_alloc( &output, NULL, 2, dstSampleCount, AV_SAMPLE_FMT_S16, 0);
   //dstSampleCount = swr_convert(m_swResampler, &output, dstSampleCount, sampleVector.data(), sampleCount);

   // handle_output(output, dstSampleCount);

   //swr_close( m_swResampler );

   return doneFrames;
}
*/
uint64_t
Utils::extractAudioF32(
   AVFrame* avFrame,
   //AVCodecContext* m_avCodecCtx,
   de::audio::SampleBuffer & dst,
   int dst_sampleRate,
   SwrContext* & resampler )
{
   if ( !avFrame )
   {
      DE_ERROR("No avFrame")
      return 0;
   }

   auto srcLayout = avFrame->channel_layout; // in [ch]
   auto srcChannels = uint32_t( avFrame->channels ); // in [ch]
   auto srcFrames = uint32_t( avFrame->nb_samples ); // in [samples/ch]
   auto srcRate = int( avFrame->sample_rate ); // in [Hz] = [1/s]
   auto srcFormat = AVSampleFormat( avFrame->format );
   auto srcType = Utils::toESampleType( srcFormat );

   if ( srcChannels > 2 ) { DE_ERROR("Got avFrame->channels(",srcChannels,")") }

   auto dstType = de::audio::ST_F32I;
   int dstRate = srcRate;
   if ( dst_sampleRate > 0 ) dstRate = dst_sampleRate;

   if ( srcRate < 199 )
   {
      DE_ERROR("Got srcRate(",srcRate,"), dstRate(",dstRate,")")
      return 0;
   }

//   // Input samples chunk header (sampletyp,channels,rate,frames):
//   size_t sampleCount = srcFrames * srcChannels;
//   if ( sampleCount < 1 )
//   {
//      DE_ERROR("No samples")
//      return 0;
//   }

   if ( !resampler )
   {
      resampler = swr_alloc_set_opts( nullptr,
         srcLayout, AV_SAMPLE_FMT_FLT, dstRate, // out_sample_rate
         srcLayout, srcFormat, srcRate,   // in_sample_rate
         0, nullptr ); // log_offset + log_ctx

      int64_t delay = swr_get_delay( resampler, srcRate );
      int64_t frameDelay = delay + srcFrames;
      int64_t dstFrames = av_rescale_rnd( frameDelay, dstRate, srcRate, AV_ROUND_UP );

      dst.setFormat( dstType, srcChannels, dstRate );
      dst.resize( srcFrames );

      DE_DEBUG("Create Resampler:")
      DE_DEBUG("srcRate(",srcRate,"), srcFrames(",srcFrames,"), srcChannels(",srcChannels,")")
      DE_DEBUG("dstRate(",dstRate,"), dstFrames(",dstFrames,"), delay(",delay,"), frameDelay(",frameDelay,")")

      bool dst_isInterleaved = de::audio::ST_isInterleaved( dstType );
      bool src_isInterleaved = de::audio::ST_isInterleaved( srcType );
      if ( src_isInterleaved )
      {
         DE_DEBUG("Got INTERLEAVED audio")
      }
      else
      {
         DE_DEBUG("Got PLANAR audio")
      }
   }

   if ( !resampler )
   {
      DE_ERROR("No resampler")
      return 0;
   }

   //swr_init( resampler );

   int64_t delay = swr_get_delay( resampler, srcRate );
   int64_t frameDelay = delay + srcFrames;
   int64_t dstFrames = av_rescale_rnd( frameDelay, dstRate, srcRate, AV_ROUND_UP );
   uint8_t* pDst = dst.data();
   uint8_t const* pSrc = avFrame->extended_data[0];

   int64_t doneFrames = swr_convert( resampler, &pDst, dstFrames, &pSrc, srcFrames );
   if ( doneFrames != dstFrames )
   {
      DE_DEBUG("doneFrames(",doneFrames,") != dstFrames(",dstFrames,"), srcFrames(",srcFrames,")")
   }

   //uint8_t* output;
   //int dstSampleCount = av_rescale_rnd( swr_get_delay( swr, srcRate ) + sampleCount, dstRate, srcRate, AV_ROUND_UP );
   //outputVector.resize(
   //av_samples_alloc( &output, NULL, 2, dstSampleCount, AV_SAMPLE_FMT_S16, 0);
   //dstSampleCount = swr_convert(m_swResampler, &output, dstSampleCount, sampleVector.data(), sampleCount);

   // handle_output(output, dstSampleCount);


#if 0
   DE_DEBUG("FRAME cc(",srcChannels,"), "
                  "srcFrames(",srcFrames,"), "
                  "sampleCount(",sampleCount,")")
   DE_DEBUG("FRAME srcType(",de::audio::ST_toString(srcType),"), "
                  "dstType(",de::audio::ST_toString(dstType),")")



   de::audio::SampleTypeConverter::Converter_t converter =
      de::audio::SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("No converter srcType(",srcType,") to dstType(",dstType,")")
      return 0;
   }

   // Prepare output buffer, only convert sample type, interleave is done at the end.
   float sampleRate = float( avFrame->sample_rate ); // in [Hz] = [1/s]
   dst.setSampleType( dstType );
   dst.setChannelCount( srcChannels );
   dst.setSampleRate( sampleRate );
   dst.resize( srcFrames );
   //DE_DEBUG( "prepared SB :: ", dst.toString() )

   bool dst_isInterleaved = de::audio::ST_isInterleaved( dstType );
   bool src_isInterleaved = de::audio::ST_isInterleaved( srcType );
   if ( src_isInterleaved )
   {
      //DE_DEBUG("INTERLEAVED")
      uint8_t* pDst = dst.data();
      uint8_t const * pSrc = avFrame->extended_data[0];
      converter( pSrc, pDst, sampleCount );

      if ( dst_isInterleaved != src_isInterleaved )
      {
         //DE_DEBUG("Deinterleave from srcType(",srcType,") to dstType(",dstType,")")
         dst.interleave();
      }

   }
   else
   {
      //DE_DEBUG("PLANAR")
//      uint8_t* pDst = dst.data();
//      uint8_t const * pSrc = avFrame->extended_data[0];
//      converter( pSrc, pDst, sampleCount );

      size_t src_bps = de::audio::ST_getBytesPerSample( srcType );

      for ( size_t i = 0; i < srcFrames; ++i )
      {
         for ( size_t c = 0; c < srcChannels; ++c )
         {
            uint8_t const * p_src = reinterpret_cast< uint8_t const * >( avFrame->extended_data[ c ] ) + i * src_bps;
            uint8_t * p_dst = dst.getSamples( i, c ); // dst is already of dstType, no interleave needed.
            converter( p_src, p_dst, 1 );
         }
      }
   }
#endif

   return dstFrames;
}

uint64_t
Utils::extractAudio(
   AVFrame* avFrame,
   //AVCodecContext* m_avCodecCtx,
   de::audio::SampleBuffer & dst,
   de::audio::ESampleType dst_convert )
{
   if ( !avFrame )
   {
      DE_ERROR("No avFrame")
      return 0;
   }
   // Input samples chunk header (sampletyp,channels,rate,frames):
   uint32_t srcChannels = uint32_t( avFrame->channels ); // in [ch]
   if ( srcChannels > 2 )
   {
      DE_ERROR("Got avFrame->channels(",srcChannels,")")
   }

   uint32_t srcFrames = uint32_t( avFrame->nb_samples ); // in [samples/ch]
   size_t sampleCount = srcFrames * srcChannels;
   if ( sampleCount < 1 )
   {
      DE_ERROR("No samples")
      return 0;
   }

   AVSampleFormat srcFormat = AVSampleFormat( avFrame->format );
   de::audio::ESampleType srcType = Utils::toESampleType( srcFormat );
   de::audio::ESampleType dstType = srcType;
   if ( dst_convert != de::audio::ST_Unknown )
   {
      dstType = dst_convert;
   }

   if ( dstType == de::audio::ST_Unknown )
   {
      DE_ERROR("No dstType, srcType(",de::audio::ST_toString(srcType),")")
      return 0;
   }

#if 0
   DE_DEBUG("FRAME cc(",srcChannels,"), "
                  "srcFrames(",srcFrames,"), "
                  "sampleCount(",sampleCount,")")
   DE_DEBUG("FRAME srcType(",de::audio::ST_toString(srcType),"), "
                  "dstType(",de::audio::ST_toString(dstType),")")
#endif


   de::audio::SampleTypeConverter::Converter_t converter =
      de::audio::SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("No converter srcType(",srcType,") to dstType(",dstType,")")
      return 0;
   }

   // Prepare output buffer, only convert sample type, interleave is done at the end.
   float sampleRate = float( avFrame->sample_rate ); // in [Hz] = [1/s]
   dst.setSampleType( dstType );
   dst.setChannelCount( srcChannels );
   dst.setSampleRate( sampleRate );
   dst.resize( srcFrames );
   //DE_DEBUG( "prepared SB :: ", dst.toString() )

   bool dst_isInterleaved = de::audio::ST_isInterleaved( dstType );
   bool src_isInterleaved = de::audio::ST_isInterleaved( srcType );
   if ( src_isInterleaved )
   {
      //DE_DEBUG("INTERLEAVED")
      uint8_t* pDst = dst.data();
      uint8_t const * pSrc = avFrame->extended_data[0];
      converter( pSrc, pDst, sampleCount );

      if ( dst_isInterleaved != src_isInterleaved )
      {
         //DE_DEBUG("Deinterleave from srcType(",srcType,") to dstType(",dstType,")")
         dst.interleave();
      }

   }
   else
   {
      //DE_DEBUG("PLANAR")
//      uint8_t* pDst = dst.data();
//      uint8_t const * pSrc = avFrame->extended_data[0];
//      converter( pSrc, pDst, sampleCount );

      size_t src_bps = de::audio::ST_getBytesPerSample( srcType );

      for ( size_t i = 0; i < srcFrames; ++i )
      {
         for ( size_t c = 0; c < srcChannels; ++c )
         {
            uint8_t const * p_src = reinterpret_cast< uint8_t const * >( avFrame->extended_data[ c ] ) + i * src_bps;
            uint8_t * p_dst = dst.getSamples( i, c ); // dst is already of dstType, no interleave needed.
            converter( p_src, p_dst, 1 );
         }
      }
   }

   return sampleCount;
}


/*
bool
MediaStream::dumpImages( std::string uri, std::string baseDir )
{
   std::string baseUri = baseDir + "/" + dbGetFileBaseName( uri ) + "_";
   DE_DEBUG("uri = ", uri, ", baseUri(", baseUri, ")")

   //av_read_frame( formatCtx, m_avPacket );
   int64_t tsStart = formatCtx->start_time;
   int64_t tsDuration = formatCtx->duration;

   if ( av_seek_frame( formatCtx, streamIndexVideo, tsStart, AVSEEK_FLAG_ANY ) < 0 )
   {
      DE_ERROR("Seek 0 failed")
      return false;
   }

   size_t srcFrames = 0;

   Image img( m_Width, m_Height );

   while ( decodeImage( img ) )
   {
      DE_DEBUG("WriteFrame[", srcFrames, "] " )

      std::stringstream s;
      s << baseUri << srcFrames << ".bmp";
      if ( !dbSaveImage( img, s.str() ) )
      {
         DE_ERROR("")
      }

      srcFrames++;
   }

   DE_DEBUG("Saved Frames = ", srcFrames )

   return true;
}
*/



} // end namespace ffmpeg
} // end namespace de
