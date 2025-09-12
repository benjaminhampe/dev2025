set(LIB_NAME de_vorbis) # We dont like to clash with system libs.
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # set(FT_WITH_ZLIB 1)
    # set(VORBIS_VERSION_MAJOR "2")
    # set(VORBIS_VERSION_MINOR "10")
    # set(VORBIS_VERSION_PATCH "0")
    # set(VORBIS_LIBRARY_VERSION "6.16.0")

    set(VORBIS_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/vorbis)
    set(VORBIS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(VORBIS_HEADERS
        ${VORBIS_HEADER_DIR}/codec.h
        ${VORBIS_HEADER_DIR}/vorbisfile.h
        ${VORBIS_HEADER_DIR}/vorbisenc.h
    )

    set(VORBIS_SOURCES
        ${VORBIS_SOURCE_DIR}/vq/bookutil.c
        ${VORBIS_SOURCE_DIR}/vq/bookutil.h
        # ${VORBIS_SOURCE_DIR}/vq/distribution.c
        # ${VORBIS_SOURCE_DIR}/vq/huffbuild.c
        # ${VORBIS_SOURCE_DIR}/vq/latticebuild.c
        # ${VORBIS_SOURCE_DIR}/vq/latticetune.c
        ${VORBIS_SOURCE_DIR}/vq/localcodebook.h
        # ${VORBIS_SOURCE_DIR}/vq/metrics.c
        ${VORBIS_SOURCE_DIR}/vq/vqgen.c
        ${VORBIS_SOURCE_DIR}/vq/vqgen.h

        ${VORBIS_SOURCE_DIR}/analysis.c
        ${VORBIS_SOURCE_DIR}/backends.h
        # ${VORBIS_SOURCE_DIR}/barkmel.c # main app
        ${VORBIS_SOURCE_DIR}/bitrate.c
        ${VORBIS_SOURCE_DIR}/bitrate.h
        ${VORBIS_SOURCE_DIR}/block.c
        ${VORBIS_SOURCE_DIR}/codebook.c
        ${VORBIS_SOURCE_DIR}/codebook.h
        ${VORBIS_SOURCE_DIR}/codec_internal.h
        ${VORBIS_SOURCE_DIR}/envelope.c
        ${VORBIS_SOURCE_DIR}/envelope.h
        ${VORBIS_SOURCE_DIR}/floor0.c
        ${VORBIS_SOURCE_DIR}/floor1.c
        ${VORBIS_SOURCE_DIR}/highlevel.h
        ${VORBIS_SOURCE_DIR}/info.c
        ${VORBIS_SOURCE_DIR}/lookup.c
        ${VORBIS_SOURCE_DIR}/lookup.h
        ${VORBIS_SOURCE_DIR}/lookup_data.h
        ${VORBIS_SOURCE_DIR}/lpc.c
        ${VORBIS_SOURCE_DIR}/lpc.h
        ${VORBIS_SOURCE_DIR}/lsp.c
        ${VORBIS_SOURCE_DIR}/lsp.h
        ${VORBIS_SOURCE_DIR}/mapping0.c
        ${VORBIS_SOURCE_DIR}/masking.h
        ${VORBIS_SOURCE_DIR}/mdct.c
        ${VORBIS_SOURCE_DIR}/mdct.h
        ${VORBIS_SOURCE_DIR}/misc.h
        ${VORBIS_SOURCE_DIR}/os.h
        ${VORBIS_SOURCE_DIR}/psy.c
        ${VORBIS_SOURCE_DIR}/psy.h
        # ${VORBIS_SOURCE_DIR}/psytune.c # app
        ${VORBIS_SOURCE_DIR}/registry.c
        ${VORBIS_SOURCE_DIR}/registry.h
        ${VORBIS_SOURCE_DIR}/res0.c
        ${VORBIS_SOURCE_DIR}/scales.h
        ${VORBIS_SOURCE_DIR}/sharedbook.c # app
        ${VORBIS_SOURCE_DIR}/smallft.c
        ${VORBIS_SOURCE_DIR}/smallft.h
        ${VORBIS_SOURCE_DIR}/synthesis.c
        # ${VORBIS_SOURCE_DIR}/tone.c # app
        ${VORBIS_SOURCE_DIR}/vorbisenc.c
        ${VORBIS_SOURCE_DIR}/vorbisfile.c
        ${VORBIS_SOURCE_DIR}/window.c
        ${VORBIS_SOURCE_DIR}/window.h
        # books
        ${VORBIS_SOURCE_DIR}/books/coupled/res_books_stereo.h
        ${VORBIS_SOURCE_DIR}/books/coupled/res_books_51.h
        ${VORBIS_SOURCE_DIR}/books/floor/floor_books.h
        ${VORBIS_SOURCE_DIR}/books/uncoupled/res_books_uncoupled.h
        # modes
        ${VORBIS_SOURCE_DIR}/modes/floor_all.h
        ${VORBIS_SOURCE_DIR}/modes/psych_11.h
        ${VORBIS_SOURCE_DIR}/modes/psych_16.h
        ${VORBIS_SOURCE_DIR}/modes/psych_44.h
        ${VORBIS_SOURCE_DIR}/modes/psych_8.h
        ${VORBIS_SOURCE_DIR}/modes/residue_16.h
        ${VORBIS_SOURCE_DIR}/modes/residue_44.h
        ${VORBIS_SOURCE_DIR}/modes/residue_44p51.h
        ${VORBIS_SOURCE_DIR}/modes/residue_44u.h
        ${VORBIS_SOURCE_DIR}/modes/residue_8.h
        ${VORBIS_SOURCE_DIR}/modes/setup_11.h
        ${VORBIS_SOURCE_DIR}/modes/setup_16.h
        ${VORBIS_SOURCE_DIR}/modes/setup_22.h
        ${VORBIS_SOURCE_DIR}/modes/setup_32.h
        ${VORBIS_SOURCE_DIR}/modes/setup_44.h
        ${VORBIS_SOURCE_DIR}/modes/setup_44p51.h
        ${VORBIS_SOURCE_DIR}/modes/setup_44u.h
        ${VORBIS_SOURCE_DIR}/modes/setup_8.h
        ${VORBIS_SOURCE_DIR}/modes/setup_X.h
    )

    if(WIN32)
        enable_language(RC)
        list(APPEND VORBIS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND VORBIS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # ${VORBIS_SOURCE_DIR}/common/win32thread.c
        # ${VORBIS_SOURCE_DIR}/common/win32thread.h
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${VORBIS_HEADERS} PRIVATE ${VORBIS_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${VORBIS_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${VORBIS_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PRIVATE 
            -Wno-implicit-fallthrough
            -Wno-unused-parameter
            -Wno-unused-variable
            -Wno-unused-but-set-variable
            -Wno-maybe-uninitialized
        )
    endif()
    
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_VORBIS=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_ogg)

endif()

# add_subdirectory(test)
