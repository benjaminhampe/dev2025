// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGE_WRITER_PPM_HPP
#define DE_IMAGE_WRITER_PPM_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_PPM_ENABLED

namespace de {
namespace image {

class ImageWriterPPM : public IImageWriter
{
public:
   // DE_CREATE_LOGGER("de.image.bmp.Writer")
   DE_CREATE_LOGGER("de.ImageWriterPPM")

   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "ppm" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "ppm" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_PPM_ENABLED

#endif // DE_IMAGE_WRITER_PPM_HPP
