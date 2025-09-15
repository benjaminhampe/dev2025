#include <de/image/Image_EXR.h>

#if defined(DE_IMAGE_READER_EXR_ENABLED) || defined(DE_IMAGE_WRITER_EXR_ENABLED)
#include <vector>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfFloatAttribute.h>
#include <OpenEXR/ImfVecAttribute.h>
#include <OpenEXR/ImfStringAttribute.h>
#include <ImathBox.h> // OpenEXR/
#include <half.h> // OpenEXR/

#include <OpenEXR/ImfInputFile.h>
//#include <OpenEXR/ImfIStream.h>
#include <vector>
#include <cstring>

#include <iostream>
#include <string>

#endif

namespace de {
namespace image {

// =================================== Reader ===================================

#ifdef DE_IMAGE_READER_EXR_ENABLED

/*
unsigned int
_halfToFloat (unsigned short y)
{

    int s = (y >> 15) & 0x00000001;
    int e = (y >> 10) & 0x0000001f;
    int m = y & 0x000003ff;

    if (e == 0)
    {
        if (m == 0)
        {
            //
            // Plus or minus zero
            //

            return s << 31;
        }
        else
        {
            //
            // Denormalized number -- renormalize it
            //

            while (!(m & 0x00000400))
            {
                m <<= 1;
                e -= 1;
            }

            e += 1;
            m &= ~0x00000400;
        }
    }
    else if (e == 31)
    {
        if (m == 0)
        {
            //
            // Positive or negative infinity
            //

            return (s << 31) | 0x7f800000;
        }
        else
        {
            //
            // Nan -- preserve sign and significand bits
            //

            return (s << 31) | 0x7f800000 | (m << 13);
        }
    }

    //
    // Normalized number
    //

    e = e + (127 - 15);
    m = m << 13;

    //
    // Assemble s, e and m.
    //

    return (s << 31) | (e << 23) | m;
}
*/

float convertToFloat(const void* data, Imf::PixelType type)
{
    switch (type)
    {
    case Imf::HALF:
        //{ uint16_t v = *reinterpret_cast<uint16_t*>(data);
        //return _halfToFloat(v); }
        return static_cast<float>(*reinterpret_cast<const half*>(data));
    case Imf::FLOAT:
        return *reinterpret_cast<const float*>(data);
    case Imf::UINT:
        return static_cast<float>(*reinterpret_cast<const uint32_t*>(data)) / 4294967295.0f;
    default:
        throw std::runtime_error("Unsupported pixel type.");
    }
}

size_t getPixelTypeSize(Imf::PixelType type)
{
    switch (type)
    {
    case Imf::HALF:  return sizeof(half);
    case Imf::FLOAT: return sizeof(float);
    case Imf::UINT:  return sizeof(uint32_t);
    default: throw std::runtime_error("Unsupported pixel type.");
    }
}

/*
    struct Header {
        Box2i displayWindow;             // Defines the boundaries of the image in pixel space
        Box2i dataWindow;                // Specifies the region for which pixel data are available
        Box2i originalDataWindow;        // Stores the data window of the original, uncropped image
        float pixelAspectRatio;          // The width divided by the height of a pixel
        V2f screenWindowCenter;          // Describes the perspective projection that produced the image
        float screenWindowWidth;         // Used in conjunction with screenWindowCenter
        LineOrder lineOrder;             // Specifies the order in which scan lines are stored in the file
        Compression compression;         // Specifies the compression method applied to the pixel data
        std::string name;                // A name for the image or layer
        std::string type;                // The type of image (e.g., "scanlineimage", "tiledimage")
        Chromaticities chromaticities;   // The chromaticities of the RGB channels
        float whiteLuminance;            // The luminance of the white point
        std::vector<Channel> channels;   // A list of channels in the image
        std::vector<TileDescription> tiles; // Tile description for tiled images
        Preview preview;                 // Thumbnail preview image
    };

    struct TileDescription {
        int xSize;                    // Width of a tile in pixels
        int ySize;                    // Height of a tile in pixels
        LevelMode mode;               // Level mode for the image
        LevelRoundingMode roundingMode; // Level rounding mode for the image
    };

    channels: Specifies the color channels present in the file. Possible values: "R", "G", "B", "A", "Y", "U", "V", "Z", etc. These channels represent Red, Green, Blue, Alpha, Luminance, Chrominance, Depth, etc.
    compression: Compression method used for the image data. Possible values: "none", "rle", "zip", "zips", "piz", "pxr24", "b44", "b44a", "dwaa", "dwab". "none": No compression. "rle": Run-Length Encoding. "zip": Zip compression (per scanline). "zips": Zip compression (per scanline block). "piz": PIZ wavelet compression. "pxr24": Lossy 24-bit compression. "b44": B44 compression. "b44a": B44A compression. "dwaa": DWAA compression. "dwab": DWAB compression.
    dataWindow: The pixel rectangle that contains valid image data. Possible values: Defined by an Imath::Box2i structure with min and max coordinates.
    displayWindow: The pixel rectangle that defines the display area for the image. Possible values: Defined by an Imath::Box2i structure with min and max coordinates.
    pixelAspectRatio: The aspect ratio of the pixels in the image. Possible values: Any positive floating-point value representing width/height.
    screenWindowCenter: Center of the screen window in pixel coordinates. Possible values: Defined by an Imath::V2f structure with x and y coordinates.
    screenWindowWidth: Width of the screen window in pixel coordinates. Possible values: Any positive floating-point value.
    lineOrder: Order in which scan lines are stored in the file. Possible values: "increasing", "decreasing", "randomY". "increasing": Lines stored from top to bottom. "decreasing": Lines stored from bottom to top. "randomY": Lines stored in a random order.
    type: Type of image. Possible values: "scanlineimage", "tiledimage", "deepimage". "scanlineimage": Image is stored in scanlines. "tiledimage": Image is stored in tiles. "deepimage": Image is a deep image (contains multiple samples per pixel).
    name: A unique name or identifier for the image. Possible values: Any string value.
    xDensity: Horizontal density of the image (pixels per inch). Possible values: Any positive floating-point value.
    owner: Name of the person who owns or created the image. Possible values: Any string value.
    comments: Comments or descriptions about the image. Possible values: Any string value.
    capDate: Date and time when the image was created (capture date). Possible values: Any string value representing date and time.
    utcOffset: Offset in seconds from Coordinated Universal Time (UTC) for the capture date. Possible values: Any floating-point value representing seconds.
    longitude: Longitude where the image was created (in degrees). Possible values: Any floating-point value representing degrees.
    latitude: Latitude where the image was created (in degrees). Possible values: Any floating-point value representing degrees.
    altitude: Altitude where the image was created (in meters). Possible values: Any floating-point value representing meters.
    focus: Focus distance of the camera (in meters). Possible values: Any floating-point value representing meters.
    expTime: Exposure time of the camera (in seconds). Possible values: Any floating-point value representing seconds.
    aperture: Aperture size of the camera lens (f-stop). Possible values: Any positive floating-point value representing f-stop.
    isoSpeed: ISO speed of the camera. Possible values: Any positive integer value.
    envmap: Type of environment map. Possible values: "latitude-longitude", "cube", etc. "latitude-longitude": Latitude-longitude map. "cube": Cube map.
    keyCode: Key code information for film images. Possible values: Any structured value representing film key code information.
    timeCode: Time code information. Possible values: Any structured value representing time code information.
    wrapmodes: Wrap modes for texture images. Possible values: "clamp", "black", "periodic", "mirror". "clamp": Clamp wrap mode. "black": Black wrap mode. "periodic": Periodic wrap mode. "mirror": Mirror wrap mode.
    framesPerSecond: Frame rate for sequence images. Possible values: Any positive floating-point value representing frames per second.
    multiView: Multi-view information for stereo or multi-view images. Possible values: Any string or array value representing view names.
    worldToCamera: Transformation matrix from world space to camera space. Possible values: Any Imath::M44f value representing the transformation matrix.
    worldToNDC: Transformation matrix from world space to normalized device coordinates. Possible values: Any Imath::M44f value representing the transformation matrix.
*/

void printMetadata( const std::string & uri )
{
    DE_WARN( FileSystem::fileName(uri) )

    Imf::RgbaInputFile file( uri.c_str() );
    const Imf::Header& header = file.header();

    size_t nInfos = 0;
    for (auto it = header.begin(); it != header.end(); ++it)
    {
        nInfos++;
    }

    DE_OK("FileHeader contains ",nInfos," infos.")
    size_t nInfo = 0;
    for (auto it = header.begin(); it != header.end(); ++it)
    {
        DE_BENNI("FileHeader[",nInfo,"] ",it.name(),": ",it.attribute().typeName())
        nInfo++;
    }

    const Imath::Box2i dw = file.header().dataWindow();
    const Imath::Box2i dd = file.header().displayWindow();

    DE_TRACE("dataWindow(min:{",dw.min.x,",",dw.min.y,"}, max:{",dw.max.x,",",dw.max.y,"})")
    DE_TRACE("displayWindow(min:{",dd.min.x,",",dd.min.y,"}, max:{",dd.max.x,",",dd.max.y,"})")

    int data_w = dw.max.x - dw.min.x + 1;
    int data_h = dw.max.y - dw.min.y + 1;
    DE_TRACE("Resolution = ",data_w," x ",data_h)

    int channelCount = 0;
    for (auto it = header.channels().begin(); it != header.channels().end(); ++it)
    {
        channelCount++;
    }
    DE_TRACE("channelCount = ", channelCount)

    if (header.findTypedAttribute<Imf::V2fAttribute>("screenWindowCenter"))
    {
        Imath::V2f center = header.typedAttribute<Imf::V2fAttribute>("screenWindowCenter").value();
        DE_INFO("Found attribute screenWindowCenter(", center.x, ", ", center.y, ")")
    }
    else
    {
        DE_WARN("Did not find attribute screenWindowCenter")
    }

    if (header.findTypedAttribute<Imf::FloatAttribute>("screenWindowWidth"))
    {
        float width = header.typedAttribute<Imf::FloatAttribute>("screenWindowWidth").value();
        DE_INFO("Found attribute screenWindowWidth(", width, ")")
    }
    else
    {
        DE_WARN("Did not find attribute screenWindowWidth")
    }

    if (header.findTypedAttribute<Imf::FloatAttribute>("exposure"))
    {
        float exposure = header.typedAttribute<Imf::FloatAttribute>("exposure").value();
        DE_INFO("Found attribute exposure(", exposure, ")")
    }
    else
    {
        DE_WARN("Did not find attribute exposure")
    }

    const Imf::ChannelList& channelList = file.header().channels();

    Imf::PixelType pixelType = channelList.begin().channel().type;
    int bitsPerPixel = 0;
    int i = 0;


    size_t baseOffset = 0;
    size_t channelIndex = 0;

    for (auto it = channelList.begin(); it != channelList.end(); ++it)
    {
        Imf::PixelType type = it.channel().type;

        if (pixelType != type)
        {
            DE_ERROR("Mixed pixelType unsupported")
        }

        std::ostringstream k;
        k << "Channel["<< i<< "] "<< it.name()<< ": ";
        switch (type)
        {
            case Imf::HALF: { bitsPerPixel += 16; k << "HALF (16-bit float)"; break; }
            case Imf::FLOAT: { bitsPerPixel += 32; k << "FLOAT (32-bit float)"; break; }
            case Imf::UINT: { bitsPerPixel += 32; k << "UINT (32-bit integer)"; break; }
            default: { k << "UNKNOWN"; break; }
        }

        DE_TRACE(k.str())

        i++;
    }

    DE_TRACE("sizeof(Rgba) = ", sizeof(Imf::Rgba))
    DE_TRACE("bitsPerPixel = ", bitsPerPixel)

    std::vector<Imf::Rgba> src(data_w * data_h);
    file.setFrameBuffer(&src[0], 1, data_w);
    file.readPixels(dw.min.y, dw.max.y);

    float fRMin = std::numeric_limits<float>::max();
    float fRMax = std::numeric_limits<float>::lowest();
    float fGMin = std::numeric_limits<float>::max();
    float fGMax = std::numeric_limits<float>::lowest();
    float fBMin = std::numeric_limits<float>::max();
    float fBMax = std::numeric_limits<float>::lowest();
    float fAMin = std::numeric_limits<float>::max();
    float fAMax = std::numeric_limits<float>::lowest();

    if (channelCount > 0)
    {
        for (const auto & pixel : src)
        {
            fRMin = std::min(fRMin, static_cast<float>(pixel.r));
            fRMax = std::max(fRMax, static_cast<float>(pixel.r));
        }
    }

    if (channelCount > 1)
    {
        for (const auto & pixel : src)
        {
            fGMin = std::min(fGMin, static_cast<float>(pixel.g));
            fGMax = std::max(fGMax, static_cast<float>(pixel.g));
        }
    }

    if (channelCount > 2)
    {
        for (const auto & pixel : src)
        {
            fBMin = std::min(fBMin, static_cast<float>(pixel.b));
            fBMax = std::max(fBMax, static_cast<float>(pixel.b));
        }
    }

    if (channelCount > 3)
    {
        for (const auto & pixel : src)
        {
            fAMin = std::min(fAMin, static_cast<float>(pixel.a));
            fAMax = std::max(fAMax, static_cast<float>(pixel.a));
        }
    }

    DE_TRACE("fRMin = ", fRMin)
    DE_TRACE("fRMax = ", fRMax)
    DE_TRACE("fGMin = ", fGMin)
    DE_TRACE("fGMax = ", fGMax)
    DE_TRACE("fBMin = ", fBMin)
    DE_TRACE("fBMax = ", fBMax)
    DE_TRACE("fAMin = ", fAMin)
    DE_TRACE("fAMax = ", fAMax)
}

// ==================================================================
// ==================================================================
// ==================================================================

// Custom memory stream implementation
class MemoryIStream : public Imf::IStream
{
public:
    MemoryIStream(const char* data, size_t size)
        : Imf::IStream("MemoryStream")
        , _data(data)
        , _size(size)
        , _pos(0)
    {}

    bool read(char c[], int n) override
    {
        if (_pos + n > _size) return false;
        std::memcpy(c, _data + _pos, n);
        _pos += n;
        return true;
    }

    uint64_t tellg() override { return _pos; }

    void seekg(uint64_t pos) override { if (pos <= _size) _pos = pos; }

    bool eof() const { return _pos >= _size; }

private:
    const char* _data;
    size_t _size;
    size_t _pos;
};

uint32_t getChannelCount( const Imf::Header& header )
{
    const Imf::ChannelList& channelList = header.channels();

    int nChannels = 0;
    for (auto it = channelList.begin(); it != channelList.end(); ++it)
    {
        nChannels++;
    }

    return nChannels;
}

// New: loadMemory resource.
bool
ImageReaderEXR::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    MemoryIStream memStream(reinterpret_cast<const char*>(p), n);
    Imf::InputFile file(memStream);

    // // Create a memory input stream
    // Imf::MemStream memStream(p,n);
    // // Load the EXR file using memory stream
    // Imf::InputFile inputFile(memStream);

    // Imf::RgbaInputFile file(uri.c_str());

    //printMetadata(file);

    // const Imf::Header& header = file.header();
    // const Imath::Box2i dw = file.dataWindow();
    // const int w = dw.max.x - dw.min.x + 1;
    // const int h = dw.max.y - dw.min.y + 1;

    // Get image dimensions from header
    const Imf::Header& header = file.header();
    Imath::Box2i dw = header.dataWindow();
    int w  = dw.max.x - dw.min.x + 1;
    int h = dw.max.y - dw.min.y + 1;
    if (w < 1)
    {
        DE_ERROR("Invalid width")
        return false;
    }
    if (h < 1)
    {
        DE_ERROR("Invalid height")
        return false;
    }


    int nChannels = getChannelCount(header);
    if (nChannels < 1)
    {
        return false;
    }
    else if (nChannels == 1)
    {
        img.setPixelFormat(PixelFormat::R32F);
    }
    else if (nChannels == 3)
    {
        img.setPixelFormat(PixelFormat::RGB32F);
    }
    else if (nChannels == 4)
    {
        img.setPixelFormat(PixelFormat::RGBA32F);
    }
    else
    {
        return false;
    }

    img.resize(w, h);

    const Imf::ChannelList& channels = header.channels();
    // UINT  = 0, // unsigned int (32 bit)
    // HALF  = 1, // half (16 bit floating point)
    // FLOAT = 2, // float (32 bit floating point)
    Imf::PixelType pixelType = channels.begin().channel().type;

    DE_ERROR("pixelType = ", pixelType)
    int bitsPerPixel = 0;

    int k = 0;
    for (auto it = channels.begin(); it != channels.end(); ++it)
    {
        Imf::PixelType type = it.channel().type;

        if (pixelType != type)
        {
            DE_ERROR("Mixed pixelType[",k,"] = ", type, ", unsupported")
            //return false;
        }
        /*
        int i = 0;
        std::ostringstream k;
        k << "Channel["<< i<< "] "<< it.name()<< ": ";
        switch (type)
        {
        case Imf::HALF: { bitsPerPixel += 16; k << "HALF (16-bit float)"; break; }
        case Imf::FLOAT: { bitsPerPixel += 32; k << "FLOAT (32-bit float)"; break; }
        case Imf::UINT: { bitsPerPixel += 32; k << "UINT (32-bit integer)"; break; }
        default: { k << "UNKNOWN"; break; }
        }

        DE_TRACE(k.str())
*/
        k++;
    }

    // frameBuffer.insert("G", Imf::Slice(Imf::FLOAT, (char*)gBuffer.data(), sizeof(float), w * sizeof(float)));
    // frameBuffer.insert("B", Imf::Slice(Imf::FLOAT, (char*)bBuffer.data(), sizeof(float), w * sizeof(float)));

    // Allocate buffers for R, G, B, A channels
    //auto nPixel = size_t(w) * size_t(h);
    Imf::Array2D<float> rPixels(h,w);
    Imf::Array2D<float> gPixels;
    Imf::Array2D<float> bPixels;
    Imf::Array2D<float> aPixels;

    // Set up the frame buffer
    Imf::FrameBuffer frameBuffer;
    int xStride = sizeof(float);
    int yStride = sizeof(float) * w;

    auto ptrOffset = [&](Imf::Array2D<float>& pix) -> char* {
        return (char*)(&pix[0][0] - dw.min.x - dw.min.y * w);
    };

    if (channels.findChannel("R"))
    {
        frameBuffer.insert("R",         // Channel name
            Imf::Slice(Imf::FLOAT, ptrOffset(rPixels), xStride, yStride));
    }

    if (channels.findChannel("G"))
    {
        gPixels.resizeErase(h,w);
        frameBuffer.insert("G",
            Imf::Slice(Imf::FLOAT, ptrOffset(gPixels), xStride, yStride));
    }

    if (channels.findChannel("B"))
    {
        bPixels.resizeErase(h,w);
        frameBuffer.insert("B",
            Imf::Slice(Imf::FLOAT, ptrOffset(bPixels), xStride, yStride));
    }

    if (channels.findChannel("A"))
    {
        aPixels.resizeErase(h,w);
        frameBuffer.insert("A",
            Imf::Slice(Imf::FLOAT, ptrOffset(aPixels), xStride, yStride));
    }

    // Load pixels into the frame buffer
    file.setFrameBuffer(frameBuffer);
    file.readPixels(dw.min.y, dw.max.y);

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
                *pDst++ = gPixels[y][x];
                *pDst++ = bPixels[y][x];
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
                *pDst++ = gPixels[y][x];
                *pDst++ = bPixels[y][x];
                *pDst++ = aPixels[y][x];
            }
        }
    }
    else
    {
        DE_ERROR("Unsupported image ", img.str(true, false))
    }

    DE_TRACE("image loaded ", img.str(true, false))
    // DE_DEBUG("fMin = ", fMin)
    // DE_DEBUG("fMax = ", fMax)

    return true;

}



bool
ImageReaderEXR::load( Image & img, const std::string& uri )
{
    if ( uri.empty() )
    {
        DE_ERROR("Empty uri")
        return false;
    }

    printMetadata( uri );

    Imf::RgbaInputFile file(uri.c_str());

    const Imf::Header& header = file.header();
    const Imath::Box2i dw = header.dataWindow();
    const int w = dw.max.x - dw.min.x + 1;
    const int h = dw.max.y - dw.min.y + 1;
    if (w < 1)
    {
        DE_ERROR("Invalid width")
        return false;
    }
    if (h < 1)
    {
        DE_ERROR("Invalid height")
        return false;
    }

    const Imf::ChannelList& channels = file.header().channels();

    int numChannels = 0;
    for (auto it = channels.begin(); it != channels.end(); ++it)
    {
        numChannels++;
    }

    if (numChannels < 1)
    {
        return false;
    }
    else if (numChannels == 1)
    {
        img.setPixelFormat(PixelFormat::R32F);
    }
    else if (numChannels == 3)
    {
        img.setPixelFormat(PixelFormat::RGB32F);
    }
    else if (numChannels == 4)
    {
        img.setPixelFormat(PixelFormat::RGBA32F);
    }
    else
    {
        DE_ERROR("Unsupported numChannels ", numChannels)
        return false;
    }

    img.resize(w, h);


    Imf::PixelType pixelType = channels.begin().channel().type;
    DE_ERROR("pixelType = ", pixelType)
    int bitsPerPixel = 0;

    int k = 0;
    for (auto it = channels.begin(); it != channels.end(); ++it)
    {
        Imf::PixelType type = it.channel().type;

        if (pixelType != type)
        {
            DE_ERROR("Mixed pixelType[",k,"] = ", type, ", unsupported")
            //return false;
        }
/*
        int i = 0;
        std::ostringstream k;
        k << "Channel["<< i<< "] "<< it.name()<< ": ";
        switch (type)
        {
        case Imf::HALF: { bitsPerPixel += 16; k << "HALF (16-bit float)"; break; }
        case Imf::FLOAT: { bitsPerPixel += 32; k << "FLOAT (32-bit float)"; break; }
        case Imf::UINT: { bitsPerPixel += 32; k << "UINT (32-bit integer)"; break; }
        default: { k << "UNKNOWN"; break; }
        }

        DE_TRACE(k.str())
*/
        k++;
    }

    std::vector<Imf::Rgba> src( size_t(w) * size_t(h));
    file.setFrameBuffer(&src[0], 1, w);
    file.readPixels(dw.min.y, dw.max.y);


    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float* pDst = img.writePtr<float>();

        auto pSrc = (const Imf::Rgba*)src.data();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const Imf::Rgba& pixel = *pSrc;

                *pDst++ = pixel.r;

                pSrc++;
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();

        auto pSrc = (const Imf::Rgba*)src.data();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const Imf::Rgba& pixel = *pSrc;

                *pDst++ = pixel.r;
                *pDst++ = pixel.g;
                *pDst++ = pixel.b;

                pSrc++;
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();

        auto pSrc = (const Imf::Rgba*)src.data();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const Imf::Rgba& pixel = *pSrc;

                *pDst++ = pixel.r;
                *pDst++ = pixel.g;
                *pDst++ = pixel.b;
                *pDst++ = pixel.a;

                pSrc++;
            }
        }
    }
    else
    {
        DE_ERROR("Unsupported image ", img.str(true, false))
    }

/*
    // Allocate buffers for R, G, B, A channels
    //auto nPixel = size_t(w) * size_t(h);
    Imf::Array2D<float> rPixels(h,w);
    Imf::Array2D<float> gPixels;
    Imf::Array2D<float> bPixels;
    Imf::Array2D<float> aPixels;

    // Set up the frame buffer
    Imf::FrameBuffer frameBuffer;
    int xStride = sizeof(float);
    int yStride = sizeof(float) * w;

    auto ptrOffset = [&](Imf::Array2D<float>& pix) -> char* {
        return (char*)(&pix[0][0] - dw.min.x - dw.min.y * w);
    };

    if (channels.findChannel("R"))
    {
        frameBuffer.insert("R",         // Channel name
                           Imf::Slice(Imf::FLOAT, ptrOffset(rPixels), xStride, yStride));
    }

    if (channels.findChannel("G"))
    {
        gPixels.resizeErase(h,w);
        frameBuffer.insert("G",
                           Imf::Slice(Imf::FLOAT, ptrOffset(gPixels), xStride, yStride));
    }

    if (channels.findChannel("B"))
    {
        bPixels.resizeErase(h,w);
        frameBuffer.insert("B",
                           Imf::Slice(Imf::FLOAT, ptrOffset(bPixels), xStride, yStride));
    }

    if (channels.findChannel("A"))
    {
        aPixels.resizeErase(h,w);
        frameBuffer.insert("A",
                           Imf::Slice(Imf::FLOAT, ptrOffset(aPixels), xStride, yStride));
    }

    // Load pixels into the frame buffer
    file.setFrameBuffer(frameBuffer);
    file.readPixels(dw.min.y, dw.max.y);

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
                *pDst++ = gPixels[y][x];
                *pDst++ = bPixels[y][x];
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *pDst++ = rPixels[y][x];
                *pDst++ = gPixels[y][x];
                *pDst++ = bPixels[y][x];
                *pDst++ = aPixels[y][x];
            }
        }
    }
    else
    {
        DE_ERROR("Unsupported image ", img.str(true, false))
    }
*/

    /*
    std::vector<Imf::Rgba> src(w * h);
    file.setFrameBuffer(&src[0], 1, w);
    file.readPixels(dw.min.y, dw.max.y);
    //Imf::FrameBuffer frameBuffer;

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float fMin = std::numeric_limits<float>::max();
        float fMax = std::numeric_limits<float>::lowest();

        // Copy src -> dst, figure out min/max on the fly.
        {
            float* pDst = img.writePtr<float>();
            for (const auto & pixel : src)
            {
                const float R = convertToFloat(&pixel.r, pixelType); // 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
                fMin = std::min(fMin, R);
                fMax = std::max(fMax, R);
            }
        }

        // Normalize - Calculate the scaling factor and offset
        {
            float* pDst = img.writePtr<float>();
            for (const auto & pixel : src)
            {
                const float R = convertToFloat(&pixel.r, pixelType); // 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
                const float R2 = R / fMax; // factor * R + offset; // Transform luminance to height value
                *pDst++ = R2;
            }
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();
        for (const auto & pixel : src)
        {
            const float R = convertToFloat(&pixel.r, pixelType);
            const float G = convertToFloat(&pixel.g, pixelType);
            const float B = convertToFloat(&pixel.b, pixelType);
            *pDst++ = R;
            *pDst++ = G;
            *pDst++ = B;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();
        for (const auto & pixel : src)
        {
            const float R = convertToFloat(&pixel.r, pixelType);
            const float G = convertToFloat(&pixel.g, pixelType);
            const float B = convertToFloat(&pixel.b, pixelType);
            const float A = convertToFloat(&pixel.a, pixelType);
            *pDst++ = R;
            *pDst++ = G;
            *pDst++ = B;
            *pDst++ = A;
        }
    }
    else
    {
        DE_ERROR("Unsupported image ", img.str(true, false))
    }
    */

    DE_TRACE("image loaded ", img.str(true, false))
    // DE_DEBUG("fMin = ", fMin)
    // DE_DEBUG("fMax = ", fMax)

    return true;
}

/*

void loadEXR(const std::string& filename) {
    Imf::InputFile file(filename.c_str());
    const Imath::Box2i& dw = file.header().dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    const Imf::ChannelList& channels = file.header().channels();
    std::vector<float> floatData(width * height * channels.size());

    Imf::FrameBuffer frameBuffer;
    for (Imf::ChannelList::ConstIterator i = channels.begin(); i != channels.end(); ++i) {
        const char* channelName = i.name();
        Imf::PixelType type = i.channel().type;

        void* data = malloc(width * height * Imf::PixelTypeSize(type)); // Allocate memory for pixel data
        frameBuffer.insert(channelName, Imf::Slice(type, (char*)data, Imf::PixelTypeSize(type) * width, Imf::PixelTypeSize(type)));

        file.setFrameBuffer(frameBuffer);
        file.readPixels(dw.min.y, dw.max.y);

        for (int y = dw.min.y; y <= dw.max.y; ++y) {
            for (int x = dw.min.x; x <= dw.max.x; ++x) {
                int index = (y - dw.min.y) * width + (x - dw.min.x);
                floatData[index] = convertToFloat((char*)data + index * Imf::PixelTypeSize(type), type);
            }
        }

        free(data); // Free allocated memory
    }

    std::cout << "Loaded image with dimensions: " << width << " x " << height << std::endl;
    // Additional processing can be done here
}

int main() {
    const std::string filename = "heightmap.exr";
    try {
        printMetadata(filename);
        loadEXR(filename);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

CustomImage loadEXR(const std::string& filename) {
    Imf::RgbaInputFile file(filename.c_str());
    Imath::Box2i dw = file.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    CustomImage image(width, height);
    std::vector<Imf::Rgba> pixels(width * height);
    file.setFrameBuffer(&pixels[0], 1, width);
    file.readPixels(dw.min.y, dw.max.y);

    for (int i = 0; i < width * height; ++i) {
        image.bytes[i * 4] = static_cast<uint8_t>(std::max(0.0f, std::min(pixels[i].r, 1.0f)) * 255.0f);
        image.bytes[i * 4 + 1] = static_cast<uint8_t>(std::max(0.0f, std::min(pixels[i].g, 1.0f)) * 255.0f);
        image.bytes[i * 4 + 2] = static_cast<uint8_t>(std::max(0.0f, std::min(pixels[i].b, 1.0f)) * 255.0f);
        image.bytes[i * 4 + 3] = static_cast<uint8_t>(std::max(0.0f, std::min(pixels[i].a, 1.0f)) * 255.0f);
    }

    return image;
}

int main() {
    const std::string filename = "heightmap.exr";
    try {
        printMetadata(filename);
        CustomImage image = loadEXR(filename);
        std::cout << "Loaded image with dimensions: " << image.width << " x " << image.height << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

int main() {
    const std::string filename = "heightmap.exr";
    try {
        CustomImage image = loadEXR(filename);
        std::cout << "Loaded image with dimensions: " << image.width << " x " << image.height << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
    }
    return 0;
}
*/

#endif // DE_IMAGE_READER_EXR_ENABLED

// =================================== Writer ===================================

#ifdef DE_IMAGE_WRITER_EXR_ENABLED

bool
ImageWriterEXR::save( Image const & img, std::string const & uri, uint32_t param )
{
   // if ( uri.empty() ) { DE_ERROR("Empty uri(", uri, ")") return false; }
   // if ( img.empty() ) { DE_ERROR("Cant save empty image uri(", uri, ")") return false; }
   // int32_t w = img.getWidth();
   // int32_t h = img.getHeight();
   // if ( w < 1 || h < 1 ) { DE_ERROR("Got empty image(", uri, ")") return false; }

   // std::ofstream fout( uri.c_str() );
   // if ( fout.is_open() )
   // {
      // fout << o.str();
      // fout.close();
   // }

   return true;
}

#endif // DE_IMAGE_WRITER_EXR_ENABLED

} // end namespace image.
} // end namespace de.



















































#if 0

// XPM 1
static char const * const xpm_test1[] = {
   "/* XPM */",
   "16 16 6 1",
   " 	c None",
   ".	c #FFFFFF",
   "+	c #000000",
   "@	c #9A9A9A",
   "#	c #FEFEFE",
   "$	c #F7F7F7",
   "................",
   "................",
   "................",
   "................",
   "..++............",
   "..++............",
   "..++..++++.++++.",
   "..++..++@#.++@#.",
   "..++..++$..++$..",
   "..++..++...++...",
   "..++..++...++...",
   "..++..++...++...",
   "................",
   "................",
   "................",
   "................"
};

// XPM 2
static const char* const write_test_xpm[] = {
   "/* XPM */",
   "32 32 407 2",
   "  	c None",
   ". 	c #7373C1",
   "+ 	c #6E6EBF",
   "@ 	c #6B6BBF",
   "# 	c #6868BF",
   "$ 	c #6464BF",
   "Z+	c #FF4A4A",
   "`+	c #FF4545",
   " @	c #E73535",
   "`@	c #B8B819",
   " #	c #B7B717",
   "0#	c #C50000",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                      . + @ # $ % & * =                         ",
   "                  - ; > , ' ) ! ~ { ] ^ / (                     ",
   "                _ : < [ } | 1 2 3 4 5 6 7 8 9                   ",
   "                0 a b b c 2 3 4 5 6 d e f g h                   ",
   "                i j k l 2 4 5 6 d e f m n o p                   ",
   "                q r s t u 6 d e f m n v w x y                   ",
   "                z A B | C D f m E F G H I J K                   ",
   "                L M 1 N O m n P Q H R S T U V W X Y Z `  ...+.  ",
   "                p @.#.$.%.&.*.=.-.;.>.,.'.).!.~.{.].^./.(._.:.<.",
   "  [.}.|.1.2.3.4.5.6.6 7.&.8.I ;.9.0.a.b.c.d.e.f.g.h.i.j.k.l.m.n.",
   "}.o.p.q.r.s.t.u.v.w.O x.y.;.9.0.z.A.B.C.D.E.F.h.i.G.k.l.H.I.J.K.",
   "L.M.N.O.P.Q.R.S.T.U.V.e W.0.z.A.X.Y.Z.`.d. +.+G.k.l.H.I.J.++@+#+",
   "$+%+&+*+R.S.=+-+;+>+,+'+)+A.X.Y.!+~+{+]+^+/+(+_+H.I.J.:+<+[+}+|+",
   "1+2+3+4+=+-+5+6+7+8+9+0+a+b+c+d+e+f+g+h+i+j+k+I.l+m+n+o+p+q+r+s+",
   "t+u+v+w+5+6+7+x+y+z+A+B+C+D+E+F+G+H+I+J+K+h.L+++M+N+p+O+P+Q+R+S+",
   "T+U+2+V+7+x+y+W+X+Y+Z+`+ @.@        +@@@#@$@%@&@*@O+P+Q+R+=@-@;@",
   ">@,@'@)@y+!@~@{@Z+`+]@^@/@(@        _@:@<@[@}@O+P+Q+R+=@-@|@1@2@",
   "3@4@5@7+6@7@8@9@0@a@b@c@d@e@        f@g@h@i@j@Q+R+=@-@|@1@k@l@m@",
   "n@y+o@p@{@9@0@a@b@c@q@r@s@t@        u@v@i@w@Q+=@-@|@1@k@l@x@y@z@",
   "A@p@-+B@0@a@b@c@q@r@C@D@E@F@        G@H@I@J@K@L@M@N@O@P@Q@R@S@T@",
   "U@7@4@V@b@c@q@r@C@D@W@X@Y@Z@          `@ #.#+#@###$#%#&#*#=#=#  ",
   "-#;#>#,#q@r@C@D@W@X@'#)#!#~#                                    ",
   "{#(@]#^#/#(#_#:#<#[#}#|#1#                                      ",
   "    2#3#4#5#6#7#8#9#1#0#                                        ",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                                                                "
};

std::string
ImageWriterXPM::colorIndexToChars( uint32_t index, uint32_t bytesPerColor ) const
{
   std::string result;

   uint32_t i = index;
   c8* buf = new c8[bytesPerColor];
   uint32_t byteIndex = 0;

   while (byteIndex < bytesPerColor)
   {
      i = index / core::Math::pow( valid_chars.size(), bytesPerColor - byteIndex );
      buf[ bytesPerColor - byteIndex - 1 ] = valid_chars[ i%valid_chars.size() ];
      byteIndex++;
   }

   result = buf;
   delete [] buf;
   return result;
}

bool
ImageWriterXPM::save( Image const & img, std::string const & uri, uint32_t param )
{
   if ( img.isEmpty() ) return false;
#if IMPLEMENTED_2020
   // abort
   if (!file)
      return false;

   // size of source-image
    core::dimension2du Size = image->getDimension();

    // abort
    if (Size == core::dimension2du(0,0))
        return false;

   // create array to hold all unique RGB colors
    core::array< XPM_Color > colors(Size.Width*Size.Height);
    colors.set_used(0);

   // loop image and find all unique colors
    for (uint32_t y=0; y<Size.Height; y++)
    {
        for (uint32_t x=0; x<Size.Width; x++)
        {
         // get color
            SColor color = image->getPixel(x,y);

            // set alpha const to 255
            color.setAlpha(255);

            // loop color array to see if color already stored
            bool found = false;
            for (uint32_t i=0; i<colors.size(); i++)
            {
                if (colors[i].value == color)
                {
                    found = true;
                    break;
                }
            }

            // if not found in array, color is unique --> store
            if (!found)
            {
               XPM_Color data(color,"");
                colors.push_back(data);
            }
        }
    }

   // calculate needed bytes to store one color
   uint32_t bytesPerColor = 1;
   uint32_t k = colors.size();
   while (k>=valid_chars.size())
   {
      k/=valid_chars.size();
      bytesPerColor++;
   }

   // now give every unique color a corresponding byte-combination out of valid_chars
   uint32_t c=0;
   while ( c < colors.size() )
   {
      colors[c].key = colorIndexToChars(c, bytesPerColor);
      c++;
   }

   // uri
   std::string uri = file->getFileName();
   uri.make_lower();
   uri.replace('\\','/');

   // fileBaseName erase path
   std::string fileBaseName = uri;
   s32 pos = fileBaseName.findLast('/');
   if (pos != -1)
   {
      fileBaseName = fileBaseName.subString(pos+1, fileBaseName.size()-(pos+1));
   }

   // fileBaseName erase extension
   pos = fileBaseName.findLast('.');
   if (pos != -1)
   {
      fileBaseName = fileBaseName.subString(0,pos);
   }

   // XPM-Header
   // static const char *const wxwin32x32_xpm[] = {
   // "32 32 407 2",
   // "  	c None",

   // /* XPM */
   file->write("/* XPM */",9);
   endLine(file);

   // static const char *const wxwin32x32_xpm[] = {
   std::string s;
   s="static const char *const "; s+=fileBaseName; s+="_xpm[] = {";
   file->write(s.c_str(),s.size() );
   endLine(file);

   // "32 32 407 2"
   // Breite des Bildes (in Pixel)
   // Höhe des Bildes (in Pixel)
   // Anzahl der Farben im Bild
   // Anzahl der Zeichen pro Pixelwert
   // (X-Position des „Hotspots") --> MausCursor
   // (Y-Position des „Hotspots") --> MausCursor
   s="\""; s+=image->getDimension().Width;
   s+=" "; s+=image->getDimension().Height;
   s+=" "; s+=colors.size();
   s+=" "; s+=bytesPerColor; s+="\",";
   file->write(s.c_str(),s.size() );
   endLine(file);

   // write color-table
   // "  	c None",
   // ". 	c #7373C1",
   // "+ 	c #6E6EBF",
   // "@ 	c #6B6BBF",
   // "# 	c #6868BF",
   // "$ 	c #6464BF",
   // "Z+	c #FF4A4A",
   // "`+	c #FF4545",
   // " @	c #E73535",
   // "`@	c #B8B819",
   // " #	c #B7B717",
   // "0#	c #C50000",

   for (uint32_t i=0; i<colors.size(); i++)
   {
      file->write("\"",1);
      file->write( colors[i].key.c_str(), colors[i].key.size() );
      file->write(" c ",3);

      // format color #RRGGBB
      c8 buf[7];
      const SColor& c = colors[i].value;
      sprintf(buf,"#%02x%02x%02x", c.getRed(), c.getGreen(), c.getBlue());
      std::string tmp = buf;
      tmp.make_upper();
      file->write(tmp.c_str(), tmp.size());

      file->write("\",",2);
      endLine(file);
   }

   // write pixels
   //	"$+%+&+*+R.S.=+-+;+>+,+'+)+A.X.Y.!+~+{+]+^+/+(+_+H.I.J.:+<+[+}+|+",

   // buffer for current line
   std::string line;

   // loop
   for (uint32_t y=0; y<Size.Height; y++)
   {
      line = "\"";
      for (uint32_t x=0; x<Size.Width; x++)
      {
         // current pixel
         SColor colorNow = image->getPixel(x,y);
         colorNow.setAlpha(255);

         // search color in color-table
         uint32_t i=0;
         while (i<colors.size())
         {
            if (colors[i].value == colorNow)
            {
               break;
            }
            i++;
         }

         // write found color-table color
         line+=colors[i].key;
      }
      line+="\"";
      if (y < Size.Height-1)
         line+=",";
      else
         line+="};";

      file->write(line.c_str(),line.size() );
      endLine(file);
   }
#endif
   return true;
}

#endif
