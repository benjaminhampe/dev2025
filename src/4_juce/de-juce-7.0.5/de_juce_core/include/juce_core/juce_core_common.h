/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_SAFT_MODULE_DECLARATION

  ID:                 juce_core
  vendor:             juce
  version:            7.0.5
  name:               JUCE core classes
  description:        The essential set of basic JUCE classes, as required by all the other JUCE modules. Includes text, container, memory, threading and i/o functionality.
  website:            http://www.juce.com/juce
  license:            ISC
  minimumCppStandard: 17

  dependencies:
  OSXFrameworks:      Cocoa Foundation IOKit
  iOSFrameworks:      Foundation
  linuxLibs:          rt dl pthread
  mingwLibs:          uuid wsock32 wininet version ole32 ws2_32 oleaut32 imm32 comdlg32 shlwapi rpcrt4 winmm

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_CORE_H_INCLUDED

//==============================================================================
#ifdef _MSC_VER
 #pragma warning (push)
 // Disable warnings for long class names, padding, and undefined preprocessor definitions.
 #pragma warning (disable: 4251 4786 4668 4820)
 #ifdef __INTEL_COMPILER
  #pragma warning (disable: 1125)
 #endif
#endif

#include "system/juce_TargetPlatform.h"

//==============================================================================
/** Config: JUCE_FORCE_DEBUG

    Normally, JUCE_DEBUG is set to 1 or 0 based on compiler and project settings,
    but if you define this value, you can override this to force it to be true or false.
*/
#ifndef JUCE_FORCE_DEBUG
//#define JUCE_FORCE_DEBUG 0
#endif

//==============================================================================
/** Config: JUCE_LOG_ASSERTIONS

    If this flag is enabled, the jassert and jassertfalse macros will always use Logger::writeToLog()
    to write a message when an assertion happens.

    Enabling it will also leave this turned on in release builds. When it's disabled,
    however, the jassert and jassertfalse macros will not be compiled in a
    release build.

    @see jassert, jassertfalse, Logger
*/
#ifndef JUCE_LOG_ASSERTIONS
 #if JUCE_ANDROID
  #define JUCE_LOG_ASSERTIONS 1
 #else
   #if JUCE_DEBUG
      #define JUCE_LOG_ASSERTIONS 0
   #else
      #define JUCE_LOG_ASSERTIONS 0
   #endif
 #endif
#endif

//==============================================================================
/** Config: JUCE_CHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the LeakedObjectDetector
    class and the JUCE_LEAK_DETECTOR macro for more details about enabling leak checking for specific classes.
*/
#if JUCE_DEBUG && ! defined (JUCE_CHECK_MEMORY_LEAKS)
 #define JUCE_CHECK_MEMORY_LEAKS 1
#endif

//==============================================================================
/** Config: JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES

    In a Windows build, this can be used to stop the required system libs being
    automatically added to the link stage.
*/
#ifndef JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
#define JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES 0
#endif

/** Config: JUCE_INCLUDE_ZLIB_CODE
    This can be used to disable Juce's embedded 3rd-party zlib code.
    You might need to tweak this if you're linking to an external zlib library in your app,
    but for normal apps, this option should be left alone.

    If you disable this, you might also want to set a value for JUCE_ZLIB_INCLUDE_PATH, to
    specify the path where your zlib headers live.
*/
#ifndef JUCE_INCLUDE_ZLIB_CODE
#define JUCE_INCLUDE_ZLIB_CODE 0
#endif

#ifndef JUCE_ZLIB_INCLUDE_PATH
#define JUCE_ZLIB_INCLUDE_PATH <zlib/zlib.h>
#endif

/** Config: JUCE_USE_CURL
    Enables http/https support via libcurl (Linux only). Enabling this will add an additional
    run-time dynamic dependency to libcurl.

    If you disable this then https/ssl support will not be available on Linux.
*/
#ifndef JUCE_USE_CURL
#define JUCE_USE_CURL 0
#endif

/** Config: JUCE_LOAD_CURL_SYMBOLS_LAZILY
    If enabled, JUCE will load libcurl lazily when required (for example, when WebInputStream
    is used). Enabling this flag may also help with library dependency errors as linking
    libcurl at compile-time may instruct the linker to hard depend on a specific version
    of libcurl. It's also useful if you want to limit the amount of JUCE dependencies and
    you are not using WebInputStream or the URL classes.
*/
#ifndef JUCE_LOAD_CURL_SYMBOLS_LAZILY
#define JUCE_LOAD_CURL_SYMBOLS_LAZILY 1
#endif

/** Config: JUCE_CATCH_UNHANDLED_EXCEPTIONS
    If enabled, this will add some exception-catching code to forward unhandled exceptions
    to your JUCEApplicationBase::unhandledException() callback.
*/
#ifndef JUCE_CATCH_UNHANDLED_EXCEPTIONS
#define JUCE_CATCH_UNHANDLED_EXCEPTIONS 0
#endif

/** Config: JUCE_ALLOW_STATIC_NULL_VARIABLES
    If disabled, this will turn off dangerous static globals like String::empty, var::null, etc
    which can cause nasty order-of-initialisation problems if they are referenced during static
    constructor code.
*/
#ifndef JUCE_ALLOW_STATIC_NULL_VARIABLES
#define JUCE_ALLOW_STATIC_NULL_VARIABLES 0
#endif

/** Config: JUCE_STRICT_REFCOUNTEDPOINTER
    If enabled, this will make the ReferenceCountedObjectPtr class stricter about allowing
    itself to be cast directly to a raw pointer. By default this is disabled, for compatibility
    with old code, but if possible, you should always enable it to improve code safety!
*/
#ifndef JUCE_STRICT_REFCOUNTEDPOINTER
#define JUCE_STRICT_REFCOUNTEDPOINTER 0
#endif

/** Config: JUCE_ENABLE_ALLOCATION_HOOKS
    If enabled, this will add global allocation functions with built-in assertions, which may
    help when debugging allocations in unit tests.
*/
#ifndef JUCE_ENABLE_ALLOCATION_HOOKS
#define JUCE_ENABLE_ALLOCATION_HOOKS 0
#endif

#if JUCE_WINDOWS
#undef small
#endif

//==============================================================================
/** Current JUCE version number.

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_MAJOR_VERSION      7
#define JUCE_MINOR_VERSION      0
#define JUCE_BUILDNUMBER        5

/** Current JUCE version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_VERSION   ((JUCE_MAJOR_VERSION << 16) + (JUCE_MINOR_VERSION << 8) + JUCE_BUILDNUMBER)

// #include <juce_core/system/juce_TargetPlatform.h>


//==============================================================================
/*  This file figures out which platform is being built, and defines some macros
    that the rest of the code can use for OS-specific compilation.

    Macros that will be set here are:

    - One of JUCE_WINDOWS, JUCE_MAC JUCE_LINUX, JUCE_IOS, JUCE_ANDROID, etc.
    - Either JUCE_32BIT or JUCE_64BIT, depending on the architecture.
    - Either JUCE_LITTLE_ENDIAN or JUCE_BIG_ENDIAN.
    - Either JUCE_INTEL or JUCE_ARM
    - Either JUCE_GCC or JUCE_CLANG or JUCE_MSVC
*/

//==============================================================================
#ifdef JUCE_APP_CONFIG_HEADER
 #include JUCE_APP_CONFIG_HEADER
#elif ! defined (JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED)
 /*
    Most projects will contain a global header file containing various settings that
    should be applied to all the code in your project. If you use the projucer, it'll
    set up a global header file for you automatically, but if you're doing things manually,
    you may want to set the JUCE_APP_CONFIG_HEADER macro with the name of a file to include,
    or just include one before all the module cpp files, in which you set
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 to silence this error.
    (Or if you don't need a global header, then you can just define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED
    globally to avoid this error).

    Note for people who hit this error when trying to compile a JUCE project created by
    a pre-v4.2 version of the Introjucer/Projucer, it's very easy to fix: just re-save
    your project with the latest version of the Projucer, and it'll magically fix this!
 */
 #error "No global header file was included!"
#endif

//==============================================================================
#if defined (_WIN32) || defined (_WIN64)
  #define       JUCE_WINDOWS 1
#elif defined (JUCE_ANDROID)
  #undef        JUCE_ANDROID
  #define       JUCE_ANDROID 1
#elif defined (__FreeBSD__) || (__OpenBSD__)
  #define       JUCE_BSD 1
#elif defined (LINUX) || defined (__linux__)
  #define       JUCE_LINUX 1
#elif defined (__APPLE_CPP__) || defined (__APPLE_CC__)
  #define CF_EXCLUDE_CSTD_HEADERS 1
  #include <TargetConditionals.h> // (needed to find out what platform we're using)
  #include <AvailabilityMacros.h>

  #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #define     JUCE_IPHONE 1
    #define     JUCE_IOS 1
  #else
    #define     JUCE_MAC 1
  #endif
#elif defined (__wasm__)
  #define       JUCE_WASM 1
#else
  #error "Unknown platform!"
#endif

//==============================================================================
#if JUCE_WINDOWS
  #ifdef _MSC_VER
    #ifdef _WIN64
      #define JUCE_64BIT 1
    #else
      #define JUCE_32BIT 1
    #endif
  #endif

  #ifdef _DEBUG
    #define JUCE_DEBUG 1
  #endif

  #ifdef __MINGW32__
    #define JUCE_MINGW 1
    #ifdef __MINGW64__
      #define JUCE_64BIT 1
    #else
      #define JUCE_32BIT 1
    #endif
  #endif

  /** If defined, this indicates that the processor is little-endian. */
  #define JUCE_LITTLE_ENDIAN 1

  #if defined (_M_ARM) || defined (_M_ARM64) || defined (__arm__) || defined (__aarch64__)
    #define JUCE_ARM 1
  #else
    #define JUCE_INTEL 1
  #endif
#endif

//==============================================================================
#if JUCE_MAC || JUCE_IOS

  #if defined (DEBUG) || defined (_DEBUG) || ! (defined (NDEBUG) || defined (_NDEBUG))
    #define JUCE_DEBUG 1
  #endif

  #if ! (defined (DEBUG) || defined (_DEBUG) || defined (NDEBUG) || defined (_NDEBUG))
    #warning "Neither NDEBUG or DEBUG has been defined - you should set one of these to make it clear whether this is a release build,"
  #endif

  #ifdef __LITTLE_ENDIAN__
    #define JUCE_LITTLE_ENDIAN 1
  #else
    #define JUCE_BIG_ENDIAN 1
  #endif

  #ifdef __LP64__
    #define JUCE_64BIT 1
  #else
    #define JUCE_32BIT 1
  #endif

  #if defined (__ppc__) || defined (__ppc64__)
    #error "PowerPC is no longer supported by JUCE!"
  #elif defined (__arm__) || defined (__arm64__)
    #define JUCE_ARM 1
  #else
    #define JUCE_INTEL 1
  #endif

  #if JUCE_MAC
    #if ! defined (MAC_OS_X_VERSION_10_14)
      #error "The 10.14 SDK (Xcode 10.1+) is required to build JUCE apps. You can create apps that run on macOS 10.9+ by changing the deployment target."
    #elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_9
      #error "Building for OSX 10.8 and earlier is no longer supported!"
    #endif
  #endif
#endif

//==============================================================================
#if JUCE_LINUX || JUCE_ANDROID || JUCE_BSD

  #ifdef _DEBUG
    #define JUCE_DEBUG 1
  #endif

  // Allow override for big-endian Linux platforms
  #if defined (__LITTLE_ENDIAN__) || ! defined (JUCE_BIG_ENDIAN)
    #define JUCE_LITTLE_ENDIAN 1
    #undef JUCE_BIG_ENDIAN
  #else
    #undef JUCE_LITTLE_ENDIAN
    #define JUCE_BIG_ENDIAN 1
  #endif

  #if defined (__LP64__) || defined (_LP64) || defined (__arm64__)
    #define JUCE_64BIT 1
  #else
    #define JUCE_32BIT 1
  #endif

  #if defined (__arm__) || defined (__arm64__) || defined (__aarch64__)
    #define JUCE_ARM 1
  #elif __MMX__ || __SSE__ || __amd64__
    #define JUCE_INTEL 1
  #endif
#endif

//==============================================================================
// Compiler type macros.

#if defined (__clang__)
  #define JUCE_CLANG 1

#elif defined (__GNUC__)
  #define JUCE_GCC 1

#elif defined (_MSC_VER)
  #define JUCE_MSVC 1

#else
  #error unknown compiler
#endif


//==============================================================================
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//==============================================================================
// #include "juce_CompilerSupport.h"
/*
   This file provides flags for compiler features that aren't supported on all platforms.
*/

//==============================================================================
//#ifndef JUCE_NATIVE_WCHAR_IS_UTF8
//#define JUCE_NATIVE_WCHAR_IS_UTF8 1
//#endif

//#ifndef JUCE_NATIVE_WCHAR_IS_UTF16
//#define JUCE_NATIVE_WCHAR_IS_UTF16 0
//#endif

//#ifndef JUCE_NATIVE_WCHAR_IS_UTF32
//#define JUCE_NATIVE_WCHAR_IS_UTF32 0
//#endif

#ifndef JUCE_STRING_UTF_TYPE
#define JUCE_STRING_UTF_TYPE 8
#endif

//==============================================================================
#if JUCE_WINDOWS && ! defined (DOXYGEN)
 #define JUCE_NATIVE_WCHAR_IS_UTF8      0
 #define JUCE_NATIVE_WCHAR_IS_UTF16     1
 #define JUCE_NATIVE_WCHAR_IS_UTF32     0
#else
 /** This macro will be set to 1 if the compiler's native wchar_t is an 8-bit type. */
 #define JUCE_NATIVE_WCHAR_IS_UTF8      0
 /** This macro will be set to 1 if the compiler's native wchar_t is a 16-bit type. */
 #define JUCE_NATIVE_WCHAR_IS_UTF16     0
 /** This macro will be set to 1 if the compiler's native wchar_t is a 32-bit type. */
 #define JUCE_NATIVE_WCHAR_IS_UTF32     1
#endif

namespace juce {

#if JUCE_NATIVE_WCHAR_IS_UTF32 || DOXYGEN
 /** A platform-independent 32-bit unicode character type. */
 using juce_wchar = wchar_t;
#else
 using juce_wchar = uint32_t;
#endif

} // end namespace juce



// GCC
#if JUCE_GCC

 #if (__GNUC__ * 100 + __GNUC_MINOR__) < 700
  #error "JUCE requires GCC 7.0 or later"
 #endif

 #ifndef JUCE_EXCEPTIONS_DISABLED
  #if ! __EXCEPTIONS
   #define JUCE_EXCEPTIONS_DISABLED 1
  #endif
 #endif

 #define JUCE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
 #define JUCE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)

#endif

//==============================================================================
// Clang
#if JUCE_CLANG

#if (__clang_major__ < 6)
#error "JUCE requires Clang 6 or later"
#endif

#ifndef JUCE_COMPILER_SUPPORTS_ARC
#define JUCE_COMPILER_SUPPORTS_ARC 1
#endif

#ifndef JUCE_EXCEPTIONS_DISABLED
#if ! __has_feature (cxx_exceptions)
#define JUCE_EXCEPTIONS_DISABLED 1
#endif
#endif

#define JUCE_CXX14_IS_AVAILABLE (__cplusplus >= 201402L)
#define JUCE_CXX17_IS_AVAILABLE (__cplusplus >= 201703L)

#endif

//==============================================================================
// MSVC
#if JUCE_MSVC

 #if _MSC_FULL_VER < 191025017  // VS2017
   #error "JUCE requires Visual Studio 2017 or later"
 #endif

 #ifndef JUCE_EXCEPTIONS_DISABLED
  #if ! _CPPUNWIND
   #define JUCE_EXCEPTIONS_DISABLED 1
  #endif
 #endif

  #define JUCE_CXX14_IS_AVAILABLE (_MSVC_LANG >= 201402L)
  #define JUCE_CXX17_IS_AVAILABLE (_MSVC_LANG >= 201703L)
#endif

//==============================================================================
#if ! JUCE_CXX17_IS_AVAILABLE
 #error "JUCE requires C++17 or later"
#endif

//==============================================================================
#ifndef DOXYGEN
 // These are old flags that are now supported on all compatible build targets
 #define JUCE_COMPILER_SUPPORTS_OVERRIDE_AND_FINAL 1
 #define JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES 1
 #define JUCE_COMPILER_SUPPORTS_INITIALIZER_LISTS 1
 #define JUCE_COMPILER_SUPPORTS_NOEXCEPT 1
 #define JUCE_DELETED_FUNCTION = delete
 #define JUCE_CONSTEXPR constexpr
 #define JUCE_NODISCARD [[nodiscard]]
#endif




#if ! DOXYGEN
#define JUCE_VERSION_ID \
    [[maybe_unused]] volatile auto juceVersionId = "juce_version_" JUCE_STRINGIFY(JUCE_MAJOR_VERSION) "_" JUCE_STRINGIFY(JUCE_MINOR_VERSION) "_" JUCE_STRINGIFY(JUCE_BUILDNUMBER);
#endif


/** Return the Nth argument. By passing a variadic pack followed by N other
    parameters, we can select one of those N parameter based on the length of
    the parameter pack.
*/
#define JUCE_NTH_ARG_(_00, _01, _02, _03, _04, _05, _06, _07, _08, _09,        \
                      _10, _11, _12, _13, _14, _15, _16, _17, _18, _19,        \
                      _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,        \
                      _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, N, ...)\
    N

#define JUCE_EACH_00_(FN)
#define JUCE_EACH_01_(FN, X)      FN(X)
#define JUCE_EACH_02_(FN, X, ...) FN(X) JUCE_EACH_01_(FN, __VA_ARGS__)
#define JUCE_EACH_03_(FN, X, ...) FN(X) JUCE_EACH_02_(FN, __VA_ARGS__)
#define JUCE_EACH_04_(FN, X, ...) FN(X) JUCE_EACH_03_(FN, __VA_ARGS__)
#define JUCE_EACH_05_(FN, X, ...) FN(X) JUCE_EACH_04_(FN, __VA_ARGS__)
#define JUCE_EACH_06_(FN, X, ...) FN(X) JUCE_EACH_05_(FN, __VA_ARGS__)
#define JUCE_EACH_07_(FN, X, ...) FN(X) JUCE_EACH_06_(FN, __VA_ARGS__)
#define JUCE_EACH_08_(FN, X, ...) FN(X) JUCE_EACH_07_(FN, __VA_ARGS__)
#define JUCE_EACH_09_(FN, X, ...) FN(X) JUCE_EACH_08_(FN, __VA_ARGS__)
#define JUCE_EACH_10_(FN, X, ...) FN(X) JUCE_EACH_09_(FN, __VA_ARGS__)
#define JUCE_EACH_11_(FN, X, ...) FN(X) JUCE_EACH_10_(FN, __VA_ARGS__)
#define JUCE_EACH_12_(FN, X, ...) FN(X) JUCE_EACH_11_(FN, __VA_ARGS__)
#define JUCE_EACH_13_(FN, X, ...) FN(X) JUCE_EACH_12_(FN, __VA_ARGS__)
#define JUCE_EACH_14_(FN, X, ...) FN(X) JUCE_EACH_13_(FN, __VA_ARGS__)
#define JUCE_EACH_15_(FN, X, ...) FN(X) JUCE_EACH_14_(FN, __VA_ARGS__)
#define JUCE_EACH_16_(FN, X, ...) FN(X) JUCE_EACH_15_(FN, __VA_ARGS__)
#define JUCE_EACH_17_(FN, X, ...) FN(X) JUCE_EACH_16_(FN, __VA_ARGS__)
#define JUCE_EACH_18_(FN, X, ...) FN(X) JUCE_EACH_17_(FN, __VA_ARGS__)
#define JUCE_EACH_19_(FN, X, ...) FN(X) JUCE_EACH_18_(FN, __VA_ARGS__)
#define JUCE_EACH_20_(FN, X, ...) FN(X) JUCE_EACH_19_(FN, __VA_ARGS__)
#define JUCE_EACH_21_(FN, X, ...) FN(X) JUCE_EACH_20_(FN, __VA_ARGS__)
#define JUCE_EACH_22_(FN, X, ...) FN(X) JUCE_EACH_21_(FN, __VA_ARGS__)
#define JUCE_EACH_23_(FN, X, ...) FN(X) JUCE_EACH_22_(FN, __VA_ARGS__)
#define JUCE_EACH_24_(FN, X, ...) FN(X) JUCE_EACH_23_(FN, __VA_ARGS__)
#define JUCE_EACH_25_(FN, X, ...) FN(X) JUCE_EACH_24_(FN, __VA_ARGS__)
#define JUCE_EACH_26_(FN, X, ...) FN(X) JUCE_EACH_25_(FN, __VA_ARGS__)
#define JUCE_EACH_27_(FN, X, ...) FN(X) JUCE_EACH_26_(FN, __VA_ARGS__)
#define JUCE_EACH_28_(FN, X, ...) FN(X) JUCE_EACH_27_(FN, __VA_ARGS__)
#define JUCE_EACH_29_(FN, X, ...) FN(X) JUCE_EACH_28_(FN, __VA_ARGS__)
#define JUCE_EACH_30_(FN, X, ...) FN(X) JUCE_EACH_29_(FN, __VA_ARGS__)
#define JUCE_EACH_31_(FN, X, ...) FN(X) JUCE_EACH_30_(FN, __VA_ARGS__)
#define JUCE_EACH_32_(FN, X, ...) FN(X) JUCE_EACH_31_(FN, __VA_ARGS__)
#define JUCE_EACH_33_(FN, X, ...) FN(X) JUCE_EACH_32_(FN, __VA_ARGS__)
#define JUCE_EACH_34_(FN, X, ...) FN(X) JUCE_EACH_33_(FN, __VA_ARGS__)
#define JUCE_EACH_35_(FN, X, ...) FN(X) JUCE_EACH_34_(FN, __VA_ARGS__)
#define JUCE_EACH_36_(FN, X, ...) FN(X) JUCE_EACH_35_(FN, __VA_ARGS__)
#define JUCE_EACH_37_(FN, X, ...) FN(X) JUCE_EACH_36_(FN, __VA_ARGS__)
#define JUCE_EACH_38_(FN, X, ...) FN(X) JUCE_EACH_37_(FN, __VA_ARGS__)
#define JUCE_EACH_39_(FN, X, ...) FN(X) JUCE_EACH_38_(FN, __VA_ARGS__)

/** Apply the macro FN to each of the other arguments. */
#define JUCE_EACH(FN, ...)                                                     \
    JUCE_NTH_ARG_(, __VA_ARGS__,                                               \
                  JUCE_EACH_39_,                                               \
                  JUCE_EACH_38_,                                               \
                  JUCE_EACH_37_,                                               \
                  JUCE_EACH_36_,                                               \
                  JUCE_EACH_35_,                                               \
                  JUCE_EACH_34_,                                               \
                  JUCE_EACH_33_,                                               \
                  JUCE_EACH_32_,                                               \
                  JUCE_EACH_31_,                                               \
                  JUCE_EACH_30_,                                               \
                  JUCE_EACH_29_,                                               \
                  JUCE_EACH_28_,                                               \
                  JUCE_EACH_27_,                                               \
                  JUCE_EACH_26_,                                               \
                  JUCE_EACH_25_,                                               \
                  JUCE_EACH_24_,                                               \
                  JUCE_EACH_23_,                                               \
                  JUCE_EACH_22_,                                               \
                  JUCE_EACH_21_,                                               \
                  JUCE_EACH_20_,                                               \
                  JUCE_EACH_19_,                                               \
                  JUCE_EACH_18_,                                               \
                  JUCE_EACH_17_,                                               \
                  JUCE_EACH_16_,                                               \
                  JUCE_EACH_15_,                                               \
                  JUCE_EACH_14_,                                               \
                  JUCE_EACH_13_,                                               \
                  JUCE_EACH_12_,                                               \
                  JUCE_EACH_11_,                                               \
                  JUCE_EACH_10_,                                               \
                  JUCE_EACH_09_,                                               \
                  JUCE_EACH_08_,                                               \
                  JUCE_EACH_07_,                                               \
                  JUCE_EACH_06_,                                               \
                  JUCE_EACH_05_,                                               \
                  JUCE_EACH_04_,                                               \
                  JUCE_EACH_03_,                                               \
                  JUCE_EACH_02_,                                               \
                  JUCE_EACH_01_,                                               \
                  JUCE_EACH_00_)                                               \
    (FN, __VA_ARGS__)

/** Concatenate two tokens to form a new token. */
#define JUCE_CONCAT_(a, b) a##b
#define JUCE_CONCAT(a, b) JUCE_CONCAT_(a, b)

/** Quote the argument, turning it into a string. */
#define JUCE_TO_STRING(x) #x

#if JUCE_CLANG || JUCE_GCC || JUCE_MINGW
    #define JUCE_IGNORE_GCC_IMPL_(compiler, warning)
    #define JUCE_IGNORE_GCC_IMPL_0(compiler, warning)
    #define JUCE_IGNORE_GCC_IMPL_1(compiler, warning)                          \
        _Pragma(JUCE_TO_STRING(compiler diagnostic ignored warning))

    /** If 'warning' is recognised by this compiler, ignore it. */
    #if defined (__has_warning)
        #define JUCE_IGNORE_GCC_LIKE(compiler, warning)                        \
            JUCE_CONCAT(JUCE_IGNORE_GCC_IMPL_, __has_warning(warning))(compiler, warning)
    #else
        #define JUCE_IGNORE_GCC_LIKE(compiler, warning)                        \
            JUCE_IGNORE_GCC_IMPL_1(compiler, warning)
    #endif

    /** Ignore GCC/clang-specific warnings. */
    #define JUCE_IGNORE_GCC(warning)   JUCE_IGNORE_GCC_LIKE(GCC, warning)
    #define JUCE_IGNORE_clang(warning) JUCE_IGNORE_GCC_LIKE(clang, warning)

    #define JUCE_IGNORE_WARNINGS_GCC_LIKE(compiler, ...)                       \
        _Pragma(JUCE_TO_STRING(compiler diagnostic push))                      \
        JUCE_EACH(JUCE_CONCAT(JUCE_IGNORE_, compiler), __VA_ARGS__)

    /** Push a new warning scope, and then ignore each warning for either clang
        or gcc. If the compiler doesn't support __has_warning, we add -Wpragmas
        as the first disabled warning because otherwise we might get complaints
        about unknown warning options.
    */
    #if defined (__has_warning)
        #define JUCE_PUSH_WARNINGS_GCC_LIKE(compiler, ...)                     \
            JUCE_IGNORE_WARNINGS_GCC_LIKE(compiler, __VA_ARGS__)
    #else
        #define JUCE_PUSH_WARNINGS_GCC_LIKE(compiler, ...)                     \
            JUCE_IGNORE_WARNINGS_GCC_LIKE(compiler, "-Wpragmas", __VA_ARGS__)
    #endif

    /** Pop the current warning scope. */
    #define JUCE_POP_WARNINGS_GCC_LIKE(compiler)                               \
        _Pragma(JUCE_TO_STRING(compiler diagnostic pop))

    /** Push/pop warnings on compilers with gcc-like warning flags.
        These macros expand to nothing on other compilers (like MSVC).
    */
    #if JUCE_CLANG
        #define JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE(...) JUCE_PUSH_WARNINGS_GCC_LIKE(clang, __VA_ARGS__)
        #define JUCE_END_IGNORE_WARNINGS_GCC_LIKE JUCE_POP_WARNINGS_GCC_LIKE(clang)
    #else
        #define JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE(...) JUCE_PUSH_WARNINGS_GCC_LIKE(GCC, __VA_ARGS__)
        #define JUCE_END_IGNORE_WARNINGS_GCC_LIKE JUCE_POP_WARNINGS_GCC_LIKE(GCC)
    #endif
#else
    #define JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE(...)
    #define JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#endif

/** Push/pop warnings on MSVC. These macros expand to nothing on other
    compilers (like clang and gcc).
*/
#if JUCE_MSVC
    #define JUCE_IGNORE_MSVC(warnings) __pragma(warning(disable:warnings))
    #define JUCE_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)              \
        __pragma(warning(push, level)) JUCE_IGNORE_MSVC(warnings)
    #define JUCE_BEGIN_IGNORE_WARNINGS_MSVC(warnings)                           \
        __pragma(warning(push)) JUCE_IGNORE_MSVC(warnings)
    #define JUCE_END_IGNORE_WARNINGS_MSVC __pragma(warning(pop))
#else
    #define JUCE_IGNORE_MSVC(warnings)
    #define JUCE_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(level, warnings)
    #define JUCE_BEGIN_IGNORE_WARNINGS_MSVC(warnings)
    #define JUCE_END_IGNORE_WARNINGS_MSVC
#endif

#if JUCE_MAC || JUCE_IOS
    #define JUCE_SANITIZER_ATTRIBUTE_MINIMUM_CLANG_VERSION 11
#else
    #define JUCE_SANITIZER_ATTRIBUTE_MINIMUM_CLANG_VERSION 9
#endif

/** Disable sanitizers for a range of functions.

    This functionality doesn't seem to exist on GCC yet, so at the moment this only works for clang.
*/
#if JUCE_CLANG && __clang_major__ >= JUCE_SANITIZER_ATTRIBUTE_MINIMUM_CLANG_VERSION
    #define JUCE_BEGIN_NO_SANITIZE(warnings)                                    \
        _Pragma(JUCE_TO_STRING(clang attribute push(__attribute__((no_sanitize(warnings))), apply_to=function)))
    #define JUCE_END_NO_SANITIZE _Pragma(JUCE_TO_STRING(clang attribute pop))
#else
    #define JUCE_BEGIN_NO_SANITIZE(warnings)
    #define JUCE_END_NO_SANITIZE
#endif

#undef JUCE_SANITIZER_ATTRIBUTE_MINIMUM_CLANG_VERSION

//==============================================================================
// Now we'll include some common OS headers..
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4514 4245 4100)

#if JUCE_MSVC
 #include <intrin.h>
#endif


#if JUCE_MAC || JUCE_IOS
 #include <libkern/OSAtomic.h>
 #include <xlocale.h>
 #include <signal.h>
#endif

#if JUCE_LINUX || JUCE_BSD
 #include <cstring>
 #include <signal.h>

 #if __INTEL_COMPILER
  #if __ia64__
   #include <ia64intrin.h>
  #else
   #include <ia32intrin.h>
  #endif
 #endif
#endif

#if JUCE_MSVC && JUCE_DEBUG
 #include <crtdbg.h>
#endif

JUCE_END_IGNORE_WARNINGS_MSVC

#if JUCE_MINGW
#include <cstring>
#include <sys/types.h>
#endif

#if JUCE_ANDROID
 #include <cstring>
 #include <byteswap.h>
#endif

// undef symbols that are sometimes set by misguided 3rd-party headers..
#undef TYPE_BOOL
#undef max
#undef min
#undef major
#undef minor
#undef KeyPress

//==============================================================================
// DLL building settings on Windows
/*
#if JUCE_MSVC
 #if defined(JUCE_DLL_BUILD)
  #define JUCE_API __declspec (dllexport)
  #pragma warning (disable: 4251)
 #elif defined (JUCE_DLL)
  #define JUCE_API __declspec (dllimport)
  #pragma warning (disable: 4251)
 #endif
 #ifdef __INTEL_COMPILER
  #pragma warning (disable: 1125) // (virtual override warning)
 #endif
#elif defined (JUCE_DLL) || defined (JUCE_DLL_BUILD)
#endif
*/
#ifndef JUCE_API
#define JUCE_API
//#define JUCE_API __attribute__ ((visibility("default")))
#endif

#if JUCE_MSVC && JUCE_DLL_BUILD
 #define JUCE_PUBLIC_IN_DLL_BUILD(declaration)  public: declaration; private:
#else
 #define JUCE_PUBLIC_IN_DLL_BUILD(declaration)  declaration;
#endif

/** This macro is added to all JUCE public function declarations. */
#define JUCE_PUBLIC_FUNCTION        JUCE_API JUCE_CALLTYPE

#ifndef DOXYGEN
 #define JUCE_NAMESPACE juce  // This old macro is deprecated: you should just use the juce namespace directly.
#endif

// #ifdef JUCE_CORE_H_INCLUDED
 // /* When you add this cpp file to your project, you mustn't include it in a file where you've
    // already included any other headers - just put it inside a file on its own, possibly with your config
    // flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    // header files that the compiler may be using.
 // */
 // #error "Incorrect use of JUCE cpp file"
// #endif

#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1
#define JUCE_CORE_INCLUDE_COM_SMART_PTR 1
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1

// #include "juce_core.h"

#include <cstdint>
#include <cctype>
#include <cstdarg>
#include <locale>
#include <thread>

#if ! JUCE_ANDROID
#include <sys/timeb.h>
#include <cwctype>
#endif

#if JUCE_WINDOWS
#include <ctime>

#if JUCE_MINGW
//  #include <ws2spi.h>
#include <cstdio>
#include <locale.h>
#else
  JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4091)
  #include <Dbghelp.h>
  JUCE_END_IGNORE_WARNINGS_MSVC

  #if ! JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
   #pragma comment (lib, "DbgHelp.lib")
  #endif
#endif

#else
 #if JUCE_LINUX || JUCE_BSD || JUCE_ANDROID
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/errno.h>
  #include <unistd.h>
  #include <netinet/in.h>
 #endif

 #if JUCE_WASM
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <errno.h>
  #include <unistd.h>
  #include <netinet/in.h>
  #include <sys/stat.h>
 #endif

 #if JUCE_LINUX || JUCE_BSD
  #include <stdio.h>
  #include <langinfo.h>
  #include <ifaddrs.h>
  #include <sys/resource.h>

  #if JUCE_USE_CURL
   #include <curl/curl.h>
  #endif
 #endif

 #include <pwd.h>
 #include <fcntl.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
 #include <sys/time.h>
 #include <net/if.h>
 #include <sys/ioctl.h>

 #if ! (JUCE_ANDROID || JUCE_WASM)
  #include <execinfo.h>
 #endif
#endif

#if JUCE_MAC || JUCE_IOS
 #include <xlocale.h>
 #include <mach/mach.h>
#endif

#if JUCE_ANDROID
 #include <ifaddrs.h>
 #include <android/log.h>
#endif

#undef check


//#include "juce_CompilerWarnings.h"

// #include "juce_PlatformDefs.h"

//==============================================================================
/*  This file defines miscellaneous macros for debugging, assertions, etc.
*/

//==============================================================================
#ifdef JUCE_FORCE_DEBUG
 #undef JUCE_DEBUG

 #if JUCE_FORCE_DEBUG
  #define JUCE_DEBUG 1
 #endif
#endif

/** This macro defines the C calling convention used as the standard for JUCE calls. */
#if JUCE_WINDOWS
 #define JUCE_CALLTYPE   __stdcall
 #define JUCE_CDECL      __cdecl
#else
 #define JUCE_CALLTYPE
 #define JUCE_CDECL
#endif

//==============================================================================
// Debugging and assertion macros

#ifndef JUCE_LOG_CURRENT_ASSERTION
 #if JUCE_LOG_ASSERTIONS || JUCE_DEBUG
  #define JUCE_LOG_CURRENT_ASSERTION    juce::logAssertion (__FILE__, __LINE__);
 #else
  #define JUCE_LOG_CURRENT_ASSERTION
 #endif
#endif

//==============================================================================
#if JUCE_IOS || JUCE_LINUX || JUCE_BSD
  /** This will try to break into the debugger if the app is currently being debugged.
      If called by an app that's not being debugged, the behaviour isn't defined - it may
      crash or not, depending on the platform.
      @see jassert()
  */
  #define JUCE_BREAK_IN_DEBUGGER        { ::kill (0, SIGTRAP); }
#elif JUCE_MSVC
  #ifndef __INTEL_COMPILER
    #pragma intrinsic (__debugbreak)
  #endif
  #define JUCE_BREAK_IN_DEBUGGER        { __debugbreak(); }
#elif JUCE_INTEL && (JUCE_GCC || JUCE_CLANG || JUCE_MAC)
  #if JUCE_NO_INLINE_ASM
   #define JUCE_BREAK_IN_DEBUGGER       { }
  #else
   #define JUCE_BREAK_IN_DEBUGGER       { asm ("int $3"); }
  #endif
#elif JUCE_ARM && JUCE_MAC
  #define JUCE_BREAK_IN_DEBUGGER        { __builtin_debugtrap(); }
#elif JUCE_ANDROID
  #define JUCE_BREAK_IN_DEBUGGER        { __builtin_trap(); }
#else
  #define JUCE_BREAK_IN_DEBUGGER        { __asm int 3 }
#endif

#if JUCE_CLANG && defined (__has_feature) && ! defined (JUCE_ANALYZER_NORETURN)
 #if __has_feature (attribute_analyzer_noreturn)
  inline void __attribute__((analyzer_noreturn)) juce_assert_noreturn() {}
  #define JUCE_ANALYZER_NORETURN juce::juce_assert_noreturn();
 #endif
#endif

#ifndef JUCE_ANALYZER_NORETURN
 #define JUCE_ANALYZER_NORETURN
#endif

/** Used to silence Wimplicit-fallthrough on Clang and GCC where available
    as there are a few places in the codebase where we need to do this
    deliberately and want to ignore the warning.
*/
#if JUCE_CLANG
 #if __has_cpp_attribute(clang::fallthrough)
  #define JUCE_FALLTHROUGH [[clang::fallthrough]];
 #else
  #define JUCE_FALLTHROUGH
 #endif
#elif JUCE_GCC
 #if __GNUC__ >= 7
  #define JUCE_FALLTHROUGH [[gnu::fallthrough]];
 #else
  #define JUCE_FALLTHROUGH
 #endif
#else
 #define JUCE_FALLTHROUGH
#endif

//==============================================================================
#if JUCE_MSVC && ! defined (DOXYGEN)
 #define JUCE_BLOCK_WITH_FORCED_SEMICOLON(x) \
   __pragma(warning(push)) \
   __pragma(warning(disable:4127)) \
   do { x } while (false) \
   __pragma(warning(pop))
#else
 /** This is the good old C++ trick for creating a macro that forces the user to put
    a semicolon after it when they use it.
 */
 #define JUCE_BLOCK_WITH_FORCED_SEMICOLON(x) do { x } while (false)
#endif

//==============================================================================
#if (JUCE_DEBUG && ! JUCE_DISABLE_ASSERTIONS) || DOXYGEN

  //==============================================================================
  /** This will always cause an assertion failure.
      It is only compiled in a debug build, (unless JUCE_LOG_ASSERTIONS is enabled for your build).
      @see jassert
  */
  #define jassertfalse
//JUCE_BLOCK_WITH_FORCED_SEMICOLON (JUCE_LOG_CURRENT_ASSERTION; if (juce::juce_isRunningUnderDebugger()) JUCE_BREAK_IN_DEBUGGER; JUCE_ANALYZER_NORETURN)

  //==============================================================================
  /** Platform-independent assertion macro.

      This macro gets turned into a no-op when you're building with debugging turned off, so be
      careful that the expression you pass to it doesn't perform any actions that are vital for the
      correct behaviour of your program!
      @see jassertfalse
  */
  #define jassert(expression)
//JUCE_BLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) jassertfalse;)

  /** Platform-independent assertion macro which suppresses ignored-variable
      warnings in all build modes. You should probably use a plain jassert()
      by default, and only replace it with jassertquiet() once you've
      convinced yourself that any unused-variable warnings emitted by the
      compiler are harmless.
  */
  #define jassertquiet(expression)
//JUCE_BLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) jassertfalse;)

#else
  //==============================================================================
  // If debugging is disabled, these dummy debug and assertion macros are used..

  #define DBG(textToWrite)
  #define jassertfalse                  JUCE_BLOCK_WITH_FORCED_SEMICOLON (JUCE_LOG_CURRENT_ASSERTION)

  #if JUCE_LOG_ASSERTIONS
   #define jassert(expression)          JUCE_BLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) jassertfalse;)
   #define jassertquiet(expression)     JUCE_BLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) jassertfalse;)
  #else
   #define jassert(expression)          JUCE_BLOCK_WITH_FORCED_SEMICOLON ( ; )
   #define jassertquiet(expression)     JUCE_BLOCK_WITH_FORCED_SEMICOLON (if (false) (void) (expression);)
  #endif

#endif

//==============================================================================
#ifndef DOXYGEN
 #define JUCE_JOIN_MACRO_HELPER(a, b) a ## b
 #define JUCE_STRINGIFY_MACRO_HELPER(a) #a
#endif

/** A good old-fashioned C macro concatenation helper.
    This combines two items (which may themselves be macros) into a single string,
    avoiding the pitfalls of the ## macro operator.
*/
#define JUCE_JOIN_MACRO(item1, item2)  JUCE_JOIN_MACRO_HELPER (item1, item2)

/** A handy C macro for stringifying any symbol, rather than just a macro parameter. */
#define JUCE_STRINGIFY(item)  JUCE_STRINGIFY_MACRO_HELPER (item)

//==============================================================================
/** This is a shorthand macro for deleting a class's copy constructor and
    copy assignment operator.

    For example, instead of
    @code
    class MyClass
    {
        etc..

    private:
        MyClass (const MyClass&);
        MyClass& operator= (const MyClass&);
    };@endcode

    ..you can just write:

    @code
    class MyClass
    {
        etc..

    private:
        JUCE_DECLARE_NON_COPYABLE (MyClass)
    };@endcode
*/
#define JUCE_DECLARE_NON_COPYABLE(className) \
    className (const className&) = delete;\
    className& operator= (const className&) = delete;

/** This is a shorthand macro for deleting a class's move constructor and
    move assignment operator.
*/
#define JUCE_DECLARE_NON_MOVEABLE(className) \
    className (className&&) = delete;\
    className& operator= (className&&) = delete;

/** This macro can be added to class definitions to disable the use of new/delete to
    allocate the object on the heap, forcing it to only be used as a stack or member variable.
*/
#define JUCE_PREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t) = delete; \
    static void operator delete (void*) = delete;

//==============================================================================
#if JUCE_MSVC && ! defined (DOXYGEN)
 #define JUCE_WARNING_HELPER(file, line, mess) message(file "(" JUCE_STRINGIFY (line) ") : Warning: " #mess)
 #define JUCE_COMPILER_WARNING(message)  __pragma(JUCE_WARNING_HELPER (__FILE__, __LINE__, message))
#else
 #ifndef DOXYGEN
  #define JUCE_WARNING_HELPER(mess) message(#mess)
 #endif

 /** This macro allows you to emit a custom compiler warning message.
     Very handy for marking bits of code as "to-do" items, or for shaming
     code written by your co-workers in a way that's hard to ignore.

     GCC and Clang provide the \#warning directive, but MSVC doesn't, so this macro
     is a cross-compiler way to get the same functionality as \#warning.
 */
 #define JUCE_COMPILER_WARNING(message)  _Pragma(JUCE_STRINGIFY (JUCE_WARNING_HELPER (message)))
#endif


//==============================================================================
#if JUCE_DEBUG || DOXYGEN
  /** A platform-independent way of forcing an inline function.
      Use the syntax: @code
      forcedinline void myfunction (int x)
      @endcode
  */
  #define forcedinline  inline
#else
  #if JUCE_MSVC
   #define forcedinline       __forceinline
  #else
   #define forcedinline       inline __attribute__((always_inline))
  #endif
#endif

#if JUCE_MSVC || DOXYGEN
  /** This can be placed before a stack or member variable declaration to tell the compiler
      to align it to the specified number of bytes. */
  #define JUCE_ALIGN(bytes)   __declspec (align (bytes))
#else
  #define JUCE_ALIGN(bytes)   __attribute__ ((aligned (bytes)))
#endif

/*
//==============================================================================
#if JUCE_ANDROID && ! defined (DOXYGEN)
#define JUCE_MODAL_LOOPS_PERMITTED 0
#elif ! defined (JUCE_MODAL_LOOPS_PERMITTED)
// Some operating environments don't provide a modal loop mechanism, so this flag can be
// used to disable any functions that try to run a modal loop.
#define JUCE_MODAL_LOOPS_PERMITTED 0
#endif
*/
#define JUCE_MODAL_LOOPS_PERMITTED 1

//==============================================================================
#if JUCE_GCC || JUCE_CLANG
 #define JUCE_PACKED __attribute__((packed))
#elif ! defined (DOXYGEN)
 #define JUCE_PACKED
#endif

//==============================================================================
#if JUCE_GCC || DOXYGEN
 /** This can be appended to a function declaration to tell gcc to disable associative
     math optimisations which break some floating point algorithms. */
 #define JUCE_NO_ASSOCIATIVE_MATH_OPTIMISATIONS   __attribute__((__optimize__("no-associative-math")))
#else
 #define JUCE_NO_ASSOCIATIVE_MATH_OPTIMISATIONS
#endif
