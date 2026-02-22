set(LIB_NAME de_opencv_features2d4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/features2d.hpp
        ${LIB_HEADER_DIR}/opencv2/features2d/features2d.hpp
        ${LIB_HEADER_DIR}/opencv2/features2d/hal/interface.h
    )
    set(LIB_SOURCES
		${LIB_SOURCE_DIR}/_benni/3rdparty/mscr/chi_table.h
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_features2d.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_features2d.hpp
        # ${LIB_SOURCE_DIR}/_benni/sift.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/sift.avx512_skx.cpp
        # ${LIB_SOURCE_DIR}/_benni/sift.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/sift.sse4_1.cpp
	
        ${LIB_SOURCE_DIR}/affine_feature.cpp
        ${LIB_SOURCE_DIR}/agast.cpp
        ${LIB_SOURCE_DIR}/agast_score.cpp
        ${LIB_SOURCE_DIR}/agast_score.hpp
        ${LIB_SOURCE_DIR}/akaze.cpp
        ${LIB_SOURCE_DIR}/bagofwords.cpp
        ${LIB_SOURCE_DIR}/blobdetector.cpp
        ${LIB_SOURCE_DIR}/brisk.cpp
        ${LIB_SOURCE_DIR}/draw.cpp
        ${LIB_SOURCE_DIR}/dynamic.cpp
        ${LIB_SOURCE_DIR}/evaluation.cpp
        # ${LIB_SOURCE_DIR}/fast.avx2.cpp
        ${LIB_SOURCE_DIR}/fast.cpp
        ${LIB_SOURCE_DIR}/fast.hpp
        ${LIB_SOURCE_DIR}/fast_score.cpp
        ${LIB_SOURCE_DIR}/fast_score.hpp
        ${LIB_SOURCE_DIR}/feature2d.cpp
        ${LIB_SOURCE_DIR}/gftt.cpp
        ${LIB_SOURCE_DIR}/hal_replacement.hpp

        ${LIB_SOURCE_DIR}/kaze/AKAZEConfig.h
        ${LIB_SOURCE_DIR}/kaze/AKAZEFeatures.cpp
        ${LIB_SOURCE_DIR}/kaze/AKAZEFeatures.h
        ${LIB_SOURCE_DIR}/kaze/fed.cpp
        ${LIB_SOURCE_DIR}/kaze/fed.h
        ${LIB_SOURCE_DIR}/kaze/KAZEConfig.h
        ${LIB_SOURCE_DIR}/kaze/KAZEFeatures.cpp
        ${LIB_SOURCE_DIR}/kaze/KAZEFeatures.h
        ${LIB_SOURCE_DIR}/kaze/nldiffusion_functions.cpp
        ${LIB_SOURCE_DIR}/kaze/nldiffusion_functions.h
        ${LIB_SOURCE_DIR}/kaze/TEvolution.h
        ${LIB_SOURCE_DIR}/kaze/utils.h
        ${LIB_SOURCE_DIR}/kaze.cpp
        ${LIB_SOURCE_DIR}/keypoint.cpp
        ${LIB_SOURCE_DIR}/main.cpp
        ${LIB_SOURCE_DIR}/matchers.cpp
        ${LIB_SOURCE_DIR}/mser.cpp
        ${LIB_SOURCE_DIR}/orb.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/sift.dispatch.cpp
        ${LIB_SOURCE_DIR}/sift.simd.hpp

        ${LIB_SOURCE_DIR}/opencl/akaze.cl
        ${LIB_SOURCE_DIR}/opencl/brute_force_match.cl
        ${LIB_SOURCE_DIR}/opencl/fast.cl
        ${LIB_SOURCE_DIR}/opencl/orb.cl		
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
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_flann4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)

endif()

# add_subdirectory(test)
