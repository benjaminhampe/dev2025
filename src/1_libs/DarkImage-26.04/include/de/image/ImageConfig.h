#pragma once
#include <de/image/Image.h>

// *.ppm
#ifndef DE_IMAGE_READER_PPM_ENABLED
#define DE_IMAGE_READER_PPM_ENABLED
#endif

#ifndef DE_IMAGE_WRITER_PPM_ENABLED
#define DE_IMAGE_WRITER_PPM_ENABLED
#endif

// *.webp. Needs lib de_webp
//#ifdef HAVE_LIBWEBP
    #ifndef DE_IMAGE_READER_WEBP_ENABLED
    #define DE_IMAGE_READER_WEBP_ENABLED
    #endif
    #ifndef DE_IMAGE_WRITER_WEBP_ENABLED
    #define DE_IMAGE_WRITER_WEBP_ENABLED
    #endif
//#endif

// *.xpm
#ifndef DE_IMAGE_READER_XPM_ENABLED
#define DE_IMAGE_READER_XPM_ENABLED
#endif
// *.xpm
#ifndef DE_IMAGE_WRITER_XPM_ENABLED
#define DE_IMAGE_WRITER_XPM_ENABLED
#endif

// *.jpg. Needs de_jpeg9f
//#ifdef HAVE_LIBJPEG
    #ifndef DE_IMAGE_READER_JPG_ENABLED
    #define DE_IMAGE_READER_JPG_ENABLED
    #endif
    #ifndef DE_IMAGE_WRITER_JPG_ENABLED
    #define DE_IMAGE_WRITER_JPG_ENABLED
    #endif
//#endif

// *.png. Needs lib de_png
//#ifdef HAVE_LIBPNG
    #ifndef DE_IMAGE_READER_PNG_ENABLED
    #define DE_IMAGE_READER_PNG_ENABLED
    #endif
    #ifndef DE_IMAGE_WRITER_PNG_ENABLED
    #define DE_IMAGE_WRITER_PNG_ENABLED
    #endif
//#endif

// *.exr. Needs lib de_openexr
//#ifdef HAVE_OPENEXR
    // #ifndef DE_IMAGE_READER_EXR_ENABLED
    // #define DE_IMAGE_READER_EXR_ENABLED
    // #endif
    // #ifndef DE_IMAGE_WRITER_EXR_ENABLED
    // #define DE_IMAGE_WRITER_EXR_ENABLED
    // #endif
//#endif

// NEW:

// *.bmp
#ifndef DE_IMAGE_READER_BMP_ENABLED
#define DE_IMAGE_READER_BMP_ENABLED
#endif
#ifndef DE_IMAGE_WRITER_BMP_ENABLED
#define DE_IMAGE_WRITER_BMP_ENABLED
#endif

// *.gif
//#ifdef HAVE_LIBGIF
    #ifndef DE_IMAGE_READER_GIF_ENABLED
    #define DE_IMAGE_READER_GIF_ENABLED
    #endif
    #ifndef DE_IMAGE_WRITER_GIF_ENABLED
    #define DE_IMAGE_WRITER_GIF_ENABLED
    #endif
//#endif

// *.tga
#ifndef DE_IMAGE_READER_TGA_ENABLED
#define DE_IMAGE_READER_TGA_ENABLED
#endif
// *.tga
#ifndef DE_IMAGE_WRITER_TGA_ENABLED
#define DE_IMAGE_WRITER_TGA_ENABLED
#endif

// *.tif, *.tiff
//#ifdef HAVE_LIBTIFF
    #ifndef DE_IMAGE_READER_TIF_ENABLED
    #define DE_IMAGE_READER_TIF_ENABLED
    #endif
    #ifndef DE_IMAGE_WRITER_TIF_ENABLED
    #define DE_IMAGE_WRITER_TIF_ENABLED
    #endif
//#endif

// *.dds
#ifndef DE_IMAGE_READER_DDS_ENABLED
#define DE_IMAGE_READER_DDS_ENABLED
#endif
// *.dds
#ifndef DE_IMAGE_WRITER_DDS_ENABLED
#define DE_IMAGE_WRITER_DDS_ENABLED
#endif

// *.ico -> AnimationReader
#ifndef DE_IMAGE_READER_ICO_ENABLED
#define DE_IMAGE_READER_ICO_ENABLED
#endif
// *.ico
#ifndef DE_IMAGE_WRITER_ICO_ENABLED
#define DE_IMAGE_WRITER_ICO_ENABLED
#endif

// *.raw
#ifndef DE_IMAGE_READER_RAW_ENABLED
#define DE_IMAGE_READER_RAW_ENABLED
#endif
// *.raw + (Also creates .raw.meta)
#ifndef DE_IMAGE_WRITER_RAW_ENABLED
#define DE_IMAGE_WRITER_RAW_ENABLED
#endif

// *.rgb/*.rgba/*.sgi Silicon Graphics Irix
#ifndef DE_IMAGE_READER_RGB_ENABLED
#define DE_IMAGE_READER_RGB_ENABLED
#endif
// *.rgb/*.rgba/*.sgi
#ifndef DE_IMAGE_WRITER_RGB_ENABLED
#define DE_IMAGE_WRITER_RGB_ENABLED
#endif

// *.wal
#ifndef DE_IMAGE_READER_WAL_ENABLED
#define DE_IMAGE_READER_WAL_ENABLED
#endif
// *.wal
#ifndef DE_IMAGE_WRITER_WAL_ENABLED
#define DE_IMAGE_WRITER_WAL_ENABLED
#endif

// ===============================================

// ===============================================
/*
// *.html
#ifndef DE_IMAGE_WRITER_HTML_ENABLED
#define DE_IMAGE_WRITER_HTML_ENABLED
#endif

// *.pcx
#ifndef DE_IMAGE_READER_PCX_ENABLED
#define DE_IMAGE_READER_PCX_ENABLED
#endif
// *.pcx
#ifndef DE_IMAGE_WRITER_PCX_ENABLED
#define DE_IMAGE_WRITER_PCX_ENABLED
#endif

// *.ppm
#ifndef DE_IMAGE_READER_PPM_ENABLED
#define DE_IMAGE_READER_PPM_ENABLED
#endif
// *.ppm
#ifndef DE_IMAGE_WRITER_PPM_ENABLED
#define DE_IMAGE_WRITER_PPM_ENABLED
#endif

*/
