#pragma once
#include <de/smesh/SMesh.h>

#ifndef DE_MESH_READER_3DS_ENABLED
#define DE_MESH_READER_3DS_ENABLED
#endif

// #ifndef DE_MESH_WRITER_3DS_ENABLED
// #define DE_MESH_WRITER_3DS_ENABLED
// #endif

// #ifndef DE_MESH_READER_ASSIMP_ENABLED
// #define DE_MESH_READER_ASSIMP_ENABLED
// #endif


namespace de {
namespace gpu {

    struct VideoDriver;

} // end namespace gpu.
} // end namespace de.

// ===========================================================================

namespace de {
namespace smesh {

// ================
struct LoadOptions
// ================
{
    uint32_t m_quality;
    int m_logLevel;
    bool m_throwOnFail;
    LoadOptions()
        : m_quality(0)
        , m_logLevel(0)
        , m_throwOnFail(false)
    {}
};

// ============
struct IReader
// ============
{
    // Virtual:
    virtual ~IReader() {}

    // Virtual:
    virtual std::vector< std::string >
    getSupportedExtensions() const = 0;

    // Virtual:
    virtual bool
    load( gpu::VideoDriver* driver, SMesh & m, std::string const & uri,
         const uint8_t* p, size_t n, LoadOptions const & opt = LoadOptions() ) = 0;

    // Inline:
    virtual bool
    isSupportedExtension( const std::string& ext ) const
    {
        auto const & exts = getSupportedExtensions();
        // Find 'ext':
        auto itFound = std::find_if(exts.begin(),exts.end(),
            [ext](auto const& cached){ return cached == ext; } );

        // Found 'ext':
        return (itFound != exts.end());
    }

    // Inline:
    virtual bool
    load( gpu::VideoDriver* driver, SMesh & m, std::string const & uri, const Blob & blob,
        LoadOptions const & opt = LoadOptions() )
    {
        return load(driver, m, uri, blob.data(), blob.size(), opt);
    }

    // Inline:
    virtual bool
    load( gpu::VideoDriver* driver, SMesh & m, std::string const & uri,
        LoadOptions const & opt = LoadOptions() )
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
        return load( driver, m, uri, blob.data(), blob.size(), opt );
    }

    // Old and candidate for removal: loadFile resource.
    //virtual bool
    //load( Image & img, Binary & file, const S& uri = "" ) { return false; }
};

// ================
struct SaveOptions
// ================
{
    uint32_t m_quality;
    int m_logLevel;

    SaveOptions()
        : m_quality(0)
        , m_logLevel(0)
    {}
};

// ============
struct IWriter
// ============
{
    virtual ~IWriter() {}

    virtual bool
    save( gpu::VideoDriver* driver, SMesh const & m,
         std::string const & uri,
         SaveOptions const& opt = SaveOptions() ) = 0;

    virtual std::vector< std::string >
    getSupportedExtensions() const = 0;

    //virtual bool
    //isSupportedExtension( std::string const & suffix ) const = 0;

    // Inline:
    virtual bool
    isSupportedExtension( const std::string& ext ) const
    {
        auto const & exts = getSupportedExtensions();
        // Find 'ext':
        auto itFound = std::find_if(exts.begin(),exts.end(),
            [ext](auto const& cached){ return cached == ext; } );

        // Found 'ext':
        return (itFound != exts.end());
    }
};

// ===========================================================================
class IOManager
// ===========================================================================
{
    gpu::VideoDriver* m_driver;
    std::vector< IReader* > m_reader;
    std::vector< IWriter* > m_writer;
    //std::vector< Image* > m_Images;

    // ThreadPoolWithTasks m_threadPoolWithTasks;

public:
    IOManager();
    ~IOManager();

    void
    setVideoDriver( gpu::VideoDriver* driver );

    static std::shared_ptr< IOManager >
    get();

    // =====================
    //   Load mesh
    // =====================
    bool
    loadMesh( SMesh & m, std::string uri,
             LoadOptions const & opt = LoadOptions() ) const;

    bool
    loadMesh( SMesh & m, std::string uri, uint8_t const* data, uint64_t bytes,
            LoadOptions const & opt = LoadOptions() ) const;

    // =====================
    //   Save mesh
    // =====================

    bool
    saveMesh( SMesh const & m, std::string uri,
             SaveOptions const & opt = SaveOptions() ) const;

    // =====================
    //   Convert meshes
    // =====================
    //bool
    //convertFile( std::string loadName, std::string saveName ) const;

    // =====================
    //   Get reader/writer
    // =====================
    IReader*
    getReaderFromExtension( std::string const & suffix ) const;

    IWriter*
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


    /*
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

};

} // end namespace mesh.
} // end namespace de.

// ===========================================================================
// DarkMesh GDK API
// ===========================================================================

bool
dbLoadMesh(de::smesh::SMesh & m, std::string uri,
           de::smesh::LoadOptions const & options = de::smesh::LoadOptions() );

bool
dbLoadMesh(de::smesh::SMesh & m, std::string uri, uint8_t const* p, uint64_t n,
           de::smesh::LoadOptions const & options = de::smesh::LoadOptions() );

bool
dbSaveMesh(de::smesh::SMesh const & m, std::string uri,
           de::smesh::SaveOptions const & options = de::smesh::SaveOptions() );

