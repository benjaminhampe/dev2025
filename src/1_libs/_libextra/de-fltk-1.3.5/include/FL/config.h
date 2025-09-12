/* config.h.  Generated from configh.in by configure.  */
/*
 * "$Id$"
 *
 * Configuration file for the Fast Light Tool Kit (FLTK).
 *
 * Copyright 1998-2016 by Bill Spitzak and others.
 *
 * This library is free software. Distribution and use rights are outlined in
 * the file "COPYING" which should have been included with this file.  If this
 * file is missing or damaged, see the license at:
 *
 *     http://www.fltk.org/COPYING.php
 *
 * Please report all bugs and problems on the following page:
 *
 *     http://www.fltk.org/str.php
 */

/*
 * Where to find files...
 */

#define FLTK_DATADIR "/usr/local/share/fltk"
#define FLTK_DOCDIR "/usr/local/share/doc/fltk"

/*
 * BORDER_WIDTH:
 *
 * Thickness of FL_UP_BOX and FL_DOWN_BOX.  Current 1,2, and 3 are
 * supported.
 *
 * 3 is the historic FLTK look.
 * 2 is the default and looks like Microsoft Windows, KDE, and Qt.
 * 1 is a plausible future evolution...
 *
 * Note that this may be simulated at runtime by redefining the boxtypes
 * using Fl::set_boxtype().
 */

#define BORDER_WIDTH 2

/*
 * HAVE_GL:
 *
 * Do you have OpenGL? Set this to 0 if you don't have or plan to use
 * OpenGL, and FLTK will be smaller.
 */

#define HAVE_GL 1

/*
 * HAVE_GL_GLU_H:
 *
 * Do you have the OpenGL Utility Library header file?
 * (many broken Mesa RPMs do not...)
 */

#define HAVE_GL_GLU_H 1

/*
 * USE_COLORMAP:
 *
 * Setting this to zero will save a good deal of code (especially for
 * fl_draw_image), but FLTK will only work on TrueColor visuals.
 */

#define USE_COLORMAP 1

// defined by cmake recipe:
//#define HAVE_GLXGETPROCADDRESSARB 1
//#define HAVE_XINERAMA 1
//#define USE_XFT 1
//#define HAVE_XDBE 1
//#define USE_XDBE HAVE_XDBE
//#define HAVE_XFIXES 1
//#define HAVE_XCURSOR 1
//#define HAVE_XRENDER 1
//#define HAVE_X11_XREGION_H 1
//#define USE_X11 1
//#define HAVE_DLFCN_H 1
//#define HAVE_DLSYM 1
//#define HAVE_ALSA_ASOUNDLIB_H 1
/*
 * HAVE_DIRENT_H, HAVE_SYS_NDIR_H, HAVE_SYS_DIR_H, HAVE_NDIR_H,
 * HAVE_SCANDIR, HAVE_SCANDIR_POSIX:
 *
 * Where is <dirent.h> (used only by fl_file_chooser and scandir).
 */

// #define HAVE_DIRENT_H 1
/* #undef HAVE_SYS_NDIR_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_NDIR_H */
// #define HAVE_SCANDIR 1
// #define HAVE_SCANDIR_POSIX 1

#define HAVE_OVERLAY 0
#define HAVE_GL_OVERLAY 1

/*
 * WORDS_BIGENDIAN:
 *
 * Byte order of your machine: 1 = big-endian, 0 = little-endian.
 */

#ifdef __APPLE__
#include <mac_endianness.h>
#else
#define WORDS_BIGENDIAN 0
#endif

/*
 * U16, U32, U64:
 *
 * Types used by fl_draw_image.  One of U32 or U64 must be defined.
 * U16 is optional but FLTK will work better with it!
 */

#define U16 unsigned short
#define U32 unsigned
#define U64 unsigned long

/*
 * Possibly missing sprintf-style functions:
 */

#define HAVE_VSNPRINTF 1
#define HAVE_SNPRINTF 1

/*
 * String functions and headers...
 */

#define HAVE_STRINGS_H 1
#define HAVE_STRCASECMP 1
/* #undef HAVE_STRLCAT */
/* #undef HAVE_STRLCPY */

/*
 * Do we have POSIX locale support?
 */

#define HAVE_LOCALE_H 1
#define HAVE_LOCALECONV 1

/*
 * HAVE_SYS_SELECT_H:
 *
 * Whether or not select() call has its own header file.
 */

#define HAVE_SYS_SELECT_H 1

/*
 * HAVE_SYS_STDTYPES_H:
 *
 * Whether or not we have the <sys/stdtypes.h> header file.
 */

/* #undef HAVE_SYS_STDTYPES_H */

/*
 * USE_POLL:
 *
 * Use the poll() call provided on Linux and Irix instead of select()
 */

#define USE_POLL 0

/*
 * Do we have various image libraries?
 */

#define HAVE_LIBPNG 1
#define HAVE_LIBZ 1
#define HAVE_LIBJPEG 1

/* #undef FLTK_USE_CAIRO */
/* #undef FLTK_HAVE_CAIRO */

#define HAVE_PNG_H 1
#define HAVE_LIBPNG_PNG_H 1
#define HAVE_PNG_GET_VALID 1
#define HAVE_PNG_SET_TRNS_TO_ALPHA 1

#define HAVE_PTHREAD 1
#define HAVE_PTHREAD_H 1

/*
 * Do we have the long long type?
 */

#define HAVE_LONG_LONG 1

#ifdef HAVE_LONG_LONG
#  define FLTK_LLFMT	"%lld"
#  define FLTK_LLCAST	(long long)
#else
#  define FLTK_LLFMT	"%ld"
#  define FLTK_LLCAST	(long)
#endif /* HAVE_LONG_LONG */

/*
 * End of "$Id$".
 */
