#pragma once
#include <juce_core/zip/juce_ZipFile.h>
#include <juce_core/memory/juce_ByteOrder.h>
#include <juce_core/streams/juce_BufferedInputStream.h>
#include <juce_core/streams/juce_FileInputSource.h>
#include <juce_core/streams/juce_MemoryOutputStream.h>
#include <juce_core/files/juce_FileOutputStream.h>
#include <juce_core/zip/juce_GZIPCompressorOutputStream.h>
#include <juce_core/files/juce_FileInputStream.h>
#include <juce_core/zip/juce_GZIPDecompressorInputStream.h>
#include <juce_core/logging/juce_Logger.h> // jassert impl

namespace juce
{

JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4309 4305 4365 6385 6326 6340)

namespace zlibNamespace
{
 #if JUCE_INCLUDE_ZLIB_CODE
  JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wconversion",
                                       "-Wsign-conversion",
                                       "-Wshadow",
                                       "-Wdeprecated-register",
                                       "-Wswitch-enum",
                                       "-Wswitch-default",
                                       "-Wredundant-decls",
                                       "-Wimplicit-fallthrough",
                                       "-Wzero-as-null-pointer-constant",
                                       "-Wcomma")

  #undef OS_CODE
  #undef fdopen
  #define ZLIB_INTERNAL
  #define NO_DUMMY_DECL
  #include "zlib/zlib.h"
  #include "zlib/adler32.c"
  #include "zlib/compress.c"
  #undef DO1
  #undef DO8
  #include "zlib/crc32.c"
  #include "zlib/deflate.c"
  #include "zlib/inffast.c"
  #undef PULLBYTE
  #undef LOAD
  #undef RESTORE
  #undef INITBITS
  #undef NEEDBITS
  #undef DROPBITS
  #undef BYTEBITS
  #include "zlib/inflate.c"
  #include "zlib/inftrees.c"
  #include "zlib/trees.c"
  #include "zlib/zutil.c"
  #undef Byte
  #undef fdopen
  #undef local
  #undef Freq
  #undef Code
  #undef Dad
  #undef Len

  JUCE_END_IGNORE_WARNINGS_GCC_LIKE
 #else
  #include JUCE_ZLIB_INCLUDE_PATH

  #ifndef z_uInt
   #ifdef uInt
    #define z_uInt uInt
   #else
    #define z_uInt unsigned int
   #endif
  #endif

 #endif
}

JUCE_END_IGNORE_WARNINGS_MSVC

} // namespace juce
