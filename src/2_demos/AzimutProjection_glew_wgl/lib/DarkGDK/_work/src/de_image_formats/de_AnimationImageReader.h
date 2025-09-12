#pragma once
#include <de_image/de_Image.h>
#include <de_image_formats/de_Binary.h>

namespace de {

// ===========================================================================
struct IAnimationReader
// ===========================================================================
{
   virtual ~IAnimationReader() = default;

   virtual std::vector< std::string >
   getSupportedReadExtensions() const = 0;

   virtual bool
   isSupportedReadExtension( std::string const & suffix ) const = 0;

   virtual bool
   load( Image & img, Binary & file, uint32_t frame = 0 ) { return false; }

   virtual bool
   load( Image & img, std::string const & uri, uint32_t frame = 0 ) = 0;

   virtual uint32_t
   getFrameCount( std::string const & uri ) { return 0; }

   virtual uint32_t
   getFrameDelayInMs( std::string const & uri, uint32_t frame = 0 ) { return 0; }
};

} // end namespace de.

