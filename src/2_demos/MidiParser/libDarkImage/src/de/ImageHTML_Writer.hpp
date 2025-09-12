// Copyright (C) 2019, Benjamin Hampe
#ifndef DE_IMAGEWRITER_HTMLTABLE_HPP
#define DE_IMAGEWRITER_HTMLTABLE_HPP

#include <de/ImageWriter.hpp>

#ifdef DE_IMAGE_WRITER_HTML_ENABLED

namespace de {
namespace image {

class ImageWriterHtmlTable : public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageWriterHTML")
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override
   {
      return std::vector< std::string >{
         "htm",
         "html" };
   }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override
   {
      if ( ext == "htm" ) return true;
      if ( ext == "html" ) return true;
      if ( ext == "html5" ) return true;
      //if ( ext == "hta" ) return true;
      return false;
   }

   bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_HTML_ENABLED

#endif // DARKIMAGE_IMAGEWRITER_HTMLTABLE_FOR_REAL_HPP
