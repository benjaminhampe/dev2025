set(LIB_NAME de_webp)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/webp/decode.h
        ${LIB_HEADER_DIR}/webp/demux.h
        ${LIB_HEADER_DIR}/webp/encode.h
        ${LIB_HEADER_DIR}/webp/format_constants.h
        ${LIB_HEADER_DIR}/webp/mux.h
        ${LIB_HEADER_DIR}/webp/mux_types.h
        ${LIB_HEADER_DIR}/webp/types.h
        ${LIB_HEADER_DIR}/sharpyuv/sharpyuv.h
        ${LIB_HEADER_DIR}/sharpyuv/sharpyuv_cpu.h
        ${LIB_HEADER_DIR}/sharpyuv/sharpyuv_csp.h
        ${LIB_HEADER_DIR}/sharpyuv/sharpyuv_dsp.h
        ${LIB_HEADER_DIR}/sharpyuv/sharpyuv_gamma.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv.c
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_cpu.c
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_csp.c
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_dsp.c
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_gamma.c
        # ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_neon.c
        ${LIB_SOURCE_DIR}/sharpyuv/sharpyuv_sse2.c

        ${LIB_SOURCE_DIR}/dec/alphai_dec.h
        ${LIB_SOURCE_DIR}/dec/alpha_dec.c
        ${LIB_SOURCE_DIR}/dec/buffer_dec.c
        ${LIB_SOURCE_DIR}/dec/common_dec.h
        ${LIB_SOURCE_DIR}/dec/frame_dec.c
        ${LIB_SOURCE_DIR}/dec/idec_dec.c
        ${LIB_SOURCE_DIR}/dec/io_dec.c
        ${LIB_SOURCE_DIR}/dec/quant_dec.c
        ${LIB_SOURCE_DIR}/dec/tree_dec.c
        ${LIB_SOURCE_DIR}/dec/vp8i_dec.h
        ${LIB_SOURCE_DIR}/dec/vp8_dec.h
        ${LIB_SOURCE_DIR}/dec/vp8_dec.c
        ${LIB_SOURCE_DIR}/dec/vp8li_dec.h
        ${LIB_SOURCE_DIR}/dec/vp8l_dec.c
        ${LIB_SOURCE_DIR}/dec/webpi_dec.h
        ${LIB_SOURCE_DIR}/dec/webp_dec.c

        ${LIB_SOURCE_DIR}/demux/anim_decode.c
        ${LIB_SOURCE_DIR}/demux/demux.c

        ${LIB_SOURCE_DIR}/dsp/alpha_processing.c
        # ${LIB_SOURCE_DIR}/dsp/alpha_processing_mips_dsp_r2.c
        # ${LIB_SOURCE_DIR}/dsp/alpha_processing_neon.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_sse2.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_sse41.c
        ${LIB_SOURCE_DIR}/dsp/common_sse2.h
        ${LIB_SOURCE_DIR}/dsp/common_sse41.h
        ${LIB_SOURCE_DIR}/dsp/cost.c
        # ${LIB_SOURCE_DIR}/dsp/cost_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/cost_mips_dsp_r2.c
        # ${LIB_SOURCE_DIR}/dsp/cost_neon.c
        ${LIB_SOURCE_DIR}/dsp/cost_sse2.c
        ${LIB_SOURCE_DIR}/dsp/cpu.c
        ${LIB_SOURCE_DIR}/dsp/cpu.h
        ${LIB_SOURCE_DIR}/dsp/dec.c
        ${LIB_SOURCE_DIR}/dsp/dec_clip_tables.c
        # ${LIB_SOURCE_DIR}/dsp/dec_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/dec_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/dec_msa.c
        # ${LIB_SOURCE_DIR}/dsp/dec_neon.c
        ${LIB_SOURCE_DIR}/dsp/dec_sse2.c
        ${LIB_SOURCE_DIR}/dsp/dec_sse41.c
        ${LIB_SOURCE_DIR}/dsp/dsp.h
        ${LIB_SOURCE_DIR}/dsp/enc.c
        # ${LIB_SOURCE_DIR}/dsp/enc_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/enc_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/enc_msa.c
        # ${LIB_SOURCE_DIR}/dsp/enc_neon.c
        ${LIB_SOURCE_DIR}/dsp/enc_sse2.c
        ${LIB_SOURCE_DIR}/dsp/enc_sse41.c
        ${LIB_SOURCE_DIR}/dsp/filters.c
        # ${LIB_SOURCE_DIR}/dsp/filters_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/filters_msa.c
        # ${LIB_SOURCE_DIR}/dsp/filters_neon.c
        ${LIB_SOURCE_DIR}/dsp/filters_sse2.c
        ${LIB_SOURCE_DIR}/dsp/lossless.c
        ${LIB_SOURCE_DIR}/dsp/lossless.h
        ${LIB_SOURCE_DIR}/dsp/lossless_avx2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_common.h
        ${LIB_SOURCE_DIR}/dsp/lossless_enc.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_avx2.c
        # ${LIB_SOURCE_DIR}/dsp/lossless_enc_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/lossless_enc_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_msa.c
        # ${LIB_SOURCE_DIR}/dsp/lossless_enc_neon.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_sse2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_sse41.c
        # ${LIB_SOURCE_DIR}/dsp/lossless_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_msa.c
        # ${LIB_SOURCE_DIR}/dsp/lossless_neon.c
        ${LIB_SOURCE_DIR}/dsp/lossless_sse2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_sse41.c

        ${LIB_SOURCE_DIR}/dsp/mips_macro.h
        ${LIB_SOURCE_DIR}/dsp/msa_macro.h
        ${LIB_SOURCE_DIR}/dsp/neon.h
        ${LIB_SOURCE_DIR}/dsp/quant.h
        ${LIB_SOURCE_DIR}/dsp/rescaler.c
        # ${LIB_SOURCE_DIR}/dsp/rescaler_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/rescaler_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_msa.c
        # ${LIB_SOURCE_DIR}/dsp/rescaler_neon.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_sse2.c

        ${LIB_SOURCE_DIR}/dsp/ssim.c
        ${LIB_SOURCE_DIR}/dsp/ssim_sse2.c

        ${LIB_SOURCE_DIR}/dsp/upsampling.c
        # ${LIB_SOURCE_DIR}/dsp/upsampling_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_msa.c
        # ${LIB_SOURCE_DIR}/dsp/upsampling_neon.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_sse2.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_sse41.c

        ${LIB_SOURCE_DIR}/dsp/yuv.c
        ${LIB_SOURCE_DIR}/dsp/yuv.h
        # ${LIB_SOURCE_DIR}/dsp/yuv_mips32.c
        # ${LIB_SOURCE_DIR}/dsp/yuv_mips_dsp_r2.c
        # ${LIB_SOURCE_DIR}/dsp/yuv_neon.c
        ${LIB_SOURCE_DIR}/dsp/yuv_sse2.c
        ${LIB_SOURCE_DIR}/dsp/yuv_sse41.c

        ${LIB_SOURCE_DIR}/enc/alpha_enc.c
        ${LIB_SOURCE_DIR}/enc/analysis_enc.c
        ${LIB_SOURCE_DIR}/enc/backward_references_cost_enc.c
        ${LIB_SOURCE_DIR}/enc/backward_references_enc.c
        ${LIB_SOURCE_DIR}/enc/backward_references_enc.h
        ${LIB_SOURCE_DIR}/enc/config_enc.c
        ${LIB_SOURCE_DIR}/enc/cost_enc.c
        ${LIB_SOURCE_DIR}/enc/cost_enc.h
        ${LIB_SOURCE_DIR}/enc/filter_enc.c
        ${LIB_SOURCE_DIR}/enc/frame_enc.c
        ${LIB_SOURCE_DIR}/enc/histogram_enc.c
        ${LIB_SOURCE_DIR}/enc/histogram_enc.h
        ${LIB_SOURCE_DIR}/enc/iterator_enc.c
        ${LIB_SOURCE_DIR}/enc/near_lossless_enc.c
        ${LIB_SOURCE_DIR}/enc/picture_csp_enc.c
        ${LIB_SOURCE_DIR}/enc/picture_enc.c
        ${LIB_SOURCE_DIR}/enc/picture_psnr_enc.c
        ${LIB_SOURCE_DIR}/enc/picture_rescale_enc.c
        ${LIB_SOURCE_DIR}/enc/picture_tools_enc.c
        ${LIB_SOURCE_DIR}/enc/predictor_enc.c
        ${LIB_SOURCE_DIR}/enc/quant_enc.c
        ${LIB_SOURCE_DIR}/enc/syntax_enc.c
        ${LIB_SOURCE_DIR}/enc/token_enc.c
        ${LIB_SOURCE_DIR}/enc/tree_enc.c
        ${LIB_SOURCE_DIR}/enc/vp8i_enc.h
        ${LIB_SOURCE_DIR}/enc/vp8li_enc.h
        ${LIB_SOURCE_DIR}/enc/vp8l_enc.c
        ${LIB_SOURCE_DIR}/enc/webp_enc.c

        ${LIB_SOURCE_DIR}/mux/animi.h
        ${LIB_SOURCE_DIR}/mux/anim_encode.c
        ${LIB_SOURCE_DIR}/mux/muxedit.c
        ${LIB_SOURCE_DIR}/mux/muxi.h
        ${LIB_SOURCE_DIR}/mux/muxinternal.c
        ${LIB_SOURCE_DIR}/mux/muxread.c

        ${LIB_SOURCE_DIR}/utils/utils.c
        ${LIB_SOURCE_DIR}/utils/utils.h
        ${LIB_SOURCE_DIR}/utils/bit_reader_inl_utils.h
        ${LIB_SOURCE_DIR}/utils/bit_reader_utils.c
        ${LIB_SOURCE_DIR}/utils/bit_reader_utils.h
        ${LIB_SOURCE_DIR}/utils/bit_writer_utils.c
        ${LIB_SOURCE_DIR}/utils/bit_writer_utils.h
        ${LIB_SOURCE_DIR}/utils/color_cache_utils.c
        ${LIB_SOURCE_DIR}/utils/color_cache_utils.h
        ${LIB_SOURCE_DIR}/utils/endian_inl_utils.h
        ${LIB_SOURCE_DIR}/utils/filters_utils.c
        ${LIB_SOURCE_DIR}/utils/filters_utils.h
        ${LIB_SOURCE_DIR}/utils/huffman_encode_utils.c
        ${LIB_SOURCE_DIR}/utils/huffman_encode_utils.h
        ${LIB_SOURCE_DIR}/utils/huffman_utils.c
        ${LIB_SOURCE_DIR}/utils/huffman_utils.h
        ${LIB_SOURCE_DIR}/utils/palette.c
        ${LIB_SOURCE_DIR}/utils/palette.h
        ${LIB_SOURCE_DIR}/utils/quant_levels_dec_utils.c
        ${LIB_SOURCE_DIR}/utils/quant_levels_dec_utils.h
        ${LIB_SOURCE_DIR}/utils/quant_levels_utils.c
        ${LIB_SOURCE_DIR}/utils/quant_levels_utils.h
        ${LIB_SOURCE_DIR}/utils/random_utils.c
        ${LIB_SOURCE_DIR}/utils/random_utils.h
        ${LIB_SOURCE_DIR}/utils/rescaler_utils.c
        ${LIB_SOURCE_DIR}/utils/rescaler_utils.h
        ${LIB_SOURCE_DIR}/utils/thread_utils.c
        ${LIB_SOURCE_DIR}/utils/thread_utils.h
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/webp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

    # target_link_libraries(${LIB_NAME} PUBLIC de_cv_sharpyuv)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_WEBP=1)
	target_compile_definitions(${LIB_NAME} PUBLIC WEBP_USE_THREAD=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)



endif()

# add_subdirectory(test)
