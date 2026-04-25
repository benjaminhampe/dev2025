#include <de/image/Image_WEBP.h>

#if defined(DE_IMAGE_READER_WEBP_ENABLED) || defined(DE_IMAGE_WRITER_WEBP_ENABLED)
    #include <vector>
#endif

#if defined(DE_IMAGE_READER_WEBP_ENABLED)
    #include <webp/decode.h>
#endif

#if defined(DE_IMAGE_WRITER_WEBP_ENABLED)
    #include <webp/encode.h>
#endif

namespace de {
namespace image {

// ===========================================================================

#ifdef DE_IMAGE_READER_WEBP_ENABLED


bool
ImageReaderWEBP::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    // Binary & file
    // const uint8_t* pData = file.data();
    // const size_t szSize = file.size();

    WebPBitstreamFeatures features;
    VP8StatusCode status = WebPGetFeatures(p, n, &features);
    if (status != VP8_STATUS_OK)
    {
        DE_ERROR("[webp] Failed to retrieve image features. ", uri)
        return false;
    }
    // DE_WARN("[webp] Image FileName: ", file.getUri())
    // DE_WARN("[webp] Image Width: ", features.width)
    // DE_WARN("[webp] Image Height: ", features.height)
    // DE_WARN("[webp] Has Alpha Channel: ", (features.has_alpha ? "Yes" : "No"))
    // DE_WARN("[webp] Is Animation: ", (features.has_animation ? "Yes" : "No"))

    PixelFormat::ePixelFormat dstFmt = PixelFormat::R8G8B8;
    if (features.has_alpha)
    {
        //DE_WARN("[webp] Likely Pixel Format: RGBA_8888")
        dstFmt = PixelFormat::R8G8B8A8;
    }
    else
    {
        //DE_WARN("[webp] Likely Pixel Format: RGB_888")
    }

    const int w = features.width;
    const int h = features.height;
    img.resize( w, h, dstFmt );

    // Decode into the allocated buffer
    uint8_t* ret = nullptr;
    if (features.has_alpha)
    {
        ret = WebPDecodeRGBAInto(p, n, img.data(), img.size(), img.stride());
    }
    else
    {
        ret = WebPDecodeRGBInto(p, n, img.data(), img.size(), img.stride());
    }

    if (!ret)
    {
        DE_ERROR("[webp] Decoding failed. ", uri)
        return false;
    }

    return true;
}

#endif // DE_IMAGE_READER_WEBP_ENABLED

// ===========================================================================

#ifdef DE_IMAGE_WRITER_WEBP_ENABLED

bool
ImageWriterWEBP::save( Image const & img, std::string const & uri, uint32_t /* quality */ )
{
    if ((img.pixelFormat() != PixelFormat::R8G8B8)
        && (img.pixelFormat() != PixelFormat::R8G8B8A8))
    {
        DE_ERROR("Unsupported image format for WebP encoder. ", img.str())
        return false;
    }

    uint8_t* pData = nullptr;
    size_t szSize = 0;

    if (img.pixelFormat() == PixelFormat::R8G8B8)
    {
        szSize = WebPEncodeLosslessRGB(img.data(), img.w(), img.h(), img.stride(), &pData);
    }
    else
    {
        szSize = WebPEncodeLosslessRGBA(img.data(), img.w(), img.h(), img.stride(), &pData);
    }

    if (!pData)
    {
        DE_ERROR("Failed to encode image to WebP. ", img.str())
        return false;
    }

    //DE_WARN("pData = ",pData)
    //DE_WARN("szSize = ",szSize)

    std::vector<uint8_t> byteVector(pData, pData + szSize);

    WebPFree(pData); // Free memory allocated by WebPEncodeRGBA

    return FileSystem::saveBin( uri, byteVector );
}

#endif // DE_IMAGE_WRITER_WEBP_ENABLED

// ===========================================================================

} // end namespace image.
} // end namespace de.
