set(LIB_NAME de_zstd)
project(${LIB_NAME} C) # ASM hier entfernt, da für Windows-Builds fehleranfällig

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # Finde die plattformunabhängige Thread-Bibliothek
    find_package(Threads REQUIRED)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/zdict.h
        ${LIB_HEADER_DIR}/zstd.h
        ${LIB_HEADER_DIR}/zstd_errors.h
        ${LIB_HEADER_DIR}/common/mem.h
        ${LIB_HEADER_DIR}/common/zstd_deps.h
        ${LIB_HEADER_DIR}/common/compiler.h
        ${LIB_HEADER_DIR}/common/portability_macros.h
        ${LIB_HEADER_DIR}/common/debug.h
        ${LIB_HEADER_DIR}/common/pool.h
        ${LIB_HEADER_DIR}/common/threading.h
        ${LIB_HEADER_DIR}/common/allocations.h
        ${LIB_HEADER_DIR}/common/bits.h
        ${LIB_HEADER_DIR}/common/bitstream.h
        ${LIB_HEADER_DIR}/common/cpu.h
        ${LIB_HEADER_DIR}/common/fse.h
        ${LIB_HEADER_DIR}/common/huf.h
        ${LIB_HEADER_DIR}/common/xxhash.h
        ${LIB_HEADER_DIR}/common/zstd_internal.h
        ${LIB_HEADER_DIR}/common/zstd_trace.h
    )
    set(LIB_SOURCES
        # common
        ${LIB_SOURCE_DIR}/common/debug.c
        ${LIB_SOURCE_DIR}/common/entropy_common.c
        ${LIB_SOURCE_DIR}/common/error_private.c
        ${LIB_SOURCE_DIR}/common/error_private.h
        ${LIB_SOURCE_DIR}/common/fse_decompress.c
        ${LIB_SOURCE_DIR}/common/pool.c
        ${LIB_SOURCE_DIR}/common/threading.c
        ${LIB_SOURCE_DIR}/common/xxhash.c
        ${LIB_SOURCE_DIR}/common/zstd_common.c
        # compress
        ${LIB_SOURCE_DIR}/compress/clevels.h
        ${LIB_SOURCE_DIR}/compress/fse_compress.c
        ${LIB_SOURCE_DIR}/compress/hist.c
        ${LIB_SOURCE_DIR}/compress/hist.h
        ${LIB_SOURCE_DIR}/compress/huf_compress.c
        ${LIB_SOURCE_DIR}/compress/zstdmt_compress.c
        ${LIB_SOURCE_DIR}/compress/zstdmt_compress.h
        ${LIB_SOURCE_DIR}/compress/zstd_compress.c
        ${LIB_SOURCE_DIR}/compress/zstd_compress_internal.h
        ${LIB_SOURCE_DIR}/compress/zstd_compress_literals.c
        ${LIB_SOURCE_DIR}/compress/zstd_compress_literals.h
        ${LIB_SOURCE_DIR}/compress/zstd_compress_sequences.c
        ${LIB_SOURCE_DIR}/compress/zstd_compress_sequences.h
        ${LIB_SOURCE_DIR}/compress/zstd_compress_superblock.c
        ${LIB_SOURCE_DIR}/compress/zstd_compress_superblock.h
        ${LIB_SOURCE_DIR}/compress/zstd_cwksp.h
        ${LIB_SOURCE_DIR}/compress/zstd_double_fast.c
        ${LIB_SOURCE_DIR}/compress/zstd_double_fast.h
        ${LIB_SOURCE_DIR}/compress/zstd_fast.c
        ${LIB_SOURCE_DIR}/compress/zstd_fast.h
        ${LIB_SOURCE_DIR}/compress/zstd_lazy.c
        ${LIB_SOURCE_DIR}/compress/zstd_lazy.h
        ${LIB_SOURCE_DIR}/compress/zstd_ldm.c
        ${LIB_SOURCE_DIR}/compress/zstd_ldm.h
        ${LIB_SOURCE_DIR}/compress/zstd_ldm_geartab.h
        ${LIB_SOURCE_DIR}/compress/zstd_opt.c
        ${LIB_SOURCE_DIR}/compress/zstd_opt.h
        ${LIB_SOURCE_DIR}/compress/zstd_preSplit.c
        ${LIB_SOURCE_DIR}/compress/zstd_preSplit.h
        # decompress
        ${LIB_SOURCE_DIR}/decompress/huf_decompress.c
        ${LIB_SOURCE_DIR}/decompress/zstd_ddict.c
        ${LIB_SOURCE_DIR}/decompress/zstd_ddict.h
        ${LIB_SOURCE_DIR}/decompress/zstd_decompress.c
        ${LIB_SOURCE_DIR}/decompress/zstd_decompress_block.c
        ${LIB_SOURCE_DIR}/decompress/zstd_decompress_block.h
        ${LIB_SOURCE_DIR}/decompress/zstd_decompress_internal.h
        # dictBuilder
        ${LIB_SOURCE_DIR}/dictBuilder/cover.c
        ${LIB_SOURCE_DIR}/dictBuilder/cover.h
        ${LIB_SOURCE_DIR}/dictBuilder/divsufsort.c
        ${LIB_SOURCE_DIR}/dictBuilder/divsufsort.h
        ${LIB_SOURCE_DIR}/dictBuilder/fastcover.c
        ${LIB_SOURCE_DIR}/dictBuilder/zdict.c
        # legacy
        ${LIB_SOURCE_DIR}/legacy/zstd_legacy.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v01.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v01.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v02.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v02.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v03.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v03.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v04.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v04.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v05.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v05.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v06.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v06.h
        ${LIB_SOURCE_DIR}/legacy/zstd_v07.c
        ${LIB_SOURCE_DIR}/legacy/zstd_v07.h
        # deprecated
        # ${LIB_SOURCE_DIR}/deprecated/zbuff.h
        # ${LIB_SOURCE_DIR}/deprecated/zbuff_common.c
        # ${LIB_SOURCE_DIR}/deprecated/zbuff_compress.c
        # ${LIB_SOURCE_DIR}/deprecated/zbuff_decompress.c
    )

    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} STATIC "")

    target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_STATIC=1)
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # ASM-Optimierung nur für Linux/Unix aktivieren, da PE/COFF (Windows) inkompatibel ist
    if (UNIX AND NOT APPLE AND (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "amd64"))
        enable_language(ASM)
        target_sources(${LIB_NAME} PRIVATE "${LIB_SOURCE_DIR}/decompress/huf_decompress_amd64.S")
        set_source_files_properties("${LIB_SOURCE_DIR}/decompress/huf_decompress_amd64.S" PROPERTIES LANGUAGE ASM)
    endif()

    # Include Verzeichnisse
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE
        ${LIB_HEADER_DIR}/common
        ${LIB_SOURCE_DIR}
        ${LIB_SOURCE_DIR}/common
        ${LIB_SOURCE_DIR}/compress
        ${LIB_SOURCE_DIR}/decompress
        ${LIB_SOURCE_DIR}/dictBuilder
        ${LIB_SOURCE_DIR}/legacy
    )

    # Compile Definitions
    target_compile_definitions(${LIB_NAME} PUBLIC
        HAVE_LIBZSTD=1
        HAVE_ZSTD=1
        ZSTD_BUILD_COMPRESSION=1
        ZSTD_BUILD_DECOMPRESSION=1
        ZSTD_BUILD_DICTBUILDER=1
        ZSTD_BUILD_DEPRECATED=0
        ZSTD_LEGACY_SUPPORT=1
        ZSTD_MULTITHREAD_SUPPORT=1
        ZSTD_MULTITHREAD=1
        ZSTD_DISABLE_ASM=1
        ZSTD_STATIC_LINKING_ONLY=1
    )

    if (MSVC)
        target_compile_definitions(${LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS=1)
        target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_HEAPMODE=0)
        if (ZSTD_DLL)
            target_compile_definitions(${LIB_NAME} PUBLIC _CONSOLE=1)
        endif()
    else()
        # Sauberer Link gegen Threads (funktioniert mit MinGW-LLVM und GCC/Clang auf Linux)
        target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads)
    endif()

endif()

add_subdirectory(test)




# set(LIB_NAME de_zstd)
# project(${LIB_NAME} C ASM) # -1.5.7

# if (TARGET ${LIB_NAME})
#     message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
# else()
#     set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
#     set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
#     set(LIB_HEADERS
#         ${LIB_HEADER_DIR}/zdict.h
#         ${LIB_HEADER_DIR}/zstd.h
#         ${LIB_HEADER_DIR}/zstd_errors.h
#         ${LIB_HEADER_DIR}/common/mem.h
#         ${LIB_HEADER_DIR}/common/zstd_deps.h
#         ${LIB_HEADER_DIR}/common/compiler.h
#         ${LIB_HEADER_DIR}/common/portability_macros.h
#         ${LIB_HEADER_DIR}/common/debug.h
#         ${LIB_HEADER_DIR}/common/pool.h
#         ${LIB_HEADER_DIR}/common/threading.h
#         ${LIB_HEADER_DIR}/common/allocations.h
#         ${LIB_HEADER_DIR}/common/bits.h
#         ${LIB_HEADER_DIR}/common/bitstream.h
#         ${LIB_HEADER_DIR}/common/cpu.h
#         ${LIB_HEADER_DIR}/common/fse.h
#         ${LIB_HEADER_DIR}/common/huf.h
#         ${LIB_HEADER_DIR}/common/xxhash.h
#         ${LIB_HEADER_DIR}/common/zstd_internal.h
#         ${LIB_HEADER_DIR}/common/zstd_trace.h
#     )
#     set(LIB_SOURCES
#         # common
#         ${LIB_SOURCE_DIR}/common/debug.c
#         ${LIB_SOURCE_DIR}/common/entropy_common.c
#         ${LIB_SOURCE_DIR}/common/error_private.c
#         ${LIB_SOURCE_DIR}/common/error_private.h
#         ${LIB_SOURCE_DIR}/common/fse_decompress.c
#         ${LIB_SOURCE_DIR}/common/pool.c
#         ${LIB_SOURCE_DIR}/common/threading.c
#         ${LIB_SOURCE_DIR}/common/xxhash.c
#         ${LIB_SOURCE_DIR}/common/zstd_common.c
#         # compress
#         ${LIB_SOURCE_DIR}/compress/clevels.h
#         ${LIB_SOURCE_DIR}/compress/fse_compress.c
#         ${LIB_SOURCE_DIR}/compress/hist.c
#         ${LIB_SOURCE_DIR}/compress/hist.h
#         ${LIB_SOURCE_DIR}/compress/huf_compress.c
#         ${LIB_SOURCE_DIR}/compress/zstdmt_compress.c
#         ${LIB_SOURCE_DIR}/compress/zstdmt_compress.h
#         ${LIB_SOURCE_DIR}/compress/zstd_compress.c
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_internal.h
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_literals.c
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_literals.h
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_sequences.c
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_sequences.h
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_superblock.c
#         ${LIB_SOURCE_DIR}/compress/zstd_compress_superblock.h
#         ${LIB_SOURCE_DIR}/compress/zstd_cwksp.h
#         ${LIB_SOURCE_DIR}/compress/zstd_double_fast.c
#         ${LIB_SOURCE_DIR}/compress/zstd_double_fast.h
#         ${LIB_SOURCE_DIR}/compress/zstd_fast.c
#         ${LIB_SOURCE_DIR}/compress/zstd_fast.h
#         ${LIB_SOURCE_DIR}/compress/zstd_lazy.c
#         ${LIB_SOURCE_DIR}/compress/zstd_lazy.h
#         ${LIB_SOURCE_DIR}/compress/zstd_ldm.c
#         ${LIB_SOURCE_DIR}/compress/zstd_ldm.h
#         ${LIB_SOURCE_DIR}/compress/zstd_ldm_geartab.h
#         ${LIB_SOURCE_DIR}/compress/zstd_opt.c
#         ${LIB_SOURCE_DIR}/compress/zstd_opt.h
#         ${LIB_SOURCE_DIR}/compress/zstd_preSplit.c
#         ${LIB_SOURCE_DIR}/compress/zstd_preSplit.h
#         # decompress
#         ${LIB_SOURCE_DIR}/decompress/huf_decompress.c
#         ${LIB_SOURCE_DIR}/decompress/zstd_ddict.c
#         ${LIB_SOURCE_DIR}/decompress/zstd_ddict.h
#         ${LIB_SOURCE_DIR}/decompress/zstd_decompress.c
#         ${LIB_SOURCE_DIR}/decompress/zstd_decompress_block.c
#         ${LIB_SOURCE_DIR}/decompress/zstd_decompress_block.h
#         ${LIB_SOURCE_DIR}/decompress/zstd_decompress_internal.h
#         # dictBuilder
#         ${LIB_SOURCE_DIR}/dictBuilder/cover.c
#         ${LIB_SOURCE_DIR}/dictBuilder/cover.h
#         ${LIB_SOURCE_DIR}/dictBuilder/divsufsort.c
#         ${LIB_SOURCE_DIR}/dictBuilder/divsufsort.h
#         ${LIB_SOURCE_DIR}/dictBuilder/fastcover.c
#         ${LIB_SOURCE_DIR}/dictBuilder/zdict.c
#         # legacy
#         ${LIB_SOURCE_DIR}/legacy/zstd_legacy.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v01.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v01.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v02.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v02.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v03.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v03.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v04.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v04.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v05.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v05.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v06.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v06.h
#         ${LIB_SOURCE_DIR}/legacy/zstd_v07.c
#         ${LIB_SOURCE_DIR}/legacy/zstd_v07.h
#         # deprecated
#         # ${LIB_SOURCE_DIR}/deprecated/zbuff.h
#         # ${LIB_SOURCE_DIR}/deprecated/zbuff_common.c
#         # ${LIB_SOURCE_DIR}/deprecated/zbuff_compress.c
#         # ${LIB_SOURCE_DIR}/deprecated/zbuff_decompress.c
#     )

#         message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
#         add_library(${LIB_NAME} STATIC "")
#         target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_STATIC=1)

#     target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

#     target_sources(${LIB_NAME} PRIVATE
#         "${LIB_SOURCE_DIR}/decompress/huf_decompress_amd64.S")
#     set_source_files_properties(
#         "${LIB_SOURCE_DIR}/decompress/huf_decompress_amd64.S"
#          PROPERTIES LANGUAGE ASM)

#     target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/common)
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/common)
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/compress)
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/decompress)
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dictBuilder)
#     target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/legacy)

#     target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBZSTD=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ZSTD=1)

#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_COMPRESSION=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_DECOMPRESSION=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_DICTBUILDER=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_BUILD_DEPRECATED=0)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_LEGACY_SUPPORT=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_MULTITHREAD_SUPPORT=1)
#     target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_MULTITHREAD=1)

#     if (MSVC)
#         target_compile_definitions(${LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS=1)
#         target_compile_definitions(${LIB_NAME} PUBLIC ZSTD_HEAPMODE=0)
#         if (ZSTD_DLL)
#             target_compile_definitions(${LIB_NAME} PUBLIC _CONSOLE=1)
#         endif()
#     else()
#         target_link_libraries(${LIB_NAME} PUBLIC pthread)
#     endif()

# endif()

# add_subdirectory(test)
