#pragma once
#include <de_image/de_Image.h>

namespace de {

// ===========================================================================
struct IImageWriter
// ===========================================================================
{
   virtual ~IImageWriter() {}

   virtual bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) = 0;

   virtual std::vector< std::string >
   getSupportedWriteExtensions() const = 0;

   virtual bool
   isSupportedWriteExtension( std::string const & suffix ) const = 0;
};

} // end namespace de.
