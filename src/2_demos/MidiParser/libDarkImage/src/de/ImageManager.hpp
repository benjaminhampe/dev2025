#ifndef DARKIMAGE_IMAGEMANAGER_HPP
#define DARKIMAGE_IMAGEMANAGER_HPP

#include <de/Image.hpp>
#include <de/ImageReader.hpp>
#include <de/ImageWriter.hpp>

namespace de {

// ===========================================================================
// ===   ImageManager
// ===========================================================================
struct ImageManager // : public IImageReader, public IImageWriter
{
   DE_CREATE_LOGGER("de.ImageManager")
   std::vector< IImageReader* > m_Reader;
   std::vector< IImageWriter* > m_Writer;
   //std::vector< Image* > m_Images;
public:
   ImageManager();
   ~ImageManager();

   static std::shared_ptr< ImageManager >
   getInstance();

   // ================
   //   Load images
   // ================
   bool
   loadImage( Image & img, std::string uri, ImageLoadOptions const & options = ImageLoadOptions() ) const;

   bool
   loadImageFromMemory( Image & img, uint8_t const* data, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const;

   // ================
   //   Save images
   // ================
   bool
   saveImage( Image const & img, std::string uri, uint32_t quality = 0 ) const;

   // ================
   //   Load images
   // ================
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

#endif // DARKIMAGE_IMAGE_HPP
