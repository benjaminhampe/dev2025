set(LIB_NAME de_84_oscpack)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/oscpack/ip/IpEndpointName.h
        ${LIB_HEADER_DIR}/oscpack/ip/NetworkingUtils.h
        ${LIB_HEADER_DIR}/oscpack/ip/PacketListener.h
        ${LIB_HEADER_DIR}/oscpack/ip/TimerListener.h
        ${LIB_HEADER_DIR}/oscpack/ip/UdpSocket.h
        ${LIB_HEADER_DIR}/oscpack/osc/MessageMappingOscPacketListener.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscException.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscHostEndianness.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscOutboundPacketStream.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscPacketListener.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscPrintReceivedElements.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscReceivedElements.h
        ${LIB_HEADER_DIR}/oscpack/osc/OscTypes.h
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/ip/IpEndpointName.cpp
        ${LIB_SOURCE_DIR}/osc/OscOutboundPacketStream.cpp
        ${LIB_SOURCE_DIR}/osc/OscPrintReceivedElements.cpp
        ${LIB_SOURCE_DIR}/osc/OscReceivedElements.cpp
        ${LIB_SOURCE_DIR}/osc/OscTypes.cpp
    )

    if (WIN32)
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/ip/win32/NetworkingUtils.cpp
            ${LIB_SOURCE_DIR}/ip/win32/UdpSocket.cpp
        )
    else()
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/ip/posix/NetworkingUtils.cpp
            ${LIB_SOURCE_DIR}/ip/posix/UdpSocket.cpp
        )
    endif()

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    # target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        if (WIN32)
            target_link_libraries(${LIB_NAME} PRIVATE ws2_32)
            target_link_libraries(${LIB_NAME} PRIVATE winmm)
        endif()
    endif()

endif()

# add_subdirectory(test)
