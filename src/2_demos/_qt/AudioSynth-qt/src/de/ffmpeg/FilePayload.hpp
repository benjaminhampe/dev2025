#pragma once
#include <de/ffmpeg/Utils.hpp>

namespace de {
namespace ffmpeg {

// ============================================================
struct FilePayload
// ============================================================
{
   typedef std::unique_ptr< FilePayload > UniquePtr;
   typedef std::shared_ptr< FilePayload > SharedPtr;
   AVMediaType typ;
   int streamIndex;
   AVRational time_base;
   double benni_pts;
   int64_t pts;
   int64_t dts;
   int64_t duration;
   //std::shared_ptr< de::Image > img;
   //std::shared_ptr< de::audio::Buffer > buf;
   std::unique_ptr< de::Image > img;
   std::unique_ptr< de::audio::SampleBuffer > buf;

   FilePayload()
      : typ(AVMEDIA_TYPE_UNKNOWN), streamIndex(-1), benni_pts(0.0), pts(0), dts(0), duration(0)
   {
      time_base.num = time_base.den = 1;
   }
   bool isAudio() const { return buf != nullptr; }
   bool isVideo() const { return img != nullptr; }
   double getDuration() const { return Utils::toDouble( time_base ) * duration; }
   double getPTS() const { return Utils::toDouble( time_base ) * pts; }

   std::string toString() const
   {
      std::stringstream s;
      if ( isAudio() ) { s << "AUDIO, "; }
      if ( isVideo() ) { s << "VIDEO, "; }
      s << "bts(" << benni_pts << "), ";
      s << "tb(" << Utils::toString( time_base ) << "), ";
      if ( pts == AV_NOPTS_VALUE )
      {
         s << "pts(AV_NOPTS_VALUE), ";
      }
      else
      {
         //auto t = dbStrSeconds( Utils::toDouble( time_base ) * pts );
         s << "pts(" << pts << "), ";
         //<< " = " << t << "), ";
      }
      if ( dts == AV_NOPTS_VALUE )
      {
         s << "dts(AV_NOPTS_VALUE), ";
      }
      else
      {
         //auto t = dbStrSeconds( Utils::toDouble( time_base ) * dts );
         s << "dts(" << dts << "), ";
         //<< " = " << t << "), ";
      }
      if ( duration == AV_NOPTS_VALUE )
      {
         s << "dur(AV_NOPTS_VALUE)";
      }
      else
      {
         //auto t = dbStrSeconds( Utils::toDouble( time_base ) * duration );
         s << "dur(" << duration << ")";
         //<< " = " << t << ")";
      }
      return s.str();
   }
};

} // end namespace ffmpeg
} // end namespace de

