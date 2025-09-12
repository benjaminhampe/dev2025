// Copyright (C) 2019, Benjamin Hampe
#pragma once
#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_TGA_ENABLED

namespace de {
namespace image {

class ImageWriterTGA : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterTGA")
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{
         "tga" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "tga" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_TGA_ENABLED
