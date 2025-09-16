#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <de/Core.h>

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

// ===========================================================================
struct DecoderTools
// ===========================================================================
{
   DE_CREATE_LOGGER("DecoderTools")

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

} // end namespace decoder
} // end namespace audio
} // end namespace de
