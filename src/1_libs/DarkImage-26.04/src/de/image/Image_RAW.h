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
    const std::vector< std::string >&
    getSupportedReadExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "raw" };
        return s_exts;
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
    const std::vector< std::string >&
    getSupportedWriteExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "raw" };
        return s_exts;
    }

    bool
    save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif

} // end namespace image.
} // end namespace de.
