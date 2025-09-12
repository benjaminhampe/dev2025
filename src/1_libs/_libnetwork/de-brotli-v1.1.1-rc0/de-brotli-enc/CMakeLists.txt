set(LIB_NAME de_brotlienc)

project(${LIB_NAME}-1.1.1-rc0)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/brotli/enc/backward_references.h
        ${LIB_HEADER_DIR}/brotli/enc/backward_references_hq.h
        ${LIB_HEADER_DIR}/brotli/enc/backward_references_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/bit_cost.h
        ${LIB_HEADER_DIR}/brotli/enc/bit_cost_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/block_encoder_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/block_splitter.h
        ${LIB_HEADER_DIR}/brotli/enc/block_splitter_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/brotli_bit_stream.h
        ${LIB_HEADER_DIR}/brotli/enc/cluster.h
        ${LIB_HEADER_DIR}/brotli/enc/cluster_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/command.h
        ${LIB_HEADER_DIR}/brotli/enc/compound_dictionary.h
        ${LIB_HEADER_DIR}/brotli/enc/compress_fragment.h
        ${LIB_HEADER_DIR}/brotli/enc/compress_fragment_two_pass.h
        ${LIB_HEADER_DIR}/brotli/enc/dictionary_hash.h
        ${LIB_HEADER_DIR}/brotli/enc/encoder_dict.h
        ${LIB_HEADER_DIR}/brotli/enc/entropy_encode.h
        ${LIB_HEADER_DIR}/brotli/enc/entropy_encode_static.h
        ${LIB_HEADER_DIR}/brotli/enc/fast_log.h
        ${LIB_HEADER_DIR}/brotli/enc/find_match_length.h
        ${LIB_HEADER_DIR}/brotli/enc/hash.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_composite_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_forgetful_chain_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_longest_match64_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_longest_match64_simd_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_longest_match_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_longest_match_quickly_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_longest_match_simd_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_rolling_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/hash_to_binary_tree_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/histogram.h
        ${LIB_HEADER_DIR}/brotli/enc/histogram_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/literal_cost.h
        ${LIB_HEADER_DIR}/brotli/enc/matching_tag_mask.h
        ${LIB_HEADER_DIR}/brotli/enc/memory.h
        ${LIB_HEADER_DIR}/brotli/enc/metablock.h
        ${LIB_HEADER_DIR}/brotli/enc/metablock_inc.h
        ${LIB_HEADER_DIR}/brotli/enc/params.h
        ${LIB_HEADER_DIR}/brotli/enc/prefix.h
        ${LIB_HEADER_DIR}/brotli/enc/quality.h
        ${LIB_HEADER_DIR}/brotli/enc/ringbuffer.h
        ${LIB_HEADER_DIR}/brotli/enc/state.h
        ${LIB_HEADER_DIR}/brotli/enc/static_dict.h
        ${LIB_HEADER_DIR}/brotli/enc/static_dict_lut.h
        ${LIB_HEADER_DIR}/brotli/enc/utf8_util.h
        ${LIB_HEADER_DIR}/brotli/enc/write_bits.h		
    )
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/enc/backward_references.c
        ${LIB_SOURCE_DIR}/enc/backward_references_hq.c
        ${LIB_SOURCE_DIR}/enc/bit_cost.c
        ${LIB_SOURCE_DIR}/enc/block_splitter.c
        ${LIB_SOURCE_DIR}/enc/brotli_bit_stream.c
        ${LIB_SOURCE_DIR}/enc/cluster.c
        ${LIB_SOURCE_DIR}/enc/command.c
        ${LIB_SOURCE_DIR}/enc/compound_dictionary.c
        ${LIB_SOURCE_DIR}/enc/compress_fragment.c
        ${LIB_SOURCE_DIR}/enc/compress_fragment_two_pass.c
        ${LIB_SOURCE_DIR}/enc/dictionary_hash.c
        ${LIB_SOURCE_DIR}/enc/encode.c
        ${LIB_SOURCE_DIR}/enc/encoder_dict.c
        ${LIB_SOURCE_DIR}/enc/entropy_encode.c
        ${LIB_SOURCE_DIR}/enc/fast_log.c
        ${LIB_SOURCE_DIR}/enc/histogram.c
        ${LIB_SOURCE_DIR}/enc/literal_cost.c
        ${LIB_SOURCE_DIR}/enc/memory.c
        ${LIB_SOURCE_DIR}/enc/metablock.c
        ${LIB_SOURCE_DIR}/enc/static_dict.c
        ${LIB_SOURCE_DIR}/enc/utf8_util.c
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/brotli/enc)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_BROTLIENC=1)
	
	target_link_libraries(${LIB_NAME} PUBLIC de_brotlicommon)

endif()

# add_subdirectory(test)
