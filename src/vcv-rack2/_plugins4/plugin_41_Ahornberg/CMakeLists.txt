set(LIB_NAME plugin_41_Ahornberg)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_H ${LIB_SOURCE_DIR}/*.h)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/Ahornberg.cpp
        ${LIB_SOURCE_DIR}/Ahornberg.hpp
      # ${LIB_SOURCE_DIR}/include
        ${LIB_SOURCE_DIR}/include/AudioFile.h
        ${LIB_SOURCE_DIR}/include/ExternalWindow.h
        ${LIB_SOURCE_DIR}/include/WindowManager.h
      # ${LIB_SOURCE_DIR}/modules
      # ${LIB_SOURCE_DIR}/modules/CopyPastePreset
        ${LIB_SOURCE_DIR}/modules/CopyPastePreset/CopyPastePreset.cpp
        ${LIB_SOURCE_DIR}/modules/CopyPastePreset/CopyPastePreset.hpp
        ${LIB_SOURCE_DIR}/modules/CopyPastePreset/CopyPastePresetWidget.cpp
        ${LIB_SOURCE_DIR}/modules/CopyPastePreset/CopyPastePresetWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/CVFreqShift
        ${LIB_SOURCE_DIR}/modules/CVFreqShift/CVFreqShift.cpp
        ${LIB_SOURCE_DIR}/modules/CVFreqShift/CVFreqShift.hpp
        ${LIB_SOURCE_DIR}/modules/CVFreqShift/CVFreqShiftWidget.cpp
        ${LIB_SOURCE_DIR}/modules/CVFreqShift/CVFreqShiftWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/EqualDivision
        ${LIB_SOURCE_DIR}/modules/EqualDivision/EqualDivision.cpp
        ${LIB_SOURCE_DIR}/modules/EqualDivision/EqualDivision.hpp
        ${LIB_SOURCE_DIR}/modules/EqualDivision/EqualDivisionWidget.cpp
        ${LIB_SOURCE_DIR}/modules/EqualDivision/EqualDivisionWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/FlyingFader
        ${LIB_SOURCE_DIR}/modules/FlyingFader/FlyingFader.cpp
        ${LIB_SOURCE_DIR}/modules/FlyingFader/FlyingFader.hpp
        ${LIB_SOURCE_DIR}/modules/FlyingFader/FlyingFaderWidget.cpp
        ${LIB_SOURCE_DIR}/modules/FlyingFader/FlyingFaderWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/FunWithFrames
        ${LIB_SOURCE_DIR}/modules/FunWithFrames/FunWithFrames.cpp
        ${LIB_SOURCE_DIR}/modules/FunWithFrames/FunWithFrames.hpp
        ${LIB_SOURCE_DIR}/modules/FunWithFrames/FunWithFramesWidget.cpp
        ${LIB_SOURCE_DIR}/modules/FunWithFrames/FunWithFramesWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Harmonizer
        ${LIB_SOURCE_DIR}/modules/Harmonizer/Harmonizer.cpp
        ${LIB_SOURCE_DIR}/modules/Harmonizer/Harmonizer.hpp
        ${LIB_SOURCE_DIR}/modules/Harmonizer/HarmonizerWidget.cpp
        ${LIB_SOURCE_DIR}/modules/Harmonizer/HarmonizerWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Metronome
        ${LIB_SOURCE_DIR}/modules/Metronome/Metronome.cpp
        ${LIB_SOURCE_DIR}/modules/Metronome/Metronome.hpp
        ${LIB_SOURCE_DIR}/modules/Metronome/MetronomeWidget.cpp
        ${LIB_SOURCE_DIR}/modules/Metronome/MetronomeWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/MIDIController
        ${LIB_SOURCE_DIR}/modules/MIDIController/MIDIController.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIController/MIDIController.hpp
        ${LIB_SOURCE_DIR}/modules/MIDIController/MIDIControllerWidget.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIController/MIDIControllerWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/MIDIOverAudio
        ${LIB_SOURCE_DIR}/modules/MIDIOverAudio/MIDIOverAudio.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIOverAudio/MIDIOverAudio.hpp
        ${LIB_SOURCE_DIR}/modules/MIDIOverAudio/MIDIOverAudioWidget.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIOverAudio/MIDIOverAudioWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/MIDIPolyExpression
        ${LIB_SOURCE_DIR}/modules/MIDIPolyExpression/MIDIPolyExpression.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIPolyExpression/MIDIPolyExpression.hpp
        ${LIB_SOURCE_DIR}/modules/MIDIPolyExpression/MIDIPolyExpressionWidget.cpp
        ${LIB_SOURCE_DIR}/modules/MIDIPolyExpression/MIDIPolyExpressionWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/MoreFunWithFrames
        ${LIB_SOURCE_DIR}/modules/MoreFunWithFrames/MoreFunWithFrames.cpp
        ${LIB_SOURCE_DIR}/modules/MoreFunWithFrames/MoreFunWithFrames.hpp
        ${LIB_SOURCE_DIR}/modules/MoreFunWithFrames/MoreFunWithFramesWidget.cpp
        ${LIB_SOURCE_DIR}/modules/MoreFunWithFrames/MoreFunWithFramesWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/TapeRecorder
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeRecorder.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeRecorder.hpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeRecorderWidget.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeRecorderWidget.hpp      
      # ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeInspector
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeInspector/TapeInspector.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeInspector/TapeInspector.hpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeInspector/TapeInspectorWidget.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorder/TapeInspector/TapeInspectorWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/TapeRecorderMixer
        ${LIB_SOURCE_DIR}/modules/TapeRecorderMixer/TapeRecorderMixer.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorderMixer/TapeRecorderMixer.hpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorderMixer/TapeRecorderMixerWidget.cpp
        ${LIB_SOURCE_DIR}/modules/TapeRecorderMixer/TapeRecorderMixerWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Tracker
        ${LIB_SOURCE_DIR}/modules/Tracker/Tracker.cpp
        ${LIB_SOURCE_DIR}/modules/Tracker/Tracker.hpp
        ${LIB_SOURCE_DIR}/modules/Tracker/TrackerWidget.cpp
        ${LIB_SOURCE_DIR}/modules/Tracker/TrackerWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/XGlide
        ${LIB_SOURCE_DIR}/modules/XGlide/XGlide.cpp
        ${LIB_SOURCE_DIR}/modules/XGlide/XGlide.hpp
        ${LIB_SOURCE_DIR}/modules/XGlide/XGlideWidget.cpp
        ${LIB_SOURCE_DIR}/modules/XGlide/XGlideWidget.hpp
      # ${LIB_SOURCE_DIR}/test
        ${LIB_SOURCE_DIR}/test/Asserts.cpp
        ${LIB_SOURCE_DIR}/test/Asserts.hpp
      # ${LIB_SOURCE_DIR}/util
        ${LIB_SOURCE_DIR}/util/Colors.hpp
        ${LIB_SOURCE_DIR}/util/Displays.cpp
        ${LIB_SOURCE_DIR}/util/Displays.hpp
        ${LIB_SOURCE_DIR}/util/Drivers.cpp
        ${LIB_SOURCE_DIR}/util/Drivers.hpp
        ${LIB_SOURCE_DIR}/util/Fonts.hpp
        ${LIB_SOURCE_DIR}/util/Knobs.cpp
        ${LIB_SOURCE_DIR}/util/Knobs.hpp
        ${LIB_SOURCE_DIR}/util/Ports.cpp
        ${LIB_SOURCE_DIR}/util/Ports.hpp
        ${LIB_SOURCE_DIR}/util/Switches.cpp
        ${LIB_SOURCE_DIR}/util/Switches.hpp
        ${LIB_SOURCE_DIR}/util/Widgets.cpp
        ${LIB_SOURCE_DIR}/util/Widgets.hpp
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

    set(VCV_NAME "Ahornberg")
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
