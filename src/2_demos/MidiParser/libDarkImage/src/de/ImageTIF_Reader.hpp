#ifndef DARKIMAGE_IMAGELOADER_TIF_HPP
#define DARKIMAGE_IMAGELOADER_TIF_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_TIF_ENABLED

namespace de {
namespace image {

class ImageReaderTIF : public IImageReader
{
public:
   DE_CREATE_LOGGER("de.ImageReaderTIF")

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "tif", "tiff" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "tif" ) return true;
      if ( ext == "tiff" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_TIF_ENABLED

#endif // DARKIMAGE_IMAGELOADER_TIF_HPP

