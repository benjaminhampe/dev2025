#ifndef DE_IMAGELOADER_ICO_HPP
#define DE_IMAGELOADER_ICO_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_ICO_ENABLED

namespace de {
namespace image {

class ImageReaderICO : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderICO")
public:
   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "ico", "cur" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "ico" ) return true;
      if ( ext == "cur" ) return true;
      return false;
   }

   bool
   load( Image & img, Binary & file ) override;

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_READER_ICO_ENABLED

#endif // DARKIMAGE_IMAGELOADER_ICO_HPP

