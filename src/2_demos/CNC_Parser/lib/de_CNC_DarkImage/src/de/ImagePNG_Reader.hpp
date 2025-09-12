#ifndef DE_IMAGELOADER_PNG_HPP
#define DE_IMAGELOADER_PNG_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_PNG_ENABLED

namespace de {
namespace image {

class ImageReaderPNG : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderPNG")
public:
   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "png" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "png" ) return true;
      return false;
   }

   bool
   load( Image & img, Binary & file ) override;

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_READER_PNG_ENABLED

#endif // DARKIMAGE_IMAGELOADER_PNG_HPP

