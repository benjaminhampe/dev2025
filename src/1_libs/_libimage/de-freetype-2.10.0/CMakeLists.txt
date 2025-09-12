set(LIB_NAME de_freetype2) # We dont like to clash with system libs.
project(${LIB_NAME}-dev-v2.10.0 C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(FT_WITH_ZLIB 1)
    set(FT_WITH_BZIP2 1)
    set(FT_WITH_PNG 1)
    set(FT_WITH_HARFBUZZ 0)
    set(FT2_VERSION_MAJOR "2")
    set(FT2_VERSION_MINOR "10")
    set(FT2_VERSION_PATCH "0")
    set(FT2_LIBRARY_VERSION "6.16.0")
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/freetype2)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/freetype2)

    set(LIB_HEADERS
        ${CMAKE_CURRENT_LIST_DIR}/include/de_freetype2.hpp # what everybody should include to use this lib.
        ${LIB_HEADER_DIR}/libFreeType2.hpp
        ${LIB_HEADER_DIR}/ft2build.h
        ${LIB_HEADER_DIR}/freetype/freetype.h
        ${LIB_HEADER_DIR}/freetype/ftadvanc.h
        ${LIB_HEADER_DIR}/freetype/ftbbox.h
        ${LIB_HEADER_DIR}/freetype/ftbdf.h
        ${LIB_HEADER_DIR}/freetype/ftbitmap.h
        ${LIB_HEADER_DIR}/freetype/ftbzip2.h
        ${LIB_HEADER_DIR}/freetype/ftcache.h
        ${LIB_HEADER_DIR}/freetype/ftchapters.h
        ${LIB_HEADER_DIR}/freetype/ftcid.h
        ${LIB_HEADER_DIR}/freetype/ftcolor.h
        ${LIB_HEADER_DIR}/freetype/ftdriver.h
        ${LIB_HEADER_DIR}/freetype/fterrdef.h
        ${LIB_HEADER_DIR}/freetype/fterrors.h
        ${LIB_HEADER_DIR}/freetype/ftfntfmt.h
        ${LIB_HEADER_DIR}/freetype/ftgasp.h
        ${LIB_HEADER_DIR}/freetype/ftglyph.h
        ${LIB_HEADER_DIR}/freetype/ftgxval.h
        ${LIB_HEADER_DIR}/freetype/ftgzip.h
        ${LIB_HEADER_DIR}/freetype/ftimage.h
        ${LIB_HEADER_DIR}/freetype/ftincrem.h
        ${LIB_HEADER_DIR}/freetype/ftlcdfil.h
        ${LIB_HEADER_DIR}/freetype/ftlist.h
        ${LIB_HEADER_DIR}/freetype/ftlzw.h
        ${LIB_HEADER_DIR}/freetype/ftmac.h
        ${LIB_HEADER_DIR}/freetype/ftmm.h
        ${LIB_HEADER_DIR}/freetype/ftmodapi.h
        ${LIB_HEADER_DIR}/freetype/ftmoderr.h
        ${LIB_HEADER_DIR}/freetype/ftotval.h
        ${LIB_HEADER_DIR}/freetype/ftoutln.h
        ${LIB_HEADER_DIR}/freetype/ftparams.h
        ${LIB_HEADER_DIR}/freetype/ftpfr.h
        ${LIB_HEADER_DIR}/freetype/ftrender.h
        ${LIB_HEADER_DIR}/freetype/ftsizes.h
        ${LIB_HEADER_DIR}/freetype/ftsnames.h
        ${LIB_HEADER_DIR}/freetype/ftstroke.h
        ${LIB_HEADER_DIR}/freetype/ftsynth.h
        ${LIB_HEADER_DIR}/freetype/ftsystem.h
        ${LIB_HEADER_DIR}/freetype/fttrigon.h
        ${LIB_HEADER_DIR}/freetype/fttypes.h
        ${LIB_HEADER_DIR}/freetype/ftwinfnt.h
        ${LIB_HEADER_DIR}/freetype/t1tables.h
        ${LIB_HEADER_DIR}/freetype/ttnameid.h
        ${LIB_HEADER_DIR}/freetype/tttables.h
        ${LIB_HEADER_DIR}/freetype/tttags.h
        # Config headers
        ${LIB_HEADER_DIR}/freetype/config/ftconfig.h
        ${LIB_HEADER_DIR}/freetype/config/ftheader.h
        ${LIB_HEADER_DIR}/freetype/config/ftmodule.h
        ${LIB_HEADER_DIR}/freetype/config/ftoption.h
        ${LIB_HEADER_DIR}/freetype/config/ftstdlib.h
        # Internal headers
        ${LIB_HEADER_DIR}/freetype/internal/autohint.h
        ${LIB_HEADER_DIR}/freetype/internal/cffotypes.h
        ${LIB_HEADER_DIR}/freetype/internal/cfftypes.h
        ${LIB_HEADER_DIR}/freetype/internal/ftcalc.h
        ${LIB_HEADER_DIR}/freetype/internal/ftdebug.h
        ${LIB_HEADER_DIR}/freetype/internal/ftdrv.h
        ${LIB_HEADER_DIR}/freetype/internal/ftgloadr.h
        ${LIB_HEADER_DIR}/freetype/internal/fthash.h
        ${LIB_HEADER_DIR}/freetype/internal/ftmemory.h
        ${LIB_HEADER_DIR}/freetype/internal/ftobjs.h
        ${LIB_HEADER_DIR}/freetype/internal/ftpsprop.h
        ${LIB_HEADER_DIR}/freetype/internal/ftrfork.h
        ${LIB_HEADER_DIR}/freetype/internal/ftserv.h
        ${LIB_HEADER_DIR}/freetype/internal/ftstream.h
        ${LIB_HEADER_DIR}/freetype/internal/fttrace.h
        ${LIB_HEADER_DIR}/freetype/internal/ftvalid.h
        ${LIB_HEADER_DIR}/freetype/internal/internal.h
        ${LIB_HEADER_DIR}/freetype/internal/psaux.h
        ${LIB_HEADER_DIR}/freetype/internal/pshints.h
        ${LIB_HEADER_DIR}/freetype/internal/sfnt.h
        ${LIB_HEADER_DIR}/freetype/internal/t1types.h
        ${LIB_HEADER_DIR}/freetype/internal/tttypes.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/autofit/autofit.c
        ${LIB_SOURCE_DIR}/base/ftbase.c
        ${LIB_SOURCE_DIR}/base/ftbbox.c
        ${LIB_SOURCE_DIR}/base/ftbdf.c
        ${LIB_SOURCE_DIR}/base/ftbitmap.c
        ${LIB_SOURCE_DIR}/base/ftcid.c
        ${LIB_SOURCE_DIR}/base/ftfstype.c
        ${LIB_SOURCE_DIR}/base/ftgasp.c
        ${LIB_SOURCE_DIR}/base/ftglyph.c
        ${LIB_SOURCE_DIR}/base/ftgxval.c
        ${LIB_SOURCE_DIR}/base/ftinit.c
        ${LIB_SOURCE_DIR}/base/ftmm.c
        ${LIB_SOURCE_DIR}/base/ftotval.c
        ${LIB_SOURCE_DIR}/base/ftpatent.c
        ${LIB_SOURCE_DIR}/base/ftpfr.c
        ${LIB_SOURCE_DIR}/base/ftstroke.c
        ${LIB_SOURCE_DIR}/base/ftsynth.c
        ${LIB_SOURCE_DIR}/base/ftsystem.c
        ${LIB_SOURCE_DIR}/base/fttype1.c
        ${LIB_SOURCE_DIR}/base/ftwinfnt.c
        ${LIB_SOURCE_DIR}/bdf/bdf.c
        ${LIB_SOURCE_DIR}/bzip2/ftbzip2.c
        ${LIB_SOURCE_DIR}/cache/ftcache.c
        ${LIB_SOURCE_DIR}/cff/cff.c
        ${LIB_SOURCE_DIR}/cid/type1cid.c
        ${LIB_SOURCE_DIR}/gzip/ftgzip.c
        ${LIB_SOURCE_DIR}/lzw/ftlzw.c
        ${LIB_SOURCE_DIR}/pcf/pcf.c
        ${LIB_SOURCE_DIR}/pfr/pfr.c
        ${LIB_SOURCE_DIR}/psaux/psaux.c
        ${LIB_SOURCE_DIR}/pshinter/pshinter.c
        ${LIB_SOURCE_DIR}/psnames/psnames.c
        ${LIB_SOURCE_DIR}/raster/raster.c
        ${LIB_SOURCE_DIR}/sfnt/sfnt.c
        ${LIB_SOURCE_DIR}/smooth/smooth.c
        ${LIB_SOURCE_DIR}/truetype/truetype.c
        ${LIB_SOURCE_DIR}/type1/type1.c
        ${LIB_SOURCE_DIR}/type42/type42.c
        ${LIB_SOURCE_DIR}/winfonts/winfnt.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (WIN32)
    #	  list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/builds/windows/ftdebug.c ${LIB_SOURCE_DIR}/base/ftver.rc)
    elseif (WINCE)
    #	  list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/builds/wince/ftdebug.c)
    else ()
    #	  list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/base/ftdebug.c)
    endif ()

    list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/base/ftdebug.c)

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FREETYPE2)
    target_compile_definitions(${LIB_NAME} PUBLIC FT2_BUILD_LIBRARY)
    target_compile_definitions(${LIB_NAME} PUBLIC FT_WITH_ZLIB=1)
    target_compile_definitions(${LIB_NAME} PUBLIC FT_WITH_BZIP2=1)
    target_compile_definitions(${LIB_NAME} PUBLIC FT_WITH_PNG=1)
    target_compile_definitions(${LIB_NAME} PUBLIC FT_WITH_HARFBUZZ=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_compile_options(${LIB_NAME} PRIVATE -Wmaybe-uninitialized)
    endif()
    
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/freetype)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/freetype/config)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/freetype/internal)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_link_libraries(${LIB_NAME} PRIVATE de_png de_bzip2 de_z) # delib_harfbuzz
    target_link_libraries(${LIB_NAME} PRIVATE de_zlib) # delib_harfbuzz

    ### add_custom_command(TARGET foo POST_BUILD
    ###    COMMAND ${CMAKE_COMMAND} -E copy
    ###            ${CMAKE_SOURCE_DIR}/test/input.txt
    ###            ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
endif()

# add_subdirectory(test)
