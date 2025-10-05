set(LIB_NAME plugin_51_Sickozell)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/Adder8.cpp
        ${LIB_SOURCE_DIR}/AdMini.cpp
        ${LIB_SOURCE_DIR}/Bgates.cpp
        ${LIB_SOURCE_DIR}/Blender.cpp
        ${LIB_SOURCE_DIR}/Blender8.cpp
        ${LIB_SOURCE_DIR}/Btoggler.cpp
        ${LIB_SOURCE_DIR}/BtogglerPlus.cpp
        ${LIB_SOURCE_DIR}/BtogglerSt.cpp
        ${LIB_SOURCE_DIR}/BtogglerStCompact.cpp
        ${LIB_SOURCE_DIR}/Calcs.cpp
        ${LIB_SOURCE_DIR}/Clocker.cpp
        ${LIB_SOURCE_DIR}/Clocker2.cpp
        ${LIB_SOURCE_DIR}/Controls.hpp
        ${LIB_SOURCE_DIR}/CVmeter.cpp
        ${LIB_SOURCE_DIR}/CVrouter.cpp
        ${LIB_SOURCE_DIR}/CVswitcher.cpp
        ${LIB_SOURCE_DIR}/Drummer.cpp
        ${LIB_SOURCE_DIR}/Drummer4.cpp
        ${LIB_SOURCE_DIR}/Drummer4Plus.cpp
        ${LIB_SOURCE_DIR}/DrumPlayer.cpp
        ${LIB_SOURCE_DIR}/DrumPlayerMini.cpp
        ${LIB_SOURCE_DIR}/DrumPlayerMk2.cpp
        ${LIB_SOURCE_DIR}/DrumPlayerPlus.cpp
        ${LIB_SOURCE_DIR}/DrumPlayerXtra.cpp
        ${LIB_SOURCE_DIR}/Enver.cpp
        ${LIB_SOURCE_DIR}/EnverMini.cpp
        ${LIB_SOURCE_DIR}/EnverMiniExp.hpp
        ${LIB_SOURCE_DIR}/EnverMiniX.cpp
      # ${LIB_SOURCE_DIR}/expTable.hpp.old
        ${LIB_SOURCE_DIR}/Holder.cpp
        ${LIB_SOURCE_DIR}/Holder8.cpp
        ${LIB_SOURCE_DIR}/HolderCompact.cpp
        ${LIB_SOURCE_DIR}/KeySampler.cpp
        ${LIB_SOURCE_DIR}/Modulator.cpp
        ${LIB_SOURCE_DIR}/Modulator7.cpp
        ${LIB_SOURCE_DIR}/Modulator7Compact.cpp
        ${LIB_SOURCE_DIR}/MultiRouter.cpp
        ${LIB_SOURCE_DIR}/MultiSwitcher.cpp
        ${LIB_SOURCE_DIR}/notes.hpp
        ${LIB_SOURCE_DIR}/Parking.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/PolyMuter16.cpp
        ${LIB_SOURCE_DIR}/PolyMuter16Plus.cpp
        ${LIB_SOURCE_DIR}/PolyMuter8.cpp
        ${LIB_SOURCE_DIR}/PolyMuter8Plus.cpp
        ${LIB_SOURCE_DIR}/RandLoops.cpp
        ${LIB_SOURCE_DIR}/RandLoops8.cpp
        ${LIB_SOURCE_DIR}/RandLoopsMini.cpp
        ${LIB_SOURCE_DIR}/RandMod7.cpp
        ${LIB_SOURCE_DIR}/RandMod7Compact.cpp
        ${LIB_SOURCE_DIR}/randModTable.hpp
        ${LIB_SOURCE_DIR}/SampleDelay.cpp
        "${LIB_SOURCE_DIR}/shapes - expon.hpp"
        "${LIB_SOURCE_DIR}/shapes - old.hpp"
        ${LIB_SOURCE_DIR}/shapes.hpp
        ${LIB_SOURCE_DIR}/Shifter.cpp
        ${LIB_SOURCE_DIR}/SickoAmp.cpp
        ${LIB_SOURCE_DIR}/SickoCrosser.cpp
        ${LIB_SOURCE_DIR}/SickoCrosser4.cpp
        ${LIB_SOURCE_DIR}/SickoLooper1.cpp
        ${LIB_SOURCE_DIR}/SickoLooper1Exp.cpp
        ${LIB_SOURCE_DIR}/SickoLooper1Exp.hpp
        ${LIB_SOURCE_DIR}/SickoLooper3.cpp
        ${LIB_SOURCE_DIR}/SickoLooper5.cpp
        ${LIB_SOURCE_DIR}/SickoPlayer.cpp
        ${LIB_SOURCE_DIR}/SickoQuant.cpp
        ${LIB_SOURCE_DIR}/SickoQuant4.cpp
        ${LIB_SOURCE_DIR}/SickoSampler.cpp
        ${LIB_SOURCE_DIR}/SickoSampler2.cpp
        ${LIB_SOURCE_DIR}/SimpleSeq4.cpp
        ${LIB_SOURCE_DIR}/Slewer.cpp
        ${LIB_SOURCE_DIR}/SlewerMini.cpp
        ${LIB_SOURCE_DIR}/StepSeq.cpp
        ${LIB_SOURCE_DIR}/StepSeq8x.cpp
        ${LIB_SOURCE_DIR}/StepSeqPlus.cpp
        ${LIB_SOURCE_DIR}/StepStation.cpp
        ${LIB_SOURCE_DIR}/StepStation_def.hpp
        ${LIB_SOURCE_DIR}/StepStation_init.hpp
        ${LIB_SOURCE_DIR}/StepStation_json.hpp
        ${LIB_SOURCE_DIR}/Switcher.cpp
        ${LIB_SOURCE_DIR}/Switcher8.cpp
        ${LIB_SOURCE_DIR}/SwitcherSt.cpp
        ${LIB_SOURCE_DIR}/Toggler.cpp
        ${LIB_SOURCE_DIR}/TogglerCompact.cpp
        ${LIB_SOURCE_DIR}/TrigSeq.cpp
        ${LIB_SOURCE_DIR}/TrigSeq8x.cpp
        ${LIB_SOURCE_DIR}/TrigSeqPlus.cpp
        ${LIB_SOURCE_DIR}/TrigStation.cpp
        ${LIB_SOURCE_DIR}/TrigStation_def.hpp
        ${LIB_SOURCE_DIR}/TrigStation_init.hpp
        ${LIB_SOURCE_DIR}/TrigStation_json.hpp
        ${LIB_SOURCE_DIR}/Wavetabler.cpp
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
    target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)
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

    set(VCV_NAME "Sickozell")
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
