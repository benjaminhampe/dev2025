// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGE_BMP_HPP
#define DE_IMAGE_BMP_HPP

#include <de/ImageReader.hpp>

#if defined(DE_IMAGE_READER_BMP_ENABLED) || defined(DE_IMAGE_WRITER_BMP_ENABLED)

#include <fstream>

namespace de {
namespace image {
namespace bmp {
// ===================================================

#pragma pack( push )
#pragma pack( 1 )

   struct BMPHeader
   {
      uint16_t	Id;					// [ 0...1] 2Bytes
                                 //          BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
                                 //          BA - OS/2 Bitmap Array
                                 //          CI - OS/2 Color Icon
                                 //          CP - OS/2 Color Pointer
                                 //          IC - OS/2 Icon
                                 //          PT - OS/2 Pointer
      uint32_t	FileSize;         // [ 2...5] 4Bytes
      uint32_t	Reserved;         // [ 6...9] 4Bytes
      uint32_t	BitmapDataOffset; // [10..13] 4Bytes
      uint32_t	BitmapHeaderSize;	// [14..17] 4Bytes // should be 28h for windows bitmaps or
                                           // 0Ch for OS/2 1.x or F0h for OS/2 2.x
      uint32_t Width;            // [18..21] 4Bytes
      uint32_t Height;           // [22..25] 4Bytes
      uint16_t Planes;           // [26..27] 2Bytes
      uint16_t BPP;					// [28..29] 2Bytes
                                 //          1: Monochrome bitmap
                                 //          4: 16 color bitmap
                                 //          8: 256 color bitmap
                                 //          16: 16bit (high color) bitmap
                                 //          24: 24bit (true color) bitmap
                                 //          32: 32bit (true color) bitmap

      uint32_t  Compression;		// [30..33] 4Bytes
                                 //          0: none (Also identified by BI_RGB)
                                 //          1: RLE 8-bit / pixel (Also identified by BI_RLE4)
                                 //          2: RLE 4-bit / pixel (Also identified by BI_RLE8)
                                 //          3: Bitfields  (Also identified by BI_BITFIELDS)

      uint32_t  BitmapDataSize;	// [34..37] 4Bytes // Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
      uint32_t  PixelPerMeterX;  // [38..41] 4Bytes
      uint32_t  PixelPerMeterY;  // [42..45] 4Bytes
      uint32_t  Colors;          // [46..49] 4Bytes
      uint32_t  ImportantColors; // [50..53] 4Bytes
   };
#pragma pack( pop )

// ===================================================

#pragma pack( push )
#pragma pack( 1 )
   typedef union PixelInfo
   {
       uint32_t ARGB;
       struct { uint8_t B, G, R, A; };
   } *PPixelInfo;

#pragma pack( pop )

// ===================================================

/*
class BMP
{
public:
    uint32_t width;
    uint32_t height;
    uint16_t bpp;
    std::vector< uint8_t > data;

public:
    BMP() : width(0), height(0), bpp(0) {}
    bool load( std::string const & fileName );
    bool hasAlphaChannel() {return bpp == 32;}

};
*/

} // end namespace bmp.
} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_CODEC_BMP_ENABLED

#endif // DE_IMAGE_BMP_HPP
