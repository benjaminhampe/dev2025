set(LIB_NAME de_ssh2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/libssh2.h
        ${LIB_HEADER_DIR}/libssh2_publickey.h
        ${LIB_HEADER_DIR}/libssh2_sftp.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/agent.c
        ${LIB_SOURCE_DIR}/agent_win.c
        ${LIB_SOURCE_DIR}/bcrypt_pbkdf.c
        ${LIB_SOURCE_DIR}/blowfish.c
        ${LIB_SOURCE_DIR}/chacha.c
        ${LIB_SOURCE_DIR}/chacha.h
        ${LIB_SOURCE_DIR}/channel.c
        ${LIB_SOURCE_DIR}/channel.h
        ${LIB_SOURCE_DIR}/cipher-chachapoly.c
        ${LIB_SOURCE_DIR}/cipher-chachapoly.h
        # ${LIB_SOURCE_DIR}/CMakeLists.txt
        ${LIB_SOURCE_DIR}/comp.c
        ${LIB_SOURCE_DIR}/comp.h
        # ${LIB_SOURCE_DIR}/config/libssh2.pc
        ${LIB_SOURCE_DIR}/config/libssh2_config.h
        ${LIB_SOURCE_DIR}/crypt.c
        ${LIB_SOURCE_DIR}/crypto.c
        ${LIB_SOURCE_DIR}/crypto.h
        ${LIB_SOURCE_DIR}/crypto_config.h
        ${LIB_SOURCE_DIR}/global.c
        ${LIB_SOURCE_DIR}/hostkey.c
        ${LIB_SOURCE_DIR}/keepalive.c
        ${LIB_SOURCE_DIR}/kex.c
        ${LIB_SOURCE_DIR}/knownhost.c
        ${LIB_SOURCE_DIR}/libgcrypt.c
        ${LIB_SOURCE_DIR}/libgcrypt.h
        # ${LIB_SOURCE_DIR}/libssh2.rc
        # ${LIB_SOURCE_DIR}/libssh2_config_cmake.h.in
        ${LIB_SOURCE_DIR}/libssh2_priv.h
        ${LIB_SOURCE_DIR}/libssh2_setup.h
        ${LIB_SOURCE_DIR}/mac.c
        ${LIB_SOURCE_DIR}/mac.h
        # ${LIB_SOURCE_DIR}/Makefile.am
        # ${LIB_SOURCE_DIR}/Makefile.inc
        ${LIB_SOURCE_DIR}/mbedtls.c
        ${LIB_SOURCE_DIR}/mbedtls.h
        ${LIB_SOURCE_DIR}/misc.c
        ${LIB_SOURCE_DIR}/misc.h
        ${LIB_SOURCE_DIR}/openssl.c
        ${LIB_SOURCE_DIR}/openssl.h
        ${LIB_SOURCE_DIR}/os400qc3.c
        ${LIB_SOURCE_DIR}/os400qc3.h
        ${LIB_SOURCE_DIR}/packet.c
        ${LIB_SOURCE_DIR}/packet.h
        ${LIB_SOURCE_DIR}/pem.c
        ${LIB_SOURCE_DIR}/poly1305.c
        ${LIB_SOURCE_DIR}/poly1305.h
        ${LIB_SOURCE_DIR}/publickey.c
        ${LIB_SOURCE_DIR}/scp.c
        ${LIB_SOURCE_DIR}/session.c
        ${LIB_SOURCE_DIR}/session.h
        ${LIB_SOURCE_DIR}/sftp.c
        ${LIB_SOURCE_DIR}/sftp.h
        ${LIB_SOURCE_DIR}/transport.c
        ${LIB_SOURCE_DIR}/transport.h
        ${LIB_SOURCE_DIR}/userauth.c
        ${LIB_SOURCE_DIR}/userauth.h
        ${LIB_SOURCE_DIR}/userauth_kbd_packet.c
        ${LIB_SOURCE_DIR}/userauth_kbd_packet.h
        ${LIB_SOURCE_DIR}/version.c
        ${LIB_SOURCE_DIR}/wincng.c
        ${LIB_SOURCE_DIR}/wincng.h
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC LIBSSH2_EXPORTS=1)

    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_LIBRARY=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2DEBUG=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_NO_DEPRECATED=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_NO_CLEAR_MEMORY=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/config)


    target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_HAVE_ZLIB=1)
    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)


    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC winmm)
        target_link_libraries(${LIB_NAME} PUBLIC ws2_32)
        target_link_libraries(${LIB_NAME} PUBLIC crypt32)
        target_link_libraries(${LIB_NAME} PUBLIC bcrypt)
    endif()

    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_static)
    # target_link_libraries(${LIB_NAME} PUBLIC de_openssl_static)

    # mbedTLS
    target_compile_definitions(${LIB_NAME} PRIVATE
        CRYPTO_BACKEND="mbedTLS"
        CRYPTO_BACKEND_DEFINE="LIBSSH2_MBEDTLS"
        CRYPTO_BACKEND_INCLUDE_DIR="")

    target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_MBEDTLS=1)
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_static)
    target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420)
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420_crypto)
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420_x509)
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420_tls)

    # OpenSSL
    # target_compile_definitions(${LIB_NAME} PRIVATE
        # CRYPTO_BACKEND="OpenSSL"
        # CRYPTO_BACKEND_DEFINE="LIBSSH2_OPENSSL"
        # CRYPTO_BACKEND_INCLUDE_DIR="")

    # target_compile_definitions(${LIB_NAME} PRIVATE LIBSSH2_OPENSSL=1)
    # target_link_libraries(${LIB_NAME} PUBLIC de_openssl_static)

    # WinCNG
    # if(WIN32)
    #     target_compile_definitions(${LIB_NAME} PRIVATE
    #         CRYPTO_BACKEND="WinCNG"
    #         CRYPTO_BACKEND_DEFINE="LIBSSH2_WINCNG"
    #         CRYPTO_BACKEND_INCLUDE_DIR=""
    #         ENABLE_ECDSA_WINCNG=1
    #         LIBSSH2_ECDSA_WINCNG=1)

    #     target_link_options(${LIB_NAME} PRIVATE -Wl,--subsystem,windows:10)
    # endif()

    if(WIN32)
        # if(MINGW)
            target_compile_definitions(${LIB_NAME} PRIVATE
                HAVE_SNPRINTF=1
                HAVE_UNISTD_H=1
                HAVE_INTTYPES_H=1
                HAVE_SYS_TIME_H=1
                HAVE_GETTIMEOFDAY=1
                HAVE_STRTOLL=1
            )
        # elseif(MSVC)
        #     set(HAVE_GETTIMEOFDAY 0)
        #     if(NOT MSVC_VERSION LESS 1800)
        #     set(HAVE_INTTYPES_H 1)
        #     set(HAVE_STRTOLL 1)
        #     else()
        #     set(HAVE_INTTYPES_H 0)
        #     set(HAVE_STRTOLL 0)
        #     set(HAVE_STRTOI64 1)
        #     endif()
        #     if(NOT MSVC_VERSION LESS 1900)
        #     set(HAVE_SNPRINTF 1)
        #     else()
        #     set(HAVE_SNPRINTF 0)
        #     endif()
        # endif()
    endif()

    # ## Platform checks
    # check_include_files("inttypes.h" HAVE_INTTYPES_H)
    # if(NOT MSVC)
    #   check_include_files("unistd.h" HAVE_UNISTD_H)
    #   check_include_files("sys/time.h" HAVE_SYS_TIME_H)
    # endif()
    # if(NOT WIN32)
    #   check_include_files("sys/select.h" HAVE_SYS_SELECT_H)
    #   check_include_files("sys/uio.h" HAVE_SYS_UIO_H)
    #   check_include_files("sys/socket.h" HAVE_SYS_SOCKET_H)
    #   check_include_files("sys/ioctl.h" HAVE_SYS_IOCTL_H)
    #   check_include_files("sys/un.h" HAVE_SYS_UN_H)
    #   check_include_files("arpa/inet.h" HAVE_ARPA_INET_H)  # example and tests
    #   check_include_files("netinet/in.h" HAVE_NETINET_IN_H)  # example and tests
    # endif()
    # if(HAVE_SYS_TIME_H)
    #     check_symbol_exists("gettimeofday" "sys/time.h" HAVE_GETTIMEOFDAY)
    # else()
    #     check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
    # endif()
    # check_symbol_exists("strtoll" "stdlib.h" HAVE_STRTOLL)
    # if(NOT HAVE_STRTOLL)
    # # Try _strtoi64() if strtoll() is not available
    # check_symbol_exists("_strtoi64" "stdlib.h" HAVE_STRTOI64)
    # endif()
    # check_symbol_exists("snprintf" "stdio.h" HAVE_SNPRINTF)
    # if(NOT WIN32)
    #     check_symbol_exists("explicit_bzero" "string.h" HAVE_EXPLICIT_BZERO)
    #     check_symbol_exists("explicit_memset" "string.h" HAVE_EXPLICIT_MEMSET)
    #     check_symbol_exists("memset_s" "string.h" HAVE_MEMSET_S)
    # endif()
    # elseif(NOT WIN32)
    #   check_function_exists("poll" HAVE_POLL)
    # endif()
    # if(WIN32)
    #   set(HAVE_SELECT 1)
    # else()
    #   check_function_exists("select" HAVE_SELECT)
    # endif()



## Cryptography backend choice

# set(CRYPTO_BACKEND "" CACHE
#   STRING "The backend to use for cryptography: OpenSSL, wolfSSL, Libgcrypt, WinCNG, mbedTLS, or empty to try any available")

# # If the crypto backend was given, rather than searching for the first
# # we are able to find, the find_package commands must abort configuration
# # and report to the user.
# if(CRYPTO_BACKEND)
#   set(_specific_crypto_requirement "REQUIRED")
# endif()

# if(CRYPTO_BACKEND STREQUAL "OpenSSL" OR NOT CRYPTO_BACKEND)

#   find_package(OpenSSL ${_specific_crypto_requirement})

#   if(OPENSSL_FOUND)
#     set(CRYPTO_BACKEND "OpenSSL")
#     set(CRYPTO_BACKEND_DEFINE "LIBSSH2_OPENSSL")
#     set(CRYPTO_BACKEND_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR})
#     list(APPEND LIBSSH2_LIBS OpenSSL::Crypto)
#     list(APPEND LIBSSH2_PC_REQUIRES_PRIVATE "libcrypto")

#     if(WIN32)
#       # Statically linking to OpenSSL requires crypt32 for some Windows APIs.
#       # This should really be handled by FindOpenSSL.cmake.
#       list(APPEND LIBSSH2_LIBS "crypt32" "bcrypt")

#       #set(CMAKE_FIND_DEBUG_MODE ON)

#       find_file(DLL_LIBCRYPTO
#         NAMES "crypto.dll"
#           "libcrypto-1_1.dll" "libcrypto-1_1-x64.dll"
#           "libcrypto-3.dll" "libcrypto-3-x64.dll"
#         HINTS ${_OPENSSL_ROOT_HINTS} PATHS ${_OPENSSL_ROOT_PATHS}
#         PATH_SUFFIXES "bin" NO_DEFAULT_PATH)
#       if(DLL_LIBCRYPTO)
#         list(APPEND _RUNTIME_DEPENDENCIES ${DLL_LIBCRYPTO})
#         message(STATUS "Found libcrypto DLL: ${DLL_LIBCRYPTO}")
#       else()
#         message(WARNING "Unable to find OpenSSL libcrypto DLL, executables may not run")
#       endif()

#       #set(CMAKE_FIND_DEBUG_MODE OFF)
#     endif()

#     find_package(ZLIB)

#     if(ZLIB_FOUND)
#       list(APPEND LIBSSH2_LIBS ${ZLIB_LIBRARIES})
#     endif()
#   endif()
# endif()

# if(CRYPTO_BACKEND STREQUAL "wolfSSL" OR NOT CRYPTO_BACKEND)

#   find_package(WolfSSL ${_specific_crypto_requirement})

#   if(WOLFSSL_FOUND)
#     set(CRYPTO_BACKEND "wolfSSL")
#     set(CRYPTO_BACKEND_DEFINE "LIBSSH2_WOLFSSL")
#     set(CRYPTO_BACKEND_INCLUDE_DIR ${WOLFSSL_INCLUDE_DIRS})
#     list(APPEND LIBSSH2_LIBS ${WOLFSSL_LIBRARIES})
#     list(APPEND LIBSSH2_LIBDIRS ${WOLFSSL_LIBRARY_DIRS})
#     list(APPEND LIBSSH2_PC_REQUIRES_PRIVATE "wolfssl")
#     link_directories(${WOLFSSL_LIBRARY_DIRS})
#     if(WOLFSSL_CFLAGS)
#       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WOLFSSL_CFLAGS}")
#     endif()

#     if(WIN32)
#       list(APPEND LIBSSH2_LIBS "crypt32")
#     endif()

#     find_package(ZLIB)

#     if(ZLIB_FOUND)
#       list(APPEND CRYPTO_BACKEND_INCLUDE_DIR ${ZLIB_INCLUDE_DIR})  # Public wolfSSL headers require zlib headers
#       list(APPEND LIBSSH2_LIBS ${ZLIB_LIBRARIES})
#     endif()
#   endif()
# endif()

# if(CRYPTO_BACKEND STREQUAL "Libgcrypt" OR NOT CRYPTO_BACKEND)

#   find_package(Libgcrypt ${_specific_crypto_requirement})

#   if(LIBGCRYPT_FOUND)
#     set(CRYPTO_BACKEND "Libgcrypt")
#     set(CRYPTO_BACKEND_DEFINE "LIBSSH2_LIBGCRYPT")
#     set(CRYPTO_BACKEND_INCLUDE_DIR ${LIBGCRYPT_INCLUDE_DIRS})
#     list(APPEND LIBSSH2_LIBS ${LIBGCRYPT_LIBRARIES})
#     list(APPEND LIBSSH2_LIBDIRS ${LIBGCRYPT_LIBRARY_DIRS})
#     list(APPEND LIBSSH2_PC_REQUIRES_PRIVATE "libgcrypt")
#     link_directories(${LIBGCRYPT_LIBRARY_DIRS})
#     if(LIBGCRYPT_CFLAGS)
#       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBGCRYPT_CFLAGS}")
#     endif()
#   endif()
# endif()

# if(CRYPTO_BACKEND STREQUAL "mbedTLS" OR NOT CRYPTO_BACKEND)

#   find_package(MbedTLS ${_specific_crypto_requirement})

#   if(MBEDTLS_FOUND)
#     set(CRYPTO_BACKEND "mbedTLS")
#     set(CRYPTO_BACKEND_DEFINE "LIBSSH2_MBEDTLS")
#     set(CRYPTO_BACKEND_INCLUDE_DIR ${MBEDTLS_INCLUDE_DIRS})
#     list(APPEND LIBSSH2_LIBS ${MBEDTLS_LIBRARIES})
#     list(APPEND LIBSSH2_LIBDIRS ${MBEDTLS_LIBRARY_DIRS})
#     list(APPEND LIBSSH2_PC_REQUIRES_PRIVATE "mbedcrypto")
#     link_directories(${MBEDTLS_LIBRARY_DIRS})
#     if(MBEDTLS_CFLAGS)
#       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MBEDTLS_CFLAGS}")
#     endif()
#   endif()
# endif()

# # Detect platform-specific crypto-backends last:

# if(CRYPTO_BACKEND STREQUAL "WinCNG" OR NOT CRYPTO_BACKEND)
#   if(WIN32)
#     set(CRYPTO_BACKEND "WinCNG")
#     set(CRYPTO_BACKEND_DEFINE "LIBSSH2_WINCNG")
#     set(CRYPTO_BACKEND_INCLUDE_DIR "")
#     list(APPEND LIBSSH2_LIBS "crypt32" "bcrypt")

#     option(ENABLE_ECDSA_WINCNG "Enable WinCNG ECDSA support (requires Windows 10 or later)" OFF)
#     add_feature_info(WinCNG ENABLE_ECDSA_WINCNG "WinCNG ECDSA support")
#     if(ENABLE_ECDSA_WINCNG)
#       add_definitions("-DLIBSSH2_ECDSA_WINCNG")
#       if(MSVC)
#         set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,10")
#       elseif(MINGW)
#         set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--subsystem,windows:10")
#       endif()
#     endif()
#   elseif(_specific_crypto_requirement STREQUAL "REQUIRED")
#     message(FATAL_ERROR "WinCNG not available")
#   endif()
# endif()

# # Global functions

# # Convert GNU Make assignments into CMake ones.
# function(transform_makefile_inc _input_file _output_file)
#   file(READ ${_input_file} _makefile_inc_cmake)

#   string(REGEX REPLACE "\\\\\n" "" _makefile_inc_cmake ${_makefile_inc_cmake})
#   string(REGEX REPLACE "([A-Za-z_]+) *= *([^\n]*)" "set(\\1 \\2)" _makefile_inc_cmake ${_makefile_inc_cmake})

#   file(WRITE ${_output_file} ${_makefile_inc_cmake})
#   set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_input_file}")
# endfunction()

# #


    # install(TARGETS enet
        # RUNTIME DESTINATION bin
        # ARCHIVE DESTINATION lib/static
        # LIBRARY DESTINATION lib)

    # install(DIRECTORY include/
            # DESTINATION include)

endif()
