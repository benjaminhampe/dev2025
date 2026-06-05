#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_RAW_ENABLED)

// ===========================================================================
class ImageReaderRAW : public IImageReader
// ===========================================================================
{
public:
    std::vector< std::string >
    getSupportedReadExtensions() const override
    {
        return { "raw" };
    }

    bool
    isSupportedReadExtension( std::string const & ext ) const override
    {
        return ext == "raw";
    }

    bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif

#if defined(DE_IMAGE_WRITER_RAW_ENABLED)

// ===========================================================================
class ImageWriterRAW : public IImageWriter
// ===========================================================================
{
public:
    std::vector< std::string >
    getSupportedWriteExtensions() const override
    {
        return { "raw" };
    }

    bool
    isSupportedWriteExtension( std::string const & ext ) const override
    {
        return ext == "raw";
    }

    bool
    save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif

} // end namespace image.
} // end namespace de.
