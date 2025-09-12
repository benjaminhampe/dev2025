#include <de/audio/buffer/io/BufferIO_SND.hpp>

#ifdef USE_LIBSNDFILE

#include "de_libsndfile.hpp"

namespace de {
namespace audio {
namespace io {

bool
BufferSND::load( Buffer & dst, std::string uri )
{
   // Check file-extension
   auto ext = dbGetFileSuffix( uri );
   int sndFmt = SNDFILE_Utils::getSndFormatFromFileExt( ext );
   if (sndFmt == -1)
   {
      DE_ERROR("Unsupported audio type for uri ", uri )
      return false;
   }

   SF_INFO sndInfo = {0};
   sndInfo.format = 0;

   SNDFILE* file = sf_open( uri.c_str(), SFM_READ, &sndInfo );
   if ( !file )
   {
      DE_ERROR("Cant open uri ", uri )
      return false;
   }

   DE_DEBUG("Loading uri ", uri )
   DE_DEBUG("Format = ",sndInfo.format );
   DE_DEBUG("Channels = ",sndInfo.channels );
   DE_DEBUG("Frames = ",sndInfo.frames );
   DE_DEBUG("SampleRate = ",sndInfo.samplerate );
   DE_DEBUG("Sections = ",sndInfo.sections );
   DE_DEBUG("Seekable = ",sndInfo.seekable );

   // Prepare: DestinationBuffer
   dst.setFormat( ST_F32I, sndInfo.channels, sndInfo.samplerate );
   dst.resize( uint64_t(sndInfo.frames) );
   auto dstPtr = reinterpret_cast< float* >( dst.data() );

   // Prepare: OneFrameBuffer ( intermediate between File and Dst )
   std::vector< uint8_t > frameBuffer( sizeof( float ) * sndInfo.channels, 0x00 );
   float* src = reinterpret_cast< float* >( frameBuffer.data() );

   // Read frame per frame ( a frame can have multiple channels )
   for ( size_t i = 0; i < sndInfo.frames; ++i )
   {
      // Write File -> FrameBuffer
      sf_seek( file, i, SEEK_CUR );
      sf_read_float( file, src, sf_count_t(1) );

      // Write FrameBuffer -> DestinationBuffer ( multiple channels )
      auto srcPtr = src;
      for ( size_t c = 0; c < sndInfo.channels; ++c )
      {
         *dstPtr = *srcPtr; srcPtr++; dstPtr++;
      }
   }

   sf_close( file );
   return true;
}

bool
BufferSND::save( Buffer const & src, std::string uri )
{
   SF_INFO header;
   ::memset( &header, 0, sizeof(SF_INFO) );
   header.frames = sf_count_t( src.getFrameCount() );
   header.channels = int( src.getChannelCount() );
   header.samplerate = int( src.getSampleRate() );
   header.format = SF_ENDIAN_CPU | SF_FORMAT_WAV | SF_FORMAT_PCM_16;
   header.sections = 0;
   header.seekable = 1;

   SNDFILE* file = sf_open( uri.c_str(), SFM_WRITE, &header );
   if ( !file )
   {
      DE_ERROR("Cant open wav 16bit stereo")
      return false;
   }

   // Write samples (frames x 2 channels)
   for ( uint64_t i = 0; i < uint64_t( header.frames ); ++i )
   {
      //sf_seek( file, i, SEEK_CUR );
      int16_t L,R;
      src.getSample( i, 0, &L, ST_S16 ); // interleaved or planar not vip using getSample() getter.
      src.getSample( i, 1, &R, ST_S16 ); // interleaved or planar not vip using getSample() getter.
      sf_write_short( file, &L, sf_count_t( 1 ) );
      sf_write_short( file, &R, sf_count_t( 1 ) );
   }

   sf_close( file );
   return true;
}


} // end namespace io
} // end namespace audio
} // end namespace de


#endif // 0










