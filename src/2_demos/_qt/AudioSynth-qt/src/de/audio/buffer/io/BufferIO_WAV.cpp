#include <de/audio/buffer/io/BufferIO_WAV.hpp>
#include <de/audio/buffer/io/Wav.hpp>

namespace de {
namespace audio {
namespace io {

bool
BufferWAV::load( Buffer & dst, std::string uri )
{
   wav::WavInFile file( uri.c_str() );
   if ( !file.is_open() )
   {
      DE_ERROR("Cant open uri ",uri)
      return false;
   }

   uint32_t bps = file.getBytesPerSample();
   uint16_t cc = file.getNumChannels();
   uint32_t sr = file.getSampleRate();
   uint64_t sc = file.getNumSamples();
   uint64_t fc = sc / cc;
   double ms = double( fc ) / double( sr );

   ESampleType st = ST_Unknown;
   if ( bps == 1 )
   {
      st = ST_S8I;
   }
   else if ( bps == 2 )
   {
      st = ST_S16I;
   }
//   else if ( bps == 3 )
//   {
//      st = ST_S32I;
//   }
   else if ( bps == 4 )
   {
      st = ST_F32I;
   }

   DE_DEBUG("WAV.sampleType = ",ST_toString(st))
   DE_DEBUG("WAV.bytesPerSample = ",bps)
   DE_DEBUG("WAV.channelCount = ",cc)
   DE_DEBUG("WAV.sampleRate = ",sr)
   DE_DEBUG("WAV.sampleCount = ",sc)
   DE_DEBUG("WAV.frameCount = ",fc)
   DE_DEBUG("WAV.durationInMs = ",ms)

   if ( st == ST_Unknown )
   {
      DE_DEBUG("WAV has unsupported type ",uri)
      return false;
   }

   // Prepare destination buffer
   dst.setFormat( st,cc,sr );
   dst.resize( fc );

   // Fill destination buffer
   uint64_t readCounter = 0;
   if ( st == ST_S8I )
   {
      readCounter = file.read( reinterpret_cast< uint8_t* >( dst.data() ), dst.getSampleCount() );
   }
   else if ( st == ST_S16I )
   {
      readCounter = file.read( reinterpret_cast< int16_t* >( dst.data() ), dst.getSampleCount() );
   }
//   else if ( st == ST_S24I )
//   {
//      readCounter = file.read( reinterpret_cast< s24* >( dst.data() ), dst.getSampleCount() );
//   }
   else if ( st == ST_F32I )
   {
      readCounter = file.read( reinterpret_cast< float* >( dst.data() ), dst.getSampleCount() );
   }
   else
   {

   }

   if ( readCounter != dst.getSampleCount() )
   {
      DE_ERROR("Got different readCounter(",readCounter,") from expectedSampleCount(",dst.getSampleCount(),")")
   }

   return true;
}

bool
BufferWAV::save( Buffer const & src, std::string uri )
{
   Buffer dst;
   if ( !src.convert( dst, ST_S16I ) )
   {
      DE_ERROR("Cant convert to S16interleaved")
      return false;
   }

   wav::WavOutFile file( uri.c_str(), dst.getSampleRate(), dst.getBitsPerSample(), dst.getChannelCount() );
   file.write_S16( reinterpret_cast< int16_t const* >( dst.data() ), dst.getSampleCount() );

   return true;
}


} // end namespace io
} // end namespace audio
} // end namespace de











