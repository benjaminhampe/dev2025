set(LIB_NAME de_sfml_network_310)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SFML/Network.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Dns.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Export.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Ftp.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Http.hpp
        ${LIB_HEADER_DIR}/SFML/Network/IpAddress.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Packet.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Sftp.hpp
        ${LIB_HEADER_DIR}/SFML/Network/Socket.hpp
        ${LIB_HEADER_DIR}/SFML/Network/SocketHandle.hpp
        ${LIB_HEADER_DIR}/SFML/Network/SocketSelector.hpp
        ${LIB_HEADER_DIR}/SFML/Network/TcpListener.hpp
        ${LIB_HEADER_DIR}/SFML/Network/TcpSocket.hpp
        ${LIB_HEADER_DIR}/SFML/Network/UdpSocket.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/SFML/Network/Dns.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/Ftp.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/Http.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/IpAddress.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/Packet.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/Sftp.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/Socket.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/SocketImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Network/SocketSelector.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/TcpListener.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/TcpSocket.cpp
        ${LIB_SOURCE_DIR}/SFML/Network/UdpSocket.cpp
    )

    if (WIN32)
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/SFML/Network/Win32/SocketImpl.cpp)
    else()
        list(APPEND LIB_SOURCES
            ${LIB_SOURCE_DIR}/SFML/Network/Unix/SocketImpl.cpp)
    endif()

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SFML_STATIC=1)
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SFML_NETWORK_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)

    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_system_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_window_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_graphics_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_audio_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_extlibs)

    target_link_libraries(${LIB_NAME} PUBLIC de_ssh2)

    # if (WIN32)
        # target_link_libraries(${LIB_NAME} PUBLIC dnsapi)
        # target_link_libraries(${LIB_NAME} PUBLIC msvcrt) # vsnprintf_s
        # # target_link_libraries(${LIB_NAME} PUBLIC ucrt) # vsnprintf_s
    # endif()

    # ld.lld: error: undefined symbol: vsnprintf_s
    # >>> referenced by libmbedcrypto.a(platform.c.obj):(mbedtls_platform_win32_vsnprintf)

    # SFML/Network/TcpSocket.cpp:340:2: error: "No Mbed TLS threading implementation available"
    # #error "No Mbed TLS threading implementation available"

    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_static)

    # target_link_libraries(${LIB_NAME} PUBLIC de_miniaudio)
    # target_link_libraries(${LIB_NAME} PUBLIC de_flac)
    # target_link_libraries(${LIB_NAME} PUBLIC de_ogg)
    # target_link_libraries(${LIB_NAME} PUBLIC de_vorbis)

endif()

add_subdirectory(test)
