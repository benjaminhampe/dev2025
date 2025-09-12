set(LIB_NAME de_webp) # We dont like to clash with system libs.
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/webp)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/types.h
        ${LIB_HEADER_DIR}/decode.h
        ${LIB_HEADER_DIR}/encode.h
        ${LIB_SOURCE_DIR}/webp/config.h
        ${LIB_SOURCE_DIR}/webp/format_constants.h
        ${LIB_SOURCE_DIR}/webp/demux.h
        ${LIB_SOURCE_DIR}/webp/mux_types.h
        ${LIB_SOURCE_DIR}/webp/mux.h
    )

    set(LIB_DECODER_SOURCES
        ${LIB_SOURCE_DIR}/dec/alpha_dec.c
        ${LIB_SOURCE_DIR}/dec/alphai_dec.h
        ${LIB_SOURCE_DIR}/dec/buffer_dec.c
        ${LIB_SOURCE_DIR}/dec/common_dec.h
        ${LIB_SOURCE_DIR}/dec/frame_dec.c
        ${LIB_SOURCE_DIR}/dec/idec_dec.c
        ${LIB_SOURCE_DIR}/dec/io_dec.c
        # ${LIB_SOURCE_DIR}/dec/Makefile.am
        ${LIB_SOURCE_DIR}/dec/quant_dec.c
        ${LIB_SOURCE_DIR}/dec/tree_dec.c
        ${LIB_SOURCE_DIR}/dec/vp8_dec.c
        ${LIB_SOURCE_DIR}/dec/vp8_dec.h
        ${LIB_SOURCE_DIR}/dec/vp8i_dec.h
        ${LIB_SOURCE_DIR}/dec/vp8l_dec.c
        ${LIB_SOURCE_DIR}/dec/vp8li_dec.h
        ${LIB_SOURCE_DIR}/dec/webp_dec.c
        ${LIB_SOURCE_DIR}/dec/webpi_dec.h
    )

    #    add_library(de_webp_decoder STATIC "")
    #    target_sources(de_webp_decoder PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_DECODER_SOURCES})
    #    target_include_directories(de_webp_decoder PRIVATE ${LIB_SOURCE_DIR}/..)

    set(LIB_UTILS_SOURCES
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
        # ${LIB_SOURCE_DIR}/utils/Makefile.am
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
        ${LIB_SOURCE_DIR}/utils/utils.c
        ${LIB_SOURCE_DIR}/utils/utils.h
    )

    #    add_library(de_webp_utils STATIC "")
    #    target_sources(de_webp_utils PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_UTILS_SOURCES})
    #    target_include_directories(de_webp_utils PRIVATE ${LIB_SOURCE_DIR}/..)
    #    
    set(LIB_ENCODER_SOURCES
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
        ${LIB_SOURCE_DIR}/enc/Makefile.am
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
        ${LIB_SOURCE_DIR}/enc/vp8l_enc.c
        ${LIB_SOURCE_DIR}/enc/vp8li_enc.h
        ${LIB_SOURCE_DIR}/enc/webp_enc.c
    )
    # 
    #    add_library(de_webp_encoder STATIC "")
    #    target_sources(de_webp_encoder PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_ENCODER_SOURCES})
    #    target_include_directories(de_webp_encoder PRIVATE ${LIB_SOURCE_DIR}/..)
    #    
    set(LIB_DSP_SOURCES      
        ${LIB_SOURCE_DIR}/dsp/alpha_processing.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_neon.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_sse2.c
        ${LIB_SOURCE_DIR}/dsp/alpha_processing_sse41.c
        ${LIB_SOURCE_DIR}/dsp/common_sse2.h
        ${LIB_SOURCE_DIR}/dsp/common_sse41.h
        ${LIB_SOURCE_DIR}/dsp/cost.c
        ${LIB_SOURCE_DIR}/dsp/cost_mips32.c
        ${LIB_SOURCE_DIR}/dsp/cost_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/cost_neon.c
        ${LIB_SOURCE_DIR}/dsp/cost_sse2.c
        ${LIB_SOURCE_DIR}/dsp/cpu.c
        ${LIB_SOURCE_DIR}/dsp/dec.c
        ${LIB_SOURCE_DIR}/dsp/dec_clip_tables.c
        ${LIB_SOURCE_DIR}/dsp/dec_mips32.c
        ${LIB_SOURCE_DIR}/dsp/dec_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/dec_msa.c
        ${LIB_SOURCE_DIR}/dsp/dec_neon.c
        ${LIB_SOURCE_DIR}/dsp/dec_sse2.c
        ${LIB_SOURCE_DIR}/dsp/dec_sse41.c
        ${LIB_SOURCE_DIR}/dsp/dsp.h
        ${LIB_SOURCE_DIR}/dsp/enc.c
        ${LIB_SOURCE_DIR}/dsp/enc_mips32.c
        ${LIB_SOURCE_DIR}/dsp/enc_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/enc_msa.c
        ${LIB_SOURCE_DIR}/dsp/enc_neon.c
        ${LIB_SOURCE_DIR}/dsp/enc_sse2.c
        ${LIB_SOURCE_DIR}/dsp/enc_sse41.c
        ${LIB_SOURCE_DIR}/dsp/filters.c
        ${LIB_SOURCE_DIR}/dsp/filters_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/filters_msa.c
        ${LIB_SOURCE_DIR}/dsp/filters_neon.c
        ${LIB_SOURCE_DIR}/dsp/filters_sse2.c
        ${LIB_SOURCE_DIR}/dsp/lossless.c
        ${LIB_SOURCE_DIR}/dsp/lossless_common.h
        ${LIB_SOURCE_DIR}/dsp/lossless_enc.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_mips32.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_msa.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_neon.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_sse2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_enc_sse41.c
        ${LIB_SOURCE_DIR}/dsp/lossless.h
        ${LIB_SOURCE_DIR}/dsp/lossless_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/lossless_msa.c
        ${LIB_SOURCE_DIR}/dsp/lossless_neon.c
        ${LIB_SOURCE_DIR}/dsp/lossless_sse2.c
        ${LIB_SOURCE_DIR}/dsp/Makefile.am
        ${LIB_SOURCE_DIR}/dsp/mips_macro.h
        ${LIB_SOURCE_DIR}/dsp/msa_macro.h
        ${LIB_SOURCE_DIR}/dsp/neon.h
        ${LIB_SOURCE_DIR}/dsp/quant.h
        ${LIB_SOURCE_DIR}/dsp/rescaler.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_mips32.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_msa.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_neon.c
        ${LIB_SOURCE_DIR}/dsp/rescaler_sse2.c
        ${LIB_SOURCE_DIR}/dsp/ssim.c
        ${LIB_SOURCE_DIR}/dsp/ssim_sse2.c
        ${LIB_SOURCE_DIR}/dsp/upsampling.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_msa.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_neon.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_sse2.c
        ${LIB_SOURCE_DIR}/dsp/upsampling_sse41.c
        ${LIB_SOURCE_DIR}/dsp/yuv.c
        ${LIB_SOURCE_DIR}/dsp/yuv.h
        ${LIB_SOURCE_DIR}/dsp/yuv_mips32.c
        ${LIB_SOURCE_DIR}/dsp/yuv_mips_dsp_r2.c
        ${LIB_SOURCE_DIR}/dsp/yuv_neon.c
        ${LIB_SOURCE_DIR}/dsp/yuv_sse2.c
        ${LIB_SOURCE_DIR}/dsp/yuv_sse41.c   
    )
    # 
    #    add_library(de_webp_dsp STATIC "")
    #    target_sources(de_webp_dsp PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_DSP_SOURCES})
    #    target_include_directories(de_webp_dsp PRIVATE ${LIB_SOURCE_DIR}/..)
    #    
    set(LIB_DEMUX_SOURCES      
        ${LIB_SOURCE_DIR}/demux/anim_decode.c
        ${LIB_SOURCE_DIR}/demux/demux.c
    )
    # 
    #    add_library(de_webp_demux STATIC "")
    #    target_sources(de_webp_demux PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_DEMUX_SOURCES})
    #    target_include_directories(de_webp_demux PRIVATE ${LIB_SOURCE_DIR}/..)
    #    
    set(LIB_MUX_SOURCES      
        ${LIB_SOURCE_DIR}/mux/anim_encode.c
        ${LIB_SOURCE_DIR}/mux/muxedit.c
        ${LIB_SOURCE_DIR}/mux/muxinternal.c
        ${LIB_SOURCE_DIR}/mux/muxread.c
        ${LIB_SOURCE_DIR}/mux/animi.h
        ${LIB_SOURCE_DIR}/mux/muxi.h
    )
    # 
    #    add_library(de_webp_mux STATIC "")
    #    target_sources(de_webp_mux PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_MUX_SOURCES})
    #    target_include_directories(de_webp_mux PRIVATE ${LIB_SOURCE_DIR}/..)

    set(LIB_IMAGEIO_SOURCES      
        ${LIB_SOURCE_DIR}/imageio/image_dec.c
        ${LIB_SOURCE_DIR}/imageio/image_dec.h
        ${LIB_SOURCE_DIR}/imageio/image_enc.c
        ${LIB_SOURCE_DIR}/imageio/image_enc.h
        ${LIB_SOURCE_DIR}/imageio/imageio_util.c
        ${LIB_SOURCE_DIR}/imageio/imageio_util.h
        ${LIB_SOURCE_DIR}/imageio/jpegdec.c
        ${LIB_SOURCE_DIR}/imageio/jpegdec.h
        # ${LIB_SOURCE_DIR}/imageio/Makefile.am
        ${LIB_SOURCE_DIR}/imageio/metadata.c
        ${LIB_SOURCE_DIR}/imageio/metadata.h
        ${LIB_SOURCE_DIR}/imageio/pngdec.c
        ${LIB_SOURCE_DIR}/imageio/pngdec.h
        ${LIB_SOURCE_DIR}/imageio/pnmdec.c
        ${LIB_SOURCE_DIR}/imageio/pnmdec.h
        ${LIB_SOURCE_DIR}/imageio/tiffdec.c
        ${LIB_SOURCE_DIR}/imageio/tiffdec.h
        ${LIB_SOURCE_DIR}/imageio/webpdec.c
        ${LIB_SOURCE_DIR}/imageio/webpdec.h
        ${LIB_SOURCE_DIR}/imageio/wicdec.c
        ${LIB_SOURCE_DIR}/imageio/wicdec.h
    )
    # 
    #    add_library(de_webp_imageio STATIC "")
    #    target_sources(de_webp_imageio PUBLIC ${LIB_HEADERS} ${LIB_PRIVATE_HEADERS} ${LIB_IMAGEIO_SOURCES})
    #    
    # if (WIN32)
    # enable_language(RC)
    # list(APPEND LIB_SOURCES 
    # ${LIB_SOURCE_DIR}/common/win32thread.c
    # ${LIB_SOURCE_DIR}/common/win32thread.h )
    # endif ()

    if (BUILD_SHARED)
		message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} SHARED "")
    else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
    endif()

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_WEBP=1)

    # target_compile_options(${LIB_NAME} PRIVATE -O3)
    target_sources(${LIB_NAME} 
        PUBLIC 
        ${LIB_HEADERS} 
        PRIVATE 
        ${LIB_SOURCES}
        ${LIB_DECODER_SOURCES}
        ${LIB_ENCODER_SOURCES}
        ${LIB_UTILS_SOURCES}
        ${LIB_DSP_SOURCES}
        ${LIB_DEMUX_SOURCES}
        ${LIB_MUX_SOURCES}
        ${LIB_IMAGEIO_SOURCES}
    )

    #   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})   

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough )
    endif()
    
    #    target_link_libraries(${LIB_NAME} PRIVATE de_webp_imageio
    #                                              de_webp_demux 
    #                                              de_webp_mux 
    #                                              de_webp_dsp
    #                                              de_webp_utils 
    #                                              de_webp_encoder 
    #                                              de_webp_decoder )

endif()


# add_subdirectory(test)
