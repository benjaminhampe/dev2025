set(LIB_NAME de_alt_firmware)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/bumps/bumps_cv_scaler.h
        ${LIB_HEADER_DIR}/bumps/bumps_generator.h
        ${LIB_HEADER_DIR}/bumps/bumps_resources.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_audio_buffer.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_correlator.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_frame.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_grain.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_granular_processor.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_granular_sample_player.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_looping_sample_player.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_mu_law.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_parameters.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_random_oscillator.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_resonestor.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_sample_rate_converter.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_window.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/etesia_wsola_sample_player.h
        # ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx/etesia_diffuser.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx/etesia_fx_engine.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx/etesia_oliverb.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx/etesia_pitch_shifter.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/fx/etesia_reverb.h
        # ${LIB_HEADER_DIR}/clouds_parasite/dsp/pvoc
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/pvoc/etesia_frame_transformation.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/pvoc/etesia_phase_vocoder.h
        ${LIB_HEADER_DIR}/clouds_parasite/dsp/pvoc/etesia_stft.h
        ${LIB_HEADER_DIR}/clouds_parasite/etesia_resources.h
        # ${LIB_HEADER_DIR}/deadman
        ${LIB_HEADER_DIR}/deadman/deadman_gate_processor.h
        ${LIB_HEADER_DIR}/deadman/deadman_processors.h
        ${LIB_HEADER_DIR}/deadman/deadman_resources.h
        # ${LIB_HEADER_DIR}/deadman/drums
        ${LIB_HEADER_DIR}/deadman/drums/deadman_bass_drum.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_cymbal.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_excitation.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_fm_drum.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_high_hat.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_snare_drum.h
        ${LIB_HEADER_DIR}/deadman/drums/deadman_svf.h
        # ${LIB_HEADER_DIR}/deadman/modulations
        ${LIB_HEADER_DIR}/deadman/modulations/deadman_bouncing_ball.h
        ${LIB_HEADER_DIR}/deadman/modulations/deadman_lfo.h
        ${LIB_HEADER_DIR}/deadman/modulations/deadman_mini_sequencer.h
        ${LIB_HEADER_DIR}/deadman/modulations/deadman_multistage_envelope.h
        ${LIB_HEADER_DIR}/deadman/modulations/deadman_turing_machine.h
        # ${LIB_HEADER_DIR}/deadman/number_station
        ${LIB_HEADER_DIR}/deadman/number_station/deadman_bytebeats.h
        ${LIB_HEADER_DIR}/deadman/number_station/deadman_number_station.h
        # ${LIB_HEADER_DIR}/deadman/pulse_processor
        ${LIB_HEADER_DIR}/deadman/pulse_processor/deadman_pulse_randomizer.h
        ${LIB_HEADER_DIR}/deadman/pulse_processor/deadman_pulse_shaper.h
        # ${LIB_HEADER_DIR}/distortiones
        ${LIB_HEADER_DIR}/distortiones/distortiones_resources.h
        # ${LIB_HEADER_DIR}/distortiones/dsp
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_filter_bank.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_limiter.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_modulator.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_oscillator.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_parameters.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_quadrature_oscillator.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_quadrature_transform.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_sample_rate_conversion_filters.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_sample_rate_converter.h
        ${LIB_HEADER_DIR}/distortiones/dsp/distortiones_vocoder.h
        # ${LIB_HEADER_DIR}/fluctus
        # ${LIB_HEADER_DIR}/fluctus/dsp
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_audio_buffer.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_correlator.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_frame.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_grain.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_granular_processor.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_granular_sample_player.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_kammerl_player.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_looping_sample_player.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_mu_law.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_parameters.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_sample_rate_converter.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_window.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fluctus_wsola_sample_player.h
        # ${LIB_HEADER_DIR}/fluctus/dsp/fx
        ${LIB_HEADER_DIR}/fluctus/dsp/fx/fluctus_diffuser.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fx/fluctus_fx_engine.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fx/fluctus_pitch_shifter.h
        ${LIB_HEADER_DIR}/fluctus/dsp/fx/fluctus_reverb.h
        # ${LIB_HEADER_DIR}/fluctus/dsp/pvoc
        ${LIB_HEADER_DIR}/fluctus/dsp/pvoc/fluctus_phase_vocoder.h
        ${LIB_HEADER_DIR}/fluctus/dsp/pvoc/fluctus_spectral_coulds_transformation.h
        ${LIB_HEADER_DIR}/fluctus/dsp/pvoc/fluctus_stft.h
        ${LIB_HEADER_DIR}/fluctus/fluctus_resources.h
        # ${LIB_HEADER_DIR}/mutuus
        # ${LIB_HEADER_DIR}/mutuus/dsp
        # ${LIB_HEADER_DIR}/mutuus/dsp/filters
        ${LIB_HEADER_DIR}/mutuus/dsp/filters/mutuus_dual_filter.h
        # ${LIB_HEADER_DIR}/mutuus/dsp/fx
        ${LIB_HEADER_DIR}/mutuus/dsp/fx/mutuus_ensemble.h
        ${LIB_HEADER_DIR}/mutuus/dsp/fx/mutuus_fx_engine.h
        ${LIB_HEADER_DIR}/mutuus/dsp/fx/mutuus_reverb.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_filter_bank.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_limiter.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_modulator.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_oscillator.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_parameters.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_quadrature_oscillator.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_quadrature_transform.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_sample_rate_conversion_filters.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_sample_rate_converter.h
        ${LIB_HEADER_DIR}/mutuus/dsp/mutuus_vocoder.h
        ${LIB_HEADER_DIR}/mutuus/mutuus_resources.h
        # ${LIB_HEADER_DIR}/parasites_stmlib
        # ${LIB_HEADER_DIR}/parasites_stmlib/algorithms
        ${LIB_HEADER_DIR}/parasites_stmlib/algorithms/parasites_pattern_predictor.h
        # ${LIB_HEADER_DIR}/parasites_stmlib/dsp
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_atan.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_cosine_oscillator.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_dsp.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_filter.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_parameter_interpolator.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_rsqrt.h
        ${LIB_HEADER_DIR}/parasites_stmlib/dsp/parasites_units.h
        # ${LIB_HEADER_DIR}/parasites_stmlib/fft
        ${LIB_HEADER_DIR}/parasites_stmlib/fft/parasites_shy_fft.h
        ${LIB_HEADER_DIR}/parasites_stmlib/parasites_stmlib.h
        # ${LIB_HEADER_DIR}/parasites_stmlib/utils
        ${LIB_HEADER_DIR}/parasites_stmlib/utils/parasites_buffer_allocator.h
        ${LIB_HEADER_DIR}/parasites_stmlib/utils/parasites_dsp.h
        ${LIB_HEADER_DIR}/parasites_stmlib/utils/parasites_random.h
        ${LIB_HEADER_DIR}/parasites_stmlib/utils/parasites_ring_buffer.h
        # ${LIB_HEADER_DIR}/renaissance
        ${LIB_HEADER_DIR}/renaissance/renaissance_analog_oscillator.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_digital_oscillator.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_envelope.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_excitation.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_macro_oscillator.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_parameter_interpolation.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_quantizer.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_quantizer_scales.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_resources.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_settings.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_signature_waveshaper.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_svf.h
        ${LIB_HEADER_DIR}/renaissance/renaissance_vco_jitter_source.h
        # ${LIB_HEADER_DIR}/renaissance/vocalist
        ${LIB_HEADER_DIR}/renaissance/vocalist/RenderTabs.h
        ${LIB_HEADER_DIR}/renaissance/vocalist/sam.h
        ${LIB_HEADER_DIR}/renaissance/vocalist/vocalist.h
        ${LIB_HEADER_DIR}/renaissance/vocalist/wordlist.h
        # ${LIB_HEADER_DIR}/scalaria
        # ${LIB_HEADER_DIR}/scalaria/dsp
        # ${LIB_HEADER_DIR}/scalaria/dsp/filters
        ${LIB_HEADER_DIR}/scalaria/dsp/filters/scalaria_ladder_filter.h
        ${LIB_HEADER_DIR}/scalaria/dsp/scalaria_modulator.h
        ${LIB_HEADER_DIR}/scalaria/dsp/scalaria_oscillator.h
        ${LIB_HEADER_DIR}/scalaria/dsp/scalaria_parameters.h
        ${LIB_HEADER_DIR}/scalaria/scalaria_resources.h
    )
    
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
        # ${LIB_SOURCE_DIR}/bumps
        ${LIB_SOURCE_DIR}/bumps/bumps_generator.cc
        ${LIB_SOURCE_DIR}/bumps/bumps_resources.cc
        # ${LIB_SOURCE_DIR}/clouds_parasite
        # ${LIB_SOURCE_DIR}/clouds_parasite/dsp
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/etesia_correlator.cc
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/etesia_granular_processor.cc
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/etesia_mu_law.cc
        # ${LIB_SOURCE_DIR}/clouds_parasite/dsp/pvoc
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/pvoc/etesia_frame_transformation.cc
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/pvoc/etesia_phase_vocoder.cc
        ${LIB_SOURCE_DIR}/clouds_parasite/dsp/pvoc/etesia_stft.cc
        ${LIB_SOURCE_DIR}/clouds_parasite/etesia_resources.cc
        # ${LIB_SOURCE_DIR}/deadman
        ${LIB_SOURCE_DIR}/deadman/deadman_processors.cc
        ${LIB_SOURCE_DIR}/deadman/deadman_resources.cc
        # ${LIB_SOURCE_DIR}/deadman/drums
        ${LIB_SOURCE_DIR}/deadman/drums/deadman_bass_drum.cc
        ${LIB_SOURCE_DIR}/deadman/drums/deadman_cymbal.cc
        ${LIB_SOURCE_DIR}/deadman/drums/deadman_fm_drum.cc
        ${LIB_SOURCE_DIR}/deadman/drums/deadman_high_hat.cc
        ${LIB_SOURCE_DIR}/deadman/drums/deadman_snare_drum.cc
        # ${LIB_SOURCE_DIR}/deadman/modulations
        ${LIB_SOURCE_DIR}/deadman/modulations/deadman_lfo.cc
        ${LIB_SOURCE_DIR}/deadman/modulations/deadman_multistage_envelope.cc
        # ${LIB_SOURCE_DIR}/deadman/number_station
        ${LIB_SOURCE_DIR}/deadman/number_station/deadman_bytebeats.cc
        ${LIB_SOURCE_DIR}/deadman/number_station/deadman_number_station.cc
        # ${LIB_SOURCE_DIR}/deadman/pulse_processor
        ${LIB_SOURCE_DIR}/deadman/pulse_processor/deadman_pulse_randomizer.cc
        ${LIB_SOURCE_DIR}/deadman/pulse_processor/deadman_pulse_shaper.cc
        # ${LIB_SOURCE_DIR}/distortiones
        ${LIB_SOURCE_DIR}/distortiones/distortiones_resources.cc
        # ${LIB_SOURCE_DIR}/distortiones/dsp
        ${LIB_SOURCE_DIR}/distortiones/dsp/distortiones_filter_bank.cc
        ${LIB_SOURCE_DIR}/distortiones/dsp/distortiones_modulator.cc
        ${LIB_SOURCE_DIR}/distortiones/dsp/distortiones_oscillator.cc
        ${LIB_SOURCE_DIR}/distortiones/dsp/distortiones_vocoder.cc
        # ${LIB_SOURCE_DIR}/fluctus
        # ${LIB_SOURCE_DIR}/fluctus/dsp
        ${LIB_SOURCE_DIR}/fluctus/dsp/fluctus_correlator.cc
        ${LIB_SOURCE_DIR}/fluctus/dsp/fluctus_granular_processor.cc
        ${LIB_SOURCE_DIR}/fluctus/dsp/fluctus_kammerl_player.cc
        ${LIB_SOURCE_DIR}/fluctus/dsp/fluctus_mu_law.cc
        # ${LIB_SOURCE_DIR}/fluctus/dsp/pvoc
        ${LIB_SOURCE_DIR}/fluctus/dsp/pvoc/fluctus_phase_vocoder.cc
        ${LIB_SOURCE_DIR}/fluctus/dsp/pvoc/fluctus_spectral_clouds_transformation.cc
        ${LIB_SOURCE_DIR}/fluctus/dsp/pvoc/fluctus_stft.cc
        ${LIB_SOURCE_DIR}/fluctus/fluctus_resources.cc
        # ${LIB_SOURCE_DIR}/mutuus
        # ${LIB_SOURCE_DIR}/mutuus/dsp
        ${LIB_SOURCE_DIR}/mutuus/dsp/mutuus_filter_bank.cc
        ${LIB_SOURCE_DIR}/mutuus/dsp/mutuus_modulator.cc
        ${LIB_SOURCE_DIR}/mutuus/dsp/mutuus_oscillator.cc
        ${LIB_SOURCE_DIR}/mutuus/dsp/mutuus_vocoder.cc
        ${LIB_SOURCE_DIR}/mutuus/mutuus_resources.cc
        # ${LIB_SOURCE_DIR}/parasites_stmlib
        # ${LIB_SOURCE_DIR}/parasites_stmlib/dsp
        ${LIB_SOURCE_DIR}/parasites_stmlib/dsp/parasites_atan.cc
        ${LIB_SOURCE_DIR}/parasites_stmlib/dsp/parasites_units.cc
        # ${LIB_SOURCE_DIR}/parasites_stmlib/utils
        ${LIB_SOURCE_DIR}/parasites_stmlib/utils/parasites_random.cc
        # ${LIB_SOURCE_DIR}/renaissance
        ${LIB_SOURCE_DIR}/renaissance/renaissance_analog_oscillator.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_digital_oscillator.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_harmonics.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_macro_oscillator.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_quantizer.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_resources.cc
        ${LIB_SOURCE_DIR}/renaissance/renaissance_stack.cc
        # ${LIB_SOURCE_DIR}/renaissance/vocalist
        ${LIB_SOURCE_DIR}/renaissance/vocalist/rendertabs.cc
        ${LIB_SOURCE_DIR}/renaissance/vocalist/sam.cc
        ${LIB_SOURCE_DIR}/renaissance/vocalist/vocalist.cc
        ${LIB_SOURCE_DIR}/renaissance/vocalist/wordlist.cc
        # ${LIB_SOURCE_DIR}/scalaria
        # ${LIB_SOURCE_DIR}/scalaria/dsp
        ${LIB_SOURCE_DIR}/scalaria/dsp/scalaria_modulator.cc
        ${LIB_SOURCE_DIR}/scalaria/dsp/scalaria_oscillator.cc
        ${LIB_SOURCE_DIR}/scalaria/scalaria_resources.cc
    )
    
    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    # target_compile_definitions(${LIB_NAME} PUBLIC NOASM=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

endif()

# add_subdirectory(test)
