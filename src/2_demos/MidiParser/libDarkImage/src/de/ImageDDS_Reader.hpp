#ifndef DE_IMAGE_READER_DDS_HPP
#define DE_IMAGE_READER_DDS_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_DDS_ENABLED

namespace de {
namespace image {

// ===========================================================================
// ImageReaderDDS
// ===========================================================================
class ImageReaderDDS : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderDDS")

public:
   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "dds" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "dds" ) return true;
      return false;
   }

   bool
   load( Image & img, Binary & file ) override;

   bool
   load( Image & img, std::string const & uri ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_DDS_ENABLED

#endif // DE_IMAGE_READER_DDS_HPP

