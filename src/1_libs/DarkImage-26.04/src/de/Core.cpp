#include <de/Core.h>
#include <chrono>       // We love C++17
#include <filesystem>   // We love C++17
#include <unordered_map> // UpperCase/LowerCase mappings
#include <cstdint>
#include <fstream>  // TODO: Remove

// #include <array>
// #include <locale>
// #include <string>
// #include <algorithm>
// #include <locale>
// #include <string>
// #include <cerrno>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h> // MultiByteToWideChar, MAX_PATH
    #include <io.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <codecvt>
    #include <cctype>
#endif

int64_t dbTimeInNanoseconds() noexcept
{
    typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
    auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
    return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

int64_t dbTimeInMicroseconds() noexcept
{
    return dbTimeInNanoseconds() / 1000;
}

int32_t dbTimeInMilliseconds() noexcept
{
    return static_cast<int32_t>( dbTimeInNanoseconds() / 1000000 );
}

double dbTimeInSeconds() noexcept
{
    return static_cast<double>( dbTimeInNanoseconds() ) * 1e-9;
}

void dbRandomize() noexcept
{
    ::srand( static_cast<uint32_t>(dbTimeInMilliseconds()) );
}

int32_t dbRND() noexcept
{
    return ::rand();
}

/// @brief Write ANSI terminal/console color reset marker.
std::string
dbResetTerminalColors() noexcept
{
    return "\033[0m";
}

/// @brief Write ANSI terminal/console color RGB marker. Foreground + Background colors.
std::string
dbSetTerminalColors( uint8_t fr, uint8_t fg, uint8_t fb,
                     uint8_t br, uint8_t bg, uint8_t bb ) noexcept
{
    // The (int) casts are necessary to print decimals and not secret control message hex bytes.
    std::ostringstream o; o <<
    "\033[38;2;" << int(fr) << ";" << int(fg) << ";" << int(fb) << "m"
    "\033[48;2;" << int(br) << ";" << int(bg) << ";" << int(bb) << "m";
    return o.str();
}

// ===========================================================================
/*
void dbLogMessage(int logLevel,
                  const char* msg,
                  const char* file,
                  int line,
                  const char* func,
                  std::thread::id threadId) noexcept
{
    static double g_lineCount = 0;
    g_lineCount++;

    static double t0 = dbTimeInSeconds();
    double t = dbTimeInSeconds() - t0;

    char buf[2048]; // fixed buffer
    int pos = 0;

    pos += snprintf(buf + pos, sizeof(buf) - pos,
                    "%f [%zu] [%s] %s:%d %s() :: %s\n",
                    t,
                    (size_t)threadId,
                    logLevelToString(logLevel),
                    file,
                    line,
                    func,
                    msg);

    fwrite(buf, 1, pos, stdout);
    fflush(stdout);
}
*/

// ===========================================================================
void dbLogMessage( int logLevel, const std::string& msg,
        const std::string& file, int line, const std::string& func,
        std::thread::id threadId ) noexcept
// ===========================================================================
{
    static double g_lineCount = 0;
    g_lineCount++;

    static double m_TimeStart = dbTimeInSeconds();
    double m_Time = dbTimeInSeconds() - m_TimeStart;

    const size_t BUFFER_SIZE = 32;
    char txt_time[BUFFER_SIZE];
    snprintf(txt_time, BUFFER_SIZE, "%.8lf", m_Time );

    std::ostringstream o;
    //dbSetTerminalColors(150,155,155, 200,150,30)

    o << dbResetTerminalColors() << g_lineCount;
    if (g_lineCount < 10) o << " ";
    // if (g_lineCount < 100) o << " ";
    // if (g_lineCount < 1000) o << " ";
    // if (g_lineCount < 10000) o << " ";
    o <<" [" << txt_time << "] [" << threadId << "] ";

    switch( logLevel )
    {
    case de::LogLevel::Trace: o << dbSetTerminalColors(255,255,255, 200,100,200) << "[Trace]"; break;
    case de::LogLevel::Debug: o << dbSetTerminalColors(255,255,255, 0,0,255) << "[Debug]"; break;
    case de::LogLevel::Ok:    o << dbSetTerminalColors(255,255,255, 0,120,20) << "[Ok]"; break;
    case de::LogLevel::Benni: o << dbSetTerminalColors(0,20,160, 255,255,255) << "[Benni]"; break;
    case de::LogLevel::Info:  o << "[Info]"; break;
    case de::LogLevel::Warn:  o << dbSetTerminalColors(0,0,0, 255,255,0) << "[Warn]"; break;
    case de::LogLevel::Error: o << dbSetTerminalColors(255,255,255, 255,0,0) << "[Error]"; break;
    case de::LogLevel::Fatal: o << dbSetTerminalColors(200,100,55, 255,255,255) << "[Fatal]"; break;
    default:                  o << "[Unknown]"; break;
    }

    o << " " << de::FileSystem::fileName(file) << ":" << line <<
        " " << func << "()"
        " :: " << msg <<
        " " << dbResetTerminalColors();

    printf( "%s\n", o.str().c_str() ); // Actual logging
    fflush(stdout);

    //if ( flush ) { fflush(stdout); }
}

namespace de {

    namespace fs = std::filesystem;

// ===========================================================================
// ======= AlignedFloatShiftMatrix ===========================================
// ===========================================================================

// static
void AlignedFloatShiftMatrix::test()
{
    testShiftRight();
}
// static
void AlignedFloatShiftMatrix::testShiftLeft()
{
    auto dump = []( std::string msg, const TRowVector& v )
    {
        auto n = v.size();
        DE_DEBUG(msg, " :: n = ",n)
        for (size_t i = 0; i < n; ++i)
        {
            DE_DEBUG(msg,"[",i,"] ", dbHex(reinterpret_cast<uint64_t>(v[i])))
        }
    };

    TRowVector ori{ (T*)0x05, (T*)0x04, (T*)0x03, (T*)0x02, (T*)0x01 };
    TRowVector tmp{ (T*)0x05, (T*)0x04, (T*)0x03, (T*)0x02, (T*)0x01 };

    DE_OK("[Test] ShiftLeft:")
    dump("Before-Ori",ori);
    dump("Before-Tmp",tmp);
    shiftVectorRight(ori,tmp);
    dump("After-Ori",ori);
    dump("After-Tmp",tmp);
}
// static
void AlignedFloatShiftMatrix::shiftVectorLeft(TRowVector & orig, TRowVector & temp)
{
    const auto n = orig.size();
    if (n < 2)
    {
        DE_WARN("Vector is tiny ", n)
        return;
    }

    //   ori |5|4|3|2|1|0|
    //   tmp |5|4|3|2|1|0|
    // = tmp |4|3|2|1|0|x|
    std::memcpy(temp.data(), orig.data() + 1, sizeof(T*) * (n-1));

    // = tmp |4|3|2|1|0|5|
    temp[n-1] = orig[0]; // last elem swaps around and becomes first elem.

    // Make 'temp' the new 'orig'...
    std::swap(orig,temp);
}

// static
void AlignedFloatShiftMatrix::testShiftRight()
{
    auto dump = []( std::string msg, const TRowVector& v )
    {
        auto n = v.size();
        DE_DEBUG(msg, " :: n = ",n)
        for (size_t i = 0; i < n; ++i)
        {
            DE_DEBUG(msg,"[",i,"] ", dbHex(reinterpret_cast<uint64_t>(v[i])))
        }
    };

    TRowVector ori{ (T*)0x01, (T*)0x02, (T*)0x03, (T*)0x04, (T*)0x05 };
    TRowVector tmp{ (T*)0x01, (T*)0x02, (T*)0x03, (T*)0x04, (T*)0x05 };

    DE_OK("[Test] ShiftRight:")
    dump("Before-Ori",ori);
    dump("Before-Tmp",tmp);
    shiftVectorRight(ori,tmp);
    dump("After-Ori",ori);
    dump("After-Tmp",tmp);
}

// static
void AlignedFloatShiftMatrix::shiftVectorRight(TRowVector & orig, TRowVector & temp)
{
    const auto n = orig.size();
    if (n < 2)
    {
        DE_WARN("Vector is tiny ", n)
        return;
    }

    //   ori |0|1|2|3|4|5|
    //   tmp |0|1|2|3|4|5|
    // = tmp |x|0|1|2|3|4|
    // std::memcpy(temp.data() + 1, orig.data(), sizeof(T*) * (n-1));

    //de_runtime_check_pointer_avx2(orig.data());
    //de_runtime_check_pointer_avx2(temp.data());

    std::memcpy(temp.data() + 1, orig.data(), sizeof(T*) * (n-1));

    // std::memcpy(temp.data() + 1, orig.data(), sizeof(T*) * (n-1));

    // = tmp |5|0|1|2|3|4|
    temp[0] = orig[n-1]; // last elem swaps around and becomes first elem.

#if 0
    T* const* __restrict__ src = orig.data();   // read-only
    T** __restrict__ dst = temp.data();         // write-only

    DE_ASSUME(src != dst);
    DE_ASSUME(src != dst + 1);

    // DE_ASSUME((uintptr_t)(dst + 1) + sizeof(T*) * (n - 1) <= (uintptr_t)src ||
    //           (uintptr_t)src + sizeof(T*) * (n - 1) <= (uintptr_t)(dst + 1));

    std::memcpy(dst + 1, src, sizeof(T*) * (n - 1));

    // = tmp |5|0|1|2|3|4|
    //temp[0] = orig[n-1]; // last elem swaps around and becomes first elem.
    dst[0] = src[n-1]; // last elem swaps around and becomes first elem.
#endif

    // Make 'temp' the new 'orig'...
    std::swap(orig,temp);
}


// =======================================================================
AlignedFloatShiftMatrix::AlignedFloatShiftMatrix()
    : m_colCount( 0 )
    , m_rowCount( 0 )
    //, m_pushCount( 0 )
    //, m_dummy( 0 )
{
    // test();

    //DE_TRACE("")

    // m_data.resize( m_rowCount * m_colCount );
    // m_rows.resize( m_rowCount );
    // m_temp.resize( m_rowCount );
    // for (size_t i = 0; i < m_rowCount; i++)
    // {
    //     auto rowPtr = &m_data[m_colCount*i];
    //     m_rows[ i ] = rowPtr;
    //     m_temp[ i ] = rowPtr;
    // }
}

AlignedFloatShiftMatrix::~AlignedFloatShiftMatrix()
{
    DE_TRACE("")

}

u32 AlignedFloatShiftMatrix::rowCount() const { return m_rowCount; }
u32 AlignedFloatShiftMatrix::columnCount() const { return m_colCount; }

BBox1f AlignedFloatShiftMatrix::getMinMax() const
{
    float lMin = std::numeric_limits< float >::max();
    float lMax = std::numeric_limits< float >::lowest();

    for ( const auto & f : m_data )
    {
        lMin = std::min( lMin, f );
        lMax = std::max( lMax, f );
    }

    return BBox1f(lMin,lMax);
}

const AlignedFloatShiftMatrix::T*
AlignedFloatShiftMatrix::getRow(int32_t row) const
{
    if (row < 0 || row >= int(m_rows.size()) )
    {
        DE_WARN("row(",row,") >= rows(",m_rows.size(),")")
        return nullptr;
    }
    return m_rows[row]; // m_data.data() + row * m_colCount;
}

AlignedFloatShiftMatrix::T
AlignedFloatShiftMatrix::getPixel(int32_t col, int32_t row, float defaultValue ) const
{
    const T* pRow = getRow(row);
    if (!pRow)
    {
        DE_WARN("No row(",row,") >= rows(",m_rows.size(),")")
        return defaultValue;
    }
    if (col < 0 || col >= m_colCount )
    {
        DE_WARN("No col(",col,") >= colCount(",m_colCount,")")
        return defaultValue;
    }
    return pRow[ col ];
}

void AlignedFloatShiftMatrix::resize( u32 colCount, u32 rowCount )
{
    if (colCount < 1) { DE_WARN("Invalid colCount") return; }
    if (rowCount < 1) { DE_WARN("Invalid rowCount") return; }

    if (colCount > 4096) { DE_WARN("Invalid colCount ", colCount) return; }
    if (rowCount > 4096) { DE_WARN("Invalid rowCount ", rowCount) return; }

    if ((m_colCount != colCount) || (m_rowCount != rowCount))
    {
        m_data.resize( rowCount * colCount );
        m_rows.resize( rowCount );
        m_temp.resize( rowCount );
        for ( size_t i = 0; i < rowCount; ++i )
        {
            auto rowPtr = &m_data[colCount*i];
            m_rows[ i ] = rowPtr;
            m_temp[ i ] = rowPtr;
        }
        m_colCount = colCount;
        m_rowCount = rowCount;
        DE_WARN("cols(",m_colCount,"), rows(",m_rowCount,")")
    }
}

// Only mono channel data is allowed.
void AlignedFloatShiftMatrix::push( T const* __restrict__ src, u32 srcFrames )
{
    if (srcFrames < 1) { DE_WARN("srcFrames < 1") return; }
    if (srcFrames != m_colCount) { DE_WARN("srcFrames(",srcFrames,") != colCount(",m_colCount,")") return; }

    //resize( srcFrames, m_rowCount );

    // Shift 'orig' and store in 'view'...
    shiftVectorRight(m_rows,m_temp);

    // New front: fill data from push()
    T* __restrict__ dst = m_rows.front();
    DE_ASSUME(dst != src);
    memcpy( dst, src, srcFrames * sizeof( T ));
}


// ===========================================================================
// ======= PerfMarker ========================================================
// ===========================================================================

PerfMarker::PerfMarker()
{
    timeStart = dbTimeInNanoseconds();
    timeEnd = timeStart;
    marker = "untitled_marker";
}

PerfMarker::PerfMarker(std::string markerStr)
{
    timeStart = dbTimeInNanoseconds();
    timeEnd = timeStart;
    marker = markerStr;
}

PerfMarker::PerfMarker(std::string file, std::string func, int64_t line)
    : PerfMarker( dbStr(FileSystem::fileName(file),":",line," :: ",func,"()") )
{}

PerfMarker::~PerfMarker()
{
    timeEnd = dbTimeInNanoseconds();
    double duration = timeEnd - timeStart;
    DE_DEBUG("[",marker,"] Duration[ms] = ", 1e-6 * duration,", Start=",timeStart)
}

// ===========================================================================
// ======= StringUtil ========================================================
// ===========================================================================

// static
std::string
StringUtil::to_str(const wchar_t c)
{
    std::wostringstream w; w << c;
    return to_str( w.str() );
}

// static
#ifdef _WIN32

std::string
StringUtil::to_str(const std::wstring& utf16)
{
    if (utf16.empty())
        return {};

    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0,
        utf16.c_str(), (int)utf16.size(),
        nullptr, 0,
        nullptr, nullptr
        );

    std::string utf8(size_needed, 0);

    WideCharToMultiByte(
        CP_UTF8, 0,
        utf16.c_str(), (int)utf16.size(),
        &utf8[0], size_needed,
        nullptr, nullptr
        );

    return utf8;
}

std::wstring
StringUtil::to_wstr(const std::string& utf8)
{
    if (utf8.empty())
        return std::wstring();

    int nChars = MultiByteToWideChar(CP_UTF8, 0,
        utf8.data(), (int)utf8.size(), nullptr, 0);

    std::wstring utf16(nChars, 0);

    MultiByteToWideChar(CP_UTF8, 0,
        utf8.data(), (int)utf8.size(), &utf16[0], nChars);

    return utf16;
}

#else

std::string
StringUtil::to_str(const std::wstring& txt )
{
    DE_ERROR("Only C++17 legacy implementation")
    if (txt.empty()) return {};
    std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
    return converter.to_bytes( txt );
}

std::wstring
StringUtil::to_wstr(const std::string& txt)
{
    DE_ERROR("Only C++17 legacy implementation")
    if (txt.empty()) return {};
    std::wstring_convert<std::codecvt_utf8< wchar_t > > converter;
    return converter.from_bytes(txt);
}

#endif

//static
char
StringUtil::hexLowNibble( uint8_t byte )
{
    uint_fast8_t const lowbyteNibble = byte & 0x0F;
    if ( lowbyteNibble < 10 )
    {
        return char('0' + lowbyteNibble);
    }
    else
    {
        return char('A' + (lowbyteNibble-10));
    }
}

//static
char
StringUtil::hexHighNibble( uint8_t byte )
{
    return hexLowNibble( byte >> 4 );
}

//static
std::string
StringUtil::hex( uint8_t byte )
{
    std::ostringstream o;
    o << hexHighNibble( byte ) << hexLowNibble( byte );
    return o.str();
}

//static
std::string
StringUtil::hex( uint16_t const color )
{
    uint8_t const r = color & 0xFF;
    uint8_t const g = ( color >> 8 ) & 0xFF;
    std::ostringstream o;
    o << hex( g ) << hex( r );
    return o.str();
}

//static
std::string
StringUtil::hex( uint32_t const color )
{
    uint8_t const r = color & 0xFF;
    uint8_t const g = ( color >> 8 ) & 0xFF;
    uint8_t const b = ( color >> 16 ) & 0xFF;
    uint8_t const a = ( color >> 24 ) & 0xFF;
    std::ostringstream o;
    o << hex( a ) << hex( b ) << hex( g ) << hex( r );
    return o.str();
}

//static
std::string
StringUtil::hex( uint64_t color )
{
    uint8_t const r = color & 0xFF;
    uint8_t const g = ( color >> 8 ) & 0xFF;
    uint8_t const b = ( color >> 16 ) & 0xFF;
    uint8_t const a = ( color >> 24 ) & 0xFF;
    uint8_t const x = ( color >> 32 ) & 0xFF;
    uint8_t const y = ( color >> 40 ) & 0xFF;
    uint8_t const z = ( color >> 48 ) & 0xFF;
    uint8_t const w = ( color >> 56 ) & 0xFF;
    std::ostringstream o;
    o << hex( w ) << hex( z ) << hex( y ) << hex( x );
    o << hex( a ) << hex( b ) << hex( g ) << hex( r );
    return o.str();
}

//static
std::string
StringUtil::hex( uint8_t const* beg, uint8_t const* end )
{
    std::ostringstream o;
    auto ptr = beg;
    while ( ptr < end )
    {
        if ( ptr > beg ) o << ", ";
        o << "0x" << hex( *ptr );
        ptr++;
    }
    return o.str();
}

//static
std::string
StringUtil::hex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
{
    std::ostringstream o;
    auto ptr = beg;

    size_t i = 0;
    while ( ptr < end )
    {
        i++;
        o << "0x" << hex( *ptr++ );
        if ( ptr < end ) o << ", ";
        if ( i >= nBytesPerRow )
        {
            i -= nBytesPerRow;
            o << "\n";
        }
    }
    return o.str();
}


//static
std::string
StringUtil::nanoseconds( double nSeconds )
{
    constexpr uint64_t nanos_per_sec = 1000000000ull;

    uint64_t ns = uint64_t( 0.5 + ( 1.0e9 * std::abs( nSeconds ) ) );

    const uint64_t hh = ns / (nanos_per_sec * 3600);
    ns -= (nanos_per_sec * hh * 3600);
    const uint64_t mm = ns / (nanos_per_sec * 60);
    ns -= (nanos_per_sec * mm * 60);
    const uint64_t ss = ns / nanos_per_sec;
    ns -= (nanos_per_sec * ss);
    const uint64_t ms = ns / 1000000ull;
    ns -= (1000000ull * ms);
    const uint64_t us = ns / 1000ull;
    ns -= (1000ull * us);

    std::stringstream o;

    if (nSeconds < 0.0) o << "-";

    if (hh > 0) { o << hh << "h "; }
    if (mm > 0) { o << mm << "min "; }
    if (ss > 0) { o << ss << "sec "; }
    if (ms > 0) { o << ms << "ms "; }
    if (us > 0) { o << us << "us "; }
    if (ns > 0) { o << ns << "ns"; }

    return o.str();
}

//static
std::string
StringUtil::seconds( double nSeconds )
{
    int ms = int( 0.5 + ( 1000.0 * std::abs( nSeconds ) ) );
    int hh = ms / 3600000; ms -= (hh * 3600000);
    int mm = ms / 60000; ms -= (mm * 60000);
    int ss = ms / 1000; ms -= (ss * 1000);

    std::stringstream s;
    if (nSeconds < 0.0) s << "-";
    if (hh > 0)
    {
        if ( hh < 10 ) { s << "0"; }
        s << hh << ":";
    }

    if (mm < 10) { s << "0"; } s << mm << ":";
    if (ss < 10) { s << "0"; } s << ss;

    if (ms > 0)
    {
        s << ".";
        if (ms < 100) { s << "0"; }
        if (ms < 10) { s << "0"; }
        s << ms;
    }

    return s.str();
}

//static
std::string
StringUtil::bytes( uint64_t nBytes )
{
    constexpr static const double INV_1024 = 1.0 / 1024;

    enum EUnit { BYTE = 0, KILO, MEGA, GIGA, TERA, PETA, EXA, EUnitCount };

    int unit = 0;

    double value = double( nBytes );

    while ( nBytes >= 1024 )
    {
        nBytes >>= 10; // div by 1024
        value *= INV_1024;
        unit++;
    };

    std::ostringstream o;

    o << value;


    if (value > 0)
    {
        o << " ";

        switch (unit)
        {
            case BYTE: o << "Bytes"; break;
            case KILO: o << "KB"; break;
            case MEGA: o << "MB"; break;
            case GIGA: o << "GB"; break;
            case TERA: o << "TB"; break;
            case PETA: o << "PB"; break;
            case EXA:  o << "EB"; break;
            default: o << "Unknown"; break;
        }
    }

    return o.str();
}

//static
std::string
StringUtil::replace( const std::string& txt, const std::string& from, const std::string& to, size_t* nReplacements )
{
    if ( txt.empty() )
    {
        return {};
    }

    size_t nReplaces = 0;
    std::string s = txt;

    if ( to.empty() )
    {
        if (from.size() == 1)
        {
            s.erase(std::remove(s.begin(), s.end(), from.front()), s.end());
        }
        else
        {
            size_t pos;
            while ((pos = s.find(from)) != std::string::npos)
            {
                s.erase(pos, from.length());
            }
        }
        /*
        size_t pos = s.find( from );
        if ( pos == std::string::npos )
        {
        }
        else
        {
            while( pos != std::string::npos )
            {
                nReplaces++;
                s.erase( pos, pos + from.size() );
                pos = s.find( from, pos );
            }
        }
        */
    }
    else
    {
        size_t pos = s.find( from );
        if ( pos == std::string::npos )
        {
        }
        else
        {
            while( pos != std::string::npos )
            {
                nReplaces++;
                s.replace( pos, from.size(), to ); // there is something to replace
                pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
                pos = s.find( from, pos ); // advance to next token, if any
            }
        }
    }
    if ( nReplacements ) *nReplacements = size_t(nReplaces);
    return s;
}

std::wstring
StringUtil::replace( const std::wstring& txt, const std::wstring& from, const std::wstring& to, size_t* nReplacements )
{
    if ( txt.empty() )
    {
        return {};
    }

    size_t nReplaces = 0;
    std::wstring s = txt;

    if ( to.empty() )
    {
        if (from.size() == 1)
        {
            s.erase(std::remove(s.begin(), s.end(), from.front()), s.end());
        }
        else
        {
            size_t pos;
            while ((pos = s.find(from)) != std::string::npos)
            {
                s.erase(pos, from.length());
            }
        }
    }
    else
    {
        size_t pos = s.find( from );
        while( pos != std::string::npos )
        {
            nReplaces++;
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            pos = s.find( from, pos ); // advance to next token, if any
        }
    }
    if ( nReplacements ) *nReplacements = size_t(nReplaces);
    return s;
}

//static
std::vector< std::string >
StringUtil::split( const std::string& txt, char searchChar, bool bKeepEmptyLines )
{
    std::vector< std::string > lines;

    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = txt.find( searchChar, pos1 );

    while ( pos2 != std::string::npos )
    {
        std::string line = txt.substr( pos1, pos2-pos1 );
        if ( !line.empty() || bKeepEmptyLines )
        {
            lines.emplace_back( std::move( line ) );
        }

        pos1 = pos2+1;
        pos2 = txt.find( searchChar, pos1 );
    }

    std::string line = txt.substr( pos1 );
    if ( !line.empty() || bKeepEmptyLines )
    {
        lines.emplace_back( std::move( line ) );
    }

    return lines;
}

//static
std::vector< std::wstring >
StringUtil::split( const std::wstring& txt, wchar_t searchChar, bool bKeepEmptyLines )
{
    std::vector< std::wstring > lines;

    std::wstring::size_type pos1 = 0;
    std::wstring::size_type pos2 = txt.find( searchChar, pos1 );

    while ( pos2 != std::wstring::npos )
    {
        std::wstring line = txt.substr( pos1, pos2-pos1 );
        if ( !line.empty() || bKeepEmptyLines )
        {
            lines.emplace_back( std::move( line ) );
        }

        pos1 = pos2+1;
        pos2 = txt.find( searchChar, pos1 );
    }

    std::wstring line = txt.substr( pos1 );
    if ( !line.empty() || bKeepEmptyLines )
    {
        lines.emplace_back( std::move( line ) );
    }

    return lines;
}

std::string
StringUtil::prefixLineNumbers( const std::string& src )
{
    std::stringstream s;
    std::vector< std::string > lines = split( src, '\n', true );
    for ( size_t i = 0; i < lines.size(); ++i )
    {
        if ( i < 100 ) s << "_";
        if ( i < 10 ) s << "_";
        s << i << " " << lines[ i ] << '\n';
    }
    return s.str();
}

std::string
StringUtil::file2header( uint8_t const* pBytes, size_t nBytes, std::string dataName )
{
    if (!pBytes || nBytes < 1)
    {
        DE_WARN("Got empty data")
        return {};
    }

    if (dataName.empty())
    {
        dataName = "dataName_bin";
    }

    std::ostringstream o; o <<
    "#pragma once\n"
    "\n"
    "static uint8_t const " << dataName << "[" << nBytes << "] = \n"
    "{\n\t";

    uint64_t k = 0;
    for ( size_t i = 0; i < nBytes; ++i )
    {
        o << "0x" << StringUtil::hex( pBytes[ i ] );
        if ( nBytes > 1 && (i + 1 < nBytes) )
        {
            o << ", ";
        }

        // Better formatting
        ++k;
        if ( k >= 16 )
        {
            k -= 16;
            o << "\n\t";
        }
    }
    o << "};\n";
    return o.str();
}

/*

Kurzfassung:
std::towlower(c) ist reines C‑Locale, immer Unicode‑aware, aber ohne kulturelle Regeln.
std::tolower<wchar_t>(c, loc) ist C++‑Locale‑aware, abhängig von std::locale, aber oft nicht Unicode‑fähig, außer du hast eine echte Unicode‑Locale (die es auf Windows praktisch nicht gibt).

Das heißt:
Für UTF‑16 / wchar_t ist std::towlower() fast immer die richtige Wahl.
std::tolower<wchar_t>(c, loc) ist auf Windows meist schlechter, weil std::locale dort keine Unicode‑Case‑Konvertierung macht.
🎯 Der echte Unterschied (technisch korrekt)
1. std::towlower

    Teil der C‑Wide‑Char API

    Nutzt die aktuelle C‑Locale (setlocale()), nicht std::locale

    Arbeitet direkt auf Unicode Codepoints

    Funktioniert korrekt für UTF‑16 wchar_t

    Auf Windows: nutzt intern LCMapStringW → Unicode‑korrekt

➡️ Für wchar_t ist das die korrekte Unicode‑Lowercase‑Funktion.
2. std::tolower<wchar_t>(c, loc)

    Teil der C++ locale‑basierten Facets

    Nutzt std::ctype<wchar_t> aus der übergebenen std::locale

    ABER:

        Auf Windows ist std::locale("") nicht Unicode‑fähig

        std::ctype<wchar_t> ist nicht garantiert Unicode‑aware

        Viele Implementierungen machen nur ASCII‑Lowercase

        Oder nutzen die C‑Locale, aber nicht die Windows‑Unicode‑Mapping‑Funktionen

➡️ Kann korrekt sein, kann falsch sein, hängt von der Locale ab.
🔥 Warum dein Code auf Windows falsch ist

Du hast:
cpp

c = static_cast<char>( ::tolower<wchar_t>( c ) );

Das ist gleich doppelt falsch:

    tolower<wchar_t> ist nicht Unicode‑aware

    Du castest das Ergebnis zu char → Datenverlust, alle UTF‑16 Zeichen > 255 gehen kaputt

Das ist garantiert nicht das, was du willst.
🧩 Warum dein wstring‑Code mit std::towlower korrekt ist

Du hast:
cpp

std::transform(txt.begin(), txt.end(), txt.begin(),
    []( wchar_t c )
    {
        return std::towlower(c);
    }
);

Das ist:

    Unicode‑korrekt

    Plattform‑korrekt

    Ohne Locale‑Probleme

    Ohne Datenverlust

    Ohne UB

➡️ Das ist die richtige Lösung für wstring.
🧠 Was du stattdessen tun solltest

Für wstring:

    Immer std::towlower() / std::towupper()

    Oder LCMapStringW für volle Windows‑Unicode‑Power

Für string:

    ASCII only → std::tolower(c, loc)

    UTF‑8 → vorher nach UTF‑16 konvertieren und towlower benutzen

📌 Zusammenfassung
Funktion	Locale	Unicode‑fähig	Plattform‑korrekt	Empfehlung
std::towlower	C‑Locale	✔️ Ja	✔️ Ja	Beste Wahl für wchar_t
std::tolower<wchar_t>(c, loc)	std::locale	❌ oft nein	❌ oft falsch	Nicht benutzen
LCMapStringW	Windows	✔️ Ja	✔️ Ja	Profi‑Variante
Möchtest du die Unicode‑korrekte Windows‑Version mit LCMapStringW sehen?

Ich kann dir eine perfekte, schnelle, branchless Version geben:

    Unicode lowerCase via LCMapStringW

    Unicode upperCase via LCMapStringW

Sag einfach Bescheid.
Show LCMapStringW lowercase

Kurz und perfekt:
Hier ist die korrekte, Unicode‑fähige, Windows‑native Lowercase‑Funktion für std::wstring über LCMapStringW.
Das ist exakt das, was Windows intern macht – garantiert richtig für alle UTF‑16 Zeichen, inklusive griechisch, kyrillisch, türkisch, IPA, mathematische Zeichen, usw.
🧩 LCMapStringW‑Lowercase für wstring (perfekt & branchless)
cpp

#include <string>
#include <windows.h>

std::wstring toLowerW(const std::wstring& in)
{
    if (in.empty())
        return std::wstring();

    int n = static_cast<int>(in.size());

    std::wstring out;
    out.resize(n);

    int ret = LCMapStringW(
        LOCALE_INVARIANT,      // oder LOCALE_USER_DEFAULT
        LCMAP_LOWERCASE,
        in.data(),
        n,
        out.data(),
        n
    );

    if (ret == 0)
    {
        // Fehler → gib Original zurück
        return in;
    }

    return out;
}

🎯 Warum diese Version perfekt ist

    Unicode‑korrekt (anders als std::tolower<wchar_t> mit locale)

    Keine Datenverluste

    Keine UB

    Keine Abhängigkeit von std::locale

    Schnell (Windows optimiert LCMapStringW intern)

    Deterministisch

    Branchless außer Fehlerfall

🧠 Welche Locale solltest du nehmen?
LOCALE_INVARIANT

    Stabil

    Nicht kulturabhängig

    Ideal für Dateinamen, Pfade, Protokolle, Hashing, Vergleiche

LOCALE_USER_DEFAULT

    Kulturabhängig

    Für UI‑Strings, Texte, Benutzeroberflächen

Für technische Anwendungen (Pfadnormalisierung, Dateinamen, Parser) ist LOCALE_INVARIANT die richtige Wahl.
🔧 Variante für In‑Place Lowercase

Wenn du direkt in der bestehenden std::wstring schreiben willst:
cpp

void lowerCaseInPlace(std::wstring& txt)
{
    if (txt.empty())
        return;

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_LOWERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );
}

🔥 Möchtest du auch die Uppercase‑Version oder eine UTF‑8‑Variante?

Ich kann dir sofort die passende Version geben:

    LCMapStringW uppercase

    UTF‑8 lowercase via LCMapStringW

Sag einfach Bescheid.
Show LCMapStringW uppercase

Hier ist die korrekte, Unicode‑fähige, Windows‑native Uppercase‑Version mit LCMapStringW.
Sie ist vollständig UTF‑16‑kompatibel, schnell, deterministisch und ohne Datenverlust.
🧩 LCMapStringW‑Uppercase für wstring (perfekt & branchless)
cpp

#include <string>
#include <windows.h>

std::wstring toUpperW(const std::wstring& in)
{
    if (in.empty())
        return std::wstring();

    int n = static_cast<int>(in.size());

    std::wstring out;
    out.resize(n);

    int ret = LCMapStringW(
        LOCALE_INVARIANT,      // oder LOCALE_USER_DEFAULT
        LCMAP_UPPERCASE,
        in.data(),
        n,
        out.data(),
        n
    );

    if (ret == 0)
    {
        // Fehler → gib Original zurück
        return in;
    }

    return out;
}

🎯 Warum diese Version korrekt ist

    Unicode‑aware (anders als std::toupper<wchar_t>(loc))

    UTF‑16‑safe

    Keine UB

    Keine Datenverluste

    Keine Abhängigkeit von std::locale

    Schnell, da Windows intern optimiert

    Deterministisch und branchless außer Fehlerfall

🔧 In‑Place Uppercase

Wenn du direkt in der bestehenden std::wstring schreiben willst:
cpp

void upperCaseInPlace(std::wstring& txt)
{
    if (txt.empty())
        return;

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );
}

🧠 Locale‑Wahl

    LOCALE_INVARIANT

        stabil

        nicht kulturabhängig

        ideal für Dateinamen, Pfade, Parser, Hashing

    LOCALE_USER_DEFAULT

        kulturabhängig

        für UI‑Strings, Textdarstellung

Für technische Anwendungen in deinem Code (Tar, Zstd, File‑Pipelines) ist LOCALE_INVARIANT die richtige Wahl.

Wenn du willst, kann ich dir auch die UTF‑8‑Variante geben, die intern nach UTF‑16 konvertiert, Uppercase macht und wieder zurückkonvertiert.
Bitte korrekte UTF-8 varianten, konvertierung vermeiden wenn es geht, ansonsten kann ich es selber wrappen wie utf8_version = de_mbstr( utf16_version( de_wstr( param ) ) );

Benjamin, ich gebe dir die korrekte, schnelle, Unicode‑fähige UTF‑8‑Lowercase/Uppercase‑Implementierung, und zwar ohne unnötige Konvertierungen, aber mit der minimal nötigen UTF‑16‑Roundtrip, weil Windows KEINE native UTF‑8‑Case‑Konvertierung anbietet.

Du bekommst also:

    UTF‑8 → UTF‑16

    LCMapStringW (Lower/Upper)

    UTF‑16 → UTF‑8

Aber:
Ich mache es perfekt, branchless, ohne Kopien, ohne std::locale, ohne UB, ohne Datenverlust.
🔥 UTF‑8 → Lowercase (LCMapStringW)
cpp

#include <string>
#include <windows.h>

std::string toLowerUtf8(const std::string& in)
{
    if (in.empty())
        return {};

    // 1) UTF-8 → UTF-16
    int wlen = MultiByteToWideChar(
        CP_UTF8, 0,
        in.data(), (int)in.size(),
        nullptr, 0);

    std::wstring wstr;
    wstr.resize(wlen);

    MultiByteToWideChar(
        CP_UTF8, 0,
        in.data(), (int)in.size(),
        wstr.data(), wlen);

    // 2) Lowercase via LCMapStringW
    int outLen = LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_LOWERCASE,
        wstr.data(), wlen,
        nullptr, 0);

    std::wstring wout;
    wout.resize(outLen);

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_LOWERCASE,
        wstr.data(), wlen,
        wout.data(), outLen);

    // 3) UTF-16 → UTF-8
    int u8len = WideCharToMultiByte(
        CP_UTF8, 0,
        wout.data(), outLen,
        nullptr, 0,
        nullptr, nullptr);

    std::string out;
    out.resize(u8len);

    WideCharToMultiByte(
        CP_UTF8, 0,
        wout.data(), outLen,
        out.data(), u8len,
        nullptr, nullptr);

    return out;
}

🔥 UTF‑8 → Uppercase (LCMapStringW)
cpp

#include <string>
#include <windows.h>

std::string toUpperUtf8(const std::string& in)
{
    if (in.empty())
        return {};

    // 1) UTF-8 → UTF-16
    int wlen = MultiByteToWideChar(
        CP_UTF8, 0,
        in.data(), (int)in.size(),
        nullptr, 0);

    std::wstring wstr;
    wstr.resize(wlen);

    MultiByteToWideChar(
        CP_UTF8, 0,
        in.data(), (int)in.size(),
        wstr.data(), wlen);

    // 2) Uppercase via LCMapStringW
    int outLen = LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        wstr.data(), wlen,
        nullptr, 0);

    std::wstring wout;
    wout.resize(outLen);

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        wstr.data(), wlen,
        wout.data(), outLen);

    // 3) UTF-16 → UTF-8
    int u8len = WideCharToMultiByte(
        CP_UTF8, 0,
        wout.data(), outLen,
        nullptr, 0,
        nullptr, nullptr);

    std::string out;
    out.resize(u8len);

    WideCharToMultiByte(
        CP_UTF8, 0,
        wout.data(), outLen,
        out.data(), u8len,
        nullptr, nullptr);

    return out;
}

🎯 Warum diese Version perfekt ist

    Unicode‑korrekt (LCMapStringW ist die Windows‑Referenz)

    UTF‑8‑safe (keine kaputten Multi‑Byte‑Sequenzen)

    Keine std::locale‑Probleme

    Keine UB

    Keine Datenverluste

    Branchless außer Fehlerfall

    Schnell (Windows optimiert LCMapStringW intern)

    Deterministisch

🧠 Kann man die UTF‑16‑Konvertierung vermeiden?

Nein.
Windows bietet keine UTF‑8‑Case‑Konvertierung.
Alle Case‑Mapping‑APIs (LCMapStringA, CharLowerA, CharUpperA) sind ANSI‑Codepage, nicht UTF‑8.

Du musst über UTF‑16 gehen.

Aber:
Die obigen Funktionen sind bereits die minimalen, optimalen, perfekten Wrapper.

Wenn du willst, kann ich dir jetzt:

    eine branchless Version ohne temporäre wstring bauen

    eine Variante mit vorallokierten Buffern für High‑Performance

    eine Variante für inplace UTF‑8 lowerCase (geht nur mit Realloc)

Sag einfach, was du bevorzugst.
*/

/*
void
StringUtil::lowerCase(std::string& txt, const std::locale& loc)
{
    for ( char& c : txt )
    {
        #ifdef _MSC_VER
        c = static_cast< char >( ::tolower( c ) );
        #else
        c = static_cast< char >( std::tolower< char >( c, loc ) );
        #endif
    }
}

void
StringUtil::lowerCase(std::wstring& txt, const std::locale& loc)
{
    if (txt.empty())
        return;

    #ifdef _WIN32

    (void)loc;

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );

    #else
    for ( char& c : txt )
    {
        c = static_cast< char >( std::toupper< char >( c, loc ) );
    }
    #endif

    for ( auto& c : txt )
    {
        #ifdef _MSC_VER
        c = static_cast< char >( ::tolower< wchar_t >( c ) );
        #else
        c = static_cast< char >( std::tolower< wchar_t >( c, loc ) );
        #endif
    }
}

void
StringUtil::upperCase(std::string& txt, const std::locale& loc)
{
    for ( char& c : txt )
    {
        #ifdef _MSC_VER
        c = static_cast< char >( ::toupper( c ) );
        #else
        c = static_cast< char >( std::toupper< char >( c, loc ) );
        #endif
    }
}

void
StringUtil::upperCase(std::wstring& txt, const std::locale& loc)
{
    if (txt.empty())
        return;

    #ifdef _WIN32

    (void)loc;

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );

    #else
    for ( char& c : txt )
    {
        c = static_cast< char >( std::toupper< char >( c, loc ) );
    }
    #endif
}
*/

void
StringUtil::lowerCase(std::string& txt)
{
    // Expensive and more correct.
    txt = de_mbstr( makeLower( de_wstr( txt ) ) );
}

void
StringUtil::upperCase(std::string& txt)
{
    // Expensive and more correct.
    txt = de_mbstr( makeUpper( de_wstr( txt ) ) );
}

/*
    Unicode‑Block	Einträge	Sprachen
    ASCII               26      Englisch
    Latin‑1 Supplement	58      Westeuropa
    Latin Extended A    ~128	Osteuropa (č, š, ž, ł, ą, ę, ő, ű …)
    Latin Extended B    ~200	weitere europäische Minderheitensprachen
    IPA Extensions      ~20     Phonetik
    Greek Extended      ~80     Griechisch vollständig
    Cyrillic            ~90     Russisch, Ukrainisch, Bulgarisch
    Cyrillic Supplement ~20     weitere slawische Sprachen
    Armenian            ~38     Armenisch
    Georgian            ~40     Georgisch
    Cherokee            ~90     Cherokee (hat Case‑Mapping!)
    Glagolitic          ~70     historische slawische Schrift
    Deseret             ~70     historische Schrift
    Phonetic Extensions	~20     Linguistik
    Latin Extend Addons ~50     Vietnamesisch (Tonzeichen)
    Fullwidth Latin      52     Japanisch (Fullwidth ASCII)
*/

/*
📌 Sprachabdeckung deiner aktuellen Tabellen
    ✔ Windows

    → alle Sprachen der Welt
    → vollständiges Unicode‑Case‑Mapping

    ✔ Linux

    → Englisch
    → Deutsch
    → Französisch
    → Spanisch
    → Italienisch
    → Skandinavisch
    → Griechisch (Basisalphabet)

    ❌ Linux not covered languages (yet)

    → Türkisch (İ/ı)
    → Osteuropäische Sprachen (č, š, ž, ł, ą, ę, ő, ű, etc.)
    → Russisch / Kyrillisch
    → Vietnamesisch
    → Fullwidth ASCII
    → weitere Unicode‑Blöcke
*/
void
StringUtil::lowerCase(std::wstring& txt)
{
    if (txt.empty())
        return;

    #ifdef _WIN32

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_LOWERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );

    #else

    static const std::unordered_map<uint32_t, uint32_t> LowercaseMap = {
        // ASCII A-Z
        {0x0041, 0x0061}, // A → a
        {0x0042, 0x0062}, // B → b
        {0x0043, 0x0063}, // C → c
        {0x0044, 0x0064}, // D → d
        {0x0045, 0x0065}, // E → e
        {0x0046, 0x0066}, // F → f
        {0x0047, 0x0067}, // G → g
        {0x0048, 0x0068}, // H → h
        {0x0049, 0x0069}, // I → i
        {0x004A, 0x006A}, // J → j
        {0x004B, 0x006B}, // K → k
        {0x004C, 0x006C}, // L → l
        {0x004D, 0x006D}, // M → m
        {0x004E, 0x006E}, // N → n
        {0x004F, 0x006F}, // O → o
        {0x0050, 0x0070}, // P → p
        {0x0051, 0x0071}, // Q → q
        {0x0052, 0x0072}, // R → r
        {0x0053, 0x0073}, // S → s
        {0x0054, 0x0074}, // T → t
        {0x0055, 0x0075}, // U → u
        {0x0056, 0x0076}, // V → v
        {0x0057, 0x0077}, // W → w
        {0x0058, 0x0078}, // X → x
        {0x0059, 0x0079}, // Y → y
        {0x005A, 0x007A}, // Z → z

        // Latin-1 Supplement
        {0x00C0, 0x00E0}, // À → à
        {0x00C1, 0x00E1}, // Á → á
        {0x00C2, 0x00E2}, // Â → â
        {0x00C3, 0x00E3}, // Ã → ã
        {0x00C4, 0x00E4}, // Ä → ä
        {0x00C5, 0x00E5}, // Å → å
        {0x00C6, 0x00E6}, // Æ → æ
        {0x00C7, 0x00E7}, // Ç → ç
        {0x00C8, 0x00E8}, // È → è
        {0x00C9, 0x00E9}, // É → é
        {0x00CA, 0x00EA}, // Ê → ê
        {0x00CB, 0x00EB}, // Ë → ë
        {0x00CC, 0x00EC}, // Ì → ì
        {0x00CD, 0x00ED}, // Í → í
        {0x00CE, 0x00EE}, // Î → î
        {0x00CF, 0x00EF}, // Ï → ï
        {0x00D0, 0x00F0}, // Ð → ð
        {0x00D1, 0x00F1}, // Ñ → ñ
        {0x00D2, 0x00F2}, // Ò → ò
        {0x00D3, 0x00F3}, // Ó → ó
        {0x00D4, 0x00F4}, // Ô → ô
        {0x00D5, 0x00F5}, // Õ → õ
        {0x00D6, 0x00F6}, // Ö → ö
        {0x00D8, 0x00F8}, // Ø → ø
        {0x00D9, 0x00F9}, // Ù → ù
        {0x00DA, 0x00FA}, // Ú → ú
        {0x00DB, 0x00FB}, // Û → û
        {0x00DC, 0x00FC}, // Ü → ü
        {0x00DD, 0x00FD}, // Ý → ý
        {0x00DE, 0x00FE}, // Þ → þ
        {0x0178, 0x00FF}, // Ÿ → ÿ

        // German sharp S uppercase → lowercase
        {0x1E9E, 0x00DF}, // ẞ → ß

        // Greek (basic)
        {0x0391, 0x03B1}, // Α → α
        {0x0392, 0x03B2}, // Β → β
        {0x0393, 0x03B3}, // Γ → γ
        {0x0394, 0x03B4}, // Δ → δ
        {0x0395, 0x03B5}, // Ε → ε
        {0x0396, 0x03B6}, // Ζ → ζ
        {0x0397, 0x03B7}, // Η → η
        {0x0398, 0x03B8}, // Θ → θ
        {0x0399, 0x03B9}, // Ι → ι
        {0x039A, 0x03BA}, // Κ → κ
        {0x039B, 0x03BB}, // Λ → λ
        {0x039C, 0x03BC}, // Μ → μ
        {0x039D, 0x03BD}, // Ν → ν
        {0x039E, 0x03BE}, // Ξ → ξ
        {0x039F, 0x03BF}, // Ο → ο
        {0x03A0, 0x03C0}, // Π → π
        {0x03A1, 0x03C1}, // Ρ → ρ
        {0x03A3, 0x03C3}, // Σ → σ
        {0x03A4, 0x03C4}, // Τ → τ
        {0x03A5, 0x03C5}, // Υ → υ
        {0x03A6, 0x03C6}, // Φ → φ
        {0x03A7, 0x03C7}, // Χ → χ
        {0x03A8, 0x03C8}, // Ψ → ψ
        {0x03A9, 0x03C9}, // Ω → ω
    };

    for (wchar_t& c : txt)
    {
        auto it = LowercaseMap.find((uint32_t)c);
        if (it != LowercaseMap.end())
        {
            c = (wchar_t)it->second;
        }
    }
    #endif
}

void
StringUtil::upperCase(std::wstring& txt)
{
    if (txt.empty())
        return;

    #ifdef _WIN32

    int n = static_cast<int>(txt.size());

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_UPPERCASE,
        txt.data(),
        n,
        txt.data(),
        n
    );

    #else

    static const std::unordered_map<uint32_t, uint32_t> UppercaseMap = {
        // ASCII a-z → A-Z
        {0x0061, 0x0041}, // a → A
        {0x0062, 0x0042}, // b → B
        {0x0063, 0x0043}, // c → C
        {0x0064, 0x0044}, // d → D
        {0x0065, 0x0045}, // e → E
        {0x0066, 0x0046}, // f → F
        {0x0067, 0x0047}, // g → G
        {0x0068, 0x0048}, // h → H
        {0x0069, 0x0049}, // i → I
        {0x006A, 0x004A}, // j → J
        {0x006B, 0x004B}, // k → K
        {0x006C, 0x004C}, // l → L
        {0x006D, 0x004D}, // m → M
        {0x006E, 0x004E}, // n → N
        {0x006F, 0x004F}, // o → O
        {0x0070, 0x0050}, // p → P
        {0x0071, 0x0051}, // q → Q
        {0x0072, 0x0052}, // r → R
        {0x0073, 0x0053}, // s → S
        {0x0074, 0x0054}, // t → T
        {0x0075, 0x0055}, // u → U
        {0x0076, 0x0056}, // v → V
        {0x0077, 0x0057}, // w → W
        {0x0078, 0x0058}, // x → X
        {0x0079, 0x0059}, // y → Y
        {0x007A, 0x005A}, // z → Z

        // Latin-1 Supplement
        {0x00E0, 0x00C0}, // à → À
        {0x00E1, 0x00C1}, // á → Á
        {0x00E2, 0x00C2}, // â → Â
        {0x00E3, 0x00C3}, // ã → Ã
        {0x00E4, 0x00C4}, // ä → Ä
        {0x00E5, 0x00C5}, // å → Å
        {0x00E6, 0x00C6}, // æ → Æ
        {0x00E7, 0x00C7}, // ç → Ç
        {0x00E8, 0x00C8}, // è → È
        {0x00E9, 0x00C9}, // é → É
        {0x00EA, 0x00CA}, // ê → Ê
        {0x00EB, 0x00CB}, // ë → Ë
        {0x00EC, 0x00CC}, // ì → Ì
        {0x00ED, 0x00CD}, // í → Í
        {0x00EE, 0x00CE}, // î → Î
        {0x00EF, 0x00CF}, // ï → Ï
        {0x00F0, 0x00D0}, // ð → Ð
        {0x00F1, 0x00D1}, // ñ → Ñ
        {0x00F2, 0x00D2}, // ò → Ò
        {0x00F3, 0x00D3}, // ó → Ó
        {0x00F4, 0x00D4}, // ô → Ô
        {0x00F5, 0x00D5}, // õ → Õ
        {0x00F6, 0x00D6}, // ö → Ö
        {0x00F8, 0x00D8}, // ø → Ø
        {0x00F9, 0x00D9}, // ù → Ù
        {0x00FA, 0x00DA}, // ú → Ú
        {0x00FB, 0x00DB}, // û → Û
        {0x00FC, 0x00DC}, // ü → Ü
        {0x00FD, 0x00DD}, // ý → Ý
        {0x00FE, 0x00DE}, // þ → Þ
        {0x00FF, 0x0178}, // ÿ → Ÿ

        // German sharp S
        {0x00DF, 0x1E9E}, // ß → ẞ

        // Greek (basic)
        {0x03B1, 0x0391}, // α → Α
        {0x03B2, 0x0392}, // β → Β
        {0x03B3, 0x0393}, // γ → Γ
        {0x03B4, 0x0394}, // δ → Δ
        {0x03B5, 0x0395}, // ε → Ε
        {0x03B6, 0x0396}, // ζ → Ζ
        {0x03B7, 0x0397}, // η → Η
        {0x03B8, 0x0398}, // θ → Θ
        {0x03B9, 0x0399}, // ι → Ι
        {0x03BA, 0x039A}, // κ → Κ
        {0x03BB, 0x039B}, // λ → Λ
        {0x03BC, 0x039C}, // μ → Μ
        {0x03BD, 0x039D}, // ν → Ν
        {0x03BE, 0x039E}, // ξ → Ξ
        {0x03BF, 0x039F}, // ο → Ο
        {0x03C0, 0x03A0}, // π → Π
        {0x03C1, 0x03A1}, // ρ → Ρ
        {0x03C3, 0x03A3}, // σ → Σ
        {0x03C4, 0x03A4}, // τ → Τ
        {0x03C5, 0x03A5}, // υ → Υ
        {0x03C6, 0x03A6}, // φ → Φ
        {0x03C7, 0x03A7}, // χ → Χ
        {0x03C8, 0x03A8}, // ψ → Ψ
        {0x03C9, 0x03A9}, // ω → Ω
    };

    for (wchar_t& c : txt)
    {
        auto it = UppercaseMap.find((uint32_t)c);
        if (it != UppercaseMap.end())
        {
            c = (wchar_t)it->second;
        }
    }
    #endif
}

std::wstring
StringUtil::makeLower( const std::wstring & txt )
{
    if (txt.empty()) return {};
    std::wstring out = txt;
    lowerCase(out);
    return out;
}

std::wstring
StringUtil::makeUpper( const std::wstring & txt )
{
    if (txt.empty()) return {};
    std::wstring out = txt;
    upperCase(out);
    return out;
}

std::string
StringUtil::makeLower( const std::string & txt )
{
    if (txt.empty()) return {};
    return de_mbstr( makeLower( de_wstr(txt) ) );
}

std::string
StringUtil::makeUpper( const std::string & txt )
{
    if (txt.empty()) return {};
    return de_mbstr( makeUpper( de_wstr(txt) ) );
}

bool
StringUtil::startsWith( const std::string& str, char c )
{
    if (str.empty()) return false;
    return str[ 0 ] == c;
}

bool
StringUtil::startsWith( const std::wstring& str, wchar_t c )
{
    if (str.empty()) return false;
    return str[ 0 ] == c;
}

bool
StringUtil::endsWith( const std::string& str, char c )
{
    if (str.empty()) return false;
    return str[ str.size() - 1 ] == c;
}

bool
StringUtil::endsWith( const std::wstring& str, wchar_t c )
{
    if (str.empty()) return false;
    return str[ str.size() - 1 ] == c;
}

template <typename T>
bool
impl_StringUtil_startsWith( const T& str, const T& query )
{
    if ( str.empty() ) return false;
    if ( query.empty() ) return false;

    if ( query.size() == str.size() )
    {
        return query == str;
    }
    else if ( query.size() < str.size() )
    {
        bool equal = true;
        for ( size_t i = 0; i < query.size(); ++i )
        {
            const auto a = str[ i ];
            const auto b = query[ i ];
            if ( a != b )
            {
                equal = false;
                break;
            }
        }

        return equal;
        //return query == str.substr( 0, query.size() );
    }
    else
    {
        return false;
    }
}

bool
StringUtil::startsWith( const std::string& str, const std::string& query )
{
    return impl_StringUtil_startsWith(str,query);
}

bool
StringUtil::startsWith( const std::wstring& str, const std::wstring& query )
{
    return impl_StringUtil_startsWith(str,query);
}

template <typename T>
bool
impl_StringUtil_endsWith( const T& str, const T& query )
{
    if ( str.empty() ) return false;
    if ( query.empty() ) return false;

    if ( query.size() == str.size() )
    {
        return query == str;
    }
    else if ( query.size() < str.size() ) // abcde(5), cde(3), pos(2=c)
    {
        bool equal = true;
        for ( size_t i = 0; i < query.size(); ++i )
        {
            const auto a = str[ str.size() - 1 - i ];
            const auto b = query[ query.size() - 1 - i ];
            if ( a != b )
            {
                equal = false;
                break;
            }
        }

        return equal;
        // return query == str.substr( str.size() - query.size(), query.size() );
    }
    else
    {
        return false;
    }
}

//static
bool
StringUtil::endsWith( const std::string& str, const std::string& query )
{
    return impl_StringUtil_endsWith(str,query);
}

bool
StringUtil::endsWith( const std::wstring& str, const std::wstring& query )
{
    return impl_StringUtil_endsWith(str,query);
}

std::string
StringUtil::joinVector( std::vector< std::string > const & v, const std::string& prefix )
{
    std::ostringstream o;

    if ( v.size() > 0 )
    {
        o << v[ 0 ];

        for ( size_t i = 1; i < v.size(); ++i )
        {
            o << prefix << v[ i ];
        }
    }

    return o.str();
}

std::wstring
StringUtil::joinVector( std::vector< std::wstring > const & v, const std::wstring& prefix )
{
    std::wostringstream o;

    if ( v.size() > 0 )
    {
        o << v[ 0 ];

        for ( size_t i = 1; i < v.size(); ++i )
        {
            o << prefix << v[ i ];
        }
    }

    return o.str();
}
std::string
StringUtil::trim( const std::string& txt, const std::string& filter )
{
    return trimRight( trimLeft( txt, filter ), filter );
}

std::string
StringUtil::trimLeft( const std::string& txt, const std::string& filter )
{
    size_t pos = 0;
    for ( size_t i = 0; i < txt.size(); ++i )
    {
        char c = txt[i];
        bool foundFilter = false;
        for ( size_t j = 0; j < filter.size(); ++j )
        {
            if ( c == filter[j] )
            {
                foundFilter = true;
                break;
            }
        }

        if ( !foundFilter )
        {
            pos = i;
            break;
        }
    }

    if ( pos > 0 )
    {
        if ( pos < txt.size() )
        {
            return txt.substr( pos );
        }
        else
        {
            DE_ERROR("(",txt,") -> pos = ",pos," of ",txt.size())
            return txt;
        }
    }
    else
    {
        return txt;
    }
}

std::string
StringUtil::trimRight( const std::string& original, const std::string& filter )
{
    if ( original.size() < 1 )
    {
        return std::string();
    }

    std::string tmp = original;

    auto isFilter = [&] ( char const c )
    {
        for ( size_t i = 0; i < filter.size(); ++i )
        {
            if ( filter[ i ] == c )
            {
                return true;
            }
        }
        return false;
    };

    bool found = isFilter( tmp[ tmp.size() - 1 ] );
    while ( found )
    {
        //tmp = tmp.substr( 0, tmp.size() - 1 );
        tmp.erase( tmp.begin() + int64_t(tmp.size()) - 1 );

        if ( tmp.size() > 0 )
        {
            found = isFilter( tmp[ tmp.size() - 1 ]);
        }
        else
        {
            found = false;
        }
    }

    // DE_DEBUG("original = ", original, ", tmp = ",tmp)

    return tmp;
}

// ===========================================================================
// ======= RAII_File ========================================================
// ===========================================================================

namespace {

// Access mode (pick exactly one)
//     _O_RDONLY — open for reading
//     _O_WRONLY — open for writing
//     _O_RDWR — open for read/write

// Creation / truncation
//     _O_CREAT — create file if missing
//     _O_EXCL — fail if file exists (use with _O_CREAT)
//     _O_TRUNC — truncate to 0 bytes
//     _O_APPEND — writes always go to end

// Binary / text
//     _O_BINARY — raw binary (no CRLF translation), Windows demands it.
//     _O_TEXT — text mode (CRLF translation)
//     For anything non‑text, ALWAYS use _O_BINARY.

// Performance hints
//     _O_SEQUENTIAL
//     _O_RANDOM
//     _O_SHORT_LIVED
//     _O_TEMPORARY

static std::string errno_to_string(int e)
{
    switch (e)
    {
        case EACCES:      return "EACCES: Permission denied";
        case EEXIST:      return "EEXIST: File exists (O_CREAT|O_EXCL used)";
        case EINVAL:      return "EINVAL: Invalid parameter";
        case EMFILE:      return "EMFILE: Process file descriptor limit reached";
        case ENFILE:      return "ENFILE: System-wide file table full";
        case ENOENT:      return "ENOENT: File or path not found";
        case ENOSPC:      return "ENOSPC: No space left on device";
        case ENOTDIR:     return "ENOTDIR: Component of path is not a directory";
        case EROFS:       return "EROFS: Read-only filesystem";
        case EBADF:       return "EBADF: Invalid file descriptor";
        case EFAULT:      return "EFAULT: Bad address";
        case ENAMETOOLONG:return "ENAMETOOLONG: Path too long";
        default:          return "Unknown error";
    }
}

static std::string native_openmode_str(const int openMode)
{
    std::ostringstream o;
    int n = 0;
    // #define O_BINARY _O_BINARY
    // #define O_RDONLY _O_RDONLY
    // #define O_WRONLY _O_WRONLY
    // #define O_RDWR _O_RDWR
    // #define O_APPEND _O_APPEND
    // #define O_CREAT _O_CREAT
    // #define O_TRUNC _O_TRUNC
    if (openMode & O_BINARY) { o << "O_BINARY"; n++; }
    if (openMode & O_RDWR)   { if (n) { o<<'|'; } o << "O_RDWR"; n++; }
    if (openMode & O_WRONLY) { if (n) { o<<'|'; } o << "O_WRONLY"; n++; }
    if (openMode & O_RDONLY) { if (n) { o<<'|'; } o << "O_RDONLY"; n++; }
    if (openMode & O_CREAT)  { if (n) { o<<'|'; } o << "O_CREAT"; n++; }
    if (openMode & O_APPEND) { if (n) { o<<'|'; } o << "O_APPEND"; n++; }
    if (openMode & O_TRUNC)  { if (n) { o<<'|'; } o << "O_TRUNC"; n++; }
    // #define O_EXCL _O_EXCL
    // #define O_TEXT _O_TEXT
    // #define O_RAW _O_BINARY
    // #define O_TEMPORARY _O_TEMPORARY
    // #define O_NOINHERIT _O_NOINHERIT
    // #define O_SEQUENTIAL _O_SEQUENTIAL
    // #define O_RANDOM _O_RANDOM
    // #define O_ACCMODE _O_ACCMODE
    if (openMode & O_EXCL)   { if (n) { o<<'|'; } o << "O_EXCL"; n++; }
    if (openMode & O_TEXT)   { if (n) { o<<'|'; } o << "O_TEXT"; n++; }
    if (openMode & O_RAW)    { if (n) { o<<'|'; } o << "O_RAW"; n++; }
    if (openMode & O_TEMPORARY)  { if (n) { o<<'|'; } o << "O_TEMPORARY"; n++; }
    if (openMode & O_NOINHERIT) { if (n) { o<<'|'; } o << "O_NOINHERIT"; n++; }
    if (openMode & O_SEQUENTIAL)  { if (n) { o<<'|'; } o << "O_SEQUENTIAL"; n++; }
    if (openMode & O_RANDOM)   { if (n) { o<<'|'; } o << "O_RANDOM"; n++; }
    if (openMode & O_ACCMODE) { if (n) { o<<'|'; } o << "O_ACCMODE"; n++; }
    return o.str();
}

int32_t to_native_openmode(const eFileMode fileMode)
{
    const uint32_t fm = static_cast<uint32_t>(fileMode);
    const bool isRead = fm & static_cast<uint32_t>(eFileMode::Read);
    const bool isWrite = fm & static_cast<uint32_t>(eFileMode::Write);
    const bool isAppend = fm & static_cast<uint32_t>(eFileMode::Append);

    int f = O_BINARY; // Best practice on Windows

    if (isRead && isWrite)  { f |= O_RDWR | O_CREAT; }
    else if (isWrite)       { f |= O_WRONLY | O_CREAT; }
    else                    { f |= O_RDONLY; }

    if (isAppend)           { f |= O_APPEND; }
    else if (isWrite)       { f |= O_TRUNC;  } // Always overwrite if exists

    return f;
}


} // end namespace.

// -------------------------------
// openW
// -------------------------------
int32_t file64_open(const std::wstring& uri, eFileMode fileMode, int32_t permission)
{
    const int nativeMode = to_native_openmode(fileMode);
#ifdef _WIN32

    if (permission == 0)
    {
        permission = 0666; // _S_IWRITE, octal 0666 = rw-rw-rw- (no executable bits)
    }
    int fd = _wopen(uri.c_str(), nativeMode, permission);

    if (fd < 0)
    {
        DE_ERROR("Open failed! errno(",errno,"), msg(",errno_to_string(errno),"), uri(",de_mbstr(uri),")")
        DE_ERROR("_wopen(",native_openmode_str(nativeMode),",",permission,")")
    }

    return fd;
#else
    return open(de_mbstr(uri).c_str(), nativeMode, permission);
#endif
}

// -------------------------------
// open
// -------------------------------
int32_t file64_open(const std::string& uri, eFileMode fileMode, int32_t permission)
{
    return file64_open(de_wstr(uri),fileMode,permission);
}

// -------------------------------
// close
// -------------------------------
int32_t file64_close(int32_t fd)
{
#ifdef _WIN32
    return _close(fd);
#else
    return close(fd);
#endif
}

// -------------------------------
// read
// -------------------------------
// - Negative return values indicate errors
// 0 Zero means EOF
// + Positive means bytes written/read
int32_t file64_read(int32_t fd, void* buf, int64_t bytes)
{
    if (bytes > std::numeric_limits<int32_t>::max())
    {
        DE_ERROR("You cannot read ", bytes, " in one call, because return value is int!")
        bytes = std::numeric_limits<int32_t>::max();
    }

#ifdef _WIN32
    return _read(fd, buf, static_cast<uint32_t>(bytes));
#else
    return read(fd, buf, static_cast<uint32_t>(bytes));
#endif
}

// -------------------------------
// write
// -------------------------------
// - Negative return values indicate errors
// 0 Zero means EOF
// + Positive means bytes written/read
int32_t file64_write(int32_t fd, const void* buf, int64_t bytes)
{
    if (bytes > std::numeric_limits<int32_t>::max())
    {
        DE_ERROR("You cannot write ", bytes, " in one call, because return value is int!")
        bytes = std::numeric_limits<int32_t>::max();
    }
#ifdef _WIN32
    return _write(fd, buf, static_cast<uint32_t>(bytes));
#else
    return write(fd, buf, static_cast<uint32_t>(bytes));
#endif
}

// -------------------------------
// seek (64‑bit)
// -------------------------------
int64_t file64_seek(int32_t fd, int64_t offset, eSeekMode seekMode)
{
    int nativeMode = SEEK_SET;
    switch (seekMode)
    {
    case eSeekMode::Cur: nativeMode = SEEK_CUR; break;
    case eSeekMode::End: nativeMode = SEEK_END; break;
    default: break;
    }

#ifdef _WIN32
    return _lseeki64(fd, offset, nativeMode);
#else
    return lseek(fd, offset, nativeMode);
#endif
}

// -------------------------------
// tell (64‑bit)
// -------------------------------
int64_t file64_tell(int32_t fd)
{
#ifdef _WIN32
    return _telli64(fd);
#else
    off_t pos = lseek(fd, 0, SEEK_CUR); // No move, only tells pos.
    return static_cast<int64_t>(pos);
#endif
}

// =======================================================================
File::File()
    // =======================================================================
    : m_fd(-1)
{}

File::~File()
{
    close();
}

File::File(const std::wstring& utf16_uri, eFileMode fm, int permission)
    : m_fd(-1)
{
    open( utf16_uri, fm, permission );
}

File::File(const std::string& utf8_uri, eFileMode fm, int permission)
    : m_fd(-1)
{
    open( utf8_uri, fm, permission );
}

bool
File::open(const std::wstring& utf16_uri, eFileMode fileMode, int permission)
{
    if ( is_open() )
    {
        DE_ERROR("Already open", de_mbstr(utf16_uri))
        return true;
    }

    m_fd = file64_open( utf16_uri, fileMode, permission );
    if ( m_fd < 0 )
    {
        DE_ERROR("Cannot open ", de_mbstr(utf16_uri))
        return false;
    }

    m_uri = de_mbstr(utf16_uri);
    return true;
}

bool
File::open(const std::string& utf8_uri, eFileMode fileMode, int permission)
{
    if ( is_open() )
    {
        DE_ERROR("Already open", utf8_uri)
        return true;
    }

    m_fd = file64_open( utf8_uri, fileMode, permission );
    if ( m_fd < 0 )
    {
        DE_ERROR("Cannot open ", utf8_uri)
        return false;
    }

    m_uri = utf8_uri;
    return true;
}

void File::close()
{
    if ( m_fd < 0 )
    {
        // DE_WARN("Already closed. ",m_uri)
        return;
    }

    file64_close( m_fd );
    m_fd = -1;
}

bool File::is_open() const
{
    return m_fd > -1;
}

int64_t File::write( const void* __restrict__ src, int64_t nBytes ) const
{
    if (!is_open())
    {
        DE_ERROR("Not open. ", m_uri)
        return 0;
    }

    if ( nBytes < 1 )
    {
        DE_WARN("Got 0 bytes to write. ", m_uri)
        return 0;
    }

    // INT_MAX/4 = 512MB chunks. Because file64_read() actually returns int32 not int64.
    const int64_t chunkBytes = std::numeric_limits<int32_t>::max()/4;
    const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(src);

    // Write File chunkwise to HDD
    int64_t writtenBytes = 0;
    while (writtenBytes < nBytes)
    {
        const int64_t reqBytes = std::min<int64_t>(nBytes - writtenBytes, chunkBytes);
        const int64_t gotBytes = file64_write( m_fd, pSrc, reqBytes );

        // ---- Hard error ----
        if (gotBytes < 0)
        {
            DE_ERROR("HARD_WRITE_ERROR ", gotBytes, ". ",m_uri)
            break;
        }

        // ---- Out of disk memory? ----
        if (gotBytes == 0)
        {
            DE_ERROR("Disk full? ", m_uri);
            break;
        }

        // ---- Partial write ----
        if (gotBytes != reqBytes)
        {
            DE_ERROR("PARTIAL write: req=", reqBytes, " got=", gotBytes, ". ",m_uri)
        }

        writtenBytes += gotBytes;
        pSrc += gotBytes;
    }

    // Check final size
    if ( writtenBytes != nBytes )
    {
        DE_ERROR("Incomplete write: writtenBytes(",writtenBytes,") != expected(",nBytes,"). ",m_uri)
    }

    return writtenBytes;
}

int64_t File::read( void* __restrict__ dst, int64_t nBytes ) const
{
    if (!is_open())
    {
        DE_ERROR("Not open. ", m_uri)
        return 0;
    }

    if ( nBytes < 1 )
    {
        DE_WARN("Got 0 bytes to read. ", m_uri)
        return 0;
    }

    // INT_MAX/4 = 512MB chunks.
    // Because file64_read() actually returns int32_t not int64_t.
    const int64_t chunkBytes = std::numeric_limits<int32_t>::max()/4;
    uint8_t* __restrict__ pDst = reinterpret_cast<uint8_t*>(dst);

    // Read File chunkwise to RAM
    int64_t readBytes = 0;
    while (readBytes < nBytes)
    {
        const int64_t reqBytes = std::min<int64_t>(nBytes - readBytes, chunkBytes);
        const int64_t gotBytes = file64_read( m_fd, pDst, reqBytes );

        // ---- Hard error ----
        if (gotBytes < 0)
        {
            DE_ERROR("HARD_READ_ERROR ", gotBytes, ". ",m_uri)
            break;
        }

        // ---- EOF ----
        if (gotBytes == 0)
        {
            // DE_WARN("EOF should not happen here. ", m_uri);
            break;
        }

        // ---- Partial read ----
        /*
        if (gotBytes != reqBytes)
        {
            DE_WARN("PARTIAL read: req=", reqBytes, " got=", gotBytes, ". ",m_uri)
        }
        */

        readBytes += gotBytes;
        pDst += gotBytes;
    }

    // Check final size
    /*
    if ( readBytes != nBytes )
    {
        DE_ERROR("File shorter than expected, gotBytes(",readBytes,") != expected(",nBytes,"). ",m_uri)
    }
    */
    return readBytes;
}

int64_t File::size() const
{
    if (!is_open())
    {
        DE_ERROR("Not open")
        return 0;
    }

    const int64_t lastPos = file64_tell( m_fd );
    file64_seek(m_fd, 0, eSeekMode::End);
    const int64_t endPos = file64_tell( m_fd );
    file64_seek(m_fd, lastPos, eSeekMode::Set);
    return endPos;
}

int64_t File::tell() const
{
    if (!is_open())
    {
        DE_ERROR("Not open")
        return 0;
    }
    return file64_tell( m_fd );
}


int64_t File::seek(int64_t offset, eSeekMode seekMode) const
{
    if (!is_open())
    {
        DE_ERROR("Not open")
        return 0;
    }

    return file64_seek(m_fd,offset,seekMode);
}

int32_t File::read_u8( uint8_t* out ) const
{
    if (!is_open())
    {
        DE_ERROR("Not open")
        return 0;
    }

    uint8_t byte;
    const int32_t r = read(&byte,1);
    if (r != 1)
    {
        DE_ERROR("Not 1 byte, got ",r)
        if (out) { *out = 0; }
        return 0;
    }
    else
    {
        if (out) { *out = byte; }
        return 1; // 1 byte consumed
    }
}

int32_t File::read_u16_be( uint16_t* out ) const
{
    uint8_t buf[2];
    const int32_t r = read(buf, 2);
    if (r != 2)
    {
        DE_ERROR("No 2 bytes, got ",r)
        if (out) { *out = 0; }
        return 0;
    }
    else
    {
        if (out)
        {
            *out = (uint16_t(buf[0]) << 8) | uint16_t(buf[1]);
        }
        return 2;
    }
}

int32_t File::read_s16_be( int16_t* out ) const
{
    uint8_t buf[2];
    const int32_t r = read(buf, 2);
    if (r != 2)
    {
        DE_ERROR("No 2 bytes, got ",r)
        if (out) { *out = 0; }
        return 0;
    }
    else
    {
        if (out)
        {
            uint16_t u = (uint16_t(buf[0]) << 8) | uint16_t(buf[1]);
            *out = static_cast<int16_t>(u);
        }
        return 2;
    }
}

int32_t File::read_u24_be( uint32_t* out ) const
{
    uint8_t buf[3];
    const int32_t r = read(buf, 3);
    if (r != 3)
    {
        DE_ERROR("No 3 bytes, got ",r)
        if (out) { *out = 0ul; }
        return 0;
    }
    else
    {
        if (out)
        {
            *out = (uint32_t(buf[0]) << 16)
                 | (uint32_t(buf[1]) <<  8)
                 |  uint32_t(buf[2]);
        }
        return 3;
    }
}

int32_t File::read_u32_be( uint32_t* out ) const
{
    uint8_t buf[4];
    const int32_t r = read(buf, 4);
    if (r != 4)
    {
        DE_ERROR("No 4 bytes, got ",r)
        if (out) { *out = 0ul; }
        return 0;
    }
    else
    {
        if (out)
        {
            *out = (uint32_t(buf[0]) << 24)
                 | (uint32_t(buf[1]) << 16)
                 | (uint32_t(buf[2]) <<  8)
                 | (uint32_t(buf[3]));
        }
        return 4;
    }
}

int32_t File::read_s32_be( int32_t* out ) const
{
    uint8_t buf[4];
    const int32_t r = read(buf, 4);
    if (r != 4)
    {
        DE_ERROR("No 4 bytes, got ",r)
        if (out) { *out = 0ul; }
        return 0;
    }
    else
    {
        if (out)
        {
            uint32_t value = (uint32_t(buf[0]) << 24)
                           | (uint32_t(buf[1]) << 16)
                           | (uint32_t(buf[2]) <<  8)
                           | (uint32_t(buf[3]));
            *out = static_cast<int32_t>(value);
        }
        return 4;
    }
}

int32_t File::read_u64_be( uint64_t* out ) const
{
    uint8_t buf[8];
    const int32_t r = read(buf, 8);
    if (r != 8)
    {
        DE_ERROR("No 8 bytes, got ",r)
        if (out) { *out = 0ull; }
        return 0;
    }
    else
    {
        if (out)
        {
            *out = (uint64_t(buf[0]) << 56)
            | (uint64_t(buf[1]) << 48)
                | (uint64_t(buf[2]) << 40)
                | (uint64_t(buf[3]) << 32)
                | (uint64_t(buf[4]) << 24)
                | (uint64_t(buf[5]) << 16)
                | (uint64_t(buf[6]) <<  8)
                | (uint64_t(buf[7]) );
        }
        return 8;
    }
}

int32_t File::read_char4( char buf[4] ) const
{
    const int32_t r = read(buf, 4);
    if (r != 4)
    {
        DE_ERROR("No 4 bytes, got ",r)
        buf[0] = '\0';
        buf[1] = '\0';
        buf[2] = '\0';
        buf[3] = '\0';
        return 0;
    }
    else
    {
        return 4;
    }
}


/*
//static
bool
// =======================================================================
FileSystem::loadByteVector( std::vector< uint8_t > & bv, const std::string& uri, uint64_t byteLimit )
// =======================================================================
{
   bv.clear(); // bv = ByteVector

   // Open file as binary
   FILE* file = ::fopen64( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open file ", uri )
      return false;
   }

   // Read file size
   ::fseeko64( file, 0, SEEK_END );
   size_t fileBytes = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   // Validate file size
   if ( fileBytes < 1 ) // No data
   {
      DE_ERROR("Empty file ",uri)
      ::fclose( file );                               // close
      return false;
   }

   // Validate file size
   if ( fileBytes > byteLimit ) // Bad, reached RAM limit.
   {
      DE_ERROR("File reached RAM limit(",byteLimit,"), abort. ",uri)
      ::fclose( file );
      return false;
   }

   // Copy entire file to RAM
   bv.resize( fileBytes );
   ::fread( bv.data(), 1, bv.size(), file );
   ::fclose( file );                                  // close

   //DE_DEBUG("File has uri = ",uri)
   //DE_DEBUG("File has byteCount = ",bv.size())

   return true;
}
*/

std::string
FileSystem::loadStr( const std::string& uri )
{
    //DE_PERF_MARKER
    std::ifstream fin( uri );
    std::ostringstream o;
    if ( fin.is_open() )
    {
        o << fin.rdbuf();
    }
    return o.str();
}
std::wstring
FileSystem::loadStrW( const std::wstring& uri )
{
    //DE_PERF_MARKER
    std::wifstream file( uri );
    std::wostringstream o;
    if ( file.is_open() )
    {
        o << file.rdbuf();
    }
    return o.str();
}
bool
FileSystem::saveStr( const std::string& uri, const std::string& txt )
{
    //DE_PERF_MARKER
    std::ofstream file( uri );
    if ( !file.is_open() ) return false;
    file << txt;
    file.close();
    return true;
}
bool
FileSystem::saveStr( const std::wstring& uri, const std::wstring& txt )
{
    //DE_PERF_MARKER
    std::wofstream file( uri.c_str() );
    if ( !file.is_open() ) return false;
    file << txt;
    file.close();
    return true;
}

/*
// Load font file into memory
std::ifstream file("font.ttf", std::ios::binary);
if (!file)
{
    std::cerr << "Error loading font file!" << std::endl;
    return -1;
}

// Read file into memory buffer
std::vector<unsigned char> fontBuffer((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
file.close();
*/

bool
FileSystem::saveBlob( const Blob& blob, const std::string& uri )
{
    // DE_PERF_MARKER

    if ( blob.empty() )
    {
        DE_WARN("Empty blob. ", uri ) // Nothing todo.
        return true;
    }

    File file( uri, eFileMode::Write );
    if ( !file.is_open() )
    {
        DE_ERROR("Cannot write blob. ", uri)
        return false;
    }

    int64_t written = file.write(blob.data(), blob.size());
    if ( written != blob.size() )
    {
        DE_ERROR("written(",written,") != blob(",blob.size(),"). ",uri)
    }
    return true;
}

bool
FileSystem::loadBlob( Blob& blob, const std::string& uri, const int64_t sizeLimit )
{
    // DE_PERF_MARKER

    File file( uri, eFileMode::Read );
    if (!file.is_open()) { DE_ERROR("Cannot read file ", uri ) return false; }

    // Check file size for RAM.
    const int64_t nBytes = file.size();
    if ( nBytes < 1 ) { DE_WARN("Empty file ", uri ) return false; }

    if ( sizeLimit > 0 && sizeLimit < nBytes )
    {
        DE_WARN("File too big (",dbStrBytes(nBytes),"), only max. ",dbStrBytes(sizeLimit)," supported. ", uri )
        return false;
    }

    // Reset and Allocate RAM for file
    blob.clear();
    try
    {
        blob.resize( nBytes ); // , 0x00
    }
    catch(...)
    {
        DE_ERROR("OutOfMemory")
        return false;
    }

    // Read File to RAM, chunkwise in INT_MAX/2 chunks. Because file_read() returns int32_t not int64_t
    const int64_t n = file.read( blob.data(), nBytes );
    if ( n != nBytes )
    {
        DE_ERROR("readBytes(",n,") != nBytes(",nBytes,"). ",uri)
    }
    return true;
}

bool
FileSystem::existFile( const std::string& uri )
{
   if ( uri.empty() ) { return false; }
   auto p = fs::u8path( uri );
   auto s = fs::status( p );
   bool ok = fs::exists( s ) && fs::is_regular_file( s );
   return ok;
}

bool
FileSystem::existFile( const std::wstring& uri )
{
    if ( uri.empty() ) { return false; }
    fs::file_status s = fs::status( uri );
    bool ok = fs::exists( s ) && fs::is_regular_file( s );
    return ok;
}

bool
FileSystem::existDirectory( const std::string& uri )
{
   if ( uri.empty() ) return false;
   auto p = fs::u8path( uri );
   auto s = fs::status( p );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}

bool
FileSystem::existDirectory( const std::wstring& uri )
{
    if ( uri.empty() ) return false;
    fs::file_status s = fs::status( uri );
    bool ok = fs::exists( s ) && fs::is_directory( s );
    return ok;
}

// static
int64_t
FileSystem::fileSize( const std::string & uri )
{
    auto p = fs::u8path( uri );
    return int64_t( fs::file_size( p ) );
}

// static
int64_t
FileSystem::fileSize( const std::wstring & uri )
{
    auto p = fs::path( uri );
    return int64_t( fs::file_size( p ) );
}

std::string
FileSystem::fileName( const std::string& uri, const std::string& relativeToPath )
{
#if 1
    auto a = de_wstr( uri );
    auto b = de_wstr( relativeToPath );
    auto c = fileName( a, b );
    return de_mbstr( c );
#else
    if ( relativeToPath.empty() )
    {
        auto p1 = fs::path( uri ).filename().string();
        if ( p1.empty() ) return uri;
        return p1;
    }
    else
    {
        const auto p1 = makeAbsolute(uri);
        const auto p2 = makeAbsolute(relativeToPath);
        const auto p3 = fs::relative(p1, p2).string();
        fprintf(stdout,"p1 = %s\n",p1.c_str());
        fprintf(stdout,"p2 = %s\n",p2.c_str());
        fprintf(stdout,"p3 = %s\n",p3.c_str());
        fflush(stdout);
        return p3;
    }
#endif
}

std::wstring
FileSystem::fileName( const std::wstring& uri, const std::wstring& relativeToPath )
{
    if ( relativeToPath.empty() )
    {
        auto p1 = fs::path( uri ).filename().wstring();
        //if ( p1.empty() ) return uri;
        return p1;
    }
    else
    {
        const auto p1 = makeAbsolute(uri);
        const auto p2 = makeAbsolute(relativeToPath);
        std::error_code ec;
        const auto p3 = fs::relative(p1, p2, ec).wstring();
        if (ec)
        {
            DE_ERROR("ec(",ec.message(),"), ",de_mbstr(uri))
        }
        // DE_DEBUG("p1 = ",p1.c_str())
        // DE_DEBUG("p2 = ",p2.c_str())
        // DE_DEBUG("p3 = ",p3.c_str())
        return p3;
    }
}

std::string
FileSystem::fileBase( const std::string& uri )
{
   return fs::u8path( uri ).stem().u8string();
}

std::wstring
FileSystem::fileBase( const std::wstring& uri )
{
    return fs::path( uri ).stem().wstring();
}

std::string
FileSystem::fileSuffix( const std::string& uri )
{
#if 1
    return de_mbstr( fileSuffix( de_wstr( uri ) ) );
#else
    if ( uri.empty() )
    {
        return {};
    }

    std::string ext = fs::u8path( uri ).extension().u8string();
    if ( ext.empty() )
    {
        return {};
    }

    if (ext[0] == '.')
    {
        ext.erase( 0, 1 );
    }

    StringUtil::lowerCase( ext );
    return ext;
#endif
}

std::wstring
FileSystem::fileSuffix( const std::wstring& uri )
{
    if ( uri.empty() )
    {
        return {};
    }

    std::wstring ext = fs::path( uri ).extension().wstring();
    if ( ext.empty() )
    {
        return {};
    }

    if (ext[0] == L'.')
    {
        ext.erase( 0, 1 );
    }

    StringUtil::lowerCase( ext );
    return ext;
}

std::string
FileSystem::fileDir( const std::string& uri )
{
    return de_mbstr( fileDir( de_wstr( uri ) ) );
}

std::wstring
FileSystem::fileDir( const std::wstring& uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        std::error_code ec;
        p = fs::absolute( p, ec );
        if (ec)
        {
            DE_ERROR("ec(",ec.message(),") for ", de_mbstr(uri))
        }
    }

    // if ( !fs::is_directory( p ) )
    // {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
        else
        {
            DE_ERROR("No parent path for ", de_mbstr(uri))
        }

    //}

    return makePosixPath(p.wstring());
}

std::string
FileSystem::parentDir( const std::string& uri )
{
    fs::path parent = fs::u8path(uri).parent_path();
    return parent.u8string();
}

std::wstring
FileSystem::parentDir( const std::wstring& uri )
{
    fs::path parent = fs::path(uri).parent_path();
    return parent.wstring();
}

std::string
FileSystem::makeAbsolute( const std::string& uri, const std::string& baseDir )
{
    std::string o = uri;
    try
    {
        const auto p1 = fs::u8path( uri );
        const auto p2 = fs::canonical( fs::absolute( p1 ) );
        o = p2.u8string();
    }
    catch ( std::exception & e )
    {
        // DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
    }

    return o;
}

std::wstring
FileSystem::makeAbsolute( const std::wstring& uri, const std::wstring& baseDir )
{
    try
    {
        const auto p1 = fs::path( uri );
        const auto p2 = fs::canonical( fs::absolute( p1 ) );
        return p2.wstring();
    }
    catch ( std::exception & e )
    {
        //DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
        return uri;
    }
}

std::string
FileSystem::makeWinPath( const std::string & uri )
{
    fs::path p1 = fs::u8path( uri );
    fs::path p2( makeWinPath( p1.wstring() ) );
    return p2.u8string();
}

std::wstring
FileSystem::makeWinPath( const std::wstring & uri )
{
    auto tmp = uri;

    if (tmp.empty()) return tmp;

    // DE_DEBUG("tmp = ", tmp)

    tmp = StringUtil::replace( tmp, L"/", L"\\" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, L"\\.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, L"\\") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

std::string
FileSystem::makePosixPath( const std::string & uri )
{
    fs::path p1 = fs::u8path( uri );
    fs::path p2( makePosixPath( p1.wstring() ) );
    return p2.u8string();
}

std::wstring
FileSystem::makePosixPath( const std::wstring & uri )
{
    if (uri.empty()) return {};

    // DE_DEBUG("tmp = ", tmp)

    auto tmp = StringUtil::replace( uri, L"\\", L"/" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, L"/.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, L"/") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

void
FileSystem::createDirectory( const std::string& uri )
{
    if ( uri.empty() ) return;

    auto p = fs::u8path( uri );
    auto s = fs::status( p );

    if ( fs::exists( s ) && fs::is_directory( s ) )
    {
        // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
        return;
    }

    std::error_code e;
    fs::create_directories( p, e );
    if ( e )
    {
        DE_ERROR("( uri:",uri,") :: Got error ",e.message())
    }
}

void
FileSystem::createDirectory( const std::wstring& uri )
{
    if ( uri.empty() ) return;

    auto p = fs::path( uri );
    auto s = fs::status( p );

    if ( fs::exists( s ) && fs::is_directory( s ) )
    {
        // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
        return;
    }

    std::error_code e;
    fs::create_directories( p, e );
    if ( e )
    {
        DE_ERROR("( uri:",de_mbstr(uri),") :: Got error ",e.message())
    }
}

void
FileSystem::removeFile( const std::string& uri )
{
    auto p = fs::u8path( uri );
    std::error_code e;
    fs::remove(p, e);
    if ( e )
    {
        DE_ERROR("Cant remove file(",uri,") :: ",e.message())
    }
}

void
FileSystem::removeFile( const std::wstring& uri )
{
    auto p = fs::path( uri );
    std::error_code e;
    fs::remove(p, e);
    if ( e )
    {
        DE_ERROR("Cant remove file(",de_mbstr(uri),") :: ",e.message())
    }
}

bool
FileSystem::copyFile( std::string src, std::string dst )
{
    DE_DEBUG("src(",src,"), dst(",dst,")")

    if ( src.empty() )
    {
        return false;
    }
    if ( dst.empty() )
    {
        return false;
    }

    makeWinPath(src);

    DE_DEBUG("makeWinPath(src) = ",src)

    FileSystem::makeAbsolute(src);

    DE_DEBUG("makeAbsolute(src) = ",src)

    makeWinPath(dst);

    DE_DEBUG("makeWinPath(dst) = ",dst)

    auto p0 = fs::u8path( src );
    auto s0 = fs::status( p0 );
    if ( !fs::exists( s0 ) )
    {
        DE_ERROR("File not exist! src(",src,"), dst(",dst,")")
        return false;
    }
    else
    {
        DE_INFO("File src exist (",src,")")
    }

    if ( !fs::is_regular_file( s0 ) )
    {
        DE_ERROR("File not a regular file! src(",src,"), dst(",dst,")")
        return false;
    }
    else
    {
        DE_INFO("File src is regular (",src,")")
    }

    auto p1 = fs::u8path( dst );
    //   fs::path p1 = dst.parent_path();
    //   std::error_code e;
    //   fs::create_directories( p_dir, e );
    //   if ( e )
    //   {
    //      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    //   }

    if ( fs::exists( p1 ) )
    {
        removeFile( p1.wstring() );
    }

    fs::copy_file( p0, p1 ); //, fs::copy_options::recursive );
    if ( fs::exists( p1 ) )
    {
        DE_DEBUG("Copied file(",src," -> ",dst,")")
        return true;
    }
    else
    {
        DE_ERROR("Cant copy file(",src,",",dst,")")
        return false;
    }

    return true;
}

std::string
FileSystem::createUniqueFileName( const std::string& userPrefix )
{
    int ms = HighResolutionClock::GetTimeInMilliseconds();

    std::ostringstream o;

    int y=0,m=0,d=0,hh=0,mm=0,ss=0;

    time_t cur = ::time( nullptr );
    struct tm* t = gmtime( &cur );
    if (t)
    {
        y = t->tm_year+1900;
        m = t->tm_mon+1;
        d = t->tm_mday;
        hh = t->tm_hour;
        mm = t->tm_min;
        ss = t->tm_sec;
    }
    o << userPrefix
    << "_" << y << "-" << m << "-" << d << "_" << hh << "-" << mm << "-" << ss << "_" << ms;
    return o.str();
}

bool
FileSystem::isAbsolute( const std::string & uri )
{
    return fs::u8path( uri ).is_absolute();
}

bool
FileSystem::isAbsolute( const std::wstring & uri )
{
    return fs::path( uri ).is_absolute();
}

std::vector<std::string>
FileSystem::entries(std::string baseDir,
                    bool recursive, bool withFiles, bool withDirs)
{
    std::vector<std::string> collection;

    size_t i = 0;

    entries( baseDir, recursive, withFiles, withDirs,
            [&]( const std::string & )
            {
                i++;
            });

    if (i > 0)
    {
        collection.reserve( i );

        entries( baseDir, recursive, withFiles, withDirs,
                [&]( const std::string & fileName )
                {
                    collection.emplace_back( fileName );
                });
    }

    return collection;
}

std::vector<std::wstring>
FileSystem::entries(std::wstring baseDir,
                    bool recursive, bool withFiles, bool withDirs)
{
    std::vector<std::wstring> collection;

    size_t i = 0;

    entries( baseDir, recursive, withFiles, withDirs,
            [&]( const std::wstring & )
            {
                i++;
            });

    if (i > 0)
    {
        collection.reserve( i );

        entries( baseDir, recursive, withFiles, withDirs,
                [&]( const std::wstring & fileName )
                {
                    collection.emplace_back( fileName );
                });
    }

    return collection;
}

bool
FileSystem::entries(std::string baseDir,
                    bool recursive,
                    bool withFiles,
                    bool withDirs,
                    const std::function< void( const std::string & ) > & onFileName )
{
    DE_WARN("baseDir = ", baseDir)

    if (baseDir.empty())
    {
        baseDir = ".";
        DE_WARN("baseDir2 = ", baseDir)
    }

    if (!isAbsolute(baseDir))
    {
        DE_WARN("baseDir3(absolute) = ", baseDir)
        baseDir = FileSystem::makeAbsolute( baseDir );
    }

    if ( !FileSystem::existDirectory( baseDir ) )
    {
        DE_ERROR("No dir ", baseDir)
        return false;
    }

    if ( recursive )
    {
        DE_WARN("scan recursive!")
        fs::recursive_directory_iterator it( fs::u8path(baseDir) );
        while ( it != fs::recursive_directory_iterator() )
        {
            const fs::path p = it->path();
            const std::wstring f1 = FileSystem::makeAbsolute( p.wstring() );
            const std::wstring f2 = FileSystem::makePosixPath( f1 );
            const std::string uri = de_mbstr(f2);

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( uri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( uri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Recursive find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    else
    {
        fs::directory_iterator it( fs::u8path(baseDir) );
        while ( it != fs::directory_iterator() )
        {
            const fs::path p = it->path();
            const std::wstring f1 = FileSystem::makeAbsolute( p.wstring() );
            const std::wstring f2 = FileSystem::makePosixPath( f1 );
            const std::string uri = de_mbstr(f2);

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( uri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( uri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Iterative find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    return true;
}

bool
FileSystem::entries(std::wstring baseDir,
                    bool recursive,
                    bool withFiles,
                    bool withDirs,
                    const std::function< void( const std::wstring & ) > & onFileName )
{
    DE_WARN("baseDir = ", de_mbstr(baseDir))

    if (baseDir.empty())
    {
        baseDir = L".";
        DE_WARN("baseDir2 = ", de_mbstr(baseDir))
    }

    if (!isAbsolute(baseDir))
    {
        DE_WARN("baseDir3(absolute) = ", de_mbstr(baseDir))
        baseDir = FileSystem::makeAbsolute( baseDir );
    }

    if ( !FileSystem::existDirectory( baseDir ) )
    {
        DE_ERROR("No dir ", de_mbstr(baseDir))
        return false;
    }

    if ( recursive )
    {
        DE_WARN("scan recursive!")
        fs::recursive_directory_iterator it( baseDir );
        while ( it != fs::recursive_directory_iterator() )
        {
            const fs::path p = it->path();
            const std::wstring f1 = FileSystem::makeAbsolute( p.wstring() );
            const std::wstring uri = FileSystem::makePosixPath( f1 );

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( uri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( uri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Recursive find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    else
    {
        fs::directory_iterator it( baseDir );
        while ( it != fs::directory_iterator() )
        {
            const fs::path p = it->path();
            const std::wstring f1 = FileSystem::makeAbsolute( p.wstring() );
            const std::wstring uri = FileSystem::makePosixPath( f1 );

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( uri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( uri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Iterative find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    return true;
}

/*
bool
FileSystem::copyDirectory( std::string const & src, std::string const & dst )
{
    //   std::error_code e;
    //   fs::create_directories( fs::path( uri ), e );
    //   if ( e )
    //   {
    //      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    //   }
    return false;
}

std::string
FileSystem::parentDir( std::string const & uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::string tmp = p.string();
    dbStrReplace( tmp, "\\", "/" );
    if ( dbStrEndsWith( tmp, "/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

std::wstring
FileSystem::parentDir( std::wstring const & uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::wstring tmp = p.wstring();
    dbStrReplace( tmp, L"\\", L"/" );
    if ( dbStrEndsWith( tmp, L"/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

void
FileSystem::createDirectory( std::string const & uri )
{
    if ( uri.empty() ) return;

    fs::path p( uri );

    fs::file_status s = fs::status( p );

    if ( fs::exists( s ) && fs::is_directory( s ) )
    {
        // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
        return;
    }

    std::error_code e;
    fs::create_directories( p, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    }
}

void
FileSystem::removeFile( std::string const & uri )
{
    std::error_code e;
    fs::remove( uri, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message() << std::endl;
    }
}

void
FileSystem::removeFile( std::wstring const & uri )
{
    std::error_code e;
    fs::remove( uri, e );
    if ( e )
    {
        std::wcout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message().c_str() << std::endl;
    }
}

void
FileSystem::removeDirectory( std::string const & uri )
{
    std::error_code e;
    fs::remove_all( fs::path( uri ), e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    }
    fs::remove( uri, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got 2nd error " << e.message() << std::endl;
    }
}

std::vector< std::string >
dbGetFilteredFiles( std::string const & uri, bool recursive, std::vector< std::string > const & exts )
{
    if ( uri.empty() )
    {
        return {};
    }

    if ( exts.empty())
    {
        return dbGetFiles( uri, recursive );
    }

    std::vector< std::string > files;

    de::FileSystem::entryList( uri, recursive, true, false,
                              [&] ( std::string const & fileName )
                              {
                                  std::string const ext = de::FileSystem::fileSuffix( fileName );

                                  int32_t found = -1;

                                  for ( size_t i = 0; i < exts.size(); ++i )
                                  {
                                      if ( ext == exts[ i ] )
                                      {
                                          found = int32_t( i );
                                          break;
                                      }
                                  }

                                  if ( found > -1 )
                                  {
                                      // DE_DEBUG( "Found file(", files.size(),") with extension(", ext, ") in fileName(", fileName, ")")
                                      files.emplace_back( fileName ); // extension is in given list.
                                  }
                              }
                              );

    //DE_DEBUG("Found (",files.size(),") files with given exts(", exts.size(), ")" )
    return files;
}

std::string
dbFirstSubDirectory( std::string const & uri, bool recursive )
{
    de::fs::path p( uri );
    if ( de::fs::is_regular_file( p ) )
    {
        p = p.parent_path();
    }
    if ( !de::fs::is_directory( p ) )
    {
        return "";
    }
    if ( !de::fs::exists( p ) )
    {
        return "";
    }

    if ( recursive )
    {
        de::fs::recursive_directory_iterator it( p );
        while ( it != de::fs::recursive_directory_iterator() )
        {
            de::fs::path p = it->path();
            if ( de::fs::is_directory( p ) )
            {
                return de::FileSystem::makeAbsolute( p.string() );
            }
            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                break;
            }
        }
    }
    else
    {
        de::fs::directory_iterator it( p );
        while ( it != de::fs::directory_iterator() )
        {
            de::fs::path p = it->path();
            if ( de::fs::is_directory( p ) )
            {
                return de::FileSystem::makeAbsolute( p.string() );
            }
            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                break;
            }
        }
    }
    return "";
}
*/

// ===========================================================================

int64_t
SteadyClock::GetTimeInNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

int64_t
SteadyClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

int32_t
SteadyClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

double
SteadyClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================

int64_t
HighResolutionClock::GetTimeInNanoseconds()
{
   typedef std::chrono::high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

int64_t
HighResolutionClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

int32_t
HighResolutionClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

double
HighResolutionClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================
PerformanceTimer::PerformanceTimer()
// ===========================================================================
   : m_isStarted( false )
{
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
}

void
PerformanceTimer::start()
{
   if ( m_isStarted ) return;
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
   m_isStarted = true;
}

void
PerformanceTimer::stop()
{
   if ( !m_isStarted ) return;
   m_timeStop = HighResolutionClock::GetTimeInNanoseconds();
   m_isStarted = false;
}

int64_t
PerformanceTimer::ns() const { return m_timeStop - m_timeStart; }

int64_t
PerformanceTimer::us() const { return ns() / 1000; }

int32_t
PerformanceTimer::ms() const { return int32_t( ns() / 1000000 ); }

double
PerformanceTimer::sec() const { return double( ns() ) * 1.0e-9; }

// ============================================================================
// Binary
// ============================================================================

uint8_t
Binary::readU8()
{
    if ( !m_ptr || m_avail < 1 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t byte = *(m_ptr + m_index);
    m_index += 1;
    m_avail -= 1;
    return byte;
}

uint16_t
Binary::readU16_lsb()
{
    if ( !m_ptr || m_avail < 2 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    m_index += 2;
    m_avail -= 2;
    return size_t( a ) | ( size_t( b ) << 8 );
}

uint32_t
Binary::readU32_lsb()
{
    if ( !m_ptr || m_avail < 4 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    uint8_t c = *(p + 2);
    uint8_t d = *(p + 3);
    m_index += 4;
    m_avail -= 4;
    return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );
}

uint16_t
Binary::readU16_msb()
{
    if ( !m_ptr || m_avail < 2 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    m_index += 2;
    m_avail -= 2;
    return size_t( b ) | ( size_t( a ) << 8 );
}

uint32_t
Binary::readU32_msb()
{
    if ( !m_ptr || m_avail < 4 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    uint8_t c = *(p + 2);
    uint8_t d = *(p + 3);
    m_index += 4;
    m_avail -= 4;
    return uint32_t( d ) |
         ( uint32_t( c ) << 8 ) |
         ( uint32_t( b ) << 16 ) |
         ( uint32_t( a ) << 24 );
}

bool Binary::save( const std::string& uri ) const
{
    FILE* file = ::fopen64( uri.c_str(), "wb" );
    if ( !file )
    {
        DE_ERROR("Cant open to write ", uri )
        return false;
    }

    if ( m_ptr )
    {
        size_t nWritten = ::fwrite( m_ptr, 1, m_size, file );
        if ( nWritten < m_size )
        {
            DE_ERROR("nWritten(",nWritten,") < m_size(",m_size,")")
        }
    }
    else
    {
        DE_ERROR("No data to write ", uri )
    }

    ::fclose( file );
    return true;
}

/*
Binary
Binary::fromRAM( uint8_t const * ptr, uint64_t count, std::string uri )
{
   Binary b;
   b.openMemory( ptr, count, uri );
   return b;
}

Binary
Binary::fromHDD( std::string uri )
{
   Binary b;
   b.load( uri );
   return b;
}

// ============================================================================
Binary::Binary()
// ============================================================================
   : m_Ptr( nullptr )
   , m_Count( 0 )
   , m_Index( 0 )
   , m_Avail( 0 )
{

}

Binary::Binary( const uint8_t * ptr, uint64_t count, std::string uri )
   : Binary()
{
    m_Ptr = ptr;
    m_Count = count;
    m_Uri = uri;
}

Binary::Binary( std::string uri )
    : Binary()
{
    load( uri );
}

//~Binary() { close(); }

uint64_t             Binary::tell() const { return m_Index; }
uint64_t             Binary::size() const { return m_Count; }
uint8_t const*       Binary::data() const { return m_Ptr; }
uint8_t*             Binary::data() { return m_Ptr; }
std::string const&   Binary::getUri() const { return m_Uri; }
bool                 Binary::is_open() const { return m_Ptr != nullptr; }
void                 Binary::close() { m_Ptr = nullptr; m_Blob.clear(); }
void                 Binary::clear() { m_Ptr = nullptr; m_Blob.clear(); }

void                 Binary::resize( size_t nBytes )
{
    m_Blob.resize( nBytes );
}

bool
Binary::save( std::string uri ) const
{
   FILE* file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open to write ", uri )
      return false;
   }

   if ( m_Ptr )
   {
      size_t bytesWritten = ::fwrite( m_Ptr, 1, m_Count, file );
      if ( bytesWritten < m_Blob.size() )
      {
         DE_ERROR("bytesWritten(",bytesWritten,") < m_Count(",m_Count,")")
      }
   }
   else
   {
      DE_ERROR("No data to write ", uri )
   }

   ::fclose( file );
   return true;
}

bool
Binary::openMemory( uint8_t const * ptr, uint64_t count, std::string uri )
{
   m_Blob.clear();
   m_Ptr = const_cast< uint8_t* >( ptr );
   m_Count = count;
   m_Avail = count;
   m_Index = 0;
   m_Uri = uri;
   return true;
}

bool
Binary::load( std::string uri )
{
   m_Uri = uri;
   //DE_DEBUG("Load binary file ",uri)

   FILE* file = ::fopen( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open ", uri )
      return false;
   }

   ::fseeko64( file, 0, SEEK_END );
   size_t byteCount = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   if ( byteCount < 1 )
   {
      DE_ERROR("Empty file content ", uri )
      return false;
   }

   m_Blob.clear();
   try
   {
      m_Blob.resize( byteCount, 0x00 );
      size_t bytesRead = ::fread( m_Blob.data(), 1, m_Blob.size(), file );
      if ( bytesRead < byteCount )
      {
         DE_ERROR("bytesRead(",bytesRead,") < byteCount(",byteCount,")")
         return false;
      }
   }
   catch(...)
   {
      DE_ERROR("OOM")
      return false;
   }

   ::fclose( file );
   m_Ptr = m_Blob.data();
   m_Count = m_Blob.size();
   m_Avail = m_Blob.size();
   m_Index = 0;
   return true;
}

void
Binary::rewind()
{
   m_Index = 0;
   m_Avail = m_Count;
}

uint8_t
Binary::readU8()
{
   if ( !m_Ptr || m_Avail < 1 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t byte = *(m_Ptr + m_Index);
   m_Index += 1;
   m_Avail -= 1;
   return byte;
}

uint16_t
Binary::readU16_lsb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( a ) | ( size_t( b ) << 8 );
}

uint32_t
Binary::readU32_lsb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );
}

uint16_t
Binary::readU16_msb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( b ) | ( size_t( a ) << 8 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      return size_t( b ) | ( size_t( a ) << 8 );
}

uint32_t
Binary::readU32_msb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );
}

uint64_t
Binary::read( void* dst, uint64_t byteCount )
{
   if ( !m_Ptr )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   if ( byteCount > m_Avail )
   {
      byteCount = m_Avail;
   }

   if ( byteCount > 0 )
   {
      ::memcpy( dst, m_Ptr + m_Index, byteCount );
      m_Index += byteCount;
      m_Avail -= byteCount;
      //DE_DEBUG("CHUNK(",byteCount,"), m_Index(",m_Index,"), m_Avail(",m_Avail,")")
   }

   return byteCount;
}

bool
Binary::seek( uint64_t byteOffset, int dir )
{
   if ( !m_Ptr || m_Count == 0 || m_Avail == 0 )
   {
      //DE_DEBUG("Not loaded")
      return false;
   }

   if ( byteOffset >= m_Count ) // byteOffset must be in range[0,m_Count-1]
   {
      DE_DEBUG("Invalid offset(",byteOffset,"), size(",m_Count,")")
      return false;
   }

   if ( dir == SEEK_SET )
   {
      m_Index = byteOffset;
      m_Avail = m_Count - m_Index;
      return true;
   }
   else if ( dir == SEEK_END )
   {
      m_Index = m_Count - byteOffset;
      m_Avail = byteOffset;
      return true;
   }
   else if ( dir == SEEK_CUR )
   {
      m_Index += byteOffset;
      m_Avail -= byteOffset;
      return true;
   }
   else
   {
      DE_DEBUG("Invalid SEEK MODE ", dir )
      return false;
   }

}
*/

FileMagic::EFileMagic
FileMagic::getFileMagicFromFile(const std::string& uri)
{
    File file;
    if ( !file.open( uri.c_str(), eFileMode::Read ) )
    {
        DE_ERROR("Cant open ", uri )
        return Unknown;
    }

    const size_t byteCount = file.size();
    if ( byteCount < 32 )
    {
        DE_ERROR("byteCount(",byteCount,") < 32")
        return Unknown;
    }

    std::array<uint8_t,32> blob{ 0 };
    file.read(blob.data(), blob.size() );
    return getFileMagic( blob.data() );
}

// ===================
// ImageMagic
// ===================

bool
FileMagic::isJPG( void const* ptr )
{
    // FF D8 FF	—	Start of Image (SOI) marker for JPEG
    // FF D8 FF E0	—	JPEG with JFIF metadata
    // FF D8 FF E1	—	JPEG with EXIF metadata
    //uint32_t const IMAGE_MAGIC_JPG 	 = 0xFFD8FF00;
    //uint32_t const IMAGE_MAGIC_JPG_JFIF = 0x4A464946;
    //uint32_t const IMAGE_MAGIC_JPG_Exif = 0x45786966;
    //uint32_t const IMAGE_MAGIC_JPG_EXIF = 0x45584946;
    const uint8_t* p = reinterpret_cast< uint8_t const* >( ptr );
    const uint8_t m1 = *p++;
    const uint8_t m2 = *p++;
    const uint8_t m3 = *p++;
    const uint8_t m4 = *p++;
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF) && (m4==0xE0) ) return true; // JFIF
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF) && (m4==0xE1) ) return true; // EXIF
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF)) return true; // SOI
    return false;
}
bool
FileMagic::isPNG( void const* ptr )
{
   //   uint32_t const IMAGE_MAGIC_PNG_1    = 0x89504E47;
   //   uint32_t const IMAGE_MAGIC_PNG_2    = 0x0D0A1A0A;
   //   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
   //   uint32_t magic1 = *p++;
   //   uint32_t magic2 = *p++;
   //   if (( magic1 == IMAGE_MAGIC_PNG_1 ) && ( magic2 == IMAGE_MAGIC_PNG_2 ))
   //      return true;
   //   else
   //      return false;
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++; uint8_t m2 = *p++; uint8_t m3 = *p++; uint8_t m4 = *p++;
   uint8_t m5 = *p++; uint8_t m6 = *p++; uint8_t m7 = *p++; uint8_t m8 = *p++;
   if (  ( m1 == 0x89 ) && ( m2 == 0x50 ) && ( m3 == 0x4E ) && ( m4 == 0x47 )
      && ( m5 == 0x0D ) && ( m6 == 0x0A ) && ( m7 == 0x1A ) && ( m8 == 0x0A ) )
      return true;
   else
      return false;
}

bool
FileMagic::isWEBP( void const* ptr )
{
    const uint8_t * p = reinterpret_cast< uint8_t const* >( ptr );
    // WebP signature: "RIFF????WEBP"
    const uint8_t R = *p++;
    const uint8_t I = *p++;
    const uint8_t F = *p++;
    const uint8_t F2 = *p++;
    p += 4;
    const uint8_t W = *p++;
    const uint8_t E = *p++;
    const uint8_t B = *p++;
    const uint8_t P = *p++;
    return R == 'R' && I == 'I' && F == 'F' && F2 == 'F' &&
           W == 'W' && E == 'E' && B == 'B' && P == 'P';
}

bool
FileMagic::isBMP( void const* ptr )
{
   //uint16_t const IMAGE_MAGIC_BMP 		= 0x424D;
   //uint16_t magic = *reinterpret_cast< uint16_t const* >( ptr );
   //if ( magic == IMAGE_MAGIC_BMP )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   if (  ( m1 == 0x42 ) &&
         ( m2 == 0x4D ) )
      return true;
   else
      return false;
}
bool
FileMagic::isGIF( void const* ptr )
{
   //uint32_t const IMAGE_MAGIC_GIF_1 = 0x47494638;
   //uint16_t const IMAGE_MAGIC_GIF_2 = 0x3761;
   //uint16_t const IMAGE_MAGIC_GIF_3 = 0x3961;
//   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
//   uint32_t magic1 = *p++;
//   uint16_t const* q = reinterpret_cast< uint16_t const* >( p );
//   uint16_t magic2 = *q;
//   if ( ( magic1 == IMAGE_MAGIC_GIF_1 )
//    && (( magic2 == IMAGE_MAGIC_GIF_2 ) || ( magic2 == IMAGE_MAGIC_GIF_3 )) )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   uint8_t m3 = *p++;
   uint8_t m4 = *p++;
   uint8_t m5 = *p++;
   uint8_t m6 = *p++;
   if (  ( m1 == 0x47 ) &&
         ( m2 == 0x49 ) &&
         ( m3 == 0x46 ) &&
         ( m4 == 0x38 ) &&
         ( (( m5 == 0x37 ) && ( m6 == 0x61 )) ||
           (( m5 == 0x39 ) && ( m6 == 0x61 )) )
      )
      return true;
   else
      return false;
}
bool
FileMagic::isTIF( void const* ptr )
{
   uint32_t const IMAGE_MAGIC_TIFF 		= 0x49204900;
   uint32_t const IMAGE_MAGIC_TIFF_LE 	= 0x49492A00;
   uint32_t const IMAGE_MAGIC_TIFF_BE 	= 0x4D4D002A;

   uint32_t magic = *reinterpret_cast< uint32_t const* >( ptr );
   if (( magic == IMAGE_MAGIC_TIFF_LE ) ||
       ( magic == IMAGE_MAGIC_TIFF_BE ) ||
       ((magic & 0xFFFFFF00) == IMAGE_MAGIC_TIFF ))
      return true;
   else
      return false;
}

bool
FileMagic::isTGA( void const* ptr )
{

   // ============================================================================
   // Mime: image/x-tga
   // TGA	- Truevision Targa Graphic file
   // Trailer:
   // 54 52 55 45 56 49 53 49   TRUEVISI
   // 4F 4E 2D 58 46 49 4C 45   ON-XFILE
   // 2E 00                     ..

    return false;
}
bool
FileMagic::isPCX( void const* ptr )
{
   return false;
}

// ===================
// AudioMagic
// ===================

// CDA
// 52 49 46 46 xx xx xx xx == RIFF....
// 43 44 44 41 66 6D 74 20 == CDDAfmt

// WAV
// 52 49 46 46 xx xx xx xx == RIFF....
// 57 41 56 45 66 6D 74 20 == WAVEfmt

uint32_t const AUDIO_MAGIC_RIFF = 0x52494646; // 'R' 'I' 'F' 'F' - Bytes 0..3
uint32_t const AUDIO_MAGIC_CDDA = 0x43444441; // 'C' 'D' 'D' 'A' - Bytes 8..11
uint32_t const AUDIO_MAGIC_WAVE = 0x57415645; // 'W' 'A' 'V' 'E' - Bytes 8..11
uint32_t const AUDIO_MAGIC_fmts = 0x666D7420; // 'f' 'm' 't' ' ' - Bytes 12..15
uint32_t const AUDIO_MAGIC_data = 0x64617461; // 'd' 'a' 't' 'a' - Bytes 12..15

// FLAC
// 66 4C 61 43 00 00 00 22 == fLaC..."

uint32_t const AUDIO_MAGIC_FLAC_1 = 0x664C6143; // 'f' 'L' 'a' 'C'
uint32_t const AUDIO_MAGIC_FLAC_2 = 0x00000022; // '\0' '\0' '\0' '"'

// OGA, OGG, OGV, OGX - Ogg Vorbis Codec compressed Multimedia file
// 4F 67 67 53 00 02 00 00  OggS....
// 00 00 00 00 00 00	 	......

uint32_t const AUDIO_MAGIC_OGG_1 = 0x4F676753; // 'O' 'g' 'g' 'S'
uint32_t const AUDIO_MAGIC_OGG_2 = 0x00020000; // '\0' '\STX' '\0' '\0'
uint32_t const AUDIO_MAGIC_OGG_3 = 0x00000000; // '\0' '\0' '\0' '\0'
uint16_t const AUDIO_MAGIC_OGG_4 = 0x0000; // '\0' '\0'

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 33 67 70 35 = [4-Byte-Offset] ftyp3gp5

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 4D 53 4E 56 = [4-Byte-Offset] ftypMSNV

// M4V - MPEG-4 video|QuickTime file
// xx xx xx xx 66 74 79 70 6D 70 34 32 = [4-Byte-Offset] ftypmp42

// M4A - Apple Lossless Audio Codec file
// xx xx xx xx 66 74 79 70 4D 34 41 20 = [4-Byte-Offset] ftypM4A

// MPEG, MPG, MP3 - MPEG audio file frame  synch pattern
// FF Ex	 	ÿ.
// FF Fx	 	ÿ.
// 0xFF FB = 0b1111 1111 1111 1011
// 0xFF FA = 0b1111 1111 1111 1010

// AAAAAAAA AAABBCCD
// A	11	(31-21)	Frame sync (all bits must be set)
// B	2	(20,19)	MPEG Audio version ID
// 00 - MPEG Version 2.5 (later extension of MPEG 2)
// 01 - reserved
// 10 - MPEG Version 2 (ISO/IEC 13818-3)
// 11 - MPEG Version 1 (ISO/IEC 11172-3)
// Note: MPEG Version 2.5 was added lately to the MPEG 2 standard. It is an extension used for very low bitrate files, allowing the use of lower sampling frequencies. If your decoder does not support this extension, it is recommended for you to use 12 bits for synchronization instead of 11 bits.
//
// C	2	(18,17)	Layer description
// 00 - reserved
// 01 - Layer III
// 10 - Layer II
// 11 - Layer I

// AAC	 	MPEG-4 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F1    0b1111 1111 1111 0001

// AAC	 	MPEG-2 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F9	0b1111 1111 1111 1001

// REG	 	Windows Registry file ( scheiss Microsoft )
// FF FE    0b1111 1111 1111 1110

bool
FileMagic::isAAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isWAV( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP3( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP4( void const* ptr )
{
   return false;
}

bool
FileMagic::isFLAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isOGG( void const* ptr )
{
   return false;
}

bool
FileMagic::isWMA( void const* ptr )
{
   return false;
}

bool
FileMagic::isM4A( void const* ptr )
{
   return false;
}

bool FileMagic::isFLV( void const* ptr )
{
   return false;
}

// ===================

Recti::Recti( int dummy )
    : x(0), y(0), w(0), h(0)
{}

Recti::Recti( int32_t x_, int32_t y_, int32_t w_, int32_t h_ )
    : x(0), y(0), w(0), h(0)
{
    set( x_, y_, w_, h_ );
}

Recti::Recti( const Posi& pos, const Sizei& size )
    : x(pos.x), y(pos.y), w(size.w), h(size.h)
{}

Recti::Recti( Recti const & r )
    : x(r.x), y(r.y), w(r.w), h(r.h)
{}

Recti& Recti::operator=( Recti const & o )
{
    x = o.x;
    y = o.y;
    w = o.w;
    h = o.h;
    return *this;
}

bool
Recti::operator==( Recti const & o ) const
{
    return x == o.x && y == o.y && w == o.w && h == o.h;
}

bool
Recti::operator!=( Recti const & o ) const { return !( o == *this ); }

void
Recti::reset() { x = y = w = h = 0; }

void
Recti::setWidth( int32_t w_ ) { w = ( w_ >= 0 ) ? w_ : 0; }

void
Recti::setHeight( int32_t h_ ) { h = ( h_ >= 0 ) ? h_: 0; }

void
Recti::set( int32_t x_, int32_t y_, int32_t w_, int32_t h_ )
{
    x = x_;
    y = y_;
    setWidth( w_ );
    setHeight( h_ );
}

//static
Recti
Recti::fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 )
{
    if ( x1 > x2 ) std::swap( x1, x2 );
    if ( y1 > y2 ) std::swap( y1, y2 );
    int w = std::clamp( x2 - x1 + 1, 0, 1024*1024*1024 ); // clamp w in [0,enough]
    int h = std::clamp( y2 - y1 + 1, 0, 1024*1024*1024 ); // clamp h in [0,enough]
    return Recti( x1, y1, w, h );
}

void
Recti::addInternalPoint( glm::ivec2 const & p )
{
    addInternalPoint( p.x, p.y );
}

void
Recti::addInternalPoint( int32_t x_, int32_t y_ )
{
    // handle x
    if ( x_ < x ) // x is lower, reposition rect, increase rect size
    {
        int32_t delta = x - x_; // groesser - kleiner
        x = x_;
        w += delta;
    }
    else // x is inside rect ( nothing todo ), or right from it ( increase rect size )
    {
        int32_t x2 = x + w - 1;
        if ( x_ > x2 ) // x is right from rectangle ( must increase rect size, no pos change )
        {
            int32_t delta = x_ - x2; // groesser - kleiner
            w += delta; // increase w by portion that was not covered from x2 to _x
        }
        // else  // x is inside rect ( nothing todo )
    }

    // handle y
    if ( y_ < y )
    {
        int32_t delta = y - y_; // groesser - kleiner
        y = y_;
        h += delta;
    }
    else // y is inside rect ( nothing todo ), or below from it ( increase rect size )
    {
        int32_t y2 = y + h - 1;
        if ( y_ > y2 ) // y is below rectangle ( must increase rect size, no pos change )
        {
            int32_t delta = y_ - y2; // groesser - kleiner
            h += delta; // increase h by portion that was not covered from y2 to _y
        }
        // else  // y is inside rect ( nothing todo )
    }
}

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

int32_t Recti::centerX() const { return x + w/2; }
int32_t Recti::centerY() const { return y + h/2; }
int32_t Recti::x1() const { return x; }
int32_t Recti::y1() const { return y; }
int32_t Recti::x2() const { return w > 1 ? x + w - 1 : x; }
int32_t Recti::y2() const { return h > 1 ? y + h - 1 : y; }
Posi    Recti::pos() const { return { x, y }; }
Sizei   Recti::size() const { return { w, h }; }
Posi    Recti::center() const { return { centerX(), centerY() }; }
Posi    Recti::topLeft() const { return { x, y }; }
Posi    Recti::bottomRight() const { return { x2(), y2() }; }

// int32_t     Recti::getX() const { return x(); }
// int32_t     Recti::getY() const { return y(); }
// int32_t     Recti::getWidth() const { return w(); }
// int32_t     Recti::getHeight() const { return h(); }
// int32_t     Recti::getCenterX() const { return centerX(); }
// int32_t     Recti::getCenterY() const { return centerY(); }
// int32_t     Recti::getX1() const { return x1(); }
// int32_t     Recti::getY1() const { return y1(); }
// int32_t     Recti::getX2() const { return x2(); }
// int32_t     Recti::getY2() const { return y2(); }
// glm::ivec2  Recti::getPos() const { return pos(); }
// glm::ivec2  Recti::getSize() const { return size(); }
// glm::ivec2  Recti::getCenter() const { return center(); }
// glm::ivec2  Recti::getTopLeft() const { return topLeft(); }
// glm::ivec2  Recti::getBottomRight() const { return bottomRight(); }

std::string Recti::str() const
{
    std::ostringstream o;
    o << x << "," << y << "," << w << "," << h;
    return o.str();
}

bool Recti::contains( int mx, int my, int touchWidth ) const
{
    if ( mx < x1() + touchWidth ) return false;
    if ( my < y1() + touchWidth ) return false;
    if ( mx > x2() - touchWidth ) return false;
    if ( my > y2() - touchWidth ) return false;
    return true;
}

glm::vec4 Recti::toVec4( int atlas_w, int atlas_h ) const
{
    float fx = float( x ) / float( atlas_w-1 );
    float fy = float( y ) / float( atlas_h-1 );
    float fw = float( w ) / float( atlas_w );
    float fh = float( h ) / float( atlas_h );
    return glm::vec4( fx, fy, fw, fh );
}

void
Recti::test()
{
    // DE_DEBUG("typename(int32_t) = ", typename(int32_t) )
    DE_DEBUG("s_Min = ", s_Min )
    DE_DEBUG("s_Max = ", s_Max )

    Recti a( 0,0,100,50);
    Recti b( 50,50,33,66);
    Recti c( 33,33,21,123);
    DE_DEBUG("abc_width = ", a.w + b.w + c.w )
    DE_DEBUG("abc_height = ", a.h + b.h + c.h )

    //      if ( a.getWidth() + b.getWidth() + c.getWidth() != 239 )
    //      {
    //         throw std::runtime_error( "Rect< int32_t >.getWidth() failed" );
    //      }

    //      if ( a.getHeight() + b.getHeight() + c.getHeight() != 239 )
    //      {
    //         throw std::runtime_error( "Rect< int32_t >.getHeight() failed" );
    //      }
}

// ===================
Rectf::Rectf()
// ===================
    : m_x( 0 )
    , m_y( 0 )
    , m_w( 0 )
    , m_h( 0 )
{}

Rectf::Rectf( float x, float y, float w, float h )
    : m_x( x )
    , m_y( y )
    , m_w( w )
    , m_h( h )
{}

Rectf::Rectf( Rectf const & other )
    : m_x( other.m_x )
    , m_y( other.m_y )
    , m_w( other.m_w )
    , m_h( other.m_h )
{}

float
Rectf::computeU1( int32_t x1, int32_t w, bool useOffset )
{
    if ( w < 2 ) return float(0);
    if ( useOffset )
        return ( float(0.5) + float( x1 )) / float( w );
    else
        return float( x1 ) / float( w );
}

float
Rectf::computeV1( int32_t y1, int32_t h, bool useOffset )
{
    if ( h < 2 ) return float(0);
    if ( useOffset )
        return ( float(0.5) + float( y1 )) / float( h );
    else
        return float( y1 ) / float( h );
}

float
Rectf::computeU2( int32_t x2, int32_t w, bool useOffset )
{
    if ( w < 2 ) return float(1);
    if ( useOffset )
        return ( float(0.5) + float( x2 )) / float( w );
    else
        return float( x2+1 ) / float( w );
}

float
Rectf::computeV2( int32_t y2, int32_t h, bool useOffset )
{
    if ( h < 2 ) return float(1);
    if ( useOffset )
        return ( float(0.5) + float( y2 )) / float( h );
    else
        return float( y2+1 ) / float( h );
}

Rectf
Rectf::fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset )
{
    int32_t x1 = pos.x1();
    int32_t y1 = pos.y1();
    int32_t x2 = pos.x2();
    int32_t y2 = pos.y2();
    float u1 = computeU1( x1, w, useOffset );
    float v1 = computeV1( y1, h, useOffset );
    float u2 = computeU2( x2, w, useOffset );
    float v2 = computeV2( y2, h, useOffset );
    return Rectf( u1, v1, u2-u1, v2-v1 );
}

std::string
Rectf::toString() const
{
    std::ostringstream s;
    s << m_x << "," << m_y << "," << m_w << "," << m_h;
    return s.str();
}

void
Rectf::zero()
{
    m_x = m_y = m_w = m_h = float(0);
}

Rectf
Rectf::identity() { return Rectf( float(0), float(0), float(1), float(1) ); }

float Rectf::x() const { return m_x; }
float Rectf::y() const { return m_y; }
float Rectf::w() const { return m_w; }
float Rectf::h() const { return m_h; }
float Rectf::x1() const { return x(); }
float Rectf::y1() const { return y(); }
float Rectf::x2() const { return x()+w(); }
float Rectf::y2() const { return y()+h(); }
float Rectf::centerX() const { return m_x + float( 0.5 ) * w(); }
float Rectf::centerY() const { return m_y + float( 0.5 ) * h(); }

glm::tvec2< float > Rectf::center() const { return { centerX(), centerY() }; }
glm::tvec2< float > Rectf::topLeft() const { return { x1(), y1() }; }
glm::tvec2< float > Rectf::bottomRight() const { return { x2(), y2() }; }

float Rectf::getX() const { return x(); }
float Rectf::getY() const { return y(); }
float Rectf::getWidth() const { return w(); }
float Rectf::getHeight() const { return h(); }
float Rectf::getX1() const { return x1(); }
float Rectf::getY1() const { return y1(); }
float Rectf::getX2() const { return x2(); }
float Rectf::getY2() const { return y2(); }
float Rectf::getCenterX() const { return centerX(); }
float Rectf::getCenterY() const { return centerY(); }
glm::tvec2< float > Rectf::getCenter() const { return center(); }
glm::tvec2< float > Rectf::getTopLeft() const { return topLeft(); }
glm::tvec2< float > Rectf::getBottomRight() const { return bottomRight(); }

Recti
Rectf::toRecti( int image_w, int image_h ) const
{
    int x = int(std::roundf( m_x * float(image_w) ));
    int y = int(std::roundf( m_y * float(image_h) ));
    int w = int(std::roundf( m_w * float(image_w) ));
    int h = int(std::roundf( m_h * float(image_h) ));
    return Recti( x, y, w, h );
}

void Rectf::addInternalPoint( glm::tvec2< float > const & point )
{
    addInternalPoint( point.x, point.y );
}

void Rectf::addInternalPoint( float x, float y )
{
    // handle _x
    if ( x < m_x )  // _x is lower, reposition rect, increase rect size
    {
        float delta = m_x - x; // groesser - kleiner
        m_x = x;
        m_w += delta;
    }
    else  // _x is inside rect ( nothing todo ), or right from it ( increase rect size )
    {
        float x2 = m_x + m_w - 1;
        if ( x > x2 ) // _x is right from rectangle ( must increase rect size, no pos change )
        {
            float delta = x - x2; // groesser - kleiner
            m_w += delta; // increase w by portion that was not covered from x2 to _x
        }
        // else  // _x is inside rect ( nothing todo )
    }

    // handle _y
    if ( y < m_y )
    {
        float delta = m_y - y; // groesser - kleiner
        m_y = y;
        m_h += delta;
    }
    else // _y is inside rect ( nothing todo ), or below from it ( increase rect size )
    {
        float y2 = m_y + m_h - 1;
        if ( y > y2 ) // _y is below rectangle ( must increase rect size, no pos change )
        {
            float delta = y - y2; // groesser - kleiner
            m_h += delta; // increase h by portion that was not covered from y2 to _y
        }
        // else  // _y is inside rect ( nothing todo )
    }
}

float Rectf::u1() const { return x1(); }
float Rectf::v1() const { return y1(); }
float Rectf::u2() const { return x2(); }
float Rectf::v2() const { return y2(); }
float Rectf::du() const { return w(); }
float Rectf::dv() const { return h(); }

float Rectf::getU1() const { return x1(); }
float Rectf::getV1() const { return y1(); }
float Rectf::getU2() const { return x2(); }
float Rectf::getV2() const { return y2(); }
float Rectf::getDU() const { return w(); }
float Rectf::getDV() const { return h(); }

} // end namespace de.

// ========================================================================
bool dbMouseOver( int mx, int my, int x1, int y1, int x2, int y2 )
// ========================================================================
{
    if ( x1 > x2 ) std::swap( x1, x2 );
    if ( y1 > y2 ) std::swap( y1, y2 );
    if ( x2 - x1 < 1 ) return false;
    if ( y2 - y1 < 1 ) return false;
    if ( mx < x1 ) return false;
    if ( my < y1 ) return false;
    if ( mx > x2 ) return false;
    if ( my > y2 ) return false;
    return true;
}

// ========================================================================
bool dbMouseOver( int mx, int my, const de::Recti& pos )
// ========================================================================
{
    const int x1 = pos.x;
    const int y1 = pos.y;
    const int x2 = pos.x + pos.w - 1;
    const int y2 = pos.y + pos.h - 1;
    return dbMouseOver(mx,my,x1,y1,x2,y2);
}

std::string dbStrVal(float val, int digits)
{
    int mg = val * std::pow(10.0f, digits );
    std::string a = std::to_string(mg);
    a.insert(a.begin() + a.size() - digits, '.');
    return a;
}

std::string dbStrVal(double val, int digits)
{
    int mg = val * std::pow(10.0, digits );
    std::string a = std::to_string(mg);
    a.insert(a.begin() + a.size() - digits, '.');
    return a;
}

std::string dbHex( uint8_t byte )
{
    return de::StringUtil::hex( byte );
}

std::string dbHex( uint16_t const color )
{
    return de::StringUtil::hex( color );
}

std::string dbHex( uint32_t const color )
{
    return de::StringUtil::hex( color );
}

std::string dbHex( uint64_t color )
{
    return de::StringUtil::hex( color );
}

std::string dbHex( uint8_t const* beg, uint8_t const* end )
{
    return de::StringUtil::hex( beg, end );
}

std::string dbHex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
{
    return de::StringUtil::hex( beg, end, nBytesPerRow );
}

std::string dbStrNanoSeconds(double nSeconds)
{
    return de::StringUtil::nanoseconds( nSeconds );
}

std::string dbStrSeconds(double nSeconds)
{
    return de::StringUtil::seconds( nSeconds );
}

std::string dbStrBytes(uint64_t nBytes)
{
    return de::StringUtil::bytes( nBytes );
}

std::string de_mbstr(const std::wstring& w ) { return de::StringUtil::to_str( w ); }
std::string de_mbstr( wchar_t const w ) { return de::StringUtil::to_str( w ); }
std::wstring de_wstr(const std::string& mb ) { return de::StringUtil::to_wstr( mb ); }

bool
dbLoadBlob( de::Blob & blob, const std::string& uri, const int64_t sizeLimit )
{
    return de::FileSystem::loadBlob(blob,uri,sizeLimit);
}
bool
dbSaveBlob( const de::Blob& blob, const std::string& uri )
{
    return de::FileSystem::saveBlob(blob,uri);
}
std::string
dbLoadText(const std::string& uri)
{
    return de::FileSystem::loadStr( uri );
}
std::wstring
dbLoadText(const std::wstring& uri)
{
    return de::FileSystem::loadStrW( uri );
}

StringListA
dbStrSplit(const std::string& txt, char searchChar, bool bKeepEmptyLines )
{
    return de::StringUtil::split( txt, searchChar, bKeepEmptyLines );
}

StringListA
dbLoadTextLn(const std::string& uri)
{
    auto content = de::FileSystem::loadStr( uri );
    return dbStrSplit( content, '\n', false);
}

StringListA
dbLoadTextLn(const std::wstring& uri)
{
    auto content = de::FileSystem::loadStr( uri );
    return dbStrSplit( content, '\n', false);
}

bool dbSaveText(const std::string& uri, const std::string& txt) { return de::FileSystem::saveStr( uri, txt ); }
bool dbSaveText(const std::wstring& uri, const std::wstring& txt) { return de::FileSystem::saveStr( uri, txt ); }

bool dbExistFile(const std::string& uri) { return de::FileSystem::existFile( uri ); }
bool dbExistDirectory(const std::string& uri) { return de::FileSystem::existDirectory( uri ); }

bool dbExistFile(const std::wstring& uri) { return de::FileSystem::existFile( uri ); }
bool dbExistDirectory(const std::wstring& uri) { return de::FileSystem::existDirectory( uri ); }

void dbStrLowerCase(std::string& txt) { return de::StringUtil::lowerCase(txt); }
void dbStrUpperCase(std::string& txt) { return de::StringUtil::upperCase(txt); }

void dbStrLowerCase(std::wstring& txt) { return de::StringUtil::lowerCase(txt); }
void dbStrUpperCase(std::wstring& txt) { return de::StringUtil::upperCase(txt); }

std::string dbStrLower(const std::string& txt) { return de::StringUtil::makeLower(txt); }
std::string dbStrUpper(const std::string& txt) { return de::StringUtil::makeUpper(txt); }

std::wstring dbStrLower(const std::wstring& txt) { return de::StringUtil::makeLower(txt); }
std::wstring dbStrUpper(const std::wstring& txt) { return de::StringUtil::makeUpper(txt); }

std::string dbStrReplace(const std::string& txt,
                         const std::string& from,
                         const std::string& to, size_t* nReplacements )
{
    return de::StringUtil::replace( txt, from, to, nReplacements );
}

std::wstring dbStrReplace(const std::wstring& txt,
                          const std::wstring& from,
                          const std::wstring& to, size_t* nReplacements )
{
    return de::StringUtil::replace( txt, from, to, nReplacements );
}

bool dbStrBeginsWith( const std::string& txt, const std::string& query ) { return de::StringUtil::startsWith(txt,query); }
bool dbStrBeginsWith( const std::wstring& txt, const std::wstring& query ) { return de::StringUtil::startsWith(txt,query); }
bool dbStrBeginsWith( const std::string& txt, char c ) { return de::StringUtil::startsWith(txt,c); }
bool dbStrBeginsWith( const std::wstring& txt, wchar_t c ) { return de::StringUtil::startsWith(txt,c); }

bool dbStrEndsWith( const std::string& txt, const std::string& query ) { return de::StringUtil::endsWith(txt,query); }
bool dbStrEndsWith( const std::wstring& txt, const std::wstring& query ) { return de::StringUtil::endsWith(txt,query); }
bool dbStrEndsWith( const std::string& txt, char c ) { return de::StringUtil::endsWith(txt,c); }
bool dbStrEndsWith( const std::wstring& txt, wchar_t c ) { return de::StringUtil::endsWith(txt,c); }

void dbRemoveFile( const std::string& uri )
{
    de::FileSystem::removeFile(uri);
}
void dbRemoveFile( const std::wstring& uri )
{
    de::FileSystem::removeFile(uri);
}

int64_t dbFileSize( const std::string & uri )
{
    return de::FileSystem::fileSize( uri );
}

int64_t dbFileSize( const std::wstring & uri )
{
    return de::FileSystem::fileSize( uri );
}

std::string dbFileName( const std::string& uri, const std::string& relativeToPath )
{
    return de::FileSystem::fileName( uri, relativeToPath );
}

std::wstring dbFileName( const std::wstring& uri, const std::wstring& relativeToPath )
{
    return de::FileSystem::fileName( uri, relativeToPath );
}

std::string dbFileBase( const std::string& uri )
{
    return de::FileSystem::fileBase( uri );
}

std::wstring dbFileBase( const std::wstring& uri )
{
    return de::FileSystem::fileBase( uri );
}

std::string dbFileSuffix( const std::string& uri )
{
    return de::FileSystem::fileSuffix( uri );
}

std::wstring dbFileSuffix( const std::wstring& uri )
{
    return de::FileSystem::fileSuffix( uri );
}

std::string dbFileDir( const std::string& uri )
{
    return de::FileSystem::fileDir( uri );
}

std::wstring dbFileDir( const std::wstring& uri )
{
    return de::FileSystem::fileDir( uri );
}

std::string dbParentDir( const std::string& uri )
{
    return de::FileSystem::parentDir(uri);
}

std::wstring dbParentDir( const std::wstring& uri )
{
    return de::FileSystem::parentDir(uri);
}

std::string dbMakePosix( const std::string & uri )
{
    return de::FileSystem::makePosixPath(uri);
}

std::wstring dbMakePosix( const std::wstring & uri )
{
    return de::FileSystem::makePosixPath(uri);
}

std::string dbMakeNT( const std::string & uri )
{
    return de::FileSystem::makeWinPath(uri);
}

std::wstring dbMakeNT( const std::wstring & uri )
{
    return de::FileSystem::makeWinPath(uri);
}
