// Copyright (C) 2019, Benjamin Hampe
#pragma once
#include <de_image_formats/de_ImageCodecConfig.h>

#ifdef DE_IMAGE_WRITER_PNG_ENABLED

#include <de_image_formats/de_ImageWriter.h>

namespace de {
namespace image {

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

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_WRITER_PNG_ENABLED
