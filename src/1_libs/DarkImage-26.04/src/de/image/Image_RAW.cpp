#include <de/image/Image_RAW.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_RAW_ENABLED)

bool
ImageReaderRAW::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    return false;
}

#endif // DE_IMAGE_READER_RAW_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_WRITER_RAW_ENABLED)

bool
ImageWriterRAW::save( Image const & img, std::string const & uri, uint32_t quality )
{
    if ((img.pixelFormat() != PixelFormat::R8G8B8) &&
        (img.pixelFormat() != PixelFormat::R8G8B8A8))
    {
        DE_ERROR("Only supports RGB24 and RGBA32 format, not ", img.pixelFormatStr())
        return false;
    }

    // --- Dump pixel data ---
    {
        File file(uri, eFileMode::Write);
        if (!file.is_open())
        {
            DE_ERROR("Cannot open ",uri)
            return false;
        }

        const uint8_t* __restrict__ pixels = img.data();
        file.write(pixels, img.size());
        file.close();
    }

    // --- Dump meta data for reconstruction ---
    {
        File file(uri + ".meta", eFileMode::Write);
        if (!file.is_open())
        {
            DE_ERROR("Cannot open ",uri,".meta")
            return false;
        }

        std::ostringstream o; o <<
        "image = " << uri << "\n"
        "width = " << img.w() << "\n"
        "height = " << img.h() << "\n"
        "channels = " << img.channelCount() << "\n"
        "maxval = 255\n"
        "# Created by libDarkImage-26.04 (c) 2026 by <benjaminhampe@gmx.de>\n";

        std::string s = o.str();
        const char* __restrict__ p = s.data();
        file.write(p, s.size());
        file.close();
    }

    return true;
}

#endif // DE_IMAGE_WRITER_RAW_ENABLED

} // end namespace image.
} // end namespace de.

