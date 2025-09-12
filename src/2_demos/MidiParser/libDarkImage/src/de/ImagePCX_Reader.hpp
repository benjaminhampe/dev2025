#ifndef DE_IMAGE_READER_PCX_HPP
#define DE_IMAGE_READER_PCX_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_PCX_ENABLED

namespace de {
namespace image {

class ImageReaderPCX : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderPCX")
public:

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return { "pcx" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "pcx" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & fileName ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_PCX_ENABLED

#endif // DE_IMAGE_READER_PCX_HPP
