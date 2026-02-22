set(LIB_NAME de_opencv_stitching4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/autocalib.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/blenders.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/camera.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/exposure_compensate.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/matchers.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/motion_estimators.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/seam_finders.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/timelapsers.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/util.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/util_inl.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/warpers.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/detail/warpers_inl.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching/warpers.hpp
        ${LIB_HEADER_DIR}/opencv2/stitching.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_stitching.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_stitching.hpp

        ${LIB_SOURCE_DIR}/autocalib.cpp
        ${LIB_SOURCE_DIR}/blenders.cpp
        ${LIB_SOURCE_DIR}/camera.cpp

        ${LIB_SOURCE_DIR}/cuda/build_warp_maps.cu
        ${LIB_SOURCE_DIR}/cuda/multiband_blend.cu
		
        ${LIB_SOURCE_DIR}/exposure_compensate.cpp
        ${LIB_SOURCE_DIR}/matchers.cpp
        ${LIB_SOURCE_DIR}/motion_estimators.cpp

        ${LIB_SOURCE_DIR}/opencl/multibandblend.cl
        ${LIB_SOURCE_DIR}/opencl/warpers.cl
		
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/seam_finders.cpp
        ${LIB_SOURCE_DIR}/stitcher.cpp
        ${LIB_SOURCE_DIR}/timelapsers.cpp
        ${LIB_SOURCE_DIR}/util.cpp
        ${LIB_SOURCE_DIR}/util_log.hpp
        ${LIB_SOURCE_DIR}/warpers.cpp
        ${LIB_SOURCE_DIR}/warpers_cuda.cpp
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
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_features2d4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_calib3d4130)

endif()

# add_subdirectory(test)
