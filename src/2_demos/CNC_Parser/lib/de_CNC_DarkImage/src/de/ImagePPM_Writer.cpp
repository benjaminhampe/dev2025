#include "ImagePPM_Writer.hpp"

#ifdef DE_IMAGE_WRITER_PPM_ENABLED

#include <fstream>
#include <iomanip>
#include "ImagePPM.hpp"

namespace de {
namespace image {

bool
ImageWriterPPM::save( Image const & img, std::string const & uri, uint32_t quality )
{
   static_cast< void >( quality ); // UNUSED

   if ( img.empty() )
   {
      DE_ERROR("Image is empty.")
      return false;
   }

   std::ofstream file( uri.c_str() );

   if ( !file.is_open() )
   {
      DE_ERROR("Cant open uri(",uri,") for text write\n")
      return false;
   }

   int32_t w = img.getWidth();
   int32_t h = img.getHeight();

   file << "P3\n";
   file << "# The P3 means colors are in ASCII, then width and height\n";
   file << "# then w255 for max color, then RGB triplets\n";
   file << w << " " << h << "\n";
   file << "255\n";
   file << "P3\n";

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x, y );
         int32_t r = static_cast< int32_t >( RGBA_G( color ) );
         int32_t g = static_cast< int32_t >( RGBA_B( color ) );
         int32_t b = static_cast< int32_t >( RGBA_R( color ) );
         //int32_t a = static_cast< int32_t >( RGBA_A( color ) );

         //r = 0;
#ifdef USE_PADDING
         if ( r < 100 ) file << " ";
         if ( r < 10 ) file << " ";
#endif
         file << r << " ";

         //g = 0;
#ifdef USE_PADDING
         if ( g < 100 ) file << " ";
         if ( g < 10 ) file << " ";
#endif
         file << g << " ";

         //b = 0;
#ifdef USE_PADDING
         if ( b < 100 ) file << " ";
         if ( b < 10 ) file << " ";
#endif
         file << b << " ";
      }
      file << "\n";
   }
   file.close();
   return true;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_PPM_ENABLED
