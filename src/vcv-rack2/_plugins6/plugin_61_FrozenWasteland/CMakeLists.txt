set(LIB_NAME plugin_61_FrozenWasteland)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/BPMLFO.cpp
        ${LIB_SOURCE_DIR}/BPMLFO2.cpp
        ${LIB_SOURCE_DIR}/BPMLFOPhaseExpander.cpp
        ${LIB_SOURCE_DIR}/CDCSeriouslySlowLFO.cpp
        ${LIB_SOURCE_DIR}/DamianLillard.cpp
        ${LIB_SOURCE_DIR}/DrunkenRampage.cpp
        ${LIB_SOURCE_DIR}/EverlastingGlottalStopper.cpp
        ${LIB_SOURCE_DIR}/FillingStation.cpp
        ${LIB_SOURCE_DIR}/FormalDinner.cpp
        ${LIB_SOURCE_DIR}/FrozenWasteland.cpp
        ${LIB_SOURCE_DIR}/FrozenWasteland.hpp
        ${LIB_SOURCE_DIR}/HairPick.cpp
        ${LIB_SOURCE_DIR}/HyperMeasures.cpp
        ${LIB_SOURCE_DIR}/JustAPhaser.cpp
        ${LIB_SOURCE_DIR}/LissajousLFO.cpp
        ${LIB_SOURCE_DIR}/ManicCompression.cpp
        ${LIB_SOURCE_DIR}/ManicCompressionMB.cpp
        ${LIB_SOURCE_DIR}/Midichlorian.cpp
        ${LIB_SOURCE_DIR}/MidiRecorder.cpp
        ${LIB_SOURCE_DIR}/MrBlueSky.cpp
        ${LIB_SOURCE_DIR}/PhasedLockedLoop.cpp
        ${LIB_SOURCE_DIR}/PNChordExpander.cpp
        ${LIB_SOURCE_DIR}/PNOctaveProbabilityExpander.cpp
        ${LIB_SOURCE_DIR}/PortlandWeather.cpp
        ${LIB_SOURCE_DIR}/ProbablyNote.cpp
        ${LIB_SOURCE_DIR}/ProbablyNoteArabic.cpp
        ${LIB_SOURCE_DIR}/ProbablyNoteBP.cpp
        ${LIB_SOURCE_DIR}/ProbablyNoteIndian.cpp
        ${LIB_SOURCE_DIR}/ProbablyNoteMN.cpp
        ${LIB_SOURCE_DIR}/PWAlgorithmicExpander.cpp
        ${LIB_SOURCE_DIR}/PWGridControlExpander.cpp
        ${LIB_SOURCE_DIR}/PWTapBreakoutExpander.cpp
        ${LIB_SOURCE_DIR}/QARBeatRotatorExpander.cpp
        ${LIB_SOURCE_DIR}/QARConditionalExpander.cpp
        ${LIB_SOURCE_DIR}/QARGridControlExpander.cpp
        ${LIB_SOURCE_DIR}/QARGrooveExpander.cpp
        ${LIB_SOURCE_DIR}/QARIrrationalityExpander.cpp
        ${LIB_SOURCE_DIR}/QARProbabilityExpander.cpp
        ${LIB_SOURCE_DIR}/QARWarpedSpaceExpander.cpp
        ${LIB_SOURCE_DIR}/QARWellFormedRhythmExpander.cpp
        ${LIB_SOURCE_DIR}/QuadAlgorithmicRhythm.cpp
        ${LIB_SOURCE_DIR}/QuantussyCell.cpp
        ${LIB_SOURCE_DIR}/RouletteLFO.cpp
        ${LIB_SOURCE_DIR}/SeedsOfChange.cpp
        ${LIB_SOURCE_DIR}/SeedsOfChangeCVExpander.cpp
        ${LIB_SOURCE_DIR}/SeedsOfChangeGateExpander.cpp
        ${LIB_SOURCE_DIR}/SeriouslySlowEG.cpp
        ${LIB_SOURCE_DIR}/SeriouslySlowLFO.cpp
        ${LIB_SOURCE_DIR}/SliceOfLife.cpp
        ${LIB_SOURCE_DIR}/StringTheory.cpp
        ${LIB_SOURCE_DIR}/TheGardener.cpp
        ${LIB_SOURCE_DIR}/TheOneRingModulator.cpp
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleComp.cpp
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleComp.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleCompProcess.inl
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleEnvelope.cpp
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleEnvelope.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleGain.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleGate.cpp
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleGate.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleGateProcess.inl
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleHeader.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleLimit.cpp
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleLimit.h
        ${LIB_SOURCE_DIR}/dsp-compressor/SimpleLimitProcess.inl
        ${LIB_SOURCE_DIR}/dsp-delay/delayLine.h
        ${LIB_SOURCE_DIR}/dsp-delay/frame.h
        ${LIB_SOURCE_DIR}/dsp-delay/granular.h
        ${LIB_SOURCE_DIR}/dsp-delay/ringbuffer.h
        ${LIB_SOURCE_DIR}/dsp-delay/windowFunction.h
        ${LIB_SOURCE_DIR}/dsp-filter/StateVariableFilter.h
        ${LIB_SOURCE_DIR}/dsp-filter/MultiModOsc.h
        ${LIB_SOURCE_DIR}/dsp-filter/SawOscillator.h
        ${LIB_SOURCE_DIR}/dsp-filter/SinOscillator.h
        ${LIB_SOURCE_DIR}/dsp-filter/DspFilter.cpp
        ${LIB_SOURCE_DIR}/dsp-filter/DspFilter.h
        ${LIB_SOURCE_DIR}/dsp-filter/AudioMath.cpp
        ${LIB_SOURCE_DIR}/dsp-filter/AudioMath.h
        ${LIB_SOURCE_DIR}/dsp-noise/base.hpp
        ${LIB_SOURCE_DIR}/dsp-noise/noise.cpp
        ${LIB_SOURCE_DIR}/dsp-noise/noise.hpp
        ${LIB_SOURCE_DIR}/filters/biquad.cpp
        ${LIB_SOURCE_DIR}/filters/biquad.h
        ${LIB_SOURCE_DIR}/filters/Compressor.hpp
        ${LIB_SOURCE_DIR}/model/Cells.hpp
        ${LIB_SOURCE_DIR}/model/ChristoffelWords.hpp
        ${LIB_SOURCE_DIR}/ui/buttons.hpp
        ${LIB_SOURCE_DIR}/ui/display.hpp
        ${LIB_SOURCE_DIR}/ui/knobs.hpp
        ${LIB_SOURCE_DIR}/ui/light.hpp
        ${LIB_SOURCE_DIR}/ui/fw_menu.cpp
        ${LIB_SOURCE_DIR}/ui/fw_menu.hpp
        ${LIB_SOURCE_DIR}/ui/ports.hpp
        ${LIB_SOURCE_DIR}/ui/shadow.hpp
        ${LIB_SOURCE_DIR}/VoxInhumana.cpp
        ${LIB_SOURCE_DIR}/VoxInhumanaExpander.cpp
        ${LIB_SOURCE_DIR}/xml/tinyxml2.cpp
        ${LIB_SOURCE_DIR}/xml/tinyxml2.h
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp-compressor)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp-delay)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp-filter)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp-noise)

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

    set(VCV_NAME "FrozenWasteland")
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
