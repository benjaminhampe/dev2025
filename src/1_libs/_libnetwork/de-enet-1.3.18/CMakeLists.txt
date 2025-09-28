set(LIB_NAME de_enet)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    # The "configure" step.
    include(CheckFunctionExists)
    include(CheckStructHasMember)
    include(CheckTypeSize)
    check_function_exists("fcntl" HAS_FCNTL)
    check_function_exists("poll" HAS_POLL)
    check_function_exists("getaddrinfo" HAS_GETADDRINFO)
    check_function_exists("getnameinfo" HAS_GETNAMEINFO)
    check_function_exists("gethostbyname_r" HAS_GETHOSTBYNAME_R)
    check_function_exists("gethostbyaddr_r" HAS_GETHOSTBYADDR_R)
    check_function_exists("inet_pton" HAS_INET_PTON)
    check_function_exists("inet_ntop" HAS_INET_NTOP)
    check_struct_has_member("struct msghdr" "msg_flags" "sys/types.h;sys/socket.h" HAS_MSGHDR_FLAGS)
    set(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h" "sys/socket.h")
    check_type_size("socklen_t" HAS_SOCKLEN_T BUILTIN_TYPES_ONLY)
    unset(CMAKE_EXTRA_INCLUDE_FILES)
    if(MSVC)
        add_definitions(-W3)
    else()
        add_definitions(-Wno-error)
    endif()

    if(HAS_FCNTL)
        add_definitions(-DHAS_FCNTL=1)
    endif()
    if(HAS_POLL)
        add_definitions(-DHAS_POLL=1)
    endif()
    if(HAS_GETNAMEINFO)
        add_definitions(-DHAS_GETNAMEINFO=1)
    endif()
    if(HAS_GETADDRINFO)
        add_definitions(-DHAS_GETADDRINFO=1)
    endif()
    if(HAS_GETHOSTBYNAME_R)
        add_definitions(-DHAS_GETHOSTBYNAME_R=1)
    endif()
    if(HAS_GETHOSTBYADDR_R)
        add_definitions(-DHAS_GETHOSTBYADDR_R=1)
    endif()
    if(HAS_INET_PTON)
        add_definitions(-DHAS_INET_PTON=1)
    endif()
    if(HAS_INET_NTOP)
        add_definitions(-DHAS_INET_NTOP=1)
    endif()
    if(HAS_MSGHDR_FLAGS)
        add_definitions(-DHAS_MSGHDR_FLAGS=1)
    endif()
    if(HAS_SOCKLEN_T)
        add_definitions(-DHAS_SOCKLEN_T=1)
    endif()

    # source_group(include FILES ${INCLUDE_FILES})
    # source_group(source FILES ${SOURCE_FILES})

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/enet/callbacks.h
        ${LIB_HEADER_DIR}/enet/enet.h
        ${LIB_HEADER_DIR}/enet/list.h
        ${LIB_HEADER_DIR}/enet/protocol.h
        ${LIB_HEADER_DIR}/enet/time.h
        ${LIB_HEADER_DIR}/enet/types.h
        ${LIB_HEADER_DIR}/enet/unix.h
        ${LIB_HEADER_DIR}/enet/utility.h
        ${LIB_HEADER_DIR}/enet/win32.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/callbacks.c
        ${LIB_SOURCE_DIR}/compress.c
        ${LIB_SOURCE_DIR}/host.c
        ${LIB_SOURCE_DIR}/list.c
        ${LIB_SOURCE_DIR}/packet.c
        ${LIB_SOURCE_DIR}/peer.c
        ${LIB_SOURCE_DIR}/protocol.c
        ${LIB_SOURCE_DIR}/unix.c
        ${LIB_SOURCE_DIR}/win32.c
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        #target_compile_definitions(${LIB_NAME} PUBLIC -DZLIB_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    if (WIN32)
        target_link_libraries(${LIB_NAME} winmm ws2_32) # enet
    endif()

    # install(TARGETS enet
        # RUNTIME DESTINATION bin
        # ARCHIVE DESTINATION lib/static
        # LIBRARY DESTINATION lib)

    # install(DIRECTORY include/
            # DESTINATION include)

endif()
