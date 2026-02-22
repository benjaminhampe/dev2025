set(LIB_NAME de_opencv_objdetect4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/objdetect/aruco_board.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/aruco_detector.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/aruco_dictionary.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/barcode.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/charuco_detector.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/detection_based_tracker.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/face.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/graphical_code_detector.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect/objdetect.hpp
        ${LIB_HEADER_DIR}/opencv2/objdetect.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_objdetect.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_objdetect.hpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/apriltag_quad_thresh.cpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/apriltag_quad_thresh.hpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/predefined_dictionaries_apriltag.hpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/unionfind.hpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/zarray.hpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/zmaxheap.cpp
        ${LIB_SOURCE_DIR}/aruco/apriltag/zmaxheap.hpp
        ${LIB_SOURCE_DIR}/aruco/aruco_board.cpp
        ${LIB_SOURCE_DIR}/aruco/aruco_detector.cpp
        ${LIB_SOURCE_DIR}/aruco/aruco_dictionary.cpp
        ${LIB_SOURCE_DIR}/aruco/aruco_utils.cpp
        ${LIB_SOURCE_DIR}/aruco/aruco_utils.hpp
        ${LIB_SOURCE_DIR}/aruco/charuco_detector.cpp
        ${LIB_SOURCE_DIR}/aruco/predefined_dictionaries.hpp
        ${LIB_SOURCE_DIR}/barcode.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/abs_decoder.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/abs_decoder.hpp

        ${LIB_SOURCE_DIR}/barcode_decoder/common/hybrid_binarizer.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/common/hybrid_binarizer.hpp
        ${LIB_SOURCE_DIR}/barcode_decoder/common/super_scale.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/common/super_scale.hpp
        ${LIB_SOURCE_DIR}/barcode_decoder/common/utils.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/common/utils.hpp
        ${LIB_SOURCE_DIR}/barcode_decoder/ean13_decoder.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/ean13_decoder.hpp
        ${LIB_SOURCE_DIR}/barcode_decoder/ean8_decoder.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/ean8_decoder.hpp
        ${LIB_SOURCE_DIR}/barcode_decoder/upcean_decoder.cpp
        ${LIB_SOURCE_DIR}/barcode_decoder/upcean_decoder.hpp

        ${LIB_SOURCE_DIR}/barcode_detector/bardetect.cpp
        ${LIB_SOURCE_DIR}/barcode_detector/bardetect.hpp
        ${LIB_SOURCE_DIR}/cascadedetect.cpp
        ${LIB_SOURCE_DIR}/cascadedetect.hpp
        ${LIB_SOURCE_DIR}/cascadedetect_convert.cpp
        ${LIB_SOURCE_DIR}/detection_based_tracker.cpp
        ${LIB_SOURCE_DIR}/face_detect.cpp
        ${LIB_SOURCE_DIR}/face_recognize.cpp
        ${LIB_SOURCE_DIR}/graphical_code_detector.cpp
        ${LIB_SOURCE_DIR}/graphical_code_detector_impl.hpp
        ${LIB_SOURCE_DIR}/hog.cpp
        ${LIB_SOURCE_DIR}/main.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/qrcode.cpp
        ${LIB_SOURCE_DIR}/qrcode_encoder.cpp
        ${LIB_SOURCE_DIR}/qrcode_encoder_table.inl.hpp

        ${LIB_SOURCE_DIR}/opencl/cascadedetect.cl
        ${LIB_SOURCE_DIR}/opencl/objdetect_hog.cl
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
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_calib3d4130)	
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_dnn4130)

endif()

# add_subdirectory(test)
