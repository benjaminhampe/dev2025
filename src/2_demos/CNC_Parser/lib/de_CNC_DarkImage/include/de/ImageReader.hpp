#pragma once
#include <de/Image.hpp>
#include <de/Binary.hpp>

namespace de {

// ===========================================================================
// ===   ImageLoadOptions
// ===========================================================================
struct ImageLoadOptions
{
   uint32_t m_SearchColor;
   uint32_t m_ReplaceColor;

   bool m_ConvertToGrey121;
   bool m_ConvertToGrey111;
   bool m_AutoSaturate;

   float m_Brighten;
   float m_Gamma;
   float m_Contrast;

   std::string m_DefaultExportExt;

   ImageLoadOptions()
      : m_SearchColor( 0 )
      , m_ReplaceColor( 0 )
      , m_ConvertToGrey121( false )
      , m_ConvertToGrey111( false )
      , m_AutoSaturate( false )
      , m_Brighten( 1.0f )
      , m_Gamma( 1.0f )
      , m_Contrast( 1.0f )
      , m_DefaultExportExt( "raw" )
   {}
};

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


// *.ico -> AnimationReader
#ifndef DE_IMAGE_READER_ICO_ENABLED
#define DE_IMAGE_READER_ICO_ENABLED
#endif
// *.bmp
#ifndef DE_IMAGE_READER_BMP_ENABLED
#define DE_IMAGE_READER_BMP_ENABLED
#endif
// *.jpg
#ifndef DE_IMAGE_READER_JPG_ENABLED
#define DE_IMAGE_READER_JPG_ENABLED
#endif
// *.png
#ifndef DE_IMAGE_READER_PNG_ENABLED
#define DE_IMAGE_READER_PNG_ENABLED
#endif
// *.gif
#ifndef DE_IMAGE_READER_GIF_ENABLED
#define DE_IMAGE_READER_GIF_ENABLED
#endif
// *.tga
#ifndef DE_IMAGE_READER_TGA_ENABLED
#define DE_IMAGE_READER_TGA_ENABLED
#endif
// *.tif
#ifndef DE_IMAGE_READER_TIF_ENABLED
#define DE_IMAGE_READER_TIF_ENABLED
#endif
// *.dds
#ifndef DE_IMAGE_READER_DDS_ENABLED
#define DE_IMAGE_READER_DDS_ENABLED
#endif
// *.pcx
#ifndef DE_IMAGE_READER_PCX_ENABLED
#define DE_IMAGE_READER_PCX_ENABLED
#endif
// *.ppm
#ifndef DE_IMAGE_READER_PPM_ENABLED
#define DE_IMAGE_READER_PPM_ENABLED
#endif
// *.xpm
#ifndef DE_IMAGE_READER_XPM_ENABLED
#define DE_IMAGE_READER_XPM_ENABLED
#endif
// *.exr
// #ifndef DE_IMAGE_READER_EXR_ENABLED
// #define DE_IMAGE_READER_EXR_ENABLED
// #endif