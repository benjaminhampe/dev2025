set(LIB_NAME de_opencv_imgproc4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/imgproc/bindings.hpp

        ${LIB_HEADER_DIR}/opencv2/imgproc/detail/gcgraph.hpp
        ${LIB_HEADER_DIR}/opencv2/imgproc/detail/legacy.hpp

        ${LIB_HEADER_DIR}/opencv2/imgproc/hal/hal.hpp
        ${LIB_HEADER_DIR}/opencv2/imgproc/hal/interface.h
        ${LIB_HEADER_DIR}/opencv2/imgproc/imgproc.hpp
        ${LIB_HEADER_DIR}/opencv2/imgproc/imgproc_c.h
        ${LIB_HEADER_DIR}/opencv2/imgproc/segmentation.hpp
        ${LIB_HEADER_DIR}/opencv2/imgproc/types_c.h
        ${LIB_HEADER_DIR}/opencv2/imgproc.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_imgproc.hpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_imgproc.cpp
        ${LIB_SOURCE_DIR}/_benni/accum.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/bilateral_filter.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/box_filter.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/color_hsv.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/color_rgb.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/color_yuv.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/filter.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/median_blur.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/morph.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/smooth.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/sumpixels.simd_declarations.hpp

        ${LIB_SOURCE_DIR}/accum.cpp
        ${LIB_SOURCE_DIR}/accum.dispatch.cpp
        ${LIB_SOURCE_DIR}/accum.simd.hpp
        ${LIB_SOURCE_DIR}/approx.cpp
        ${LIB_SOURCE_DIR}/bilateral_filter.dispatch.cpp
        ${LIB_SOURCE_DIR}/bilateral_filter.simd.hpp
        ${LIB_SOURCE_DIR}/blend.cpp
        ${LIB_SOURCE_DIR}/box_filter.dispatch.cpp
        ${LIB_SOURCE_DIR}/box_filter.simd.hpp
        ${LIB_SOURCE_DIR}/canny.cpp
        ${LIB_SOURCE_DIR}/ccl_bolelli_forest.inc.hpp
        ${LIB_SOURCE_DIR}/ccl_bolelli_forest_firstline.inc.hpp
        ${LIB_SOURCE_DIR}/ccl_bolelli_forest_lastline.inc.hpp
        ${LIB_SOURCE_DIR}/ccl_bolelli_forest_singleline.inc.hpp
        ${LIB_SOURCE_DIR}/clahe.cpp
        ${LIB_SOURCE_DIR}/color.cpp
        ${LIB_SOURCE_DIR}/color.hpp
        ${LIB_SOURCE_DIR}/color.simd_helpers.hpp
        ${LIB_SOURCE_DIR}/colormap.cpp
        ${LIB_SOURCE_DIR}/color_hsv.dispatch.cpp
        ${LIB_SOURCE_DIR}/color_hsv.simd.hpp
        ${LIB_SOURCE_DIR}/color_lab.cpp
        ${LIB_SOURCE_DIR}/color_rgb.dispatch.cpp
        ${LIB_SOURCE_DIR}/color_rgb.simd.hpp
        ${LIB_SOURCE_DIR}/color_yuv.dispatch.cpp
        ${LIB_SOURCE_DIR}/color_yuv.simd.hpp
        ${LIB_SOURCE_DIR}/connectedcomponents.cpp
        ${LIB_SOURCE_DIR}/contours.cpp
        ${LIB_SOURCE_DIR}/contours_approx.cpp
        ${LIB_SOURCE_DIR}/contours_blockstorage.hpp
        ${LIB_SOURCE_DIR}/contours_common.cpp
        ${LIB_SOURCE_DIR}/contours_common.hpp
        ${LIB_SOURCE_DIR}/contours_link.cpp
        ${LIB_SOURCE_DIR}/contours_new.cpp
        ${LIB_SOURCE_DIR}/convhull.cpp
        # ${LIB_SOURCE_DIR}/corner.avx.cpp
        ${LIB_SOURCE_DIR}/corner.cpp
        ${LIB_SOURCE_DIR}/corner.hpp
        ${LIB_SOURCE_DIR}/cornersubpix.cpp
        ${LIB_SOURCE_DIR}/demosaicing.cpp
        ${LIB_SOURCE_DIR}/deriv.cpp
        ${LIB_SOURCE_DIR}/distransform.cpp
        ${LIB_SOURCE_DIR}/drawing.cpp
        ${LIB_SOURCE_DIR}/emd.cpp
        ${LIB_SOURCE_DIR}/emd_new.cpp
        ${LIB_SOURCE_DIR}/featureselect.cpp
        ${LIB_SOURCE_DIR}/filter.dispatch.cpp
        ${LIB_SOURCE_DIR}/filter.hpp
        ${LIB_SOURCE_DIR}/filter.simd.hpp
        ${LIB_SOURCE_DIR}/filterengine.hpp
        ${LIB_SOURCE_DIR}/fixedpoint.inl.hpp
        ${LIB_SOURCE_DIR}/floodfill.cpp
        ${LIB_SOURCE_DIR}/gabor.cpp
        ${LIB_SOURCE_DIR}/generalized_hough.cpp
        ${LIB_SOURCE_DIR}/geometry.cpp
        ${LIB_SOURCE_DIR}/grabcut.cpp
        ${LIB_SOURCE_DIR}/hal_replacement.hpp
        ${LIB_SOURCE_DIR}/hershey_fonts.cpp
        ${LIB_SOURCE_DIR}/histogram.cpp
        ${LIB_SOURCE_DIR}/hough.cpp
        # ${LIB_SOURCE_DIR}/imgwarp.avx2.cpp
        ${LIB_SOURCE_DIR}/imgwarp.cpp
        ${LIB_SOURCE_DIR}/imgwarp.hpp
        # ${LIB_SOURCE_DIR}/imgwarp.lasx.cpp
        # ${LIB_SOURCE_DIR}/imgwarp.sse4_1.cpp
        ${LIB_SOURCE_DIR}/intelligent_scissors.cpp
        ${LIB_SOURCE_DIR}/intersection.cpp
        ${LIB_SOURCE_DIR}/linefit.cpp
        ${LIB_SOURCE_DIR}/lsd.cpp
        ${LIB_SOURCE_DIR}/main.cpp
        ${LIB_SOURCE_DIR}/matchcontours.cpp
        ${LIB_SOURCE_DIR}/median_blur.dispatch.cpp
        ${LIB_SOURCE_DIR}/median_blur.simd.hpp
        ${LIB_SOURCE_DIR}/min_enclosing_convex_polygon.cpp
        ${LIB_SOURCE_DIR}/min_enclosing_triangle.cpp
        ${LIB_SOURCE_DIR}/moments.cpp
        ${LIB_SOURCE_DIR}/morph.dispatch.cpp
        ${LIB_SOURCE_DIR}/morph.simd.hpp

        ${LIB_SOURCE_DIR}/opencl/accumulate.cl
        ${LIB_SOURCE_DIR}/opencl/bilateral.cl
        ${LIB_SOURCE_DIR}/opencl/blend_linear.cl
        ${LIB_SOURCE_DIR}/opencl/boxFilter.cl
        ${LIB_SOURCE_DIR}/opencl/boxFilter3x3.cl
        ${LIB_SOURCE_DIR}/opencl/calc_back_project.cl
        ${LIB_SOURCE_DIR}/opencl/canny.cl
        ${LIB_SOURCE_DIR}/opencl/clahe.cl
        ${LIB_SOURCE_DIR}/opencl/color_hsv.cl
        ${LIB_SOURCE_DIR}/opencl/color_lab.cl
        ${LIB_SOURCE_DIR}/opencl/color_rgb.cl
        ${LIB_SOURCE_DIR}/opencl/color_yuv.cl
        ${LIB_SOURCE_DIR}/opencl/corner.cl
        ${LIB_SOURCE_DIR}/opencl/covardata.cl
        ${LIB_SOURCE_DIR}/opencl/filter2D.cl
        ${LIB_SOURCE_DIR}/opencl/filter2DSmall.cl
        ${LIB_SOURCE_DIR}/opencl/filterSepCol.cl
        ${LIB_SOURCE_DIR}/opencl/filterSepRow.cl
        ${LIB_SOURCE_DIR}/opencl/filterSep_singlePass.cl
        ${LIB_SOURCE_DIR}/opencl/filterSmall.cl
        ${LIB_SOURCE_DIR}/opencl/gaussianBlur3x3.cl
        ${LIB_SOURCE_DIR}/opencl/gaussianBlur5x5.cl
        ${LIB_SOURCE_DIR}/opencl/gftt.cl
        ${LIB_SOURCE_DIR}/opencl/histogram.cl
        ${LIB_SOURCE_DIR}/opencl/hough_lines.cl
        ${LIB_SOURCE_DIR}/opencl/integral_sum.cl
        ${LIB_SOURCE_DIR}/opencl/laplacian3.cl
        ${LIB_SOURCE_DIR}/opencl/laplacian5.cl
        ${LIB_SOURCE_DIR}/opencl/linearPolar.cl
        ${LIB_SOURCE_DIR}/opencl/logPolar.cl
        ${LIB_SOURCE_DIR}/opencl/match_template.cl
        ${LIB_SOURCE_DIR}/opencl/medianFilter.cl
        ${LIB_SOURCE_DIR}/opencl/moments.cl
        ${LIB_SOURCE_DIR}/opencl/morph.cl
        ${LIB_SOURCE_DIR}/opencl/morph3x3.cl
        ${LIB_SOURCE_DIR}/opencl/precornerdetect.cl
        ${LIB_SOURCE_DIR}/opencl/pyramid_up.cl
        ${LIB_SOURCE_DIR}/opencl/pyr_down.cl
        ${LIB_SOURCE_DIR}/opencl/pyr_up.cl
        ${LIB_SOURCE_DIR}/opencl/remap.cl
        ${LIB_SOURCE_DIR}/opencl/resize.cl
        ${LIB_SOURCE_DIR}/opencl/sepFilter3x3.cl
        ${LIB_SOURCE_DIR}/opencl/threshold.cl
        ${LIB_SOURCE_DIR}/opencl/threshold_mask.cl
        ${LIB_SOURCE_DIR}/opencl/warp_affine.cl
        ${LIB_SOURCE_DIR}/opencl/warp_perspective.cl
        ${LIB_SOURCE_DIR}/opencl/warp_transform.cl
        ${LIB_SOURCE_DIR}/phasecorr.cpp
        ${LIB_SOURCE_DIR}/phasecorr_iterative.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/pyramids.cpp
        # ${LIB_SOURCE_DIR}/resize.avx2.cpp
        ${LIB_SOURCE_DIR}/resize.cpp
        ${LIB_SOURCE_DIR}/resize.hpp
        # ${LIB_SOURCE_DIR}/resize.lasx.cpp
        # ${LIB_SOURCE_DIR}/resize.sse4_1.cpp
        ${LIB_SOURCE_DIR}/rotcalipers.cpp
        ${LIB_SOURCE_DIR}/samplers.cpp
        ${LIB_SOURCE_DIR}/segmentation.cpp
        ${LIB_SOURCE_DIR}/shapedescr.cpp
        ${LIB_SOURCE_DIR}/smooth.dispatch.cpp
        ${LIB_SOURCE_DIR}/smooth.simd.hpp
        ${LIB_SOURCE_DIR}/spatialgradient.cpp
        ${LIB_SOURCE_DIR}/stackblur.cpp
        ${LIB_SOURCE_DIR}/subdivision2d.cpp
        ${LIB_SOURCE_DIR}/sumpixels.avx512_skx.hpp
        ${LIB_SOURCE_DIR}/sumpixels.dispatch.cpp
        ${LIB_SOURCE_DIR}/sumpixels.simd.hpp
        ${LIB_SOURCE_DIR}/tables.cpp
        ${LIB_SOURCE_DIR}/templmatch.cpp
        ${LIB_SOURCE_DIR}/thresh.cpp
        ${LIB_SOURCE_DIR}/utils.cpp
        ${LIB_SOURCE_DIR}/_geom.h
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

endif()

# add_subdirectory(test)
