// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGE_WRITER_DDS_HPP
#define DE_IMAGE_WRITER_DDS_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_DDS_ENABLED

namespace de {
namespace image {

class ImageWriterDDS : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterDDS")

public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{ "dds" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "dds" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_DDS_ENABLED

#endif // DE_IMAGE_WRITER_DDS_HPP
