#include <de/audio/buffer/io/BufferIO_LAME.hpp>

extern "C"
{
   #include <lame/lame.h>
}

namespace de {
namespace audio {
namespace io {

bool
BufferLAME::save( Buffer const & src, std::string uri, SB_SaveOptions const & options )
{
   lame_global_flags* ctx = lame_init();
   uint32_t bitRate = 128; // 192; // obviously in kbit/sec
   uint32_t channelCount = 2;
   uint32_t sampleRate = uint32_t( src.getSampleRate() );

   int64_t frameCount = 0;
   if ( src.getFrameCount() > 0xFFFFFFFF )
   {
      frameCount = 0xFFFFFFFF;
      DE_WARN("Limit frameCount from(",src.getFrameCount(),") to(",frameCount,")")
   }
   else
   {
      frameCount = src.getFrameCount();
   }

   lame_set_num_channels( ctx, channelCount );
   lame_set_num_samples( ctx, frameCount );
   lame_set_in_samplerate( ctx, sampleRate );
   lame_set_out_samplerate( ctx, sampleRate );
   //lame_set_brate( ctx, bitRate );
   lame_set_mode( ctx, STEREO );

   lame_set_disable_reservoir( ctx, 1 ); //bit reservoir has to be disabled for seamless streaming
   lame_set_VBR( ctx, vbr_off );
   lame_set_brate( ctx, bitRate );

   //lame_set_VBR( ctx, vbr_default );
   //lame_set_VBR_min_bitrate_kbps( ctx, 64 );
   //lame_set_VBR_max_bitrate_kbps( ctx, 192 );

   //lame_set_quality( ctx, 3 ); // 0-best, 9-worst

   lame_set_bWriteVbrTag( ctx, 1 );
   //lame_set_asm_optimizations()

   if ( -1 == lame_init_params( ctx ) )
   {
      DE_ERROR("-1")
      return false;
   }

   int64_t chunkFrames = lame_get_framesize( ctx );
   int64_t chunkSamples = chunkFrames * channelCount;
   int64_t chunkBytes = chunkSamples * 4;

   if ( options.debug )
   {
      DE_DEBUG("src(",src.toString(),")" )
      DE_DEBUG("bitRate(",bitRate,")" )
      DE_DEBUG("sampleRate(",sampleRate,")" )
      DE_DEBUG("frameCount(",frameCount,")" )
      DE_DEBUG("chunkFrames(",chunkFrames,")" )
      DE_DEBUG("chunkSamples(",chunkSamples,")" )
      DE_DEBUG("chunkBytes(",chunkBytes,")" )
   }

   FILE* file = fopen( uri.c_str(), "wb");
   if ( !file )
   {
      DE_ERROR("Cant save ", uri )
      return false;
   }

   // MPEG1:
   //    num_samples*(bitrate/8)/samplerate + 4*1152*(bitrate/8)/samplerate + 512
   // MPEG2:
   //    num_samples*(bitrate/8)/samplerate + 4*576*(bitrate/8)/samplerate + 256
//   double f = double(bitRate >> 3) / double(sampleRate);
//   std::vector< float > ch0( chunkFrames, 0.0f );
//   std::vector< float > ch1( chunkFrames, 0.0f );
//   DE_DEBUG("f(",f,")" )
//   DE_FLUSH

   Buffer convBuf( ST_F32I, 2, src.getSampleRate(), chunkFrames );

   //Buffer tmp( ST_F32I, 2, src.getSampleRate(), chunkFrames );
   std::vector< uint8_t > fileBuf( 4 * 2 * chunkFrames, 0x00 );
   //std::vector< uint8_t > mp3( 4 * 2 * chunkFrames, 0x00 );

   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( src.getSampleType(), ST_F32I );
   if ( !converter ) { DE_ERROR("No converter") return false; }

   int64_t frameIndex = 0;
   int64_t chunkIndex = 0;
   int64_t filebIndex = 0;
   int64_t filecIndex = 0;
   while ( 1 )
   {
      //DE_ERROR("chunk(",chunkIndex,"), frameIndex(",frameIndex,"), frameCount(",frameCount,")" )
      //DE_FLUSH

      if ( frameIndex + chunkFrames >= frameCount )
      {
         uint64_t availFrames = frameCount - frameIndex;

         auto dst = reinterpret_cast< float* >( convBuf.data() );
         //auto bps = convBuf.getBytesPerSample();

         for ( uint64_t i = 0; i < availFrames; ++i )
         {
            for ( uint32_t c = 0; c < 2; ++c )
            {
               float sample = 0.0f;
               src.getSample( frameIndex + i, c, (void*)&sample, converter );

               *dst = sample;
               dst++;
            }
         }

         int fileRet = lame_encode_buffer_interleaved_ieee_float(
            ctx,
            reinterpret_cast< float const* >( convBuf.data() ),
            availFrames,
            fileBuf.data(),
            fileBuf.size() );

         if ( fileRet <= 0 )
         {
            DE_ERROR("fileRet(",fileRet,") <= 0")
         }
         else
         {
            fwrite( fileBuf.data(), 1, fileRet, file );
            filebIndex += fileRet;
         }

         fileRet = lame_encode_flush( ctx, fileBuf.data(), fileBuf.size() );
         if ( fileRet <= 0 )
         {
            DE_ERROR("Flush got fileRet(",fileRet,") <= 0")
         }
         else
         {
            fwrite( fileBuf.data(), 1, fileRet, file );
            filebIndex += fileRet;
         }

         break;
      }
      else
      {
         auto dst = reinterpret_cast< float* >( convBuf.data() );
         //auto bps = convBuf.getBytesPerSample();

         for ( uint64_t i = 0; i < chunkFrames; ++i )
         {
            for ( uint32_t c = 0; c < 2; ++c )
            {
               float sample = 0.0f;
               src.getSample( frameIndex + i, c, &sample, converter );

               *dst = sample;
               dst++;
            }
         }

         int fileRet = lame_encode_buffer_interleaved_ieee_float(
            ctx,
            reinterpret_cast< float const* >( convBuf.data() ),
            chunkFrames,
            fileBuf.data(),
            fileBuf.size() );

         if ( fileRet <= 0 )
         {
            DE_ERROR("fileRet(",fileRet,") <= 0")
         }
         else
         {
            fwrite( fileBuf.data(), 1, fileRet, file );
            filebIndex += fileRet;
         }

         frameIndex += chunkFrames;
      }
      chunkIndex++;


      if ( filebIndex - filecIndex >= 10*1024*1024 )
      {
         DE_DEBUG("Wrote MP3 ", dbStrBytes( filebIndex ) )
         filecIndex = filebIndex;
      }
   }

   lame_get_lametag_frame( ctx, fileBuf.data(), fileBuf.size() );
   //lame_get_id3v2_tag( )
   lame_close( ctx );
   fclose(file);
   DE_DEBUG("Saved ", uri, ", src(",src.toString(),")" )
   return true;
}

#if 0
   while ( frameIndex + chunkFrames < frameCount )
   {
      DE_ERROR("chunkFrames(",chunkFrames,"), frameIndex(",frameIndex,"), frameCount(",frameCount,")" )
      DE_FLUSH
      memset( ch0.data(), 0, ch0.size() * 4 );
      memset( ch1.data(), 0, ch1.size() * 4 );

      for ( size_t i = 0; i < chunkFrames; ++i )
      {
         ch0[ i ] = src.getSampleF32( frameIndex + i, 0 );
         ch1[ i ] = src.getSampleF32( frameIndex + i, 1 );
      }

      int ret = lame_encode_buffer_ieee_float( ctx,
            ch0.data(), ch1.data(), chunkFrames, tmp.data(), tmp.size() );

      if ( ret < 0 )
      {
         DE_ERROR("ret(",ret,") < 0")
      }
      else
      {
         fwrite( tmp.data(), 1, tmp.size(), file );
      }

      frameIndex += chunkFrames;
   }

   int ret = lame_encode_flush( ctx, tmp.data(), tmp.size() );
   if ( ret < 0 )
   {
      DE_ERROR("Flush got ret(",ret,") < 0")
   }
   else
   {
      fwrite( tmp.data(), 1, tmp.size(), file );
   }
#endif

} // end namespace io
} // end namespace audio
} // end namespace de











