#pragma once

/* Platform-specific settings */
#define WIN32 1
#define _WIN32 1

/* Compiler features */
#define inline __inline
#define restrict __restrict

/* Header availability */
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MALLOC_H 1
#define HAVE_MEMORY_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1

/* Type definitions */
#define HAVE_INT32_T 1
#define HAVE_UINT32_T 1
#define HAVE_INT16_T 1
#define HAVE_UINT16_T 1
#define HAVE_INT8_T 1
#define HAVE_UINT8_T 1

/* Endianness */
#define WORDS_BIGENDIAN 0

/* Floating point support */
#define FLOATING_POINT 1

/* Enable fixed-point math if needed (comment out if using floating point) */
// #define FIXED_POINT 1

/* Enable debugging (optional) */
// #define ENABLE_ASSERTIONS 1

/* Disable visibility attributes (not supported on Windows) */
#define EXPORT
