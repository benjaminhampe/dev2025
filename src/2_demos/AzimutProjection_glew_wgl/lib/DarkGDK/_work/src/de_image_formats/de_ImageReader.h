#pragma once
#include <de_image_formats/de_ImageLoadOptions.h>
#include <de_image_formats/de_Binary.h>
#include <de_image/de_Image.h>

namespace de {

// ===========================================================================
struct IImageReader
// ===========================================================================
{
   virtual ~IImageReader() {}

   virtual std::vector< std::string >
   getSupportedReadExtensions() const = 0;

   virtual bool
   isSupportedReadExtension( std::string const & suffix ) const = 0;

   virtual bool
   load( Image & img, Binary & file ) { return false; }

   virtual bool
   load( Image & img, std::string const & uri ) = 0;
};

} // end namespace de.
