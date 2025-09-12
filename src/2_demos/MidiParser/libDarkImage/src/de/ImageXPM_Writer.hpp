// Copyright (C) 2002-2012, Benjamin Hampe
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#pragma once
#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_XPM_ENABLED

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
