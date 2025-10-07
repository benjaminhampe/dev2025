set(LIB_NAME plugin_100_SurgeRack)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Delay.cpp
        ${LIB_SOURCE_DIR}/Delay.h
        ${LIB_SOURCE_DIR}/DelayLineByFreq.cpp
        ${LIB_SOURCE_DIR}/DelayLineByFreq.h
        ${LIB_SOURCE_DIR}/DelayLineByFreqExpanded.cpp
        ${LIB_SOURCE_DIR}/DelayLineByFreqExpanded.h
        ${LIB_SOURCE_DIR}/DigitalRingMod.cpp
        ${LIB_SOURCE_DIR}/DigitalRingMod.h
        ${LIB_SOURCE_DIR}/EGxVCA.cpp
        ${LIB_SOURCE_DIR}/EGxVCA.h
        ${LIB_SOURCE_DIR}/FX.cpp
        ${LIB_SOURCE_DIR}/FX.h
        ${LIB_SOURCE_DIR}/FXConfig.h
        ${LIB_SOURCE_DIR}/LayoutConstants.h
        ${LIB_SOURCE_DIR}/LayoutEngine.h
        ${LIB_SOURCE_DIR}/LFO.cpp
        ${LIB_SOURCE_DIR}/LFO.h
        ${LIB_SOURCE_DIR}/Mixer.cpp
        ${LIB_SOURCE_DIR}/Mixer.h
        ${LIB_SOURCE_DIR}/ModMatrix.cpp
        ${LIB_SOURCE_DIR}/ModMatrix.h
        ${LIB_SOURCE_DIR}/NBarEditorWidget.h
        ${LIB_SOURCE_DIR}/QuadAD.cpp
        ${LIB_SOURCE_DIR}/QuadAD.h
        ${LIB_SOURCE_DIR}/QuadLFO.cpp
        ${LIB_SOURCE_DIR}/QuadLFO.h
        ${LIB_SOURCE_DIR}/SurgeXT.cpp
        ${LIB_SOURCE_DIR}/SurgeXT.h
        ${LIB_SOURCE_DIR}/TemposyncSupport.h
        ${LIB_SOURCE_DIR}/UnisonHelper.cpp
        ${LIB_SOURCE_DIR}/UnisonHelper.h
        ${LIB_SOURCE_DIR}/VCF.cpp
        ${LIB_SOURCE_DIR}/VCF.h
        ${LIB_SOURCE_DIR}/VCO.cpp
        ${LIB_SOURCE_DIR}/VCO.h
        ${LIB_SOURCE_DIR}/VCOConfig.h
        ${LIB_SOURCE_DIR}/Waveshaper.cpp
        ${LIB_SOURCE_DIR}/Waveshaper.h
        ${LIB_SOURCE_DIR}/XTModule.cpp
        ${LIB_SOURCE_DIR}/XTModule.h
        ${LIB_SOURCE_DIR}/XTModuleWidget.cpp
        ${LIB_SOURCE_DIR}/XTModuleWidget.h
        ${LIB_SOURCE_DIR}/XTStyle.cpp
        ${LIB_SOURCE_DIR}/XTStyle.h
        ${LIB_SOURCE_DIR}/XTWidgets.cpp
        ${LIB_SOURCE_DIR}/XTWidgets.h
        ${LIB_SOURCE_DIR}/fxconfig/Bonsai.h
        ${LIB_SOURCE_DIR}/fxconfig/Chorus.h
        ${LIB_SOURCE_DIR}/fxconfig/Chow.h
        ${LIB_SOURCE_DIR}/fxconfig/Combulator.h
        ${LIB_SOURCE_DIR}/fxconfig/Distortion.h
        ${LIB_SOURCE_DIR}/fxconfig/Ensemble.h
        ${LIB_SOURCE_DIR}/fxconfig/Exciter.h
        ${LIB_SOURCE_DIR}/fxconfig/Flanger.h
        ${LIB_SOURCE_DIR}/fxconfig/FrequencyShifter.h
        ${LIB_SOURCE_DIR}/fxconfig/Neuron.h
        ${LIB_SOURCE_DIR}/fxconfig/Nimbus.h
        ${LIB_SOURCE_DIR}/fxconfig/Phaser.h
        ${LIB_SOURCE_DIR}/fxconfig/Resonator.h
        ${LIB_SOURCE_DIR}/fxconfig/Reverb1.h
        ${LIB_SOURCE_DIR}/fxconfig/Reverb2.h
        ${LIB_SOURCE_DIR}/fxconfig/RingModulator.h
        ${LIB_SOURCE_DIR}/fxconfig/RotarySpeaker.h
        ${LIB_SOURCE_DIR}/fxconfig/SpringReverb.h
        ${LIB_SOURCE_DIR}/fxconfig/TreeMonster.h
        ${LIB_SOURCE_DIR}/fxconfig/Vocoder.h
        ${LIB_SOURCE_DIR}/vcoconfig/Alias.h
        ${LIB_SOURCE_DIR}/vcoconfig/Classic.h
        ${LIB_SOURCE_DIR}/vcoconfig/FM2.h
        ${LIB_SOURCE_DIR}/vcoconfig/FM3.h
        ${LIB_SOURCE_DIR}/vcoconfig/Modern.h
        ${LIB_SOURCE_DIR}/vcoconfig/SHNoise.h
        ${LIB_SOURCE_DIR}/vcoconfig/Sine.h
        ${LIB_SOURCE_DIR}/vcoconfig/String.h
        ${LIB_SOURCE_DIR}/vcoconfig/Twist.h
        ${LIB_SOURCE_DIR}/vcoconfig/Wavetable.h
        ${LIB_SOURCE_DIR}/vcoconfig/Window.h
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
    target_link_libraries(${LIB_NAME} PRIVATE de_surge_common)
    target_link_libraries(${LIB_NAME} PRIVATE de_sst_rackhelpers)
    target_link_libraries(${LIB_NAME} PRIVATE de_sst_plugininfra)
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

    set(VCV_NAME "SurgeRack")
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
