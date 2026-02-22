set(LIB_NAME de_opencv_videoio4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/videoio/cap_ios.h
        ${LIB_HEADER_DIR}/opencv2/videoio/cap_winrt.hpp
        ${LIB_HEADER_DIR}/opencv2/videoio/container_avi.private.hpp

        ${LIB_HEADER_DIR}/opencv2/videoio/doc/hwaccel.doc.hpp

        ${LIB_HEADER_DIR}/opencv2/videoio/legacy/constants_c.h
        ${LIB_HEADER_DIR}/opencv2/videoio/registry.hpp
        ${LIB_HEADER_DIR}/opencv2/videoio/utils.private.hpp
        ${LIB_HEADER_DIR}/opencv2/videoio/videoio.hpp
        ${LIB_HEADER_DIR}/opencv2/videoio/videoio_c.h
        ${LIB_HEADER_DIR}/opencv2/videoio.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/backend.hpp
        ${LIB_SOURCE_DIR}/backend_plugin.cpp
        ${LIB_SOURCE_DIR}/backend_plugin_legacy.impl.hpp
        ${LIB_SOURCE_DIR}/backend_static.cpp
        ${LIB_SOURCE_DIR}/cap.cpp
        # ${LIB_SOURCE_DIR}/cap_android_camera.cpp
        # ${LIB_SOURCE_DIR}/cap_android_mediandk.cpp
        ${LIB_SOURCE_DIR}/cap_aravis.cpp
        # ${LIB_SOURCE_DIR}/cap_avfoundation.mm
        # ${LIB_SOURCE_DIR}/cap_avfoundation_mac.mm
        ${LIB_SOURCE_DIR}/cap_dc1394_v2.cpp
        ${LIB_SOURCE_DIR}/cap_dshow.cpp
        ${LIB_SOURCE_DIR}/cap_dshow.hpp

        # ${LIB_SOURCE_DIR}/cap_ffmpeg.cpp
        # ${LIB_SOURCE_DIR}/cap_ffmpeg_hw.hpp
        # ${LIB_SOURCE_DIR}/cap_ffmpeg_impl.hpp
        # ${LIB_SOURCE_DIR}/cap_ffmpeg_legacy_api.hpp

        ${LIB_SOURCE_DIR}/cap_gphoto2.cpp

        # ${LIB_SOURCE_DIR}/cap_gstreamer.cpp

        ${LIB_SOURCE_DIR}/cap_images.cpp
        ${LIB_SOURCE_DIR}/cap_interface.hpp
        # ${LIB_SOURCE_DIR}/cap_ios_abstract_camera.mm
        # ${LIB_SOURCE_DIR}/cap_ios_photo_camera.mm
        # ${LIB_SOURCE_DIR}/cap_ios_video_camera.mm
        ${LIB_SOURCE_DIR}/cap_librealsense.cpp
        ${LIB_SOURCE_DIR}/cap_librealsense.hpp

        # ${LIB_SOURCE_DIR}/cap_mfx_common.cpp
        # ${LIB_SOURCE_DIR}/cap_mfx_common.hpp
        # ${LIB_SOURCE_DIR}/cap_mfx_plugin.cpp
        # ${LIB_SOURCE_DIR}/cap_mfx_reader.cpp
        # ${LIB_SOURCE_DIR}/cap_mfx_reader.hpp
        # ${LIB_SOURCE_DIR}/cap_mfx_writer.cpp
        # ${LIB_SOURCE_DIR}/cap_mfx_writer.hpp

        ${LIB_SOURCE_DIR}/cap_mjpeg_decoder.cpp
        ${LIB_SOURCE_DIR}/cap_mjpeg_encoder.cpp

        # ${LIB_SOURCE_DIR}/cap_msmf.cpp # swhlapi.h shows error!
        # ${LIB_SOURCE_DIR}/cap_msmf.hpp

        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_stream_channel_interface.hpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_stream_channel_msmf.cpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_stream_channel_msmf.hpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_stream_channel_v4l2.cpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_stream_channel_v4l2.hpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_uvc_stream_channel.cpp
        ${LIB_SOURCE_DIR}/cap_obsensor/obsensor_uvc_stream_channel.hpp
        ${LIB_SOURCE_DIR}/cap_obsensor_capture.cpp
        ${LIB_SOURCE_DIR}/cap_obsensor_capture.hpp
        ${LIB_SOURCE_DIR}/cap_obsensor_liborbbec.cpp
        ${LIB_SOURCE_DIR}/cap_obsensor_liborbbec.hpp
        ${LIB_SOURCE_DIR}/cap_openni2.cpp
        ${LIB_SOURCE_DIR}/cap_pvapi.cpp
        # ${LIB_SOURCE_DIR}/cap_ueye.cpp
        ${LIB_SOURCE_DIR}/cap_v4l.cpp

        # ${LIB_SOURCE_DIR}/cap_winrt/CaptureFrameGrabber.cpp
        # ${LIB_SOURCE_DIR}/cap_winrt/CaptureFrameGrabber.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt/MediaSink.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt/MediaStreamSink.cpp
        # ${LIB_SOURCE_DIR}/cap_winrt/MediaStreamSink.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt/MFIncludes.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt_bridge.cpp
        # ${LIB_SOURCE_DIR}/cap_winrt_bridge.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt_capture.cpp
        # ${LIB_SOURCE_DIR}/cap_winrt_capture.hpp
        # ${LIB_SOURCE_DIR}/cap_winrt_video.cpp
        # ${LIB_SOURCE_DIR}/cap_winrt_video.hpp

        # ${LIB_SOURCE_DIR}/cap_ximea.cpp
        # ${LIB_SOURCE_DIR}/cap_xine.cpp
        ${LIB_SOURCE_DIR}/container_avi.cpp
        ${LIB_SOURCE_DIR}/ffmpeg_codecs.hpp
        ${LIB_SOURCE_DIR}/plugin_api.hpp
        ${LIB_SOURCE_DIR}/plugin_capture_api.hpp
        ${LIB_SOURCE_DIR}/plugin_writer_api.hpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/videoio_c.cpp
        ${LIB_SOURCE_DIR}/videoio_registry.cpp
        ${LIB_SOURCE_DIR}/videoio_registry.hpp
        ${LIB_SOURCE_DIR}/wrl.h
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

    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)

    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_DSHOW=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_MSMF=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_MSMF_DXVA=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE WINVER=0x0602) # Win8+
    # target_compile_definitions(${LIB_NAME} PRIVATE _WIN32_WINNT=0x0602) # Win8+


    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgcodecs4130)

endif()

# add_subdirectory(test)
