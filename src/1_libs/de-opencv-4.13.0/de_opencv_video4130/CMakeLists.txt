set(LIB_NAME de_opencv_video4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/video/background_segm.hpp

        ${LIB_HEADER_DIR}/opencv2/video/detail/tracking.detail.hpp

        ${LIB_HEADER_DIR}/opencv2/video/legacy/constants_c.h
        ${LIB_HEADER_DIR}/opencv2/video/tracking.hpp
        ${LIB_HEADER_DIR}/opencv2/video/video.hpp
        ${LIB_HEADER_DIR}/opencv2/video.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_video.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_video.hpp

        ${LIB_SOURCE_DIR}/bgfg_gaussmix2.cpp
        ${LIB_SOURCE_DIR}/bgfg_KNN.cpp
        ${LIB_SOURCE_DIR}/camshift.cpp
        ${LIB_SOURCE_DIR}/dis_flow.cpp
        ${LIB_SOURCE_DIR}/ecc.cpp
        ${LIB_SOURCE_DIR}/hal_replacement.hpp
        ${LIB_SOURCE_DIR}/kalman.cpp
        ${LIB_SOURCE_DIR}/lkpyramid.cpp
        ${LIB_SOURCE_DIR}/lkpyramid.hpp

        ${LIB_SOURCE_DIR}/opencl/bgfg_knn.cl
        ${LIB_SOURCE_DIR}/opencl/bgfg_mog2.cl
        ${LIB_SOURCE_DIR}/opencl/dis_flow.cl
        ${LIB_SOURCE_DIR}/opencl/optical_flow_farneback.cl
        ${LIB_SOURCE_DIR}/opencl/pyrlk.cl

        ${LIB_SOURCE_DIR}/optflowgf.cpp
        ${LIB_SOURCE_DIR}/optical_flow_io.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp

        ${LIB_SOURCE_DIR}/tracking/detail/tracker_feature.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_feature_haar.impl.hpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_feature_set.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_mil_model.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_mil_model.hpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_mil_state.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_mil_state.hpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_model.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_sampler.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_sampler_algorithm.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracker_state_estimator.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracking_feature.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracking_feature.hpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracking_online_mil.cpp
        ${LIB_SOURCE_DIR}/tracking/detail/tracking_online_mil.hpp
        ${LIB_SOURCE_DIR}/tracking/tracker.cpp
        ${LIB_SOURCE_DIR}/tracking/tracker_dasiamrpn.cpp
        ${LIB_SOURCE_DIR}/tracking/tracker_goturn.cpp
        ${LIB_SOURCE_DIR}/tracking/tracker_mil.cpp
        ${LIB_SOURCE_DIR}/tracking/tracker_nano.cpp
        ${LIB_SOURCE_DIR}/tracking/tracker_vit.cpp
        ${LIB_SOURCE_DIR}/variational_refinement.cpp
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
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_dnn4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_calib3d4130)

endif()

# add_subdirectory(test)
