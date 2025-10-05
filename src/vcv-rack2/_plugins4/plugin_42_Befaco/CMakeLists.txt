set(LIB_NAME plugin_42_Befaco)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/ABC.cpp
        ${LIB_SOURCE_DIR}/ADSR.cpp
        ${LIB_SOURCE_DIR}/Atte.cpp
        ${LIB_SOURCE_DIR}/AxBC.cpp
        ${LIB_SOURCE_DIR}/Bandit.cpp
        ${LIB_SOURCE_DIR}/Burst.cpp
        ${LIB_SOURCE_DIR}/Bypass.cpp
        ${LIB_SOURCE_DIR}/ChoppingKinky.cpp
        ${LIB_SOURCE_DIR}/ChowDSP.hpp
        ${LIB_SOURCE_DIR}/DualAtenuverter.cpp
        ${LIB_SOURCE_DIR}/EvenVCO.cpp
        ${LIB_SOURCE_DIR}/HexmixVCA.cpp
        ${LIB_SOURCE_DIR}/Kickall.cpp
        ${LIB_SOURCE_DIR}/MidiThing.cpp
        ${LIB_SOURCE_DIR}/Mixer.cpp
        ${LIB_SOURCE_DIR}/Mixer2.cpp
        ${LIB_SOURCE_DIR}/Morphader.cpp
        ${LIB_SOURCE_DIR}/MotionMTR.cpp
        ${LIB_SOURCE_DIR}/MuDi.cpp
        ${LIB_SOURCE_DIR}/Muxlicer.cpp
        ${LIB_SOURCE_DIR}/NoisePlethora.cpp
        ${LIB_SOURCE_DIR}/Octaves.cpp
        ${LIB_SOURCE_DIR}/Percall.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/PonyVCO.cpp
        ${LIB_SOURCE_DIR}/Rampage.cpp
        ${LIB_SOURCE_DIR}/SamplingModulator.cpp
        ${LIB_SOURCE_DIR}/Slew.cpp
        ${LIB_SOURCE_DIR}/SlewLimiter.cpp
        ${LIB_SOURCE_DIR}/SpringReverb.cpp
        ${LIB_SOURCE_DIR}/StereoStrip.cpp
        ${LIB_SOURCE_DIR}/STMix.cpp
        ${LIB_SOURCE_DIR}/Voltio.cpp        
      # ${LIB_SOURCE_DIR}/noise-plethora
      # ${LIB_SOURCE_DIR}/noise-plethora/LICENSE.md
      # ${LIB_SOURCE_DIR}/noise-plethora/plugins
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/Banks.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/Banks.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/Banks_Def.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/NoisePlethoraPlugin.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/ProgramSelector.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_arrayOnTheRocks.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_Atari.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_BasuraTotal.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_basurilla.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_clusterSaw.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_crCluster2.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_CrossModRing.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_existencelsPain.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_FibonacciCluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_grainGlitch.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_grainGlitchII.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_grainGlitchIII.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_partialCluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_phasingCluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_PrimeCluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_PrimeCnoise.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_pwCluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_radioOhNo.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_resonoise.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_Rwalk_BitCrushPW.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_Rwalk_LFree.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_Rwalk_SineFMFlange.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_satanWorkout.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_sineFMcluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_S_H.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_TeensyAlt.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_TestPlugin.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_TriFMcluster.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_WalkingFilomena.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_WhiteNoise.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_whoKnows.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_XModRingSine.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/plugins/P_xModRingSqr.hpp
      # ${LIB_SOURCE_DIR}/noise-plethora/README.md
      # ${LIB_SOURCE_DIR}/noise-plethora/teensy
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/audio_core.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/dspinst.h
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_bitcrusher.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_bitcrusher.h
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_combine.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_combine.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_flange.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_flange.h
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_freeverb.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_freeverb.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_granular.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_granular.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_multiply.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_multiply.h
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_wavefolder.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/effect_wavefolder.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/filter_variable.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/filter_variable.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/mixer.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_dc.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_pinknoise.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_pinknoise.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_pwm.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_pwm.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_sine.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_waveform.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_whitenoise.cpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/synth_whitenoise.hpp
        ${LIB_SOURCE_DIR}/noise-plethora/teensy/TeensyAudioReplacements.hpp
      # ${LIB_SOURCE_DIR}/noise-plethora/teensy-audio-device.png
      # ${LIB_SOURCE_DIR}/noise-plethora/teensy-gui.png
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
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
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

    set(VCV_NAME "Befaco")
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
