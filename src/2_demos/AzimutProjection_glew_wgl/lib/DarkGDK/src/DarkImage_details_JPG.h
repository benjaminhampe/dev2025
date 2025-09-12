#pragma once
#include "DarkImage_details.h"

namespace DarkGDK {
namespace image {

#ifdef DE_IMAGE_READER_JPG_ENABLED

class ImageReaderJPG : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderJPG")
public:

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return { "jpg", "jpeg" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "jpg" ) return true;
      if ( ext == "jpeg" ) return true;
      return false;
   }

   bool
   load( Image & img, Binary & file ) override;

   bool
   load( Image & img, std::string const & fileName ) override;
};

#endif // DE_IMAGE_READER_JPG_ENABLED

#ifdef DE_IMAGE_WRITER_JPG_ENABLED

class ImageWriterJPG : public IImageWriter
{
   DE_CREATE_LOGGER("ImageWriterJPG")
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{
         "jpg", "jpeg" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "jpg" ) return true;
      if ( ext == "jpeg" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_JPG_ENABLED


} // end namespace image.
} // end namespace DarkGDK.

