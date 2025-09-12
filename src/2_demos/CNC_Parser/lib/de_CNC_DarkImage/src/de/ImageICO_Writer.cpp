#include "ImageICO_Writer.hpp"

#ifdef DE_IMAGE_WRITER_ICO_ENABLED

#include <png/png.h>

namespace de {
namespace image {
namespace ico {

} // end namespace ico

bool
ImageWriterICO::save( Image const & img, std::string const & uri, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR( "Empty image(", img.toString(), ")" )
      return false;
   }

   FILE * file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR( "Failed openToWriteBinary(", uri, ")." )
      return false;
   }

/*
   uint32_t w = img.getWidth();
   uint32_t h = img.getHeight();
   uint32_t scanlineBytes = img.getStride();

   size_t byteCount = size_t( scanlineBytes ) * h;
   std::vector< uint8_t > tmp;
   tmp.resize( byteCount, 0 );

   // DE_DEBUG( "Reserved scanline bytes(", byteCount, ")." )

   if ( img.getFormat() == ColorFormat::RGBA8888 )
   {
      //transformType = PNG_TRANSFORM_BGR;
      //DE_DEBUG( "ColorFormat is ARGB8888 for uri(", uri, ")." )
      ColorConverter::convert_ARGB8888toARGB8888( pixels, tmp.data(), pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::ARGB1555 )
   {
      //transformType = PNG_TRANSFORM_BGR;
      //DE_DEBUG( "ColorFormat is ARGB1555 for uri(", uri, ")." )
      ColorConverter::convert_ARGB1555toARGB8888( pixels, tmp.data(), pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::RGB888 )
   {
      colorType = PNG_COLOR_TYPE_RGB;
      //DE_DEBUG( "ColorFormat is RGB888 for uri(", uri, ")." )
      //ColorConverter::convert_RGB888toRGB888( pixels, w * h, tmp.data() );
      ColorConverter::convert_RGB888toARGB8888( pixels, tmp.data(), pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::RGB565 )
   {
      colorType = PNG_COLOR_TYPE_RGB;
      //DE_DEBUG( "ColorFormat is RGB565 for uri(", uri, ")." )
      ColorConverter::convert_RGB565toRGB888( pixels, tmp.data(), pixelCount );
   }

   // Fill array of pointers to rows in image data
   std::vector< uint8_t* > rows;
   rows.resize( h, nullptr );

   for ( size_t y = 0; y < h; ++y )
   {
      rows[ y ] = pixels;
      pixels += scanlineBytes;
   }


   ::fclose( file );
*/
   return true;
}

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_WRITER_ICO_ENABLED








