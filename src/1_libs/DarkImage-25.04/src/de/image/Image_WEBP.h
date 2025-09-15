#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_WEBP_ENABLED

// ===========================================================================
class ImageReaderWEBP : public IImageReader
// ===========================================================================
{
public:
   std::vector< std::string > getSupportedReadExtensions() const override { return { "webp" }; }

   bool isSupportedReadExtension( std::string const & ext ) const override { return ext == "webp"; }

   bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_WEBP_ENABLED

#ifdef DE_IMAGE_WRITER_WEBP_ENABLED

// ===========================================================================
class ImageWriterWEBP : public IImageWriter
// ===========================================================================
{
public:
   std::vector< std::string >
   getSupportedWriteExtensions() const override { return std::vector< std::string >{ "webp" }; }

   bool
   isSupportedWriteExtension( std::string const & ext ) const override { return ext == "webp"; }

   bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_WEBP_ENABLED

} // end namespace image.
} // end namespace de.
