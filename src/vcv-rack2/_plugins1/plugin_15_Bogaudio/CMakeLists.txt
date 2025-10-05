set(LIB_NAME plugin_15_Bogaudio)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/AD.cpp
        ${LIB_SOURCE_DIR}/AD.hpp
        ${LIB_SOURCE_DIR}/Additator.cpp
        ${LIB_SOURCE_DIR}/Additator.hpp
        ${LIB_SOURCE_DIR}/addressable_sequence.cpp
        ${LIB_SOURCE_DIR}/addressable_sequence.hpp
        ${LIB_SOURCE_DIR}/AddrSeq.cpp
        ${LIB_SOURCE_DIR}/AddrSeq.hpp
        ${LIB_SOURCE_DIR}/AddrSeqX.cpp
        ${LIB_SOURCE_DIR}/AddrSeqX.hpp
        ${LIB_SOURCE_DIR}/AddrSeq_shared.hpp
        ${LIB_SOURCE_DIR}/ADSR.cpp
        ${LIB_SOURCE_DIR}/ADSR.hpp
        ${LIB_SOURCE_DIR}/AMRM.cpp
        ${LIB_SOURCE_DIR}/AMRM.hpp
        ${LIB_SOURCE_DIR}/Analyzer.cpp
        ${LIB_SOURCE_DIR}/Analyzer.hpp
        ${LIB_SOURCE_DIR}/AnalyzerXL.cpp
        ${LIB_SOURCE_DIR}/AnalyzerXL.hpp
        ${LIB_SOURCE_DIR}/analyzer_base.cpp
        ${LIB_SOURCE_DIR}/analyzer_base.hpp
        ${LIB_SOURCE_DIR}/Arp.cpp
        ${LIB_SOURCE_DIR}/Arp.hpp
        ${LIB_SOURCE_DIR}/ASR.cpp
        ${LIB_SOURCE_DIR}/ASR.hpp
        ${LIB_SOURCE_DIR}/Assign.cpp
        ${LIB_SOURCE_DIR}/Assign.hpp
        ${LIB_SOURCE_DIR}/Blank3.cpp
        ${LIB_SOURCE_DIR}/Blank3.hpp
        ${LIB_SOURCE_DIR}/Blank6.cpp
        ${LIB_SOURCE_DIR}/Blank6.hpp
        ${LIB_SOURCE_DIR}/bogaudio.cpp
        ${LIB_SOURCE_DIR}/bogaudio.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_analyzer.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_analyzer.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_base.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_buffer.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_envelope.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_envelope.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_equalizer.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_equalizer.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_experiments.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_experiments.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_filter.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_filter.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_multimode.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_multimode.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_resample.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_resample.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_utility.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_filters_utility.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_fixed.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_math.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_math.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_noise.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_noise.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_oscillator.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_oscillator.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_pitch.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_signal.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_signal.hpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_table.cpp
        ${LIB_SOURCE_DIR}/bogaudio_dsp_table.hpp
        ${LIB_SOURCE_DIR}/Bool.cpp
        ${LIB_SOURCE_DIR}/Bool.hpp
        ${LIB_SOURCE_DIR}/Chirp.cpp
        ${LIB_SOURCE_DIR}/Chirp.hpp
        ${LIB_SOURCE_DIR}/Clpr.cpp
        ${LIB_SOURCE_DIR}/Clpr.hpp
        ${LIB_SOURCE_DIR}/Cmp.cpp
        ${LIB_SOURCE_DIR}/Cmp.hpp
        ${LIB_SOURCE_DIR}/CmpDist.cpp
        ${LIB_SOURCE_DIR}/CmpDist.hpp
        ${LIB_SOURCE_DIR}/CVD.cpp
        ${LIB_SOURCE_DIR}/CVD.hpp
        ${LIB_SOURCE_DIR}/DADSRH.cpp
        ${LIB_SOURCE_DIR}/DADSRH.hpp
        ${LIB_SOURCE_DIR}/DADSRHPlus.cpp
        ${LIB_SOURCE_DIR}/DADSRHPlus.hpp
        ${LIB_SOURCE_DIR}/dadsrh_core.cpp
        ${LIB_SOURCE_DIR}/dadsrh_core.hpp
        ${LIB_SOURCE_DIR}/Detune.cpp
        ${LIB_SOURCE_DIR}/Detune.hpp
        ${LIB_SOURCE_DIR}/DGate.cpp
        ${LIB_SOURCE_DIR}/DGate.hpp
        ${LIB_SOURCE_DIR}/disable_output_limit.cpp
        ${LIB_SOURCE_DIR}/disable_output_limit.hpp
        ${LIB_SOURCE_DIR}/Edge.cpp
        ${LIB_SOURCE_DIR}/Edge.hpp
        ${LIB_SOURCE_DIR}/EightFO.cpp
        ${LIB_SOURCE_DIR}/EightFO.hpp
        ${LIB_SOURCE_DIR}/EightOne.cpp
        ${LIB_SOURCE_DIR}/EightOne.hpp
        ${LIB_SOURCE_DIR}/EQ.cpp
        ${LIB_SOURCE_DIR}/EQ.hpp
        ${LIB_SOURCE_DIR}/EQS.cpp
        ${LIB_SOURCE_DIR}/EQS.hpp
        ${LIB_SOURCE_DIR}/expanders.hpp
        ${LIB_SOURCE_DIR}/FFB.cpp
        ${LIB_SOURCE_DIR}/FFB.hpp
        ${LIB_SOURCE_DIR}/FlipFlop.cpp
        ${LIB_SOURCE_DIR}/FlipFlop.hpp
        ${LIB_SOURCE_DIR}/FMOp.cpp
        ${LIB_SOURCE_DIR}/FMOp.hpp
        ${LIB_SOURCE_DIR}/Follow.cpp
        ${LIB_SOURCE_DIR}/Follow.hpp
        ${LIB_SOURCE_DIR}/follower_base.cpp
        ${LIB_SOURCE_DIR}/follower_base.hpp
        ${LIB_SOURCE_DIR}/FourFO.cpp
        ${LIB_SOURCE_DIR}/FourFO.hpp
        ${LIB_SOURCE_DIR}/FourMan.cpp
        ${LIB_SOURCE_DIR}/FourMan.hpp
        ${LIB_SOURCE_DIR}/Inv.cpp
        ${LIB_SOURCE_DIR}/Inv.hpp
        ${LIB_SOURCE_DIR}/Lag.cpp
        ${LIB_SOURCE_DIR}/Lag.hpp
        ${LIB_SOURCE_DIR}/LFO.cpp
        ${LIB_SOURCE_DIR}/LFO.hpp
        ${LIB_SOURCE_DIR}/lfo_base.cpp
        ${LIB_SOURCE_DIR}/lfo_base.hpp
        ${LIB_SOURCE_DIR}/Lgsw.cpp
        ${LIB_SOURCE_DIR}/Lgsw.hpp
        ${LIB_SOURCE_DIR}/LLFO.cpp
        ${LIB_SOURCE_DIR}/LLFO.hpp
        ${LIB_SOURCE_DIR}/LLPG.cpp
        ${LIB_SOURCE_DIR}/LLPG.hpp
        ${LIB_SOURCE_DIR}/Lmtr.cpp
        ${LIB_SOURCE_DIR}/Lmtr.hpp
        ${LIB_SOURCE_DIR}/LPG.cpp
        ${LIB_SOURCE_DIR}/LPG.hpp
        ${LIB_SOURCE_DIR}/lpg_common.cpp
        ${LIB_SOURCE_DIR}/lpg_common.hpp
        ${LIB_SOURCE_DIR}/LVCF.cpp
        ${LIB_SOURCE_DIR}/LVCF.hpp
        ${LIB_SOURCE_DIR}/LVCO.cpp
        ${LIB_SOURCE_DIR}/LVCO.hpp
        ${LIB_SOURCE_DIR}/Manual.cpp
        ${LIB_SOURCE_DIR}/Manual.hpp
        ${LIB_SOURCE_DIR}/Matrix18.cpp
        ${LIB_SOURCE_DIR}/Matrix18.hpp
        ${LIB_SOURCE_DIR}/Matrix44.cpp
        ${LIB_SOURCE_DIR}/Matrix44.hpp
        ${LIB_SOURCE_DIR}/Matrix44Cvm.cpp
        ${LIB_SOURCE_DIR}/Matrix44Cvm.hpp
        ${LIB_SOURCE_DIR}/Matrix44_shared.hpp
        ${LIB_SOURCE_DIR}/Matrix81.cpp
        ${LIB_SOURCE_DIR}/Matrix81.hpp
        ${LIB_SOURCE_DIR}/Matrix88.cpp
        ${LIB_SOURCE_DIR}/Matrix88.hpp
        ${LIB_SOURCE_DIR}/Matrix88Cv.cpp
        ${LIB_SOURCE_DIR}/Matrix88Cv.hpp
        ${LIB_SOURCE_DIR}/Matrix88M.cpp
        ${LIB_SOURCE_DIR}/Matrix88M.hpp
        ${LIB_SOURCE_DIR}/Matrix88_shared.hpp
        ${LIB_SOURCE_DIR}/matrix_base.cpp
        ${LIB_SOURCE_DIR}/matrix_base.hpp
        ${LIB_SOURCE_DIR}/MegaGate.cpp
        ${LIB_SOURCE_DIR}/MegaGate.hpp
        ${LIB_SOURCE_DIR}/menu.cpp
        ${LIB_SOURCE_DIR}/menu.hpp
        ${LIB_SOURCE_DIR}/Mix1.cpp
        ${LIB_SOURCE_DIR}/Mix1.hpp
        ${LIB_SOURCE_DIR}/Mix2.cpp
        ${LIB_SOURCE_DIR}/Mix2.hpp
        ${LIB_SOURCE_DIR}/Mix4.cpp
        ${LIB_SOURCE_DIR}/Mix4.hpp
        ${LIB_SOURCE_DIR}/Mix4x.cpp
        ${LIB_SOURCE_DIR}/Mix4x.hpp
        ${LIB_SOURCE_DIR}/Mix4_shared.hpp
        ${LIB_SOURCE_DIR}/Mix8.cpp
        ${LIB_SOURCE_DIR}/Mix8.hpp
        ${LIB_SOURCE_DIR}/Mix8x.cpp
        ${LIB_SOURCE_DIR}/Mix8x.hpp
        ${LIB_SOURCE_DIR}/Mix8_shared.hpp
        ${LIB_SOURCE_DIR}/mixer.cpp
        ${LIB_SOURCE_DIR}/mixer.hpp
        ${LIB_SOURCE_DIR}/mixer_expander.cpp
        ${LIB_SOURCE_DIR}/mixer_expander.hpp
        ${LIB_SOURCE_DIR}/module.cpp
        ${LIB_SOURCE_DIR}/module.hpp
        ${LIB_SOURCE_DIR}/Mono.cpp
        ${LIB_SOURCE_DIR}/Mono.hpp
        ${LIB_SOURCE_DIR}/Mult.cpp
        ${LIB_SOURCE_DIR}/Mult.hpp
        ${LIB_SOURCE_DIR}/Mumix.cpp
        ${LIB_SOURCE_DIR}/Mumix.hpp
        ${LIB_SOURCE_DIR}/Mute8.cpp
        ${LIB_SOURCE_DIR}/Mute8.hpp
        ${LIB_SOURCE_DIR}/Noise.cpp
        ${LIB_SOURCE_DIR}/Noise.hpp
        ${LIB_SOURCE_DIR}/Nsgt.cpp
        ${LIB_SOURCE_DIR}/Nsgt.hpp
        ${LIB_SOURCE_DIR}/Offset.cpp
        ${LIB_SOURCE_DIR}/Offset.hpp
        ${LIB_SOURCE_DIR}/OneEight.cpp
        ${LIB_SOURCE_DIR}/OneEight.hpp
        ${LIB_SOURCE_DIR}/output_range.cpp
        ${LIB_SOURCE_DIR}/output_range.hpp
        ${LIB_SOURCE_DIR}/Pan.cpp
        ${LIB_SOURCE_DIR}/Pan.hpp
        ${LIB_SOURCE_DIR}/parametric_eq.cpp
        ${LIB_SOURCE_DIR}/parametric_eq.hpp
        ${LIB_SOURCE_DIR}/param_quantities.cpp
        ${LIB_SOURCE_DIR}/param_quantities.hpp
        ${LIB_SOURCE_DIR}/PEQ.cpp
        ${LIB_SOURCE_DIR}/PEQ.hpp
        ${LIB_SOURCE_DIR}/PEQ14.cpp
        ${LIB_SOURCE_DIR}/PEQ14.hpp
        ${LIB_SOURCE_DIR}/PEQ14XF.cpp
        ${LIB_SOURCE_DIR}/PEQ14XF.hpp
        ${LIB_SOURCE_DIR}/PEQ14XR.cpp
        ${LIB_SOURCE_DIR}/PEQ14XR.hpp
        ${LIB_SOURCE_DIR}/PEQ14XV.cpp
        ${LIB_SOURCE_DIR}/PEQ14XV.hpp
        ${LIB_SOURCE_DIR}/PEQ14_shared.hpp
        ${LIB_SOURCE_DIR}/PEQ6.cpp
        ${LIB_SOURCE_DIR}/PEQ6.hpp
        ${LIB_SOURCE_DIR}/PEQ6XF.cpp
        ${LIB_SOURCE_DIR}/PEQ6XF.hpp
        ${LIB_SOURCE_DIR}/PEQ6_shared.hpp
        ${LIB_SOURCE_DIR}/Pgmr.cpp
        ${LIB_SOURCE_DIR}/Pgmr.hpp
        ${LIB_SOURCE_DIR}/PgmrX.cpp
        ${LIB_SOURCE_DIR}/PgmrX.hpp
        ${LIB_SOURCE_DIR}/Pgmr_shared.cpp
        ${LIB_SOURCE_DIR}/Pgmr_shared.hpp
        ${LIB_SOURCE_DIR}/PolyCon16.cpp
        ${LIB_SOURCE_DIR}/PolyCon16.hpp
        ${LIB_SOURCE_DIR}/PolyCon8.cpp
        ${LIB_SOURCE_DIR}/PolyCon8.hpp
        ${LIB_SOURCE_DIR}/PolyMult.cpp
        ${LIB_SOURCE_DIR}/PolyMult.hpp
        ${LIB_SOURCE_DIR}/PolyOff16.cpp
        ${LIB_SOURCE_DIR}/PolyOff16.hpp
        ${LIB_SOURCE_DIR}/PolyOff8.cpp
        ${LIB_SOURCE_DIR}/PolyOff8.hpp
        ${LIB_SOURCE_DIR}/poly_channels.cpp
        ${LIB_SOURCE_DIR}/poly_channels.hpp
        ${LIB_SOURCE_DIR}/Pressor.cpp
        ${LIB_SOURCE_DIR}/Pressor.hpp
        ${LIB_SOURCE_DIR}/Pulse.cpp
        ${LIB_SOURCE_DIR}/Pulse.hpp
        ${LIB_SOURCE_DIR}/rack_overrides.cpp
        ${LIB_SOURCE_DIR}/rack_overrides.hpp
        ${LIB_SOURCE_DIR}/Ranalyzer.cpp
        ${LIB_SOURCE_DIR}/Ranalyzer.hpp
        ${LIB_SOURCE_DIR}/Reftone.cpp
        ${LIB_SOURCE_DIR}/Reftone.hpp
        ${LIB_SOURCE_DIR}/RGate.cpp
        ${LIB_SOURCE_DIR}/RGate.hpp
        ${LIB_SOURCE_DIR}/SampleHold.cpp
        ${LIB_SOURCE_DIR}/SampleHold.hpp
        ${LIB_SOURCE_DIR}/save_latch_to_patch.cpp
        ${LIB_SOURCE_DIR}/save_latch_to_patch.hpp
        ${LIB_SOURCE_DIR}/Shaper.cpp
        ${LIB_SOURCE_DIR}/Shaper.hpp
        ${LIB_SOURCE_DIR}/ShaperPlus.cpp
        ${LIB_SOURCE_DIR}/ShaperPlus.hpp
        ${LIB_SOURCE_DIR}/shaper_core.cpp
        ${LIB_SOURCE_DIR}/shaper_core.hpp
        ${LIB_SOURCE_DIR}/Sine.cpp
        ${LIB_SOURCE_DIR}/Sine.hpp
        ${LIB_SOURCE_DIR}/skins.cpp
        ${LIB_SOURCE_DIR}/skins.hpp
        ${LIB_SOURCE_DIR}/Slew.cpp
        ${LIB_SOURCE_DIR}/Slew.hpp
        ${LIB_SOURCE_DIR}/slew_common.cpp
        ${LIB_SOURCE_DIR}/slew_common.hpp
        ${LIB_SOURCE_DIR}/Stack.cpp
        ${LIB_SOURCE_DIR}/Stack.hpp
        ${LIB_SOURCE_DIR}/Sums.cpp
        ${LIB_SOURCE_DIR}/Sums.hpp
        ${LIB_SOURCE_DIR}/Switch.cpp
        ${LIB_SOURCE_DIR}/Switch.hpp
        ${LIB_SOURCE_DIR}/Switch1616.cpp
        ${LIB_SOURCE_DIR}/Switch1616.hpp
        ${LIB_SOURCE_DIR}/Switch18.cpp
        ${LIB_SOURCE_DIR}/Switch18.hpp
        ${LIB_SOURCE_DIR}/Switch44.cpp
        ${LIB_SOURCE_DIR}/Switch44.hpp
        ${LIB_SOURCE_DIR}/Switch81.cpp
        ${LIB_SOURCE_DIR}/Switch81.hpp
        ${LIB_SOURCE_DIR}/Switch88.cpp
        ${LIB_SOURCE_DIR}/Switch88.hpp
        ${LIB_SOURCE_DIR}/Test.cpp
        ${LIB_SOURCE_DIR}/Test.hpp
        ${LIB_SOURCE_DIR}/Test2.cpp
        ${LIB_SOURCE_DIR}/Test2.hpp
        ${LIB_SOURCE_DIR}/TestExpander.cpp
        ${LIB_SOURCE_DIR}/TestExpander.hpp
        ${LIB_SOURCE_DIR}/TestGl.cpp
        ${LIB_SOURCE_DIR}/TestGl.hpp
        ${LIB_SOURCE_DIR}/TestVCF.cpp
        ${LIB_SOURCE_DIR}/TestVCF.hpp
        ${LIB_SOURCE_DIR}/trigger_on_load.cpp
        ${LIB_SOURCE_DIR}/trigger_on_load.hpp
        ${LIB_SOURCE_DIR}/UMix.cpp
        ${LIB_SOURCE_DIR}/UMix.hpp
        ${LIB_SOURCE_DIR}/Unison.cpp
        ${LIB_SOURCE_DIR}/Unison.hpp
        ${LIB_SOURCE_DIR}/utils.cpp
        ${LIB_SOURCE_DIR}/utils.hpp
        ${LIB_SOURCE_DIR}/VCA.cpp
        ${LIB_SOURCE_DIR}/VCA.hpp
        ${LIB_SOURCE_DIR}/VCAmp.cpp
        ${LIB_SOURCE_DIR}/VCAmp.hpp
        ${LIB_SOURCE_DIR}/VCF.cpp
        ${LIB_SOURCE_DIR}/VCF.hpp
        ${LIB_SOURCE_DIR}/VCM.cpp
        ${LIB_SOURCE_DIR}/VCM.hpp
        ${LIB_SOURCE_DIR}/VCO.cpp
        ${LIB_SOURCE_DIR}/VCO.hpp
        ${LIB_SOURCE_DIR}/vco_base.cpp
        ${LIB_SOURCE_DIR}/vco_base.hpp
        ${LIB_SOURCE_DIR}/Velo.cpp
        ${LIB_SOURCE_DIR}/Velo.hpp
        ${LIB_SOURCE_DIR}/Vish.cpp
        ${LIB_SOURCE_DIR}/Vish.hpp
        ${LIB_SOURCE_DIR}/VU.cpp
        ${LIB_SOURCE_DIR}/VU.hpp
        ${LIB_SOURCE_DIR}/Walk.cpp
        ${LIB_SOURCE_DIR}/Walk.hpp
        ${LIB_SOURCE_DIR}/Walk2.cpp
        ${LIB_SOURCE_DIR}/Walk2.hpp
        ${LIB_SOURCE_DIR}/widgets.cpp
        ${LIB_SOURCE_DIR}/widgets.hpp
        ${LIB_SOURCE_DIR}/XCO.cpp
        ${LIB_SOURCE_DIR}/XCO.hpp
        ${LIB_SOURCE_DIR}/XFade.cpp
        ${LIB_SOURCE_DIR}/XFade.hpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_bogaudio_ffft)
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

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            COMMENT "Stripping debug symbols"
        )

    # endif()

    set(VCV_NAME "Bogaudio")
    add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # Create working directory "z_benni/Gemini"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}"
        # Move libGemini.dll to working directory Gemini and rename to plugin.dll
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${LIB_NAME}.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}/plugin.dll"
        # Compress directory Gemini to Gemini.tar.zst
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni"
                tar -cf "${VCV_NAME}.tar" "${VCV_NAME}"
        COMMAND zstd -o
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar"
        # Move Gemini.tar.zst to final destination and rename to .vcvplugin
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins"
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/${VCV_NAME}.vcvplugin"
    )

    file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
