#include "ImagePPM_Reader.hpp"

#ifdef DE_IMAGE_READER_PPM_ENABLED

namespace de {
namespace image {

bool
ImageReaderPPM::load( Image & img, std::string const & uri )
{
   // bmp::BMP bmp;
   // if ( !bmp.load( fileName ) )
   // {
      // DE_ERROR("Cant read binary file", fileName)
      // return nullptr;
   // }

   // DE_DEBUG("Opened BMP ",fileName,", w:",bmp.width,", h:",bmp.height,", bpp:",bmp.bpp )

   // ColorConverter::Converter_t converter = nullptr;

   return false;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_PPM_ENABLED
