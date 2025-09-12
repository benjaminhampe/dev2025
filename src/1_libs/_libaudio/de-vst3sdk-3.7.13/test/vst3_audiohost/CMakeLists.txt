set(LIB_NAME vst3_audiohost)

project(${LIB_NAME}
        VERSION 3.73.0
        DESCRIPTION "Steinberg VST 3 Sync Delay example")

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    # set(audiohost_sources
        # "${SDK_ROOT}/public.sdk/samples/vst-hosting/editorhost/source/platform/iapplication.h"
        # "${SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.cpp"
        # "${SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.h"
        # source/audiohost.cpp
        # source/audiohost.h
        # source/media/audioclient.cpp
        # source/media/audioclient.h
        # source/media/imediaserver.h
        # source/media/iparameterclient.h
        # source/media/jack/jackclient.cpp
        # source/media/miditovst.h
        # source/platform/appinit.h
        # source/usediids.cpp
    # )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/plugprovider.cpp
        ${LIB_SOURCE_DIR}/audiohost.cpp
        ${LIB_SOURCE_DIR}/audiohost.h
        # ${LIB_SOURCE_DIR}/editorhost.cpp
        # ${LIB_SOURCE_DIR}/editorhost.h

        ${LIB_SOURCE_DIR}/media/audioclient.cpp
        ${LIB_SOURCE_DIR}/media/audioclient.h
        ${LIB_SOURCE_DIR}/media/imediaserver.h
        ${LIB_SOURCE_DIR}/media/iparameterclient.h

        ${LIB_SOURCE_DIR}/media/jack/jackclient.cpp
        ${LIB_SOURCE_DIR}/media/miditovst.h

        ${LIB_SOURCE_DIR}/platform/appinit.h
        ${LIB_SOURCE_DIR}/platform/iapplication.h
        ${LIB_SOURCE_DIR}/platform/iplatform.h
        ${LIB_SOURCE_DIR}/platform/iwindow.h

        # ${LIB_SOURCE_DIR}/platform/linux/irunloopimpl.h
        # ${LIB_SOURCE_DIR}/platform/linux/platform.cpp
        # ${LIB_SOURCE_DIR}/platform/linux/runloop.cpp
        # ${LIB_SOURCE_DIR}/platform/linux/runloop.h
        # ${LIB_SOURCE_DIR}/platform/linux/window.cpp
        # ${LIB_SOURCE_DIR}/platform/linux/window.h

        # ${LIB_SOURCE_DIR}/platform/mac/platform.mm
        # ${LIB_SOURCE_DIR}/platform/mac/window.h
        # ${LIB_SOURCE_DIR}/platform/mac/window.mm

        ${LIB_SOURCE_DIR}/platform/win32/platform.cpp
        ${LIB_SOURCE_DIR}/platform/win32/window.cpp
        ${LIB_SOURCE_DIR}/platform/win32/window.h
        ${LIB_SOURCE_DIR}/usediids.cpp

        # ${LIB_SOURCE_DIR}/audiohost.cpp
        # ${LIB_SOURCE_DIR}/audiohost.h
        # ${LIB_SOURCE_DIR}/usediids.cpp

        # ${LIB_SOURCE_DIR}/media/audioclient.cpp
        # ${LIB_SOURCE_DIR}/media/audioclient.h
        # ${LIB_SOURCE_DIR}/media/imediaserver.h
        # ${LIB_SOURCE_DIR}/media/iparameterclient.h

        # # ${LIB_SOURCE_DIR}/media/jack/jackclient.cpp
        # ${LIB_SOURCE_DIR}/media/miditovst.h

        # ${LIB_SOURCE_DIR}/platform/appinit.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${LIB_NAME}${ColourReset}")
    add_executable(${LIB_NAME} "")

    target_sources(${LIB_NAME} PRIVATE ${LIB_SOURCES})
	
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_link_options(${LIB_NAME} PRIVATE "-Wl,-subsystem,console")
	
    target_link_libraries(${LIB_NAME} PRIVATE de_vst3sdk_hosting)
    target_link_libraries(${LIB_NAME} PRIVATE de_vstgui4_standalone)
    target_link_libraries(${LIB_NAME} PRIVATE de_jack2)

endif()
