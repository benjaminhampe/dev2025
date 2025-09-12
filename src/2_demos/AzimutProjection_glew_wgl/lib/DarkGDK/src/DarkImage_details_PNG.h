#pragma once
#include "DarkImage_details.h"

namespace DarkGDK {
namespace image {

#ifdef DE_IMAGE_READER_PNG_ENABLED

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

#endif // DE_IMAGE_READER_PNG_ENABLED

#ifdef DE_IMAGE_WRITER_PNG_ENABLED

class ImageWriterPNG : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterPNG")

public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "png" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "png" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_PNG_ENABLED

} // end namespace image
} // end namespace DarkGDK.


