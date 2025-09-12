#include <de/Byteswap.hpp>

#if defined(WIN32) || defined(_WIN32)
   #ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #endif
   #ifdef _WIN32_WINNT
   #undef _WIN32_WINNT
   #endif
   #define _WIN32_WINNT 0x0600
   #include <windows.h>
   #include <winnt.h>
#else
   #include <stdio.h>
   #include <time.h>
   #include <sys/time.h>
#endif

// #if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
   // #include <SDL/SDL_endian.h>
   // #define bswap_16(X) SDL_Swap16(X)
   // #define bswap_32(X) SDL_Swap32(X)
// #elif defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER > 1298)
   // #include <stdlib.h>
   // #define bswap_16(X) _byteswap_ushort(X)
   // #define bswap_32(X) _byteswap_ulong(X)
// #if (_MSC_VER >= 1400)
   // #define localtime _localtime_s
// #endif
// #elif defined(_IRR_OSX_PLATFORM_)
   // #include <libkern/OSByteOrder.h>
   // #define bswap_16(X) OSReadSwapInt16(&X,0)
   // #define bswap_32(X) OSReadSwapInt32(&X,0)
// #elif defined(__FreeBSD__) || defined(__OpenBSD__)
   // #include <sys/endian.h>
   // #define bswap_16(X) bswap16(X)
   // #define bswap_32(X) bswap32(X)
// #elif !defined(_IRR_SOLARIS_PLATFORM_) && !defined(__PPC__) && !defined(_IRR_WINDOWS_API_)
   // #include <byteswap.h>
// #else
   // #define bswap_16(X) ((((X)&0xFF) << 8) | (((X)&0xFF00) >> 8))
   // #define bswap_32(X) ( (((X)&0x000000FF)<<24) | (((X)&0xFF000000) >> 24) | (((X)&0x0000FF00) << 8) | (((X) &0x00FF0000) >> 8))
// #endif


namespace de
{
/*
   u16 Byteswap::byteswap(u16 num) {return bswap_16(num);}
   s16 Byteswap::byteswap(s16 num) {return bswap_16(num);}
   u32 Byteswap::byteswap(u32 num) {return bswap_32(num);}
   s32 Byteswap::byteswap(s32 num) {return bswap_32(num);}
   f32 Byteswap::byteswap(f32 num) {u32 tmp=IR(num); tmp=bswap_32(tmp); return (FR(tmp));}
   // prevent accidental byte swapping of chars
   u8  Byteswap::byteswap(u8 num)  {return num;}
   c8  Byteswap::byteswap(c8 num)  {return num;}
*/

} // end namespace de