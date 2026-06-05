#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_PNG_ENABLED

class ImageReaderPNG : public IImageReader
{
public:
    const std::vector< std::string >&
    getSupportedReadExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "png" };
        return s_exts;
    }

    bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_PNG_ENABLED

#ifdef DE_IMAGE_WRITER_PNG_ENABLED

class ImageWriterPNG : public IImageWriter
{
public:
    const std::vector< std::string >&
    getSupportedWriteExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "png" };
        return s_exts;
    }

    bool
    save( const Image& img, const std::string& fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_PNG_ENABLED

} // end namespace image
} // end namespace de.


