set(LIB_NAME de_tiff4)
project(lib${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/tiff4)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/tiffconf.h
        ${LIB_HEADER_DIR}/tiff.h
        ${LIB_HEADER_DIR}/tiffio.h
        ${LIB_HEADER_DIR}/tiffvers.h
    )

    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/mkg3states.c # app
        # ${LIB_SOURCE_DIR}/mkspans.c # app
        ${LIB_SOURCE_DIR}/t4.h
        ${LIB_SOURCE_DIR}/tif_aux.c
        ${LIB_SOURCE_DIR}/tif_close.c
        ${LIB_SOURCE_DIR}/tif_codec.c
        ${LIB_SOURCE_DIR}/tif_color.c
        ${LIB_SOURCE_DIR}/tif_compress.c
        ${LIB_SOURCE_DIR}/tif_config.h
        ${LIB_SOURCE_DIR}/tif_dir.c
        ${LIB_SOURCE_DIR}/tif_dir.h
        ${LIB_SOURCE_DIR}/tif_dirinfo.c
        ${LIB_SOURCE_DIR}/tif_dirread.c
        ${LIB_SOURCE_DIR}/tif_dirwrite.c
        ${LIB_SOURCE_DIR}/tif_dumpmode.c
        ${LIB_SOURCE_DIR}/tif_error.c
        ${LIB_SOURCE_DIR}/tif_extension.c
        ${LIB_SOURCE_DIR}/tif_fax3.c
        ${LIB_SOURCE_DIR}/tif_fax3.h
        ${LIB_SOURCE_DIR}/tif_fax3sm.c
        ${LIB_SOURCE_DIR}/tiffconf.h
        ${LIB_SOURCE_DIR}/tiffio.hxx
        ${LIB_SOURCE_DIR}/tiffiop.h
        ${LIB_SOURCE_DIR}/tif_flush.c
        ${LIB_SOURCE_DIR}/tiffvers.h.in
        ${LIB_SOURCE_DIR}/tif_getimage.c
        ${LIB_SOURCE_DIR}/tif_jbig.c
        ${LIB_SOURCE_DIR}/tif_jpeg_12.c
        ${LIB_SOURCE_DIR}/tif_jpeg.c
        ${LIB_SOURCE_DIR}/tif_luv.c
        ${LIB_SOURCE_DIR}/tif_lzma.c
        ${LIB_SOURCE_DIR}/tif_lzw.c
        ${LIB_SOURCE_DIR}/tif_next.c
        ${LIB_SOURCE_DIR}/tif_ojpeg.c
        ${LIB_SOURCE_DIR}/tif_open.c
        ${LIB_SOURCE_DIR}/tif_packbits.c
        ${LIB_SOURCE_DIR}/tif_pixarlog.c
        ${LIB_SOURCE_DIR}/tif_predict.c
        ${LIB_SOURCE_DIR}/tif_predict.h
        ${LIB_SOURCE_DIR}/tif_print.c
        ${LIB_SOURCE_DIR}/tif_read.c
        # ${LIB_SOURCE_DIR}/tif_stream.cxx
        ${LIB_SOURCE_DIR}/tif_strip.c
        ${LIB_SOURCE_DIR}/tif_swab.c
        ${LIB_SOURCE_DIR}/tif_thunder.c
        ${LIB_SOURCE_DIR}/tif_tile.c
        ${LIB_SOURCE_DIR}/tif_version.c
        ${LIB_SOURCE_DIR}/tif_warning.c
        ${LIB_SOURCE_DIR}/tif_write.c
        ${LIB_SOURCE_DIR}/tif_zip.c
        ${LIB_SOURCE_DIR}/uvcode.h
        # ${LIB_SOURCE_DIR}/tif_vms.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    if (WIN32)
        list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/tif_win32.c)
    endif ()

    if (WINCE)
        list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/tif_wince.c)
    endif ()

    if (UNIX)
        list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/tif_unix.c)
    endif ()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBTIFF4=1)
    target_compile_definitions(${LIB_NAME} PUBLIC USE_TIFF4=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough
                    -Wno-unused-parameter
                    -Wno-unused-but-set-variable
                    -Wno-int-to-pointer-cast
                    -Wno-pointer-to-int-cast
                    -Wno-format
                    -Wno-format-extra-args
                    -Wno-misleading-indentation
        )
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)
    target_link_libraries(${LIB_NAME} PUBLIC de_jpeg8d)
    
endif()
