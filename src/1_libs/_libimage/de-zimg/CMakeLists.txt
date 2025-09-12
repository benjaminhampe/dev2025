set(LIB_NAME de_zimg) # We dont like to clash with system libs.
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/zimg)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/zimg)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/zimg.h
        ${LIB_HEADER_DIR}/zimg++.hpp
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/zimg.cpp # api folder
        ${LIB_SOURCE_DIR}/colorspace/colorspace.cpp
        ${LIB_SOURCE_DIR}/colorspace/colorspace_param.cpp
        ${LIB_SOURCE_DIR}/colorspace/gamma.cpp
        ${LIB_SOURCE_DIR}/colorspace/graph.cpp
        ${LIB_SOURCE_DIR}/colorspace/matrix3.cpp
        ${LIB_SOURCE_DIR}/colorspace/operation.cpp
        ${LIB_SOURCE_DIR}/colorspace/operation_impl.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/gamma_constants_avx512.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_avx.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_avx2.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_avx512.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_sse.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_sse2.cpp
        ${LIB_SOURCE_DIR}/colorspace/x86/operation_impl_x86.cpp
        ${LIB_SOURCE_DIR}/common/cpuinfo.cpp
        ${LIB_SOURCE_DIR}/common/libm_wrapper.cpp
        ${LIB_SOURCE_DIR}/common/matrix.cpp
        ${LIB_SOURCE_DIR}/common/x86/cpuinfo_x86.cpp
        ${LIB_SOURCE_DIR}/common/x86/x86util.cpp
        ${LIB_SOURCE_DIR}/depth/blue.cpp
        ${LIB_SOURCE_DIR}/depth/depth.cpp
        ${LIB_SOURCE_DIR}/depth/depth_convert.cpp
        ${LIB_SOURCE_DIR}/depth/dither.cpp
        ${LIB_SOURCE_DIR}/depth/quantize.cpp
        ${LIB_SOURCE_DIR}/depth/x86/depth_convert_avx2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/depth_convert_avx512.cpp
        ${LIB_SOURCE_DIR}/depth/x86/depth_convert_sse2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/depth_convert_x86.cpp
        ${LIB_SOURCE_DIR}/depth/x86/dither_avx2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/dither_avx512.cpp
        ${LIB_SOURCE_DIR}/depth/x86/dither_sse2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/dither_x86.cpp
        ${LIB_SOURCE_DIR}/depth/x86/error_diffusion_avx2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/error_diffusion_sse2.cpp
        ${LIB_SOURCE_DIR}/depth/x86/f16c_ivb.cpp
        ${LIB_SOURCE_DIR}/depth/x86/f16c_sse2.cpp
        ${LIB_SOURCE_DIR}/graph/basic_filter.cpp
        ${LIB_SOURCE_DIR}/graph/filtergraph.cpp
        ${LIB_SOURCE_DIR}/graph/graphbuilder.cpp
        ${LIB_SOURCE_DIR}/graph/graphnode.cpp
        ${LIB_SOURCE_DIR}/resize/filter.cpp
        ${LIB_SOURCE_DIR}/resize/resize.cpp
        ${LIB_SOURCE_DIR}/resize/resize_impl.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_avx.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_avx2.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_avx512.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_avx512_vnni.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_sse.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_sse2.cpp
        ${LIB_SOURCE_DIR}/resize/x86/resize_impl_x86.cpp
        ${LIB_SOURCE_DIR}/unresize/bilinear.cpp
        ${LIB_SOURCE_DIR}/unresize/unresize.cpp
        ${LIB_SOURCE_DIR}/unresize/unresize_impl.cpp
    )

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        # target_compile_definitions(${LIB_NAME} PUBLIC -DLIBZIMG_STATIC=1)
    endif()

    # target_compile_definitions(${LIB_NAME} PRIVATE ARCH_IS_64BIT=1 )
    # target_compile_definitions(${LIB_NAME} PRIVATE ARCH_IS_X86_64=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ZIMG=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # user <zimg/zimg.h>

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) # for src
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}) # for src

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})  

    if (MSVC)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE m)
    # target_compile_options(${LIB_NAME} PRIVATE -O3)   

    endif()
    
    
endif()

add_subdirectory(test)
