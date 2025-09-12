#pragma once
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 8
#define IRRLICHT_VERSION_REVISION 4
#define IRRLICHT_SDK_VERSION "1.8.5"
#include <stdio.h>

#define __IRR_HAS_S64
#define _IRR_COMPILE_WITH_CONSOLE_DEVICE_
#define _IRR_MATERIAL_MAX_TEXTURES_ 4
#define _IRR_COMPILE_WITH_OPENGL_
#define _IRR_OPENGL_USE_EXTPOINTER_
#define _IRR_COMPILE_WITH_GUI_
// Freshly deactivated:
//#define _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
//#define _IRR_COMPILE_WITH_SOFTWARE_
//#define _IRR_COMPILE_WITH_BURNINGSVIDEO_
//#define BURNINGVIDEO_RENDERER_BEAUTIFUL // 32 Bit + Vertexcolor + Lighting + Per Pixel Perspective Correct + SubPixel/SubTexel Correct + Bilinear Texturefiltering + WBuffer
// Old deactive:
//#define BURNINGVIDEO_RENDERER_FAST // 32 Bit + Per Pixel Perspective Correct + SubPixel/SubTexel Correct + WBuffer + Bilinear Dithering TextureFiltering
//#define BURNINGVIDEO_RENDERER_ULTRA_FAST // 16Bit + SubPixel/SubTexel Correct + ZBuffer
//#define BURNINGVIDEO_RENDERER_CE
//#define _IRR_SCENEMANAGER_DEBUG
//#define _IRR_COMPILE_WITH_SDL_DEVICE_

#if defined(_IRR_WINDOWS_API_)
   //#define _IRR_COMPILE_WITH_DIRECT3D_8_
   //#define _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
   //#define _IRR_COMPILE_WITH_DIRECT3D_9_
   //#define _IRR_D3D_USE_LEGACY_HLSL_COMPILER
   //#define _IRR_COMPILE_WITH_CG_
   #undef _IRR_USE_NVIDIA_PERFHUD_ // <http://developer.nvidia.com/object/nvperfhud_home.html>.
   #define _IRR_D3D_NO_SHADER_DEBUGGING

#elif defined(_IRR_LINUX_PLATFORM_)
   //! On some Linux systems the XF86 vidmode extension or X11 RandR are missing. Use these flags
   //! to remove the dependencies such that Irrlicht will compile on those systems, too.
   //! If you don't need colored cursors you can also disable the Xcursor extension
   #define _IRR_COMPILE_WITH_X11_
   #define _IRR_LINUX_X11_VIDMODE_
   #define _IRR_LINUX_X11_RANDR_
   #define _IRR_LINUX_XCURSOR_
   //! X11 has by default only monochrome cursors, but using the Xcursor library we can also get color cursor support.
   //! If you have the need for custom color cursors on X11 then enable this and make sure you also link
   //! to the Xcursor library in your Makefile/Projectfile.
#endif

#ifndef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_ // ZIP and GZIP archives
#endif
#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
   #define _IRR_COMPILE_WITH_ZLIB_
   #define _IRR_COMPILE_WITH_ZIP_ENCRYPTION_ // read AES-encrypted ZIP archives
   #define _IRR_COMPILE_WITH_BZIP2_ // bzip2 is superior to the original zip file compression modes
   #define _IRR_COMPILE_WITH_LZMA_
   //  #define _IRR_USE_NON_SYSTEM_ZLIB_
   //  #define _IRR_USE_NON_SYSTEM_BZLIB_
   #define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
   #define __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
   #define __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_ // Nebula Device NPK archives
   #define __IRR_COMPILE_WITH_TAR_ARCHIVE_LOADER_
   #define __IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#endif

#define _IRR_COMPILE_WITH_LIBJPEG_           //! libjpeg.
#define _IRR_COMPILE_WITH_JPG_LOADER_        //! .jpg files
#define _IRR_COMPILE_WITH_JPG_WRITER_        //! write .jpg files

#define _IRR_COMPILE_WITH_LIBPNG_            //! libpng
#define _IRR_COMPILE_WITH_PNG_LOADER_        //! .png files
#define _IRR_COMPILE_WITH_PNG_WRITER_        //! write .png files

#define _IRR_COMPILE_WITH_BMP_LOADER_        //! .bmp files
#define _IRR_COMPILE_WITH_BMP_WRITER_        //! write .bmp files

#define _IRR_COMPILE_WITH_TGA_LOADER_        //! .tga files
#define _IRR_COMPILE_WITH_TGA_WRITER_        //! write .tga files

// #define _IRR_COMPILE_WITH_PCX_LOADER_        //! .pcx files
// #define _IRR_COMPILE_WITH_PCX_WRITER_        //! write .pcx files

// #define _IRR_COMPILE_WITH_PPM_LOADER_        //! .ppm/.pgm/.pbm files
// #define _IRR_COMPILE_WITH_PPM_WRITER_        //! write .ppm files

// #define _IRR_COMPILE_WITH_PSD_LOADER_        //! .psd files
// #define _IRR_COMPILE_WITH_PSD_WRITER_        //! write .psd files

// #define _IRR_COMPILE_WITH_WAL_LOADER_        //! load .wal files
// #define _IRR_COMPILE_WITH_LMP_LOADER_        //! load .lmp files
// #define _IRR_COMPILE_WITH_RGB_LOADER_        //! load Silicon Graphics .rgb/.rgba/.sgi/.int/.inta/.bw files
// #define _IRR_COMPILE_WITH_DDS_LOADER_      //! .dds files
// #define _IRR_COMPILE_WITH_IRR_SCENE_LOADER_     //! .irr scenes using ISceneManager::loadScene().

#define _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_  //! loads B3D, MS3D or X meshes
#ifdef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
   //#define _IRR_COMPILE_WITH_B3D_LOADER_  // Blitz3D files
   //#define _IRR_COMPILE_WITH_MS3D_LOADER_ // Milkshape files
   #define _IRR_COMPILE_WITH_X_LOADER_    // Microsoft X files
   //#define _IRR_COMPILE_WITH_OGRE_LOADER_ // Ogre 3D files
#endif

#define _IRR_COMPILE_WITH_3DS_LOADER_        //! 3D Studio Max files

#define _IRR_COMPILE_WITH_OBJ_LOADER_        //! Wavefront OBJ files
#define _IRR_COMPILE_WITH_OBJ_WRITER_        //! .obj files

#define _IRR_COMPILE_WITH_STL_LOADER_        //! stereolithography files
#define _IRR_COMPILE_WITH_STL_WRITER_        //! .stl files

#define _IRR_COMPILE_WITH_PLY_LOADER_        //! Polygon (Stanford Triangle) files
#define _IRR_COMPILE_WITH_PLY_WRITER_        //! .ply files

//#define _IRR_COMPILE_WITH_IRR_MESH_LOADER_   //! .irrmesh files
//#define _IRR_COMPILE_WITH_IRR_WRITER_        //! static .irrMesh files

//#define _IRR_COMPILE_WITH_HALFLIFE_LOADER_   //! Halflife animated files
//#define _IRR_COMPILE_WITH_MD2_LOADER_        //! Quake 2 animated files
//#define _IRR_COMPILE_WITH_MD3_LOADER_        //! Quake 3 animated files

//#define _IRR_COMPILE_WITH_COLLADA_LOADER_    //! Collada files
//#define _IRR_COMPILE_WITH_COLLADA_WRITER_    //! Collada files

//#define _IRR_COMPILE_WITH_CSM_LOADER_        //! Cartography Shop files
//#define _IRR_COMPILE_WITH_BSP_LOADER_        //! Quake 3 BSP files
//#define _IRR_COMPILE_WITH_DMF_LOADER_        //! DeleD files
//#define _IRR_COMPILE_WITH_LMTS_LOADER_       //! LMTools files
//#define _IRR_COMPILE_WITH_MY3D_LOADER_       //! MY3D files
//#define _IRR_COMPILE_WITH_OCT_LOADER_        //! FSRad OCT files
//#define _IRR_COMPILE_WITH_LWO_LOADER_        //! Lightwave3D files
//#define _IRR_COMPILE_WITH_SMF_LOADER_        //! 3D World Studio mesh files

#if defined(UNICODE) || defined(_UNICODE)
   #ifndef _IRR_WCHAR_FILESYSTEM
   #define _IRR_WCHAR_FILESYSTEM
   #endif
#endif

//! The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
   #define _IRR_WINDOWS_
   #define _IRR_WINDOWS_API_
   #define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif

//! WINCE is a very restricted environment for mobile devices
#if defined(_WIN32_WCE)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_
#define _IRR_WINDOWS_CE_PLATFORM_
#define _IRR_COMPILE_WITH_WINDOWS_CE_DEVICE_
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#  error "Only Microsoft Visual Studio 7.0 and later are supported."
#endif

//! XBox only suppots the native Window stuff
#if defined(_XBOX)
   #undef _IRR_WINDOWS_
   #define _IRR_XBOX_PLATFORM_
   #define _IRR_WINDOWS_API_
   //#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
   #undef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
   //#define _IRR_COMPILE_WITH_SDL_DEVICE_

   #include <xtl.h>
#endif

#if defined(__APPLE__) || defined(MACOSX)
#if !defined(MACOSX)
#define MACOSX // legacy support
#endif
#define _IRR_OSX_PLATFORM_
#define _IRR_COMPILE_WITH_OSX_DEVICE_
#endif

#if !defined(_IRR_WINDOWS_API_) && !defined(_IRR_OSX_PLATFORM_)
#ifndef _IRR_SOLARIS_PLATFORM_
#define _IRR_LINUX_PLATFORM_
#endif
#define _IRR_POSIX_API_
#define _IRR_COMPILE_WITH_X11_DEVICE_
#endif

//! Set FPU settings, currently X86 only
#if !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_SOLARIS_PLATFORM_)
   //#define IRRLICHT_FAST_MATH
   #ifdef NO_IRRLICHT_FAST_MATH
   #undef IRRLICHT_FAST_MATH
   #endif
#endif

// Some cleanup and standard stuff
#define IRRCALLCONV __cdecl

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifdef _IRR_WINDOWS_API_

   // To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
   // Irrlicht build, *and* in the user application, before #including <irrlicht.h>
   #ifndef _IRR_STATIC_LIB_
      #ifdef IRRLICHT_EXPORTS
      #define IRRLICHT_API __declspec(dllexport)
      #else
      #define IRRLICHT_API __declspec(dllimport)
      #endif // IRRLICHT_EXPORT
   #else
      #define IRRLICHT_API
   #endif // _IRR_STATIC_LIB_

   // Declare the calling convention.
   #if defined(_STDCALL_SUPPORTED)
      #define IRRCALLCONV __stdcall
   #else
      #define IRRCALLCONV __cdecl
   #endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_

// XBox does not have OpenGL or DirectX9
#if defined(_IRR_XBOX_PLATFORM_)
   #undef _IRR_COMPILE_WITH_OPENGL_
   #undef _IRR_COMPILE_WITH_DIRECT3D_9_
#endif

#ifndef _IRR_WINDOWS_API_
   #undef _IRR_WCHAR_FILESYSTEM
#endif

#if defined(__sparc__) || defined(__sun__)
#define __BIG_ENDIAN__
#endif

#if defined(_IRR_SOLARIS_PLATFORM_)
   #undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif

#if defined(__BORLANDC__)
   #include <tchar.h>

   // Borland 5.5.1 does not have _strcmpi defined
   #if __BORLANDC__ == 0x551
   //    #define _strcmpi strcmpi
      #undef _tfinddata_t
      #undef _tfindfirst
      #undef _tfindnext

      #define _tfinddata_t __tfinddata_t
      #define _tfindfirst  __tfindfirst
      #define _tfindnext   __tfindnext
      typedef long intptr_t;
   #endif

#endif


//! WinCE does not have OpenGL or DirectX9. use minimal loaders
#if defined(_WIN32_WCE)
   #undef _IRR_COMPILE_WITH_OPENGL_
   #undef _IRR_COMPILE_WITH_DIRECT3D_8_
   #undef _IRR_COMPILE_WITH_DIRECT3D_9_

   #undef BURNINGVIDEO_RENDERER_BEAUTIFUL
   #undef BURNINGVIDEO_RENDERER_FAST
   #undef BURNINGVIDEO_RENDERER_ULTRA_FAST
   #define BURNINGVIDEO_RENDERER_CE

   #undef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
   #define _IRR_COMPILE_WITH_WINDOWS_CE_DEVICE_
   //#define _IRR_WCHAR_FILESYSTEM

   #undef _IRR_COMPILE_WITH_IRR_MESH_LOADER_
   //#undef _IRR_COMPILE_WITH_MD2_LOADER_
   #undef _IRR_COMPILE_WITH_MD3_LOADER_
   #undef _IRR_COMPILE_WITH_3DS_LOADER_
   #undef _IRR_COMPILE_WITH_COLLADA_LOADER_
   #undef _IRR_COMPILE_WITH_CSM_LOADER_
   #undef _IRR_COMPILE_WITH_BSP_LOADER_
   #undef _IRR_COMPILE_WITH_DMF_LOADER_
   #undef _IRR_COMPILE_WITH_LMTS_LOADER_
   #undef _IRR_COMPILE_WITH_MY3D_LOADER_
   #undef _IRR_COMPILE_WITH_OBJ_LOADER_
   #undef _IRR_COMPILE_WITH_OCT_LOADER_
   #undef _IRR_COMPILE_WITH_OGRE_LOADER_
   #undef _IRR_COMPILE_WITH_LWO_LOADER_
   #undef _IRR_COMPILE_WITH_STL_LOADER_
   #undef _IRR_COMPILE_WITH_IRR_WRITER_
   #undef _IRR_COMPILE_WITH_COLLADA_WRITER_
   #undef _IRR_COMPILE_WITH_STL_WRITER_
   #undef _IRR_COMPILE_WITH_OBJ_WRITER_
   //#undef _IRR_COMPILE_WITH_BMP_LOADER_
   //#undef _IRR_COMPILE_WITH_JPG_LOADER_
   #undef _IRR_COMPILE_WITH_PCX_LOADER_
   //#undef _IRR_COMPILE_WITH_PNG_LOADER_
   #undef _IRR_COMPILE_WITH_PPM_LOADER_
   #undef _IRR_COMPILE_WITH_PSD_LOADER_
   //#undef _IRR_COMPILE_WITH_TGA_LOADER_
   #undef _IRR_COMPILE_WITH_WAL_LOADER_
   #undef _IRR_COMPILE_WITH_BMP_WRITER_
   #undef _IRR_COMPILE_WITH_JPG_WRITER_
   #undef _IRR_COMPILE_WITH_PCX_WRITER_
   #undef _IRR_COMPILE_WITH_PNG_WRITER_
   #undef _IRR_COMPILE_WITH_PPM_WRITER_
   #undef _IRR_COMPILE_WITH_PSD_WRITER_
   #undef _IRR_COMPILE_WITH_TGA_WRITER_

#endif
