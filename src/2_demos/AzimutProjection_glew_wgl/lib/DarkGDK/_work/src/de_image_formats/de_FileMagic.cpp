#include <de_image_formats/de_FileMagic.h>

namespace de {

// ============================================================================
/// ImageMagic
// ============================================================================

bool
FileMagic::isJPG( void const* ptr )
{
   //uint32_t const IMAGE_MAGIC_JPG 		= 0xFFD8FF00;
   //uint32_t const IMAGE_MAGIC_JPG_JFIF = 0x4A464946;
   //uint32_t const IMAGE_MAGIC_JPG_Exif = 0x45786966;
   //uint32_t const IMAGE_MAGIC_JPG_EXIF = 0x45584946;
   //   uint32_t magic = *reinterpret_cast< uint32_t const* >( ptr );
   //   if ( (magic & 0xFFFFFF00) == IMAGE_MAGIC_JPG )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   uint8_t m3 = *p++;
   uint8_t m4 = *p++;
   if (  ( m1 == 0xFF ) &&
         ( m2 == 0xD8 ) &&
         ( m3 == 0xFF ) &&
         ( m4 == 0x00 ) )
      return true;
   else
      return false;
}
bool
FileMagic::isPNG( void const* ptr )
{
   //   uint32_t const IMAGE_MAGIC_PNG_1    = 0x89504E47;
   //   uint32_t const IMAGE_MAGIC_PNG_2    = 0x0D0A1A0A;
   //   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
   //   uint32_t magic1 = *p++;
   //   uint32_t magic2 = *p++;
   //   if (( magic1 == IMAGE_MAGIC_PNG_1 ) && ( magic2 == IMAGE_MAGIC_PNG_2 ))
   //      return true;
   //   else
   //      return false;
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++; uint8_t m2 = *p++; uint8_t m3 = *p++; uint8_t m4 = *p++;
   uint8_t m5 = *p++; uint8_t m6 = *p++; uint8_t m7 = *p++; uint8_t m8 = *p++;
   if (  ( m1 == 0x89 ) && ( m2 == 0x50 ) && ( m3 == 0x4E ) && ( m4 == 0x47 )
      && ( m5 == 0x0D ) && ( m6 == 0x0A ) && ( m7 == 0x1A ) && ( m8 == 0x0A ) )
      return true;
   else
      return false;
}

bool
FileMagic::isBMP( void const* ptr )
{
   //uint16_t const IMAGE_MAGIC_BMP 		= 0x424D;
   //uint16_t magic = *reinterpret_cast< uint16_t const* >( ptr );
   //if ( magic == IMAGE_MAGIC_BMP )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   if (  ( m1 == 0x42 ) &&
         ( m2 == 0x4D ) )
      return true;
   else
      return false;
}
bool
FileMagic::isGIF( void const* ptr )
{
   //uint32_t const IMAGE_MAGIC_GIF_1 = 0x47494638;
   //uint16_t const IMAGE_MAGIC_GIF_2 = 0x3761;
   //uint16_t const IMAGE_MAGIC_GIF_3 = 0x3961;
//   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
//   uint32_t magic1 = *p++;
//   uint16_t const* q = reinterpret_cast< uint16_t const* >( p );
//   uint16_t magic2 = *q;
//   if ( ( magic1 == IMAGE_MAGIC_GIF_1 )
//    && (( magic2 == IMAGE_MAGIC_GIF_2 ) || ( magic2 == IMAGE_MAGIC_GIF_3 )) )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   uint8_t m3 = *p++;
   uint8_t m4 = *p++;
   uint8_t m5 = *p++;
   uint8_t m6 = *p++;
   if (  ( m1 == 0x47 ) &&
         ( m2 == 0x49 ) &&
         ( m3 == 0x46 ) &&
         ( m4 == 0x38 ) &&
         ( (( m5 == 0x37 ) && ( m6 == 0x61 )) ||
           (( m5 == 0x39 ) && ( m6 == 0x61 )) )
      )
      return true;
   else
      return false;
}
bool
FileMagic::isTIF( void const* ptr )
{
   uint32_t const IMAGE_MAGIC_TIFF 		= 0x49204900;
   uint32_t const IMAGE_MAGIC_TIFF_LE 	= 0x49492A00;
   uint32_t const IMAGE_MAGIC_TIFF_BE 	= 0x4D4D002A;

   uint32_t magic = *reinterpret_cast< uint32_t const* >( ptr );
   if (( magic == IMAGE_MAGIC_TIFF_LE ) ||
       ( magic == IMAGE_MAGIC_TIFF_BE ) ||
       ((magic & 0xFFFFFF00) == IMAGE_MAGIC_TIFF ))
      return true;
   else
      return false;
}

bool
FileMagic::isTGA( void const* ptr )
{

   // ============================================================================
   // Mime: image/x-tga
   // TGA	- Truevision Targa Graphic file
   // Trailer:
   // 54 52 55 45 56 49 53 49   TRUEVISI
   // 4F 4E 2D 58 46 49 4C 45   ON-XFILE
   // 2E 00                     ..

    return false;
}
bool
FileMagic::isPCX( void const* ptr )
{
   return false;
}

// ============================================================================
/// AudioMagic
// ============================================================================

// CDA
// 52 49 46 46 xx xx xx xx == RIFF....
// 43 44 44 41 66 6D 74 20 == CDDAfmt

// WAV
// 52 49 46 46 xx xx xx xx == RIFF....
// 57 41 56 45 66 6D 74 20 == WAVEfmt

uint32_t const AUDIO_MAGIC_RIFF = 0x52494646; // 'R' 'I' 'F' 'F' - Bytes 0..3
uint32_t const AUDIO_MAGIC_CDDA = 0x43444441; // 'C' 'D' 'D' 'A' - Bytes 8..11
uint32_t const AUDIO_MAGIC_WAVE = 0x57415645; // 'W' 'A' 'V' 'E' - Bytes 8..11
uint32_t const AUDIO_MAGIC_fmts = 0x666D7420; // 'f' 'm' 't' ' ' - Bytes 12..15
uint32_t const AUDIO_MAGIC_data = 0x64617461; // 'd' 'a' 't' 'a' - Bytes 12..15

// FLAC
// 66 4C 61 43 00 00 00 22 == fLaC..."

uint32_t const AUDIO_MAGIC_FLAC_1 = 0x664C6143; // 'f' 'L' 'a' 'C'
uint32_t const AUDIO_MAGIC_FLAC_2 = 0x00000022; // '\0' '\0' '\0' '"'

// OGA, OGG, OGV, OGX - Ogg Vorbis Codec compressed Multimedia file
// 4F 67 67 53 00 02 00 00  OggS....
// 00 00 00 00 00 00	 	......

uint32_t const AUDIO_MAGIC_OGG_1 = 0x4F676753; // 'O' 'g' 'g' 'S'
uint32_t const AUDIO_MAGIC_OGG_2 = 0x00020000; // '\0' '\STX' '\0' '\0'
uint32_t const AUDIO_MAGIC_OGG_3 = 0x00000000; // '\0' '\0' '\0' '\0'
uint16_t const AUDIO_MAGIC_OGG_4 = 0x0000; // '\0' '\0'

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 33 67 70 35 = [4-Byte-Offset] ftyp3gp5

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 4D 53 4E 56 = [4-Byte-Offset] ftypMSNV

// M4V - MPEG-4 video|QuickTime file
// xx xx xx xx 66 74 79 70 6D 70 34 32 = [4-Byte-Offset] ftypmp42

// M4A - Apple Lossless Audio Codec file
// xx xx xx xx 66 74 79 70 4D 34 41 20 = [4-Byte-Offset] ftypM4A

// MPEG, MPG, MP3 - MPEG audio file frame  synch pattern
// FF Ex	 	ÿ.
// FF Fx	 	ÿ.
// 0xFF FB = 0b1111 1111 1111 1011
// 0xFF FA = 0b1111 1111 1111 1010

// AAAAAAAA AAABBCCD
// A	11	(31-21)	Frame sync (all bits must be set)
// B	2	(20,19)	MPEG Audio version ID
// 00 - MPEG Version 2.5 (later extension of MPEG 2)
// 01 - reserved
// 10 - MPEG Version 2 (ISO/IEC 13818-3)
// 11 - MPEG Version 1 (ISO/IEC 11172-3)
// Note: MPEG Version 2.5 was added lately to the MPEG 2 standard. It is an extension used for very low bitrate files, allowing the use of lower sampling frequencies. If your decoder does not support this extension, it is recommended for you to use 12 bits for synchronization instead of 11 bits.
//
// C	2	(18,17)	Layer description
// 00 - reserved
// 01 - Layer III
// 10 - Layer II
// 11 - Layer I

// AAC	 	MPEG-4 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F1    0b1111 1111 1111 0001

// AAC	 	MPEG-2 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F9	0b1111 1111 1111 1001

// REG	 	Windows Registry file ( scheiss Microsoft )
// FF FE    0b1111 1111 1111 1110

bool
FileMagic::isAAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isWAV( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP3( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP4( void const* ptr )
{
   return false;
}

bool
FileMagic::isFLAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isOGG( void const* ptr )
{
   return false;
}

bool
FileMagic::isWMA( void const* ptr )
{
   return false;
}

bool
FileMagic::isM4A( void const* ptr )
{
   return false;
}

bool FileMagic::isFLV( void const* ptr )
{
   return false;
}

} // end namespace de
