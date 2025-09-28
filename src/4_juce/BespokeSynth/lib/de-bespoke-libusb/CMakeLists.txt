set(LIB_NAME de_bespoke_libusb)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/libusb/libusb.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/libusb/config.h
        ${LIB_SOURCE_DIR}/libusb/config_msvc.h
        ${LIB_SOURCE_DIR}/libusb/config_xcode.h
        ${LIB_SOURCE_DIR}/libusb/core.c
        ${LIB_SOURCE_DIR}/libusb/descriptor.c
        ${LIB_SOURCE_DIR}/libusb/hotplug.c
        ${LIB_SOURCE_DIR}/libusb/hotplug.h
        ${LIB_SOURCE_DIR}/libusb/io.c
        # ${LIB_SOURCE_DIR}/libusb/libusb-1.0.def
        # ${LIB_SOURCE_DIR}/libusb/libusb-1.0.rc
        ${LIB_SOURCE_DIR}/libusb/libusbi.h
        # ${LIB_SOURCE_DIR}/libusb/Makefile.am
        ${LIB_SOURCE_DIR}/libusb/strerror.c
        ${LIB_SOURCE_DIR}/libusb/sync.c
        ${LIB_SOURCE_DIR}/libusb/version.h
        ${LIB_SOURCE_DIR}/libusb/version_nano.h
    )

    if (WIN32)
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/libusb/os/poll_windows.c
            ${LIB_SOURCE_DIR}/libusb/os/poll_windows.h
            ${LIB_SOURCE_DIR}/libusb/os/threads_windows.c
            ${LIB_SOURCE_DIR}/libusb/os/threads_windows.h
            ${LIB_SOURCE_DIR}/libusb/os/windows_common.h
            ${LIB_SOURCE_DIR}/libusb/os/windows_nt_common.c
            ${LIB_SOURCE_DIR}/libusb/os/windows_nt_common.h
            ${LIB_SOURCE_DIR}/libusb/os/windows_usbdk.c
            ${LIB_SOURCE_DIR}/libusb/os/windows_usbdk.h
            ${LIB_SOURCE_DIR}/libusb/os/windows_winusb.c
            ${LIB_SOURCE_DIR}/libusb/os/windows_winusb.h
        )
    else()
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/libusb/os/darwin_usb.c
            ${LIB_SOURCE_DIR}/libusb/os/darwin_usb.h
            ${LIB_SOURCE_DIR}/libusb/os/haiku_pollfs.cpp
            ${LIB_SOURCE_DIR}/libusb/os/haiku_usb.h
            ${LIB_SOURCE_DIR}/libusb/os/haiku_usb_backend.cpp
            ${LIB_SOURCE_DIR}/libusb/os/haiku_usb_raw.cpp
            ${LIB_SOURCE_DIR}/libusb/os/haiku_usb_raw.h
            ${LIB_SOURCE_DIR}/libusb/os/linux_netlink.c
            ${LIB_SOURCE_DIR}/libusb/os/linux_udev.c
            ${LIB_SOURCE_DIR}/libusb/os/linux_usbfs.c
            ${LIB_SOURCE_DIR}/libusb/os/linux_usbfs.h

            ${LIB_SOURCE_DIR}/libusb/os/netbsd_usb.c
            ${LIB_SOURCE_DIR}/libusb/os/openbsd_usb.c
            ${LIB_SOURCE_DIR}/libusb/os/poll_posix.c
            ${LIB_SOURCE_DIR}/libusb/os/poll_posix.h

            ${LIB_SOURCE_DIR}/libusb/os/sunos_usb.c
            ${LIB_SOURCE_DIR}/libusb/os/sunos_usb.h
            ${LIB_SOURCE_DIR}/libusb/os/threads_posix.c
            ${LIB_SOURCE_DIR}/libusb/os/threads_posix.h

            ${LIB_SOURCE_DIR}/libusb/os/wince_usb.c
            ${LIB_SOURCE_DIR}/libusb/os/wince_usb.h
        )
    endif()

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libusb)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/libusb)

    if (WIN32)
            target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_STRUCT_TIMESPEC=1)
            target_compile_definitions(${PROJECT_NAME} PRIVATE THREADS_WINDOWS=1)
    endif()

    # target_link_libraries(${LIB_NAME} PUBLIC de_boost_asio_standalone)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _bespoke)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
