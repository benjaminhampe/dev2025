set(LIB_NAME plugin_43_CountModula)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/CountModula.cpp
        ${LIB_SOURCE_DIR}/CountModula.hpp
        ${LIB_SOURCE_DIR}/DeclareModels.hpp
        ${LIB_SOURCE_DIR}/InitialiseModels.hpp        
      # ${LIB_SOURCE_DIR}/components
        ${LIB_SOURCE_DIR}/components/CountModulaComponents.hpp
        ${LIB_SOURCE_DIR}/components/CountModulaKnobs.hpp
        ${LIB_SOURCE_DIR}/components/CountModulaLEDDisplay.hpp
        ${LIB_SOURCE_DIR}/components/CountModulaPushButtons.hpp
        ${LIB_SOURCE_DIR}/components/StdComponentPositions.cpp
        ${LIB_SOURCE_DIR}/components/StdComponentPositions.hpp
        ${LIB_SOURCE_DIR}/components/stdScrews.hpp
      # ${LIB_SOURCE_DIR}/inc
        ${LIB_SOURCE_DIR}/inc/AddExpanderMenuItem.hpp
        ${LIB_SOURCE_DIR}/inc/ClockedRandomGateExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/ClockOscillator.hpp
        ${LIB_SOURCE_DIR}/inc/EuclideanAlgorithm.hpp
        ${LIB_SOURCE_DIR}/inc/EuclidExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/FadeExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/FrequencyDivider.hpp
        ${LIB_SOURCE_DIR}/inc/GateDelayLine.hpp
        ${LIB_SOURCE_DIR}/inc/GateProcessor.hpp
        ${LIB_SOURCE_DIR}/inc/HyperManiacalLFOExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/Inverter.hpp
        ${LIB_SOURCE_DIR}/inc/MegalomaniacControllerMessage.hpp
        ${LIB_SOURCE_DIR}/inc/MixerEngine.hpp
        ${LIB_SOURCE_DIR}/inc/OctetTriggerSequencerExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/Polarizer.hpp
        ${LIB_SOURCE_DIR}/inc/PulseModifier.hpp
        ${LIB_SOURCE_DIR}/inc/SequencerChannelMessage.hpp
        ${LIB_SOURCE_DIR}/inc/SequencerExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/inc/SlewLimiter.hpp
        ${LIB_SOURCE_DIR}/inc/Utility.hpp
      # ${LIB_SOURCE_DIR}/modules
        ${LIB_SOURCE_DIR}/modules/AnalogueShiftRegister.cpp
        ${LIB_SOURCE_DIR}/modules/Arpeggiator.cpp
        ${LIB_SOURCE_DIR}/modules/Attenuator.cpp
        ${LIB_SOURCE_DIR}/modules/Attenuverter.cpp
        ${LIB_SOURCE_DIR}/modules/BarGraph.cpp
        ${LIB_SOURCE_DIR}/modules/BasicSequencer8.cpp
        ${LIB_SOURCE_DIR}/modules/BinaryComparator.cpp
        ${LIB_SOURCE_DIR}/modules/BinarySequencer.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel12HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel16HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel20HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel24HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel2HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel4HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanel8HP.cpp
        ${LIB_SOURCE_DIR}/modules/BlankPanels.hpp
        ${LIB_SOURCE_DIR}/modules/BooleanAND.cpp
        ${LIB_SOURCE_DIR}/modules/BooleanOR.cpp
        ${LIB_SOURCE_DIR}/modules/BooleanVCNOT.cpp
        ${LIB_SOURCE_DIR}/modules/BooleanXOR.cpp
        ${LIB_SOURCE_DIR}/modules/Breakout.cpp
        ${LIB_SOURCE_DIR}/modules/BurstGenerator.cpp
        ${LIB_SOURCE_DIR}/modules/BurstGenerator64.cpp
        ${LIB_SOURCE_DIR}/modules/BusRoute.cpp
        ${LIB_SOURCE_DIR}/modules/BusRoute2.cpp
        ${LIB_SOURCE_DIR}/modules/Carousel.cpp
        ${LIB_SOURCE_DIR}/modules/Chances.cpp
        ${LIB_SOURCE_DIR}/modules/ClockDivider.cpp
        ${LIB_SOURCE_DIR}/modules/ClockedRandomGateExpanderCV.cpp
        ${LIB_SOURCE_DIR}/modules/ClockedRandomGateExpanderLog.cpp
        ${LIB_SOURCE_DIR}/modules/ClockedRandomGates.cpp
        ${LIB_SOURCE_DIR}/modules/Comparator.cpp
        ${LIB_SOURCE_DIR}/modules/CVSpreader.cpp
        ${LIB_SOURCE_DIR}/modules/DFlipFlop.cpp
        ${LIB_SOURCE_DIR}/modules/DrumSequencer16.cpp
        ${LIB_SOURCE_DIR}/modules/Euclid.cpp
        ${LIB_SOURCE_DIR}/modules/EuclidExpanderCV.cpp
        ${LIB_SOURCE_DIR}/modules/EventArranger.cpp
        ${LIB_SOURCE_DIR}/modules/EventTimer.cpp
        ${LIB_SOURCE_DIR}/modules/EventTimer.hpp
        ${LIB_SOURCE_DIR}/modules/EventTimer2.cpp
        ${LIB_SOURCE_DIR}/modules/Fade.cpp
        ${LIB_SOURCE_DIR}/modules/FadeExpander.cpp
        ${LIB_SOURCE_DIR}/modules/G2T.cpp
        ${LIB_SOURCE_DIR}/modules/GatedComparator.cpp
        ${LIB_SOURCE_DIR}/modules/GateDelay.cpp
        ${LIB_SOURCE_DIR}/modules/GateDelayMT.cpp
        ${LIB_SOURCE_DIR}/modules/GateModifier.cpp
        ${LIB_SOURCE_DIR}/modules/GateSequencer16.cpp
        ${LIB_SOURCE_DIR}/modules/GateSequencer8.cpp
        ${LIB_SOURCE_DIR}/modules/GateSequencerSrc.hpp
        ${LIB_SOURCE_DIR}/modules/HyperManiac.cpp
        ${LIB_SOURCE_DIR}/modules/HyperManiacalLFO.cpp
        ${LIB_SOURCE_DIR}/modules/HyperManiacalLFOExpander.cpp
        ${LIB_SOURCE_DIR}/modules/LightStrip.cpp
        ${LIB_SOURCE_DIR}/modules/Mangler.cpp
        ${LIB_SOURCE_DIR}/modules/Manifold.cpp
        ${LIB_SOURCE_DIR}/modules/ManualCV.cpp
        ${LIB_SOURCE_DIR}/modules/ManualCV2.cpp
        ${LIB_SOURCE_DIR}/modules/ManualGate.cpp
        ${LIB_SOURCE_DIR}/modules/MasterReset.cpp
        ${LIB_SOURCE_DIR}/modules/MatrixCombiner.cpp
        ${LIB_SOURCE_DIR}/modules/MatrixMixer.cpp
        ${LIB_SOURCE_DIR}/modules/MinimusMaximus.cpp
        ${LIB_SOURCE_DIR}/modules/Mixer.cpp
        ${LIB_SOURCE_DIR}/modules/MorphShaper.cpp
        ${LIB_SOURCE_DIR}/modules/Mult.cpp
        ${LIB_SOURCE_DIR}/modules/Multiplexer.cpp
        ${LIB_SOURCE_DIR}/modules/MultiStepSequencer.cpp
        ${LIB_SOURCE_DIR}/modules/Mute-iple.cpp
        ${LIB_SOURCE_DIR}/modules/Mute.cpp
        ${LIB_SOURCE_DIR}/modules/NibbleTriggerSequencer.cpp
        ${LIB_SOURCE_DIR}/modules/OctetTriggerSequencer.cpp
        ${LIB_SOURCE_DIR}/modules/OctetTriggerSequencerCVExpander.cpp
        ${LIB_SOURCE_DIR}/modules/OctetTriggerSequencerGateExpander.cpp
        ${LIB_SOURCE_DIR}/modules/OffsetGenerator.cpp
        ${LIB_SOURCE_DIR}/modules/Oscilloscope.cpp
        ${LIB_SOURCE_DIR}/modules/Palette.cpp
        ${LIB_SOURCE_DIR}/modules/PolyChances.cpp
        ${LIB_SOURCE_DIR}/modules/PolyG2T.cpp
        ${LIB_SOURCE_DIR}/modules/PolyGateModifier.cpp
        ${LIB_SOURCE_DIR}/modules/PolyLogic.cpp
        ${LIB_SOURCE_DIR}/modules/PolyMinMax.cpp
        ${LIB_SOURCE_DIR}/modules/PolyMute.cpp
        ${LIB_SOURCE_DIR}/modules/PolyrhythmicGenerator.cpp
        ${LIB_SOURCE_DIR}/modules/PolyrhythmicGeneratorMkII.cpp
        ${LIB_SOURCE_DIR}/modules/PolyVCPolarizer.cpp
        ${LIB_SOURCE_DIR}/modules/PolyVCSwitch.cpp
        ${LIB_SOURCE_DIR}/modules/RackEarL.cpp
        ${LIB_SOURCE_DIR}/modules/RackEarR.cpp
        ${LIB_SOURCE_DIR}/modules/RackEars.hpp
        ${LIB_SOURCE_DIR}/modules/RandomAccessSwitch18.cpp
        ${LIB_SOURCE_DIR}/modules/RandomAccessSwitch81.cpp
        ${LIB_SOURCE_DIR}/modules/Rectifier.cpp
        ${LIB_SOURCE_DIR}/modules/SampleAndHold.cpp
        ${LIB_SOURCE_DIR}/modules/SampleAndHold2.cpp
        ${LIB_SOURCE_DIR}/modules/SequenceEncoder.cpp
        ${LIB_SOURCE_DIR}/modules/Sequencer16.cpp
        ${LIB_SOURCE_DIR}/modules/Sequencer64.cpp
        ${LIB_SOURCE_DIR}/modules/Sequencer8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerChannel16.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerChannel8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerChannelSrc.hpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderCV8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderLog8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderOut8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderRM8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderTrig8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerExpanderTSG.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerGates16.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerGates8.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerGatesSrc.hpp
        ${LIB_SOURCE_DIR}/modules/SequencerSrc.hpp
        ${LIB_SOURCE_DIR}/modules/SequencerTriggers16.cpp
        ${LIB_SOURCE_DIR}/modules/SequencerTriggers8.cpp
        ${LIB_SOURCE_DIR}/modules/ShepardGenerator.cpp
        ${LIB_SOURCE_DIR}/modules/ShiftRegister16.cpp
        ${LIB_SOURCE_DIR}/modules/ShiftRegister32.cpp
        ${LIB_SOURCE_DIR}/modules/ShiftRegisterSrc.hpp
        ${LIB_SOURCE_DIR}/modules/SlopeDetector.cpp
        ${LIB_SOURCE_DIR}/modules/SRFlipFlop.cpp
        ${LIB_SOURCE_DIR}/modules/Stack.cpp
        ${LIB_SOURCE_DIR}/modules/StartupDelay.cpp
        ${LIB_SOURCE_DIR}/modules/StepSequencer8.cpp
        ${LIB_SOURCE_DIR}/modules/SubHarmonicGenerator.cpp
        ${LIB_SOURCE_DIR}/modules/Switch16To1.cpp
        ${LIB_SOURCE_DIR}/modules/Switch1To16.cpp
        ${LIB_SOURCE_DIR}/modules/Switch1To8.cpp
        ${LIB_SOURCE_DIR}/modules/Switch2.cpp
        ${LIB_SOURCE_DIR}/modules/Switch3.cpp
        ${LIB_SOURCE_DIR}/modules/Switch4.cpp
        ${LIB_SOURCE_DIR}/modules/Switch8To1.cpp
        ${LIB_SOURCE_DIR}/modules/SwitchNToNSrc.hpp
        ${LIB_SOURCE_DIR}/modules/TFlipFlop.cpp
        ${LIB_SOURCE_DIR}/modules/TriggerSequencer16.cpp
        ${LIB_SOURCE_DIR}/modules/TriggerSequencer8.cpp
        ${LIB_SOURCE_DIR}/modules/TriggerSequencerSrc.hpp
        ${LIB_SOURCE_DIR}/modules/VCFrequencyDivider.cpp
        ${LIB_SOURCE_DIR}/modules/VCFrequencyDividerMkII.cpp
        ${LIB_SOURCE_DIR}/modules/VCPolarizer.cpp
        ${LIB_SOURCE_DIR}/modules/VCPulseDivider.cpp
        ${LIB_SOURCE_DIR}/modules/VoltageControlledSwitch.cpp
        ${LIB_SOURCE_DIR}/modules/VoltageInverter.cpp
        ${LIB_SOURCE_DIR}/modules/VoltageScaler.cpp
      # ${LIB_SOURCE_DIR}/themes
        ${LIB_SOURCE_DIR}/themes/dataFromJson.hpp
        ${LIB_SOURCE_DIR}/themes/dataToJson.hpp
        ${LIB_SOURCE_DIR}/themes/setDefaultTheme.hpp
        ${LIB_SOURCE_DIR}/themes/setPanel.hpp
        ${LIB_SOURCE_DIR}/themes/step.hpp
        ${LIB_SOURCE_DIR}/themes/ThemeMenuItem.hpp
        ${LIB_SOURCE_DIR}/themes/themeMenus.hpp
        ${LIB_SOURCE_DIR}/themes/variables.hpp
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

    set(VCV_NAME "CountModula")
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
