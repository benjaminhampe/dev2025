#pragma once
#include <de/image/ImageConfig.h>
// #include <de_image_formats/de_ImageReader.h>
// #include <de_image_formats/de_ImageWriter.h>
// #include <de_core/de_StringUtil.h>
// #include <unordered_map>

#ifdef DE_IMAGE_READER_EXR_ENABLED
#include <half.h>
//#include <OpenEXR/half.h>
#endif

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_EXR_ENABLED

// =================================== Reader ===================================
class ImageReaderEXR : public IImageReader
// =================================== Reader ===================================
{
public:
    std::vector< std::string > getSupportedReadExtensions() const override { return { "exr" }; }

    bool isSupportedReadExtension( std::string const & ext ) const override { return ext == "exr"; }

    bool load( Image & img, const std::string & uri ) override; // Legacy

    bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override; // The Future
/*
    ImageReaderEXR()
    {
        DE_TRACE("Half Min Value: ", std::numeric_limits<half>::min())
        DE_TRACE("Half Max Value: ", std::numeric_limits<half>::max())
        DE_TRACE("Half Epsilon Value: ", std::numeric_limits<half>::epsilon())
    }
*/
};

#endif // DE_IMAGE_READER_EXR_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#ifdef DE_IMAGE_WRITER_EXR_ENABLED

// =================================== Writer ===================================
class ImageWriterEXR : public IImageWriter
// =================================== Writer ===================================
{
public:
    std::vector< std::string > getSupportedWriteExtensions() const override { return { "exr" }; }

    bool isSupportedWriteExtension( std::string const & ext ) const override { return ext == "exr"; }

    bool save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_EXR_ENABLED


} // end namespace image.
} // end namespace de.

