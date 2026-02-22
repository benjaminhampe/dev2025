#include <de/smesh/SMeshIO.h>
#include <de/gpu/VideoDriver.h>

#ifndef DE_MESH_READER_3DS_ENABLED
#define DE_MESH_READER_3DS_ENABLED
#endif

// #ifndef DE_MESH_WRITER_3DS_ENABLED
// #define DE_MESH_WRITER_3DS_ENABLED
// #endif

#ifndef DE_MESH_READER_ASSIMP_ENABLED
#define DE_MESH_READER_ASSIMP_ENABLED
#endif


// *.3ds
#if defined(DE_MESH_READER_3DS_ENABLED)
    #include <de/smesh/io/SMeshReader_3DS.h>
#endif
#if defined(DE_MESH_WRITER_3DS_ENABLED)
    #include <de/smesh/io/SMeshWriter_3DS.h>
#endif

// *.fbx
#if defined(DE_MESH_READER_ASSIMP_ENABLED)
    #include <de/smesh/io/SMeshReader_ASSIMP.h>
#endif
#if defined(DE_MESH_WRITER_ASSIMP_ENABLED)
    #include <de/smesh/io/SMeshWriter_ASSIMP.h>
#endif

namespace de {
namespace smesh {


// ===========================================================================
// ===   MeshCodecManager
// ===========================================================================

IOManager::IOManager()
    : m_driver(nullptr)
{
    // initThreadPool();

    #ifdef DE_MESH_READER_3DS_ENABLED
    m_reader.push_back( new SMeshReader3DS );
    #endif
    #ifdef DE_MESH_WRITER_3DS_ENABLED
    m_writer.push_back( new SMeshWriter3DS );
    #endif
    #ifdef DE_MESH_READER_ASSIMP_ENABLED
        m_reader.push_back( new SMeshReaderASSIMP );
    #endif
    #ifdef DE_MESH_WRITER_ASSIMP_ENABLED
        m_writer.push_back( new SMeshWriterASSIMP );
    #endif

    DE_DEBUG("Supported mesh reader: ", m_reader.size())
    DE_DEBUG("Supported mesh writer: ", m_writer.size())
}

IOManager::~IOManager()
{
    DE_DEBUG("Released ",m_reader.size()," MeshReader")
    DE_DEBUG("Released ",m_writer.size()," MeshWriter")

    for ( size_t i = 0; i < m_reader.size(); ++i )
    {
        if ( m_reader[ i ] )
        {
            delete m_reader[ i ];
        }
    }
    m_reader.clear();

    for ( size_t i = 0; i < m_writer.size(); ++i )
    {
        if ( m_writer[ i ] )
        {
            delete m_writer[ i ];
        }
    }
    m_writer.clear();

    // clearImages();
}

void
IOManager::setVideoDriver( gpu::VideoDriver* driver )
{
    m_driver = driver;
}

// static
std::shared_ptr< IOManager >
IOManager::get()
{
    static std::shared_ptr< IOManager > s_Manager( new IOManager() );
    return s_Manager;
}


bool
IOManager::loadMesh( SMesh & m, std::string uri,
    uint8_t const* p, uint64_t n, LoadOptions const & options ) const
{
    if ( !p ) { DE_ERROR("!p") return false; }

    if ( n < 12 ) { DE_ERROR("n(",n,") < 12") return false; }

    PerformanceTimer timer;

    if (options.m_logLevel > 0)
    {
        timer.start();
    }

    auto ext = FileSystem::fileSuffix( uri );
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

    auto reader = getReaderFromExtension( ext );
    if ( !reader )
    {
        std::ostringstream o; o << "No reader for ext("<<ext<<"), uri("<<uri<<"), nBytes(" << n << ")";
        DE_ERROR(o.str())
        if (options.m_throwOnFail)
        {
            throw std::runtime_error(o.str());
        }
        return false;
    }

    auto ok = reader->load( m_driver, m, uri, p, n, options );
    if ( ok )
    {
        // applyMeshLoadOptions( m, options );
    }

    timer.stop();

    if (options.m_logLevel > 0)
    {
        if ( ok )
        {
            DE_DEBUG("ms(",timer.ms(),") to load img(",m.str(),")")
        }
        else
        {
            DE_ERROR("Cant load image ms(",timer.ms(),"), uri(",uri,")")
        }
    }
    return ok;
}


bool
IOManager::loadMesh( SMesh & m, std::string uri, LoadOptions const & options ) const
{
    PerformanceTimer timer;

    if (options.m_logLevel > 0)
    {
        timer.start();
    }

    uri = FileSystem::makeAbsolute( uri );

    if (!FileSystem::existFile(uri))
    {
        if (options.m_throwOnFail)
        {
            std::ostringstream o;
            o << "ImageFile does not exist, " << uri;
            throw std::runtime_error(o.str());
        }
        return false;
    }

    auto suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        const auto fileMagic = FileMagic::getFileMagicFromFile( uri );
        if (!fileMagic)
        {
            std::ostringstream o;
            o << "Cannot determine fileMagic from empty suffix. " << uri;
            DE_ERROR(o.str())
            if (options.m_throwOnFail)
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

    auto reader = getReaderFromExtension( suffix );
    if ( !reader )
    {
        auto o = dbStr("No ImageReader found file(",uri,"), suffix(", suffix, ")");
        DE_ERROR(o)

        if (options.m_throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    auto ok = reader->load( m_driver, m, uri );
    if ( !ok )
    {
        m.setName( uri );
        timer.stop();
        auto o = dbStr("Cant Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,")");
        DE_ERROR(o)
        if (options.m_throwOnFail)
        {
            throw std::runtime_error(o);
        }
        return false;
    }

    // applyMeshLoadOptions( m, options );
    m.setName( uri );

    if (options.m_logLevel > 0)
    {
        timer.stop();
        DE_OK("[",suffix,"] ", timer.ms(), "ms|", uri,"|", m.str(false))
    }
    return ok;
}

bool
IOManager::saveMesh( SMesh const & m, std::string uri,
                        SaveOptions const & opt ) const
{
    PerformanceTimer timer;

    if (opt.m_logLevel > 0)
    {
        timer.start();
    }

    if ( uri.empty() )
    {
        DE_ERROR("Empty uri.")
        return false;
    }

    uri = FileSystem::makeAbsolute( uri );

    auto suffix = FileSystem::fileSuffix( uri );
    if ( suffix.empty() )
    {
        DE_ERROR("Empty suffix")
        return false;
    }

    auto writer = getWriterFromExtension(suffix);
    if (!writer)
    {
        DE_ERROR("No writer for extension ", suffix)
        return false;
    }

    auto dir = FileSystem::fileDir( uri );
    if (!FileSystem::existDirectory( dir ))
    {
        FileSystem::createDirectory( dir );
    }

    auto ok = writer->save( m_driver, m, uri, opt );

    if (opt.m_logLevel > 0)
    {
        timer.stop();

        if ( ok )
        {
            DE_OK("[",suffix,"] ",timer.ms(),"ms|", uri, "|", m.str(false))
        }
        else
        {
            DE_ERROR("Cant Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), mesh(", m.str(false), ")")
        }
    }

    return ok;
}

IReader*
IOManager::getReaderFromExtension( std::string const & suffix ) const
{
    for ( auto reader : m_reader )
    {
        if ( reader->isSupportedExtension( suffix ) )
        {
            return reader;
        }
    }
    return nullptr;
}

IWriter*
IOManager::getWriterFromExtension( std::string const & suffix ) const
{
    for ( auto writer : m_writer )
    {
        if ( writer->isSupportedExtension( suffix ) )
        {
            return writer;
        }
    }
    return nullptr;
}

} // end namespace smesh.
} // end namespace de.


// ===========================================================================
// DarkMesh GDK API
// ===========================================================================

bool
dbLoadMesh(de::smesh::SMesh & m, std::string uri,
    de::smesh::LoadOptions const & options )
{
    return de::smesh::IOManager::get()->loadMesh( m, uri, options );
}

bool
dbLoadMesh(de::smesh::SMesh & m, std::string uri, uint8_t const* p, uint64_t n,
    de::smesh::LoadOptions const & options )
{
    return de::smesh::IOManager::get()->loadMesh( m, uri, p, n, options );
}

bool
dbSaveMesh(de::smesh::SMesh const & m, std::string uri,
    de::smesh::SaveOptions const & options )
{
    return de::smesh::IOManager::get()->saveMesh( m, uri, options );
}


/*
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

Image
Image::convert( PixelFormat outputFormat ) const
{
    if ( this->w() < 1 || this->h() < 1 )
    {
        DE_ERROR("Empty image cannot be converted." )
        return Image();
    }

    auto pConverter = PixelFormatConverter::getConverter( this->pixelFormat(), outputFormat );
    if ( !pConverter )
    {
        DE_ERROR("Cannot convert image(", str(), ") to format ", outputFormat.str() )
        return Image();
    }

    Image outputImage( this->w(), this->h(), outputFormat );
    pConverter(this->data(), outputImage.data(), this->pixelCount());
    return outputImage;
}

inline void applyImageLoadOptions( Image & img, const ImageLoadOptions & options )
{
    if ( options.outputFormat != PixelFormat::Unknown
        && options.outputFormat != img.pixelFormat() )
    {
        Image tmp = img.convert(options.outputFormat);
        img = tmp;
    }

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
