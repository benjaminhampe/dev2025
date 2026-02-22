#include <dbConvertToMP3.h>
// #include <de/Core.h>
// #include <de/os/CommonDialogs.h>

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
//#include <de/Core.h>

#include <DarkImage.h>

#include <dbConvertToMP3_lame.h>

#include <EncoderLAME.hpp>

#if 1

#ifdef __cplusplus
extern "C"
{
#endif

   #include <libavcodec/avcodec.h>
   #include <libavformat/avformat.h>
   #include <libavutil/avutil.h>
   #include <libavutil/opt.h>
   #include <libavutil/channel_layout.h>
   #include <libavutil/common.h>
   #include <libavutil/frame.h>
   #include <libavutil/imgutils.h>
   #include <libavutil/mathematics.h>
   #include <libavutil/samplefmt.h>
   #include <libavutil/mem.h>
   #include <libswscale/swscale.h>
   #include <libswresample/swresample.h>
   #include <inttypes.h>

#ifdef __cplusplus
}
#endif

namespace de {
namespace audio {
namespace decoder {

// Payload is always float 32 bit
// Payload is always interleaved channels
struct AudioPayload
{
   double pts;
   uint64_t frameCount = 0;
   uint32_t channelCount = 0;
   uint32_t sampleRate = 0;
   std::vector< float > samples;

   uint64_t byteCount() const { return samples.size() * sizeof(float); }

   double duration() const {
      if ( sampleRate < 1 ) return 0.0;
      return double( frameCount ) / double( sampleRate );
   }

   std::string toString() const
   {
      std::stringstream s;
      s << ", pts:" << pts
      << "cc:" << channelCount
         << ", fc:" << frameCount
         << ", sr:" << sampleRate
         << ", samples:" << samples.size();
      return s.str();
   }
};

typedef std::function< void(AudioPayload const &) > ON_AUDIO_PAYLOAD;

// ===========================================================================
struct DecoderTools
// ===========================================================================
{
   static double
   toDouble( AVRational const & r );

   // Extract a single sample and convert to float.
   static float
   getSample( AVSampleFormat const & fmt, uint8_t const* src, uint32_t srcBPS );

   static std::string
   getErrorStr( int errnum );

   static bool
   isInterleaved( AVSampleFormat fmt );

   static uint64_t
   extractAudio( AVFrame* avFrame, AudioPayload & payload, bool debug = false );
};

struct DecoderCtxTools
{
   static void
   initDecoder( DecoderCtx & ds );

   static void
   closeDecoder( DecoderCtx & ds );

   static bool
   openDecoder( DecoderCtx & ds, std::string uri, int streamIndex = -1, bool debug = false );

   static int32_t
   readFrame( DecoderCtx & ds, ON_AUDIO_PAYLOAD const & onPayload );
};

/*
// ===========================================================================
struct DecoderCtx
// ===========================================================================
{
   AVFormatContext* formatCtx;
   AVPacket* packet;
   AVFrame* frame;
   bool isOpen;
   bool isDebug;
   bool isSeekable;
   bool hasFrameCount;
   bool hasDuration;
   std::string uri;

   int streamIndex;
   int64_t durationTicks;
   AVRational timeBase;
   AVStream* stream;
   AVCodecParameters* codecParams;
   AVCodecID codecId;
   AVCodec* codec;
   AVCodecContext* codecCtx;
   double position;
   double duration;

   uint32_t channelCount;
   uint32_t sampleRate;
   uint64_t frameCount;    // Can be 0 for bad files

   SwrContext* swResampler;
};
*/

struct DecoderFFMPEG
{
   // DecoderCtxImpl* _d;
   AVFormatContext* formatCtx;
   AVPacket* packet;
   AVFrame* frame;
   AVRational timeBase;
   AVStream* stream;
   AVCodecParameters* codecParams;
   AVCodecID codecId;
   AVCodec* codec;
   AVCodecContext* codecCtx;
   SwrContext* swResampler;

   int streamIndex;
   int64_t durationTicks;

   double position;
   double duration;

   uint32_t channelCount;
   uint32_t sampleRate;
   uint64_t frameCount;    // Can be 0 for bad files

   bool isOpen;
   bool isDebug;
   bool isSeekable;
   bool hasFrameCount;
   bool hasDuration;
   std::string uri;
   
   //DecoderCtx m_data;

   DecoderFFMPEG() { DecoderTools::initDecoder( m_data ); }
   ~DecoderFFMPEG() { close(); }

   bool
   open( std::string uri, int streamIndex = -1, bool debug = false )
   {
      return DecoderTools::openDecoder( m_data, uri, streamIndex, debug );
   }

   void
   close() { DecoderTools::closeDecoder( m_data ); }

   bool
   is_open() const { return m_data.isOpen; }

   bool
   is_seekable() const { return m_data.isSeekable; }

   bool
   hasDuration() const { return m_data.hasDuration; }
   double
   getDuration() const { return m_data.hasDuration; }

   std::string
   getUri() const { return m_data.uri; }

   int32_t
   readFrame( ON_AUDIO_PAYLOAD const & handlePayload )
   {
      return DecoderTools::readFrame( m_data, handlePayload );
   }

//   double getPosition() const;
//   void setPosition( double positionInSeconds );

//   bool getCoverArt( Image & img );
//   void fillCache();

//protected:
//   void fillCacheUnguarded();
   
};


/*
// ===========================================================================
struct DecoderTools
// ===========================================================================
{
   static double
   toDouble( AVRational const & r );

   // Extract a single sample and convert to float.
   static float
   getSample( AVSampleFormat const & fmt, uint8_t const* src, uint32_t srcBPS );

   static std::string
   getErrorStr( int errnum );

   static bool
   isInterleaved( AVSampleFormat fmt );

   static void
   initDecoder( DecoderCtx & ds );

   static void
   closeDecoder( DecoderCtx & ds );

   static bool
   openDecoder( DecoderCtx & ds, std::string uri, int streamIndex = -1, bool debug = false );

   static uint64_t
   extractAudio( AVFrame* avFrame, AudioPayload & payload, bool debug = false );

   static int32_t
   readFrame( DecoderCtx & ds, ON_AUDIO_PAYLOAD const & onPayload );
};

// ===========================================================================
struct DecoderFFMPEG
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.DecoderFFMPEG")
   DecoderCtx m_data;

   DecoderFFMPEG() { DecoderTools::initDecoder( m_data ); }
   ~DecoderFFMPEG() { close(); }

   bool
   open( std::string uri, int streamIndex = -1, bool debug = false )
   {
      return DecoderTools::openDecoder( m_data, uri, streamIndex, debug );
   }

   void
   close() { DecoderTools::closeDecoder( m_data ); }

   bool
   is_open() const { return m_data.isOpen; }

   bool
   is_seekable() const { return m_data.isSeekable; }

   bool
   hasDuration() const { return m_data.hasDuration; }
   double
   getDuration() const { return m_data.hasDuration; }

   std::string
   getUri() const { return m_data.uri; }

   int32_t
   readFrame( ON_AUDIO_PAYLOAD const & handlePayload )
   {
      return DecoderTools::readFrame( m_data, handlePayload );
   }

//   double getPosition() const;
//   void setPosition( double positionInSeconds );

//   bool getCoverArt( Image & img );
//   void fillCache();

//protected:
//   void fillCacheUnguarded();
};

*/

} // end namespace decoder
} // end namespace audio
} // end namespace de



namespace de {
namespace audio {
namespace decoder {



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

    
// ===========================================================================
struct DecoderFFMPEG
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.DecoderFFMPEG")
   DecoderCtx m_data;

   DecoderFFMPEG() { DecoderTools::initDecoder( m_data ); }
   ~DecoderFFMPEG() { close(); }

   bool
   open( std::string uri, int streamIndex = -1, bool debug = false )
   {
      return DecoderTools::openDecoder( m_data, uri, streamIndex, debug );
   }

   void
   close() { DecoderTools::closeDecoder( m_data ); }

   bool
   is_open() const { return m_data.isOpen; }

   bool
   is_seekable() const { return m_data.isSeekable; }

   bool
   hasDuration() const { return m_data.hasDuration; }
   double
   getDuration() const { return m_data.hasDuration; }

   std::string
   getUri() const { return m_data.uri; }

   int32_t
   readFrame( ON_AUDIO_PAYLOAD const & handlePayload )
   {
      return DecoderTools::readFrame( m_data, handlePayload );
   }

//   double getPosition() const;
//   void setPosition( double positionInSeconds );

//   bool getCoverArt( Image & img );
//   void fillCache();

//protected:
//   void fillCacheUnguarded();
};

} // end namespace decoder
} // end namespace audio
} // end namespace de

*/



