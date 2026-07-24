#pragma once
#include <de/sound/Sound.h>
// #include <de/ThreadPoolWithTasks.h>

// *.mp3 -> dr.mp3
#ifndef DE_SOUND_READER_MP3_ENABLED
#define DE_SOUND_READER_MP3_ENABLED
#endif

// *.mp3 -> lame3100
#ifndef DE_IMAGE_WRITER_MP3_ENABLED
#define DE_IMAGE_WRITER_MP3_ENABLED
#endif

namespace de {
    
// =======================================================================
struct SoundLoadOptions
// =======================================================================
{
    Sound::eSampleType outputFormat = Sound::ST_Unknown;
    bool throwOnFail = false;
    bool debugLog = false;
};

// =======================================================================
struct SoundSaveOptions
// =======================================================================
{
    Sound::eSampleType outputFormat = Sound::ST_Unknown;
    bool throwOnFail = false;
    bool debugLog = false;
    uint32_t bitrate = 224; // in kbit/sec
    uint32_t quality = 0; // psychoacustic model, 0 = best, 9 = worst.
};

// ===========================================================================
struct ISoundReader
// ===========================================================================
{
    virtual ~ISoundReader() {}

    virtual const std::vector< std::string >&
    getSupportedReadExtensions() const = 0;

    bool isSupportedReadExtension( const std::string & suffix ) const
    {
        const auto& liste = getSupportedReadExtensions();
        return std::find(liste.begin(), liste.end(), suffix) != liste.end();
    }

    // New: loadMemory resource.
    virtual bool
    load( Sound & sound, const uint8_t* p, size_t n, const std::string& uri = "" ) = 0;

    // New: loadMemory resource.
    virtual bool
    load( Sound & sound, const Blob & blob, const std::string& uri = "" )
    {
        return load(img, blob.data(), blob.size(), uri);
    }

    // Old: loadFile resource.
    virtual bool
    load( Sound & sound, const std::string& uri )
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
};

// ===========================================================================
struct ISoundWriter
// ===========================================================================
{
    virtual ~ISoundWriter() {}

    virtual const std::vector< std::string >&
    getSupportedWriteExtensions() const = 0;

    bool
    isSupportedWriteExtension( const std::string & suffix ) const
    {
        const auto& liste = getSupportedWriteExtensions();
        return std::find(liste.begin(), liste.end(), suffix) != liste.end();
    }

    virtual bool
    save(const Sound& sound, const std::string& uri, const SaveOptions& options ) = 0;
};


// ===========================================================================
class SoundFactory
// ===========================================================================
{
    std::vector< ISoundReader* > m_reader;
    std::vector< ISoundWriter* > m_writer;
    //std::vector< Image* > m_Images;

    ThreadPoolWithTasks m_threadPoolWithTasks;

public:
    SoundFactory();
    ~SoundFactory();

    static std::shared_ptr< SoundFactory >
    get();

    ThreadPoolWithTasks &
    getThreadPool() { return m_threadPoolWithTasks; }

    void initThreadPool()
    {
        uint32_t nThreads = std::thread::hardware_concurrency() - 1;
        m_threadPoolWithTasks.reset( nThreads );
    }

    typedef std::function<void(Sound*)> FN_LOAD_SOUND_CALLBACK;
    typedef std::function<void(Sound*)> FN_SAVE_SOUND_CALLBACK;

    static void
    joinAsync() { get()->getThreadPool().wait_for_tasks(); }

    static void
    loadAsync( std::string uri,
               const LoadOptions& options,
               const FN_LOAD_SOUND_CALLBACK& onLoadFinished )
    {

        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                auto snd = new Sound();
                if (get()->loadSound(*snd,uri))
                {
                    onLoadFinished(snd);
                }
                else
                {
                    delete snd;
                }
            }
        );

        //g_threadPool.wait_for_tasks();
    }

    static void
    saveAsync(Sound* snd, std::string uri,
              const SaveOptions& options,
              const FN_SAVE_SOUND_CALLBACK& onSaveFinished )
    {
        if (!snd)
        {
            return;
        }

        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                if (get()->saveImage(*snd,uri))
                {
                    onSaveFinished(snd); // User must delete images
                }
                else
                {
                    delete snd;
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
    //   Load sound
    // =====================
    bool
    loadSound(Sound & snd, std::string uri, SoundLoadOptions const & options = SoundLoadOptions() ) const;

    bool
    loadSound(Sound & snd, uint8_t const* data, uint64_t bytes, std::string uri, SoundLoadOptions const & options ) const;

    // =====================
    //   Save sound
    // =====================

    bool
    saveSound(const Sound & snd, std::string uri, SoundSaveOptions options) const;

    // =====================
    //   Get reader/writer
    // =====================

    ISoundReader*
    getReaderFromExtension( const std::string& suffix ) const;

    ISoundWriter*
    getWriterFromExtension( const std::string& suffix ) const;


    /*
    // =====================
    //   Convert images
    // =====================
    bool
    convertFile( std::string loadName, std::string saveName ) const;
    */
};

} // end namespace de.

/*

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

*/