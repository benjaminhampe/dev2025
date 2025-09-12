#ifndef DE_IMAGE_READER_GIF_HPP
#define DE_IMAGE_READER_GIF_HPP

#include "ImageGIF.hpp"

#ifdef DE_IMAGE_READER_GIF_ENABLED

namespace de {
namespace image {

// ===========================================================================
// ImageReaderGIF
// ===========================================================================
class ImageReaderGIF : public IImageReader
{
public:
   DE_CREATE_LOGGER("de.ImageReaderGIF")

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "gif" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "gif" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_GIF_ENABLED

#endif // DE_IMAGE_READER_GIF_HPP
