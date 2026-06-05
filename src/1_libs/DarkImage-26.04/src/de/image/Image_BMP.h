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
    const std::vector< std::string >&
    getSupportedReadExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "bmp" };
        return s_exts;
    }

    bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
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
    const std::vector< std::string >&
    getSupportedWriteExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "bmp" };
        return s_exts;
    }

    bool
    save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_BMP_ENABLED

} // end namespace image.
} // end namespace de.
