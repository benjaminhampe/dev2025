// Copyright (C) 2019, Benjamin Hampe
#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_ICO_ENABLED)

// ============================================================================
class ImageReaderICO : public IImageReader
// ============================================================================
{
public:
    std::vector< std::string > getSupportedReadExtensions() const override { return { "ico", "cur" }; }
    bool isSupportedReadExtension( std::string const & ext ) const override { return (ext == "ico") || (ext == "cur"); }
    bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif

#if defined(DE_IMAGE_WRITER_ICO_ENABLED)

// ============================================================================
class ImageWriterICO : public IImageWriter
// ============================================================================
{
public:
    std::vector< std::string >
    getSupportedWriteExtensions() const override { return std::vector< std::string >{ "ico", "cur" }; }

    bool
    isSupportedWriteExtension( std::string const & ext ) const override
    {
        return (ext == "ico") || (ext == "cur");
    }

    bool
    save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

#endif

} // end namespace image
} // end namespace de.
