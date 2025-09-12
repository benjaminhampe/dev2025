// Copyright (C) 2002-2012 Benjamin Hampe
#pragma once
#include <de_image_formats/de_ImageCodecConfig.h>

#ifdef DE_IMAGE_WRITER_XPM_ENABLED

#include <de_image_formats/de_ImageWriter.h>

namespace de {
namespace image {

class ImageWriterXPM : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterXPM")
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return { "xpm", "h", "hpp" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "xpm" ) return true;
      if ( ext == "h" ) return true;
      if ( ext == "hpp" ) return true;
      //if ( ext == "txt" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;

//   struct XPM_Color
//   {
//      uint32_t color;
//      std::string key;

//      XPM_Color( uint32_t color_ = 0, std::string const & charCombi_ = "" )
//         : color( color_ ), key( charCombi_ )
//      {}
//   };

};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_XPM_ENABLED
