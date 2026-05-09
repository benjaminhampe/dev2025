set(LIB_NAME de_zix) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/zix/allocator.h
        ${LIB_HEADER_DIR}/zix/attributes.h
        ${LIB_HEADER_DIR}/zix/btree.h
        ${LIB_HEADER_DIR}/zix/bump_allocator.h
        ${LIB_HEADER_DIR}/zix/digest.h
        ${LIB_HEADER_DIR}/zix/environment.h
        ${LIB_HEADER_DIR}/zix/filesystem.h
        ${LIB_HEADER_DIR}/zix/hash.h
        ${LIB_HEADER_DIR}/zix/path.h
        ${LIB_HEADER_DIR}/zix/ring.h
        ${LIB_HEADER_DIR}/zix/sem.h
        ${LIB_HEADER_DIR}/zix/status.h
        ${LIB_HEADER_DIR}/zix/string_view.h
        ${LIB_HEADER_DIR}/zix/thread.h
        ${LIB_HEADER_DIR}/zix/tree.h
        ${LIB_HEADER_DIR}/zix/warnings.h
        ${LIB_HEADER_DIR}/zix/zix.h
    )
    
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/zix_config.h    
        ${LIB_SOURCE_DIR}/allocator.c
        ${LIB_SOURCE_DIR}/btree.c
        ${LIB_SOURCE_DIR}/bump_allocator.c

        ${LIB_SOURCE_DIR}/digest.c
        ${LIB_SOURCE_DIR}/errno_status.c
        ${LIB_SOURCE_DIR}/errno_status.h
        ${LIB_SOURCE_DIR}/filesystem.c
        ${LIB_SOURCE_DIR}/hash.c
        ${LIB_SOURCE_DIR}/index_range.h
        ${LIB_SOURCE_DIR}/path.c
        ${LIB_SOURCE_DIR}/path_iter.h

        ${LIB_SOURCE_DIR}/qualifiers.h
        ${LIB_SOURCE_DIR}/ring.c
        ${LIB_SOURCE_DIR}/status.c
        ${LIB_SOURCE_DIR}/string_view.c
        ${LIB_SOURCE_DIR}/system.c
        ${LIB_SOURCE_DIR}/system.h
        ${LIB_SOURCE_DIR}/tree.c

        # ${LIB_SOURCE_DIR}/darwin/sem_darwin.c

        # ${LIB_SOURCE_DIR}/posix/environment_posix.c
        # ${LIB_SOURCE_DIR}/posix/filesystem_posix.c
        # ${LIB_SOURCE_DIR}/posix/sem_posix.c
        # ${LIB_SOURCE_DIR}/posix/system_posix.c
        # ${LIB_SOURCE_DIR}/posix/thread_posix.c

        ${LIB_SOURCE_DIR}/win32/environment_win32.c
        ${LIB_SOURCE_DIR}/win32/filesystem_win32.c
        ${LIB_SOURCE_DIR}/win32/sem_win32.c
        ${LIB_SOURCE_DIR}/win32/system_win32.c
        ${LIB_SOURCE_DIR}/win32/thread_win32.c
        ${LIB_SOURCE_DIR}/win32/win32_util.c
        ${LIB_SOURCE_DIR}/win32/win32_util.h
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")

        # # if(WIN32)
            # # enable_language(RC)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # # endif()
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC ZIX_STATIC=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE LILV_DYN_MANIFEST=1)
    # endif()

    target_compile_definitions(${LIB_NAME} PRIVATE ZIX_INTERNAL=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

endif()
