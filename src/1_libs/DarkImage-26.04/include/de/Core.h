#pragma once
#include <cstdint>
#include <sstream>
#include <thread>
#include <cassert>
#include <vector>
#include <de_glm.hpp>

#include <cstdlib>

// Comment this #define out to disable logging,
// will replace all macros with empty implementations.
// Logging cant really be a source of bugs, atleast not the current well tested version.
// Disabling can speed things up, though its already fast because i use printf and not std::cout.
// TODO: Convert to new std::format|std::print?!
#ifndef USE_DE_LOGGING
#define USE_DE_LOGGING
#endif

#ifndef DE_FORCE_INLINE
#define DE_FORCE_INLINE inline __attribute__((__always_inline__))
#endif

DE_FORCE_INLINE void* de_aligned_malloc( size_t n, size_t alignBytes )
{
   return _aligned_malloc( n, alignBytes );
}

DE_FORCE_INLINE void de_aligned_free( void* ptr )
{
   if (!ptr) return;
   _aligned_free( ptr );
}

DE_FORCE_INLINE void* de_aligned_realloc( void* ptr, size_t n, size_t alignBytes )
{
   return _aligned_realloc( ptr, n, alignBytes );
}

inline void
better_runtime_error_impl( std::thread::id threadId,
                  std::string file,
                  int line,
                  std::string func,
                  std::string func_args,
                  std::string log_msg )
{
   std::ostringstream s;
   s << file << ":" << line << "\n" << func << "(" << func_args << ") :: " << log_msg;
   throw std::runtime_error( s.str() );
}

// The macro 'DE_RUNTIME_ERROR'
// auto adds preprocessor infos like func,line-number and file in the source-code
// This tremendously enhances informativity of runtime_error. The user gives 2 args.
// @param args A string being a list of params ( that failed or where involved in error )
// @param msg A string being a specific log_message like 'Wrong index' or 'No such file'.

// Usage: in SceneManager.cpp:
//
//          if ( !dbExistFile( uri ) )
//          {
//             DE_RUNTIME_ERROR( uri, "No such mesh file")
//          }
//
//          -> will throw an exception with some informative text
//
// Output example:
//
//    terminate called after throwing an instance of 'std::runtime_error'
//    what():  G:/Projects/AudioSynth/src/lib/libDarkGPU-23.04/src/de/gpu/scene/SceneManager.cpp:211
//    getMesh(../../media/EmpireEarth23/bla/building_garnitur.stl) :: No such mesh file

//
#ifndef DE_RUNTIME_ERROR
#define DE_RUNTIME_ERROR(args,msg) { ::better_runtime_error_impl( std::this_thread::get_id(), __FILE__, __LINE__, __func__, (args), (msg) ); }
#endif

// ===========================================================================
// DarkTimer (uses std::chrono)
// ===========================================================================

int64_t dbTimeInNanoseconds();
int64_t dbTimeInMicroseconds();
int32_t dbTimeInMilliseconds();
double  dbTimeInSeconds();

// ===========================================================================
// DarkRandom
// ===========================================================================
void    dbRandomize();
int32_t dbRND();

/// Since ANSI standard exists only 50yrs (1970!) its to few time for MS to implement it.
/// cmd.exe is not an ANSI console. AND
/// cmd.exe is nothing i would use and call myself a professional at the same time.
/// On Windows64 use the ANSI terminal in git4windows setup (based on MSYS2) or ConEmu64.
/// On Linux just open a terminal and be happy.
/// On Mac good luck, but i heard its a Linux ripoff
/// On Android good luck, but i heard its a Linux ripoff aswell.

/// @brief Write ANSI terminal/console color reset marker.
inline std::string
dbResetTerminalColors() { return "\033[0m"; }

/// @brief Write ANSI terminal/console color RGB marker. Foreground + Background colors.
inline std::string
dbSetTerminalColors( uint8_t fr, uint8_t fg, uint8_t fb,
                     uint8_t br, uint8_t bg, uint8_t bb )
{
   // The (int) casts are necessary to print decimals and not secret control message hex bytes.
   std::ostringstream o; o <<
   "\033[38;2;" << int(fr) << ";" << int(fg) << ";" << int(fb) << "m"
   "\033[48;2;" << int(br) << ";" << int(bg) << ";" << int(bb) << "m";
   return o.str();
}

// ===========================================================================
// ===   LogMacros
// ===========================================================================

template< typename ... T >
std::string dbStr( T const & ... t )
{
   std::ostringstream s;
   (void)std::initializer_list<int>{ (s<<t, 0)... };
   return s.str();
}

template< typename ... T >
std::wstring dbStrW( T const & ... t )
{
    std::wostringstream s;
    (void)std::initializer_list<int>{ (s<<t, 0)... };
    return s.str();
}

void
dbLogMessage(  int logLevel, // 0=Trace, 1=Debug, 2=Ok, 3=Benni, 4=Info, 5=Warn, 6=Error, 7=Fatal
               const std::string& msg,
               const std::string& file, // = "DeineMutter.cpp",
               int line, // = 666,
               const std::string& func, // = "knatscheln",
               std::thread::id threadId = std::this_thread::get_id() );

// Legacy -> nop
#ifndef DE_CREATE_LOGGER
#define DE_CREATE_LOGGER(x)
#endif

#ifndef DE_OK
#define DE_OK(...) {  dbLogMessage(  de::LogLevel::Ok, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_BENNI
#define DE_BENNI(...) {  dbLogMessage(  de::LogLevel::Benni, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_INFO
#define DE_INFO(...) {  dbLogMessage(  de::LogLevel::Info, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_WARN
#define DE_WARN(...) {  dbLogMessage(  de::LogLevel::Warn, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_ERROR
#define DE_ERROR(...) {  dbLogMessage(  de::LogLevel::Error, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_FATAL
#define DE_FATAL(...) {  dbLogMessage(  de::LogLevel::Fatal, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_DEBUG
#define DE_DEBUG(...) {  dbLogMessage(  de::LogLevel::Debug, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif
#ifndef DE_TRACE
#define DE_TRACE(...) {  dbLogMessage(  de::LogLevel::Trace, \
                                       de::StringUtil::join( __VA_ARGS__ ), \
                                       __FILE__, __LINE__, __func__, \
                                       std::this_thread::get_id() ); }
#endif

// PerfMarker
#ifndef DE_PERF_MARKER
#define DE_PERF_MARKER volatile de::PerfMarker perfMarker(__FILE__,__func__,__LINE__);
#endif

// #ifndef DE_SRC_F32
// #define DE_SRC_F32 const float* __restrict__
// #endif

// #ifndef DE_DST_F32
// #define DE_DST_F32 float* __restrict__
// #endif

#ifndef DE_ABORT
    #if defined(_MSC_VER)
        #define DE_ABORT __debugbreak();
    #else
        #define DE_ABORT __builtin_trap();
    #endif
#endif

/*  DE_ASSUME

    T* const* __restrict__ src = orig.data();
    T** __restrict__ dst = temp.data() + 1;

    DE_ASSUME_NO_OVERLAP_ELEMS(dst, src, n - 1);

    std::memcpy(dst, src, sizeof(T*) * (n - 1));
*/

#ifndef DE_ASSUME
    #if defined(__clang__)
        #define DE_ASSUME(expr) __builtin_assume(expr)
    #elif defined(_MSC_VER)
        #define DE_ASSUME(expr) __assume(expr)
    #elif defined(__GNUC__)
        #define DE_ASSUME(expr) if (!(expr)) __builtin_unreachable()
    #else
        #define DE_ASSUME(expr) ((void)0)
    #endif

    #ifndef DE_ASSUME_NO_OVERLAP
    #define DE_ASSUME_NO_OVERLAP(dst, src, bytes) \
        DE_ASSUME( \
            (uintptr_t)(dst) + (uintptr_t)(bytes) <= (uintptr_t)(src) ||  \
            (uintptr_t)(src) + (uintptr_t)(bytes) <= (uintptr_t)(dst))
    #endif

    #ifndef DE_ASSUME_NO_OVERLAP_ELEMS
    #define DE_ASSUME_NO_OVERLAP_ELEMS(dst, src, elems) \
        DE_ASSUME( \
            (uintptr_t)(dst) + (uintptr_t)(elems) * sizeof(*(dst)) <= (uintptr_t)(src) || \
            (uintptr_t)(src) + (uintptr_t)(elems) * sizeof(*(src)) <= (uintptr_t)(dst))
    #endif

    #ifndef DE_ASSUME_POINTER_AVX
    #define DE_ASSUME_POINTER_AVX(p) DE_ASSUME(((uintptr_t)(p) & 15) == 0);
    #endif

    #ifndef DE_ASSUME_POINTER_AVX2
    #define DE_ASSUME_POINTER_AVX2(p) DE_ASSUME(((uintptr_t)(p) & 31) == 0);
    #endif

    #ifndef DE_ASSUME_POINTER_AVX512
    #define DE_ASSUME_POINTER_AVX512(p) DE_ASSUME(((uintptr_t)(p) & 63) == 0);
    #endif
#endif

void de_memcpy_no_overlap(void* __restrict__ dst,
                          const void* __restrict__ src,
                          uint64_t bytes);

void de_memcpy_no_overlap_avx2(void* __restrict__ dst,
                          const void* __restrict__ src,
                          uint64_t bytes);

namespace de
{

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef long double f80;




// ===========================================================================
struct PerfMarker
// ===========================================================================
{
    int64_t timeStart;
    int64_t timeEnd;
    std::string marker;

    PerfMarker();
    PerfMarker(std::string marker);
    PerfMarker(std::string file, std::string func, int64_t line);
    ~PerfMarker();
};

// ===========================================================================
struct LogLevel
// ===========================================================================
{
    enum ELevel { None = 0, Trace, Debug, Ok, Benni, Info, Warn, Error, Fatal, Count };
    ELevel m_Level = None;
    LogLevel( ELevel logLevel ) : m_Level(logLevel) {}
    operator uint32_t() const { return m_Level; }
    void setLevel( ELevel logLevel ) { m_Level = logLevel; }
    ELevel getLevel() const { return m_Level; }
};

// =======================================================================
struct StringUtil
// =======================================================================
{
    static std::string
    to_str(const std::wstring& txt );

    static std::string
    to_str( wchar_t const wc );

    static std::wstring
    to_wstr(const std::string& utf8);

    static char
    hexLowNibble( uint8_t byte );

    static char
    hexHighNibble( uint8_t byte );

    static std::string
    hex( uint8_t byte );

    static std::string
    hex( uint16_t const color );

    static std::string
    hex( uint32_t const color );

    static std::string
    hex( uint64_t color );

    static std::string
    hex( uint8_t const* beg, uint8_t const* end );

    static std::string
    hex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow );

    template< typename ... T >
    static std::string
    join( T const & ... t )
    {
        std::ostringstream s;
        (void)std::initializer_list<int>{ (s<<t, 0)... };
        return s.str();
    }

    template< typename ... T >
    static std::wstring
    joinW( T const & ... t )
    {
        std::wostringstream s;
        (void)std::initializer_list<int>{ (s<<t, 0)... };
        return s.str();
    }

    static std::string
    nanoseconds( double nNanoSeconds );

    static std::string
    seconds( double nSeconds );

    static std::string
    bytes( uint64_t nBytes );

    static std::string
    replace( const std::string& txt, const std::string& from,
             const std::string& to, size_t* nReplacements = nullptr );

    static std::wstring
    replace( const std::wstring& txt, const std::wstring& from,
             const std::wstring& to, size_t* nReplacements = nullptr );

    static std::vector< std::string >
    split( const std::string& txt, char searchChar, bool bKeepEmptyLines = false );

    static std::string
    makeLower( const std::string & txt, const std::locale & loc = std::locale() );

    static std::string
    makeUpper( const std::string & txt, const std::locale & loc = std::locale() );

    // static void
    // lowerCase( std::string & out, const std::locale & loc = std::locale() );

    // static void
    // upperCase( std::string & out, const std::locale & loc = std::locale() );

    static void
    lowerCase(std::string& txt, const std::locale& loc = std::locale());

    static void
    upperCase(std::string& txt, const std::locale& loc = std::locale());

    static void
    lowerCase(std::wstring& txt);

    static void
    upperCase(std::wstring& txt);

    static bool
    startsWith( const std::string& str, const std::string& query );

    static bool
    startsWith( const std::wstring& str, const std::wstring& query );

    static bool
    startsWith( const std::string& str, char c );

    static bool
    startsWith( const std::wstring& str, wchar_t c );

    static bool
    endsWith( const std::string& str, const std::string& query );

    static bool
    endsWith( const std::wstring& str, const std::wstring& query );

    static bool
    endsWith( const std::string& str, char c );

    static bool
    endsWith( const std::wstring& str, wchar_t c );

    static std::string
    joinVector( std::vector< std::string > const & v, const std::string& prefix );

    static std::string
    trim( const std::string& txt, const std::string& filter );

    static std::string
    trimLeft( const std::string& txt, const std::string& filter );

    static std::string
    trimRight( const std::string& original, const std::string& filter );

    static std::string
    prefixLineNumbers( const std::string& src );

    static std::string
    file2header( uint8_t const* pBytes, size_t nBytes, std::string dataName );
};


/*
📌 eFileMode::Append means:

    Writes always go to the end of the file, regardless of the current file offset.
    The OS forces the write pointer to EOF on every write() call.
    Seeking does not change where writes go.

    POSIX   O_APPEND
    Windows _O_APPEND

    write(fd, ...) → always appends
    lseek(fd, 0, SEEK_SET) → moves read pointer, but writes still append
    Safe for multi‑process logging (atomic append)

    Use cases:  Logs, Journals,
                any file where you never overwrite existing data.

📌 eFileMode::Create means:

    If the file does not exist → create it.
    If the file does exist → do nothing (unless combined with Truncate).

    POSIX   O_CREAT
    Windows _O_CREAT

    Requires a mode argument on POSIX (permissions)
    Does not modify existing files
    Often combined with O_EXCL to prevent overwriting

    Use cases:  Creating new files,
                Ensuring a file exists before writing
                Safe creation with O_CREAT | O_EXCL

📌 eFileMode::Truncate means:

    If the file exists → its size becomes 0 immediately.
    If the file does not exist → ignored unless combined with Create.

    POSIX   O_TRUNC
    Windows _O_TRUNC

    Requires write permission (O_WRONLY or O_RDWR)
    Clears all existing content
    File pointer starts at offset 0

    Use cases:  Overwriting a file from scratch
                Resetting a log
                Rewriting configuration files


@param permissionBits = Control access, for create file only.

    Windows uses POSIX‑style permission bits:
        0400 — owner read
        0200 — owner write
        0100 — owner execute
        0040 — group read
        0020 — group write
        0010 — group execute
        0004 — others read
        0002 — others write
        0001 — others execute

    Typical values:
        0644 — rw‑r‑r (common for data files)
        0600 — rw‑‑‑‑ (private files)
        0666 — rw‑rw‑rw (rarely recommended)

*/

enum class eFileMode // Always binary, there is no text mode.
{
    Read      = 1,
    Write     = 2,
    Append    = 4,
    Create    = 8,
    Truncate  = 16,
    ReadWrite = Read | Write
};

enum class eSeekMode
{
    Set = 0,
    Cur = 1,
    End = 2
};

int32_t file64_open(const char* path, eFileMode fileMode, int32_t permission = 0);
int32_t file64_close(int32_t fd);
int32_t file64_read(int32_t fd, void* buf, int64_t bytes);
int32_t file64_write(int32_t fd, const void* buf, int64_t bytes);
int64_t file64_seek(int32_t fd, int64_t offset, eSeekMode seekMode);
int64_t file64_tell(int32_t fd);

// =======================================================================
struct File
// =======================================================================
{
    int m_fd; // m_fileDescriptor;

    File();
    ~File();

    File(const std::string& utf8_uri, eFileMode fm = eFileMode::Read,
         int permissionBits = 0);

    bool
    open(const std::string& utf8_uri, eFileMode fm = eFileMode::Read,
         int permissionBits = 0);

    void close();
    bool is_open() const;

    int64_t size() const;
    int64_t tell() const;

    // Read/Write are limited to 2GB-1 for ancient reasons.
    // The return values are int (ancient).
    //  - Negative return values indicate errors.
    //  0 Zero return value means EOF.
    //  + Positive return value means bytes written/read.
    // We like to know exactly how many bytes are read/written,
    // and we can only use positive half of int return value, ergo 2GB-1 at max.
    int32_t write(const void* __restrict__ src, int64_t nBytes ) const;
    int32_t read(void* __restrict__ dst, int64_t nBytes ) const;

    // @param offset
    // @param seekMode
    // eSeekMode::Set — Absolute from beginning, should be positive.
    // eSeekMode::Cur — from current position, can be positive or negative.
    // eSeekMode::End — from end of file, should be negative for read-only.
    // Should never point before file beginning.
    // @return New absolute position or error.
    //  >= 0 → new absolute file position
    //  -1 → error
    int64_t seek(int64_t offset, eSeekMode seekMode = eSeekMode::Set) const;

    int32_t read_u8( u8* out ) const;
    int32_t read_u16_be( uint16_t* out ) const; // Big endian
    int32_t read_s16_be(  int16_t* out ) const; // Big endian
    int32_t read_u24_be( uint32_t* out ) const; // Big endian
    int32_t read_u32_be( uint32_t* out ) const; // Big endian
    int32_t read_s32_be(  int32_t* out ) const; // Big endian
    int32_t read_u64_be( uint64_t* out ) const; // Big endian
    int32_t read_char4( char buf[4] ) const;
};

typedef std::vector<uint8_t> Blob;

// =======================================================================
struct FileSystem
// =======================================================================
{
    static std::wstring
    loadStrW( const std::wstring& uri );

    static std::wstring
    loadStrW( const std::string& uri )
    {
        return loadStrW( StringUtil::to_wstr(uri));
    }

    static std::string
    loadStr( const std::string& uri );

    static std::string
    loadStr( const std::wstring& uri ) { return loadStr( StringUtil::to_str(uri) ); }

    static bool
    saveStr( const std::string& uri, const std::string& txt );

    static bool
    saveStr( const std::wstring& uri, const std::string& txt )
    {
        return saveStr( StringUtil::to_str(uri), txt);
    }

    static bool
    saveStr( const std::wstring& uri, const std::wstring& txt );

    // 1GB RAM limit (2^30) for a midi file, else regarded as error/malformed/broken.
    /*
    static bool
    loadByteVector( std::vector< uint8_t > & bv, const std::string& uri,
        uint64_t byteLimit = (uint64_t(1) << 30) );
    */
    static Blob
    loadBlob( const std::string& uri );

    static bool
    loadBlob( Blob& blob, const std::string& uri );

    static bool
    saveBlob( const Blob& blob, const std::string& uri );

    static bool
    loadBin( const std::string& uri, Blob& blob );

    static bool
    saveBin( const std::string& uri, const Blob& blob );

    static bool
    existFile(const std::string &uri );

    static bool
    existFile(const std::wstring &uri );

    static bool
    existDirectory( const std::string& uri );

    static bool
    existDirectory( const std::wstring& uri );

    static int64_t
    fileSize( const std::string & uri );

    static int64_t
    fileSize( const std::wstring & uri );

    static std::string
    fileName( const std::string& uri, const std::string& relativeToPath = "" );

    static std::wstring
    fileName( const std::wstring& uri, const std::wstring& relativeToPath = L"" );

    static std::string
    fileBase( const std::string& uri );

    static std::wstring
    fileBase( const std::wstring& uri );

    static std::string
    fileSuffix( const std::string& uri );

    static std::wstring
    fileSuffix( const std::wstring& uri );

    static std::string
    fileDir( const std::string& uri );

    static std::wstring
    fileDir( const std::wstring& uri );

    static std::string
    parentDir( const std::string& uri );

    static std::wstring
    parentDir( const std::wstring& uri );

    static std::string
    makeAbsolute( std::string uri, std::string baseDir = "" );

    static std::wstring
    makeAbsolute( std::wstring uri, std::wstring baseDir = L"" );

    static std::string
    makeWinPath( const std::string & uri );

    static std::wstring
    makeWinPath( const std::wstring & uri );

    static std::string
    makePosixPath( const std::string & uri );

    static std::wstring
    makePosixPath( const std::wstring & uri );

    static void
    createDirectory( const std::string& uri );

    static void
    createDirectory( const std::wstring& uri );

    static void
    removeFile( const std::string& uri );

    static bool
    copyFile( std::string src, std::string dst );

    static std::string
    createUniqueFileName( const std::string& userPrefix = "untitled_file_" );

    static bool
    isAbsolute( const std::string & uri );

    static bool
    isAbsolute( const std::wstring & uri );

    static bool
    entries(std::string baseDir,
            bool recursive,
            bool withFiles,
            bool withDirs,
            const std::function< void( const std::string & ) > & onFileName );

    static bool
    entries(std::wstring baseDir,
            bool recursive,
            bool withFiles,
            bool withDirs,
            const std::function< void( const std::wstring & ) > & onFileName );

    static std::vector<std::string>
    entries(std::string baseDir,
            bool recursive,
            bool withFiles,
            bool withDirs);

    static std::vector<std::wstring>
    entries(std::wstring baseDir,
            bool recursive,
            bool withFiles,
            bool withDirs);
};

//===================================================================
struct SteadyClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct HighResolutionClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct PerformanceTimer
//===================================================================
{
   PerformanceTimer();

   void start();
   void stop();

   int64_t ns() const;  // Nanoseconds
   int64_t us() const;  // Microseconds
   int32_t ms() const;  // Milliseconds
   double sec() const;  // Seconds


   int64_t m_timeStart; // In Nanoseconds
   int64_t m_timeStop; // In Nanoseconds
   bool m_isStarted = false;
};


enum class SeekMode
{
   SET = 0,
   CUR = 1,
   END = 2,
   Count = 3,
   Default = SeekMode::SET
};

//
// Load any file fully to RAM, then fast seek and rewind on it.
//
// ============================================================================
struct Binary
// ============================================================================
{
    //std::vector< uint8_t > m_Blob;
    const uint8_t* m_ptr;
    uint64_t m_size;
    uint64_t m_index;
    uint64_t m_avail;
    std::string m_uri;

    Binary()
        : m_ptr(nullptr)
        , m_size(0)
        , m_index(0)
        , m_avail(0)
    {}

    Binary( uint8_t const * p, uint64_t n, std::string uri )
        : m_ptr(p)
        , m_size(n)
        , m_index(0)
        , m_avail(n)
        , m_uri(uri)
    {}

    bool open( uint8_t const * p, uint64_t n, std::string uri )
    {
        m_ptr = p;
        m_size = n;
        m_avail = n;
        m_index = 0;
        m_uri = uri;
        return true;
    }

    bool is_open() const
    {
        return m_ptr && m_size > 0;
    }

    void close()
    {
        m_ptr = nullptr;
        m_size = 0;
        m_avail = 0;
        m_index = 0;
    }

    uint64_t read( void* dst, uint64_t n )
    {
        if ( !dst ) { DE_DEBUG("No dst") return 0; }
        if ( !is_open() ) { DE_DEBUG("Not open") return 0; }

        // Clamp:
        if ( n > m_avail ) { n = m_avail; }

        // Copy to dst:
        ::memcpy( dst, m_ptr + m_index, n );
        m_index += n;
        m_avail -= n;
        //DE_DEBUG("CHUNK(",n,") ",m_index," of ",m_avail,".")
        return n;
   }

    void rewind() { m_index = 0; m_avail = m_size; }

    // File and memory stuff (RAM)
    const std::string&   uri() const { return m_uri; }

    uint64_t             tell() const { return m_index; }
    uint64_t             size() const { return m_size; }
    uint8_t const*       data() const { return m_ptr; }
    //uint8_t*           data() { return m_ptr; }

    // FileStream stuff

    bool seek( uint64_t o, int dir = SEEK_SET )
    {
        if ( !is_open() ) { return false; }

        if ( o >= m_size ) // byteOffset o must be in range[0,m_Count-1]
        {
            DE_DEBUG("Invalid offset(",o,"), size(",m_size,")")
            return false;
        }

        if ( dir == SEEK_SET )
        {
            m_index = o;
            m_avail = m_size - m_index;
            return true;
        }
        else if ( dir == SEEK_END )
        {
            m_index = m_size - o;
            m_avail = o;
            return true;
        }
        else if ( dir == SEEK_CUR )
        {
            m_index += o;
            m_avail -= o;
            return true;
        }

        DE_DEBUG("Invalid SEEK MODE ", dir )
        return false;
    }
    // ICO file reader
    uint8_t     readU8();
    uint16_t    readU16_lsb();
    uint32_t    readU32_lsb();
    uint16_t    readU16_msb();
    uint32_t    readU32_msb();

    bool save( const std::string& uri ) const;
};

//===================================================================
struct FileMagic
//===================================================================
{
    enum EFileMagic
    {
        Unknown = 0,

        XPM, JPG, PNG, WEBP, BMP, GIF, TGA, DDS, TIF, PCX,

        WAV, AAC, MP3, MP4, M4A, FLAC, OGG, WMA, FLV
    };

    static EFileMagic
    getFileMagicFromFile(const std::string& uri);

    static EFileMagic
    getFileMagic( void const* ptr )
    {
        if ( !ptr ) return Unknown;

        if      (isJPG( ptr )) return JPG;
        else if (isPNG( ptr )) return PNG;
        else if (isWEBP( ptr )) return WEBP;
        else if (isBMP( ptr )) return BMP;
        else if (isGIF( ptr )) return GIF;
        else if (isTIF( ptr )) return TIF;
        else if (isTGA( ptr )) return TGA;
        else if (isPCX( ptr )) return PCX;

        else if (isWAV( ptr )) return WAV;
        else if (isAAC( ptr )) return AAC;
        else if (isMP3( ptr )) return MP3;
        else if (isMP4( ptr )) return MP4;
        else if (isM4A( ptr )) return M4A;
        else if (isFLAC( ptr ))return FLAC;
        else if (isOGG( ptr )) return OGG;
        else if (isWMA( ptr )) return WMA;
        else if (isFLV( ptr )) return FLV;

        else return Unknown;
    }

    static std::string
    getString( EFileMagic const fileMagic )
    {
        switch (fileMagic)
        {
        case XPM: return "xpm";
        case JPG: return "jpg";
        case PNG: return "png";
        case WEBP: return "webp";
        case BMP: return "bmp";
        case GIF: return "gif";
        case TIF: return "tif";
        case TGA: return "tga";
        case PCX: return "pcx";

        case WAV: return "wav";
        case AAC: return "aac";
        case MP3: return "mp3";
        case MP4: return "mp4";
        case M4A: return "m4a";
        case FLAC:return "flac";
        case OGG: return "ogg";
        case WMA: return "wma";
        case FLV: return "flv";

        default: return "";
        }
    }

   // ============================================================================
   /// ImageMagic
   // ============================================================================
   static bool isJPG( void const* ptr );
   static bool isPNG( void const* ptr );
   static bool isWEBP( void const* ptr );
   static bool isBMP( void const* ptr );
   static bool isGIF( void const* ptr );
   static bool isTIF( void const* ptr );
   static bool isTGA( void const* ptr );
   static bool isPCX( void const* ptr );

   // ============================================================================
   /// AudioMagic
   // ============================================================================
   static bool isAAC( void const* ptr );
   static bool isWAV( void const* ptr );
   static bool isMP3( void const* ptr );
   static bool isMP4( void const* ptr );
   static bool isFLAC( void const* ptr );
   static bool isOGG( void const* ptr );
   static bool isWMA( void const* ptr );
   static bool isM4A( void const* ptr );
   static bool isFLV( void const* ptr );
};

// ===========================================================================
// SM2 - H1 - History1 = StateMachine2 : Holds 2 states ( curr, last )
// ===========================================================================
template < typename T >
struct SM2
{
    T curr, last;
    SM2() { curr = last = T(0); }
    SM2( T const & t ) { curr = last = t; }
    SM2& operator= ( T const & t ) { curr = last = t; return *this; }
};

// ===========================================================================
// SM3 - H2 - History2 = StateMachine3 : Holds 3 states ( curr, last, start )
// ===========================================================================
template < typename T >
struct SM3
{
    T curr;
    T last;
    T init;
    SM3() { init = curr = last = T{}; }
    SM3( T const & t ) { init = curr = last = t; }
    SM3& operator= ( T const & t ) { init = curr = last = t; return *this; }
};

struct Rectf;

// ===========================================================================
struct Posi
// ===========================================================================
{
    typedef int32_t T;
    constexpr static T const s_Min = std::numeric_limits< T >::lowest();
    constexpr static T const s_Max = std::numeric_limits< T >::max();

    T x;
    T y;

    Posi() : x(0), y(0) {}
    Posi( T ox, T oy ) : x(ox), y(oy) {}
    Posi( const Posi & o ) : x(o.x), y(o.y) {}

    void reset() { x = y = 0; }
    void set( T ox, T oy ) { x = ox; y = oy; }
    void setX( T x_ ) { x = x_; }
    void setY( T y_ ) { y = y_; }
    //glm::ivec2 v2i() const { return { x, y }; }
    //glm::vec2 v2f() const { return glm::vec2( x, y ); }
    std::string str() const { std::ostringstream o; o << x << "," << y; return o.str(); }
    Posi operator+( const Posi & o ) const { return Posi(x + o.x, y + o.y); }
    Posi operator-( const Posi & o ) const { return Posi(x - o.x, y - o.y); }
    Posi& operator=( const Posi & o ) { x = o.x; y = o.y; return *this; }
    bool operator==( const Posi & o ) const { return x == o.x && y == o.y; }
    bool operator!=( const Posi & other ) const { return !( other == *this ); }

    void test()
    {
        // DE_DEBUG("typename(int32_t) = ", typename(int32_t) )
        DE_DEBUG("s_Min = ", s_Min )
        DE_DEBUG("s_Max = ", s_Max )

        Posi a(10,10);
        Posi b(80,50);
        Posi c(43,33);
        DE_DEBUG("a + b = ", (a+b).str() )
        DE_DEBUG("b + c = ", (b+c).str() )
    }
};

// ===========================================================================
struct Sizei
// ===========================================================================
{
    typedef int32_t T;
    constexpr static T const s_Min = std::numeric_limits< T >::lowest();
    constexpr static T const s_Max = std::numeric_limits< T >::max();

    T w;
    T h;

    Sizei() : w(0), h(0) {}
    Sizei( T ox, T oy ) : w(ox), h(oy) {}
    Sizei( const Sizei & o ) : w(o.w), h(o.h) {}

    void reset() { w = h = 0; }
    void set( T w_, T h_ ) { w = w_; h = h_; }
    void setW( T w_ ) { w = w_; }
    void setH( T h_ ) { h = h_; }
    //glm::ivec2 v2i() const { return { x, y }; }
    //glm::vec2 v2f() const { return glm::vec2( x, y ); }
    std::string str() const { std::ostringstream o; o << w << "," << h; return o.str(); }

    Sizei operator+( const Sizei & o ) const { return Sizei(w + o.w, h + o.h); }
    Sizei operator-( const Sizei & o ) const { return Sizei(w - o.w, h - o.h); }
    Sizei& operator=( const Sizei & o ) { w = o.w; h = o.h; return *this; }
    bool operator==( const Sizei & o ) const { return w == o.w && h == o.h; }
    bool operator!=( const Sizei & other ) const { return !( other == *this ); }

    void test()
    {
        // DE_DEBUG("typename(int32_t) = ", typename(int32_t) )
        DE_DEBUG("s_Min = ", s_Min )
        DE_DEBUG("s_Max = ", s_Max )

        Sizei a(10,10);
        Sizei b(80,50);
        Sizei c(43,33);
        DE_DEBUG("a + b = ", (a+b).str() )
        DE_DEBUG("b + c = ", (b+c).str() )
    }
};

// ===========================================================================
struct Recti
// ===========================================================================
{
    constexpr static int32_t const s_Min = std::numeric_limits< int32_t >::lowest();
    constexpr static int32_t const s_Max = std::numeric_limits< int32_t >::max();

    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;

    Recti( int dummy = 0 );
    Recti( int32_t x_, int32_t y_, int32_t w_, int32_t h_ );
    Recti( const Posi& pos, const Sizei& size );
    Recti( const Recti& r );

    Recti& operator=(const Recti &o );
    bool operator==( const Recti &o ) const;
    bool operator!=( const Recti &o ) const;
    Recti operator+( const Posi & o ) const { return Recti( x+o.x, y+o.y, w, h ); }
    Recti operator-( const Posi & o ) const { return Recti( x-o.x, y-o.y, w, h ); }

    void reset();
    void setWidth( int32_t w_ );
    void setHeight( int32_t h_ );
    void set( int32_t x_, int32_t y_, int32_t w_, int32_t h_ );
    static Recti fromPoints( int32_t x1_, int32_t y1_, int32_t x2_, int32_t y2_ );
    void addInternalPoint( glm::ivec2 const & p );
    void addInternalPoint( int32_t x_, int32_t y_ );

    inline bool isPointInside( int mx, int my ) const
    {
        int x1_ = x1();
        int y1_ = y1();
        int x2_ = x2();
        int y2_ = y2();
        if ( x1_ > x2_ ) std::swap( x1_, x2_ );
        if ( y1_ > y2_ ) std::swap( y1_, y2_ );
        if ( mx < x1_ ) return false;
        if ( my < y1_ ) return false;
        if ( mx > x2_ ) return false;
        if ( my > y2_ ) return false;
        return true;
    }

    int32_t centerX() const;
    int32_t centerY() const;
    int32_t x1() const;
    int32_t y1() const;
    int32_t x2() const;
    int32_t y2() const;
    Posi pos() const;
    Sizei size() const;
    Posi center() const;
    Posi topLeft() const;
    Posi bottomRight() const;

    std::string str() const;

    //   Recti operator+ ( int32_t v ) const { return Recti( *this ) += v; }
    //   Recti operator- ( int32_t v ) const { return Recti( *this ) -= v; }
    //   Recti operator* ( int32_t v ) const { return Recti( *this ) *= v; }
    //   Recti operator/ ( int32_t v ) const { return Recti( *this ) /= v; }

    // Translate x,y. (w,h) stay untouched.
    //   Recti& operator+= ( glm::ivec2 const & p ) { x += p.x; y += p.y; return *this; }
    //   Recti& operator-= ( glm::ivec2 const & p ) { x -= p.x; y -= p.y; return *this; }
    //   Recti& operator+= ( int32_t v ) { x += v; y1 += v; x2 += v; y2 += v; return *this; }
    //   Recti& operator-= ( int32_t v ) { x -= v; y1 -= v; x2 -= v; y2 -= v; return *this; }
    //   Recti& operator*= ( int32_t v ) { x *= v; y *= v; x2 *= v; y2 *= v; return *this; }
    //   Recti& operator/= ( int32_t v ) { x /= v; y /= v; x2 /= v; y2 /= v; return *this; }

    bool contains( int mx, int my, int touchWidth = 0 ) const;
    glm::vec4 toVec4( int atlas_w, int atlas_h ) const;

    static void test();

};

//=============================================================================
struct Rectf
//=============================================================================
{
    //DE_CREATE_LOGGER("de.Rectf")

    Rectf();
    Rectf( float x, float y, float w, float h );
    Rectf( Rectf const & other );

    static float computeU1( int32_t x1, int32_t w, bool useOffset = true );
    static float computeV1( int32_t y1, int32_t h, bool useOffset = true );
    static float computeU2( int32_t x2, int32_t w, bool useOffset = true );
    static float computeV2( int32_t y2, int32_t h, bool useOffset = true );

    static Rectf
    fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset = true );

    std::string toString() const;
    void zero();

    static Rectf identity();

    float x() const;
    float y() const;
    float w() const;
    float h() const;
    float x1() const;
    float y1() const;
    float x2() const;
    float y2() const;
    float centerX() const;
    float centerY() const;

    glm::tvec2< float > center() const;
    glm::tvec2< float > topLeft() const;
    glm::tvec2< float > bottomRight() const;

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    float getX1() const;
    float getY1() const;
    float getX2() const;
    float getY2() const;
    float getCenterX() const;
    float getCenterY() const;
    glm::tvec2< float > getCenter() const;
    glm::tvec2< float > getTopLeft() const;
    glm::tvec2< float > getBottomRight() const;

    Recti toRecti( int image_w = 1, int image_h = 1 ) const;
    void addInternalPoint( glm::tvec2< float > const & point );
    void addInternalPoint( float x, float y );

    float u1() const;
    float v1() const;
    float u2() const;
    float v2() const;
    float du() const;
    float dv() const;

    float getU1() const;
    float getV1() const;
    float getU2() const;
    float getV2() const;
    float getDU() const;
    float getDV() const;

    float m_x;
    float m_y;
    float m_w;
    float m_h;
};

struct BBox1f
{
    float m_min;
    float m_max;

    BBox1f() : m_min(0.f), m_max(0.f)
    {}
    BBox1f(float min, float max) : m_min(min), m_max(max)
    {}

};

struct BBox3f
{
    typedef float T;
    typedef glm::vec3 V3;
    V3 m_min;
    V3 m_max;

    BBox3f() : m_min(0.f,0.f,0.f), m_max(0.f,0.f,0.f)
    {}
    BBox3f(T dx, T dy, T dz) : m_min(-dx,-dy,-dz), m_max(dx,dy,dz)
    {}
    BBox3f(T x1, T y1, T z1, T x2, T y2, T z2) : m_min(x1,y1,z1), m_max(x2,y2,z2)
    {}
    BBox3f(V3 a_min, V3 a_max) : m_min(a_min), m_max(a_max)
    {}
};

// =======================================================================
struct Align
// =======================================================================
{
    uint8_t m_align;

    Align() : m_align(Default) {}
    Align( uint8_t align ) : m_align(align) {}
    Align( Align const & other ) : m_align(other.m_align) {}

    operator uint8_t() const
    {
        return m_align;
    }

    Align& operator=( Align const& other )
    {
        m_align = other.m_align;
        return *this;
    }

    enum EAlign
    {
        NoAlign = 0,
        Left = 1,
        Center = 2,
        Right = 4,
        Top = 8,
        Middle = 16,
        Bottom = 32,
        Baseline = 64,

        Centered = Align::Center | Align::Middle,

        TopLeft = Align::Left | Align::Top,
        TopCenter = Align::Center | Align::Top,
        TopRight = Align::Right | Align::Top,
        MiddleLeft = Align::Left | Align::Middle,
        MiddleCenter = Align::Centered,
        MiddleRight = Align::Right | Align::Middle,
        BottomLeft = Align::Left | Align::Bottom,
        BottomCenter = Align::Center | Align::Bottom,
        BottomRight = Align::Right | Align::Bottom,

        LeftTop = Align::TopLeft,
        CenterTop = Align::TopCenter,
        RightTop = Align::TopRight,
        LeftMiddle = Align::MiddleLeft,
        CenterMiddle = Align::Centered,
        RightMiddle = Align::MiddleRight,
        LeftBottom = Align::BottomLeft,
        CenterBottom = Align::BottomCenter,
        RightBottom = Align::BottomRight,

        Default = Align::TopLeft
    };

    static Recti apply( const Recti& pos, const Align& align)
    {
        int x = pos.x;
        int y = pos.y;
        const int w = pos.w;
        const int h = pos.h;

        // Apply horizontal align:
        if ( align & Align::Center ){ x -= w/2; }
        else if ( align & Align::Right ) { x -= w; }
        else {}
        // Apply vertical align:
        if ( align & Align::Middle ){ y -= h/2; }
        else if ( align & Align::Bottom ){ y -= h;   }
        else {}

        return Recti(x,y,w,h);
    }
};

} // end namespace de.

std::string dbStrVal(float val, int digits = 1);
std::string dbStrVal(double val, int digits = 1);

std::string dbHex( uint8_t byte );
std::string dbHex( uint16_t const color );
std::string dbHex( uint32_t const color );
std::string dbHex( uint64_t color );
std::string dbHex( uint8_t const* beg, uint8_t const* end );
std::string dbHex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow );

std::string dbStrNanoSeconds(double nSeconds);
std::string dbStrSeconds(double nSeconds);
std::string dbStrBytes(uint64_t nBytes);

std::string de_mbstr(const std::wstring& w );
std::string de_mbstr( wchar_t const w );
std::wstring de_wstr(const std::string& mb );

de::Blob dbLoadBlob( const std::string& uri );
bool dbLoadBlob( de::Blob & blob, const std::string& uri );
bool dbSaveBlob( const de::Blob& blob, const std::string& uri );

std::wstring dbLoadText(const std::wstring& uri);
std::string dbLoadText(const std::string& uri);

bool dbSaveText(const std::string& uri, const std::string& txt);
bool dbSaveText(const std::wstring& uri, const std::wstring& txt);

bool dbExistFile(const std::string& uri);
bool dbExistFile(const std::wstring& uri);

bool dbExistDirectory(const std::string& uri);
bool dbExistDirectory(const std::wstring& uri);

void dbStrLowerCase(std::string& txt, const std::locale& loc = std::locale());
void dbStrUpperCase(std::string& txt, const std::locale& loc = std::locale());

void dbStrLowerCase(std::wstring& txt);
void dbStrUpperCase(std::wstring& txt);

std::string dbStrReplace(const std::string& txt,
                         const std::string& from,
                         const std::string& to,
                         size_t* nReplacements = nullptr );

std::wstring dbStrReplace(const std::wstring& txt,
                          const std::wstring& from,
                          const std::wstring& to,
                          size_t* nReplacements = nullptr );

bool dbStrBeginsWith( const std::string& txt, const std::string& query );
bool dbStrBeginsWith( const std::wstring& txt, const std::wstring& query );
bool dbStrBeginsWith( const std::string& txt, char c );
bool dbStrBeginsWith( const std::wstring& txt, wchar_t c );
bool dbStrEndsWith( const std::string& txt, const std::string& query );
bool dbStrEndsWith( const std::wstring& txt, const std::wstring& query );
bool dbStrEndsWith( const std::string& txt, char c );
bool dbStrEndsWith( const std::wstring& txt, wchar_t c );

typedef std::string DE_StringA;
typedef std::vector< DE_StringA > DE_StringsA;
typedef std::wstring DE_StringW;
typedef std::vector< DE_StringW > DE_StringsW;

DE_StringsA
dbStrSplit(const std::string& txt, char searchChar, bool bKeepEmptyLines = false );

DE_StringsA
dbLoadTextLn(const std::string& uri);

DE_StringsA
dbLoadTextLn(const std::wstring& uri);

void dbRemoveFile( const std::string& uri );
int64_t dbFileSize( const std::string & uri );
int64_t dbFileSize( const std::wstring & uri );
std::string dbFileName( const std::string& uri, const std::string& relativeToPath = "" );
std::wstring dbFileName( const std::wstring& uri, const std::wstring& relativeToPath = L"" );
std::string dbFileBase( const std::string& uri );
std::wstring dbFileBase( const std::wstring& uri );
std::string dbFileSuffix( const std::string& uri );
std::wstring dbFileSuffix( const std::wstring& uri );
std::string dbFileDir( const std::string& uri );
std::wstring dbFileDir( const std::wstring& uri );
std::string dbParentDir( const std::string& uri );
std::wstring dbParentDir( const std::wstring& uri );

