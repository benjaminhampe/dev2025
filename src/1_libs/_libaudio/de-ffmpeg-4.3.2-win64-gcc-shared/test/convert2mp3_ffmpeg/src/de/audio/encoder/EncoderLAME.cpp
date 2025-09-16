#include <de/audio/encoder/EncoderLAME.hpp>

namespace de {
namespace audio {
namespace encoder {

EncoderLAME::EncoderLAME()
   : m_isOpen( false )
   , m_isDebug( false )
   , m_lame( nullptr )
   , m_kilobitRate( 192 )
   , m_channelCount( 2 )
   , m_sampleRate( 44100 )
   , m_frameCount( 0 )
   , m_byteCount( 0 )
   , m_byteCount2( 0 )
   , m_chunkFrames( 0 )
   , m_chunkSamples( 0 )
   , m_chunkBytes( 0 )
   , m_file( nullptr )
{}

EncoderLAME::~EncoderLAME()
{
   close();
}

void
EncoderLAME::close()
{
   if ( m_isOpen )
   {
      lame_set_num_samples( m_lame, m_frameCount );

      int n = lame_encode_flush( m_lame, m_fileBuf.data(), m_fileBuf.size() );
      if ( n <= 0 )
      {
         DE_ERROR("Flush got ",n," <= 0")
      }
      else
      {
         fwrite( m_fileBuf.data(), 1, n, m_file );
      }


      lame_get_lametag_frame( m_lame, m_fileBuf.data(), m_fileBuf.size() );
      lame_close( m_lame );

      ::fclose( m_file );

      DE_DEBUG("Closed ", m_uri )
      m_isOpen = false;
   }
}

bool
EncoderLAME::open( std::string uri, uint32_t srcChannels, uint32_t srcRate, uint32_t kiloBitRate )
{
   if ( srcChannels < 1 || srcChannels > 2 )
   {
      DE_ERROR("Wrong channels ",srcChannels)
      return false;
   }

   if ( m_isOpen ) { DE_ERROR("Already open ",uri) return false; }

   m_lame = lame_init();
   m_kilobitRate = kiloBitRate;
   m_channelCount = srcChannels;
   m_sampleRate = srcRate;
   m_frameCount = 0;

   lame_set_num_channels( m_lame, m_channelCount );
   lame_set_in_samplerate( m_lame, m_sampleRate );
   lame_set_out_samplerate( m_lame, m_sampleRate );

   // # lame_set_num_samples( m_lame, frameCount );

   //lame_set_brate( m_lame, bitRate );
   if ( srcChannels == 1 )
   {
      lame_set_mode( m_lame, MONO );
   }
   else if ( srcChannels == 2 )
   {
      lame_set_mode( m_lame, STEREO );
   }

   lame_set_disable_reservoir( m_lame, 1 ); //bit reservoir has to be disabled for seamless streaming
   lame_set_VBR( m_lame, vbr_off );
   lame_set_brate( m_lame, m_kilobitRate );

   //lame_set_VBR( m_lame, vbr_default );
   //lame_set_VBR_min_bitrate_kbps( m_lame, 64 );
   //lame_set_VBR_max_bitrate_kbps( m_lame, 192 );
   //lame_set_quality( m_lame, 3 ); // 0-best, 9-worst
   lame_set_bWriteVbrTag( m_lame, 1 );
   //lame_set_asm_optimizations()

   if ( -1 == lame_init_params( m_lame ) )
   {
      DE_ERROR("-1 == lame_init_params( m_lame )")
      return false;
   }

   m_chunkFrames = lame_get_framesize( m_lame );
   m_chunkSamples = m_chunkFrames * m_channelCount;
   m_chunkBytes = m_chunkSamples * sizeof( float );

   if ( m_isDebug )
   {
      //DE_DEBUG("src(",src.toString(),")" )
      DE_DEBUG("kilobitRate(",m_kilobitRate,")" )
      DE_DEBUG("sampleRate(",m_sampleRate,")" )
      DE_DEBUG("frameCount(",m_frameCount,")" )
      DE_DEBUG("chunkFrames(",m_chunkFrames,")" )
      DE_DEBUG("chunkSamples(",m_chunkSamples,")" )
      DE_DEBUG("chunkBytes(",m_chunkBytes,")" )
   }

   m_file = fopen( uri.c_str(), "wb");
   if ( !m_file )
   {
      DE_ERROR("Cant save ", uri )
      return false;
   }

   // MPEG1:
   //    num_samples*(bitrate/8)/samplerate + 4*1152*(bitrate/8)/samplerate + 512
   // MPEG2:
   //    num_samples*(bitrate/8)/samplerate + 4*576*(bitrate/8)/samplerate + 256
   m_fileBuf.resize( m_chunkBytes, 0x00 );
   return true;
}

bool
EncoderLAME::writeSamples( float const* src, uint64_t srcFrames )
{
   if ( !src || srcFrames < 1)
   {
      DE_ERROR("No src")
      return false;
   }

   // lame_set_num_samples( m_lame, m_frameCount );

   int n = lame_encode_buffer_interleaved_ieee_float(
      m_lame, src, srcFrames, m_fileBuf.data(), m_fileBuf.size() );

   m_chunkCount++;

   if ( n <= 0 )
   {
      //DE_ERROR("No samples encoded")
      return false;
   }

   m_frameCount += srcFrames;

   fwrite( m_fileBuf.data(), 1, n, m_file );
   m_byteCount += n;

   if ( m_byteCount - m_byteCount2 >= 10*1024*1024 )
   {
      DE_DEBUG("Wrote MP3 ", dbStrBytes( m_byteCount ) )
      m_byteCount2 = m_byteCount;
   }
   return true;
}

} // end namespace encoder
} // end namespace audio
} // end namespace de

/*
namespace de {
namespace audio {
namespace encoder {

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
      DE_FLUSH
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
      DE_FLUSH
   }

   FILE* file = fopen( uri.c_str(), "wb");
   if ( !file )
   {
      DE_ERROR("Cant save ", uri )
      DE_FLUSH
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
         DE_DEBUG("Wrote MP3 ", dbStrByteCount( filebIndex ) )
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

} // end namespace io
} // end namespace audio
} // end namespace de




*/






