#include <de_image/de_PixelFormatConverter.h>
#include <de_image/de_PixelFormatUtil.h>

namespace de {


// static
PixelFormatConverter::Converter_t
PixelFormatConverter::getConverter( PixelFormat const & src, PixelFormat const & dst )
{
        if ( src == PixelFormat::R8G8B8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R8G8B8A8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R8G8B8A8toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8A8toR5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8A8toR5G6B5" ); return nullptr;   }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8A8toB8G8R8; }
   }
   else if ( src == PixelFormat::R8G8B8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R8G8B8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R8G8B8toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8toR5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8toB8G8R8; }
   }
   else if ( src == PixelFormat::R5G5B5A1 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G5B5A1toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G5B5A1toR8G8B8;   }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G5B5A1toR5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G5B5A1toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G5B5A1toB8G8R8;   }
   }
   else if ( src == PixelFormat::R5G6B5 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G6B5toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G6B5toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G6B5toR5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G6B5toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G6B5toB8G8R8; }
   }
   else if ( src == PixelFormat::B8G8R8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_B8G8R8toR8G8B8;   }
      else                                      { throw std::runtime_error( "No convert_B8G8R8toFancy" ); }
   }
   else if ( src == PixelFormat::B8G8R8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8A8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { throw std::runtime_error( "No convert_B8G8R8A8toR8G8B8" ); }
      else                                      { throw std::runtime_error( "No convert_B8G8R8A8toFancy" ); }
   }
   else
   {
      std::ostringstream o;
      o << "Unknown color converter from (" << PixelFormatUtil::getString( src ) << ") to (" << PixelFormatUtil::getString( dst ) << ")";
      throw std::runtime_error( o.str() );
   }
   return nullptr;
}


// static
void
PixelFormatConverter::convert_R8G8B8toR8G8B8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 3 );
}

// static
void
PixelFormatConverter::convert_R8G8B8A8toR8G8B8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R5G6B5toR8G8B8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R5G6B5toR8G8B8A8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R5G5B5A1toR8G8B8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R8G8B8A8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 4 );
}

// static
uint32_t
PixelFormatConverter::R5G5B5A1toR8G8B8A8( uint16_t color )
{
   return (( -( (int32_t) color & 0x00008000 ) >> (int32_t) 31 ) & 0xFF000000 )
        | (( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4)
        | (( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1)
        | (( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2);
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toR8G8B8A8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint32_t* d = (uint32_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G5B5A1toR8G8B8A8(*s++);
   }
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toR5G5B5A1( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 2 );
}

// static
void
PixelFormatConverter::convert_R8G8B8toR8G8B8A8( void const * src, void* dst, size_t n )
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

// static
uint16_t
PixelFormatConverter::R5G6B5toR5G5B5A1( uint16_t color ) // Returns A1R5G5B5 Color from R5G6B5 color
{
   return 0x8000 | ( ( ( color & 0xFFC0 ) >> 1 ) | ( color & 0x1F ) );
}

// Used in: ImageWriterTGA

// static
void
PixelFormatConverter::convert_R5G6B5toR5G5B5A1( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint16_t* d = (uint16_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G6B5toR5G5B5A1( *s++ );
   }
}

// Used in: ImageWriterBMP

// static
void
PixelFormatConverter::convert24BitTo24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr )
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
PixelFormatConverter::convert_R8G8B8A8toB8G8R8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R8G8B8toB8G8R8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_B8G8R8toR8G8B8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8toB8G8R8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R8G8B8A8toB8G8R8A8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_B8G8R8A8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8A8toB8G8R8A8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toB8G8R8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R5G6B5toB8G8R8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_R8G8B8toB8G8R8A8( void const * src, void* dst, size_t n )
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
PixelFormatConverter::convert_B8G8R8toR8G8B8A8( void const * src, void* dst, size_t n )
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

} // end namespace de.
