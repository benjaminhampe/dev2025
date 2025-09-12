#include <de/image/Image.h>
#include <de/image/ImagePainter.h>
#include <de/image/ImageConfig.h>

// *.jpg, *.jpeg
#if defined(DE_IMAGE_READER_JPG_ENABLED) || defined(DE_IMAGE_WRITER_JPG_ENABLED)
   #include <de/image/Image_JPG.h>
#endif

// *.png
#if defined(DE_IMAGE_READER_PNG_ENABLED) || defined(DE_IMAGE_WRITER_PNG_ENABLED)
   #include <de/image/Image_PNG.h>
#endif

// *.xpm
#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)
   #include <de/image/Image_XPM.h>
#endif

// *.exr HDR formats
#if defined(DE_IMAGE_READER_EXR_ENABLED) || defined(DE_IMAGE_WRITER_EXR_ENABLED)
   #include <de/image/Image_EXR.h>
#endif

// *.webp
#if defined(DE_IMAGE_READER_WEBP_ENABLED) || defined(DE_IMAGE_WRITER_WEBP_ENABLED)
   #include <de/image/Image_WEBP.h>
#endif

// NEW:

// *.bmp
#if defined(DE_IMAGE_READER_BMP_ENABLED) || defined(DE_IMAGE_WRITER_BMP_ENABLED)
   #include <de/image/Image_BMP.h>
#endif

// *.dds
#if defined(DE_IMAGE_READER_DDS_ENABLED) || defined(DE_IMAGE_WRITER_DDS_ENABLED)
   #include <de/image/Image_DDS.h>
#endif

// *.gif
#if defined(DE_IMAGE_READER_GIF_ENABLED) || defined(DE_IMAGE_WRITER_GIF_ENABLED)
   #include <de/image/Image_GIF.h>
#endif

// *.ico, *.cur
#if defined(DE_IMAGE_READER_ICO_ENABLED) || defined(DE_IMAGE_WRITER_ICO_ENABLED)
   #include <de/image/Image_ICO.h>
#endif

// *.tga
#if defined(DE_IMAGE_READER_TGA_ENABLED) || defined(DE_IMAGE_WRITER_TGA_ENABLED)
   #include <de/image/Image_TGA.h>
#endif

// *.tif, *.tiff, *.tif4, *.tiff4
#if defined(DE_IMAGE_READER_TIF_ENABLED) || defined(DE_IMAGE_WRITER_TIF_ENABLED)
    #include <de/image/Image_TIF.h>
#endif

// *.rgb
#if defined(DE_IMAGE_READER_RGB_ENABLED) || defined(DE_IMAGE_WRITER_RGB_ENABLED)
   #include <de/image/Image_RGB.h>
#endif

// *.wal
#if defined(DE_IMAGE_READER_WAL_ENABLED) || defined(DE_IMAGE_WRITER_WAL_ENABLED)
    #include <de/image/Image_WAL.h>
#endif


// *.pcx
#if defined(DE_IMAGE_READER_PCX_ENABLED) || defined(DE_IMAGE_WRITER_PCX_ENABLED)
   #include <de/image/Image_PCX.h>
#endif

// *.ppm
#if defined(DE_IMAGE_READER_PPM_ENABLED) || defined(DE_IMAGE_WRITER_PPM_ENABLED)
   #include <de/image/Image_PPM.h>
#endif

// *.html
#if defined(DE_IMAGE_READER_HTML_ENABLED) || defined(DE_IMAGE_WRITER_HTML_ENABLED)
   #include <de/image/Image_HTML.h>
#endif

namespace de
{

// static
std::string PixelFormat::getString( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8 : return "R8G8B8A8";
    case PixelFormat::R8G8B8   : return "R8G8B8";
    case PixelFormat::R5G6B5   : return "R5G6B5";
    case PixelFormat::R5G5B5A1 : return "R5G5B5A1";
    case PixelFormat::R8       : return "R8";
    case PixelFormat::R16      : return "R16";
    case PixelFormat::R24      : return "R24";
    case PixelFormat::R32      : return "R32";
    case PixelFormat::R32F     : return "R32F";
    //case PixelFormat::R32FG32FB32F : return "R32FG32FB32F";
    case PixelFormat::RGB16F  : return "RGB16F";
    case PixelFormat::RGBA16F : return "RGBA16F";
    case PixelFormat::RGB32F  : return "RGB32F";
    case PixelFormat::RGBA32F : return "RGBA32F";

    case PixelFormat::A1R5G5B5 : return "A1R5G5B5";
    case PixelFormat::B8G8R8A8 : return "B8G8R8A8";
    case PixelFormat::B8G8R8   : return "B8G8R8";
    case PixelFormat::D24S8    : return "D24S8";
    case PixelFormat::D32F     : return "D32F";
    case PixelFormat::D32FS8   : return "D32FS8";
    case PixelFormat::S8       : return "S8";
    default                    : return "Unknown"; // Auto
    }
}

// static
uint32_t PixelFormat::getBitsPerPixel( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8 : return 32;
    case PixelFormat::R8G8B8   : return 24;
    case PixelFormat::R5G6B5   : return 16;
    case PixelFormat::R5G5B5A1 : return 16;
    case PixelFormat::R8       : return 8;
    case PixelFormat::R16      : return 16;
    case PixelFormat::R24      : return 24;
    case PixelFormat::R32      : return 32;
    case PixelFormat::R32F     : return 32;
    case PixelFormat::RGB32F   : return 96;
    case PixelFormat::RGBA32F  : return 128;
    case PixelFormat::RGB16F   : return 48;
    case PixelFormat::RGBA16F  : return 64;
    case PixelFormat::A1R5G5B5 : return 16;
    case PixelFormat::B8G8R8A8 : return 32;
    case PixelFormat::B8G8R8   : return 24;
    case PixelFormat::D24S8    : return 32;
    case PixelFormat::D32F     : return 32;
    case PixelFormat::D32FS8   : return 40;
    case PixelFormat::S8       : return 8;
    default                    : return 0;
    }
}

// static
uint32_t PixelFormat::getBytesPerPixel( int const fmt )
{
    return getBitsPerPixel(fmt) / 8;
}

// static
uint32_t PixelFormat::getChannelCount( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8  : return 4;
    case PixelFormat::R8G8B8    : return 3;
    case PixelFormat::R5G6B5    : return 3;
    case PixelFormat::R5G5B5A1  : return 4;
    case PixelFormat::R8        : return 1;
    case PixelFormat::R16       : return 1;
    case PixelFormat::R24       : return 1;
    case PixelFormat::R32       : return 1;
    case PixelFormat::R32F      : return 1;
    //case PixelFormat::R32FG32FB32F   : return 3;
    case PixelFormat::RGB16F    : return 3;
    case PixelFormat::RGBA16F   : return 4;
    case PixelFormat::RGB32F    : return 3;
    case PixelFormat::RGBA32F   : return 4;
    case PixelFormat::A1R5G5B5  : return 4;
    case PixelFormat::B8G8R8A8  : return 4;
    case PixelFormat::B8G8R8    : return 3;
    case PixelFormat::D24S8     : return 2;
    case PixelFormat::D32F      : return 1;
    case PixelFormat::D32FS8    : return 2;
    case PixelFormat::S8        : return 1;
    default                     : return 0;
    }
}

// static
uint8_t PixelFormat::getRedBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8  : return 8;
    case PixelFormat::R8G8B8    : return 8;
    case PixelFormat::R5G6B5    : return 5;
    case PixelFormat::R5G5B5A1  : return 5;
    case PixelFormat::R8        : return 8;
    case PixelFormat::R16       : return 16;
    case PixelFormat::R24       : return 24;
    case PixelFormat::R32       : return 32;
    case PixelFormat::R32F      : return 32;
        //case PixelFormat::R32FG32FB32F : return 32;
    case PixelFormat::RGB16F    : return 16;
    case PixelFormat::RGBA16F   : return 16;
    case PixelFormat::RGB32F    : return 32;
    case PixelFormat::RGBA32F   : return 32;
    case PixelFormat::A1R5G5B5  : return 5;
    case PixelFormat::B8G8R8A8  : return 8;
    case PixelFormat::B8G8R8    : return 8;
    default: return 0;
    }
}

// static
uint8_t PixelFormat::getGreenBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8  : return 8;
    case PixelFormat::R8G8B8    : return 8;
    case PixelFormat::R5G6B5    : return 6;
    case PixelFormat::R5G5B5A1  : return 5;
        //case PixelFormat::R32FG32FB32F : return 32;
    case PixelFormat::RGB16F    : return 16;
    case PixelFormat::RGBA16F   : return 16;
    case PixelFormat::RGB32F    : return 32;
    case PixelFormat::RGBA32F   : return 32;
    case PixelFormat::A1R5G5B5  : return 5;
    case PixelFormat::B8G8R8A8  : return 8;
    case PixelFormat::B8G8R8    : return 8;
    default: return 0;
    }
}

// static
uint8_t PixelFormat::getBlueBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8  : return 8;
    case PixelFormat::R8G8B8    : return 8;
    case PixelFormat::R5G6B5    : return 5;
    case PixelFormat::R5G5B5A1  : return 5;
        //case PixelFormat::R32FG32FB32F : return 32;
    case PixelFormat::RGB16F    : return 16;
    case PixelFormat::RGBA16F   : return 16;
    case PixelFormat::RGB32F    : return 32;
    case PixelFormat::RGBA32F   : return 32;
    case PixelFormat::A1R5G5B5  : return 5;
    case PixelFormat::B8G8R8A8  : return 8;
    case PixelFormat::B8G8R8    : return 8;
    default: return 0;
    }
}

// static
uint8_t PixelFormat::getAlphaBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8  : return 8;
    case PixelFormat::R5G5B5A1  : return 1;
    case PixelFormat::A1R5G5B5  : return 1;
    case PixelFormat::B8G8R8A8  : return 8;
    case PixelFormat::RGBA16F   : return 16; // New
    case PixelFormat::RGBA32F   : return 32; // New
    default                     : return 0;
    }
}

// static
uint8_t PixelFormat::getDepthBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::D24S8 : return 24;
    case PixelFormat::D32F : return 32;
    case PixelFormat::D32FS8 : return 32;
    default: return 0;
    }
}

// static
uint8_t PixelFormat::getStencilBits( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::D24S8 : return 8;
    case PixelFormat::D32FS8 : return 8;
    case PixelFormat::S8     : return 8;
    default: return 0;
    }
}

// static
PixelFormatConverter::Converter_t
PixelFormatConverter::getConverter( PixelFormat const & src, PixelFormat const & dst )
{
    if ( src == PixelFormat::R32F )
    {
             if ( dst == PixelFormat::R8G8B8A8 ){ return convert_R32F_to_RGBA_8888; }
        else if ( dst == PixelFormat::R8G8B8 )  { return convert_R32F_to_RGB_888; }
        else if ( dst == PixelFormat::R32F )    { return convert_R32F_to_R32F; }
        else                                    { throw std::runtime_error( "No convert_R32F" ); return nullptr; }
    }
    else if ( src == PixelFormat::RGB32F )
    {
             if ( dst == PixelFormat::R8G8B8A8 ){ return convert_RGB32F_to_RGBA_8888; }
        else if ( dst == PixelFormat::R8G8B8 )  { return convert_RGB32F_to_RGB_888; }
        else if ( dst == PixelFormat::R32F )    { return convert_RGB32F_to_RGB_888; }
        else                                    { throw std::runtime_error( "No convert_RGB32F" ); return nullptr; }
    }
    else if ( src == PixelFormat::RGBA32F )
    {
        if ( dst == PixelFormat::R32F )         { return convert_RGBA32F_to_R32F; }
        else                                    { throw std::runtime_error( "No convert_RGBA32F" ); return nullptr; }
    }
    else if ( src == PixelFormat::R8G8B8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R8G8B8A8_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R8G8B8A8_to_R8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8A8_to_R5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8A8_to_R5G6B5" ); return nullptr;   }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8A8_to_B8G8R8; }
   }
   else if ( src == PixelFormat::R8G8B8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_RGB_24_to_RGBA_32; } // BUG_2025_05_11 return convert_R8G8B8_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_RGB24_to_RGB24; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8_to_R5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8_to_R5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8_to_B8G8R8; }
   }
   else if ( src == PixelFormat::R5G5B5A1 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G5B5A1_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G5B5A1_to_R8G8B8;   }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G5B5A1_to_R5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G5B5A1_to_R5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G5B5A1_to_B8G8R8;   }
   }
   else if ( src == PixelFormat::R5G6B5 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G6B5_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G6B5_to_R8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G6B5_to_R5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G6B5_to_R5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G6B5_to_B8G8R8; }
   }
   else if ( src == PixelFormat::B8G8R8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_B8G8R8_to_R8G8B8;   }
      else                                      { throw std::runtime_error( "No convert_B8G8R8_to_Fancy" ); }
   }
   else if ( src == PixelFormat::B8G8R8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8A8_to_R8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { throw std::runtime_error( "No convert_B8G8R8A8_to_R8G8B8" ); }
      else                                      { throw std::runtime_error( "No convert_B8G8R8A8_to_Fancy" ); }
   }
   else
   {
      std::ostringstream o;
      o << "Unknown color converter from (" << src.str() << ") _to_ (" << dst.str() << ")";
      throw std::runtime_error( o.str() );
   }
   return nullptr;
}

// static
void
PixelFormatConverter::convert_RGB24_to_RGB24( void const * src, void* dst, size_t n )
{
    size_t nBytes = n * sizeof(uint8_t) * 3;
    ::memcpy( dst, src, nBytes );
}

// static
/*
void
PixelFormatConverter::convert_R8G8B8_to_R8G8B8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 3 );
}
*/

// static
void
PixelFormatConverter::convert_R8G8B8A8_to_R8G8B8( void const * src, void* dst, size_t n )
{
   uint8_t* s = (uint8_t*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[0];
      d[1] = s[1];
      d[2] = s[2];
      s += 4; // source[3] is alpha, jumps over A.
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5_to_R8G8B8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t * d = (uint8_t *)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[2] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[0] = (*s & 0x001f) << 3;
      s += 1; // += 2 Byte
      d += 3; // += 3 Byte
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t * d = (uint8_t *)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[3] = 255;
      d[2] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[0] = (*s & 0x001f) << 3;
      s += 1;
      d += 4;
   }
}

// static
void
PixelFormatConverter::convert_R5G5B5A1_to_R8G8B8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t*  d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[2] = (*s & 0x7c00) >> 7;
      d[1] = (*s & 0x03e0) >> 2;
      d[0] = (*s & 0x1f) << 3;
      s += 1;
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R8G8B8A8_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 4 );
}

// static
uint32_t
PixelFormatConverter::R5G5B5A1_to_R8G8B8A8( uint16_t color )
{
   return (( -( (int32_t) color & 0x00008000 ) >> (int32_t) 31 ) & 0xFF000000 )
        | (( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4)
        | (( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1)
        | (( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2);
}

// static
void
PixelFormatConverter::convert_R5G5B5A1_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint32_t* d = (uint32_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G5B5A1_to_R8G8B8A8(*s++);
   }
}

// static
void PixelFormatConverter::convert_R5G5B5A1_to_R5G5B5A1( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 2 );
}

/*
// static
void PixelFormatConverter::convert_R8G8B8_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   uint8_t* s = (uint8_t*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[0];
      d[1] = s[1];
      d[2] = s[2];
      d[3] = 255;
      s += 3;
      d += 4;
   }
}
*/
// static
void PixelFormatConverter::convert_RGB_24_to_RGBA_32( void const * src, void* dst, size_t n )
{
    uint8_t* pSrc = (uint8_t*)src;
    uint32_t* pDst = (uint32_t*)dst;

    for ( size_t i = 0; i < n; ++i )
    {
        uint8_t r = *pSrc++;
        uint8_t g = *pSrc++;
        uint8_t b = *pSrc++;
        *pDst++ = dbRGBA(r,g,b);
    }
}

// static
uint16_t
PixelFormatConverter::R5G6B5_to_R5G5B5A1( uint16_t color ) // Returns A1R5G5B5 Color from R5G6B5 color
{
   return 0x8000 | ( ( ( color & 0xFFC0 ) >> 1 ) | ( color & 0x1F ) );
}

// Used in: ImageWriterTGA

// static
void
PixelFormatConverter::convert_R5G6B5_to_R5G5B5A1( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint16_t* d = (uint16_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G6B5_to_R5G5B5A1( *s++ );
   }
}

// Used in: ImageWriterBMP

// static
void
PixelFormatConverter::convert24Bit_to_24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr )
{
   if ( !in || !out ) return;
   int32_t const lineWidth = 3 * width;
   if ( flip )
   {
      out += lineWidth * height;
   }

   for ( int32_t y = 0; y < height; ++y )
   {
      if ( flip )
      {
         out -= lineWidth;
      }
      if ( bgr )
      {
         for ( int32_t x = 0; x < lineWidth; x += 3 )
         {
            out[ x+0 ] = in[ x+2 ];
            out[ x+1 ] = in[ x+1 ];
            out[ x+2 ] = in[ x+0 ];
         }
      }
      else
      {
         ::memcpy( out, in, lineWidth );
      }
      if ( !flip )
      {
         out += lineWidth;
      }
      in += lineWidth;
      in += linepad;
   }
}

// Used in: ImageWriterBMP

// static
void
PixelFormatConverter::convert_R8G8B8A8_to_B8G8R8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      s += 4; // s[3] is alpha and skipped, jumped over.
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R8G8B8_to_B8G8R8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      s += 3;
      d += 3;
   }
}


// static
void
PixelFormatConverter::convert_B8G8R8_to_R8G8B8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8_to_B8G8R8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R8G8B8A8_to_B8G8R8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      d[3] = s[3];
      s += 4;
      d += 4;
   }
}

// static
void
PixelFormatConverter::convert_B8G8R8A8_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8A8_to_B8G8R8A8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R5G5B5A1_to_B8G8R8( void const * src, void* dst, size_t n )
{
   uint16_t const* s = (uint16_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = (*s & 0x7c00) >> 7;
      d[1] = (*s & 0x03e0) >> 2;
      d[2] = (*s & 0x1f) << 3;
      s += 1;
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5_to_B8G8R8( void const * src, void* dst, size_t n )
{
   uint16_t const* s = (uint16_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[2] = (*s & 0x001f) << 3;
      s += 1;
      d += 3;
   }
}



// static
void
PixelFormatConverter::convert_R8G8B8_to_B8G8R8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      d[3] = 255;
      s += 3;
      d += 4;
   }
}


// used in bmp format
void
PixelFormatConverter::convert_B8G8R8_to_R8G8B8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];   // B <= B
      d[1] = s[1];   // G <= G
      d[2] = s[0];   // R <= R
      d[3] = 255;    // A = 255
      s += 3;
      d += 4;
   }
}

// used in HDR exr format
void
PixelFormatConverter::convert_R32F_to_R32F( void const * src, void* dst, size_t n )
{
    ::memcpy( dst, src, n * sizeof(float) );
}

// used in HDR exr format
void
PixelFormatConverter::convert_R32F_to_RGB_888( void const * src, void* dst, size_t n )
{
    float const* s = (float const*)src;
    uint8_t* d = (uint8_t*)dst;
    for ( size_t i = 0; i < n; ++i )
    {
        uint8_t r = static_cast<uint8_t>(std::clamp( int(std::lround(s[0] * 255.0f)), 0, 255));
        d[0] = r;
        d[1] = r;
        d[2] = r;
        s += 1;
        d += 3;
    }
}

// used in HDR exr format
void
PixelFormatConverter::convert_R32F_to_RGBA_8888( void const * src, void* dst, size_t n )
{
    auto s = reinterpret_cast<float const*>(src);
    auto d = reinterpret_cast<uint8_t*>(dst);
    for ( size_t i = 0; i < n; ++i )
    {
        uint8_t r = static_cast<uint8_t>(std::clamp( int(std::lround(s[0] * 255.0f)), 0, 255));
        d[0] = r;
        d[1] = r;
        d[2] = r;
        d[3] = 255;
        s += 1;
        d += 4;
    }
}

// used in HDR exr format
void
PixelFormatConverter::convert_RGB32F_to_RGB_888( void const * src, void* dst, size_t n )
{
    auto s = reinterpret_cast<float const*>(src);
    auto d = reinterpret_cast<uint8_t*>(dst);
    for ( size_t i = 0; i < n; ++i )
    {
        d[0] = static_cast<uint8_t>(std::clamp( std::lround(255.0f * s[0]), long(0), long(255)));
        d[1] = static_cast<uint8_t>(std::clamp( std::lround(255.0f * s[1]), long(0), long(255)));
        d[2] = static_cast<uint8_t>(std::clamp( std::lround(255.0f * s[2]), long(0), long(255)));
        s += 3;
        d += 3;
    }
}

// used in HDR exr format
void
PixelFormatConverter::convert_RGB32F_to_RGBA_8888( void const * src, void* dst, size_t n )
{
    float const* s = (float const*)src;
    uint8_t* d = (uint8_t*)dst;
    for ( size_t i = 0; i < n; ++i )
    {
        d[0] = std::clamp( int(std::lround(s[0] * 255.0f)), 0, 255);
        d[1] = std::clamp( int(std::lround(s[1] * 255.0f)), 0, 255);
        d[2] = std::clamp( int(std::lround(s[2] * 255.0f)), 0, 255);
        d[3] = 255;    // A = 255
        s += 3;
        d += 4;
    }
}

// used in HDR exr format
void
PixelFormatConverter::convert_RGB32F_to_R32F( void const * src, void* dst, size_t n )
{
    float const* s = reinterpret_cast<float const*>(src);
    float* d = reinterpret_cast<float*>(dst);
    for ( size_t i = 0; i < n; ++i )
    {
        *d = s[0];
        s += 3;
        d++;
    }
}

// used in HDR exr format
void
PixelFormatConverter::convert_RGBA32F_to_R32F( void const * src, void* dst, size_t n )
{
    float const* s = reinterpret_cast<float const*>(src);
    float* d = reinterpret_cast<float*>(dst);
    for ( size_t i = 0; i < n; ++i )
    {
        *d = s[0];
        s += 4;
        d++;
    }
}

// ===========================================================================
// ===   constructor
// ===========================================================================

Image::Image()
   : m_width( 0 )
   , m_height( 0 )
   , m_pixelFormat( PixelFormat::R8G8B8A8 )
   , m_debug( false )
   , m_bytesPerPixel(0)
{
    //DE_DEBUG("w(",w,"), h(",h,"), pixelCount(",pixelCount,")")
}

Image::Image( int32_t w, int32_t h, PixelFormat fmt )
   : m_width( w )
   , m_height( h )
   , m_pixelFormat( fmt )
   , m_debug( false )
   , m_bytesPerPixel(PixelFormat::getBytesPerPixel( fmt ))
{
    const uint64_t byteCount = uint64_t( w ) * uint64_t( h ) * m_bytesPerPixel;
    m_data.resize( byteCount, 0x00 );
}

Image::Image( int32_t w, int32_t h, uint32_t fillColor, PixelFormat fmt )
   : m_width( w )
   , m_height( h )
   , m_pixelFormat( fmt )
   , m_debug( false )
   , m_bytesPerPixel( PixelFormat::getBytesPerPixel(fmt) )
{
   const uint64_t byteCount = uint64_t( w ) * uint64_t( h ) * m_bytesPerPixel;
   m_data.resize( byteCount, 0x00 );
   fill( fillColor );
}

bool
Image::equals( const Image& other ) const
{
   if ( w() != other.w() ) return false;
   if ( h() != other.h() ) return false;
   if ( pixelFormat() != other.pixelFormat() ) return false;

   return 0 == memcmp( data(), other.data(), other.size() );
}

bool
Image::empty() const
{
    if ( w() < 1 ) { return true; }
    if ( h() < 1 ) { return true; }
    if ( byteCount() > m_data.size() ) { return true; }
    return false;
}

void
Image::clear( bool forceShrink )
{
    m_data.clear();
    if ( forceShrink )
    {
        m_data.shrink_to_fit(); // here it actually frees memory, could be heavy load.
    }

    m_width = 0;
    m_height = 0;
    // Dont touch format
    // Dont touch uri string
}

void
Image::setPixelFormat( PixelFormat colorFormat )
{
   m_pixelFormat = colorFormat;
   m_bytesPerPixel = PixelFormat::getBytesPerPixel( m_pixelFormat );
}

void
Image::setPixel( int32_t x, int32_t y, uint32_t color, bool blend )
{
   if ( m_width < 1 || m_height < 1 )
   {
      DE_ERROR("Empty image")
      return;
   }

   if ( x < 0 || x >= m_width )
   {
      //DE_ERROR("Invalid x = ", x)
      return;
   }

   if ( y < 0 || y >= m_height )
   {
      //DE_ERROR("Invalid y = ", y)
      return;
   }

   /*
   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw )
   {
      //DE_ERROR("Invalid x(",x,") >= w(",m_Width,")")
      return;
   }
   if ( uy >= uh )
   {
      //DE_ERROR("Invalid y(",y,") >= h(",m_Height,")")
      return;
   }
   */

   uint64_t byteOffset = uint64_t( m_width ) * uint32_t(y) + uint32_t(x);
   byteOffset *= m_bytesPerPixel;
   if ( byteOffset + m_bytesPerPixel > m_data.size() )
   {
      DE_ERROR("byteOffset > m_Data.size()")
      return;
   }

   if ( blend )
   {
      color = blendColor( getPixel( x,y), color );
   }

   if ( m_bytesPerPixel >= 4 )
   {
      uint32_t* p = reinterpret_cast< uint32_t* >( m_data.data() + byteOffset );
      *p = color;
   }
   else if ( m_bytesPerPixel == 3 )
   {
      *(m_data.data() + byteOffset+0) = dbRGBA_R( color );
      *(m_data.data() + byteOffset+1) = dbRGBA_G( color );
      *(m_data.data() + byteOffset+2) = dbRGBA_B( color );
   }
   else if ( m_bytesPerPixel == 2 )
   {
      *(m_data.data() + byteOffset+0) = dbRGBA_R( color );
      *(m_data.data() + byteOffset+1) = dbRGBA_G( color );
   }
   else if ( m_bytesPerPixel == 1 )
   {
      *(m_data.data() + byteOffset+0) = dbRGBA_R( color );
   }
   else // if ( m_BytesPerPixel == 1 )
   {
      DE_ERROR("No bps")
   }
}

uint8_t*
Image::getPixels( int32_t x, int32_t y )
{
   if ( m_width < 1 || m_height < 1 ) return nullptr;
   if ( x < 1 || y < 1 ) return nullptr;
   if ( x >= m_width || y >= m_height ) return nullptr;

   uint64_t byteOffset = (uint64_t( m_width ) * uint32_t(y) + uint32_t(x)) * m_bytesPerPixel;
   if ( byteOffset + m_bytesPerPixel > m_data.size() ) return nullptr;
   return m_data.data() + byteOffset;
}

void
Image::setPixel4f( int32_t x, int32_t y, glm::vec4 color, bool blend )
{
    if ( m_width < 1 || m_height < 1 )
    {
        DE_ERROR("Empty image")
        return;
    }

    if ( x < 0 || x >= m_width ) { return; }

    if ( y < 0 || y >= m_height ) { return; }

    uint64_t byteOffset = uint64_t( m_width ) * uint32_t(y) + uint32_t(x);
    byteOffset *= m_bytesPerPixel;
    if ( byteOffset + m_bytesPerPixel > m_data.size() )
    {
        DE_ERROR("byteOffset > m_Data.size()")
        return;
    }

    if ( blend )
    {
        color = blendColor( getPixel4f(x,y), color );
    }

    float* pDst = reinterpret_cast< float* >( m_data.data() + byteOffset );

    if ( m_pixelFormat == PixelFormat::RGBA32F )
    {
        *pDst = color.r; pDst++;
        *pDst = color.g; pDst++;
        *pDst = color.b; pDst++;
        *pDst = color.a;
    }
    else if ( m_pixelFormat == PixelFormat::RGB32F )
    {
        *pDst = color.r; pDst++;
        *pDst = color.g; pDst++;
        *pDst = color.b;
    }
    else if ( m_pixelFormat == PixelFormat::R32F )
    {
        *pDst = color.r;
    }
    else // if ( m_BytesPerPixel == 1 )
    {
        DE_RUNTIME_ERROR("Unsupported pixelFormat ", m_pixelFormat.str());
    }
}

glm::vec4
Image::getPixel4f( int32_t x, int32_t y, const glm::vec4& colorKey ) const
{
    if ( m_width < 1 || m_height < 1 )
    {
        DE_ERROR("Empty image (",x,",",y,")")
        return colorKey;
    }

    if ( m_bytesPerPixel < 4 )
    {
        DE_ERROR("m_BytesPerPixel < 4")
        return colorKey;
    }

    uint32_t ux = uint32_t( x );
    uint32_t uy = uint32_t( y );
    uint32_t uw = uint32_t( m_width );
    uint32_t uh = uint32_t( m_height );
    if ( ux >= uw || uy >= uh )
    {
        DE_ERROR("Coords outside range (",x,",",y,")")
        return colorKey;
    }

    size_t byteOffset = (size_t( m_width ) * uy + ux) * m_bytesPerPixel;
    if ( byteOffset + m_bytesPerPixel > m_data.size() )
    {
        DE_ERROR("byteOffset + m_BytesPerPixel > m_Data.size()")
        return colorKey;
    }

    glm::vec4 color( 0,0,0,1 );

    const float* pSrc = reinterpret_cast< const float* >( m_data.data() + byteOffset );

    color.r = *pSrc;

    if (m_pixelFormat == PixelFormat::R32F)
    {
        //color.a = 1.0f;
    }
    else if (m_pixelFormat == PixelFormat::RGB32F)
    {
        pSrc++;
        color.g = *pSrc;
        pSrc++;
        color.b = *pSrc;
    }
    else if (m_pixelFormat == PixelFormat::RGBA32F)
    {
        pSrc++;
        color.g = *pSrc;
        pSrc++;
        color.b = *pSrc;
        pSrc++;
        color.a = *pSrc;
    }
    else
    {
        DE_RUNTIME_ERROR("Unsupported float pixelFormat ", m_pixelFormat.str());
    }

    return color;
}

float
Image::getPixelf( int32_t x, int32_t y, float colorKey ) const
{
    if ( m_width < 1 || m_height < 1 )
    {
        DE_ERROR("Empty image (",x,",",y,")")
        return colorKey;
    }

    if ( m_bytesPerPixel < 4 )
    {
        DE_ERROR("m_BytesPerPixel < 4")
        return colorKey;
    }

    uint32_t ux = uint32_t( x );
    uint32_t uy = uint32_t( y );
    uint32_t uw = uint32_t( m_width );
    uint32_t uh = uint32_t( m_height );
    if ( ux >= uw || uy >= uh )
    {
        DE_ERROR("Coords outside range (",x,",",y,")")
        return colorKey;
    }

    size_t byteOffset = (size_t( m_width ) * uy + ux) * m_bytesPerPixel;
    if ( byteOffset + m_bytesPerPixel > m_data.size() )
    {
        DE_ERROR("byteOffset + m_BytesPerPixel > m_Data.size()")
        return colorKey;
    }

    return *reinterpret_cast< float const * const >( m_data.data() + byteOffset );
}

uint32_t
Image::getPixel( int32_t x, int32_t y, uint32_t colorKey ) const
{
    if ( m_width < 1 || m_height < 1 )
    {
        if (m_debug) { DE_ERROR("Empty image (",x,",",y,")") }
        return colorKey;
    }

    if ( x < 0 || x >= m_width )
    {
        if (m_debug) { DE_ERROR("X-coord outside range (",x,",",y,"), img(",str(),")") }
        return colorKey;
    }

    if ( y < 0 || y >= m_height )
    {
        if (m_debug) { DE_ERROR("Y-coord outside range (",x,",",y,"), img(",str(),")") }
        return colorKey;
    }

    auto fma64u = [] (uint64_t a, uint64_t b, uint64_t c)
    {
        return (a*b)+c;
    };

    const uint64_t byteOffset = fma64u( m_width, y, x) * m_bytesPerPixel;
    if ( byteOffset + m_bytesPerPixel > m_data.size() )
    {
        if (m_debug) { DE_ERROR("ByteOffset outside range (",x,",",y,"), img(",str(),")") }
        return colorKey;
    }

    if ( m_bytesPerPixel == 4 )
    {
        return *reinterpret_cast< uint32_t const * const >( m_data.data() + byteOffset );
    }
    else if ( m_bytesPerPixel == 3 )
    {
        uint8_t r = *(m_data.data() + byteOffset);
        uint8_t g = *(m_data.data() + byteOffset + 1);
        uint8_t b = *(m_data.data() + byteOffset + 2);
        return dbRGBA( r,g,b );
    }
    else if ( m_bytesPerPixel == 2 )
    {
        uint8_t r = *(m_data.data() + byteOffset);
        uint8_t g = *(m_data.data() + byteOffset + 1);
        return dbRGBA( r,g,0 );
    }
    else if ( m_bytesPerPixel == 1 )
    {
        uint8_t r = *(m_data.data() + byteOffset);
        return dbRGBA( r,r,r );
    }
    else // if ( m_BytesPerPixel > 4 )
    {
        DE_WARN("Unsupported format (",x,",",y,"), img(",str(),")")
        return *reinterpret_cast< uint32_t const * const >( m_data.data() + byteOffset );
    }
}

void
Image::resize( int32_t w, int32_t h, PixelFormat pixelFormat )
{
    setPixelFormat( pixelFormat );
    resize(w,h);
}

void
Image::resize( int32_t w, int32_t h )
{
    if ( w < 1 || h < 1 )
    {
        clear();
        return; // cleared image
    }

    uint64_t byteCount = uint64_t( w ) * uint64_t( h ) * m_bytesPerPixel;
    m_data.resize( byteCount, 0x00 );
    m_width = w;
    m_height = h;
}

void Image::shrink_to_fit() { return m_data.shrink_to_fit(); }

void
Image::fill( const glm::vec4& color )
{
    if (empty()) { return; } // Nothing todo

    if (m_pixelFormat == PixelFormat::R32F)
    {
        auto p = writePtr< float >();
        auto n = uint64_t( m_width ) * size_t( m_height );
        for ( size_t i = 0; i < n; ++i )
        {
            *p++ = color.r;
        }
    }
    /*
    else if (m_pixelFormat == PixelFormat::R8G8B8)
    {
        uint8_t r = dbRGBA_R(color);
        uint8_t g = dbRGBA_G(color);
        uint8_t b = dbRGBA_B(color);
        uint8_t * pixels = writePtr< uint8_t >();
        for ( size_t i = 0; i < uint64_t( m_Width ) * size_t( m_Height ); ++i )
        {
            *pixels++ = r;
            *pixels++ = g;
            *pixels++ = b;
        }
    }
*/
    else
    {
        DE_ERROR("Unsupported format ", str())
    }
}

void
Image::fill( uint32_t color )
{
    if (empty())
    {
        return;
    }

    if (m_pixelFormat == PixelFormat::R8G8B8A8)
    {
        uint32_t * pixels = writePtr< uint32_t >();
        for ( size_t i = 0; i < uint64_t( m_width ) * size_t( m_height ); ++i )
        {
            *pixels = color;
            pixels++;
        }
    }
/*
    else if (m_pixelFormat == PixelFormat::R8G8B8)
    {
        uint8_t r = dbRGBA_R(color);
        uint8_t g = dbRGBA_G(color);
        uint8_t b = dbRGBA_B(color);
        uint8_t * pixels = writePtr< uint8_t >();
        for ( size_t i = 0; i < uint64_t( m_Width ) * size_t( m_Height ); ++i )
        {
            *pixels++ = r;
            *pixels++ = g;
            *pixels++ = b;
        }
    }
*/
    else
    {
        DE_ERROR("Unsupported format ", str())
    }
}

void
Image::fillZero()
{
    if (m_data.empty()) return;
   ::memset( m_data.data(), 0x00, m_data.size() );
}

void
Image::floodFill( int32_t x, int32_t y, uint32_t newColor, uint32_t oldColor )
{
    //double const sec0 = Timer::GetTimeInSeconds();

    int32_t const w = m_width;
    int32_t const h = m_height;

    //DWORD oldColor=MemblockReadDword(mem,memX,memY,x,y);
    if ( getPixel( x,y ) == newColor ) return; // nothing todo

    int32_t lg, rg;
    int32_t px = x;

    while ( getPixel( x,y ) == oldColor )
    {
        setPixel( x,y, newColor );
        x--;
    }
    lg = x+1;
    x = px+1;
    while ( getPixel( x,y ) == oldColor )
    {
        setPixel( x,y, newColor );
        x++;
    }
    rg = x-1;
    for ( x = rg; x >= lg; --x )
    {
        if ( ( getPixel( x,y-1 ) == oldColor ) && ( y > 1 ) )
        {
            floodFill( x, y-1, newColor, oldColor );
        }
        if ( ( getPixel( x,y+1 ) == oldColor ) && ( y < h - 1 ) )
        {
            floodFill( x, y+1, newColor, oldColor );
        }
    }

    //double const sec1 = Timer::GetTimeInSeconds();
    //double const ms = 1000.0*(sec1 - sec0);
    //DE_INFO( "w = ", w, ", h = ", h, ", ms = ", ms )
}

void
Image::flipVertical()
{
   std::vector< uint8_t > row_buf1;
   std::vector< uint8_t > row_buf2;
   for ( int32_t y = 0; y < h()/2; ++y )
   {
      copyRowFrom( row_buf1, y );
      copyRowFrom( row_buf2, h() - 1 - y );
      copyRowTo( row_buf1, h() - 1 - y );
      copyRowTo( row_buf2, y );
   }
}

void
Image::flipHorizontal()
{
   std::vector< uint8_t > col1;
   std::vector< uint8_t > col2;
   for ( int32_t x = 0; x < w()/2; ++x )
   {
      copyColFrom( col1, x );
      copyColFrom( col2, w() - 1 - x );
      copyColTo( col1, w() - 1 - x );
      copyColTo( col2, x );
   }
}

Image
Image::copy( Recti const & pos ) const
{
   int w = pos.w;
   int h = pos.h;
   Image dst( w,h, m_pixelFormat );
   for ( int j = 0; j < h; ++j )
   {
      for ( int i = 0; i < w; ++i )
      {
         auto color = getPixel( pos.x + i, pos.y + j );
         dst.setPixel( i, j, color );
      }
   }
   return dst;
}

void
Image::copyRowFrom( std::vector< uint8_t > & out, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( w() ) * m_bytesPerPixel;
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   out.resize( bpr, 0x00 );
   ::memcpy( out.data(), data() + (bpr * y), bpr );
}

void
Image::copyRowTo( std::vector< uint8_t > const & row, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( w() ) * m_bytesPerPixel;
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   if ( bpr != row.size() )
   {
      DE_ERROR("Cant copy incomplete row ", y )
      return;
   }

   ::memcpy( data() + (bpr * y), row.data(), bpr );
}

void
Image::copyColFrom( std::vector< uint8_t > & col, int32_t x )
{
   if ( uint32_t( x ) >= uint32_t( m_width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

   col.resize( size_t(m_height) * m_bytesPerPixel );

   uint8_t* dst = col.data();
   for ( int32_t y = 0; y < m_height; ++y )
   {
      uint8_t* src = getPixels(x,y);
      ::memcpy( dst, src, m_bytesPerPixel );
      dst += m_bytesPerPixel;
   }
}

void
Image::copyColTo( std::vector< uint8_t > const & col, int32_t x )
{
//   if ( uint32_t( x ) >= uint32_t( m_Width ) )
//   {
//      DE_DEBUG("Invalid col index(",y,")")
//      return;
//   }

//   uint64_t bpr = uint64_t( w() ) * getBytesPerPixel();
//   if ( bpr < 1 )
//   {
//      DE_DEBUG("Nothing todo")
//      return;
//   }

//   if ( bpr != row.size() )
//   {
//      DE_ERROR("Cant copy incomplete row ", y )
//      return;
//   }

//   ::memcpy( data() + (bpr * y), row.data(), bpr );
   if ( uint32_t( x ) >= uint32_t( m_width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

//   uint32_t bpp = getBytesPerPixel();

//   uint32_t bpc = uint32_t(m_Height) * bpp;
//   col.resize( bpc );

//   uint8_t* dst = col.data();
//   for ( int32_t y = 0; y < m_Height; ++y )
//   {
//      uint8_t* src = getPixels(x,y);
//      ::memcpy( dst, src, bpp );
//      dst += bpp;
//   }
}

void
Image::switchRB()
{
   for ( int32_t y = 0; y < h(); ++y )
   {
      for ( int32_t x = 0; x < w(); ++x )
      {
         uint32_t c = getPixel( x, y );
         setPixel( x, y, dbRGBA( dbRGBA_B( c ), dbRGBA_G( c ), dbRGBA_R( c ), dbRGBA_A( c ) ) );
      }
   }
}

uint8_t*
Image::getRow( int32_t y )
{
   // TODO: Necessary?
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   // CheckPoint 1: Test user param row-y if valid
   if ( y < 0 || y >= m_height )
   {
      DE_DEBUG("Invalid y(",y,") of h(",m_height,")")
      return nullptr;
   }

   // Compute linear byte index...
   uint64_t const byteOffset = uint64_t( y ) * stride();

   // CheckPoint 2: Test linear byte index if valid
   if ( byteOffset >= m_data.size() )
   {
      DE_DEBUG("Invalid byteOffset(",byteOffset,"), y(",y,") of h(",m_height,")")
      return nullptr;
   }

   // Output address (base+offset) of first pixel of row y...
   return m_data.data() + byteOffset;
}

uint8_t*
Image::getColumn( int32_t x )
{
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   if ( x < 0 || x >= m_width )
   {
      DE_DEBUG("Invalid x(",x,") of w(",m_width,")")
      return nullptr;
   }

   if ( x >= int32_t(m_data.size()) )
   {
      DE_DEBUG("Invalid x(",x,") > m_Data.size(",m_data.size(),")")
      return nullptr;
   }
   return m_data.data() + x;
}


std::string
Image::str( bool withUri, bool withBytes ) const
{
    std::ostringstream o;
    if ( empty() )
    {
        o << "empty";
    }
    else
    {
        o << "[" << dbStrBytes( computeMemoryConsumption() ) << "] ";
        o << m_width << "," << m_height << "," << pixelFormatStr();
        if ( withUri ) o << "," << m_uri;

        if ( withBytes )
        {
            o << "\n";
            const float* pSrc = readPtr<float>();
            for (int y = 0; y < h(); ++y)
            {
                std::ostringstream q;
                for (int x = 0; x < w(); ++x)
                {
                    q << *pSrc++ << ",";
                }
                o << q.str() << "\n";
            }
        }
    }

    return o.str();
}



Image
Image::convert( PixelFormat outputFormat ) const
{
    if ( this->w() < 1 || this->h() < 1 )
    {
        DE_ERROR("Empty image cannot be converted." )
        return Image();
    }

    auto pConverter = PixelFormatConverter::getConverter( this->pixelFormat(), outputFormat );
    if ( !pConverter )
    {
        DE_ERROR("Cannot convert image(", str(), ") to format ", outputFormat.str() )
        return Image();
    }

    Image outputImage( this->w(), this->h(), outputFormat );
    pConverter(this->data(), outputImage.data(), this->pixelCount());
    return outputImage;
}

// ===========================================================================
// ===   ImageLoadOptions
// ===========================================================================

/*
inline void repairBadAlpha( Image & img )
{
    for ( int y = 0; y < img.h(); ++y )
    {
        for ( int x = 0; x < img.w(); ++x )
        {
            const u32 color = img.getPixel(x,y);
            const uint8_t r = dbRGBA_R(color);
            const uint8_t g = dbRGBA_G(color);
            const uint8_t b = dbRGBA_B(color);
            uint8_t a = dbRGBA_A(color);
            if (r > 0 || g > 0 || b > 0)
            {
                a = 255;
            }
            img.setPixel(x,y,dbRGBA(r,g,b,a));
        }
    }
}
*/

inline void repairBadAlpha( Image & img )
{
    for ( int y = 0; y < img.h(); ++y )
    {
        for ( int x = 0; x < img.w(); ++x )
        {
            const u32 color = img.getPixel(x,y);
            uint8_t r = dbRGBA_R(color);
            uint8_t g = dbRGBA_G(color);
            uint8_t b = dbRGBA_B(color);
            uint8_t a = dbRGBA_A(color);
            if (a < 10)
            {
                r = 0;
                g = 0;
                b = 0;
                a = 0;
            }
            else
            {
                a = 255;
            }
            img.setPixel(x,y,dbRGBA(r,g,b,a));
        }
    }
}

inline void applyImageLoadOptions( Image & img, const ImageLoadOptions & options )
{
    if ( options.outputFormat != PixelFormat::Unknown
        && options.outputFormat != img.pixelFormat() )
    {
        Image tmp = img.convert(options.outputFormat);
        img = tmp;
    }

    /*
    if ( options.repairBadAlpha )
    {
        repairBadAlpha( img );
    }

    if ( options.rotate90 )
    {
        img = ImageRotate::rotateRight( img );
    }

    if ( options.m_SearchColor != options.m_ReplaceColor )
    {
    // DE_MAIN_DEBUG("ReplaceColor")
    ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    if ( options.m_AutoSaturate )
    {
    // DE_MAIN_DEBUG("Saturate")
    ImagePainter::autoSaturate( img );
    }

    if ( options.m_Brighten != 1.0f )
    {
    ImagePainter::brighten( img, options.m_Brighten );
    }
    if ( options.m_Gamma != 1.0f )
    {
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    if ( options.m_Contrast != 1.0f )
    {
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    */
    // if ( options.m_SearchColor != options.m_ReplaceColor )
    // {
    // // DE_MAIN_DEBUG("ReplaceColor")
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // if ( options.m_AutoSaturate )
    // {
    // // DE_MAIN_DEBUG("Saturate")
    // ImagePainter::autoSaturate( img );
    // }

    // if ( options.m_Brighten != 1.0f )
    // {
    // ImagePainter::brighten( img, options.m_Brighten );
    // }
    // if ( options.m_Gamma != 1.0f )
    // {
    // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // if ( options.m_Contrast != 1.0f )
    // {
    // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // }

}




/*
bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
    std::string loadSuffix = FileSystem::fileSuffix( loadName );
    std::string saveSuffix = FileSystem::fileSuffix( saveName );
    if ( loadSuffix == saveSuffix )
    {
        //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
        return FileSystem::copyFile( loadName, saveName );
    }

    loadName = FileSystem::makeAbsolute( loadName );
    saveName = FileSystem::makeAbsolute( saveName );

    Image img;
    bool ok = loadImage( img, loadName );
    if ( ok )
    {
        //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
        ok = saveImage( img, saveName );
        if ( ok )
        {
            DE_OK("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
        }
        else
        {
            DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
        }
    }
    else
    {
        DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
    }

    return ok;
}
*/


// ===========================================================================
// ===   ImageCodecManager
// ===========================================================================

ImageCodecManager::ImageCodecManager()
{
    initThreadPool();

#ifdef DE_IMAGE_READER_JPG_ENABLED
   m_Reader.push_back( new image::ImageReaderJPG );
#endif
#ifdef DE_IMAGE_WRITER_JPG_ENABLED
   m_Writer.push_back( new image::ImageWriterJPG );
#endif

#ifdef DE_IMAGE_READER_PNG_ENABLED
   m_Reader.push_back( new image::ImageReaderPNG );
#endif
#ifdef DE_IMAGE_WRITER_PNG_ENABLED
   m_Writer.push_back( new image::ImageWriterPNG );
#endif

#ifdef DE_IMAGE_READER_WEBP_ENABLED
    m_Reader.push_back( new image::ImageReaderWEBP );
#endif
#ifdef DE_IMAGE_WRITER_WEBP_ENABLED
    m_Writer.push_back( new image::ImageWriterWEBP );
#endif

#ifdef DE_IMAGE_READER_XPM_ENABLED
    m_Reader.push_back( new image::ImageReaderXPM );
#endif
#ifdef DE_IMAGE_WRITER_XPM_ENABLED
    m_Writer.push_back( new image::ImageWriterXPM );
#endif

#ifdef DE_IMAGE_READER_EXR_ENABLED
    m_Reader.push_back( new image::ImageReaderEXR );
#endif
#ifdef DE_IMAGE_WRITER_EXR_ENABLED
    m_Writer.push_back( new image::ImageWriterEXR );
#endif


// NEW:

#ifdef DE_IMAGE_READER_BMP_ENABLED
   m_Reader.push_back( new image::ImageReaderBMP );
#endif
#ifdef DE_IMAGE_WRITER_BMP_ENABLED
   m_Writer.push_back( new image::ImageWriterBMP );
#endif

#ifdef DE_IMAGE_READER_DDS_ENABLED
   m_Reader.push_back( new image::ImageReaderDDS );
#endif
#ifdef DE_IMAGE_WRITER_DDS_ENABLED
   m_Writer.push_back( new image::ImageWriterDDS );
#endif

#ifdef DE_IMAGE_READER_GIF_ENABLED
   m_Reader.push_back( new image::ImageReaderGIF );
#endif
#ifdef DE_IMAGE_WRITER_GIF_ENABLED
   m_Writer.push_back( new image::ImageWriterGIF );
#endif

#ifdef DE_IMAGE_READER_ICO_ENABLED
   m_Reader.push_back( new image::ImageReaderICO );
#endif
#ifdef DE_IMAGE_WRITER_ICO_ENABLED
   m_Writer.push_back( new image::ImageWriterICO );
#endif

#ifdef DE_IMAGE_READER_TGA_ENABLED
   m_Reader.push_back( new image::ImageReaderTGA );
#endif
#ifdef DE_IMAGE_WRITER_TGA_ENABLED
   m_Writer.push_back( new image::ImageWriterTGA );
#endif

#ifdef DE_IMAGE_READER_TIF_ENABLED
   m_Reader.push_back( new image::ImageReaderTIF );
#endif
#ifdef DE_IMAGE_WRITER_TIF_ENABLED
   m_Writer.push_back( new image::ImageWriterTIF );
#endif

#ifdef DE_IMAGE_READER_RGB_ENABLED
   m_Reader.push_back( new image::ImageReaderRGB );
#endif
#ifdef DE_IMAGE_WRITER_RGB_ENABLED
   m_Writer.push_back( new image::ImageWriterRGB );
#endif

#ifdef DE_IMAGE_READER_WAL_ENABLED
   m_Reader.push_back( new image::ImageReaderWAL );
#endif
#ifdef DE_IMAGE_WRITER_WAL_ENABLED
   m_Writer.push_back( new image::ImageWriterWAL );
#endif

#ifdef DE_IMAGE_READER_PSD_ENABLED
   m_Reader.push_back( new image::ImageReaderPSD );
#endif
#ifdef DE_IMAGE_WRITER_PSD_ENABLED
   m_Writer.push_back( new image::ImageWriterPSD );
#endif

#ifdef DE_IMAGE_READER_PPM_ENABLED
   m_Reader.push_back( new image::ImageReaderPPM );
#endif
#ifdef DE_IMAGE_WRITER_PPM_ENABLED
   m_Writer.push_back( new image::ImageWriterPPM );
#endif

#ifdef DE_IMAGE_READER_PCX_ENABLED
   m_Reader.push_back( new image::ImageReaderPCX );
#endif
#ifdef DE_IMAGE_WRITER_PCX_ENABLED
   m_Writer.push_back( new image::ImageWriterPCX );
#endif


#ifdef DE_IMAGE_WRITER_HTML_ENABLED
   m_Writer.push_back( new image::ImageWriterHtmlTable );
#endif

#ifdef _DEBUG
   DE_DEBUG("Add image codec readers and writers")
   DE_DEBUG("Supported image reader: ", m_Reader.size())
   DE_DEBUG("Supported image writer: ", m_Writer.size())
   // AsciiArt::test();
#endif
}

ImageCodecManager::~ImageCodecManager()
{
#ifdef _DEBUG
    DE_DEBUG("Released ",m_Reader.size()," ImageReader")
    DE_DEBUG("Released ",m_Writer.size()," ImageWriter")
    // std::string load_ext = //getSupportedReadExtensions();
    // std::string save_ext = //getSupportedWriteExtensions();
    // DE_DEBUG("Released ",m_Reader.size()," ImageReader with FileFormats: ", load_ext )
    // DE_DEBUG("Released ",m_Writer.size()," ImageWriter with FileFormats: ", save_ext )
#endif
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      if ( m_Reader[ i ] )
      {
         delete m_Reader[ i ];
      }
   }
   m_Reader.clear();

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      if ( m_Writer[ i ] )
      {
         delete m_Writer[ i ];
      }
   }
   m_Writer.clear();

   // clearImages();
}

// static
std::shared_ptr< ImageCodecManager >
ImageCodecManager::get()
{
   static std::shared_ptr< ImageCodecManager > s_ImageManager( new ImageCodecManager() );
   return s_ImageManager;
}


// =====================
//   Convert images
// =====================
bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
    std::string loadSuffix = FileSystem::fileSuffix( loadName );
    std::string saveSuffix = FileSystem::fileSuffix( saveName );
    if ( loadSuffix == saveSuffix )
    {
        //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
        return FileSystem::copyFile( loadName, saveName );
    }

    loadName = FileSystem::makeAbsolute( loadName );
    saveName = FileSystem::makeAbsolute( saveName );

    Image img;
    bool ok = loadImage( img, loadName );
    if ( ok )
    {
        //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
        ok = saveImage( img, saveName );
        if ( ok )
        {
            //DE_OK("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
        }
        else
        {
            //DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
        }
    }
    else
    {
        //DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
    }

    return ok;
}

bool
ImageCodecManager::loadImage( Image & img, uint8_t const* p, uint64_t n, std::string uri, ImageLoadOptions const & options ) const
{
    if ( !p ) { DE_ERROR("!p") return false; }

    if ( n < 12 ) { DE_ERROR("n(",n,") < 12") return false; }

    PerformanceTimer timer;

    if (options.debugLog)
    {
        timer.start();
    }

    std::string ext = FileSystem::fileSuffix( uri );
    if (ext.empty())
    {
        const auto fileMagic = FileMagic::getFileMagic( p );
        if (fileMagic)
        {
            ext = FileMagic::getString(fileMagic);
        }
        else
        {
            auto b0 = StringUtil::hex(p[0]);
            auto b1 = StringUtil::hex(p[1]);
            auto b2 = StringUtil::hex(p[2]);
            auto b3 = StringUtil::hex(p[3]);
            DE_ERROR("Did not find FileMagic for bytes b0(",b0,"), b1(",b1,"), b2(",b2,"), b3(",b3,")")
        }
    }

    IImageReader* reader = getReaderFromExtension( ext );
    if ( !reader )
    {
        std::ostringstream o; o << "No reader for ext("<<ext<<"), uri("<<uri<<"), nBytes(" << n << ")";
        DE_ERROR(o.str())
        if (options.throwOnFail)
        {
            throw std::runtime_error(o.str());
        }
        return false;
    }

    bool ok = reader->load( img, p, n, uri );
    if ( ok )
    {
        applyImageLoadOptions( img, options );
    }

    timer.stop();

    if (options.debugLog)
    {
        if ( ok )
        {
            DE_DEBUG("ms(",timer.ms(),") to load img(",img.str(),")")
        }
        else
        {
            DE_ERROR("Cant load image ms(",timer.ms(),"), uri(",uri,")")
        }
    }
    return ok;
}


bool
ImageCodecManager::loadImage( Image & img, std::string uri, ImageLoadOptions const & options ) const
{
    PerformanceTimer timer;

    if (options.debugLog)
    {
        timer.start();
    }

    uri = FileSystem::makeAbsolute( uri );

    if (!FileSystem::existFile(uri))
    {
        if (options.throwOnFail)
        {
            std::ostringstream o;
            o << "ImageFile does not exist, " << uri;
            throw std::runtime_error(o.str());
        }        
        return false;
    }

    std::string suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        const auto fileMagic = FileMagic::getFileMagicFromFile( uri );
        if (!fileMagic)
        {
            std::ostringstream o;
            o << "Cannot determine fileMagic from empty suffix. " << uri;
            DE_ERROR(o.str())
            if (options.throwOnFail)
            {
                throw std::runtime_error(o.str());
            }
            return false;
        }
        else
        {
            suffix = FileMagic::getString(fileMagic);
        }
    }

    IImageReader* reader = getReaderFromExtension( suffix );
    if ( !reader )
    {
        auto o = dbStr("No ImageReader found file(",uri,"), suffix(", suffix, ")");
        DE_ERROR(o)

        if (options.throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    bool ok = reader->load( img, uri );
    if ( !ok )
    {
        img.setUri( uri );
        timer.stop();
        auto o = dbStr("Cant Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,")");
        DE_ERROR(o)
        if (options.throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    applyImageLoadOptions( img, options );
    img.setUri( uri );

    if (options.debugLog)
    {
        timer.stop();
        DE_OK("[",suffix,"] ", timer.ms(), "ms|", uri,"|", img.str(false))
    }
    return ok;
}

bool
ImageCodecManager::saveImage( Image const & img, std::string uri, uint32_t quality ) const
{
    PerformanceTimer timer;

    bool debugLog = true;

    if (debugLog)
    {
        timer.start();
    }

    if ( uri.empty() )
    {
        DE_ERROR("Empty uri.")
        return false;
    }

    uri = FileSystem::makeAbsolute( uri );

    std::string suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        DE_ERROR("Empty suffix")
        return false;
    }

    IImageWriter* writer = getWriterFromExtension(suffix);
    if (!writer)
    {
        DE_ERROR("No writer for extension ", suffix)
        return false;
    }

    std::string dir = FileSystem::fileDir( uri );
    if (!FileSystem::existDirectory( dir ))
    {
        FileSystem::createDirectory( dir );
    }

    const bool ok = writer->save( img, uri, quality );

    if (debugLog)
    {
        timer.stop();

        if ( ok )
        {
            DE_OK("[",suffix,"] ",timer.ms(),"ms|", uri, "|", img.str(false))
        }
        else
        {
            DE_ERROR("Cant Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.str(false), ")")
        }
    }

    return ok;
}

IImageReader*
ImageCodecManager::getReaderFromExtension( std::string const & suffix ) const
{
    for ( IImageReader* reader : m_Reader )
    {
        if ( reader->isSupportedReadExtension( suffix ) )
        {
            return reader;
        }
    }
    return nullptr;
}

IImageWriter*
ImageCodecManager::getWriterFromExtension( std::string const & suffix ) const
{
    for ( IImageWriter* writer : m_Writer )
    {
        if ( writer->isSupportedWriteExtension( suffix ) )
        {
            return writer;
        }
    }
    return nullptr;
}

/*
std::vector< std::string >
ImageCodecManager::getSupportedReadExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( !reader ) continue;
      for ( std::string const & ext : reader->getSupportedReadExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::vector< std::string >
ImageCodecManager::getSupportedWriteExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( !writer ) continue;
      for ( std::string const & ext : writer->getSupportedWriteExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::string
ImageCodecManager::getSupportedReadExtensions() const
{
   return StringUtil::joinVector( getSupportedReadExtensionVector(), " " );
}

std::string
ImageCodecManager::getSupportedWriteExtensions() const
{
   return StringUtil::joinVector( getSupportedWriteExtensionVector(), " " );
}



bool
ImageCodecManager::isSupportedReadExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( reader && reader->isSupportedReadExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}

bool
ImageCodecManager::isSupportedWriteExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( writer && writer->isSupportedWriteExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}
*/
    
} // end namespace de.


// ===========================================================================
// DarkImageAPI
// ===========================================================================

bool
dbLoadImage(de::Image & img, std::string uri,
            de::ImageLoadOptions const & options )
{
    return de::ImageCodecManager::get()->loadImage( img, uri, options );
}

bool
dbSaveImage(de::Image const & img, std::string uri, uint32_t quality )
{
    return de::ImageCodecManager::get()->saveImage( img, uri, quality );
}

bool
dbLoadImage(de::Image & img, uint8_t const* data, uint64_t bytes, std::string uri,
            de::ImageLoadOptions const & options )
{
    return de::ImageCodecManager::get()->loadImage( img, data, bytes, uri, options );
}

/*
#if defined(DE_IMAGE_READER_XPM_ENABLED)

bool
dbLoadImageXPM( de::Image & dst, std::vector< std::string > const & xpm_data )
{
    //   char const ** start = xpm_data;
    //   size_t count = 0;
    //   while (*start)
    //   {
    //      count++;
    //      start++;
    //   }

    //   std::vector< std::string > lines;
    //   lines.reserve( count );

    //   start = xpm_data;
    //   while (*start)
    //   {
    //      std::string s = *start;
    //      lines.emplace_back( s );
    //      start++;
    //   }

    bool ok = de::image::ImageReaderXPM::loadImpl( dst, xpm_data );
    if ( ok )
    {
        dst.setUri( "const_xpm_data_to_memory.xpm" );
    }
    return ok;
}

#endif
*/

bool
dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst )
{
    de::PerformanceTimer perf;
    perf.start();
    int w = src.w();
    int h = src.h();
    dst.setPixelFormat( de::PixelFormat::R8G8B8A8 );
    dst.resize( w, h );

    float const fmin = 1e7f;

    auto color_converter = [&] ( float height )
    {
        uint32_t color = uint32_t( height + fmin );
        uint8_t r = dbRGBA_R( color );
        uint8_t g = dbRGBA_G( color );
        uint8_t b = dbRGBA_B( color );
        return dbRGBA( r,g,b );
    };

    for ( int32_t y = 0; y < h; ++y )
    {
        for ( int32_t x = 0; x < w; ++x )
        {
            uint32_t const color = src.getPixel( x,y );
            float const height = *reinterpret_cast< float const* >( &color );
            dst.setPixel( x,y, color_converter( height ) );
        }
    }

    perf.stop();
    DE_DEBUG("[Convert] needed ",perf.ms()," ms.")
    return true;
}
