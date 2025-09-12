#pragma once
#include <de/Image.hpp>
#include <de/Binary.hpp>

namespace de {

// ===========================================================================
struct IImageWriter
// ===========================================================================
{
   virtual ~IImageWriter() {}

   virtual std::vector< std::string >
   getSupportedWriteExtensions() const = 0;

   virtual bool
   isSupportedWriteExtension( std::string const & suffix ) const = 0;

   virtual bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) = 0;
};

} // end namespace de.

// *.html
#ifndef DE_IMAGE_WRITER_HTML_ENABLED
#define DE_IMAGE_WRITER_HTML_ENABLED
#endif
// *.ico
#ifndef DE_IMAGE_WRITER_ICO_ENABLED
#define DE_IMAGE_WRITER_ICO_ENABLED
#endif
// *.bmp
#ifndef DE_IMAGE_WRITER_BMP_ENABLED
#define DE_IMAGE_WRITER_BMP_ENABLED
#endif
// *.jpg
#ifndef DE_IMAGE_WRITER_JPG_ENABLED
#define DE_IMAGE_WRITER_JPG_ENABLED
#endif
// *.png
#ifndef DE_IMAGE_WRITER_PNG_ENABLED
#define DE_IMAGE_WRITER_PNG_ENABLED
#endif
// *.gif
#ifndef DE_IMAGE_WRITER_GIF_ENABLED
#define DE_IMAGE_WRITER_GIF_ENABLED
#endif
// *.tga
#ifndef DE_IMAGE_WRITER_TGA_ENABLED
#define DE_IMAGE_WRITER_TGA_ENABLED
#endif
// *.tif
#ifndef DE_IMAGE_WRITER_TIF_ENABLED
#define DE_IMAGE_WRITER_TIF_ENABLED
#endif
// *.dds
#ifndef DE_IMAGE_WRITER_DDS_ENABLED
#define DE_IMAGE_WRITER_DDS_ENABLED
#endif
// *.pcx
#ifndef DE_IMAGE_WRITER_PCX_ENABLED
#define DE_IMAGE_WRITER_PCX_ENABLED
#endif
// *.ppm
#ifndef DE_IMAGE_WRITER_PPM_ENABLED
#define DE_IMAGE_WRITER_PPM_ENABLED
#endif
// *.xpm
#ifndef DE_IMAGE_WRITER_XPM_ENABLED
#define DE_IMAGE_WRITER_XPM_ENABLED
#endif
// *.exr
// #ifndef DE_IMAGE_WRITER_EXR_ENABLED
// #define DE_IMAGE_WRITER_EXR_ENABLED
// #endif


