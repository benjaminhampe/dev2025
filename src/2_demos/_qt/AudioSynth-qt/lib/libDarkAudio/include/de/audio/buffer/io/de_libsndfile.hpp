#ifndef DE_AUDIO_LIB_SNDFILE_UTILS_HPP
#define DE_AUDIO_LIB_SNDFILE_UTILS_HPP

#include <de/audio/buffer/SampleType.hpp>

// #ifdef USE_LIBSNDFILE
#include <sndfile.h>

namespace de {
namespace audio {

struct SNDFILE_Utils
{
   static int
   getSndFormatFromFileExt( std::string const & ext )
   {
      if ( ext.empty() ) return -1;
           if (ext=="wav")  return SF_FORMAT_WAV;
      else if (ext=="ogg")  return SF_FORMAT_OGG;
      else if (ext=="aif")  return SF_FORMAT_AIFF;
      else if (ext=="aiff") return SF_FORMAT_AIFF;
      else if (ext=="flac") return SF_FORMAT_FLAC;
      else if (ext=="au")   return SF_FORMAT_AU;
      else if (ext=="raw")  return SF_FORMAT_RAW;
      else if (ext=="paf")  return SF_FORMAT_PAF;
      else if (ext=="svx")  return SF_FORMAT_SVX;
      else if (ext=="nist") return SF_FORMAT_NIST;
      else if (ext=="voc")  return SF_FORMAT_VOC;
      else if (ext=="sf")   return SF_FORMAT_IRCAM;
      else if (ext=="w64")  return SF_FORMAT_W64;
      else if (ext=="mat4") return SF_FORMAT_MAT4;
      else if (ext=="mat5") return SF_FORMAT_MAT5;
      else if (ext=="pvf")  return SF_FORMAT_PVF;
      else if (ext=="xi")   return SF_FORMAT_XI;
      else if (ext=="htk")  return SF_FORMAT_HTK;
      else if (ext=="sds")  return SF_FORMAT_SDS;
      else if (ext=="avr")  return SF_FORMAT_AVR;
      else if (ext=="sd2")  return SF_FORMAT_SD2;
      else if (ext=="caf")  return SF_FORMAT_CAF;
      else if (ext=="wve")  return SF_FORMAT_WVE;
      else if (ext=="mpc2k")return SF_FORMAT_MPC2K;
      else if (ext=="rf64") return SF_FORMAT_RF64;
      return -1;
   }

   static uint32_t
   getBitsPerSampleFromSndFormat( int format )
   {
      uint32_t result = 0;
      switch ( format )
      {
         case SF_FORMAT_PCM_S8: result = 8; break;
         case SF_FORMAT_PCM_16: result = 16; break;
         case SF_FORMAT_PCM_24: result = 24; break;
         case SF_FORMAT_PCM_32: result = 32; break; /* Signed 32 bit data */
         case SF_FORMAT_PCM_U8: result = 8; break;
         case SF_FORMAT_FLOAT: result = 32; break;
         case SF_FORMAT_DOUBLE: result = 64; break;
         case SF_FORMAT_ULAW: result = 8; break;
         case SF_FORMAT_ALAW: result = 8; break;
         case SF_FORMAT_IMA_ADPCM: result = 16; break;
         case SF_FORMAT_MS_ADPCM: result = 16; break;
         default:
            break;
      }

      return result;
   }

   static ESampleType
   getSampleTypeFromFormat( int format )
   {
      ESampleType result = ST_Unknown;
      switch ( format )
      {
         case SF_FORMAT_PCM_S8: result = ST_S8; break;
         case SF_FORMAT_PCM_16: result = ST_S16; break;
         case SF_FORMAT_PCM_24: result = ST_S24; break;
         case SF_FORMAT_PCM_32: result = ST_S32; break; /* Signed 32 bit data */
         case SF_FORMAT_PCM_U8: result = ST_U8; break;
         case SF_FORMAT_FLOAT: result = ST_F32; break;
         case SF_FORMAT_DOUBLE: result = ST_F64; break;
   //        case SF_FORMAT_ULAW: result = EST_ULAW_8; break;
   //        case SF_FORMAT_ALAW: result = EST_ALAW_8; break;
   //        case SF_FORMAT_IMA_ADPCM: result = EST_S16; break;
   //        case SF_FORMAT_MS_ADPCM: result = EST_S16; break;
         default: break;
      }

      return result;
   }

};


} // end namespace audio
} // end namespace de


#endif
