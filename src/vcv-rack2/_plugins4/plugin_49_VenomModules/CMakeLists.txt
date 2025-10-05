set(LIB_NAME plugin_49_VenomModules)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})  
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/AD_ASR.cpp
        ${LIB_SOURCE_DIR}/AuxClone.cpp
        ${LIB_SOURCE_DIR}/BayInput.cpp
        ${LIB_SOURCE_DIR}/BayModule.hpp
        ${LIB_SOURCE_DIR}/BayNorm.cpp
        ${LIB_SOURCE_DIR}/BayOutput.cpp
        ${LIB_SOURCE_DIR}/BenjolinGatesExpander.cpp
        ${LIB_SOURCE_DIR}/BenjolinModule.hpp
        ${LIB_SOURCE_DIR}/BenjolinOsc.cpp
        ${LIB_SOURCE_DIR}/BenjolinVoltsExpander.cpp
        ${LIB_SOURCE_DIR}/BernoulliSwitch.cpp
        ${LIB_SOURCE_DIR}/BernoulliSwitchExpander.cpp
        ${LIB_SOURCE_DIR}/BernoulliSwitchExpander.hpp
        ${LIB_SOURCE_DIR}/Blocker.cpp
        ${LIB_SOURCE_DIR}/Bypass.cpp
        ${LIB_SOURCE_DIR}/CloneMerge.cpp
        ${LIB_SOURCE_DIR}/CloneModule.hpp
        ${LIB_SOURCE_DIR}/Compare2.cpp
        ${LIB_SOURCE_DIR}/CrossFade3D.cpp
        ${LIB_SOURCE_DIR}/Filter.hpp
        ${LIB_SOURCE_DIR}/HQ.cpp
      # ${LIB_SOURCE_DIR}/HQ.data
        ${LIB_SOURCE_DIR}/Knob5.cpp
        ${LIB_SOURCE_DIR}/LinearBeats.cpp
        ${LIB_SOURCE_DIR}/LinearBeatsExpander.cpp
        ${LIB_SOURCE_DIR}/LinearBeatsExpander.hpp
        ${LIB_SOURCE_DIR}/Logic.cpp
        ${LIB_SOURCE_DIR}/math.hpp
        ${LIB_SOURCE_DIR}/Mix4.cpp
        ${LIB_SOURCE_DIR}/Mix4Stereo.cpp
        ${LIB_SOURCE_DIR}/MixFade.cpp
        ${LIB_SOURCE_DIR}/MixFade2.cpp
        ${LIB_SOURCE_DIR}/MixModule.hpp
        ${LIB_SOURCE_DIR}/MixMute.cpp
        ${LIB_SOURCE_DIR}/MixOffset.cpp
        ${LIB_SOURCE_DIR}/MixPan.cpp
        ${LIB_SOURCE_DIR}/MixSend.cpp
        ${LIB_SOURCE_DIR}/MixSolo.cpp
        ${LIB_SOURCE_DIR}/MousePad.cpp
        ${LIB_SOURCE_DIR}/MultiMerge.cpp
        ${LIB_SOURCE_DIR}/MultiSplit.cpp
        ${LIB_SOURCE_DIR}/NORS-IQ.cpp
        ${LIB_SOURCE_DIR}/NORSIQChord2Scale.cpp
        ${LIB_SOURCE_DIR}/Oscillator.cpp
        ${LIB_SOURCE_DIR}/Pan3D.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/PolyClone.cpp
        ${LIB_SOURCE_DIR}/PolyFade.cpp
        ${LIB_SOURCE_DIR}/PolyOffset.cpp
        ${LIB_SOURCE_DIR}/PolyScale.cpp
        ${LIB_SOURCE_DIR}/PolySHASR.cpp
        ${LIB_SOURCE_DIR}/PolyUnison.cpp
        ${LIB_SOURCE_DIR}/Push5.cpp
        ${LIB_SOURCE_DIR}/QuadVCPolarizer.cpp
        ${LIB_SOURCE_DIR}/Recurse.cpp
        ${LIB_SOURCE_DIR}/RecurseStereo.cpp
        ${LIB_SOURCE_DIR}/Reformation.cpp
        ${LIB_SOURCE_DIR}/RhythmExplorer.cpp
        ${LIB_SOURCE_DIR}/ShapedVCA.cpp
        ${LIB_SOURCE_DIR}/Slew.cpp
        ${LIB_SOURCE_DIR}/SphereToXYZ.cpp
        ${LIB_SOURCE_DIR}/TaskWorker.hpp
        ${LIB_SOURCE_DIR}/Thru.cpp
        ${LIB_SOURCE_DIR}/VCAMix4.cpp
        ${LIB_SOURCE_DIR}/VCAMix4Stereo.cpp
        ${LIB_SOURCE_DIR}/VCOUnit.cpp
        ${LIB_SOURCE_DIR}/VenomBlank.cpp
        ${LIB_SOURCE_DIR}/WaveFolder.cpp
        ${LIB_SOURCE_DIR}/WaveMangler.cpp
        ${LIB_SOURCE_DIR}/WaveMultiplier.cpp
        ${LIB_SOURCE_DIR}/WidgetMenuExtender.cpp
        ${LIB_SOURCE_DIR}/WinComp.cpp
        ${LIB_SOURCE_DIR}/XM_OP.cpp
      # ${LIB_SOURCE_DIR}/XM_OP.data
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

    set(VCV_NAME "VenomModules")
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
