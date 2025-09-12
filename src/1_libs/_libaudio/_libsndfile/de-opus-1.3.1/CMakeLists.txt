set(LIB_NAME de_opus) # We dont like to clash with system libs.
project(${LIB_NAME}-1.3.1-dev C ASM_NASM)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opus/opus.h
        ${LIB_HEADER_DIR}/opus/opus_custom.h
        ${LIB_HEADER_DIR}/opus/opus_defines.h
        ${LIB_HEADER_DIR}/opus/opus_multistream.h
        ${LIB_HEADER_DIR}/opus/opus_projection.h
        ${LIB_HEADER_DIR}/opus/opus_types.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/config.h
        ${LIB_SOURCE_DIR}/analysis.c
        ${LIB_SOURCE_DIR}/analysis.h
        ${LIB_SOURCE_DIR}/mapping_matrix.c
        ${LIB_SOURCE_DIR}/mapping_matrix.h
        ${LIB_SOURCE_DIR}/mlp.c
        ${LIB_SOURCE_DIR}/mlp.h
        ${LIB_SOURCE_DIR}/mlp_data.c
        ${LIB_SOURCE_DIR}/opus.c
        # ${LIB_SOURCE_DIR}/opus_compare.c # app
        ${LIB_SOURCE_DIR}/opus_decoder.c
        # ${LIB_SOURCE_DIR}/opus_demo.c # app
        ${LIB_SOURCE_DIR}/opus_encoder.c
        ${LIB_SOURCE_DIR}/opus_multistream.c
        ${LIB_SOURCE_DIR}/opus_multistream_decoder.c
        ${LIB_SOURCE_DIR}/opus_multistream_encoder.c
        ${LIB_SOURCE_DIR}/opus_private.h
        ${LIB_SOURCE_DIR}/opus_projection_decoder.c
        ${LIB_SOURCE_DIR}/opus_projection_encoder.c
        ${LIB_SOURCE_DIR}/repacketizer.c
        # ${LIB_SOURCE_DIR}/repacketizer_demo.c # app
        ${LIB_SOURCE_DIR}/tansig_table.h

        ${LIB_SOURCE_DIR}/celt/celt.h
        ${LIB_SOURCE_DIR}/celt/arch.h
        ${LIB_SOURCE_DIR}/celt/entcode.h
        ${LIB_SOURCE_DIR}/celt/entdec.h
        ${LIB_SOURCE_DIR}/celt/entenc.h
        ${LIB_SOURCE_DIR}/celt/ecintrin.h		
        ${LIB_SOURCE_DIR}/celt/bands.c
        ${LIB_SOURCE_DIR}/celt/bands.h
        ${LIB_SOURCE_DIR}/celt/celt.c
        ${LIB_SOURCE_DIR}/celt/celt_decoder.c
        ${LIB_SOURCE_DIR}/celt/celt_encoder.c
        ${LIB_SOURCE_DIR}/celt/celt_lpc.c
        ${LIB_SOURCE_DIR}/celt/celt_lpc.h
        ${LIB_SOURCE_DIR}/celt/cpu_support.h
        ${LIB_SOURCE_DIR}/celt/cwrs.c
        ${LIB_SOURCE_DIR}/celt/cwrs.h
        ${LIB_SOURCE_DIR}/celt/entcode.c
        ${LIB_SOURCE_DIR}/celt/entdec.c
        ${LIB_SOURCE_DIR}/celt/entenc.c
        ${LIB_SOURCE_DIR}/celt/fixed_debug.h
        ${LIB_SOURCE_DIR}/celt/fixed_generic.h
        ${LIB_SOURCE_DIR}/celt/float_cast.h
        ${LIB_SOURCE_DIR}/celt/kiss_fft.c
        ${LIB_SOURCE_DIR}/celt/kiss_fft.h
        ${LIB_SOURCE_DIR}/celt/laplace.c
        ${LIB_SOURCE_DIR}/celt/laplace.h
        ${LIB_SOURCE_DIR}/celt/mathops.c
        ${LIB_SOURCE_DIR}/celt/mathops.h
        ${LIB_SOURCE_DIR}/celt/mdct.c
        ${LIB_SOURCE_DIR}/celt/mdct.h
        ${LIB_SOURCE_DIR}/celt/mfrngcod.h
        ${LIB_SOURCE_DIR}/celt/modes.c
        ${LIB_SOURCE_DIR}/celt/modes.h
        # ${LIB_SOURCE_DIR}/celt/opus_custom_demo.c # app
        ${LIB_SOURCE_DIR}/celt/os_support.h
        ${LIB_SOURCE_DIR}/celt/pitch.c
        ${LIB_SOURCE_DIR}/celt/pitch.h
        ${LIB_SOURCE_DIR}/celt/quant_bands.c
        ${LIB_SOURCE_DIR}/celt/quant_bands.h
        ${LIB_SOURCE_DIR}/celt/rate.c
        ${LIB_SOURCE_DIR}/celt/rate.h
        ${LIB_SOURCE_DIR}/celt/stack_alloc.h
        ${LIB_SOURCE_DIR}/celt/static_modes_fixed.h
        ${LIB_SOURCE_DIR}/celt/static_modes_fixed_arm_ne10.h
        ${LIB_SOURCE_DIR}/celt/static_modes_float.h
        ${LIB_SOURCE_DIR}/celt/static_modes_float_arm_ne10.h
        ${LIB_SOURCE_DIR}/celt/vq.c
        ${LIB_SOURCE_DIR}/celt/vq.h

        # ${LIB_SOURCE_DIR}/silk/
        ${LIB_SOURCE_DIR}/silk/A2NLSF.c
        ${LIB_SOURCE_DIR}/silk/ana_filt_bank_1.c
        ${LIB_SOURCE_DIR}/silk/API.h
        # ${LIB_SOURCE_DIR}/silk/arm/
        ${LIB_SOURCE_DIR}/silk/biquad_alt.c
        ${LIB_SOURCE_DIR}/silk/bwexpander.c
        ${LIB_SOURCE_DIR}/silk/bwexpander_32.c
        ${LIB_SOURCE_DIR}/silk/check_control_input.c
        ${LIB_SOURCE_DIR}/silk/CNG.c
        ${LIB_SOURCE_DIR}/silk/code_signs.c
        ${LIB_SOURCE_DIR}/silk/control.h
        ${LIB_SOURCE_DIR}/silk/control_audio_bandwidth.c
        ${LIB_SOURCE_DIR}/silk/control_codec.c
        ${LIB_SOURCE_DIR}/silk/control_SNR.c
        ${LIB_SOURCE_DIR}/silk/debug.c
        ${LIB_SOURCE_DIR}/silk/debug.h
        ${LIB_SOURCE_DIR}/silk/dec_API.c
        ${LIB_SOURCE_DIR}/silk/decode_core.c
        ${LIB_SOURCE_DIR}/silk/decode_frame.c
        ${LIB_SOURCE_DIR}/silk/decode_indices.c
        ${LIB_SOURCE_DIR}/silk/decode_parameters.c
        ${LIB_SOURCE_DIR}/silk/decode_pitch.c
        ${LIB_SOURCE_DIR}/silk/decode_pulses.c
        ${LIB_SOURCE_DIR}/silk/decoder_set_fs.c
        ${LIB_SOURCE_DIR}/silk/define.h
        ${LIB_SOURCE_DIR}/silk/enc_API.c
        ${LIB_SOURCE_DIR}/silk/encode_indices.c
        ${LIB_SOURCE_DIR}/silk/encode_pulses.c
        ${LIB_SOURCE_DIR}/silk/errors.h
        ${LIB_SOURCE_DIR}/silk/gain_quant.c
        ${LIB_SOURCE_DIR}/silk/HP_variable_cutoff.c
        ${LIB_SOURCE_DIR}/silk/init_decoder.c
        ${LIB_SOURCE_DIR}/silk/init_encoder.c
        ${LIB_SOURCE_DIR}/silk/Inlines.h
        ${LIB_SOURCE_DIR}/silk/inner_prod_aligned.c
        ${LIB_SOURCE_DIR}/silk/interpolate.c
        ${LIB_SOURCE_DIR}/silk/lin2log.c
        ${LIB_SOURCE_DIR}/silk/log2lin.c
        ${LIB_SOURCE_DIR}/silk/LP_variable_cutoff.c
        ${LIB_SOURCE_DIR}/silk/LPC_analysis_filter.c
        ${LIB_SOURCE_DIR}/silk/LPC_fit.c
        ${LIB_SOURCE_DIR}/silk/LPC_inv_pred_gain.c
        ${LIB_SOURCE_DIR}/silk/MacroCount.h
        ${LIB_SOURCE_DIR}/silk/MacroDebug.h
        ${LIB_SOURCE_DIR}/silk/macros.h
        ${LIB_SOURCE_DIR}/silk/silk_main.h
        # ${LIB_SOURCE_DIR}/silk/mips/
        ${LIB_SOURCE_DIR}/silk/NLSF_decode.c
        ${LIB_SOURCE_DIR}/silk/NLSF_del_dec_quant.c
        ${LIB_SOURCE_DIR}/silk/NLSF_encode.c
        ${LIB_SOURCE_DIR}/silk/NLSF_stabilize.c
        ${LIB_SOURCE_DIR}/silk/NLSF_unpack.c
        ${LIB_SOURCE_DIR}/silk/NLSF_VQ.c
        ${LIB_SOURCE_DIR}/silk/NLSF_VQ_weights_laroia.c
        ${LIB_SOURCE_DIR}/silk/NLSF2A.c
        ${LIB_SOURCE_DIR}/silk/NSQ.c
        ${LIB_SOURCE_DIR}/silk/NSQ.h
        ${LIB_SOURCE_DIR}/silk/NSQ_del_dec.c
        ${LIB_SOURCE_DIR}/silk/pitch_est_defines.h
        ${LIB_SOURCE_DIR}/silk/pitch_est_tables.c
        ${LIB_SOURCE_DIR}/silk/PLC.c
        ${LIB_SOURCE_DIR}/silk/PLC.h
        ${LIB_SOURCE_DIR}/silk/process_NLSFs.c
        ${LIB_SOURCE_DIR}/silk/quant_LTP_gains.c
        ${LIB_SOURCE_DIR}/silk/resampler.c
        ${LIB_SOURCE_DIR}/silk/resampler_down2.c
        ${LIB_SOURCE_DIR}/silk/resampler_down2_3.c
        ${LIB_SOURCE_DIR}/silk/resampler_private.h
        ${LIB_SOURCE_DIR}/silk/resampler_private_AR2.c
        ${LIB_SOURCE_DIR}/silk/resampler_private_down_FIR.c
        ${LIB_SOURCE_DIR}/silk/resampler_private_IIR_FIR.c
        ${LIB_SOURCE_DIR}/silk/resampler_private_up2_HQ.c
        ${LIB_SOURCE_DIR}/silk/resampler_rom.c
        ${LIB_SOURCE_DIR}/silk/resampler_rom.h
        ${LIB_SOURCE_DIR}/silk/resampler_structs.h
        ${LIB_SOURCE_DIR}/silk/shell_coder.c
        ${LIB_SOURCE_DIR}/silk/sigm_Q15.c
        ${LIB_SOURCE_DIR}/silk/SigProc_FIX.h
        ${LIB_SOURCE_DIR}/silk/sort.c
        ${LIB_SOURCE_DIR}/silk/stereo_decode_pred.c
        ${LIB_SOURCE_DIR}/silk/stereo_encode_pred.c
        ${LIB_SOURCE_DIR}/silk/stereo_find_predictor.c
        ${LIB_SOURCE_DIR}/silk/stereo_LR_to_MS.c
        ${LIB_SOURCE_DIR}/silk/stereo_MS_to_LR.c
        ${LIB_SOURCE_DIR}/silk/stereo_quant_pred.c
        ${LIB_SOURCE_DIR}/silk/structs.h
        ${LIB_SOURCE_DIR}/silk/sum_sqr_shift.c
        ${LIB_SOURCE_DIR}/silk/table_LSF_cos.c
        ${LIB_SOURCE_DIR}/silk/tables.h
        ${LIB_SOURCE_DIR}/silk/tables_gain.c
        ${LIB_SOURCE_DIR}/silk/tables_LTP.c
        ${LIB_SOURCE_DIR}/silk/tables_NLSF_CB_NB_MB.c
        ${LIB_SOURCE_DIR}/silk/tables_NLSF_CB_WB.c
        ${LIB_SOURCE_DIR}/silk/tables_other.c
        ${LIB_SOURCE_DIR}/silk/tables_pitch_lag.c
        ${LIB_SOURCE_DIR}/silk/tables_pulses_per_block.c
        # silk/tests/
        ${LIB_SOURCE_DIR}/silk/tuning_parameters.h
        ${LIB_SOURCE_DIR}/silk/typedef.h
        ${LIB_SOURCE_DIR}/silk/VAD.c
        ${LIB_SOURCE_DIR}/silk/VQ_WMat_EC.c
        # silk/fixed/
        # ${LIB_SOURCE_DIR}/silk/fixed/apply_sine_window_FIX.c
        # silk/fixed/arm/
        # ${LIB_SOURCE_DIR}/silk/fixed/autocorr_FIX.c
        # # ${LIB_SOURCE_DIR}/silk/fixed/burg_modified_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/corrMatrix_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/encode_frame_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/find_LPC_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/find_LTP_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/find_pitch_lags_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/find_pred_coefs_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/k2a_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/k2a_Q16_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/LTP_analysis_filter_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/LTP_scale_ctrl_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/main_FIX.h
        # # silk/fixed/mips/
        # ${LIB_SOURCE_DIR}/silk/fixed/noise_shape_analysis_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/pitch_analysis_core_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/process_gains_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/regularize_correlations_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/residual_energy_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/residual_energy16_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/schur_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/schur64_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/structs_FIX.h
        # ${LIB_SOURCE_DIR}/silk/fixed/vector_ops_FIX.c
        # ${LIB_SOURCE_DIR}/silk/fixed/warped_autocorrelation_FIX.c
        #${LIB_SOURCE_DIR}/silk/float/
        ${LIB_SOURCE_DIR}/silk/float/apply_sine_window_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/autocorrelation_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/burg_modified_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/bwexpander_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/corrMatrix_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/encode_frame_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/energy_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/find_LPC_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/find_LTP_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/find_pitch_lags_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/find_pred_coefs_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/inner_product_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/k2a_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/LPC_analysis_filter_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/LPC_inv_pred_gain_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/LTP_analysis_filter_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/LTP_scale_ctrl_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/main_FLP.h
        ${LIB_SOURCE_DIR}/silk/float/noise_shape_analysis_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/pitch_analysis_core_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/process_gains_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/regularize_correlations_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/residual_energy_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/scale_copy_vector_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/scale_vector_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/schur_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/SigProc_FLP.h
        ${LIB_SOURCE_DIR}/silk/float/sort_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/structs_FLP.h
        ${LIB_SOURCE_DIR}/silk/float/warped_autocorrelation_FLP.c
        ${LIB_SOURCE_DIR}/silk/float/wrappers_FLP.c
    )

    list(APPEND LIB_SOURCES
        ${LIB_SOURCE_DIR}/silk/x86/main_sse.h
        # ${LIB_SOURCE_DIR}/silk/x86/NSQ_del_dec_sse4_1.c
        # ${LIB_SOURCE_DIR}/silk/x86/NSQ_sse4_1.c
        ${LIB_SOURCE_DIR}/silk/x86/SigProc_FIX_sse.h
        # ${LIB_SOURCE_DIR}/silk/x86/VAD_sse4_1.c
        # ${LIB_SOURCE_DIR}/silk/x86/VQ_WMat_EC_sse4_1.c
        ${LIB_SOURCE_DIR}/silk/x86/x86_silk_map.c
        # ${LIB_SOURCE_DIR}/silk/fixed/x86/burg_modified_FIX_sse4_1.c
        # ${LIB_SOURCE_DIR}/silk/fixed/x86/vector_ops_FIX_sse4_1.c
    )

    list(APPEND LIB_SOURCES
        ${LIB_SOURCE_DIR}/celt/x86/x86_celt_map.c
        ${LIB_SOURCE_DIR}/celt/x86/x86cpu.c
        ${LIB_SOURCE_DIR}/celt/x86/x86cpu.h
        ${LIB_SOURCE_DIR}/celt/x86/vq_sse.h
        ${LIB_SOURCE_DIR}/celt/x86/vq_sse2.c
        ${LIB_SOURCE_DIR}/celt/x86/pitch_sse.c
        ${LIB_SOURCE_DIR}/celt/x86/pitch_sse.h
        # ${LIB_SOURCE_DIR}/celt/x86/celt_lpc_sse.h
        # ${LIB_SOURCE_DIR}/celt/x86/celt_lpc_sse4_1.c
        ${LIB_SOURCE_DIR}/celt/x86/pitch_sse2.c
        # ${LIB_SOURCE_DIR}/celt/x86/pitch_sse4_1.c
    )

    # if (USE_NASM)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DHAVE_MMX=1)
    # enable_language(NASM)
    # # common/x86
    # list(APPEND LIB_SOURCES
    # ${LIB_SOURCE_DIR}/common/x86/bitstream.h
    # )
    # endif ()

    if(WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/opus)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/celt)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/silk)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/silk/fixed)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/silk/float)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_OPUS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_CELT=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SILK=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)
    target_link_libraries(${LIB_NAME} PUBLIC de_celt)

    if (MSVC)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
        target_link_libraries(${LIB_NAME} PUBLIC pthread)
    endif()
endif()

# add_subdirectory(test)
