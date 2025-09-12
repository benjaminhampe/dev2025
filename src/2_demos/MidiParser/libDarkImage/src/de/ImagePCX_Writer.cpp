#include "ImagePCX_Writer.hpp"

#ifdef DE_IMAGE_WRITER_PCX_ENABLED

namespace de {
namespace image {

bool
ImageWriterPCX::save( Image const & img, std::string const & uri, uint32_t quality )
{
   static_cast< void >( quality ); // UNUSED

   if ( img.empty() )
   {
      DE_ERROR("Image is empty.")
      return false;
   }

//   std::ofstream file( uri.c_str() );

//   if ( !file.is_open() )
//   {
//      DE_ERROR("Cant open uri(",uri,") for text write\n")
//      return false;
//   }

//   int32_t w = img.getWidth();
//   int32_t h = img.getHeight();



//   file.close();
   return true;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_PCX_ENABLED
