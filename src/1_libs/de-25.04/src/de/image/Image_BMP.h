// Copyright (C) 2019, Benjamin Hampe
#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_BMP_ENABLED)

// ============================================================================
class ImageReaderBMP : public IImageReader
// ============================================================================
{
public:
    std::vector< std::string > getSupportedReadExtensions() const override { return { "bmp" }; }

    bool isSupportedReadExtension( std::string const & ext ) const override { return ext == "bmp"; }

    bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_BMP_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_WRITER_BMP_ENABLED)

// ============================================================================
class ImageWriterBMP : public IImageWriter
// ============================================================================
{
public:
    std::vector< std::string >
    getSupportedWriteExtensions() const override { return std::vector< std::string >{ "bmp" }; }

    bool
    isSupportedWriteExtension( std::string const & ext ) const override { return ext == "bmp"; }

    bool
    save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_BMP_ENABLED

} // end namespace image.
} // end namespace de.
