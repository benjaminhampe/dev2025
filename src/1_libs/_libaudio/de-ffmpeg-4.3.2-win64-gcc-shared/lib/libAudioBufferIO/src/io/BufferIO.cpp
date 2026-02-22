#include <de/audio/buffer/BufferIO.hpp>
#include <de/audio/buffer/io/BufferIO_WAV.hpp>
#include <de/audio/buffer/io/BufferIO_FFMPEG.hpp>
#include <de/audio/buffer/io/BufferIO_LAME.hpp>
#include <de/audio/buffer/io/BufferIO_SND.hpp>

namespace de {
namespace audio {

bool
BufferIO::load( Buffer & dst, std::string uri, int streamIndex,
                        ESampleType convertType, int sampleRate )
{
   PerformanceTimer perf;
   perf.start();

   bool ok = false;

//   if ( uri.empty() )
//   {
//      DE_ERROR("Empty uri, ", uri )
//      return ok;
//   }

//   std::string suffix = FileSystem::fileSuffix( uri );
//   if ( suffix.empty() )
//   {
//      DE_ERROR("Empty suffix, ", uri )
//      return ok;
//   }

//   if ( suffix == "wav" )
//   {
//      try
//      {
//         ok = io::BufferWAV::load( dst, uri );
//      }
//      catch ( std::exception & e ) // Input file is corrupt or not a WAV file
//      {
//         DE_ERROR("BufferWAV::load() got exception ",e.what()," for uri ", uri )
//         ok = false;
//      }
//   }

//#ifdef USE_LIBSNDFILE
//   if ( !ok )
//   {
//      ok = io::BufferSND::load( dst, uri );
//   }
//#endif

   if ( !ok )
   {
      ok = io::BufferFFMPEG::load( dst, uri, streamIndex, convertType, sampleRate );
   }

   perf.stop();

   if ( ok )
   {
      //DE_DEBUG("[Load] time(", perf.ms(),"ms), uri(", uri, "), dst(", dst.toString(),")" )
   }
   else
   {
      //DE_ERROR("[Load] time(", perf.ms(),"ms), uri(", uri, "), dst(", dst.toString(),")" )
   }

   return ok;
}

bool
BufferIO::save( Buffer const & src, std::string uri )
{
   PerformanceTimer perf;
   perf.start();

   bool ok = false;

   if ( uri.empty() )
   {
      DE_ERROR("Empty uri, ", uri )
      return ok;
   }

   std::string suffix = FileSystem::fileSuffix( uri );
   if ( suffix.empty() )
   {
      DE_ERROR("Empty suffix, ", uri )
      return ok;
   }

   if ( suffix == "wav" )
   {
      ok = io::BufferWAV::save( src, uri );
   }

#ifdef USE_LIBSNDFILE

   if ( !ok || suffix == "ogg" || suffix == "flac" )
   {
      ok = io::BufferSND::save( src, uri );
   }

#endif

   if ( !ok || suffix == "mp3" )
   {
      ok = io::BufferLAME::save( src, uri );
   }

   if ( !ok )
   {
      ok = io::BufferFFMPEG::save( src, uri );
   }

   perf.stop();

   if ( ok )
   {
      DE_DEBUG("[Save] time(", perf.ms(),"ms), uri(", uri, "), src(", src.toString(),")" )
   }
   else
   {
      DE_ERROR("[Save] time(", perf.ms(),"ms), uri(", uri, "), src(", src.toString(),")" )
   }

   return ok;
}




} // end namespace audio
} // end namespace de











