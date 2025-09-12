# @title Benni's C/C++ toolchain ( GCC on Windows64 using QtSDK+MinGW7.3.1+cmake-benni
# BUILD=x86_64-pc-mingw64 x86_64-w64-mingw32
# HOST=x86_64-pc-mingw64 x86_64-w64-mingw32
# @author Benjamin Hampe <benjaminhampe@gmx.de>
# @platforms Windows64, Linux64
#define CASE_RETURN(x) case x: return L ## #x

# toolchain.cmake
# Can be with 'ninja' instead of 'mingw32-make' or not.
# Always uses parallel linking since big apps really need too much time on a single core.
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_LINKER lld)
set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_RC_COMPILER windres) # C:\sdk\Qt\Tools\llvm-mingw1706_64\bin\windres.exe ||| llvm-rc sucks!
set(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target x86_64-w64-windows-gnu")
# set(CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -target x86_64-w64-windows-gnu")
# set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")	# OR gold OR ld
# set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld")	# OR gold OR ld
# set(CMAKE_MODULE_LINKER_FLAGS "-fuse-ld=lld")	# OR gold OR ld
# CALL: cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -G Ninja ..

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(BENNI_USE_QT 1)     # <optional> There are various other libs to build UI
set(BENNI_USE_NASM 0)   # <broken-now> Very common, for compress, network and audio libs.
set(BENNI_USE_ASAN 0)   # <broken> MinGW is not ready yet.
set(BENNI_USE_AVX512 0)
set(BENNI_USE_AVX2 1)
set(BENNI_USE_FMA3 1)
set(BENNI_USE_SSE42 0)
set(BENNI_USE_SSE41 0)
set(BENNI_USE_SSSE3 0)
set(BENNI_USE_SSE3 0)
set(BENNI_USE_SSE2 0)
set(BENNI_USE_SSE 0)
set(BENNI_USE_MMX 0)
set(BENNI_USE_LTO 0)
set(BENNI_USE_LLD 0) # Parallel Linker

set(BENNI_USE_OMIT_FRAMEPOINTER 0)
set(BENNI_MINGW_DIR C:/sdk/mingw64_1706_llvm)
set(BENNI_QT6_DIR C:/sdk/Qt/6.9.1/llvm-mingw_64)
# set(BENNI_MINGW_DIR ${CMAKE_CURRENT_LIST_DIR}/toolchain/mingw1310_64)
# set(BENNI_QT6_DIR ${CMAKE_CURRENT_LIST_DIR}/toolchain/Qt-6.8.2_mingw_64)

string(ASCII 27 Esc)
set(ColourReset "${Esc}[m")
set(ColourBold  "${Esc}[1m")
set(Red         "${Esc}[31m")
set(Green       "${Esc}[32m")
set(Yellow      "${Esc}[33m")
set(Blue        "${Esc}[34m")
set(Magenta     "${Esc}[35m")
set(Cyan        "${Esc}[36m")
set(White       "${Esc}[37m")
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")

if (BENNI_USE_QT EQUAL 1)
    # D:\dev\_benni_dev\toolchain\Qt-6.8.2_mingw_64\bin
    set(QT6_DIR ${BENNI_QT6_DIR})
    set(QT_DIR ${BENNI_QT6_DIR})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${BENNI_QT6_DIR}/lib/cmake)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${BENNI_QT6_DIR}/lib/cmake)
endif()

find_program(NASM_EXECUTABLE nasm
    PATHS "C:/sdk/nasm" "C:/NASM"
    DOC "Path to nasm executable"
    REQUIRED
)

# find_program(NASM_EXECUTABLE nasm PATHS "C:/sdk/nasm" REQUIRED)
set(CMAKE_ASM_NASM_COMPILER "${NASM_EXECUTABLE}" CACHE FILEPATH "NASM compiler")
enable_language(ASM_NASM)

# SIMD Level	Compiler Flag	Defined Macro(s)	Intrinsics Header(s)
# AMX (Tile)	-mamx-tile	__AMX__, __AMXTILE__	<amxintrin.h>
# AVX-VNNI	-mavxvnni	__AVXVNNI__             <immintrin.h>
# AVX-512VNNI	-mavx512vnni	__AVX512VNNI__          <immintrin.h>
# AVX-512VBMI	-mavx512vbmi	__AVX512VBMI__          <immintrin.h>
# AVX-512CD	-mavx512cd	__AVX512CD__            <immintrin.h>
# AVX-512VL	-mavx512vl	__AVX512VL__            <immintrin.h>
# AVX-512BW	-mavx512bw	__AVX512BW__            <immintrin.h>
# AVX-512DQ	-mavx512dq	__AVX512DQ__            <immintrin.h>
# AVX-512F	-mavx512f	__AVX512F__             <immintrin.h>
# BMI2          -mbmi2          __BMI2__                <immintrin.h>
# BMI1          -mbmi           __BMI__                 <immintrin.h>
# FMA3          -mfma           __FMA__                 <immintrin.h>
# AVX2          -mavx2          __AVX2__                <immintrin.h>
# AVX           -mavx           __AVX__                 <immintrin.h>
# PCLMULQDQ	-mpclmul	__PCLMUL__              <wmmintrin.h>
# AES-NI        -maes           __AES__                 <wmmintrin.h>
# POPCNT        -mpopcnt	__POPCNT__              <popcntintrin.h>
# SSE4.2        -msse4.2	__SSE4_2__              <nmmintrin.h>
# SSE4.1        -msse4.1	__SSE4_1__              <smmintrin.h>
# SSSE3         -mssse3         __SSSE3__               <tmmintrin.h>
# SSE3          -msse3          __SSE3__                <pmmintrin.h>
# SSE2          -msse2          __SSE2__                <emmintrin.h>
# SSE           -msse           __SSE__                 <xmmintrin.h>
# MMX           -mmmx           __MMX__                 <mmintrin.h>

# <INIT>
set(MY_C_FLAGS "-target x86_64-w64-windows-gnu")
set(MY_CXX_FLAGS "-target x86_64-w64-windows-gnu")
set(MY_LD_FLAGS "-fuse-ld=lld")
	
if(NOT DEFINED BENNI_BUILD_STATIC)
    set(BENNI_BUILD_STATIC 0)
	message(AUTHOR_WARNING "BENNI_BUILD_STATIC was not defined, so i set it to 0.")
endif()

if (BENNI_BUILD_STATIC)
	set(BUILD_SHARED_LIBS 0)
else()
	set(BUILD_SHARED_LIBS 1)
endif()

# <SHARED|STATIC>
if (BUILD_SHARED_LIBS)
   message("[benni] BUILD_SHARED")
else()
   message("[benni] BUILD_STATIC")
   # set(MY_LD_FLAGS "${MY_LD_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# <LTO>
if (BENNI_USE_LTO)
    message("[benni] HAVE_LTO")
    set(MY_C_FLAGS "${MY_C_FLAGS} -flto")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -flto")
    set(MY_LD_FLAGS "${MY_LD_FLAGS} -flto")
endif()

# <OMIT-FRAME-POINTER>
if (BENNI_USE_OMIT_FRAMEPOINTER)
    message("[benni] HAVE_OMIT_FRAMEPOINTER")
    set(MY_C_FLAGS "${MY_C_FLAGS} -fno-omit-frame-pointer")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -fno-omit-frame-pointer")
endif()

# <SIMD> Don't use add_compile_options(), it would pollute ASM commandLines.
if (BENNI_USE_FMA3)
    message("[benni] HAVE_FMA3")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -mfma")
    set(MY_C_FLAGS "${MY_C_FLAGS} -mfma")
    add_compile_definitions(HAVE_FMA3)
endif()

if (BENNI_USE_AVX512)
    message("[benni] HAVE_AVX512")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -mavx512f -mavx512dq -mavx512bw -mavx512vl")
    set(MY_C_FLAGS "${MY_C_FLAGS} -mavx512f -mavx512dq -mavx512bw -mavx512vl")
    add_compile_definitions(HAVE_AVX512)
    set(BENNI_USE_AVX2 1)
endif()

if (BENNI_USE_AVX2)
    message("[benni] HAVE_AVX2")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -mavx2")
    set(MY_C_FLAGS "${MY_C_FLAGS} -mavx2")
    add_compile_definitions(HAVE_AVX2)
    set(BENNI_USE_AVX 1)
endif()

if (BENNI_USE_AVX)
    message("[benni] HAVE_AVX")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -mavx")
    set(MY_C_FLAGS "${MY_C_FLAGS} -mavx")
    add_compile_definitions(HAVE_AVX)
    set(BENNI_USE_SSE42 1)
endif()

if (BENNI_USE_SSE42)
    message("[benni] HAVE_SSE42")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -msse4.2")
    set(MY_C_FLAGS "${MY_C_FLAGS} -msse4.2")
    add_compile_definitions(HAVE_SSE4_2)
    set(BENNI_USE_SSE41 1)
endif()

if (BENNI_USE_SSE41)
    message("[benni] HAVE_SSE41")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -msse4.1")
    set(MY_C_FLAGS "${MY_C_FLAGS} -msse4.1")
    add_compile_definitions(HAVE_SSE4_1)
    set(BENNI_USE_SSSE3 1)
endif()

if (BENNI_USE_SSSE3)
    message("[benni] HAVE_SSSE3")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -mssse3")
    set(MY_C_FLAGS "${MY_C_FLAGS} -mssse3")
    add_compile_definitions(HAVE_SSSE3)
    set(BENNI_USE_SSE3 1)
endif()

if (BENNI_USE_SSE3)
    message("[benni] HAVE_SSE3")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -msse3")
    set(MY_C_FLAGS "${MY_C_FLAGS} -msse3")
    add_compile_definitions(HAVE_SSE3)
    set(BENNI_USE_SSE2 1)
endif()

if (BENNI_USE_SSE2)
    message("[benni] HAVE_SSE2")
    set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -msse2")
    set(MY_C_FLAGS "${MY_C_FLAGS} -msse2")
    add_compile_definitions(HAVE_SSE2)
    add_compile_definitions(HAVE_SSE)
    add_compile_definitions(HAVE_MMX)
endif()


project(DarkC++_Qt6_mingw64_1706_llvm)

# set(CMAKE_ASM_NASM_COMPILER "C:/sdk/nasm/nasm.exe" CACHE FILEPATH "Path to NASM executable")
# enable_language(ASM_NASM)

# set(BENNI_TOOLS_DIR ${CMAKE_CURRENT_LIST_DIR}/toolchain64_mingw11) # Benni Toolchain for Windows64 using the same tools as on linux.
#set(BENNI_MINGW_DIR C:/sdk/Qt15.15.2/Tools/mingw1310_64)
# set(BENNI_MINGW_DIR C:/dev/sdk/mingw64_1410-posix-seh-msvcrt-rt_v12-rev0/bin)
#                   C:\dev\sdk\Qt15.15.2\Tools\mingw81_64
#set(BENNI_QT6_DIR C:/dev/sdk/Qt/6.8.2/mingw_64) # We copy runtime dlls from qtsdk to output dir
#set(BENNI_MINGW_DIR C:/dev/sdk/Qt/Tools/mingw1310_64)
#set(BENNI_QT6_DIR C:/dev/sdk/Qt15.15.2/Tools/mingw81_64) # We copy runtime dlls from qtsdk to output dir
#set(BENNI_MINGW_DIR ${BENNI_QT6_DIR})

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${BENNI_MINGW_DIR}/x86_64-w64-mingw32/include)
# find_program(CMAKE_RC_COMPILER NAMES "${BENNI_MINGW_DIR}/bin/windres32.exe")
# set(MINGW_COMPILER_PREFIX "x86_64-w64-mingw32" CACHE STRING "mingw prefix")
# set(MINGW_SYSROOT "${BENNI_MINGW_DIR}/bin" CACHE STRING "mingw sysroot")
# set(CMAKE_FIND_ROOT_PATH "${MINGW_SYSROOT}")
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# C:\_skeleton\toolchain64_mingw11\mingw64-11.0.1-seh\bin
# set(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_MAKE_PROGRAM "${BENNI_MINGW_DIR}/bin/mingw32-make.exe")
# set(CMAKE_C_COMPILER "${BENNI_MINGW_DIR}/bin/gcc.exe")
# set(CMAKE_CXX_COMPILER "${BENNI_MINGW_DIR}/bin/g++.exe")
# find_program(CMAKE_C_COMPILER NAMES "${MINGW_SYSROOT}/gcc.exe")
# find_program(CMAKE_CXX_COMPILER NAMES "${MINGW_SYSROOT}/g++.exe")
# find_program(CMAKE_MAKE_PROGRAM NAMES "${MINGW_SYSROOT}/mingw32-make.exe")
# set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
# set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
# message("[benni] CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

# :-1: Fehler: Generator: execution of make failed. Make command was: =;C:/SDK/Qt5.12.3/Tools/mingw730_64/bin/mingw32-make.exe -j12 /mingw32-make.exe -j12 /mingw32-make.exe -j12  all
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# set(CMAKE_UNITY_BUILD 1)  # Available only in cmake >= 3.16
# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS ON)

#include(CheckPIESupportrd)
#check_pie_supported()
#target_precompiled_header # Available only in cmake >= 3.16

#if(BENNI_USE_QT)
#   set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${BENNI_QT6_DIR}/lib/cmake")
#   message("${BoldWhite}CMAKE_MODULE_PATH =${CMAKE_MODULE_PATH}${ColourReset}")
#endif()


if (WIN32)
    # set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -DWIN32=1)
    add_compile_definitions(_FILE_OFFSET_BITS=64) # enable fseeko64()
    add_compile_definitions(_USE_MATH_DEFINES=1) # enable M_PI
    add_compile_definitions(_WIN32=1)
	add_compile_definitions(-DWIN32=1)
endif()

# Use folders (for IDE project grouping)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CODEBLOCKS_MAKE_ARGUMENTS "-j16")

# set(BENNI_BUILD_NASM 0)
# -DCMAKE_PREFIX_PATH=C:/SDK/Qt/5.12.3/mingw73_64/lib/cmake
# set(BENNI_QT6_DIR "C:/SDK/Qt/5.12.3/mingw73_64") # Hardcoded is easier here
# set(CMAKE_PREFIX_PATH="${BENNI_QT6_DIR}/lib/cmake")
# set(BENNI_SANITIZE 1)
# set(CMAKE_FIND_ROOT_PATH "C:/SDK/Qt/5.12.3/mingw73_64/lib/cmake")
# set(CMAKE_RC_CREATE_SHARED_LIBRARY

# @buildenv Setup C++ Toolchain on Window64
#           1. Install QtSDK to C:/SDK/Qt5.12.3
#           1.1. Select QtCreator
#           1.2. Select QtLibs ( only select MinGW compiler built libs, to be compatible with benni magic )
#           2.3. Select Tools/MinGW7.3.1 and get a modern GNU C++ compiler on Win64, convincingly simple.
#           2.4. DONT select Tools/StrawberryPerl because it overwrites PATH var with crap (very old g++ that is delivered with perl)
#           2. Install CMake to C:/SDK/CMake
#           3. Install Git   to C:/SDK/Git ( i use its bash terminal and .bashrc to add my fasterwork commands )

# Tell MSVC to use main instead of WinMain for Windows subsystem executables
#if (MSVC)
# set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
#endif()
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE PACKAGE_GIT_VERSION
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)

#execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#RESULT_VARIABLE GIT_SUBMOD_RESULT)
#if(NOT GIT_SUBMOD_RESULT EQUAL "0")
# message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
#endif()

#execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
#                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#                OUTPUT_VARIABLE PACKAGE_GIT_VERSION
#                ERROR_QUIET
#                OUTPUT_STRIP_TRAILING_WHITESPACE)

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)

# if (BUILD_STATIC)
   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
# endif()

# if (BENNI_USE_ASAN)
#     set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#     set (CMAKE_CXX_LINKER_FLAGS_DEBUG "${CMAKE_CXX_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -lasan")
# endif()

# =============================================================
# Detect debug type, e.g. build Release, Debug, MinSizeRel=default
# =============================================================
if(CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "Release")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(BUILD_DEBUG=1) # Used in applications to add "d" postfix to TARGET_NAME
   add_compile_definitions(_DEBUG=1)
   add_compile_definitions(DEBUG=1)
   add_compile_definitions(BUILD_DEBUG=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
   # add_compile_definitions(NDEBUG=0)
   add_compile_definitions(BUILD_RELEASE=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
   add_compile_definitions(NDEBUG=1)
   add_compile_definitions(BUILD_RELEASE=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
   add_compile_definitions(NDEBUG=1)
   add_compile_definitions(BUILD_RELEASE=1)
endif()
message("[benni] CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

if(WIN32)
   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

if (MSVC)
else()
    # set(MY_CXX_FLAGS "${MY_C_FLAGS} -Wall -Wfatal-errors -fext-numeric-literals") #  -Werror=no-return
    # set(MY_C_FLAGS "-fpic")
    # set(MY_C_FLAGS "${MY_C_FLAGS} -mtune=native")
    # set(MY_C_FLAGS "${MY_C_FLAGS} -march=native")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -O0 -g")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wl,--demangle")
       # set(MY_LD_FLAGS "${MY_LD_FLAGS} -Wl,--demangle")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -fsanitize=address")
       #set(MY_LD_FLAGS "${MY_LD_FLAGS} -lasan")
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -O2 -g")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -fsanitize=address")
       #set(MY_LD_FLAGS "${MY_LD_FLAGS} -lasan")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -mfpmath=sse -msse3 -mmmx -O2")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -fno-asynchronous-unwind-tables")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-unused-but-set-variable")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-unknown-pragmas")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-sign-compare")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-unused-variable")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-unused-function")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-comment")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-strict-aliasing")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -Wno-maybe-uninitialized")
       # set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -fexpensive-optimizations -ftree-vectorize")
       # set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -fno-rtti")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -fno-exceptions")
       # set(MY_C_FLAGS "${MY_C_FLAGS} -fstrict-aliasing")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -ffast-math")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -fassociative-math")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -funsafe-math-optimizations")
       #set(MY_C_FLAGS "${MY_C_FLAGS} -funroll-loops")
    endif()

endif()

# set(CMAKE_LINKER_FLAGS "-pthread" CACHE STRING "Linker Flags" FORCE)
# add_definitions(MY_CXX)

# Depending on the boolean chosen above, set CPU_OPTS to the correct cpu opts
#IF (CPUINTEL_native) SET(CPU_OPTS "-mtune=native -march=native -mfpmath=sse -msse3 -mmmx -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops ")
#ELSEIF (CPUAMD_k8) SET(CPU_OPTS "-mtune=athlon64 -mfpmath=sse -msse2 -mmmx -m3dnow -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -fprefetch-loop-arrays ")
#ELSEIF (CPUAMD_K9) SET(CPU_OPTS "-mtune=k8-sse3 -mfpmath=sse -msse3 -mmmx -m3dnow -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops ")
#ELSEIF (CPUINTEL_p4) SET(CPU_OPTS "-mtune=pentium4 -mfpmath=sse -msse2 -mmmx -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops ")
#ELSEIF (CPUINTEL_prescott) SET(CPU_OPTS "-mtune=prescott -mfpmath=sse -msse3 -mmmx -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops " )
#ELSEIF (CPUINTEL_nocona) SET(CPU_OPTS "-mtune=nocona -mfpmath=sse -msse3 -mmmx -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops ")
#ELSEIF (CPUINTEL_core2) SET(CPU_OPTS "-mtune=core2 -mfpmath=sse -msse3 -mmmx -ftree-vectorize -ffast-math -fassociative-math -funsafe-math-optimizations -funroll-loops ")
#ELSEIF (CPUAMD_vishera) SET(CPU_OPTS "-march=bdver2 -mfpmath=both -mmmx -msse4a -msse4.2 -maes -mpclmul -mfsgsbase -mrdrnd -mf16c -mfma -mfma4 -m3dnow -mxop -mlwp -mtbm -mavx2 -mvzeroupper -msahf -mcx16 -funsafe-loop-optimizations ")
#ENDIF (CPUAMD_k8)

#IF (CPU_SMP GREATER 1)
#    SET(CPU_OPTS " ${CPU_OPTS} -ftree-parallelize-loops=${CPU_SMP} ")
#ENDIF (CPU_SMP GREATER 1)

# =============================================================
# Detect host OS name, e.g. Linux/Win/Mac
# =============================================================
message("[benni] Detect OS - operating system ...")
if (WIN32)
   message("[benni] WIN32")
   set(BENNI_OS_NAME "win")
   message("[benni] BENNI_OS_NAME = win")
   add_compile_definitions(BENNI_OS_WINDOWS=1)
   add_compile_definitions(WIN32_LEAN_AND_MEAN)
   add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
   add_compile_definitions(_CRT_SECURE_NO_WARNINGS) 	# Shut up about unsafe stuff
   add_compile_definitions(_CRT_NONSTDC_NO_WARNINGS)
   if (MSYS)
      message("[benni] Has MSYS")
   endif()
   if (MINGW)
      message("[benni] Has MINGW")
   endif()
   if (CYGWIN)
      message("[benni] Has CYGWIN")
   endif()
endif()
if (UNIX)
   message("[benni] UNIX")
   set(BENNI_OS_NAME "linux")
   message("[benni] BENNI_OS_NAME = win")
   add_compile_definitions(BENNI_OS_LINUX=1)
endif()
if (APPLE)
   message("[benni] APPLE")
   set(BENNI_OS_NAME "apple")
   message("[benni] BENNI_OS_NAME = apple")
   add_compile_definitions(BENNI_OS_MAC=1)
endif()

# =============================================================
# Detect machine word size:
# =============================================================
# 64 bit host
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(BENNI_MACHINE 64)
   add_compile_definitions(BENNI_MACHINE=64)
   message("[benni] Machine x64_86")
# 32 bit host
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
   set(BENNI_MACHINE 32)
   add_compile_definitions(BENNI_MACHINE=32)
   message("[benni] Machine x86")
endif()

# =============================================================
# Combine os name + address size 32|64
# =============================================================
set(BENNI_HOST "${BENNI_OS_NAME}${BENNI_MACHINE}") # linux64, win64
message("[benni] BENNI_HOST = ${BENNI_HOST}")

# BENNI_BUILD_DIR: <win64|linux64>_<shared|static>_<Debug|Release|MinSizeRel>
set(BUILD_NAME "${BENNI_HOST}_${CMAKE_BUILD_TYPE}_static")
if (BUILD_SHARED_LIBS OR BUILD_SHARED)
   set(BUILD_NAME "${BENNI_HOST}_${CMAKE_BUILD_TYPE}_shared")
endif()
if (BUILD_STATIC)
   set(BUILD_NAME "${BENNI_HOST}_${CMAKE_BUILD_TYPE}_static")
endif()
message("[benni] BENNI_BUILD = ${BUILD_NAME}")

# BENNI_BUILD_DIR: build/<win|linux|mac><32|64>_<shared|static>_<Debug|Release|MinSizeRel>
set(BUILD_DIR "build/${BUILD_NAME}")
message("[benni] BENNI_BUILD_DIR = ${BUILD_DIR}")

# BENNI_OUTPUT_DIR
# set(BENNI_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/bin/${BENNI_HOST}")
# set(BENNI_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/bin/${BUILD_NAME}")
set(BENNI_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/bin/${BUILD_NAME}")
message("[benni] BENNI_OUTPUT_DIR = ${BENNI_OUTPUT_DIR}")

# Output directories for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BENNI_OUTPUT_DIR}") # Where cmake puts .exe files.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BENNI_OUTPUT_DIR}") # Where cmake puts .dll files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ar") # Where cmake puts .a/.lib files.
set(CMAKE_LIBRARY_PATH "${CMAKE_BINARY_DIR}") # Separated list of directories for the find_library() command.
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}") # ?
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_CURRENT_LIST_DIR}/install/include")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_CURRENT_LIST_DIR}/install/lib")
set(CMAKE_INSTALL_BINDIR "${CMAKE_CURRENT_LIST_DIR}/install/bin")

message("[benni] Final CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
message("[benni] Final CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message("[benni] Final CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("[benni] Final CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("[benni] Final CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message("[benni] Final CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}")
message("[benni] Final CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message("[benni] Final CMAKE_INSTALL_INCLUDEDIR = ${CMAKE_INSTALL_INCLUDEDIR}")
message("[benni] Final CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")
message("[benni] Final CMAKE_INSTALL_BINDIR = ${CMAKE_INSTALL_BINDIR}")
message("[benni] Final CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}")
message("[benni] ============================================================================")
message("[benni] Final CMAKE_LD_FLAGS = ${CMAKE_LD_FLAGS}")
message("[benni] Final CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message("[benni] Final CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
message("[benni] Final CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR} ")
message("[benni] Final CMAKE_C_COMPILER_ID = ${CMAKE_C_COMPILER_ID}")
message("[benni] Final CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message("[benni] Final CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message("[benni] Final CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message("[benni] Final CMAKE_MAKE_PROGRAM = ${CMAKE_MAKE_PROGRAM} (Should be ninja)")
message("[benni] Final CMAKE_LINKER = ${CMAKE_LINKER}")
message("[benni] Final CMAKE_AR = ${CMAKE_AR}")
message("[benni] Final CMAKE_RANLIB = ${CMAKE_RANLIB}")
message("[benni] Final CMAKE_RC_COMPILER = ${CMAKE_RC_COMPILER}")
message("[benni] ============================================================================")
message("[benni] MY_C_FLAGS = ${MY_C_FLAGS}")
message("[benni] MY_CXX_FLAGS = ${MY_CXX_FLAGS}")
message("[benni] MY_LD_FLAGS = ${MY_LD_FLAGS}")
message("[benni] ============================================================================")
message("[benni] Before CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message("[benni] Before CMAKE_C_FLAGS_DEBUG = ${CMAKE_C_FLAGS_DEBUG}")
message("[benni] Before CMAKE_C_FLAGS_RELEASE = ${CMAKE_C_FLAGS_RELEASE}")
message("[benni] Before CMAKE_C_FLAGS_RELWITHDEBINFO = ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("[benni] Before CMAKE_C_FLAGS_MINSIZEREL = ${CMAKE_C_FLAGS_MINSIZEREL}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MY_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${CMAKE_C_FLAGS}")
message("[benni] Final CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message("[benni] Final CMAKE_C_FLAGS_DEBUG = ${CMAKE_C_FLAGS_DEBUG}")
message("[benni] Final CMAKE_C_FLAGS_RELEASE = ${CMAKE_C_FLAGS_RELEASE}")
message("[benni] Final CMAKE_C_FLAGS_RELWITHDEBINFO = ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("[benni] Final CMAKE_C_FLAGS_MINSIZEREL = ${CMAKE_C_FLAGS_MINSIZEREL}")
message("[benni] ============================================================================")
message("[benni] Before CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("[benni] Before CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
message("[benni] Before CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")
message("[benni] Before CMAKE_CXX_FLAGS_RELWITHDEBINFO = ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message("[benni] Before CMAKE_CXX_FLAGS_MINSIZEREL = ${CMAKE_CXX_FLAGS_MINSIZEREL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${CMAKE_CXX_FLAGS}")
message("[benni] Final CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("[benni] Final CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
message("[benni] Final CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")
message("[benni] Final CMAKE_CXX_FLAGS_RELWITHDEBINFO = ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message("[benni] Final CMAKE_CXX_FLAGS_MINSIZEREL = ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message("[benni] ============================================================================")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MY_LD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} ${CMAKE_EXE_LINKER_FLAGS}")
message("[benni] Final CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message("[benni] Final CMAKE_EXE_LINKER_FLAGS_DEBUG = ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message("[benni] Final CMAKE_EXE_LINKER_FLAGS_RELEASE = ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message("[benni] Final CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO = ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
message("[benni] Final CMAKE_EXE_LINKER_FLAGS_MINSIZEREL = ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
message("[benni] ============================================================================")
message("[benni] Final CMAKE_STATIC_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
message("[benni] Final CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
message("[benni] Final CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")
# set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fuse-ld=lld")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")


# NEW 2021: This flag is now automated, but it needs the Moved

# Benni: Set to '1' when tools like the assembler are needed,
# The tools get built into <bin/win64> folder and can be used now.
# After building tools set to '0' again and build normally.
# 1 = build tools, including nasm, required to build normally with audio libs
# 0 = build normally ( all activated projects and libraries ).
#set(BENNI_BUILD_NASM 0)   # Build normal ( libs + apps, they need nasm )
#set(BENNI_BUILD_NASM 1) # Build nasm tools
#if(EXISTS ${BENNI_OUTPUT_DIR}/nasm)
#   set(BENNI_BUILD_NASM 0)
#   message("[benni] FOUND ASSEMBLER nasm, build normal")
#endif()
#if(EXISTS ${BENNI_OUTPUT_DIR}/nasm.exe)
#   set(BENNI_BUILD_NASM 0)
#   message("[benni] FOUND ASSEMBLER nasm.exe, build normal")
#endif()
#if(BENNI_BUILD_NASM)
#   message("[benni] NO ASSEMBLER FOUND, BUILD NOW nasm.exe, THEN START AGAIN to build normal")
#endif()

# Tell cmake to use our custom built Assembler tools/nasm.exe
if (WIN32)
    set(BENNI_NASM_EXE "${CMAKE_CURRENT_LIST_DIR}/bin/${BUILD_NAME}/nasm.exe")
    message("${BoldRed}[benni] BENNI_NASM_EXE = ${BENNI_NASM_EXE}${ColourReset}")
else()
    set(BENNI_NASM_EXE "${CMAKE_CURRENT_LIST_DIR}/bin/${BUILD_NAME}/nasm")
    message("${BoldRed}[benni] BENNI_NASM_EXE = ${BENNI_NASM_EXE}${ColourReset}")
endif()

add_subdirectory(src)

message("${ColourReset}")

# Runtime .dlls:
if (WIN32) 
    if (MSVC)
    else()
        if(BUILD_SHARED_LIBS OR BENNI_USE_QT) # Get runtime dlls from LLVM Compiler ( clang++ )

            file(GLOB RUNTIME_FILES "${CMAKE_CURRENT_LIST_DIR}/runtime/*")
            foreach(runtime_file ${RUNTIME_FILES})
                file(COPY ${runtime_file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            endforeach()
            
            # Get runtime dlls from MinGW Compiler ( g++-7.3.0 or so ) in QtSDK
            # file(COPY "${BENNI_MINGW_DIR}/bin/libgcc_s_seh-1.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )  # gcc runtime for windows
            # file(COPY "${BENNI_MINGW_DIR}/bin/libstdc++-6.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})      # gcc runtime for windows
            # file(COPY "${BENNI_MINGW_DIR}/bin/libwinpthread-1.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})  # gcc runtime for windows
            # Get runtime dlls from LLVM Compiler ( clang++ )
            # file(COPY "${BENNI_MINGW_DIR}/bin/libgcc_s_seh-1.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )  # gcc runtime for windows
            # file(COPY "${BENNI_MINGW_DIR}/bin/libstdc++-6.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})      # gcc runtime for windows
            # file(COPY "${BENNI_MINGW_DIR}/bin/libwinpthread-1.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})  # gcc runtime for windows
            # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
            
        else()
        # Do not copy gcc runtime if we have static build and do not use QtSDK (which needs runtime dlls itself)

        endif()
    endif()

    if (BENNI_USE_QT)

      # Get qtwidget dlls from installed QtSDK (with integrated MinGW C++ Compiler)
      ### Debug mode
      # if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      #    file(COPY "${BENNI_QT6_DIR}/bin/Qt5Cored.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/bin/Qt5Guid.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/bin/Qt5Widgetsd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/bin/Qt5WinExtrasd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/bin/Qt5Svgd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/plugins/platforms/qwindowsd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms) # Debug
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qgifd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qicnsd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qicod.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qjpegd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qsvgd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qtgad.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qtiffd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qwbmpd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      #    file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qwebpd.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
      ### Release|MinSizeRel mode
      # else()
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Core.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Concurrent.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Gui.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Network.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6OpenGL.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		 file(COPY "${BENNI_QT6_DIR}/bin/Qt6OpenGLWidgets.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6PrintSupport.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Qml.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6QmlModels.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Quick.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6QuickWidgets.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Sql.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Svg.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/bin/Qt6Widgets.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         #file(COPY "${BENNI_QT6_DIR}/bin/Qt6WinExtras.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         file(COPY "${BENNI_QT6_DIR}/plugins/platforms/qwindows.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms)
         file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qgif.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qgif.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         #file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qicns.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qico.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qjpeg.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qsvg.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         #file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qtga.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         #file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qtiff.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         #file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qwbmp.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
         #file(COPY "${BENNI_QT6_DIR}/plugins/imageformats/qwebp.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imageformats)
     # endif()
   endif()
endif()

#[[
if (BENNI_USE_NASM)
    if (NOT EXISTS ${BENNI_NASM_EXE})
        message("${BoldRed}[benni] Build Phase I (setup) - tools needed for Phase II${ColourReset}")
        message("${BoldRed}[benni] Build assembler nasm.exe (${BENNI_NASM_EXE})${ColourReset}")
        add_subdirectory(src/0_tools)
        message("${BoldRed}[benni] Build Phase I (setup) - Done.${ColourReset}")
        message("${BoldRed}[benni] Please run this again for Phase II ${ColourReset}")
    else()
        message("${BoldRed}[benni] Build Phase II (normal) - Libs and Apps${ColourReset}")
        # set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
        set(CMAKE_ASM_NASM_COMPILER "${BENNI_NASM_EXE}")
        # set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm S)
        # set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
        # set(CMAKE_ASM_OBJECT_FORMAT elf64)
        # set(CMAKE_ASM_OBJECT_FORMAT ${CMAKE_ASM_NASM_OBJECT_FORMAT})
        # set(CMAKE_NASM_COMPILER_ENV_VAR ${CMAKE_NASM_COMPILER})
        # set(CMAKE_ASM_OBJECT_FORMAT elf64)
        #set(CMAKE_NASM_COMPILER ${CMAKE_ASM_NASM_COMPILER})

        # support for the asm assembler
        # set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <INCLUDES> \
        #   <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

        # support for the nasm assembler
        set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
          <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

        # set_source_files_properties(src/strlen.asm PROPERTIES COMPILE_FLAGS "-g -Fdwarf")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS} -g") # -Fdwarf
        else()
            set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS}")
        endif()

        # if (NOT EXISTS CMAKE_ASM_NASM_COMPILER)
        # # set(BENNI_BUILD_NASM 1)
        # add_subdirectory(tools)
        # else()
        # # set(BENNI_BUILD_NASM 0)
        # endif()

        if(NOT CMAKE_ASM_NASM_OBJECT_FORMAT)
            if(WIN32)
                if(CMAKE_C_SIZEOF_DATA_PTR EQUAL 8)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
                else()
                set(CMAKE_ASM_NASM_OBJECT_FORMAT win32)
                endif()
            elseif(APPLE)
                if(CMAKE_C_SIZEOF_DATA_PTR EQUAL 8)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT macho64)
                else()
                set(CMAKE_ASM_NASM_OBJECT_FORMAT macho)
                endif()
            else()
                if(CMAKE_C_SIZEOF_DATA_PTR EQUAL 8)
                set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
                else()
                set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
                endif()
            endif()
        endif()

        add_subdirectory(src)

    endif()
else()
    add_subdirectory(src)
endif()
]]

if (0)
   set(CMAKE_NASM_COMPILER ${CMAKE_ASM_NASM_COMPILER})
   set(CMAKE_NASM_COMPILER_ENV_VAR ${CMAKE_NASM_COMPILER})
   set(CMAKE_ASM_OBJECT_FORMAT ${CMAKE_ASM_NASM_OBJECT_FORMAT})
   set(CMAKE_ASM_COMPILER ${CMAKE_ASM_NASM_COMPILER})

   # support for the asm assembler
   set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <INCLUDES> \
       <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

   # support for the nasm assembler
   set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
       <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

   # set_source_files_properties(src/strlen.asm PROPERTIES COMPILE_FLAGS "-g -Fdwarf")
   if(CMAKE_BUILD_TYPE STREQUAL "Debug")
       set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS} -g -Fdwarf")
   else()
       set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS}")
   endif()

   # support for the nasm assembler
   set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
       <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

   message("[benni] CMAKE_ASM_NASM_FLAGS = ${CMAKE_ASM_NASM_FLAGS}")
   message("[benni] CMAKE_ASM_NASM_COMPILER = ${CMAKE_ASM_NASM_COMPILER}")
   message("[benni] CMAKE_ASM_COMPILER = ${CMAKE_ASM_COMPILER}")
   message("[benni] CMAKE_ASM_COMPILER_ENV_VAR = ${CMAKE_ASM_COMPILER_ENV_VAR}")
   message("[benni] CMAKE_NASM_COMPILER = ${CMAKE_NASM_COMPILER}")
   message("[benni] CMAKE_NASM_COMPILER_ENV_VAR = ${CMAKE_NASM_COMPILER_ENV_VAR}")
   message("[benni] CMAKE_ASM_NASM_OBJECT_FORMAT = ${CMAKE_ASM_NASM_OBJECT_FORMAT}")
   message("[benni] CMAKE_ASM_NASM_COMPILE_OBJECT = ${CMAKE_ASM_NASM_COMPILE_OBJECT}")
   message("${ColourReset}")

   enable_language(ASM_NASM)
endif(0)
