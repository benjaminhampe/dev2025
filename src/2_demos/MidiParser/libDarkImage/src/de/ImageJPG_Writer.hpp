// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGEWRITER_JPG_HPP
#define DE_IMAGEWRITER_JPG_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_JPG_ENABLED

namespace de {
namespace image {

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

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_JPG_ENABLED

#endif // DARKIMAGE_IMAGEWRITER_JPG_HPP
