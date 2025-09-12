#pragma once
#include <de/ImageTGA.hpp>

#ifdef DE_IMAGE_READER_TGA_ENABLED

namespace de {
namespace image {

class ImageReaderTGA : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderTGA")
public:

   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      return std::vector< std::string >{ "tga" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      if ( ext == "tga" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & uri ) override;


   static size_t
   parseColorTable( std::vector< uint32_t > & palette, tga::TGAHeader const & header, de::Binary & file );
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_TGA_ENABLED
