#ifndef DE_IMAGE_BMP_READER_HPP
#define DE_IMAGE_BMP_READER_HPP

#include "ImageBMP.hpp"

#ifdef DE_IMAGE_READER_BMP_ENABLED

namespace de {
namespace image {

class ImageReaderBMP : public IImageReader
{
public:
   DE_CREATE_LOGGER("de.ImageReaderBMP")

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "bmp" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "bmp" ) return true;
      return false;
   }

   bool
   load( Image & img, Binary & file ) override;

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_BMP_ENABLED

#endif // DE_IMAGE_BMP_READER_HPP
