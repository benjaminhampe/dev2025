#pragma once
#include <cstdint>
#include <sstream>

namespace de {

struct FileMagic
{
   enum EFileMagic
   {
      XPM = 0,
      JPG,
      PNG,
      BMP,
      GIF,
      TGA,
      DDS,
      TIF,
      PCX,

      WAV,
      AAC,
      MP3,
      MP4,
      M4A,
      FLAC,
      OGG,
      WMA,
      FLV,

      EFileMagicMax
   };

   static EFileMagic
   getFileMagic( void const* ptr )
   {
      if ( !ptr ) return EFileMagicMax;

      if      (isJPG( ptr )) return JPG;
      else if (isPNG( ptr )) return PNG;
      else if (isBMP( ptr )) return BMP;
      else if (isGIF( ptr )) return GIF;
      else if (isTIF( ptr )) return TIF;
      else if (isTGA( ptr )) return TGA;
      else if (isPCX( ptr )) return PCX;

      else if (isWAV( ptr )) return WAV;
      else if (isAAC( ptr )) return AAC;
      else if (isMP3( ptr )) return MP3;
      else if (isMP4( ptr )) return MP4;
      else if (isM4A( ptr )) return M4A;
      else if (isFLAC( ptr ))return FLAC;
      else if (isOGG( ptr )) return OGG;
      else if (isWMA( ptr )) return WMA;
      else if (isFLV( ptr )) return FLV;

      else return EFileMagicMax;
   }

   static std::string
   getString( EFileMagic const fileMagic )
   {
      switch (fileMagic)
      {
      case XPM: return "XPM";
      case JPG: return "JPG";
      case PNG: return "PNG";
      case BMP: return "BMP";
      case GIF: return "GIF";
      case TIF: return "TIF";
      case TGA: return "TGA";
      case PCX: return "PCX";

      case WAV: return "WAV";
      case AAC: return "AAC";
      case MP3: return "MP3";
      case MP4: return "MP4";
      case M4A: return "M4A";
      case FLAC:return "FLAC";
      case OGG: return "OGG";
      case WMA: return "WMA";
      case FLV: return "FLV";

      default: return "EFileMagicMax";
      }
   }

   // ============================================================================
   /// ImageMagic
   // ============================================================================
   static bool isJPG( void const* ptr );
   static bool isPNG( void const* ptr );
   static bool isBMP( void const* ptr );
   static bool isGIF( void const* ptr );
   static bool isTIF( void const* ptr );
   static bool isTGA( void const* ptr );
   static bool isPCX( void const* ptr );

   // ============================================================================
   /// AudioMagic
   // ============================================================================
   static bool isAAC( void const* ptr );
   static bool isWAV( void const* ptr );
   static bool isMP3( void const* ptr );
   static bool isMP4( void const* ptr );
   static bool isFLAC( void const* ptr );
   static bool isOGG( void const* ptr );
   static bool isWMA( void const* ptr );
   static bool isM4A( void const* ptr );
   static bool isFLV( void const* ptr );
};

} // end namespace de

/*
inline de::EFileType dbGetFileMagic( void const* ptr )
{
   return de::getFileMagic( ptr );
}

inline std::string dbGetFileMagicStr( void const* ptr )
{
   auto fileType = de::getFileMagic( ptr );
   return de::convertFileMagic2String( fileType );
}
*/
