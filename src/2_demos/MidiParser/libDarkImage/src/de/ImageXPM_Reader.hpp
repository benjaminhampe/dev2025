// Copyright (C) 2002-2012, Benjamin Hampe

#ifndef DE_IMAGELOADER_XPM_HPP
#define DE_IMAGELOADER_XPM_HPP

#include <de/ImageReader.hpp>

#ifdef DE_IMAGE_READER_XPM_ENABLED

#include "ImageXPM.hpp"

namespace de {
namespace image {

class ImageReaderXPM : public IImageReader
{
   DE_CREATE_LOGGER("de.ImageReaderXPM")
public:
   std::vector< std::string >
   getSupportedReadExtensions() const override
   {
      DE_DEBUG("")
      return std::vector< std::string >{ "xpm" };
   }

   bool
   isSupportedReadExtension( std::string const & ext ) const override
   {
      DE_DEBUG("")
      if ( ext == "xpm" ) return true;
      return false;
   }

   bool
   load( Image & img, std::string const & fileName ) override;

   static bool
   loadImpl( Image & img, std::vector< std::string > const & xpm_data );
};

} // end namespace image.
} // end namespace de.



#endif // DE_IMAGE_READER_XPM_ENABLED

#endif

