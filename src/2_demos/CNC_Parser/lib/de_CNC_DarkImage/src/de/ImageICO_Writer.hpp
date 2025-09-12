// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGEWRITER_ICO_HPP
#define DE_IMAGEWRITER_ICO_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_ICO_ENABLED

namespace de {
namespace image {

class ImageWriterICO : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterICO")

public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "ico" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "ico" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_WRITER_ICO_ENABLED

#endif // DE_IMAGEWRITER_ICO_HPP
