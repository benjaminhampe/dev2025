#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_PPM_ENABLED

// ===========================================================================
class ImageReaderPPM : public IImageReader
// ===========================================================================
{
public:
    std::vector< std::string >
    getSupportedReadExtensions() const override
    {
        return { "ppm", "pam" };
    }

    bool
    isSupportedReadExtension( const std::string& ext ) const override
    {
        return (ext == "ppm") || (ext == "pam");
    }

    bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_PPM_ENABLED

#ifdef DE_IMAGE_WRITER_PPM_ENABLED

// ===========================================================================
class ImageWriterPPM : public IImageWriter
// ===========================================================================
{
public:
    std::vector< std::string >
    getSupportedWriteExtensions() const override
    {
        return { "ppm", "pam" };
    }

    bool
    isSupportedWriteExtension( const std::string& ext ) const override
    {
        return (ext == "ppm") || (ext == "pam");
    }

    bool
    save( const Image& img, const std::string& fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_PPM_ENABLED

} // end namespace image.
} // end namespace de.
