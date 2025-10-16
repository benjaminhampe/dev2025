set(LIB_NAME plugin_128_Hetrick)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/2To4.cpp
        ${LIB_SOURCE_DIR}/AmplitudeShaper.cpp
        ${LIB_SOURCE_DIR}/AnalogToDigital.cpp
        ${LIB_SOURCE_DIR}/ASR.cpp
        ${LIB_SOURCE_DIR}/BinaryCounter.cpp
        ${LIB_SOURCE_DIR}/BinaryGate.cpp
        ${LIB_SOURCE_DIR}/BinaryNoise.cpp
        ${LIB_SOURCE_DIR}/Bitshift.cpp
        ${LIB_SOURCE_DIR}/BlankPanel.cpp
        ${LIB_SOURCE_DIR}/Boolean3.cpp
        ${LIB_SOURCE_DIR}/Chaos1Op.cpp
        ${LIB_SOURCE_DIR}/Chaos2Op.cpp
        ${LIB_SOURCE_DIR}/Chaos3Op.cpp
        ${LIB_SOURCE_DIR}/ChaoticAttractors.cpp
        ${LIB_SOURCE_DIR}/ClockedNoise.cpp
        ${LIB_SOURCE_DIR}/ClockToPhasor.cpp
        ${LIB_SOURCE_DIR}/Comparator.cpp
        ${LIB_SOURCE_DIR}/Contrast.cpp
        ${LIB_SOURCE_DIR}/Crackle.cpp
        ${LIB_SOURCE_DIR}/DataCompander.cpp
        ${LIB_SOURCE_DIR}/Delta.cpp
        ${LIB_SOURCE_DIR}/DigitalToAnalog.cpp
        ${LIB_SOURCE_DIR}/Dust.cpp
        ${LIB_SOURCE_DIR}/Exponent.cpp
        ${LIB_SOURCE_DIR}/FBSineChaos.cpp
        ${LIB_SOURCE_DIR}/FlipFlop.cpp
        ${LIB_SOURCE_DIR}/FlipPan.cpp
        ${LIB_SOURCE_DIR}/GateDelay.cpp
        ${LIB_SOURCE_DIR}/GateJunction.cpp
        ${LIB_SOURCE_DIR}/GateJunctionExp.cpp
        ${LIB_SOURCE_DIR}/Gingerbread.cpp
        ${LIB_SOURCE_DIR}/HetrickCV.cpp
        ${LIB_SOURCE_DIR}/HetrickCV.hpp
        ${LIB_SOURCE_DIR}/HetrickUtilities.cpp
        ${LIB_SOURCE_DIR}/HetrickUtilities.hpp
        ${LIB_SOURCE_DIR}/LogicCombine.cpp
        ${LIB_SOURCE_DIR}/MidSide.cpp
        ${LIB_SOURCE_DIR}/MinMax.cpp
        ${LIB_SOURCE_DIR}/Normals.cpp
        ${LIB_SOURCE_DIR}/PhaseDrivenSequencer.cpp
        ${LIB_SOURCE_DIR}/PhaseDrivenSequencer32.cpp
        ${LIB_SOURCE_DIR}/PhasorAnalyzer.cpp
        ${LIB_SOURCE_DIR}/PhasorBurstGen.cpp
        ${LIB_SOURCE_DIR}/PhasorDivMult.cpp
        ${LIB_SOURCE_DIR}/PhasorEuclidean.cpp
        ${LIB_SOURCE_DIR}/PhasorFreezer.cpp
        ${LIB_SOURCE_DIR}/PhasorGates.cpp
        ${LIB_SOURCE_DIR}/PhasorGates32.cpp
        ${LIB_SOURCE_DIR}/PhasorGates64.cpp
        ${LIB_SOURCE_DIR}/PhasorGen.cpp
        ${LIB_SOURCE_DIR}/PhasorGeometry.cpp
        ${LIB_SOURCE_DIR}/PhasorHumanizer.cpp
        ${LIB_SOURCE_DIR}/PhasorMixer.cpp
        ${LIB_SOURCE_DIR}/PhasorOctature.cpp
        ${LIB_SOURCE_DIR}/PhasorProbability.cpp
        ${LIB_SOURCE_DIR}/PhasorQuadrature.cpp
        ${LIB_SOURCE_DIR}/PhasorRandom.cpp
        ${LIB_SOURCE_DIR}/PhasorRanger.cpp
        ${LIB_SOURCE_DIR}/PhasorReset.cpp
        ${LIB_SOURCE_DIR}/PhasorRhythmGroup.cpp
        ${LIB_SOURCE_DIR}/PhasorShape.cpp
        ${LIB_SOURCE_DIR}/PhasorShift.cpp
        ${LIB_SOURCE_DIR}/PhasorSplitter.cpp
        ${LIB_SOURCE_DIR}/PhasorStutter.cpp
        ${LIB_SOURCE_DIR}/PhasorSubstepShape.cpp
        ${LIB_SOURCE_DIR}/PhasorSwing.cpp
        ${LIB_SOURCE_DIR}/PhasorTimetable.cpp
        ${LIB_SOURCE_DIR}/PhasorToClock.cpp
        ${LIB_SOURCE_DIR}/PhasorToLFO.cpp
        ${LIB_SOURCE_DIR}/PhasorToRandom.cpp
        ${LIB_SOURCE_DIR}/PhasorToWaveforms.cpp
        ${LIB_SOURCE_DIR}/PolymetricPhasors.cpp
        ${LIB_SOURCE_DIR}/Probability.cpp
        ${LIB_SOURCE_DIR}/RandomGates.cpp
        ${LIB_SOURCE_DIR}/Rotator.cpp
        ${LIB_SOURCE_DIR}/Rungler.cpp
        ${LIB_SOURCE_DIR}/Scanner.cpp
        ${LIB_SOURCE_DIR}/TrigShaper.cpp
        ${LIB_SOURCE_DIR}/VectorMix.cpp
        ${LIB_SOURCE_DIR}/Waveshaper.cpp
        ${LIB_SOURCE_DIR}/XYToPolar.cpp
        ${LIB_SOURCE_DIR}/DSP/HCVChaos.cpp
        ${LIB_SOURCE_DIR}/DSP/HCVChaos.h
        ${LIB_SOURCE_DIR}/DSP/HCVCrackle.h
        ${LIB_SOURCE_DIR}/DSP/HCVDCFilter.h
        ${LIB_SOURCE_DIR}/DSP/HCVFunctions.h
        ${LIB_SOURCE_DIR}/DSP/HCVRandom.h
        ${LIB_SOURCE_DIR}/DSP/HCVSampleRate.h
        ${LIB_SOURCE_DIR}/DSP/HCVScanner.h
        ${LIB_SOURCE_DIR}/DSP/HCVShiftRegister.h
        ${LIB_SOURCE_DIR}/DSP/HCVSlewedCrossfader.h
        ${LIB_SOURCE_DIR}/DSP/HCVSlewLimiter.h
        ${LIB_SOURCE_DIR}/DSP/HCVTiming.h
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasor.h
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasorAnalyzers.cpp
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasorAnalyzers.h
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasorCommon.h
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasorEffects.cpp
        ${LIB_SOURCE_DIR}/DSP/Phasors/HCVPhasorEffects.h        
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
    target_link_libraries(${LIB_NAME} PRIVATE de_gamma_128_Hetrick)
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

    set(VCV_NAME "Hetrick")
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
