#ifndef DE_IMAGE_WRITER_PCX_HPP
#define DE_IMAGE_WRITER_PCX_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_PCX_ENABLED

namespace de {
namespace image {

class ImageWriterPCX : public IImageWriter
{
public:
   // DE_CREATE_LOGGER("de.image.bmp.Writer")
   DE_CREATE_LOGGER("de.ImageWriterPCX")

   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "pcx" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "pcx" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_PCX_ENABLED

#endif // DE_IMAGE_WRITER_PCX_HPP
