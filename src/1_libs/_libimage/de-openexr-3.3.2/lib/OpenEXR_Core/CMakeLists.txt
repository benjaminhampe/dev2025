set(LIB_NAME de_OpenEXR_Core)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/openexr.h
		${LIB_HEADER_DIR}/openexr_attr.h
		${LIB_HEADER_DIR}/openexr_base.h
		${LIB_HEADER_DIR}/openexr_chunkio.h
		${LIB_HEADER_DIR}/openexr_coding.h
		${LIB_HEADER_DIR}/openexr_compression.h
		${LIB_HEADER_DIR}/openexr_config.h
		${LIB_HEADER_DIR}/openexr_context.h
		${LIB_HEADER_DIR}/openexr_debug.h
		${LIB_HEADER_DIR}/openexr_decode.h
		${LIB_HEADER_DIR}/openexr_encode.h
		${LIB_HEADER_DIR}/openexr_errors.h
		${LIB_HEADER_DIR}/openexr_part.h
		${LIB_HEADER_DIR}/openexr_std_attr.h
		${LIB_HEADER_DIR}/openexr_version.h
	)
	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/backward_compatibility.h
		${LIB_SOURCE_DIR}/dwaLookups.h
		${LIB_SOURCE_DIR}/internal_attr.h
		${LIB_SOURCE_DIR}/internal_channel_list.h
		${LIB_SOURCE_DIR}/internal_coding.h
		${LIB_SOURCE_DIR}/internal_compress.h
		${LIB_SOURCE_DIR}/internal_constants.h
		${LIB_SOURCE_DIR}/internal_cpuid.h
		${LIB_SOURCE_DIR}/internal_decompress.h
		${LIB_SOURCE_DIR}/internal_dwa_channeldata.h
		${LIB_SOURCE_DIR}/internal_dwa_classifier.h
		${LIB_SOURCE_DIR}/internal_dwa_compressor.h
		${LIB_SOURCE_DIR}/internal_dwa_decoder.h
		${LIB_SOURCE_DIR}/internal_dwa_encoder.h
		${LIB_SOURCE_DIR}/internal_dwa_helpers.h
		${LIB_SOURCE_DIR}/internal_dwa_simd.h
		${LIB_SOURCE_DIR}/internal_file.h
		${LIB_SOURCE_DIR}/internal_float_vector.h
		${LIB_SOURCE_DIR}/internal_huf.h
		${LIB_SOURCE_DIR}/internal_memory.h
		${LIB_SOURCE_DIR}/internal_opaque.h
		${LIB_SOURCE_DIR}/internal_posix_file_impl.h
		${LIB_SOURCE_DIR}/internal_preview.h
		${LIB_SOURCE_DIR}/internal_string.h
		${LIB_SOURCE_DIR}/internal_string_vector.h
		${LIB_SOURCE_DIR}/internal_structs.h
		${LIB_SOURCE_DIR}/internal_util.h
		${LIB_SOURCE_DIR}/internal_win32_file_impl.h
		${LIB_SOURCE_DIR}/internal_xdr.h
        
		${LIB_SOURCE_DIR}/attributes.c
		${LIB_SOURCE_DIR}/base.c
		${LIB_SOURCE_DIR}/channel_list.c
		${LIB_SOURCE_DIR}/chunk.c
		${LIB_SOURCE_DIR}/coding.c
		${LIB_SOURCE_DIR}/compression.c
		${LIB_SOURCE_DIR}/context.c
		${LIB_SOURCE_DIR}/debug.c
		${LIB_SOURCE_DIR}/decoding.c
		${LIB_SOURCE_DIR}/encoding.c
		${LIB_SOURCE_DIR}/float_vector.c
		${LIB_SOURCE_DIR}/internal_b44.c
		${LIB_SOURCE_DIR}/internal_b44_table.c
		${LIB_SOURCE_DIR}/internal_dwa.c
		${LIB_SOURCE_DIR}/internal_huf.c
		${LIB_SOURCE_DIR}/internal_piz.c
		${LIB_SOURCE_DIR}/internal_pxr24.c
		${LIB_SOURCE_DIR}/internal_rle.c
		${LIB_SOURCE_DIR}/internal_structs.c
		${LIB_SOURCE_DIR}/internal_zip.c
		${LIB_SOURCE_DIR}/memory.c
		${LIB_SOURCE_DIR}/opaque.c
		${LIB_SOURCE_DIR}/pack.c
		${LIB_SOURCE_DIR}/parse_header.c
		${LIB_SOURCE_DIR}/part.c
		${LIB_SOURCE_DIR}/part_attr.c
		${LIB_SOURCE_DIR}/preview.c
		${LIB_SOURCE_DIR}/std_attr.c
		${LIB_SOURCE_DIR}/string.c
		${LIB_SOURCE_DIR}/string_vector.c
		${LIB_SOURCE_DIR}/unpack.c
		${LIB_SOURCE_DIR}/validation.c
		${LIB_SOURCE_DIR}/write_header.c
	)

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_BUILD_LIBS=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
        target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-conversion)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-int-conversion)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-used-but-marked-unused)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-shorten-64-to-32)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-padded)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-cast-qual)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-disabled-macro-expansion)
    endif()

	target_link_libraries(${LIB_NAME} PUBLIC de_deflate)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Imath)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_IlmThread)

    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
