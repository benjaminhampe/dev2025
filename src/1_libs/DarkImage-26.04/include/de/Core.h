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
    #if defined(_MSC_VER)
    #define DE_FORCE_INLINE __forceinline
    #else
    #define DE_FORCE_INLINE __attribute__((always_inline)) inline
    #endif
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

int64_t dbTimeInNanoseconds() noexcept;
int64_t dbTimeInMicroseconds() noexcept;
int32_t dbTimeInMilliseconds() noexcept;
double  dbTimeInSeconds() noexcept;

// ===========================================================================
// DarkRandom
// ===========================================================================
void    dbRandomize() noexcept;
int32_t dbRND() noexcept;

/// Since ANSI standard exists only 50yrs (1970!) its to few time for MS to implement it.
/// cmd.exe is not an ANSI console. AND
/// cmd.exe is nothing i would use and call myself a professional at the same time.
/// On Windows64 use the ANSI terminal in git4windows setup (based on MSYS2) or ConEmu64.
/// On Linux just open a terminal and be happy.
/// On Mac good luck, but i heard its a Linux ripoff
/// On Android good luck, but i heard its a Linux ripoff aswell.

/// @brief Write ANSI terminal/console color reset marker.
std::string
dbResetTerminalColors() noexcept;

/// @brief Write ANSI terminal/console color RGB marker. Foreground + Background colors.
std::string
dbSetTerminalColors(uint8_t fr, uint8_t fg, uint8_t fb,
                    uint8_t br, uint8_t bg, uint8_t bb) noexcept;

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
                  std::thread::id threadId = std::this_thread::get_id() ) noexcept;

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

// DebugBreak
#ifndef DE_ABORT
    #if defined(_MSC_VER)
        #define DE_ABORT __debugbreak();
    #else
        #define DE_ABORT __builtin_trap();
    #endif
#endif

/*  DE_ASSUME: Usage pattern:
    {
        const T* __restrict__ src = original.data();
              T* __restrict__ dst = temporary.data();

        DE_ASSUME_NO_OVERLAP(dst, src, nBytes); // -> Should help compiler to optimize. 'src' must be different memory region than 'dst'.

        std::memcpy(dst, src, nBytes); // -> With macro the compiler will optimize this aggressively. Even for AVX2.
    }
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

namespace de
{

// Quick and always nice typedefs:

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

// π
constexpr float TWO_PI32 = float( 2.0 * M_PI );
constexpr double TWO_PI64 = 2.0 * M_PI;

inline bool isPowerOfTwo(uint32_t x)
{
    return x && !(x & (x - 1));
}

inline uint32_t nextPowerOf2(uint32_t v)
{
    if (v == 0) return 1;   // handle edge case

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline float clampf(float x, float lo, float hi)
{
    return fminf(fmaxf(x, lo), hi);
}

inline double clampd(double x, double lo, double hi)
{
    return fmin(fmax(x, lo), hi);
}

inline float absf(float x)
{
    uint32_t bits;
    memcpy(&bits, &x, sizeof(bits));
    bits &= 0x7fffffff;
    memcpy(&x, &bits, sizeof(x));
    return x;
}

// Quick perf: prints time duration in [ms] from ctr to dtr call to console.
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

// Obviously used by log functions and macros.
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

// Used by AlignedShiftVector + AlignedShiftMatrix.
// ===========================================================================
struct BBox1f
// ===========================================================================
{
    float m_min;
    float m_max;

    BBox1f() : m_min(0.f), m_max(0.f)
    {}
    BBox1f(float min, float max) : m_min(min), m_max(max)
    {}

};

// Deprecated, use much more advanced Box3<f> in Math3D.h.
// ===========================================================================
struct BBox3f
// ===========================================================================
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

// Precious StringUtil.
// Uses Win32Api to convert between wide and mbstr. Since C++17 string_converter is deprecated.
// Many functions have 2 versions, one for mbstr (utf8) and wide (unicode)
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


// Fused <de/Core.h> with <de/AlignedMemory.h> since i use these classes very often.
// And several things in <de/Core.h> now use AlignedMemory. Like the memory Blob.
// Alignment = 32 (bytes) -> AVX2 ready. (current Default)
// Alignment = 64 (bytes) -> AVX512 ready. A full 64byte cache-line.
// ===========================================================================
template <typename T, std::size_t Alignment>
struct TAlignedVectorAllocator
// ===========================================================================
{
    static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be power of two");
    static_assert(Alignment >= alignof(T), "Alignment must be >= alignof(T)");

    // All this stuff is necessary, believe me.
    // And the commented out stuff needs to commented out.
    // When copying vectors the propagate stuff must be correct
    // and operator==/!= must be commented out, or it copies vector with wrong allocator. Boom.
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    //using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::true_type;

    // Rebind support
    template <typename U> struct rebind
    {
        using other = TAlignedVectorAllocator<U, Alignment>;
    };

    T* allocate(std::size_t n)
    {
        if (n > max_size())
        {
            throw std::bad_alloc();
        }

        size_t nBytes = n * sizeof(T);
        if (nBytes % Alignment != 0) // Padd vector at end to reach alignment
        {
            //DE_WARN(nBytes," not a multiple of Alignment(",Alignment,")")
            size_t nMofA = nBytes / Alignment;
            nBytes = (nMofA+1) * Alignment;
            //DE_WARN(nBytes," adapted to a multiple of Alignment(",Alignment,")")
            size_type padded = (nBytes + (Alignment - 1)) & ~(Alignment - 1);
            //DE_WARN(padded," padded to a multiple of Alignment(",Alignment,")")
        }
        void* ptr = _aligned_malloc(nBytes, Alignment); // Padd vector at begin to reach alignment.
        //std::aligned_alloc(Alignment, n * sizeof(T));
        if (!ptr) throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        if (p)
        {
            _aligned_free(p);
        }
    }

    std::size_t max_size() const noexcept
    {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // Keep this commented out!
    //bool operator==(const TAlignedVectorAllocator&) const noexcept { return true; }
    //bool operator!=(const TAlignedVectorAllocator&) const noexcept { return false; }
};

template <typename T>
using TAlignedVector = std::vector<T, TAlignedVectorAllocator<T, 32>>; // AVX2 ready

template <typename T>
using TAlignedVector64 = std::vector<T, TAlignedVectorAllocator<T, 64>>; // AVX512 ready

typedef TAlignedVector<f32> AlignedFloatVector; // AVX2 ready
typedef TAlignedVector<u8>  AlignedByteVector;  // AVX2 ready

// Compute 1D boundingBox of data:
inline BBox1f
computeMinMax(AlignedFloatVector const & v)
{
    f32 lMin = std::numeric_limits< f32 >::max();
    f32 lMax = std::numeric_limits< f32 >::lowest();

    for ( const f32& f : v )
    {
        lMin = std::min( lMin, f );
        lMax = std::max( lMax, f );
    }

    return BBox1f(lMin,lMax);
}

// ----------------------------------------------------
// TAlignedShiftVector
// ----------------------------------------------------
template < typename T >
struct TAlignedShiftVector
{
    using Vector = TAlignedVector<T>;
    using FN_onFullVector = std::function< void(Vector const &)>;
    u64 m_free;
    u64 m_used;
    TAlignedVector< T > m_data;
    FN_onFullVector m_onFullVector;

    TAlignedShiftVector( size_t n = 0 )
        : m_free(0)
        , m_used(0)
        , m_onFullVector([] (Vector const &) {})
    {
        if (n>0)
        {
            m_data.resize(n);
        }
    }

    void setCallback_onFullVector( FN_onFullVector const & onFullVector ) { m_onFullVector = onFullVector; }

    BBox1f getMinMax() const { return computeMinMax(m_data); }

    void resize( size_t desired )
    {
        if (m_data.size() != desired)
        {
            m_data.resize( desired );
            m_used = 0;
            m_free = m_data.size();
        }
    }

    // ShiftVector<float>(8)
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |   ?   |
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // ShiftVector<float>(8).push([0,1,2,3,4,5,6])
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   ?   |
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // ShiftVector<float>(8).push([7,8])
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // ShiftVector<float>(8).push([A,B,C,D])
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // |   5   |   6   |   7   |   8   |   A   |   B   |   C   |   D   |
    // +-------+-------+-------+-------+-------+-------+-------+-------+;
    // ShiftVector<float>(8).push([E,F,B])
    // +-------+-------+-------+-------+-------+-------+-------+-------+
    // |   8   |   A   |   B   |   C   |   D   |   E   |   F   |   B   |
    // +-------+-------+-------+-------+-------+-------+-------+-------+;

    void push(const T* __restrict__ pSamples, u32 nSamples)
    {
        if (nSamples > size())
        {
            // resize(nSamples);
            DE_ERROR("Split logic is not recursive. nSamples too large for single iteration.")
            nSamples = size();
        }

        if (size() == nSamples)
        {
            std::memcpy(m_data.data(), pSamples, nSamples * sizeof(T));

            m_onFullVector(m_data);

            m_used = 0;

            return;
        }

        // TAlignedShiftVector<float>(8) v8: nAvail=8
        // ++---+---+---+---++---+---+---+---++
        // ||   |   |   |   ||   |   |   |   ||
        // ++---+---+---+---++---+---+---+---++

        // 0.) push([0,1,2,3,4])
        // - Before: nAvail=8, nSamples=5       (nAvail >= nSamples)
        // - After:  nAvail=3
        // ++---+---+---+---++---+---+---+---++
        // || 0 | 1 | 2 | 3 || 4 | ? | ? | ? ||
        // ++---+---+---+---++---+---+---+---++

        // 1.) push([5,6,7,8,9])                (nAvail >= nSamples)
        // - Before: nAvail=3, nSamples=5

        // 2.) push([5,6,7])
        // - After:  nAvail=0, nSamples=2         ---> Notify onFullRow()
        // - After:  nAvail=8, nSamples=2       (nAvail > nSamples)
        // ++---+---+---+---++---+---+---+---++
        // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||
        // ++---+---+---+---++---+---+---+---++

        // 3.) push([8,9])
        // - After:  nAvail=6, nSamples=0
        // ++---+---+---+---++---+---+---+---++
        // || 8 | 9 |   |   ||   |   |   |   ||
        // ++---+---+---+---++---+---+---+---++

        if (nSamples + used() > size())
        {
            // push([5,6,7,8,9]) :: nSamples = 5, nAvail = size(8)-used(5);
            u64 nAvail = avail();
            // I. push([5,6,7]) :: nLeft = min(nSamples,nAvail) = min(nSamples,3) = 3;
            u64 n1 = std::min<u64>(nSamples, nAvail);
            // II. push([8,9]) :: nRight = nSamples - nLeft = 2;
            u64 n2 = nSamples - n1;

            // I. Copy until end of row ... ( push([5,6,7]) )
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 |[5]|[6]|[7]|| :: nLeft = 3 = nSamples - (m_size - m_used);
            // ++---+---+---+---++---+---+---+---++
            if (n1 > 0)
            {
                const T* __restrict__ src = pSamples;  // src=[5,6,7|8,9]
                T* __restrict__ dst = data() + used(); // dst=[0,1,2,3,4|?,?,?]
                std::memcpy(dst, src, n1 * sizeof(T));
            }

            // II. Notify row ...
            // ++---+---+---+---++---+---+---+---++
            // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
            // ++---+---+---+---++---+---+---+---++
            // III. Reset row ...
            // ++---+---+---+---++---+---+---+---++
            // ||   |   |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++
            // m_used = 0, size() = 8;
            m_onFullVector(m_data);
            m_used = 0;

            // DE_OK("[",caller,"] onFullVector(", m_data.size(),"), used(",m_used,") :: push(",n1,") :: END")
            // DE_OK("[",caller,"] onFullVector(", m_data.size(),"), used(",m_used,") :: push(",n2,") :: END")
            //

            // IV. Final push([8,9]) :: nRight = 2 = nSamples - Left
            // ++---+---+---+---++---+---+---+---++
            // || 8 | 9 |   |   ||   |   |   |   ||
            // ++---+---+---+---++---+---+---+---++
            // m_used = 2;
            if (n2 > 0)
            {
                const T* __restrict__ src = pSamples + n1; // Read remain input token.
                T* __restrict__ dst = data(); // Write to begin() of shiftbuffer.
                std::memcpy( dst, src, n2 * sizeof(T) );
                m_used = n2;
            }
        }
        else
        {
            // DE_OK("MID")
            const T* __restrict__ src = pSamples;
            T* __restrict__ dst = data() + used();
            std::memcpy( dst, src, nSamples * sizeof(T));
            m_used += nSamples;
        }
    }

    T const * data() const { return m_data.data(); }
    T * data() { return m_data.data(); }

    T const & at( size_t i ) const { return m_data.at( i ); }
    T & at( size_t i ) { return m_data.at( i ); }

    T const & operator[] ( size_t i ) const { return m_data.at( i ); }
    T & operator[] ( size_t i ) { return m_data.at( i ); }

    u64 avail() const
    {
        if (m_data.size() < m_used)
        {
            throw std::runtime_error("m_data.size() < m_used");
        }
        return m_data.size() - m_used;
    }

    u64 used() const { return m_used; }
    u64 size() const { return m_data.size(); }
    u64 capacity() const { return m_data.capacity(); }
    void clear() { m_used = 0; m_free = m_data.size(); }
    void fill( T const & value ) { for ( T & f : m_data) f = value; }
    void fillZero() { for ( auto & f : m_data) f = 0.0f; }

    static bool compare( TAlignedVector<T> const & a, TAlignedVector<T> const & b )
    {
        if (a.size() != b.size())
        {
            DE_ERROR("a.size() != b.size()")
            return false;
        }

        for (size_t i = 0; i < a.size(); i++)
        {
            if (a[i] != b[i])
            {
                DE_ERROR("a[i] != b[i]")
                return false;
            }
        }
        return true;
    }

    bool operator==( TAlignedVector<T> const & other ) const
    {
        return compare(*this,other);
    }

    bool operator!=( TAlignedVector<T> const & other ) const
    {
        return !compare(*this,other);
    }

    std::string str() const
    {
        std::stringstream o;

        o << "n = " << m_data.size() << "\n";
        for ( u64 i = 0; i < m_data.size(); ++i )
        {
            o << "[" << i << "] " << m_data[ i ] << "\n";
        }
        return o.str();
    }
};

// ----------------------------------------------------
// ShiftVectorTest
// ----------------------------------------------------
struct ShiftVectorTest
{
    static void
    test()
    {
        typedef float T;

        // TAlignedShiftVector<float>(8) v8: m_used = 0;
        // ++---+---+---+---++---+---+---+---++
        // ||   |   |   |   ||   |   |   |   ||
        // ++---+---+---+---++---+---+---+---++

        TAlignedShiftVector<T> testObj; // (8);
        testObj.resize(8);

        if (testObj.size() != 8)
        {
            DE_ERROR("testObj.size() != 8")
            return;
        }

        // [n=8] Push([0,1,2,3,4]) :: nSamples = 5, testObj.m_used = 0;
        // ++---+---+---+---++---+---+---+---++
        // || 0 | 1 | 2 | 3 || 4 | ? | ? | ? ||
        // ++---+---+---+---++---+---+---+---++
        TAlignedVector<T> a{0,1,2,3,4};
        testObj.push(a.data(),a.size());
        TAlignedVector<T> b{0,1,2,3,4,0,0,0};
        if (testObj.m_data != b)
        {
            DE_ERROR("Test(1)[n=8] != {0,1,2,3,4,5,6}")
            return;
        }

        // [n=8] Push([5,6,7,8,9]) :: nSamples = 5, testObj.m_used = 5, testObj.m_size = 8;

        // = Push([5,6,7]) :: nLeft = 3 = nSamples - (testObj.m_size - testObj.m_used);
        // ++---+---+---+---++---+---+---+---++
        // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
        // ++---+---+---+---++---+---+---+---++

        // + Push([8,9]) :: nRight = 2 = nSamples - Left, testObj.m_used = 2;
        // ++---+---+---+---++---+---+---+---++
        // || 8 | 9 |   |   ||   |   |   |   ||
        // ++---+---+---+---++---+---+---+---++

        // push([5,6,7]) :: n1 = 3 = nSamples - (m_size - m_used);
        // push([8,9]) :: n2 = 2 = nSamples - n1;

        // ++---+---+---+---++---+---+---+---++
        // || 0 | 1 | 2 | 3 || 4 | 5 | 6 | 7 ||  ---> Notify onFullRow()
        // ++---+---+---+---++---+---+---+---++

        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |       |       |       |       |       |       |       |       |
        // |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   ?   |
        // |       |       |       |       |       |       |       |       |
        // +-------+-------+-------+-------+-------+-------+-------+-------+

        // [n=8] Push([7,8]):
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |       |       |       |       |       |       |       |       |
        // |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
        // |       |       |       |       |       |       |       |       |
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        bool bTestFull1 = false;
        testObj.setCallback_onFullVector([&](TAlignedVector<T> const &v)
                            { bTestFull1 = true; });
        TAlignedVector<T> c{7,8};
        testObj.push(c.data(),c.size());
        if (!bTestFull1)
        {
            DE_ERROR("!bTestFull1")
            return;
        }

        if (testObj.m_data != TAlignedVector<T>{1,2,3,4,5,6,7,8})
        {
            DE_ERROR("Test(2)[n=8]  != {1,2,3,4,5,6,7,8}")
            return;
        }
        // [n=8] Push([4,4,6,0]):
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |       |       |       |       |       |       |       |       |
        // |   5   |   6   |   7   |   8   |   4   |   4   |   6   |   0   |
        // |       |       |       |       |       |       |       |       |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;
        bool bTestFull2 = false;
        testObj.setCallback_onFullVector([&](TAlignedVector<T> const &v){ bTestFull2 = true; });
        TAlignedVector<T> d{4,4,6,0};
        testObj.push(d.data(),d.size());

        if (!bTestFull2)
        {
            DE_ERROR("!bTestFull2")
            //return;
        }

        if (testObj.m_data != TAlignedVector<T>{5,6,7,8,4,4,6,0})
        {
            DE_ERROR("Test(3)[n=8]  != {5,6,7,8,4,4,6,0}")
            return;
        }
        // [n=8] Push([1,1,2]):
        // +-------+-------+-------+-------+-------+-------+-------+-------+
        // |       |       |       |       |       |       |       |       |
        // |   8   |   4   |   4   |   6   |   0   |   1   |   1   |   2   |
        // |       |       |       |       |       |       |       |       |
        // +-------+-------+-------+-------+-------+-------+-------+-------+;
        TAlignedVector<T> e{1,1,2};
        testObj.push(e.data(),e.size());
        if (testObj.m_data != TAlignedVector<T>{8,4,4,6,0,1,1,2})
        {
            DE_ERROR("Test(4)[n=8]  != {8,4,4,6,0,1,1,2}")
            return;
        }

        /*
        TAlignedVector< T > a{ 0,1,2,3,4,5,6,7,8,9};
        TAlignedVector< T > b{ 10,11,12,13,14,15,16,17,18,19};
        TAlignedVector< T > c{ 20,21,22,23,24,25,26,27,28,29};

        TAlignedShiftVector<T> testObj2;

        testObj.resize( 20 );
        DE_DEBUG( "TestResult[0] :: Resize(20): ", testObj.str() )

        testObj.push( "test1", a.data(), 10, true );
        DE_DEBUG( "TestResult[1] :: Push(a): ", testObj.str() )

        testObj.push( "test2", b.data(), 10, true );
        DE_DEBUG( "TestResult[2] :: Push(b): ", testObj.str() )

        testObj.push( "test3", c.data(), 10, true );
        DE_DEBUG( "TestResult[3] :: Push(c): ", testObj.str() )
        */
    }
};

typedef TAlignedShiftVector<f32>    AlignedFloatShiftVector;
typedef TAlignedShiftVector<u8>     AlignedByteShiftVector;

// Manages a rows that are automaticly shifted when new data arrives.
//
// 3D meshes are created out of this data to render with OpenGLES.
//
// Can hold AudioWaveform ( 1 float per sample )
//       or AudioWaveformAmp in dB ( 1 float ), 1 dB = 20 * log10( amp*amp );
//       or AudioSpektrumAmplitude in dB ( 1 float )
//       or AudioSpektrumPhase in radians? or cents? ( 1 float )
//
// does only swap pointers and does not move memory!
// push() manages matrix shift and always refills the 0-th row
// if m_shiftBuffer collected enough samples to fill a new row.
// A ShiftBuffer is used to decouple different audio callback size and matrix column size
// But only meaningful if m_dacFrames <= m_shiftBuffer.size()
//
// To have a more quadratic matrix it uses m_dacFrames = m_shiftBuffer.size()
// so the shiftbuffer probably collects only once untils its already full.
// With colCount == m_dacFrames each row represents exactly one audio callback.
//
// Since push() is called constantly we delay expensive postfx like log10f() until render().
// render() collects data from the matrix using m_samples, not m_orig or m_copy.
// =======================================================================
struct AlignedFloatShiftMatrix
// =======================================================================
{
    typedef float T;
    typedef TAlignedVector< T > TData;
    typedef TAlignedVector< T* > TRowVector;

    u32 m_colCount;  // Count matrix cols = m_shiftBuffer.size()
    u32 m_rowCount;  // Count matrix rows
    TRowVector m_rows; // Row Viewer ( original rows )
    TRowVector m_temp; // Row Viewer ( shuffled rows )
    TData m_data;

    AlignedFloatShiftMatrix();
    ~AlignedFloatShiftMatrix();

    void resize( u32 colCount, u32 rowCount );
    void push( T const* __restrict__ src, u32 srcFrames );
    u32 rowCount() const;
    u32 columnCount() const;
    BBox1f getMinMax() const;
    const T* getRow(int32_t row) const;
    T getPixel(int32_t col, int32_t row, float defaultValue = 0.0f) const;

    static void
    shiftVectorLeft(TRowVector & orig, TRowVector & temp);

    static void
    shiftVectorRight(TRowVector & orig, TRowVector & temp);


    static void test();
    static void testShiftLeft();
    static void testShiftRight();
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
    ReadWrite = Read | Write
};

enum class eSeekMode
{
    Set = 0,
    Cur = 1,
    End = 2
};

int32_t file64_open(const std::wstring& utf16path, eFileMode fileMode, int32_t permission = 0);
int32_t file64_open(const std::string& utf8path, eFileMode fileMode, int32_t permission = 0);
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
    std::string m_uri;

    File();
    ~File();

    File(const std::wstring& utf8_uri, eFileMode fm = eFileMode::Read,
         int permissionBits = 0);

    File(const std::string& utf8_uri, eFileMode fm = eFileMode::Read,
         int permissionBits = 0);

    bool
    open(const std::wstring& utf8_uri, eFileMode fm = eFileMode::Read,
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
    int64_t write(const void* __restrict__ src, int64_t nBytes ) const;
    int64_t read(void* __restrict__ dst, int64_t nBytes ) const;

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

// New: Uses AlignedMemory now. Yay!
typedef TAlignedVector<uint8_t> Blob;

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

    static bool
    saveBlob( const Blob& blob, const std::string& uri );

    static bool
    loadBlob( Blob& blob, const std::string& uri, const int64_t sizeLimit = int64_t(1024*1024*1204) * 5 ); // 5GB limit

    // static Blob
    // loadBlob( const std::string& uri );

    // static bool
    // saveBin( const std::string& uri, const Blob& blob );

    // static bool
    // loadBin( const std::string& uri, Blob& blob );

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
    makeAbsolute( const std::string& uri, const std::string& baseDir = "" );

    static std::wstring
    makeAbsolute( const std::wstring& uri, const std::wstring& baseDir = L"" );

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

    static void
    removeFile( const std::wstring& uri );

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

// bool dbIsPowerOfTwo(uint32_t x);

// uint32_t dbNextPowerOf2(uint32_t v);

//==============================================================================
/*
DE_FORCE_INLINE bool dbIsPowerOfTwo(uint32_t k) noexcept
{
    if (k < 2) return false;
    return 0 == (k & (k - 1));
}
*/
DE_FORCE_INLINE bool dbIsPowerOfTwo(uint8_t x) noexcept
{
    return x && !(x & (x - 1));
}

DE_FORCE_INLINE bool dbIsPowerOfTwo(uint16_t x) noexcept
{
    return x && !(x & (x - 1));
}

DE_FORCE_INLINE bool dbIsPowerOfTwo(uint32_t x) noexcept
{
    return x && !(x & (x - 1));
}

DE_FORCE_INLINE bool dbIsPowerOfTwo(uint64_t x) noexcept
{
    return x && !(x & (x - 1));
}

/*
DE_FORCE_INLINE uint32_t dbNextPowerOf2(uint32_t v) noexcept
{
    if (v == 0) return 2;   // handle edge case

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}
*/

// Returns the smallest PowerOf2 greater or equal than the given integral value.

DE_FORCE_INLINE uint16_t dbNextPowerOf2_16(uint16_t k) noexcept
{
    if (k < 2) return 2;
    --k;
    k |= (k >> 1);
    k |= (k >> 2);
    k |= (k >> 4);
    k |= (k >> 8);
    return k + 1;
}

DE_FORCE_INLINE uint32_t dbNextPowerOf2( uint16_t k ) noexcept
{
   if (k < 2) return 2;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   return k + 1;
}

// Returns the smallest PowerOf2 greater or equal than the given integral value.
DE_FORCE_INLINE uint32_t dbNextPowerOf2( uint32_t k ) noexcept
{
   if (k < 2) return 2;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   k |= (k >> 16);
   return k + 1;
}

DE_FORCE_INLINE uint32_t dbNextPowerOf2_noverflow( uint32_t k ) noexcept
{
   if (k < 2) return 2;
   if (k >= 0x80000000u) return 0x80000000u;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   k |= (k >> 16);
   return k + 1;
}

DE_FORCE_INLINE uint32_t dbNextPowerOf2_branchless(uint32_t k) noexcept
{
    uint32_t isSmall = (k < 2);
    uint32_t x = k - 1;

    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);

    uint32_t next = x + 1;

    // If k < 2, return 2; otherwise return next.
    return (isSmall * 2) | ((~isSmall) & next);
}

// Returns the smallest PowerOf2 greater or equal than the given integral value.
DE_FORCE_INLINE uint64_t dbNextPowerOf2( uint64_t k ) noexcept
{
   if (k < 2) return 2;
   --k;
   k |= (k >> 1);
   k |= (k >> 2);
   k |= (k >> 4);
   k |= (k >> 8);
   k |= (k >> 16);
   k |= (k >> 32);
   return k + 1;
}

DE_FORCE_INLINE de::s32 dbClampi( de::s32 v, de::s32 vmin, de::s32 vmax )
{
    return std::clamp<de::s32>( v, vmin, vmax );
}

DE_FORCE_INLINE de::f32 dbClampf(de::f32 x, de::f32 lo, de::f32 hi)
{
    return fminf(fmaxf(x, lo), hi);
}

DE_FORCE_INLINE de::f64 dbClampd(de::f64 x, de::f64 lo, de::f64 hi)
{
    return fmin(fmax(x, lo), hi);
}

DE_FORCE_INLINE de::f32 dbAbsf(de::f32 x)
{
    de::u32 bits;
    memcpy(&bits, &x, sizeof(bits));
    bits &= 0x7fffffff;
    memcpy(&x, &bits, sizeof(x));
    return x;
}

DE_FORCE_INLINE de::f64 dbAbsd(de::f64 x)
{
    de::u64 bits;
    memcpy(&bits, &x, sizeof(bits));
    bits &= 0x7fffffffffffffff;
    memcpy(&x, &bits, sizeof(x));
    return x;
}

// Round:
DE_FORCE_INLINE de::f32 dbRound( de::f32 v ) { return std::roundf( v ); }
DE_FORCE_INLINE de::f64 dbRound( de::f64 v ) { return std::round( v ); }
DE_FORCE_INLINE de::f80 dbRound( de::f80 v ) { return std::roundl( v ); }

// Round32:
DE_FORCE_INLINE int32_t dbRound32( de::f32 v ) { return std::lroundf( v ); }
DE_FORCE_INLINE int32_t dbRound32( de::f64 v ) { return std::lround( v ); }
DE_FORCE_INLINE int32_t dbRound32( de::f80 v ) { return std::lroundl( v ); }

// Round64:
DE_FORCE_INLINE int64_t dbRound64( de::f32 v ) { return std::llroundf( v ); }
DE_FORCE_INLINE int64_t dbRound64( de::f64 v ) { return std::llround( v ); }
DE_FORCE_INLINE int64_t dbRound64( de::f80 v ) { return std::llroundl( v ); }

// SafeModulo with negative numbers
DE_FORCE_INLINE int32_t dbSafeMod( int32_t a, int32_t b )
{
   if ( a == 0 || b == 0 ) { return 0; }
   return std::abs( a ) % std::abs( b );
}

// Min():
DE_FORCE_INLINE de::u8  dbMin( de::u8  a, de::u8  b ) { return std::min<de::u8> (a,b); }
DE_FORCE_INLINE de::s8  dbMin( de::s8  a, de::s8  b ) { return std::min<de::s8> (a,b); }
DE_FORCE_INLINE de::u16 dbMin( de::u16 a, de::u16 b ) { return std::min<de::u16>(a,b); }
DE_FORCE_INLINE de::s16 dbMin( de::s16 a, de::s16 b ) { return std::min<de::s16>(a,b); }
DE_FORCE_INLINE de::u32 dbMin( de::u32 a, de::u32 b ) { return std::min<de::u32>(a,b); }
DE_FORCE_INLINE de::s32 dbMin( de::s32 a, de::s32 b ) { return std::min<de::s32>(a,b); }
DE_FORCE_INLINE de::u64 dbMin( de::u64 a, de::u64 b ) { return std::min<de::u64>(a,b); }
DE_FORCE_INLINE de::s64 dbMin( de::s64 a, de::s64 b ) { return std::min<de::s64>(a,b); }
DE_FORCE_INLINE de::f32 dbMin( de::f32 a, de::f32 b ) { return std::fminf(a,b); }
DE_FORCE_INLINE de::f64 dbMin( de::f64 a, de::f64 b ) { return std::fmin(a,b); }
DE_FORCE_INLINE de::f80 dbMin( de::f80 a, de::f80 b ) { return std::fminl(a,b); }

// Max():
DE_FORCE_INLINE de::u8  dbMax( de::u8  a, de::u8  b ) { return std::max<de::u8> (a,b); }
DE_FORCE_INLINE de::s8  dbMax( de::s8  a, de::s8  b ) { return std::max<de::s8> (a,b); }
DE_FORCE_INLINE de::u16 dbMax( de::u16 a, de::u16 b ) { return std::max<de::u16>(a,b); }
DE_FORCE_INLINE de::s16 dbMax( de::s16 a, de::s16 b ) { return std::max<de::s16>(a,b); }
DE_FORCE_INLINE de::u32 dbMax( de::u32 a, de::u32 b ) { return std::max<de::u32>(a,b); }
DE_FORCE_INLINE de::s32 dbMax( de::s32 a, de::s32 b ) { return std::max<de::s32>(a,b); }
DE_FORCE_INLINE de::u64 dbMax( de::u64 a, de::u64 b ) { return std::max<de::u64>(a,b); }
DE_FORCE_INLINE de::s64 dbMax( de::s64 a, de::s64 b ) { return std::max<de::s64>(a,b); }
DE_FORCE_INLINE de::f32 dbMax( de::f32 a, de::f32 b ) { return std::fmaxf(a,b); }
DE_FORCE_INLINE de::f64 dbMax( de::f64 a, de::f64 b ) { return std::fmax(a,b); }
DE_FORCE_INLINE de::f80 dbMax( de::f80 a, de::f80 b ) { return std::fmaxl(a,b); }

// Abs():
DE_FORCE_INLINE de::u8  dbAbs( de::u8  v ) { return v; }
DE_FORCE_INLINE de::s8  dbAbs( de::s8  v ) { return std::abs(v); }
DE_FORCE_INLINE de::u16 dbAbs( de::u16 v ) { return v; }
DE_FORCE_INLINE de::s16 dbAbs( de::s16 v ) { return std::abs(v); }
DE_FORCE_INLINE de::u32 dbAbs( de::u32 v ) { return v; }
DE_FORCE_INLINE de::s32 dbAbs( de::s32 v ) { return std::abs(v); }
DE_FORCE_INLINE de::u64 dbAbs( de::u64 v ) { return v; }
DE_FORCE_INLINE de::s64 dbAbs( de::s64 v ) { return std::abs(v); }
DE_FORCE_INLINE de::f32 dbAbs( de::f32 v ) { return std::fabsf( v ); }
DE_FORCE_INLINE de::f64 dbAbs( de::f64 v ) { return std::fabs( v ); }
DE_FORCE_INLINE de::f80 dbAbs( de::f80 v ) { return std::fabsl( v ); }

// Sin():
DE_FORCE_INLINE de::f32 dbSin( de::f32 v ) { return ::sinf( v ); }
DE_FORCE_INLINE de::f64 dbSin( de::f64 v ) { return ::sin( v ); }
DE_FORCE_INLINE de::f80 dbSin( de::f80 v ) { return ::sinl( v ); }

// Cos():
DE_FORCE_INLINE de::f32 dbCos( de::f32 v ) { return ::cosf( v ); }
DE_FORCE_INLINE de::f64 dbCos( de::f64 v ) { return ::cos( v ); }
DE_FORCE_INLINE de::f80 dbCos( de::f80 v ) { return ::cosl( v ); }

// Atan2():
DE_FORCE_INLINE de::f32 dbAtan2( de::f32 a, de::f32 b ) { return ::atan2f( a,b ); }
DE_FORCE_INLINE de::f64 dbAtan2( de::f64 a, de::f64 b ) { return ::atan2( a,b ); }
DE_FORCE_INLINE de::f80 dbAtan2( de::f80 a, de::f80 b ) { return ::atan2l( a,b ); }

// Arcsin():
DE_FORCE_INLINE de::f32 dbAsin( de::f32 v ) { return ::asinf( v ); }
DE_FORCE_INLINE de::f64 dbAsin( de::f64 v ) { return ::asin( v ); }
DE_FORCE_INLINE de::f80 dbAsin( de::f80 v ) { return ::asinl( v ); }

// Arccos():
DE_FORCE_INLINE de::f32 dbAcos( de::f32 v ) { return ::acosf( v ); }
DE_FORCE_INLINE de::f64 dbAcos( de::f64 v ) { return ::acos( v ); }
DE_FORCE_INLINE de::f80 dbAcos( de::f80 v ) { return ::acosl( v ); }

// SQRT():
DE_FORCE_INLINE de::f32 dbSqrt( de::f32 v ) { return ::sqrtf( v ); }
DE_FORCE_INLINE de::f64 dbSqrt( de::f64 v ) { return ::sqrt( v ); }
DE_FORCE_INLINE de::f80 dbSqrt( de::f80 v ) { return ::sqrtl( v ); }

// isInfinite():
DE_FORCE_INLINE de::f32 dbIsInf( de::f32 v ) { return std::isinf( v ) || std::isnan( v ); }
DE_FORCE_INLINE de::f64 dbIsInf( de::f64 v ) { return std::isinf( v ) || std::isnan( v ); }
DE_FORCE_INLINE de::f80 dbIsInf( de::f80 v ) { return std::isinf( v ) || std::isnan( v ); }

// DEG: ( convert radians to degrees )
DE_FORCE_INLINE de::f32 dbDEG( de::f32 radians ) { return radians * de::f32(180.0 / 3.1415926535897932384626433832795028841971693993751); }
DE_FORCE_INLINE de::f64 dbDEG( de::f64 radians ) { return radians * de::f64(180.0 / 3.1415926535897932384626433832795028841971693993751); }
DE_FORCE_INLINE de::f80 dbDEG( de::f80 radians ) { return radians * de::f80(180.0 / 3.1415926535897932384626433832795028841971693993751); }

template < typename T > glm::tvec2< T > dbDEG( glm::tvec2< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ) }; }
template < typename T > glm::tvec3< T > dbDEG( glm::tvec3< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ), dbDEG( v.z ) }; }
template < typename T > glm::tvec4< T > dbDEG( glm::tvec4< T > const & v ) { return { dbDEG( v.x ), dbDEG( v.y ), dbDEG( v.z ), dbDEG( v.w ) }; }

// RAD: ( convert degrees to radians )
DE_FORCE_INLINE de::f32 dbRAD( de::f32 degrees ) { return degrees * de::f32(3.1415926535897932384626433832795028841971693993751 / 180.0); }
DE_FORCE_INLINE de::f64 dbRAD( de::f64 degrees ) { return degrees * de::f64(3.1415926535897932384626433832795028841971693993751 / 180.0); }
DE_FORCE_INLINE de::f80 dbRAD( de::f80 degrees ) { return degrees * de::f80(3.1415926535897932384626433832795028841971693993751 / 180.0); }

template < typename T > glm::tvec2< T > dbRAD( glm::tvec2< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ) }; }
template < typename T > glm::tvec3< T > dbRAD( glm::tvec3< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ), dbRAD( v.z ) }; }
template < typename T > glm::tvec4< T > dbRAD( glm::tvec4< T > const & v ) { return { dbRAD( v.x ), dbRAD( v.y ), dbRAD( v.z ), dbRAD( v.w ) }; }

// Clamp:
DE_FORCE_INLINE de::s32 dbClamp( de::s32 v, de::s32 vmin, de::s32 vmax ) { return std::clamp<de::s32>( v, vmin, vmax ); }
DE_FORCE_INLINE de::f32 dbClamp( de::f32 v, de::f32 vmin, de::f32 vmax ) { return dbClampf( v, vmin, vmax ); }
DE_FORCE_INLINE de::f64 dbClamp( de::f64 v, de::f64 vmin, de::f64 vmax ) { return dbClampd( v, vmin, vmax ); }

// Reciprocal:
DE_FORCE_INLINE de::f32 dbReciprocal( de::f32 v ) { de::f32 i = 1.0f / v; if ( std::isinf( i ) || std::isnan( i ) ) { return 0.0f; } return i; }
DE_FORCE_INLINE de::f64 dbReciprocal( de::f64 v ) { de::f64 i = 1.0 / v; if ( std::isinf( i ) || std::isnan( i ) ) { return 0.0; } return i; }

// Equals:
DE_FORCE_INLINE bool dbEquals( de::f32 a, de::f32 b, de::f32 eps = 1.0e-6f ) { return dbAbsf( b-a ) <= eps; }
DE_FORCE_INLINE bool dbEquals( de::f64 a, de::f64 b, de::f64 eps = 1.0e-9 ) { return dbAbsd( b-a ) <= eps; }
DE_FORCE_INLINE bool dbEquals( glm::vec3 const & a, glm::vec3 const & b, float eps = 1.0e-6f )
{
   return dbEquals( a.x,b.x,eps ) && dbEquals( a.y,b.y,eps ) && dbEquals( a.z,b.z,eps );
}
DE_FORCE_INLINE bool dbEquals( glm::dvec3 const & a, glm::dvec3 const & b, double eps = 1e-6 )
{
   return dbEquals( a.x, b.x, eps ) && dbEquals( a.y, b.y, eps ) && dbEquals( a.z, b.z, eps );
}

// IsZero:
DE_FORCE_INLINE de::f32 dbIsZero( de::f32 v ) { return dbEquals( v, 0.0f ); }
DE_FORCE_INLINE de::f64 dbIsZero( de::f64 v ) { return dbEquals( v, 0.0, 1.0e-15 ); }

/*
// Byte2float
DE_FORCE_INLINE float dbByte2float( uint8_t b_0_255 )
{
   return float( b_0_255 ) / 255.0f;
}

// Float2byte
DE_FORCE_INLINE uint8_t dbFloat2byte( float f_0_1 )
{
   return uint8_t( dbClamp( int( f_0_1 * 255.0f ), 0, 255 ) );
}
*/

// ========================================================================
bool dbMouseOver( int mx, int my, int x1, int y1, int x2, int y2 );
bool dbMouseOver( int mx, int my, const de::Recti& pos );
// ========================================================================
std::string dbStrVal(float val, int digits = 1);
std::string dbStrVal(double val, int digits = 1);
// ========================================================================
std::string dbHex( uint8_t byte );
std::string dbHex( uint16_t const color );
std::string dbHex( uint32_t const color );
std::string dbHex( uint64_t color );
std::string dbHex( uint8_t const* beg, uint8_t const* end );
std::string dbHex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow );
// ========================================================================
std::string dbStrNanoSeconds(double nSeconds);
std::string dbStrSeconds(double nSeconds);
std::string dbStrBytes(uint64_t nBytes);
// ========================================================================
std::string de_mbstr(const std::wstring& w );
std::string de_mbstr( wchar_t const w );
std::wstring de_wstr(const std::string& mb );
// ========================================================================
bool dbLoadBlob( de::Blob & blob, const std::string& uri, const int64_t sizeLimit = int64_t(1024*1024*1204) * 5 );
bool dbSaveBlob( const de::Blob& blob, const std::string& uri );
// ========================================================================
std::wstring dbLoadText(const std::wstring& uri);
std::string dbLoadText(const std::string& uri);
// ========================================================================
bool dbSaveText(const std::string& uri, const std::string& txt);
bool dbSaveText(const std::wstring& uri, const std::wstring& txt);
// ========================================================================
bool dbExistFile(const std::string& uri);
bool dbExistFile(const std::wstring& uri);
// ========================================================================
bool dbExistDirectory(const std::string& uri);
bool dbExistDirectory(const std::wstring& uri);
// ========================================================================
void dbStrLowerCase(std::string& txt, const std::locale& loc = std::locale());
void dbStrUpperCase(std::string& txt, const std::locale& loc = std::locale());
// ========================================================================
void dbStrLowerCase(std::wstring& txt);
void dbStrUpperCase(std::wstring& txt);
// ========================================================================
std::string dbStrReplace(const std::string& txt,
                         const std::string& from,
                         const std::string& to,
                         size_t* nReplacements = nullptr );

std::wstring dbStrReplace(const std::wstring& txt,
                          const std::wstring& from,
                          const std::wstring& to,
                          size_t* nReplacements = nullptr );
// ========================================================================
bool dbStrBeginsWith( const std::string& txt, const std::string& query );
bool dbStrBeginsWith( const std::wstring& txt, const std::wstring& query );
bool dbStrBeginsWith( const std::string& txt, char c );
bool dbStrBeginsWith( const std::wstring& txt, wchar_t c );
// ========================================================================
bool dbStrEndsWith( const std::string& txt, const std::string& query );
bool dbStrEndsWith( const std::wstring& txt, const std::wstring& query );
bool dbStrEndsWith( const std::string& txt, char c );
bool dbStrEndsWith( const std::wstring& txt, wchar_t c );
// ========================================================================
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
// ========================================================================
int64_t dbFileSize( const std::string & uri );
int64_t dbFileSize( const std::wstring & uri );
// ========================================================================
std::string dbFileName( const std::string& uri, const std::string& relativeToPath = "" );
std::wstring dbFileName( const std::wstring& uri, const std::wstring& relativeToPath = L"" );
// ========================================================================
std::string dbFileBase( const std::string& uri );
std::wstring dbFileBase( const std::wstring& uri );
// ========================================================================
std::string dbFileSuffix( const std::string& uri );
std::wstring dbFileSuffix( const std::wstring& uri );
// ========================================================================
std::string dbFileDir( const std::string& uri );
std::wstring dbFileDir( const std::wstring& uri );
// ========================================================================
std::string dbParentDir( const std::string& uri );
std::wstring dbParentDir( const std::wstring& uri );

