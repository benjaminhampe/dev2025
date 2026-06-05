#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_JPG_ENABLED

class ImageReaderJPG : public IImageReader
{
public:
    const std::vector< std::string >&
    getSupportedReadExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "jpg", "jpeg" };
        return s_exts;
    }

    bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_JPG_ENABLED

#ifdef DE_IMAGE_WRITER_JPG_ENABLED

class ImageWriterJPG : public IImageWriter
{
public:
    const std::vector< std::string >&
    getSupportedWriteExtensions() const override
    {
        static const std::vector< std::string > s_exts{ "jpg", "jpeg" };
        return s_exts;
    }

    bool
    save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_JPG_ENABLED

} // end namespace image.
} // end namespace de.

