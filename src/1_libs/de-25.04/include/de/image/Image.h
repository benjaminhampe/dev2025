#pragma once
#include <cstdint>
//#include <sstream>
#include <vector>
//#include <array>
#include <memory> // SharedPtr
//#include <map>    // GlyphCache

#include <de/Color.h>

// We like to load/save images async with 100% CPU usage, then try -march=native and such.
#include <de/ThreadPoolWithTasks.h>

namespace de {

// ===================================================================
class PixelFormat /// @brief A color format for images and textures mostly.
// ===================================================================
{
public:
    enum ePixelFormat : uint8_t
    {
        Unknown = 0,
        // 32-Bit (default)
        R8G8B8A8,   // RGBA_32  :: 32-bit color (default) for all decent painting operations, for format, png, webp, tga.
        B8G8R8A8,   // BGRA_32  :: for format, png, webp,
        A8B8G8R8,   // ABGR_32  :: for format, png, webp,
        A8GR88B8,   // ARGB_32  :: for format, png, webp,
        // 24-Bit (default)
        R8G8B8,     // RGB_24   :: 24-bit 888 opaque color, for jpeg and bmp
        B8G8R8,     // BGR_24   :: 24-bit 888 opaque color, for jpeg and bmp
        // 16-Bit (default)

        R5G6B5,     // RGB_16   :: 16-bit 565 opaque color.
        B5G6R5,     // BGR_16   :: 16-bit 565 opaque color.
        // 16-Bit
        R5G5B5A1,   // RGBA_16  :: 16-bit 5551 transparent color. (1-bit transparency)
        B5G5R5A1,   // BGRA_16  :: 16-bit 5551 transparent color. (1-bit transparency)
        A1R5G5B5,   // ARGB_16  :: 16-bit 5551 transparent color. (1-bit transparentcy)
        A1B5G5R5,   // ABGR_16  :: 16-bit 5551 transparent color. (1-bit transparentcy)
        // 15-Bit
        R5G5B5X1,   // RGBx_16  :: 16-bit 555x opaque color. (1-bit non functional transparency)
        B5G5R5X1,   // BGRx_16  :: 16-bit 555x opaque color. (1-bit non functional transparency)
        X1R5G5B5,   // xRGB_16  :: 16-bit 555x opaque color. (1-bit non functional transparency)
        X1B5G5R5,   // xBGR_16  :: 16-bit 555x opaque color. (1-bit non functional transparency)

        // Red channel format:
        R8,         // :: 16-bit red channel format.
        R16,        // :: 16-bit red channel format.
        R24,        // :: 16-bit red channel format.
        R32,        // :: 16-bit red channel format.
        R32F,       // :: 16-bit red channel format. HeightMap geoTIFF (*.tif) or HDR (.exr) for 3D engine terrain meshes.

        // 32-Bit:
        N24H8,      // :: Combined NormalMap(RGB_24) + Height/Bump/ElevationMap(R_8) format as (RGBA_32). (32 Bit)
        D24S8,      // :: Combined DepthMap(D_24) + StencilMap(S_8) format. as (D24_S8) (32 Bit)
        D32F,       // :: Separate DepthMap(D_32f) format. (32-Bit) format.
        //S32,      // :: Separate StencilMap integer S32 (32-Bit).
        // 40-Bit:
        D32FS8,     // :: Combined DepthMap + StencilMap combined format D32f+S8 (40 Bit)
        // 8-Bit:
        S8,         // :: Separate StencilMap integer S8 (8-Bit).

        // 48-Bit:
        RGB16F,
        RGBA16F,
        RGB32F, // DiffuseMap HDR (.exr) for 3D engine terrain meshes.
        RGBA32F, // DiffuseMap HDR (.exr) for 3D engine terrain meshes.

        MaxCount,

        // ==================
        // Alias: 32-Bit (default)
        RGBA_32 = R8G8B8A8,
        BGRA_32 = B8G8R8A8,
        ABGR_32 = A8B8G8R8,
        ARGB_32 = A8GR88B8,
        RGBA_8888 = R8G8B8A8,
        BGRA_8888 = B8G8R8A8,
        ABGR_8888 = A8B8G8R8,
        ARGB_8888 = A8GR88B8,
        // ==================
        // Alias: 24-Bit
        RGB_24 = R8G8B8,
        BGR_24 = B8G8R8,
        RGB_888 = R8G8B8,
        BGR_888 = B8G8R8,
        // ==================
        // Alias: 16-Bit
        RGB_16 = R5G6B5,        // :: 16-bit 565 opaque color.
        BGR_16 = B5G6R5,        // :: 16-bit 565 opaque color.
        RGB_565 = R5G6B5,       // :: 16-bit 565 opaque color.
        BGR_565 = B5G6R5,       // :: 16-bit 565 opaque color.
        // ==================
        // Alias: 15x-Bit
        RGBx_16 = R5G5B5X1,     // :: 16-bit 555 color with 1-bit transparency.
        BGRx_16 = B5G5R5X1,     // :: 16-bit 555 color with 1-bit transparency.
        xRGB_16 = X1R5G5B5,     // :: 16-bit 555 color with 1-bit transparency.
        xBGR_16 = X1B5G5R5,     // :: 16-bit 555 color with 1-bit transparency.
        // ==================
        // Alias: 16-Bit
        RGBA_16 = R5G5B5A1,     // :: 16-bit 555 color with 1-bit transparency.
        BGRA_16 = B5G5R5A1,     // :: 16-bit 555 color with 1-bit transparency.
        ARGB_16 = A1R5G5B5,     // :: 16-bit 555 color with 1-bit transparency.
        ABGR_16 = A1B5G5R5,     // :: 16-bit 555 color with 1-bit transparency.
        RGBA_5551 = R5G5B5A1,   // :: 16-bit 555 color with 1-bit transparency.
        BGRA_5551 = B5G5R5A1,   // :: 16-bit 555 color with 1-bit transparency.
        ARGB_1555 = A1R5G5B5,   // :: 16-bit 555 color with 1-bit transparency.
        ABGR_1555 = A1B5G5R5,   // :: 16-bit 555 color with 1-bit transparency.
    };

    uint8_t m_pixelFormat;

    PixelFormat() : m_pixelFormat(Unknown) {}
    PixelFormat(ePixelFormat pxfmt) : m_pixelFormat(pxfmt) {}
    std::string str() const { return getString(m_pixelFormat); }
    operator int() const { return m_pixelFormat; }
    uint32_t bitsPerPixel() const { return getBitsPerPixel(m_pixelFormat); }
    uint32_t bytesPerPixel() const { return getBytesPerPixel(m_pixelFormat); }

    static std::string getString( int const fmt );
    static uint32_t getBitsPerPixel( int const fmt );
    static uint32_t getBytesPerPixel( int const fmt );
    static uint32_t getChannelCount( int const fmt );
    static uint8_t getRedBits( int const fmt );
    static uint8_t getGreenBits( int const fmt );
    static uint8_t getBlueBits( int const fmt );
    static uint8_t getAlphaBits( int const fmt );
    static uint8_t getDepthBits( int const fmt );
    static uint8_t getStencilBits( int const fmt );
};

// =======================================================================
struct ImageLoadOptions
// =======================================================================
{
    PixelFormat outputFormat;
    uint32_t searchColor;
    uint32_t replaceColor;

    bool throwOnFail;
    bool debugLog;
    bool rotate90;
    // bool convertToGrey121;
    // bool convertToGrey111;
    // bool autoSaturate;
    // bool repairBadAlpha;
    // float brighten;
    // float gamma;
    // float contrast;

    // std::string m_DefaultExportExt;

    ImageLoadOptions()
        : outputFormat( PixelFormat::Unknown )
        , searchColor( 0 )
        , replaceColor( 0 )
        , throwOnFail( false )
        , debugLog( false )
        , rotate90( false )
        // , m_ConvertToGrey121( false )
        // , m_ConvertToGrey111( false )
        // , m_AutoSaturate( false )
        // , m_RepairBadAlpha( false )
        // , m_Brighten( 1.0f )
        // , m_Gamma( 1.0f )
        // , m_Contrast( 1.0f )
        // , m_DefaultExportExt( "raw" )
    {}
};

// ===========================================================================
struct Image
// ===========================================================================
{
    typedef std::unique_ptr< Image > Ptr;
    typedef std::shared_ptr< Image > SharedPtr;
    static Image* create( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
    static Ptr createPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
    static SharedPtr createSharedPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );

    Image();
    Image( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
    Image( int32_t w, int32_t h, uint32_t fillColor, PixelFormat format );
    // ~Image() = default;
    // Image& operator= ( Image const & other );

    int32_t w() const { return m_width; }
    int32_t h() const { return m_height; }
    uint32_t r() const { return PixelFormat::getRedBits( m_pixelFormat ); }
    uint32_t g() const { return PixelFormat::getGreenBits( m_pixelFormat ); }
    uint32_t b() const { return PixelFormat::getBlueBits( m_pixelFormat ); }
    uint32_t a() const { return PixelFormat::getAlphaBits( m_pixelFormat ); }
    uint32_t d() const { return PixelFormat::getDepthBits( m_pixelFormat ); }
    uint32_t s() const { return PixelFormat::getStencilBits( m_pixelFormat ); }
    Recti rect() const { return Recti(0,0,w(),h()); }
    std::string const & uri() const { return m_uri; }
    PixelFormat pixelFormat() const { return m_pixelFormat; }
    bool isDebug() const { return m_debug; }
    double pts() const { return m_pts; }
    //inline glm::ivec2 dim() const { return { w(), h() }; }
    void setWidth( int w ) { m_width = w; }
    void setHeight( int h ) { m_height = h; }
    void setUri( std::string uri ) { m_uri = uri; }
    void setPixelFormat( PixelFormat fmt );
    void setDebug(bool enable) { m_debug = enable; }
    void setPTS(double pts) { m_pts = pts; }

    uint32_t channelCount() const { return PixelFormat::getChannelCount( m_pixelFormat ); }
    std::string pixelFormatStr() const { return PixelFormat::getString( m_pixelFormat ); }

    uint32_t bitsPerPixel() const { return m_bytesPerPixel * 8; }
    uint32_t bytesPerPixel() const { return m_bytesPerPixel; }
    uint64_t byteCount() const { return pixelCount() * bytesPerPixel(); }
    uint64_t pixelCount() const { return uint64_t( m_width ) * uint64_t( m_height ); }
    uint32_t stride() const { return uint32_t( w() ) * bytesPerPixel(); }

    uint64_t computeMemoryConsumption() const
    {
        uint64_t n = sizeof(*this);
        n += m_data.capacity();
        return n;
    }

    std::string str( bool withUri = true, bool withBytes = false ) const;
    bool empty() const;
    bool equals( const Image& other ) const;

    void clear( bool forceShrink = false );
    void resize( int32_t w, int32_t h );
    void resize( int32_t w, int32_t h, PixelFormat pixelFormat );

    Image convert( PixelFormat pixelFormat ) const;

    void shrink_to_fit();
    void fill( const glm::vec4& color );
    void fill( uint32_t color );
    void fillZero();
    void flipVertical();
    void flipHorizontal();
    Image copy( Recti const & pos ) const;
    void floodFill( int32_t x, int32_t y, uint32_t newColor, uint32_t oldColor );
    void switchRB();

    uint8_t* getPixels( int32_t x, int32_t y );
    uint8_t const* getPixels( int32_t x, int32_t y ) const { return getPixels(x,y); }

    void setPixel( int32_t x, int32_t y, uint32_t color, bool blend = false );
    uint32_t getPixel( int32_t x, int32_t y, uint32_t colorKey = 0x00000000 ) const;

    float getPixelf( int32_t x, int32_t y, float colorKey = 0.0f ) const;

    glm::vec4
    getPixel4f( int32_t x, int32_t y, const glm::vec4& colorKey = glm::vec4() ) const;

    void
    setPixel4f( int32_t x, int32_t y, glm::vec4 color, bool blend = false );

    void copyColFrom( std::vector< uint8_t > & out, int32_t x );
    void copyColTo( std::vector< uint8_t > const & col, int32_t x );
    void copyRowFrom( std::vector< uint8_t > & out, int32_t y );
    void copyRowTo( std::vector< uint8_t > const & row, int32_t y );

    uint8_t* getColumn( int32_t x );
    uint8_t* getRow( int32_t y );

    uint8_t const* getColumn( int32_t x ) const { return getColumn( x ); }
    uint8_t const* getRow( int32_t y ) const { return getRow( y ); }

    //   float getPixelR32F( int32_t x, int32_t y ) const
    //   {
    //      return float( getPixel(x,y) );
    //      //return *reinterpret_cast< float const * const >( &color );
    //   }

    template < typename T > T*       writePtr() { return reinterpret_cast< T* >( m_data.data() ); }
    template < typename T > T const* readPtr() const { return reinterpret_cast< T const * >( m_data.data() ); }
    uint8_t*                         data() { return m_data.data(); }
    uint8_t const*                   data() const { return m_data.data(); }
    uint64_t                         size() const { return m_data.size(); }
    uint64_t                         capacity() const { return m_data.capacity(); }

    Blob &                           blob() { return m_data; }
    Blob const &                     blob() const { return m_data; }

private:
    int32_t m_width;           // same as (min) col count that has meaningful data
    int32_t m_height;          // same as (min) row count
    uint16_t m_bytesPerPixel;
    PixelFormat m_pixelFormat; // ColorBits A + R + G + B, if any
    bool m_debug;
    double m_pts;
    Blob m_data;
    std::string m_uri;
};

// ===========================================================================
struct IImageReader
// ===========================================================================
{
    virtual ~IImageReader() {}

    virtual std::vector< std::string >
    getSupportedReadExtensions() const = 0;

    virtual bool
    isSupportedReadExtension( const std::string& suffix ) const = 0;

    // New: loadMemory resource.
    virtual bool
    load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) = 0;

    // New: loadMemory resource.
    virtual bool
    load( Image & img, const Blob & blob, const std::string& uri = "" )
    {
        return load(img, blob.data(), blob.size(), uri);
    }

    // Old: loadFile resource.
    virtual bool
    load( Image & img, const std::string& uri )
    {
        if ( uri.empty() )
        {
            DE_ERROR("Empty uri")
            return false;
        }

        Blob blob;
        if (!dbLoadBlob(blob, uri))
        {
            DE_ERROR( "No blob ",uri )
            return false;
        }
        return load( img, blob.data(), blob.size(), uri );
    }

    // Old and candidate for removal: loadFile resource.
    //virtual bool
    //load( Image & img, Binary & file, const S& uri = "" ) { return false; }
};

// ===========================================================================
struct IImageWriter
// ===========================================================================
{
   virtual ~IImageWriter() {}

   virtual bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) = 0;

   virtual std::vector< std::string >
   getSupportedWriteExtensions() const = 0;

   virtual bool
   isSupportedWriteExtension( std::string const & suffix ) const = 0;
};


// ===========================================================================
class ImageCodecManager
// ===========================================================================
{
    std::vector< IImageReader* > m_Reader;
    std::vector< IImageWriter* > m_Writer;
    //std::vector< Image* > m_Images;

    ThreadPoolWithTasks m_threadPoolWithTasks;

public:
    ImageCodecManager();
    ~ImageCodecManager();

    static std::shared_ptr< ImageCodecManager >
    get();

    ThreadPoolWithTasks &
    getThreadPool() { return m_threadPoolWithTasks; }

    void initThreadPool()
    {
        uint32_t nThreads = std::thread::hardware_concurrency() - 1;
        m_threadPoolWithTasks.reset( nThreads );
    }

    typedef std::function<void(Image*)> FN_IMAGE_CALLBACK;

    static void
    joinAsync() { get()->getThreadPool().wait_for_tasks(); }

    static void
    loadAsync( std::string uri,
                    const ImageLoadOptions& options,
                    const FN_IMAGE_CALLBACK& onLoadFinished )
    {

        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                auto img = new Image();
                if (get()->loadImage(*img,uri))
                {
                    onLoadFinished(img);
                }
                else
                {
                    delete img;
                }
            }
        );

        //g_threadPool.wait_for_tasks();
    }

    static void
    saveAsync(Image* img, std::string uri,
                     const ImageLoadOptions& options,
                     const FN_IMAGE_CALLBACK& onSaveFinished )
    {
        if (!img)
        {
            return;
        }

        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                if (get()->saveImage(*img,uri))
                {
                    onSaveFinished(img); // User must delete images
                }
                else
                {
                    delete img;
                }
            }
        );

        //g_threadPool.wait_for_tasks();
    }

    /*
    static void
    addAsyncConvertTask(std::string loadUri, std::string saveUri,
                     const ImageLoadOptions& options,
                     const FN_IMAGE_CALLBACK& onSaveFinished )
    {


        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                if (get()->saveImage(*img,uri))
                {
                    onSaveFinished(img); // User must delete images
                }
                else
                {
                    delete img;
                }
            }
            );

        //g_threadPool.wait_for_tasks();
    }
    */

    // =====================
    //   Load images
    // =====================
    bool
    loadImage( Image & img, std::string uri, ImageLoadOptions const & options = ImageLoadOptions() ) const;

    bool
    loadImage( Image & img, uint8_t const* data, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const;

    // =====================
    //   Save images
    // =====================

    bool
    saveImage( Image const & img, std::string uri, uint32_t quality = 0 ) const;

    // =====================
    //   Convert images
    // =====================
    bool
    convertFile( std::string loadName, std::string saveName ) const;

    // =====================
    //   Get reader/writer
    // =====================

    IImageReader*
    getReaderFromExtension( std::string const & suffix ) const;

    IImageWriter*
    getWriterFromExtension( std::string const & suffix ) const;

    // =====================
    //   Deprecated
    // =====================

    // bool
    // isSupportedReadExtension( std::string const & uri ) const;

    // std::vector< std::string >
    // getSupportedReadExtensionVector() const;

    // std::string
    // getSupportedReadExtensions() const;

    // bool
    // isSupportedWriteExtension( std::string const & uri ) const;

    // std::string
    // getSupportedWriteExtensions() const;

    // std::vector< std::string >
    // getSupportedWriteExtensionVector() const;

};


// ===================================================================
struct PixelFormatConverter
// ===================================================================
{
    /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
    /// @param[i] s Source color array
    /// @param[i] n Number of elements to convert, not a byte count.
    /// @param[o] d Destination color array
    typedef void (*Converter_t)( void const * /* src */, void * /* dst */, size_t /* n-color-reads */ );
    /// @brief Get color converter
    static Converter_t getConverter( PixelFormat const & src, PixelFormat const & dst );
    /// @brief Convert color 24 to 24, used in ImageWriterJPG
    //static void convert_R8G8B8_to_R8G8B8( void const * src, void* dst, size_t n );
    static void convert_RGB24_to_RGB24( void const * src, void* dst, size_t n );
    /// @brief Convert color 32 to 24, used in ImageWriterJPG
    static void convert_R8G8B8A8_to_R8G8B8( void const * src, void* dst, size_t n );
    /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
    static void convert_R5G6B5_to_R8G8B8( void const * src, void* dst, size_t n );
    /// @brief Convert color 16 to 32, used in ImageWriterPNG
    static void convert_R5G6B5_to_R8G8B8A8( void const * src, void* dst, size_t n );
    /// @brief Convert color 16 to 24, used in ImageWriterBMP
    static void convert_R5G5B5A1_to_R8G8B8( void const * src, void* dst, size_t n );
    /// @brief Convert color 32 to 32, used in ImageWriterBMP.
    static void convert_R8G8B8A8_to_R8G8B8A8( void const * src, void* dst, size_t n );
    /// @brief Convert color 16 to 32, used in ImageWriterBMP.
    static uint32_t R5G5B5A1_to_R8G8B8A8( uint16_t color );
    /// @brief Convert color 16 to 32
    static void convert_R5G5B5A1_to_R8G8B8A8( void const * src, void* dst, size_t n );
    /// @brief Convert color 16 to 16, used in ImageWriterBMP.
    static void convert_R5G5B5A1_to_R5G5B5A1( void const * src, void* dst, size_t n );
    /// @brief Convert color 24 to 32, used in ImageLoaderJPG.
    //static void convert_R8G8B8_to_R8G8B8A8( void const * src, void* dst, size_t n );
    static void convert_RGB_24_to_RGBA_32( void const * src, void* dst, size_t n );
    /// @brief Convert color A1R5G5B5 Color from R5G6B5 color
    static uint16_t R5G6B5_to_R5G5B5A1( uint16_t color );
    // Used in: ImageWriterTGA
    static void convert_R5G6B5_to_R5G5B5A1( void const * src, void* dst, size_t n );
    // Used in: ImageWriterBMP
    /// copies R8G8B8 24bit data to 24bit data
    static void convert24Bit_to_24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr );
    // Used in: ImageWriterBMP
    static void convert_R8G8B8A8_to_B8G8R8( void const * src, void* dst, size_t n );
    static void convert_R8G8B8_to_B8G8R8( void const * src, void* dst, size_t n );
    static void convert_B8G8R8_to_R8G8B8( void const * src, void* dst, size_t n );
    static void convert_R5G5B5A1_to_B8G8R8( void const * src, void* dst, size_t n );
    static void convert_R5G6B5_to_B8G8R8( void const * src, void* dst, size_t n );
    static void convert_R8G8B8A8_to_B8G8R8A8( void const * src, void* dst, size_t n );
    static void convert_B8G8R8A8_to_R8G8B8A8( void const * src, void* dst, size_t n );
    static void convert_R8G8B8_to_B8G8R8A8( void const * src, void* dst, size_t n );
    // used in ImageReaderBMP
    static void convert_B8G8R8_to_R8G8B8A8( void const * src, void* dst, size_t n );
    // used in ImageReaderEXR
    static void convert_R32F_to_R32F( void const * src, void* dst, size_t n );
    static void convert_R32F_to_RGB_888( void const * src, void* dst, size_t n );
    static void convert_R32F_to_RGBA_8888( void const * src, void* dst, size_t n );
    static void convert_RGB32F_to_RGB_888( void const * src, void* dst, size_t n );
    static void convert_RGB32F_to_RGBA_8888( void const * src, void* dst, size_t n );

    static void convert_RGB32F_to_R32F( void const * src, void* dst, size_t n );
    static void convert_RGBA32F_to_R32F( void const * src, void* dst, size_t n );


    static void convert_R8G8B8A8_to_R8( void const * src, void* dst, size_t n );
    static void convert_R8G8B8_to_R8( void const * src, void* dst, size_t n );
    static void convert_B8G8R8_to_R8( void const * src, void* dst, size_t n );
    static void convert_B8G8R8A8_to_R8( void const * src, void* dst, size_t n );

    static void convert_R5G5B5A1_to_R8( void const * src, void* dst, size_t n );
    static void convert_R5G6B5_to_R8( void const * src, void* dst, size_t n );

    static void convert_R32F_to_R8( void const * src, void* dst, size_t n );
    static void convert_RGB32F_to_R8( void const * src, void* dst, size_t n );
    static void convert_RGBA32F_to_R8( void const * src, void* dst, size_t n );

    /*
    static void ConvertRGB_to_YUV(const uint8_t* rgb_data, size_t pixel_count)
    {
        for (size_t i = 0; i < pixel_count; ++i)
        {
            uint8_t r = rgb_data[i * 3 + 0];
            uint8_t g = rgb_data[i * 3 + 1];
            uint8_t b = rgb_data[i * 3 + 2];

            float y = 0.299f * r + 0.587f * g + 0.114f * b;
            float u = -0.147f * r - 0.289f * g + 0.436f * b;
            float v = 0.615f * r - 0.515f * g - 0.100f * b;

            //std::cout << "Pixel " << i << ": Y=" << y << ", U=" << u << ", V=" << v << "\n";

            // return { y,u,v };
        }
    }
    */
};


// =======================================================================
struct Brush
// =======================================================================
{
   Brush()
      : color( 0xFFFFFFFF )
      , pattern( nullptr )
   {}
   Brush( uint32_t color_,
          Image* pattern_ = nullptr )
      : color( color_ )
      , pattern( pattern_ )
   {}

   uint32_t color;
   Image* pattern;
};

} // end namespace de.

// From file:
bool
dbLoadImage(de::Image & img, std::string uri,
            de::ImageLoadOptions const & options = de::ImageLoadOptions() );

// From memory:
bool
dbLoadImage(de::Image & img, uint8_t const* data, uint64_t bytes, std::string uri,
            de::ImageLoadOptions const & options = de::ImageLoadOptions() );

// To file:
bool
dbSaveImage(de::Image const & img, std::string uri, uint32_t quality = 0 );


/*
#if defined(DE_IMAGE_READER_XPM_ENABLED)
bool
dbLoadImageXPM( de::Image & dst, std::vector< std::string > const & xpm_data );
#endif
*/

bool
dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst );

//de::Image
//dbResizeImage(  de::Image const & img,
//                int w,
//                int h,
//                int scaleOp = 1,
//                uint32_t keyColor = 0x00000000 );

//bool
//dbConvertImageFile( std::string loadName, std::string saveName );

//void
//dbDrawText5x8(  de::Image & img,
//                int x,
//                int y,
//                std::string const & msg,
//                uint32_t color = 0xFF000000,
//                de::Align align = de::Align::Default );

/*
void
dbDrawText5x8(    de::Image & img, int x, int y,
               std::string const & msg, uint32_t color, de::Align align )
{
   auto font = de::getFontFace5x8();
   if ( !font ) return;
   font->drawText( img, x, y, msg, color, align );
}

*/


// bool dbExistImage( int imageId );
// bool dbLoadImage( int imageId, std::string const & uri, ImageLoadOptions const & options = ImageLoadOptions() );
// bool dbLoadImage( int imageId, std::string const & uri, uint8_t const* fileData, uint64_t fileSize, ImageLoadOptions const & options = ImageLoadOptions() );
// bool dbImageFromXPM( int imageId, std::vector< std::string > const & xpm_data );
// bool dbSaveImage( int imageId, std::string const & uri, int quality = 0 );
// bool dbDeleteImage( int imageId );
// int dbImageWidth( int imageId );
// int dbImageHeight( int imageId );
// int dbImageFormat( int imageId );
// void* dbImageRowPtr( int imageId, int y );
// int dbImageRowByteSize( int imageId );
// int dbImagePixelBytes( int imageId );
// void* dbImagePixels( int imageId );
// void* dbImagePixels( int imageId, int x, int y );
// uint32_t dbGetImagePixel( int imageId, int x, int y );
// void dbSetImagePixel( int imageId, int x, int y, int color );

/*
// bool dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst );

// de::Image dbResizeImage( de::Image const & img, int w, int h, int scaleOp = 1, uint32_t keyColor = 0x00000000 );

// bool dbConvertImageFile( std::string loadName, std::string saveName );

// void dbDrawText5x8(  de::Image & img, int x, int y, std::string const & msg, uint32_t color = 0xFF000000, de::Align align = de::Align::Default );

void dbDrawText5x8( de::Image & img, int x, int y, std::string const & msg, uint32_t color, de::Align align )
{
   auto font = de::getFontFace5x8();
   if ( !font ) return;
   font->drawText( img, x, y, msg, color, align );
}

*/
