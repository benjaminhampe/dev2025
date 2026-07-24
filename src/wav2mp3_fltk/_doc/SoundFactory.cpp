#include <de/sound/SoundFactory.h>
//#include <de/sound/SoundFactoryConfig.h>

// *.ppm
#if defined(DE_SOUND_READER_PPM_ENABLED) || defined(DE_SOUND_WRITER_PPM_ENABLED)
   #include <de/image/Image_PPM.h>
#endif

// *.jpg, *.jpeg
#if defined(DE_SOUND_READER_JPG_ENABLED) || defined(DE_SOUND_WRITER_JPG_ENABLED)
   #include <de/image/Image_JPG.h>
#endif

// *.png
#if defined(DE_SOUND_READER_PNG_ENABLED) || defined(DE_SOUND_WRITER_PNG_ENABLED)
   #include <de/image/Image_PNG.h>
#endif

// *.xpm
#if defined(DE_SOUND_READER_XPM_ENABLED) || defined(DE_SOUND_WRITER_XPM_ENABLED)
   #include <de/image/Image_XPM.h>
#endif


// *.exr HDR formats
#if defined(DE_SOUND_READER_EXR_ENABLED) || defined(DE_SOUND_WRITER_EXR_ENABLED)
   #include <de/image/Image_EXR.h>
#endif

// *.webp
#if defined(DE_SOUND_READER_WEBP_ENABLED) || defined(DE_SOUND_WRITER_WEBP_ENABLED)
   #include <de/image/Image_WEBP.h>
#endif

// NEW:

// *.bmp
#if defined(DE_SOUND_READER_BMP_ENABLED) || defined(DE_SOUND_WRITER_BMP_ENABLED)
   #include <de/image/Image_BMP.h>
#endif

// *.dds
#if defined(DE_SOUND_READER_DDS_ENABLED) || defined(DE_SOUND_WRITER_DDS_ENABLED)
   #include <de/image/Image_DDS.h>
#endif

// *.gif
#if defined(DE_SOUND_READER_GIF_ENABLED) || defined(DE_SOUND_WRITER_GIF_ENABLED)
   #include <de/image/Image_GIF.h>
#endif

// *.ico, *.cur
#if defined(DE_SOUND_READER_ICO_ENABLED) || defined(DE_SOUND_WRITER_ICO_ENABLED)
   #include <de/image/Image_ICO.h>
#endif

// *.tga
#if defined(DE_SOUND_READER_TGA_ENABLED) || defined(DE_SOUND_WRITER_TGA_ENABLED)
   #include <de/image/Image_TGA.h>
#endif

// *.tif, *.tiff, *.tif4, *.tiff4
#if defined(DE_SOUND_READER_TIF_ENABLED) || defined(DE_SOUND_WRITER_TIF_ENABLED)
    #include <de/image/Image_TIF.h>
#endif

// *.raw/*.raw.meta
#if defined(DE_SOUND_READER_RAW_ENABLED) || defined(DE_SOUND_WRITER_RAW_ENABLED)
   #include <de/image/Image_RAW.h>
#endif

// *.rgb/*.rgba/*.sgi
#if defined(DE_SOUND_READER_RGB_ENABLED) || defined(DE_SOUND_WRITER_RGB_ENABLED)
   #include <de/image/Image_RGB.h>
#endif

// *.wal
#if defined(DE_SOUND_READER_WAL_ENABLED) || defined(DE_SOUND_WRITER_WAL_ENABLED)
    #include <de/image/Image_WAL.h>
#endif


// *.pcx
#if defined(DE_SOUND_READER_PCX_ENABLED) || defined(DE_SOUND_WRITER_PCX_ENABLED)
   #include <de/image/Image_PCX.h>
#endif

// *.ppm
#if defined(DE_SOUND_READER_PPM_ENABLED) || defined(DE_SOUND_WRITER_PPM_ENABLED)
   #include <de/image/Image_PPM.h>
#endif

// *.html
#if defined(DE_SOUND_READER_HTML_ENABLED) || defined(DE_SOUND_WRITER_HTML_ENABLED)
   #include <de/image/Image_HTML.h>
#endif

namespace de
{

// static
std::string PixelFormat::getString( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8 : return "R8G8B8A8";
    case PixelFormat::R8G8B8   : return "R8G8B8";
    case PixelFormat::R5G6B5   : return "R5G6B5";
    case PixelFormat::R5G5B5A1 : return "R5G5B5A1";
    case PixelFormat::R8       : return "R8";
    case PixelFormat::R16      : return "R16";
    case PixelFormat::R24      : return "R24";
    case PixelFormat::R32      : return "R32";
    case PixelFormat::R32F     : return "R32F";
    //case PixelFormat::R32FG32FB32F : return "R32FG32FB32F";
    case PixelFormat::RGB16F  : return "RGB16F";
    case PixelFormat::RGBA16F : return "RGBA16F";
    case PixelFormat::RGB32F  : return "RGB32F";
    case PixelFormat::RGBA32F : return "RGBA32F";

    case PixelFormat::A1R5G5B5 : return "A1R5G5B5";
    case PixelFormat::B8G8R8A8 : return "B8G8R8A8";
    case PixelFormat::B8G8R8   : return "B8G8R8";
    case PixelFormat::D24S8    : return "D24S8";
    case PixelFormat::D32F     : return "D32F";
    case PixelFormat::D32FS8   : return "D32FS8";
    case PixelFormat::S8       : return "S8";
    default                    : return "Unknown"; // Auto
    }
}

// static
uint32_t PixelFormat::getBitsPerPixel( int const fmt )
{
    switch (fmt)
    {
    case PixelFormat::R8G8B8A8 : return 32;
    case PixelFormat::R8G8B8   : return 24;
    case PixelFormat::R5G6B5   : return 16;
    case PixelFormat::R5G5B5A1 : return 16;
    case PixelFormat::R8       : return 8;
    case PixelFormat::R16      : return 16;
    case PixelFormat::R24      : return 24;
    case PixelFormat::R32      : return 32;
    case PixelFormat::R32F     : return 32;
    case PixelFormat::RGB32F   : return 96;
    case PixelFormat::RGBA32F  : return 128;
    case PixelFormat::RGB16F   : return 48;
    case PixelFormat::RGBA16F  : return 64;
    case PixelFormat::A1R5G5B5 : return 16;
    case PixelFormat::B8G8R8A8 : return 32;
    case PixelFormat::B8G8R8   : return 24;
    case PixelFormat::D24S8    : return 32;
    case PixelFormat::D32F     : return 32;
    case PixelFormat::D32FS8   : return 40;
    case PixelFormat::S8       : return 8;
    default                    : return 0;
    }
}

// static
uint32_t PixelFormat::getBytesPerPixel( int const fmt )
{
    return getBitsPerPixel(fmt) / 8;
}

// ===========================================================================
// ===   ImageLoadOptions
// ===========================================================================

inline void applyImageLoadOptions( Image & img, const ImageLoadOptions & options )
{
    if ( options.outputFormat != PixelFormat::Unknown
        && options.outputFormat != img.pixelFormat() )
    {
        Image tmp = img.convert(options.outputFormat);
        img = tmp;
    }

    /*
    if ( options.repairBadAlpha )
    {
        repairBadAlpha( img );
    }

    if ( options.rotate90 )
    {
        img = ImageRotate::rotateRight( img );
    }

    if ( options.m_SearchColor != options.m_ReplaceColor )
    {
    // DE_MAIN_DEBUG("ReplaceColor")
    ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    if ( options.m_AutoSaturate )
    {
    // DE_MAIN_DEBUG("Saturate")
    ImagePainter::autoSaturate( img );
    }

    if ( options.m_Brighten != 1.0f )
    {
    ImagePainter::brighten( img, options.m_Brighten );
    }
    if ( options.m_Gamma != 1.0f )
    {
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    if ( options.m_Contrast != 1.0f )
    {
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    }
    */
    // if ( options.m_SearchColor != options.m_ReplaceColor )
    // {
    // // DE_MAIN_DEBUG("ReplaceColor")
    // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // if ( options.m_AutoSaturate )
    // {
    // // DE_MAIN_DEBUG("Saturate")
    // ImagePainter::autoSaturate( img );
    // }

    // if ( options.m_Brighten != 1.0f )
    // {
    // ImagePainter::brighten( img, options.m_Brighten );
    // }
    // if ( options.m_Gamma != 1.0f )
    // {
    // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // if ( options.m_Contrast != 1.0f )
    // {
    // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
    // }
    // }

}



// ===========================================================================
// ===   ImageCodecManager
// ===========================================================================

ImageCodecManager::ImageCodecManager()
{
    initThreadPool();

#ifdef DE_SOUND_READER_JPG_ENABLED
   m_Reader.push_back( new image::ImageReaderJPG );
#endif
#ifdef DE_SOUND_WRITER_JPG_ENABLED
   m_Writer.push_back( new image::ImageWriterJPG );
#endif

#ifdef DE_SOUND_READER_PNG_ENABLED
   m_Reader.push_back( new image::ImageReaderPNG );
#endif
#ifdef DE_SOUND_WRITER_PNG_ENABLED
   m_Writer.push_back( new image::ImageWriterPNG );
#endif

#ifdef DE_SOUND_READER_WEBP_ENABLED
    m_Reader.push_back( new image::ImageReaderWEBP );
#endif
#ifdef DE_SOUND_WRITER_WEBP_ENABLED
    m_Writer.push_back( new image::ImageWriterWEBP );
#endif

#ifdef DE_SOUND_READER_PPM_ENABLED
   m_Reader.push_back( new image::ImageReaderPPM );
#endif
#ifdef DE_SOUND_WRITER_PPM_ENABLED
   m_Writer.push_back( new image::ImageWriterPPM );
#endif

#ifdef DE_SOUND_READER_XPM_ENABLED
    m_Reader.push_back( new image::ImageReaderXPM );
#endif
#ifdef DE_SOUND_WRITER_XPM_ENABLED
    m_Writer.push_back( new image::ImageWriterXPM );
#endif

#ifdef DE_SOUND_READER_EXR_ENABLED
    m_Reader.push_back( new image::ImageReaderEXR );
#endif
#ifdef DE_SOUND_WRITER_EXR_ENABLED
    m_Writer.push_back( new image::ImageWriterEXR );
#endif


// NEW:

#ifdef DE_SOUND_READER_BMP_ENABLED
   m_Reader.push_back( new image::ImageReaderBMP );
#endif
#ifdef DE_SOUND_WRITER_BMP_ENABLED
   m_Writer.push_back( new image::ImageWriterBMP );
#endif

#ifdef DE_SOUND_READER_DDS_ENABLED
   m_Reader.push_back( new image::ImageReaderDDS );
#endif
#ifdef DE_SOUND_WRITER_DDS_ENABLED
   m_Writer.push_back( new image::ImageWriterDDS );
#endif

#ifdef DE_SOUND_READER_GIF_ENABLED
   m_Reader.push_back( new image::ImageReaderGIF );
#endif
#ifdef DE_SOUND_WRITER_GIF_ENABLED
   m_Writer.push_back( new image::ImageWriterGIF );
#endif

#ifdef DE_SOUND_READER_ICO_ENABLED
   m_Reader.push_back( new image::ImageReaderICO );
#endif
#ifdef DE_SOUND_WRITER_ICO_ENABLED
   m_Writer.push_back( new image::ImageWriterICO );
#endif

#ifdef DE_SOUND_READER_TGA_ENABLED
   m_Reader.push_back( new image::ImageReaderTGA );
#endif
#ifdef DE_SOUND_WRITER_TGA_ENABLED
   m_Writer.push_back( new image::ImageWriterTGA );
#endif

#ifdef DE_SOUND_READER_TIF_ENABLED
   m_Reader.push_back( new image::ImageReaderTIF );
#endif
#ifdef DE_SOUND_WRITER_TIF_ENABLED
   m_Writer.push_back( new image::ImageWriterTIF );
#endif

#ifdef DE_SOUND_READER_RAW_ENABLED
   m_Reader.push_back( new image::ImageReaderRAW );
#endif
#ifdef DE_SOUND_WRITER_RAW_ENABLED
   m_Writer.push_back( new image::ImageWriterRAW );
#endif

#ifdef DE_SOUND_READER_RGB_ENABLED
   m_Reader.push_back( new image::ImageReaderRGB );
#endif
#ifdef DE_SOUND_WRITER_RGB_ENABLED
   m_Writer.push_back( new image::ImageWriterRGB );
#endif

#ifdef DE_SOUND_READER_WAL_ENABLED
   m_Reader.push_back( new image::ImageReaderWAL );
#endif
#ifdef DE_SOUND_WRITER_WAL_ENABLED
   m_Writer.push_back( new image::ImageWriterWAL );
#endif

#ifdef DE_SOUND_READER_PSD_ENABLED
   m_Reader.push_back( new image::ImageReaderPSD );
#endif
#ifdef DE_SOUND_WRITER_PSD_ENABLED
   m_Writer.push_back( new image::ImageWriterPSD );
#endif

#ifdef DE_SOUND_READER_PPM_ENABLED
   m_Reader.push_back( new image::ImageReaderPPM );
#endif
#ifdef DE_SOUND_WRITER_PPM_ENABLED
   m_Writer.push_back( new image::ImageWriterPPM );
#endif

#ifdef DE_SOUND_READER_PCX_ENABLED
   m_Reader.push_back( new image::ImageReaderPCX );
#endif
#ifdef DE_SOUND_WRITER_PCX_ENABLED
   m_Writer.push_back( new image::ImageWriterPCX );
#endif


#ifdef DE_SOUND_WRITER_HTML_ENABLED
   m_Writer.push_back( new image::ImageWriterHtmlTable );
#endif

#ifdef _DEBUG
   DE_DEBUG("Add image codec readers and writers")
   DE_DEBUG("Supported image reader: ", m_Reader.size())
   DE_DEBUG("Supported image writer: ", m_Writer.size())
   // AsciiArt::test();
#endif
}

ImageCodecManager::~ImageCodecManager()
{
#ifdef _DEBUG
    DE_DEBUG("Released ",m_Reader.size()," ImageReader")
    DE_DEBUG("Released ",m_Writer.size()," ImageWriter")
    // std::string load_ext = //getSupportedReadExtensions();
    // std::string save_ext = //getSupportedWriteExtensions();
    // DE_DEBUG("Released ",m_Reader.size()," ImageReader with FileFormats: ", load_ext )
    // DE_DEBUG("Released ",m_Writer.size()," ImageWriter with FileFormats: ", save_ext )
#endif
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      if ( m_Reader[ i ] )
      {
         delete m_Reader[ i ];
      }
   }
   m_Reader.clear();

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      if ( m_Writer[ i ] )
      {
         delete m_Writer[ i ];
      }
   }
   m_Writer.clear();

   // clearImages();
}

// static
std::shared_ptr< ImageCodecManager >
ImageCodecManager::get()
{
   static std::shared_ptr< ImageCodecManager > s_ImageManager( new ImageCodecManager() );
   return s_ImageManager;
}


// =====================
//   Convert images
// =====================
bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
    std::string loadSuffix = FileSystem::fileSuffix( loadName );
    std::string saveSuffix = FileSystem::fileSuffix( saveName );
    if ( loadSuffix == saveSuffix )
    {
        //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
        return FileSystem::copyFile( loadName, saveName );
    }

    loadName = FileSystem::makeAbsolute( loadName );
    saveName = FileSystem::makeAbsolute( saveName );

    Image img;
    bool ok = loadImage( img, loadName );
    if ( ok )
    {
        //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
        ok = saveImage( img, saveName );
        if ( ok )
        {
            //DE_OK("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
        }
        else
        {
            //DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
        }
    }
    else
    {
        //DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
    }

    return ok;
}

bool
ImageCodecManager::loadImage( Image & img, uint8_t const* p, uint64_t n, std::string uri, ImageLoadOptions const & options ) const
{
    if ( !p ) { DE_ERROR("!p") return false; }

    if ( n < 12 ) { DE_ERROR("n(",n,") < 12") return false; }

    PerformanceTimer timer;

    if (options.debugLog)
    {
        timer.start();
    }

    std::string ext = FileSystem::fileSuffix( uri );
    if (ext.empty())
    {
        const auto fileMagic = FileMagic::getFileMagic( p );
        if (fileMagic)
        {
            ext = FileMagic::getString(fileMagic);
        }
        else
        {
            auto b0 = StringUtil::hex(p[0]);
            auto b1 = StringUtil::hex(p[1]);
            auto b2 = StringUtil::hex(p[2]);
            auto b3 = StringUtil::hex(p[3]);
            DE_ERROR("Did not find FileMagic for bytes b0(",b0,"), b1(",b1,"), b2(",b2,"), b3(",b3,")")
        }
    }

    IImageReader* reader = getReaderFromExtension( ext );
    if ( !reader )
    {
        std::ostringstream o; o << "No reader for ext("<<ext<<"), uri("<<uri<<"), nBytes(" << n << ")";
        DE_ERROR(o.str())
        if (options.throwOnFail)
        {
            throw std::runtime_error(o.str());
        }
        return false;
    }

    bool ok = reader->load( img, p, n, uri );
    if ( ok )
    {
        applyImageLoadOptions( img, options );
    }

    timer.stop();

    if (options.debugLog)
    {
        if ( ok )
        {
            DE_DEBUG("ms(",timer.ms(),") to load img(",img.str(),")")
        }
        else
        {
            DE_ERROR("Cant load image ms(",timer.ms(),"), uri(",uri,")")
        }
    }
    return ok;
}


bool
ImageCodecManager::loadImage( Image & img, std::string uri, ImageLoadOptions const & options ) const
{
    PerformanceTimer timer;

    if (options.debugLog)
    {
        timer.start();
    }

    uri = FileSystem::makeAbsolute( uri );

    if (!FileSystem::existFile(uri))
    {
        if (options.throwOnFail)
        {
            std::ostringstream o;
            o << "ImageFile does not exist, " << uri;
            throw std::runtime_error(o.str());
        }
        return false;
    }

    std::string suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        const auto fileMagic = FileMagic::getFileMagicFromFile( uri );
        if (!fileMagic)
        {
            std::ostringstream o;
            o << "Cannot determine fileMagic from empty suffix. " << uri;
            DE_ERROR(o.str())
            if (options.throwOnFail)
            {
                throw std::runtime_error(o.str());
            }
            return false;
        }
        else
        {
            suffix = FileMagic::getString(fileMagic);
        }
    }

    IImageReader* reader = getReaderFromExtension( suffix );
    if ( !reader )
    {
        auto o = dbStr("No ImageReader found file(",uri,"), suffix(", suffix, ")");
        DE_ERROR(o)

        if (options.throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    bool ok = reader->load( img, uri );
    if ( !ok )
    {
        img.setUri( uri );
        timer.stop();
        auto o = dbStr("Cant Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,")");
        DE_ERROR(o)
        if (options.throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    applyImageLoadOptions( img, options );
    img.setUri( uri );

    if (options.debugLog)
    {
        timer.stop();
        DE_OK("[",suffix,"] ", timer.ms(), "ms|", uri,"|", img.str(false))
    }
    return ok;
}

bool
ImageCodecManager::saveImage( Image const & img, std::string uri, uint32_t quality ) const
{
    PerformanceTimer timer;

    bool debugLog = true;

    if (debugLog)
    {
        timer.start();
    }

    if ( uri.empty() )
    {
        DE_ERROR("Empty uri.")
        return false;
    }

    uri = FileSystem::makeAbsolute( uri );

    std::string suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        DE_ERROR("Empty suffix")
        return false;
    }

    IImageWriter* writer = getWriterFromExtension(suffix);
    if (!writer)
    {
        DE_ERROR("No writer for extension ", suffix)
        return false;
    }

    std::string dir = FileSystem::fileDir( uri );
    if (!FileSystem::existDirectory( dir ))
    {
        FileSystem::createDirectory( dir );
    }

    const bool ok = writer->save( img, uri, quality );

    if (debugLog)
    {
        timer.stop();

        if ( ok )
        {
            DE_OK("[",suffix,"] ",timer.ms(),"ms|", uri, "|", img.str(false))
        }
        else
        {
            DE_ERROR("Cant Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.str(false), ")")
        }
    }

    return ok;
}

IImageReader*
ImageCodecManager::getReaderFromExtension( std::string const & suffix ) const
{
    for ( IImageReader* reader : m_Reader )
    {
        if ( reader->isSupportedReadExtension( suffix ) )
        {
            return reader;
        }
    }
    return nullptr;
}

IImageWriter*
ImageCodecManager::getWriterFromExtension( std::string const & suffix ) const
{
    for ( IImageWriter* writer : m_Writer )
    {
        if ( writer->isSupportedWriteExtension( suffix ) )
        {
            return writer;
        }
    }
    return nullptr;
}


/*
bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
    std::string loadSuffix = FileSystem::fileSuffix( loadName );
    std::string saveSuffix = FileSystem::fileSuffix( saveName );
    if ( loadSuffix == saveSuffix )
    {
        //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
        return FileSystem::copyFile( loadName, saveName );
    }

    loadName = FileSystem::makeAbsolute( loadName );
    saveName = FileSystem::makeAbsolute( saveName );

    Image img;
    bool ok = loadImage( img, loadName );
    if ( ok )
    {
        //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
        ok = saveImage( img, saveName );
        if ( ok )
        {
            DE_OK("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
        }
        else
        {
            DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
        }
    }
    else
    {
        DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
    }

    return ok;
}
*/

} // end namespace de.



// ===========================================================================
// DarkImageAPI
// ===========================================================================

bool
dbLoadImage(de::Image & img, std::string uri,
            de::ImageLoadOptions const & options )
{
    return de::ImageCodecManager::get()->loadImage( img, uri, options );
}

bool
dbSaveImage(de::Image const & img, std::string uri, uint32_t quality )
{
    return de::ImageCodecManager::get()->saveImage( img, uri, quality );
}

bool
dbLoadImage(de::Image & img, uint8_t const* data, uint64_t bytes, std::string uri,
            de::ImageLoadOptions const & options )
{
    return de::ImageCodecManager::get()->loadImage( img, data, bytes, uri, options );
}
