#ifndef OPENCV_CVCONFIG_H_INCLUDED
#define OPENCV_CVCONFIG_H_INCLUDED

/* OpenCV compiled as static or dynamic libs */
/* #undef BUILD_SHARED_LIBS */
// #ifdef _WIN32
// #ifndef HAVE_DSHOW
// #define HAVE_DSHOW
// #endif
// #endif

/* OpenCV intrinsics optimized code */
#define CV_ENABLE_INTRINSICS

/* OpenCV additional optimized code */
/* #undef CV_DISABLE_OPTIMIZATION */

/* Compile for 'real' NVIDIA GPU architectures */
#define CUDA_ARCH_BIN ""

/* NVIDIA GPU features are used */
#define CUDA_ARCH_FEATURES ""

/* Compile for 'virtual' NVIDIA PTX architectures */
#define CUDA_ARCH_PTX ""

/* AMD's Basic Linear Algebra Subprograms Library*/
/* #undef HAVE_CLAMDBLAS */

/* AMD's OpenCL Fast Fourier Transform Library*/
/* #undef HAVE_CLAMDFFT */

/* Clp support */
/* #undef HAVE_CLP */

/* NVIDIA CUDA Runtime API*/
/* #undef HAVE_CUDA */

/* NVIDIA CUDA Basic Linear Algebra Subprograms (BLAS) API*/
/* #undef HAVE_CUBLAS */

/* NVIDIA CUDA Deep Neural Network (cuDNN) API*/
/* #undef HAVE_CUDNN */

/* NVIDIA CUDA Fast Fourier Transform (FFT) API*/
/* #undef HAVE_CUFFT */

/* DirectX */
#define HAVE_DIRECTX
#define HAVE_DIRECTX_NV12
/* NVIDIA OpenCL D3D Extensions support */
//#define HAVE_OPENCL_D3D11_NV
#define HAVE_D3D11
#define HAVE_D3D10
#define HAVE_D3D9

/* Eigen Matrix & Linear Algebra Library */
/* #undef HAVE_EIGEN */

/* Geospatial Data Abstraction Library */
/* #undef HAVE_GDAL */

/* Halide support */
/* #undef HAVE_HALIDE */

/* Vulkan support */
/* #undef HAVE_VULKAN */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Intel Integrated Performance Primitives */
/* #undef HAVE_IPP */
/* #undef HAVE_IPP_ICV */
/* #undef HAVE_IPP_IW */
/* #undef HAVE_IPP_IW_LL */

/* JPEG-2000 codec */
// #ifndef HAVE_OPENJPEG
// #define HAVE_OPENJPEG
// #endif
/* #undef HAVE_JASPER */

/* AVIF codec */
/* #undef HAVE_AVIF */

/* IJG JPEG codec */
// #ifndef HAVE_JPEG
// #define HAVE_JPEG
// #endif
/* JPEG XL codec */
/* #undef HAVE_JPEGXL */

/* GDCM DICOM codec */
/* #undef HAVE_GDCM */

/* NVIDIA Video Decoding API*/
/* #undef HAVE_NVCUVID */
/* #undef HAVE_NVCUVID_HEADER */
/* #undef HAVE_DYNLINK_NVCUVID_HEADER */

/* NVIDIA Video Encoding API*/
/* #undef HAVE_NVCUVENC */

/* OpenCL Support */
// #ifndef HAVE_OPENCL
// #define HAVE_OPENCL
// #endif
/* #undef HAVE_OPENCL_STATIC */
/* #undef HAVE_OPENCL_SVM */


/* OpenEXR codec */
// #ifndef HAVE_OPENEXR
// #define HAVE_OPENEXR
// #endif

/* OpenGL support*/
/* #undef HAVE_OPENGL */

/* PNG codec */
// #ifndef HAVE_PNG
// #define HAVE_PNG
// #endif

/* PNG codec */
/* #undef HAVE_SPNG */

/* Posix threads (pthreads) */
#ifndef HAVE_PTHREAD
#define HAVE_PTHREAD
#endif

/* parallel_for with pthreads */
#define HAVE_PTHREADS_PF

/* Intel Threading Building Blocks */
/* #undef HAVE_TBB */

/* Ste||ar Group High Performance ParallelX */
/* #undef HAVE_HPX */

/* TIFF codec */
// #ifndef HAVE_TIFF
// #define HAVE_TIFF
// #endif

/* Define if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* VA library (libva) */
/* #undef HAVE_VA */

/* Intel VA-API/OpenCL */
/* #undef HAVE_VA_INTEL */

/* Lapack */
/* #undef HAVE_LAPACK */

/* Library was compiled with functions instrumentation */
/* #undef ENABLE_INSTRUMENTATION */

/* OpenVX */
/* #undef HAVE_OPENVX */

/* OpenCV trace utilities */
#define OPENCV_TRACE

/* Library QR-code decoding */
/* #undef HAVE_QUIRC */

#endif // OPENCV_CVCONFIG_H_INCLUDED
