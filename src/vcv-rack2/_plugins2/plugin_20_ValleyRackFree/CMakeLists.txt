set(LIB_NAME plugin_20_ValleyRackFree)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/valley_sse_include.h
        ${LIB_SOURCE_DIR}/Valley.cpp
        ${LIB_SOURCE_DIR}/Valley.hpp
        ${LIB_SOURCE_DIR}/Amalgam/Amalgam.cpp
        ${LIB_SOURCE_DIR}/Amalgam/Amalgam.hpp
        ${LIB_SOURCE_DIR}/Amalgam/VecAmalgam.cpp
        ${LIB_SOURCE_DIR}/Amalgam/VecAmalgam.hpp
        # ${LIB_SOURCE_DIR}/Amalgam/VecAmalgamOld.cpp.old
        ${LIB_SOURCE_DIR}/dep/dr_wav.h
        ${LIB_SOURCE_DIR}/Dexter/Chords.cpp
        ${LIB_SOURCE_DIR}/Dexter/Chords.hpp
        ${LIB_SOURCE_DIR}/Dexter/Dexter.cpp
        ${LIB_SOURCE_DIR}/Dexter/Dexter.hpp
        ${LIB_SOURCE_DIR}/Dexter/DexterCore.cpp
        ${LIB_SOURCE_DIR}/Dexter/DexterCore.hpp
        ${LIB_SOURCE_DIR}/Dexter/DexterRoutingMatrix.cpp
        ${LIB_SOURCE_DIR}/Dexter/DexterRoutingMatrix.hpp
        ${LIB_SOURCE_DIR}/Dexter/DexterWavetableROM.hpp
        ${LIB_SOURCE_DIR}/dsp/delays/AllpassFilter.hpp
        ${LIB_SOURCE_DIR}/dsp/delays/InterpDelay.hpp
        ${LIB_SOURCE_DIR}/dsp/delays/MultiTapAllpassFilter.hpp
        ${LIB_SOURCE_DIR}/dsp/delays/MultiTapInterpDelay.hpp
        ${LIB_SOURCE_DIR}/dsp/filters/OnePoleFilters.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/OnePoleFilters.hpp
        ${LIB_SOURCE_DIR}/dsp/filters/OTAFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/OTAFilter.hpp
        ${LIB_SOURCE_DIR}/dsp/filters/VecLPG.hpp
        ${LIB_SOURCE_DIR}/dsp/filters/VecOnePoleFilters.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/VecOnePoleFilters.hpp
        ${LIB_SOURCE_DIR}/dsp/filters/VecOTAFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/VecOTAFilter.hpp
        ${LIB_SOURCE_DIR}/dsp/fx/DiodeRingMod.cpp
        ${LIB_SOURCE_DIR}/dsp/fx/DiodeRingMod.hpp
        ${LIB_SOURCE_DIR}/dsp/fx/VecDiodeRingMod.cpp
        ${LIB_SOURCE_DIR}/dsp/fx/VecDiodeRingMod.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/DOsc.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/Noise.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/QuadOsc.cpp
        ${LIB_SOURCE_DIR}/dsp/generators/QuadOsc.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/TFormSubOsc.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/VecDirectOsc.hpp
        ${LIB_SOURCE_DIR}/dsp/generators/VecSineLFO.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/DADSR.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/DLFO.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/LFO.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/LinearEnvelope.cpp
        ${LIB_SOURCE_DIR}/dsp/modulation/LinearEnvelope.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/VecAREnvelope.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/VecLoopingADSR.cpp
        ${LIB_SOURCE_DIR}/dsp/modulation/VecLoopingADSR.hpp
        ${LIB_SOURCE_DIR}/dsp/modulation/VecSegment.hpp
        ${LIB_SOURCE_DIR}/dsp/shaping/Enhancer.hpp
        ${LIB_SOURCE_DIR}/dsp/shaping/NonLinear.hpp
        ${LIB_SOURCE_DIR}/dsp/shaping/Shaper.cpp
        ${LIB_SOURCE_DIR}/dsp/shaping/Shaper.hpp
        ${LIB_SOURCE_DIR}/dsp/shaping/VecNonLinear.hpp
        ${LIB_SOURCE_DIR}/dsp/shaping/VecODE.hpp
        ${LIB_SOURCE_DIR}/Feline/Feline.cpp
        ${LIB_SOURCE_DIR}/Feline/Feline.hpp
        ${LIB_SOURCE_DIR}/gui/DigitalDisplay.cpp
        ${LIB_SOURCE_DIR}/gui/DigitalDisplay.hpp
        ${LIB_SOURCE_DIR}/gui/DynamicMenu.cpp
        ${LIB_SOURCE_DIR}/gui/PlainText.cpp
        ${LIB_SOURCE_DIR}/gui/SteppedSlider.cpp
        ${LIB_SOURCE_DIR}/gui/ValleyChoiceMenu.cpp
        ${LIB_SOURCE_DIR}/gui/ValleyChoiceMenu.hpp
        ${LIB_SOURCE_DIR}/gui/ValleyComponents.hpp
        ${LIB_SOURCE_DIR}/gui/ValleyWidgets.hpp
        ${LIB_SOURCE_DIR}/Interzone/Interzone.cpp
        ${LIB_SOURCE_DIR}/Interzone/Interzone.hpp
        ${LIB_SOURCE_DIR}/Plateau/Dattorro.cpp
        ${LIB_SOURCE_DIR}/Plateau/Dattorro.hpp
        ${LIB_SOURCE_DIR}/Plateau/Plateau.cpp
        ${LIB_SOURCE_DIR}/Plateau/Plateau.hpp
        ${LIB_SOURCE_DIR}/simd/SIMDUtilities.hpp
        ${LIB_SOURCE_DIR}/Terrorform/CellRoutingMatrix.cpp
        ${LIB_SOURCE_DIR}/Terrorform/CellRoutingMatrix.hpp
        ${LIB_SOURCE_DIR}/Terrorform/Terrorform.cpp
        ${LIB_SOURCE_DIR}/Terrorform/Terrorform.hpp
        ${LIB_SOURCE_DIR}/Terrorform/TerrorformWaveBank.hpp
        ${LIB_SOURCE_DIR}/Terrorform/TerrorformWaveTableEditor.cpp
        ${LIB_SOURCE_DIR}/Terrorform/TerrorformWaveTableEditor.hpp
        ${LIB_SOURCE_DIR}/Terrorform/TerrorformWavetableROM.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/ClearMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/ClearMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/CloneMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/CloneMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/LoadMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/LoadMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MainMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MainMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MenuBase.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MenuBase.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MoveMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/MoveMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/QuestionMenu.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/QuestionMenu.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormEditorButton.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormEditorButton.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormEditorChoice.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormEditorChoice.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormEditorGrid.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormTextField.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/TFormTextField.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/ViewPane.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/ViewPane.hpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/WaveDisplay.cpp
        ${LIB_SOURCE_DIR}/Terrorform/WavetableEditor/WaveDisplay.hpp
        ${LIB_SOURCE_DIR}/timers/Metronome.cpp
        ${LIB_SOURCE_DIR}/timers/Metronome.hpp
        ${LIB_SOURCE_DIR}/timers/Oneshot.cpp
        ${LIB_SOURCE_DIR}/timers/Oneshot.hpp
        ${LIB_SOURCE_DIR}/Topograph/Topograph.cpp
        ${LIB_SOURCE_DIR}/Topograph/Topograph.hpp
        ${LIB_SOURCE_DIR}/Topograph/TopographPatternGenerator.cpp
        ${LIB_SOURCE_DIR}/Topograph/TopographPatternGenerator.hpp
        ${LIB_SOURCE_DIR}/Topograph/TopographResources.hpp
        ${LIB_SOURCE_DIR}/uGraph/uGraph.cpp
        ${LIB_SOURCE_DIR}/utilities/Utilities.cpp
        ${LIB_SOURCE_DIR}/utilities/Utilities.hpp
    )

    list(APPEND LIB_SOURCES
        ${LIB_SOURCE_DIR}/_bin/ADD_BANK1.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_BANK2.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_BANK3.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_BANK4.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_SAW.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_SINE.cpp
        ${LIB_SOURCE_DIR}/_bin/ADD_SQR.cpp
        ${LIB_SOURCE_DIR}/_bin/ALTOSAX.cpp
        ${LIB_SOURCE_DIR}/_bin/AM_HARM.cpp
        ${LIB_SOURCE_DIR}/_bin/BASIC.cpp
        ${LIB_SOURCE_DIR}/_bin/BITCRUSH1.cpp
        ${LIB_SOURCE_DIR}/_bin/BITCRUSH2.cpp
        ${LIB_SOURCE_DIR}/_bin/BI_PULSE.cpp
        ${LIB_SOURCE_DIR}/_bin/CELLO_1.cpp
        ${LIB_SOURCE_DIR}/_bin/CELLO_2.cpp
        ${LIB_SOURCE_DIR}/_bin/CHIP_1.cpp
        ${LIB_SOURCE_DIR}/_bin/CHIP_2.cpp
        ${LIB_SOURCE_DIR}/_bin/CHIRP.cpp
        ${LIB_SOURCE_DIR}/_bin/DISTORT.cpp
        ${LIB_SOURCE_DIR}/_bin/EBASS.cpp
        ${LIB_SOURCE_DIR}/_bin/FM1.cpp
        ${LIB_SOURCE_DIR}/_bin/FM2.cpp
        ${LIB_SOURCE_DIR}/_bin/FM3.cpp
        ${LIB_SOURCE_DIR}/_bin/FM4.cpp
        ${LIB_SOURCE_DIR}/_bin/FOLD_SINE.cpp
        ${LIB_SOURCE_DIR}/_bin/GMTRY_1.cpp
        ${LIB_SOURCE_DIR}/_bin/GMTRY_2.cpp
        ${LIB_SOURCE_DIR}/_bin/GMTRY_3.cpp
        ${LIB_SOURCE_DIR}/_bin/GRIT.cpp
        ${LIB_SOURCE_DIR}/_bin/LINEAR.cpp
        ${LIB_SOURCE_DIR}/_bin/OBOE.cpp
        ${LIB_SOURCE_DIR}/_bin/OPAL.cpp
        ${LIB_SOURCE_DIR}/_bin/OVERTONE1.cpp
        ${LIB_SOURCE_DIR}/_bin/OVERTONE2.cpp
        ${LIB_SOURCE_DIR}/_bin/PIANO.cpp
        ${LIB_SOURCE_DIR}/_bin/PLAITS_1.cpp
        ${LIB_SOURCE_DIR}/_bin/PLAITS_2.cpp
        ${LIB_SOURCE_DIR}/_bin/PLAITS_3.cpp
        ${LIB_SOURCE_DIR}/_bin/PLUCK.cpp
        ${LIB_SOURCE_DIR}/_bin/PWM.cpp
        ${LIB_SOURCE_DIR}/_bin/REED.cpp
        ${LIB_SOURCE_DIR}/_bin/RESO_SAW.cpp
        ${LIB_SOURCE_DIR}/_bin/RESO_SQR.cpp
        ${LIB_SOURCE_DIR}/_bin/SAW_GAP1.cpp
        ${LIB_SOURCE_DIR}/_bin/SAW_GAP2.cpp
        ${LIB_SOURCE_DIR}/_bin/SAW_PHASE.cpp
        ${LIB_SOURCE_DIR}/_bin/SINE_HARM.cpp
        ${LIB_SOURCE_DIR}/_bin/SWEEPHARM.cpp
        ${LIB_SOURCE_DIR}/_bin/SYMMETRY.cpp
        ${LIB_SOURCE_DIR}/_bin/TEE_EKS.cpp
        ${LIB_SOURCE_DIR}/_bin/THEREMIN.cpp
        ${LIB_SOURCE_DIR}/_bin/TWO_OPFM1.cpp
        ${LIB_SOURCE_DIR}/_bin/TWO_OPFM2.cpp
        ${LIB_SOURCE_DIR}/_bin/TWO_OP_RAND.cpp
        ${LIB_SOURCE_DIR}/_bin/VIDEOGAME.cpp
        ${LIB_SOURCE_DIR}/_bin/VIOLIN.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_1.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_2.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_3.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_4.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_5.cpp
        ${LIB_SOURCE_DIR}/_bin/VOICE_6.cpp
        ${LIB_SOURCE_DIR}/_bin/VOX_MACH.cpp
        ${LIB_SOURCE_DIR}/_bin/XFADE.cpp
    )

    # file(GLOB_RECURSE BINARIES ${LIB_SOURCE_DIR}/*.bin)
    # set(BINARIES
        # ${LIB_SOURCE_DIR}/ADD_BANK1.bin
        # ${LIB_SOURCE_DIR}/ADD_BANK2.bin
        # ${LIB_SOURCE_DIR}/ADD_BANK3.bin
        # ${LIB_SOURCE_DIR}/ADD_BANK4.bin
        # ${LIB_SOURCE_DIR}/ADD_SAW.bin
        # ${LIB_SOURCE_DIR}/ADD_SINE.bin
        # ${LIB_SOURCE_DIR}/ADD_SQR.bin
        # ${LIB_SOURCE_DIR}/ALTOSAX.bin
        # ${LIB_SOURCE_DIR}/AM_HARM.bin
        # ${LIB_SOURCE_DIR}/BASIC.bin
        # ${LIB_SOURCE_DIR}/BITCRUSH1.bin
        # ${LIB_SOURCE_DIR}/BITCRUSH2.bin
        # ${LIB_SOURCE_DIR}/BI_PULSE.bin
        # ${LIB_SOURCE_DIR}/CELLO_1.bin
        # ${LIB_SOURCE_DIR}/CELLO_2.bin
        # ${LIB_SOURCE_DIR}/CHIP_1.bin
        # ${LIB_SOURCE_DIR}/CHIP_2.bin
        # ${LIB_SOURCE_DIR}/CHIRP.bin
        # ${LIB_SOURCE_DIR}/DISTORT.bin
        # ${LIB_SOURCE_DIR}/EBASS.bin
        # ${LIB_SOURCE_DIR}/FM1.bin
        # ${LIB_SOURCE_DIR}/FM2.bin
        # ${LIB_SOURCE_DIR}/FM3.bin
        # ${LIB_SOURCE_DIR}/FM4.bin
        # ${LIB_SOURCE_DIR}/FOLD_SINE.bin
        # ${LIB_SOURCE_DIR}/GMTRY_1.bin
        # ${LIB_SOURCE_DIR}/GMTRY_2.bin
        # ${LIB_SOURCE_DIR}/GMTRY_3.bin
        # ${LIB_SOURCE_DIR}/GRIT.bin
        # ${LIB_SOURCE_DIR}/LINEAR.bin
        # ${LIB_SOURCE_DIR}/OBOE.bin
        # ${LIB_SOURCE_DIR}/OPAL.bin
        # ${LIB_SOURCE_DIR}/OVERTONE1.bin
        # ${LIB_SOURCE_DIR}/OVERTONE2.bin
        # ${LIB_SOURCE_DIR}/PIANO.bin
        # ${LIB_SOURCE_DIR}/PLAITS_1.bin
        # ${LIB_SOURCE_DIR}/PLAITS_2.bin
        # ${LIB_SOURCE_DIR}/PLAITS_3.bin
        # ${LIB_SOURCE_DIR}/PLUCK.bin
        # ${LIB_SOURCE_DIR}/PWM.bin
        # ${LIB_SOURCE_DIR}/REED.bin
        # ${LIB_SOURCE_DIR}/RESO_SAW.bin
        # ${LIB_SOURCE_DIR}/RESO_SQR.bin
        # ${LIB_SOURCE_DIR}/SAW_GAP1.bin
        # ${LIB_SOURCE_DIR}/SAW_GAP2.bin
        # ${LIB_SOURCE_DIR}/SAW_PHASE.bin
        # ${LIB_SOURCE_DIR}/SINE_HARM.bin
        # ${LIB_SOURCE_DIR}/SWEEPHARM.bin
        # ${LIB_SOURCE_DIR}/SYMMETRY.bin
        # ${LIB_SOURCE_DIR}/TEE_EKS.bin
        # ${LIB_SOURCE_DIR}/THEREMIN.bin
        # ${LIB_SOURCE_DIR}/TWO_OPFM1.bin
        # ${LIB_SOURCE_DIR}/TWO_OPFM2.bin
        # ${LIB_SOURCE_DIR}/TWO_OP_RAND.bin
        # ${LIB_SOURCE_DIR}/VIDEOGAME.bin
        # ${LIB_SOURCE_DIR}/VIOLIN.bin
        # ${LIB_SOURCE_DIR}/VOICE_1.bin
        # ${LIB_SOURCE_DIR}/VOICE_2.bin
        # ${LIB_SOURCE_DIR}/VOICE_3.bin
        # ${LIB_SOURCE_DIR}/VOICE_4.bin
        # ${LIB_SOURCE_DIR}/VOICE_5.bin
        # ${LIB_SOURCE_DIR}/VOICE_6.bin
        # ${LIB_SOURCE_DIR}/VOX_MACH.bin
        # ${LIB_SOURCE_DIR}/XFADE.bin
    # )

    # List your .bin files
    # set(BINARIES
    #     src/ADD_BANK1.bin
    #     src/ADD_BANK2.bin
    #     src/ADD_BANK3.bin
    #     # ... add more as needed
    # )

    # set(BINARY_OBJECTS "")
    # foreach(BIN ${BINARIES})
    #     get_filename_component(BASENAME ${BIN} NAME_WE)
    #     get_filename_component(BIN_DIR ${BIN} DIRECTORY)
    #     set(OBJ "${CMAKE_BINARY_DIR}/binobj/${BASENAME}.bin.o")

    #     add_custom_command(
    #         OUTPUT ${OBJ}
    #         COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/binobj
    #         COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/${BIN_DIR}
    #                 xxd -i ${BASENAME}.bin |
    #                 ${CMAKE_C_COMPILER} -xc -c -o ${OBJ} ${CMAKE_C_FLAGS}
    #         DEPENDS ${BIN}
    #         COMMENT "Embedding ${BIN} as ${OBJ} using cmake -E chdir"
    #         VERBATIM
    #     )

    #     list(APPEND BINARY_OBJECTS ${OBJ})
    # endforeach()

    # set(BINARY_OBJECTS "")
    # set(GENERATED_CPP_FILES "")

    # foreach(BIN ${BINARIES})

        # get_filename_component(BASENAME ${BIN} NAME_WE)

        # set(OUT_DIR ${CMAKE_BINARY_DIR}/binobj)
        # set(OUT_FILE ${OUT_DIR}/${BASENAME}.cpp)

        # # Step 1: Declare how to generate the file
        # add_custom_command(
            # OUTPUT ${OUT_FILE}
            # COMMAND xxd -i ${BIN} > ${OUT_FILE}
            # DEPENDS ${BIN}
        # )

        # # get_filename_component(BASENAME ${BIN} NAME_WE)
        # # get_filename_component(BIN_DIR ${BIN} DIRECTORY)

        # # message("${Yellow}[benni] BASENAME = ${BASENAME}, BIN_DIR = ${BIN_DIR}${ColourReset}")

        # # set(BIN_CPP "${BASENAME}_bin.cpp")
        # # set(BIN_O "${BASENAME}.bin.o")

        # # set(TMP_DIR ${CMAKE_BINARY_DIR}/binobj)
        # # message("${Yellow}[benni] TMP_DIR = ${TMP_DIR}${ColourReset}")

        # # add_custom_command(
        # #     OUTPUT ${TMP_DIR}/${BIN_CPP}
        # #     COMMAND ${CMAKE_COMMAND} -E make_directory ${TMP_DIR}
        # #     COMMAND ${CMAKE_COMMAND} -E chdir ${TMP_DIR} xxd -i ${BIN} > ${BIN_CPP}
        # #     # COMMAND ${CMAKE_C_COMPILER} -xc -c ${BIN_CPP} -o ${BIN_O} ${CMAKE_CPP_FLAGS}
        # #     DEPENDS ${BIN}
        # #     COMMENT "Embedding ${BIN} as ${BIN_O} via ${BIN_CPP}"
        # #     VERBATIM
        # # )

        # # list(APPEND GENERATED_CPP_FILES ${TMP_DIR}/${BIN_CPP})
        # list(APPEND GENERATED_CPP_FILES ${OUT_FILE})
    # endforeach()

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES}) #  ${GENERATED_CPP_FILES}
        target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    # target_link_libraries(${LIB_NAME} PRIVATE ${BINARY_OBJECTS})
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav) # This uses older version!
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)


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

    set(VCV_NAME "ValleyRackFree")
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
