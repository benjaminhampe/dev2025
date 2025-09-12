// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGE_WRITER_GIF_HPP
#define DE_IMAGE_WRITER_GIF_HPP

#include "ImageGIF.hpp"

#ifdef DE_IMAGE_WRITER_GIF_ENABLED

namespace de {
namespace image {

class ImageWriterGIF : public IImageWriter
{
public:
   DE_CREATE_LOGGER("de.ImageWriterGIF")

   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "gif" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "gif" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_GIF_ENABLED

#endif // DE_IMAGE_WRITER_GIF_HPP
