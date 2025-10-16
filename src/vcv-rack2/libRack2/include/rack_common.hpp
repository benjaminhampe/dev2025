#pragma once
// Include most of the C stdlib for convenience
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cstdarg>
#include <climits>
#include <cmath>
#include <cstring>
#include <cassert>

// Include some of the C++ stdlib for convenience
#include <string>
#include <stdexcept>

// Check CPU
#if defined __x86_64__
	#define ARCH_X64 1
#endif
#if defined __aarch64__
	#define ARCH_ARM64 1
#endif

// Check OS
#if defined _WIN32
	#define ARCH_WIN 1
#endif
#if defined __APPLE__
	#define ARCH_MAC 1
#endif
#if defined __linux__
	#define ARCH_LIN 1
#endif

#ifdef _WIN32
  #ifdef BUILDING_RACK_DLL
    #define RACK_DLL_API __declspec(dllexport)
  #else
    #define RACK_DLL_API __declspec(dllimport)
  #endif
  #define RACK_DLL_CALL __stdcall
#else
  #define RACK_DLL_API __attribute__((visibility("default")))
  #define RACK_DLL_CALL
#endif


/** Attribute for deprecated functions and symbols.
E.g.

    DEPRECATED void foo();
*/
#define DEPRECATED __attribute__((deprecated))

/** Attribute for private functions not intended to be called by plugins.
When #including rack.hpp, attempting to call PRIVATE functions will result in a compile-time error.
*/
#ifndef PRIVATE
#define PRIVATE
#endif


/** Concatenates two literals or two macros
Example:

    #define COUNT 42
    CONCAT(myVariable, COUNT)

expands to

    myVariable42
*/
#define CONCAT_LITERAL(x, y) x ## y
#define CONCAT(x, y) CONCAT_LITERAL(x, y)


/** Surrounds raw text with quotes
Example:

    #define NAME "world"
    printf("Hello " TOSTRING(NAME))

expands to

    printf("Hello " "world")

and of course the C++ lexer/parser then concatenates the string literals.
*/
#define TOSTRING_LITERAL(x) #x
#define TOSTRING(x) TOSTRING_LITERAL(x)


/** Produces the length of a static array in number of elements */
#define LENGTHOF(arr) (sizeof(arr) / sizeof((arr)[0]))


/** Reserve space for `count` enums starting with `name`.
Example:

    enum Foo {
        ENUMS(BAR, 14),
        BAZ
    };

`BAR + 0` to `BAR + 13` is reserved. `BAZ` has a value of 14.
*/
#define ENUMS(name, count) name, name ## _LAST = name + (count) - 1


/** References binary files compiled into the program.
For example, to include a file "Test.dat" directly into your program binary, add

    BINARIES += Test.dat

to your Makefile and declare

    BINARY(Test_dat)

at the root of a .c or .cpp source file. Note that special characters like "." are replaced with "_". Then use

    BINARY_START(Test_dat)
    BINARY_END(Test_dat)

to reference the data beginning and end as an unsigned char* array, and

    BINARY_SIZE(Test_dat)

to get its size in bytes.
*/
// Use synbols generated from `xxd -i`
#define BINARY(sym) extern "C" {extern const unsigned char sym[]; extern const unsigned int sym##_len;}
#define BINARY_START(sym) (sym)
#define BINARY_END(sym) (sym + sym##_len)
#define BINARY_SIZE(sym) (sym##_len)


/** Helpful user-defined literals for specifying exact integer and float types.
Usage examples:
    42_i8
    -4242_u16
    0x4a2b_i32
    0b11010111000000_u64
    42_f32
    4.2e-4_f64
*/
inline int8_t operator"" _i8(unsigned long long x) {return x;}
inline int16_t operator"" _i16(unsigned long long x) {return x;}
inline int32_t operator"" _i32(unsigned long long x) {return x;}
inline int64_t operator"" _i64(unsigned long long x) {return x;}
inline uint8_t operator"" _u8(unsigned long long x) {return x;}
inline uint16_t operator"" _u16(unsigned long long x) {return x;}
inline uint32_t operator"" _u32(unsigned long long x) {return x;}
inline uint64_t operator"" _u64(unsigned long long x) {return x;}
inline float operator"" _f32(long double x) {return x;}
inline float operator"" _f32(unsigned long long x) {return x;}
inline double operator"" _f64(long double x) {return x;}
inline double operator"" _f64(unsigned long long x) {return x;}


#if defined ARCH_WIN
// wchar_t on Windows should be 2 bytes
static_assert(sizeof(wchar_t) == 2);

// Windows C standard functions are ASCII-8 instead of UTF-8, so redirect these functions to wrappers which convert to UTF-8
#define fopen fopen_u8

extern "C" {
RACK_DLL_API FILE* RACK_DLL_CALL fopen_u8(const char* filename, const char* mode);
}

namespace std {
    using ::fopen_u8;
}
#endif


/** Root namespace for the Rack API */
namespace rack {


/** Casts a primitive, preserving its bits instead of converting. */
template <typename To, typename From>
To bitCast(From from) {
    static_assert(sizeof(From) == sizeof(To), "Types must be the same size");
    To to;
    // This is optimized out
    std::memcpy(&to, &from, sizeof(From));
    return to;
}


/** C#-style property constructor
Example:

    Foo *foo = construct<Foo>(&Foo::greeting, "Hello world", &Foo::legs, 2);
*/
template <typename T>
T* construct() {
    return new T;
}

template <typename T, typename F, typename V, typename... Args>
T* construct(F f, V v, Args... args) {
    T* o = construct<T>(args...);
    o->*f = v;
    return o;
}


/** Defers running code until the scope is destructed

From http://www.gingerbill.org/article/defer-in-cpp.html.
Example:

    file = fopen(...);
    DEFER({
        fclose(file);
    });
*/
template <typename F>
struct DeferWrapper {
    F f;
    DeferWrapper(F f) : f(f) {}
    ~DeferWrapper() {
        f();
    }
};

template <typename F>
DeferWrapper<F> deferWrapper(F f) {
    return DeferWrapper<F>(f);
}

#define DEFER(code) auto CONCAT(_defer_, __COUNTER__) = rack::deferWrapper([&]() code)


/** An exception explicitly thrown by Rack or a Rack plugin.
Can be subclassed to throw/catch specific custom exceptions.
*/
struct RACK_DLL_API Exception : std::exception {
    std::string msg;

    // Attribute index 1 refers to `Exception*` argument so use 2.
    __attribute__((format(printf, 2, 3)))
    Exception(const char* format, ...);
    Exception(const std::string& msg) : msg(msg) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};


/** Given a std::map `c`, returns the value of the given key, or returns `def` if the key doesn't exist.
Does *not* add the default value to the map.

Posted to https://stackoverflow.com/a/63683271/272642.
Example:

    std::map<std::string, int> m;
    int v = get(m, "a", 3);
    // v is 3 because the key "a" does not exist

    int w = get(m, "a");
    // w is 0 because no default value is given, so it assumes the default int.
*/
template <typename C>
const typename C::mapped_type& get(const C& c, const typename C::key_type& key, const typename C::mapped_type& def = typename C::mapped_type()) {
    typename C::const_iterator it = c.find(key);
    if (it == c.end())
        return def;
    return it->second;
}


/** Given a std::vector `c`, returns the value of the given index `i`, or returns `def` if the index is out of bounds.
*/
template <typename C>
const typename C::value_type& get(const C& c, typename C::size_type i, const typename C::value_type& def = typename C::value_type()) {
    if (i >= c.size())
        return def;
    return c[i];
}


// config

extern RACK_DLL_API const std::string APP_NAME;
extern RACK_DLL_API const std::string APP_EDITION;
extern RACK_DLL_API const std::string APP_EDITION_NAME;
extern RACK_DLL_API const std::string APP_VERSION_MAJOR;
extern RACK_DLL_API const std::string APP_VERSION;
extern RACK_DLL_API const std::string APP_OS;
extern RACK_DLL_API const std::string APP_OS_NAME;
extern RACK_DLL_API const std::string APP_CPU;
extern RACK_DLL_API const std::string APP_CPU_NAME;
extern RACK_DLL_API const std::string API_URL;


} // namespace rack


// Logger depends on common.hpp, but it is handy to include it along with common.hpp.

/** Example usage:

    DEBUG("error: %d", errno);

will print something like

    [0.123 debug myfile.cpp:45] error: 67
*/

#ifndef RK_DEBUG
#define RK_DEBUG(format, ...) rack::logger::log(rack::logger::DEBUG_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif
#ifndef RK_INFO
#define RK_INFO(format, ...) rack::logger::log(rack::logger::INFO_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif
#ifndef RK_WARN
#define RK_WARN(format, ...) rack::logger::log(rack::logger::WARN_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif
#ifndef RK_FATAL
#define RK_FATAL(format, ...) rack::logger::log(rack::logger::FATAL_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

namespace rack {
/** Logs messages to a file or the terminal */
namespace logger {

enum Level {
    DEBUG_LEVEL,
    INFO_LEVEL,
    WARN_LEVEL,
    FATAL_LEVEL
};

/** Returns whether logger was successfully initialized. */
RACK_DLL_API bool RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL destroy();
/** Do not use this function directly. Use the macros above.
Thread-safe, meaning messages cannot overlap each other in the log.
*/
__attribute__((format(printf, 5, 6)))
RACK_DLL_API void RACK_DLL_CALL log(Level level, const char* filename, int line, const char* func, const char* format, ...);
/** Returns whether the last log file failed to end properly, due to a possible crash.
*/
RACK_DLL_API bool RACK_DLL_CALL wasTruncated();

extern RACK_DLL_API std::string logPath;


} // namespace logger
} // namespace rack
