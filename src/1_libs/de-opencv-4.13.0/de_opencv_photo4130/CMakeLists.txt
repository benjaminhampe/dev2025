set(LIB_NAME de_opencv_photo4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/photo/cuda.hpp
        ${LIB_HEADER_DIR}/opencv2/photo/legacy/constants_c.h
        ${LIB_HEADER_DIR}/opencv2/photo/photo.hpp
        ${LIB_HEADER_DIR}/opencv2/photo.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_photo.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_photo.hpp		
        ${LIB_SOURCE_DIR}/align.cpp
        ${LIB_SOURCE_DIR}/arrays.hpp
        ${LIB_SOURCE_DIR}/calibrate.cpp
        ${LIB_SOURCE_DIR}/contrast_preserve.cpp
        ${LIB_SOURCE_DIR}/contrast_preserve.hpp

        # ${LIB_SOURCE_DIR}/cuda/nlm.cu
		
        ${LIB_SOURCE_DIR}/denoise_tvl1.cpp
        ${LIB_SOURCE_DIR}/denoising.cpp
        # ${LIB_SOURCE_DIR}/denoising.cuda.cpp
        ${LIB_SOURCE_DIR}/fast_nlmeans_denoising_invoker.hpp
        ${LIB_SOURCE_DIR}/fast_nlmeans_denoising_invoker_commons.hpp
        ${LIB_SOURCE_DIR}/fast_nlmeans_denoising_opencl.hpp
        ${LIB_SOURCE_DIR}/fast_nlmeans_multi_denoising_invoker.hpp
        ${LIB_SOURCE_DIR}/hdr_common.cpp
        ${LIB_SOURCE_DIR}/hdr_common.hpp
        ${LIB_SOURCE_DIR}/inpaint.cpp
        ${LIB_SOURCE_DIR}/merge.cpp
        ${LIB_SOURCE_DIR}/npr.cpp
        ${LIB_SOURCE_DIR}/npr.hpp

        ${LIB_SOURCE_DIR}/opencl/nlmeans.cl
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/seamless_cloning.cpp
        ${LIB_SOURCE_DIR}/seamless_cloning.hpp
        ${LIB_SOURCE_DIR}/seamless_cloning_impl.cpp
        ${LIB_SOURCE_DIR}/tonemap.cpp
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)

endif()

# add_subdirectory(test)
