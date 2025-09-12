#pragma once
#include <de_image_formats/de_ImageReader.h>
#include <de_image_formats/de_ImageWriter.h>

namespace de {

// ===========================================================================
struct ImageCodecManager
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImageCodecManager")
   std::vector< IImageReader* > m_Reader;
   std::vector< IImageWriter* > m_Writer;
   //std::vector< Image* > m_Images;
public:
   ImageCodecManager();
   ~ImageCodecManager();

   static std::shared_ptr< ImageCodecManager >
   getInstance();

   // ================
   //   Load images
   // ================
   bool
   loadImage( Image & img, std::string uri, ImageLoadOptions const & options = ImageLoadOptions() ) const;

   bool
   loadImageFromMemory( Image & img, uint8_t const* data, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const;

   bool
   isSupportedReadExtension( std::string const & uri ) const;

   std::vector< std::string >
   getSupportedReadExtensionVector() const;

   std::string
   getSupportedReadExtensions() const;

   // ================
   //   Save images
   // ================

   bool
   saveImage( Image const & img, std::string uri, uint32_t quality = 0 ) const;

   bool
   isSupportedWriteExtension( std::string const & uri ) const;

   std::string
   getSupportedWriteExtensions() const;

   std::vector< std::string >
   getSupportedWriteExtensionVector() const;

   // ================
   //   Convert image files
   // ================
   bool
   convertFile( std::string loadName, std::string saveName ) const;
};

} // end namespace de.
