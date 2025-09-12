// Copyright (C) 2019, Benjamin Hampe
#ifndef DARKIMAGE_IMAGEWRITER_BMP_HPP
#define DARKIMAGE_IMAGEWRITER_BMP_HPP

#include "ImageBMP.hpp"
#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_BMP_ENABLED

namespace de {
namespace image {

class ImageWriterBMP : public IImageWriter
{
public:
   // DE_CREATE_LOGGER("de.image.bmp.Writer")
   DE_CREATE_LOGGER("de.ImageWriterBMP")

   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{
         "bmp" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "bmp" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_BMP_ENABLED

#endif // DARKIMAGE_IMAGEWRITER_BMP_HPP
