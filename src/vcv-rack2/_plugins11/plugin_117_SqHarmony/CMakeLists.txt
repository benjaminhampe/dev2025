set(LIB_NAME plugin_117_SqHarmony)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/a_plugin.cpp
        ${LIB_SOURCE_DIR}/a_plugin.hpp
        ${LIB_SOURCE_DIR}/Additive.cpp
        ${LIB_SOURCE_DIR}/Arpeggiator1.cpp
        ${LIB_SOURCE_DIR}/Harmony1.cpp
        ${LIB_SOURCE_DIR}/Harmony1Module.h
        ${LIB_SOURCE_DIR}/Harmony2Module.cpp
        ${LIB_SOURCE_DIR}/LFN2Module.cpp
        ${LIB_SOURCE_DIR}/MultiplierModule.cpp
        ${LIB_SOURCE_DIR}/MutatorModule.cpp
        ${LIB_SOURCE_DIR}/MutatorModule.h
        ${LIB_SOURCE_DIR}/PhasePatternsModule.cpp
        ${LIB_SOURCE_DIR}/Score.h
        ${LIB_SOURCE_DIR}/ScoreChord.h
        ${LIB_SOURCE_DIR}/VisualizerModule.cpp
        ${LIB_SOURCE_DIR}/VisualizerModule.h

        ${LIB_SOURCE_DIR}/composites/Additive.h
        ${LIB_SOURCE_DIR}/composites/AdditiveGainLogic.h
        ${LIB_SOURCE_DIR}/composites/AdditiveModLogic.h
        ${LIB_SOURCE_DIR}/composites/AdditivePitchLogic.h
        ${LIB_SOURCE_DIR}/composites/Arpeggiator.h
        ${LIB_SOURCE_DIR}/composites/CompositeUpdater.h
        ${LIB_SOURCE_DIR}/composites/Harmony.h
        ${LIB_SOURCE_DIR}/composites/Harmony2.h
        ${LIB_SOURCE_DIR}/composites/LFN2.h
        ${LIB_SOURCE_DIR}/composites/Multiplier.h
        ${LIB_SOURCE_DIR}/composites/Mutator.h
        ${LIB_SOURCE_DIR}/composites/PhasePatterns.h
        ${LIB_SOURCE_DIR}/composites/TestComposite.h
        ${LIB_SOURCE_DIR}/composites/Visualizer.h
        ${LIB_SOURCE_DIR}/composites/WidgetComposite.h

        ${LIB_SOURCE_DIR}/ctrl/BufferingParent.h
        ${LIB_SOURCE_DIR}/ctrl/GfxUtils.h
        ${LIB_SOURCE_DIR}/ctrl/ParamSelectorMenu.h
        ${LIB_SOURCE_DIR}/ctrl/PopupMenuParamWidget.h
        ${LIB_SOURCE_DIR}/ctrl/SqLabel.h
        ${LIB_SOURCE_DIR}/ctrl/SqMenuItem.h
        ${LIB_SOURCE_DIR}/ctrl/SqUI.h

        ${LIB_SOURCE_DIR}/notes/Chord4.cpp
        ${LIB_SOURCE_DIR}/notes/Chord4.h
        ${LIB_SOURCE_DIR}/notes/Chord4List.cpp
        ${LIB_SOURCE_DIR}/notes/Chord4List.h
        ${LIB_SOURCE_DIR}/notes/Chord4Manager.h
        ${LIB_SOURCE_DIR}/notes/ChordRelativeNote.h
        ${LIB_SOURCE_DIR}/notes/HarmonyChords.cpp
        ${LIB_SOURCE_DIR}/notes/HarmonyChords.h
        ${LIB_SOURCE_DIR}/notes/HarmonyNote.h
        ${LIB_SOURCE_DIR}/notes/HarmonySong.cpp
        ${LIB_SOURCE_DIR}/notes/HarmonySong.h
        ${LIB_SOURCE_DIR}/notes/KeysigOld.cpp
        ${LIB_SOURCE_DIR}/notes/KeysigOld.h
        ${LIB_SOURCE_DIR}/notes/NotationNote.h
        ${LIB_SOURCE_DIR}/notes/Options.h
        ${LIB_SOURCE_DIR}/notes/PitchKnowledge.cpp
        ${LIB_SOURCE_DIR}/notes/PitchKnowledge.h
        ${LIB_SOURCE_DIR}/notes/ProgressionAnalyzer.cpp
        ${LIB_SOURCE_DIR}/notes/ProgressionAnalyzer.h
        ${LIB_SOURCE_DIR}/notes/RankedChord.cpp
        ${LIB_SOURCE_DIR}/notes/RankedChord.h
        ${LIB_SOURCE_DIR}/notes/RawChordGenerator.h
        ${LIB_SOURCE_DIR}/notes/ScaleRelativeNote.cpp
        ${LIB_SOURCE_DIR}/notes/ScaleRelativeNote.h
        ${LIB_SOURCE_DIR}/notes/ScoreDrawUtils.cpp
        ${LIB_SOURCE_DIR}/notes/ScoreDrawUtils.h
        ${LIB_SOURCE_DIR}/notes/ScorePitchUtils.cpp
        ${LIB_SOURCE_DIR}/notes/ScorePitchUtils.h
        ${LIB_SOURCE_DIR}/notes/Style.cpp
        ${LIB_SOURCE_DIR}/notes/Style.h

        ${LIB_SOURCE_DIR}/util/ArpegPlayer.cpp
        ${LIB_SOURCE_DIR}/util/ArpegPlayer.h
        ${LIB_SOURCE_DIR}/util/ArpegRhythmPlayer.h
        ${LIB_SOURCE_DIR}/util/asserts.h
        ${LIB_SOURCE_DIR}/util/AudioMath.cpp
        ${LIB_SOURCE_DIR}/util/AudioMath.h
        ${LIB_SOURCE_DIR}/util/BitDelay.h
        ${LIB_SOURCE_DIR}/util/BitDelay2.h
        ${LIB_SOURCE_DIR}/util/ChordRecognizer.cpp
        ${LIB_SOURCE_DIR}/util/ChordRecognizer.h
        ${LIB_SOURCE_DIR}/util/ClockMult.h
        ${LIB_SOURCE_DIR}/util/ClockShifter3.h
        ${LIB_SOURCE_DIR}/util/ClockShifter5.h
        ${LIB_SOURCE_DIR}/util/ClockShifter6.h
        ${LIB_SOURCE_DIR}/util/Constants.h
        ${LIB_SOURCE_DIR}/util/DeGlitch.h
        ${LIB_SOURCE_DIR}/util/DifferentialClockCounter.h
        ${LIB_SOURCE_DIR}/util/Divider.h
        ${LIB_SOURCE_DIR}/util/FreqMeasure.h
        ${LIB_SOURCE_DIR}/util/FreqMeasure2.h
        ${LIB_SOURCE_DIR}/util/FreqMeasure3.h
        ${LIB_SOURCE_DIR}/util/GateDelay.h
        ${LIB_SOURCE_DIR}/util/GateTrigger.h
        ${LIB_SOURCE_DIR}/util/MelodyEvaluator.cpp
        ${LIB_SOURCE_DIR}/util/MelodyEvaluator.h
        ${LIB_SOURCE_DIR}/util/MelodyGenerator.cpp
        ${LIB_SOURCE_DIR}/util/MelodyGenerator.h
        ${LIB_SOURCE_DIR}/util/NumberFormatter.h
        ${LIB_SOURCE_DIR}/util/OneShot.h
        ${LIB_SOURCE_DIR}/util/OneShotSampleTimer.h
        ${LIB_SOURCE_DIR}/util/Ratchet.h
        ${LIB_SOURCE_DIR}/util/ResettablePhaseAccumulator.h
        ${LIB_SOURCE_DIR}/util/SchmidtTrigger.h
        ${LIB_SOURCE_DIR}/util/SeqClock.h
        ${LIB_SOURCE_DIR}/util/ShiftCalc.h
        ${LIB_SOURCE_DIR}/util/ShiftMath.h
        ${LIB_SOURCE_DIR}/util/SqLog.cpp
        ${LIB_SOURCE_DIR}/util/SqLog.h
        ${LIB_SOURCE_DIR}/util/SqStream.h
        ${LIB_SOURCE_DIR}/util/sq_rack.h

        ${LIB_SOURCE_DIR}/util/container/AtomicRingBuffer.h
        ${LIB_SOURCE_DIR}/util/container/NoteBuffer.h
        ${LIB_SOURCE_DIR}/util/container/ShiftingArray.h
        ${LIB_SOURCE_DIR}/util/container/SqArray.h
        ${LIB_SOURCE_DIR}/util/container/SqRingBuffer.h
        ${LIB_SOURCE_DIR}/util/container/SqRingBuffer2.h

        ${LIB_SOURCE_DIR}/util/dsp/Filter4pButter.h
        ${LIB_SOURCE_DIR}/util/dsp/LFNDsp.h
        ${LIB_SOURCE_DIR}/util/dsp/NoiseGen.h
        ${LIB_SOURCE_DIR}/util/dsp/PinkFilter.h
        ${LIB_SOURCE_DIR}/util/dsp/simd.h
        ${LIB_SOURCE_DIR}/util/dsp/SimdBlocks.h
        ${LIB_SOURCE_DIR}/util/dsp/SinesVCO.h
        ${LIB_SOURCE_DIR}/util/dsp/SqRandom.hpp

        ${LIB_SOURCE_DIR}/util/gray_code/GcGenerator.h
        ${LIB_SOURCE_DIR}/util/gray_code/GcUtils.h

        ${LIB_SOURCE_DIR}/util/quant/FloatNote.h
        ${LIB_SOURCE_DIR}/util/quant/KsigSharpFlatMonitor.h
        ${LIB_SOURCE_DIR}/util/quant/MidiNote.h
        ${LIB_SOURCE_DIR}/util/quant/NoteConvert.cpp
        ${LIB_SOURCE_DIR}/util/quant/NoteConvert.h
        ${LIB_SOURCE_DIR}/util/quant/PESConverter.h
        ${LIB_SOURCE_DIR}/util/quant/Scale.cpp
        ${LIB_SOURCE_DIR}/util/quant/Scale.h
        ${LIB_SOURCE_DIR}/util/quant/ScaleNote.h
        ${LIB_SOURCE_DIR}/util/quant/ScaleQuantizer.cpp
        ${LIB_SOURCE_DIR}/util/quant/ScaleQuantizer.h
        ${LIB_SOURCE_DIR}/util/quant/SharpsFlatsPref.cpp
        ${LIB_SOURCE_DIR}/util/quant/SharpsFlatsPref.h
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/composites)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ctrl)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/notes)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util/container)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util/dsp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util/gray_code)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util/quant)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __PLUGIN=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)

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

    set(VCV_NAME "SqHarmony")
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
