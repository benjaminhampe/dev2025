#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <de_libavcodec.hpp>
#include <DarkImage.h>
#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace ffmpeg {

struct Utils
{
   DE_CREATE_LOGGER("Utils")

   static bool
   isStream( AVFormatContext const * m_avFormatCtx, int i );

   static bool
   hasDuration( AVFormatContext* formatCtx, int streamIndex );

   static double
   getDuration( AVFormatContext* formatCtx, int streamIndex );

   static double
   getDuration( AVFormatContext* formatCtx );

   static bool
   seek(
      AVFormatContext* m_avFormatCtx,
      int streamIndex,
      double timeInSec,
      bool backward = false );

   static bool
   seekTimeStamp(
      AVFormatContext* m_avFormatCtx,
      int streamIndex,
      int64_t ts,
      bool backward = false );

   static bool
   seekPercent(
      AVFormatContext* m_avFormatCtx,
      int streamIndex,
      double pc );

   // check that a given sample format is supported by the encoder
   static bool
   hasAVSampleFormat( AVCodec const * codec, AVSampleFormat searchToken );

   // just pick the highest supported samplerate
   static int
   getBestSampleRate( AVCodec const * codec );

   // select layout with the highest channel count
   static int
   getChannelCountFromLayout( uint64_t channel_layout );

   // select layout with the highest channel count
   static int
   getBestChannelLayout( AVCodec const * codec);

   static int32_t
   findBestStream( AVFormatContext * formatCtx, AVMediaType mediaType );

   static int32_t
   findBestAudio( AVFormatContext * formatCtx );

   static int32_t
   findBestVideo( AVFormatContext * formatCtx );

   static double
   toDouble( AVRational const & r );

   static std::string
   toString( AVRational const & r );

   static std::string
   getErrorStr( int errnum );

   static de::audio::ESampleType
   toESampleType( AVSampleFormat sample_fmt );

   static AVSampleFormat
   toAVSampleFormat( de::audio::ESampleType sampleType );

   static std::string
   toString( AVSampleFormat sampleFormat );

   static std::string
   toString( AVPixelFormat pixelFormat );

   static std::string
   toString( AVMediaType mediaType );

   // Logs infos to console and possibly a file, duration not guaranteed without a full scan
   static void
   dumpStreamInfos( AVFormatContext * file, bool dumpInfoFilePerStream = true );

   static de::PixelFormat
   toEColorFormat( AVPixelFormat pixelFormat );



//   static bool
//   extractFirstImage(
//      AVFormatContext* m_avFormatCtx,
//      AVFrame* m_avFrame,
//      AVPacket* m_avPacket,
//      AVCodecContext* m_avCodecCtx,
//      int streamIndex,
//      de::Image & img,
//      int dst_w = -1,  // -1 for auto src_w, else scale to user_w
//      int dst_h = -1,  // -1 for auto src_h, else scale to user_h
//      int scale_quality = SWS_BILINEAR );

   static int32_t
   decodeFrame(
      AVFormatContext* m_avFormatCtx,
      AVFrame* m_avFrame,
      AVPacket* m_avPacket,
      AVCodecContext* m_avCodecCtx,
      int streamIndex,
      std::function< void(AVFrame*) > const & handleFrame = [](AVFrame*){} );

   static int32_t
   encodeFrame(
      //AVFormatContext* m_avFormatCtx,
      AVFrame* m_avFrame,
      AVPacket* m_avPacket,
      AVCodecContext* m_avCodecCtx,
      //int streamIndex,
      std::function< void(uint8_t const *, uint64_t) > const & writePacket =
                       [](uint8_t const *, uint64_t ){} );

   static bool
   extractImage(
      // AVFormatContext* m_avFormatCtx,
      AVFrame* m_avFrame,
      //AVCodecContext* m_avCodecCtx,
      de::Image & img,
      int dst_w = -1,  // -1 for auto src_w, else scale to user_w
      int dst_h = -1,  // -1 for auto src_h, else scale to user_h
      int scale_quality = SWS_BILINEAR );

   static uint64_t
   extractAudio(
      AVFrame* m_avFrame,
      //AVCodecContext* m_avCodecCtx,
      de::audio::SampleBuffer & dst,
      de::audio::ESampleType dst_convert = de::audio::ST_Unknown );


   static uint64_t
   extractAudioF32(
      AVFrame* m_avFrame,
      //AVCodecContext* m_avCodecCtx,
      de::audio::SampleBuffer & dst,
      int dst_sampleRate,
      SwrContext* & resampler );
};

} // end namespace ffmpeg
} // end namespace de

