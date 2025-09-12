set(LIB_NAME de_sndfile)
project(${LIB_NAME}-dev-1.0.28 C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(CPU_CLIPS_POSITIVE 0)
    set(CPU_CLIPS_NEGATIVE 0)
    set(ENABLE_EXPERIMENTAL_CODE 0)

    # if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    # set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=gnu99 -Wall -Wextra" CACHE STRING "" FORCE)
    # set (CMAKE_CXX__FLAGS "${CMAKE_C_FLAGS} -O3 -std=gnu99 -Wall -Wextra" CACHE STRING "" FORCE)
    # set (COMPILER_IS_GCC 1)
    # set (_POSIX_SOURCE 1)
    # if (${Werror} MATCHES "on")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror" CACHE STRING "" FORCE)
    # endif ()
    # endif ()

    set(SND_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(SND_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(SND_HEADERS ${SND_HEADER_DIR}/sndfile.h)
    set(SND_SOURCES
        ${SND_SOURCE_DIR}/ALAC/ALACBitUtilities.c
        ${SND_SOURCE_DIR}/ALAC/ag_dec.c
        ${SND_SOURCE_DIR}/ALAC/ag_enc.c
        ${SND_SOURCE_DIR}/ALAC/alac_decoder.c
        ${SND_SOURCE_DIR}/ALAC/alac_encoder.c
        ${SND_SOURCE_DIR}/ALAC/dp_dec.c
        ${SND_SOURCE_DIR}/ALAC/dp_enc.c
        ${SND_SOURCE_DIR}/ALAC/matrix_dec.c
        ${SND_SOURCE_DIR}/ALAC/matrix_enc.c
        ${SND_SOURCE_DIR}/G72x/g721.c
        ${SND_SOURCE_DIR}/G72x/g723_16.c
        ${SND_SOURCE_DIR}/G72x/g723_24.c
        ${SND_SOURCE_DIR}/G72x/g723_40.c
        ${SND_SOURCE_DIR}/G72x/g72x.c
        # ${SND_SOURCE_DIR}/G72x/g72x_test.c
        ${SND_SOURCE_DIR}/GSM610/add.c
        ${SND_SOURCE_DIR}/GSM610/code.c
        ${SND_SOURCE_DIR}/GSM610/decode.c
        ${SND_SOURCE_DIR}/GSM610/gsm_create.c
        ${SND_SOURCE_DIR}/GSM610/gsm_decode.c
        ${SND_SOURCE_DIR}/GSM610/gsm_destroy.c
        ${SND_SOURCE_DIR}/GSM610/gsm_encode.c
        ${SND_SOURCE_DIR}/GSM610/gsm_option.c
        ${SND_SOURCE_DIR}/GSM610/long_term.c
        ${SND_SOURCE_DIR}/GSM610/lpc.c
        ${SND_SOURCE_DIR}/GSM610/preprocess.c
        ${SND_SOURCE_DIR}/GSM610/rpe.c
        ${SND_SOURCE_DIR}/GSM610/short_term.c
        ${SND_SOURCE_DIR}/GSM610/table.c
        ${SND_SOURCE_DIR}/aiff.c
        ${SND_SOURCE_DIR}/alac.c
        ${SND_SOURCE_DIR}/alaw.c
        ${SND_SOURCE_DIR}/au.c
        ${SND_SOURCE_DIR}/audio_detect.c
        ${SND_SOURCE_DIR}/avr.c
        ${SND_SOURCE_DIR}/broadcast.c
        ${SND_SOURCE_DIR}/caf.c
        ${SND_SOURCE_DIR}/cart.c
        ${SND_SOURCE_DIR}/chanmap.c
        ${SND_SOURCE_DIR}/chunk.c
        ${SND_SOURCE_DIR}/command.c
        ${SND_SOURCE_DIR}/common.c
        ${SND_SOURCE_DIR}/dither.c
        ${SND_SOURCE_DIR}/double64.c
        ${SND_SOURCE_DIR}/dwd.c
        ${SND_SOURCE_DIR}/dwvw.c
        ${SND_SOURCE_DIR}/file_io.c
        ${SND_SOURCE_DIR}/flac.c
        ${SND_SOURCE_DIR}/float32.c
        ${SND_SOURCE_DIR}/g72x.c
        ${SND_SOURCE_DIR}/gsm610.c
        ${SND_SOURCE_DIR}/htk.c
        ${SND_SOURCE_DIR}/id3.c
        ${SND_SOURCE_DIR}/ima_adpcm.c
        ${SND_SOURCE_DIR}/ima_oki_adpcm.c
        ${SND_SOURCE_DIR}/interleave.c
        ${SND_SOURCE_DIR}/ircam.c
        ${SND_SOURCE_DIR}/macos.c
        ${SND_SOURCE_DIR}/mat4.c
        ${SND_SOURCE_DIR}/mat5.c
        ${SND_SOURCE_DIR}/mpc2k.c
        ${SND_SOURCE_DIR}/ms_adpcm.c
        ${SND_SOURCE_DIR}/nist.c
        ${SND_SOURCE_DIR}/ogg.c
        ${SND_SOURCE_DIR}/ogg_opus.c
        ${SND_SOURCE_DIR}/ogg_pcm.c
        ${SND_SOURCE_DIR}/ogg_speex.c
        ${SND_SOURCE_DIR}/ogg_vorbis.c
        ${SND_SOURCE_DIR}/paf.c
        ${SND_SOURCE_DIR}/pcm.c
        ${SND_SOURCE_DIR}/pvf.c
        ${SND_SOURCE_DIR}/raw.c
        ${SND_SOURCE_DIR}/rf64.c
        ${SND_SOURCE_DIR}/rx2.c
        ${SND_SOURCE_DIR}/sd2.c
        ${SND_SOURCE_DIR}/sds.c
        ${SND_SOURCE_DIR}/sndfile.c
        ${SND_SOURCE_DIR}/strings.c
        ${SND_SOURCE_DIR}/svx.c
        ${SND_SOURCE_DIR}/txw.c
        ${SND_SOURCE_DIR}/ulaw.c
        ${SND_SOURCE_DIR}/voc.c
        ${SND_SOURCE_DIR}/vox_adpcm.c
        ${SND_SOURCE_DIR}/w64.c
        ${SND_SOURCE_DIR}/wav.c
        ${SND_SOURCE_DIR}/wavlike.c
        ${SND_SOURCE_DIR}/wve.c
        ${SND_SOURCE_DIR}/xi.c
    )

    if (WIN32)
        list(APPEND SND_SOURCES ${SND_SOURCE_DIR}/windows.c)
    endif()

    if(WIN32)
        enable_language(RC)
        list(APPEND SND_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND SND_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        if (WIN32)
            target_compile_definitions(${LIB_NAME} PRIVATE WIN32_TARGET_DLL=1 )
        endif()
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE WORDS_BIGENDIAN=0 )
    # target_compile_definitions(${LIB_NAME} PRIVATE -D_FILE_OFFSET_BITS=64 )
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter )

    if (WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE USE_WINDOWS_API=1 )
    else()
        target_compile_definitions(${LIB_NAME} PRIVATE USE_WINDOWS_API=0 )
    endif()

    target_sources(${LIB_NAME} PUBLIC ${SND_HEADERS} PRIVATE ${SND_SOURCES})
    
    target_include_directories(${LIB_NAME} PUBLIC ${SND_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${SND_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SNDFILE=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_EXTERNAL_XIPH_LIBS=1 )

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-expansion-to-defined)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-parameter)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    
    target_link_libraries(${LIB_NAME} PUBLIC de_ogg)
    target_link_libraries(${LIB_NAME} PUBLIC de_flac)
    target_link_libraries(${LIB_NAME} PUBLIC de_opus)
    target_link_libraries(${LIB_NAME} PUBLIC de_theora)
    target_link_libraries(${LIB_NAME} PUBLIC de_vorbis)

endif()

# add_subdirectory(test)
