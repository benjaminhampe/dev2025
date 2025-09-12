#if 0

#ifndef DE_VIDEO_DECODER_FFMPEG_HPP
#define DE_VIDEO_DECODER_FFMPEG_HPP


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

#include <de/Logger.hpp>
#include <de/Image.hpp>
#include <de/image/io/ImageIO.hpp>
//#include <QImage>

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

// Payload is always float 32 bit
// Payload is always interleaved channels
struct ImagePayload
{
   double pts;
   uint64_t frameCount;
   uint32_t channelCount;
   uint32_t sampleRate;
   std::shared_ptr< de::Image > img;

   ImagePayload()
      : pts( 0.0 )
      , frameCount( 0 )
      , channelCount( 0 )
      , sampleRate( 0 )
   {}
//   uint64_t byteCount() const { return img.sizeInBytes(); }
//   double duration() const { return 0.0; }

   // std::string toString() const
   // {
      // std::stringstream s;
      // s << ", pts:" << pts
      // << "cc:" << channelCount
         // << ", fc:" << frameCount
         // << ", sr:" << sampleRate
         // << ", samples:" << samples.size();
      // return s.str();
   // }
};

typedef std::function< void(ImagePayload) > ON_IMAGE_PAYLOAD;


// ===========================================================================
struct VideoDecoderCtx
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
   std::wstring wri;

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

   SwsContext* swScaler;
};

// ===========================================================================
struct VideoDecoderTools
// ===========================================================================
{
   DE_CREATE_LOGGER("VideoDecoderTools")

   static double
   toDouble( AVRational const & r );

   static std::string
   getErrorStr( int errnum );

   static void
   initDecoder( VideoDecoderCtx & ds );

   static void
   closeDecoder( VideoDecoderCtx & ds );

   static bool
   openDecoder( VideoDecoderCtx & ds, std::string uri, int streamIndex = -1, bool debug = false );

   static bool
   openDecoderW( VideoDecoderCtx & ds, std::wstring uri, int streamIndex = -1, bool debug = false );

   //int dst_w = -1,  // -1 for auto src_w, else scale to user_w
   //int dst_h = -1,  // -1 for auto src_h, else scale to user_h
   //int scale_quality = SWS_BILINEAR );

   static bool
   extractImage( AVFrame* avFrame, int streamIndex, ImagePayload & payload, bool debug );

   static int32_t
   readFrame( VideoDecoderCtx & ds, ON_IMAGE_PAYLOAD const & onPayload, bool debug );

   static std::string
   toString( AVPixelFormat pixelFormat )
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
};

// ===========================================================================
struct VideoFile
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.VideoFile")
   VideoDecoderCtx m_data;

   VideoFile() { VideoDecoderTools::initDecoder( m_data ); }
   ~VideoFile() { close(); }

   bool
   open( std::string uri, int streamIndex = -1, bool debug = false )
   {
      return VideoDecoderTools::openDecoder( m_data, uri, streamIndex, debug );
   }

   bool
   open( std::wstring uri, int streamIndex = -1, bool debug = false )
   {
      return VideoDecoderTools::openDecoderW( m_data, uri, streamIndex, debug );
   }

   void
   close() { VideoDecoderTools::closeDecoder( m_data ); }

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
   readFrame( ON_IMAGE_PAYLOAD const & handlePayload, bool debug = false )
   {
      return VideoDecoderTools::readFrame( m_data, handlePayload, debug );
   }

//   double getPosition() const;
//   void setPosition( double positionInSeconds );

//   bool getCoverArt( Image & img );
//   void fillCache();

//protected:
//   void fillCacheUnguarded();
};

inline std::string
repairString( std::wstring loadUri )
{
   //std::wstring w = dbStrW( loadUri );
   //std::wcout << "LoadUri = " << w << L'\n';

   std::stringstream s;
   for ( size_t i = 0; i < loadUri.size(); ++i )
   {
      uint32_t c = loadUri[ i ];
      if ( c <= 30 ) c = '_';
      if ( c > 'z' ) c = '_';
      s << char( c );
   }

   std::string r = s.str();
   std::cout << "RepairedUri = " << r << std::endl;
   return r;

}

inline std::shared_ptr< de::Image >
getCoverArtW( std::wstring uri, int streamIndex = -1, bool debug = false )
{
   VideoFile file;
   if ( !file.open( uri, streamIndex, debug ) )
   {
      return nullptr;
   }

   bool foundCoverArt = false;

   int loopCounter = 0;

   std::shared_ptr< de::Image > img;

   auto handlePayload = [&]( ImagePayload payload )
                        {
                           if ( payload.img && !payload.img->empty() )
                           {
                              foundCoverArt = true;
                              img = payload.img;
                              loopCounter++;
                           }
                        };

   while ( !foundCoverArt )
   {
      int e = file.readFrame( handlePayload );
      if ( e == AVERROR_EOF )
      {
         break;
      }
   }

   file.close();

//   if ( img && foundCoverArt )
//   {
//      //DEM_DEBUG("Loaded CoverArt img(",img->toString(),") after ",loopCounter," loops. ", uri )
//      dbSaveImage( *img, repairString(uri) + ".coverArt.png" );
//   }
//   else
//   {
//      //DEM_ERROR("No CoverArt after ",loopCounter," loops. ", uri )
//   }

   DE_FLUSH
   return img;
}

inline std::shared_ptr< de::Image >
getCoverArt( std::string uri, int streamIndex = -1, bool debug = false )
{
   std::shared_ptr< de::Image > img( nullptr );

   try
   {

      VideoFile file;
      if ( !file.open( uri, streamIndex, debug ) )
      {
         return nullptr;
      }

      auto handlePayload = [&]( ImagePayload payload )
                           {
                              if ( payload.img && !payload.img->empty() )
                              {
                                 img = payload.img;
                              }
                           };

      int loopCounter = 0;
      while ( !img && loopCounter < 30000 )
      {
         int e = file.readFrame( handlePayload );
         if ( e == AVERROR_EOF )
         {
            break;
         }
         loopCounter++;
      }

      file.close();

      if ( img )
      {
         DEM_DEBUG("Loaded CoverArt img(",img->toString(),") after ",loopCounter," loops."  )
         //dbSaveImage( *img, uri + ".coverArt.png" );
         DE_FLUSH
      }
      else
      {
         DEM_ERROR("No CoverArt after ",loopCounter," loops." )
         DE_FLUSH
      }

   }
   catch (...)
   {

   }


   return img;
}

#endif

#endif // 0
