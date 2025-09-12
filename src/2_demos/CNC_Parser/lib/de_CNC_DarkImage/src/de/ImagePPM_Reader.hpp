#ifndef DE_IMAGE_READER_PPM_HPP
#define DE_IMAGE_READER_PPM_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_PPM_ENABLED

#include "ImagePPM.hpp"

namespace de {
namespace image {

class ImageReaderPPM : public IImageReader
{
public:
   DE_CREATE_LOGGER("de.ImageReaderPPM")

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "ppm" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "ppm" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_PPM_ENABLED

#endif // DE_IMAGE_READER_PPM_HPP
