set(LIB_NAME plugin_57_Stoermelder-P1)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/a_plugin.cpp
        ${LIB_SOURCE_DIR}/a_plugin.hpp
        ${LIB_SOURCE_DIR}/Affix.cpp
        ${LIB_SOURCE_DIR}/Arena.cpp
        ${LIB_SOURCE_DIR}/AudioInterface64.cpp
        ${LIB_SOURCE_DIR}/Bolt.cpp
        ${LIB_SOURCE_DIR}/CVMap.cpp
        ${LIB_SOURCE_DIR}/CVMap.hpp
        ${LIB_SOURCE_DIR}/CVMapCtx.cpp
        ${LIB_SOURCE_DIR}/CVMapMicro.cpp
        ${LIB_SOURCE_DIR}/CVPam.cpp
        ${LIB_SOURCE_DIR}/Dirt.cpp
        ${LIB_SOURCE_DIR}/EightFace.cpp
        ${LIB_SOURCE_DIR}/EightFace.hpp
        ${LIB_SOURCE_DIR}/EightFaceMk2.cpp
        ${LIB_SOURCE_DIR}/EightFaceMk2Base.hpp
        ${LIB_SOURCE_DIR}/EightFaceMk2Ex.cpp
        ${LIB_SOURCE_DIR}/FourRounds.cpp
        ${LIB_SOURCE_DIR}/Glue.cpp
        ${LIB_SOURCE_DIR}/Goto.cpp
        ${LIB_SOURCE_DIR}/Grip.cpp
        ${LIB_SOURCE_DIR}/Hive.cpp
        ${LIB_SOURCE_DIR}/HiveGrid.hpp
        ${LIB_SOURCE_DIR}/Infix.cpp
        ${LIB_SOURCE_DIR}/Intermix.cpp
        ${LIB_SOURCE_DIR}/IntermixBase.hpp
        ${LIB_SOURCE_DIR}/IntermixEnv.cpp
        ${LIB_SOURCE_DIR}/IntermixFade.cpp
        ${LIB_SOURCE_DIR}/IntermixGate.cpp
        ${LIB_SOURCE_DIR}/Macro.cpp
        ${LIB_SOURCE_DIR}/MapModuleBase.hpp
        ${LIB_SOURCE_DIR}/Maze.cpp
        ${LIB_SOURCE_DIR}/Mb.cpp
        ${LIB_SOURCE_DIR}/Me.cpp
        ${LIB_SOURCE_DIR}/MidiCat.cpp
        ${LIB_SOURCE_DIR}/MidiCat.hpp
        ${LIB_SOURCE_DIR}/MidiCatClk.cpp
        ${LIB_SOURCE_DIR}/MidiCatCtx.cpp
        ${LIB_SOURCE_DIR}/MidiCatMem.cpp
        ${LIB_SOURCE_DIR}/MidiKey.cpp
        ${LIB_SOURCE_DIR}/MidiMon.cpp
        ${LIB_SOURCE_DIR}/MidiPlug.cpp
        ${LIB_SOURCE_DIR}/MidiStep.cpp
        ${LIB_SOURCE_DIR}/Mirror.cpp
        ${LIB_SOURCE_DIR}/Orbit.cpp
        ${LIB_SOURCE_DIR}/Pile.cpp
        ${LIB_SOURCE_DIR}/PilePoly.cpp
        ${LIB_SOURCE_DIR}/pluginhelpers.hpp
        ${LIB_SOURCE_DIR}/pluginsettings.cpp
        ${LIB_SOURCE_DIR}/pluginsettings.hpp
        ${LIB_SOURCE_DIR}/Raw.cpp
        ${LIB_SOURCE_DIR}/ReMove.cpp
        ${LIB_SOURCE_DIR}/RotorA.cpp
        ${LIB_SOURCE_DIR}/Sail.cpp
        ${LIB_SOURCE_DIR}/Sipo.cpp
        ${LIB_SOURCE_DIR}/Spin.cpp
        ${LIB_SOURCE_DIR}/Strip.cpp
        ${LIB_SOURCE_DIR}/Strip.hpp
        ${LIB_SOURCE_DIR}/StripBay.cpp
        ${LIB_SOURCE_DIR}/StripPp.cpp
        ${LIB_SOURCE_DIR}/Stroke.cpp
        ${LIB_SOURCE_DIR}/Transit.cpp
        ${LIB_SOURCE_DIR}/TransitBase.hpp
        ${LIB_SOURCE_DIR}/TransitEx.cpp
        ${LIB_SOURCE_DIR}/X4.cpp
        ${LIB_SOURCE_DIR}/components/components.hpp
        ${LIB_SOURCE_DIR}/components/CurveMenuItem.hpp
        ${LIB_SOURCE_DIR}/components/Knobs.hpp
        ${LIB_SOURCE_DIR}/components/LedDisplayCenterChoiceEx.hpp
        ${LIB_SOURCE_DIR}/components/LedTextDisplay.hpp
        ${LIB_SOURCE_DIR}/components/LedTextField.hpp
        ${LIB_SOURCE_DIR}/components/LogDisplay.hpp
        ${LIB_SOURCE_DIR}/components/MapButton.hpp
        ${LIB_SOURCE_DIR}/components/MatrixButton.hpp
        ${LIB_SOURCE_DIR}/components/MenuColorField.hpp
        ${LIB_SOURCE_DIR}/components/MenuColorLabel.hpp
        ${LIB_SOURCE_DIR}/components/MenuColorPicker.hpp
        ${LIB_SOURCE_DIR}/components/MenuLabelEx.hpp
        ${LIB_SOURCE_DIR}/components/MidiWidget.hpp
        ${LIB_SOURCE_DIR}/components/ParamHandleIndicator.hpp
        ${LIB_SOURCE_DIR}/components/SubMenuSlider.hpp
        ${LIB_SOURCE_DIR}/components/VoltageLedDisplay.hpp
        ${LIB_SOURCE_DIR}/components/XyScreenWidget.hpp
        ${LIB_SOURCE_DIR}/components/XySeqWidget.hpp
        ${LIB_SOURCE_DIR}/digital/digital.hpp
        ${LIB_SOURCE_DIR}/digital/ScaledMapParam.hpp
        ${LIB_SOURCE_DIR}/digital/ShapedSlewLimiter.hpp
        ${LIB_SOURCE_DIR}/helpers/StripIdFixModule.hpp
        ${LIB_SOURCE_DIR}/helpers/TaskProcessor.hpp
        ${LIB_SOURCE_DIR}/helpers/TaskWorker.hpp
        ${LIB_SOURCE_DIR}/mb/Mb.hpp
        ${LIB_SOURCE_DIR}/mb/Mb_v06.hpp
        ${LIB_SOURCE_DIR}/mb/Mb_v1.cpp
        ${LIB_SOURCE_DIR}/mb/Mb_v1.hpp
        ${LIB_SOURCE_DIR}/strippp/SelectionPreview.cpp
        ${LIB_SOURCE_DIR}/ui/keyboard.hpp
        ${LIB_SOURCE_DIR}/ui/ModuleSelectProcessor.hpp
        ${LIB_SOURCE_DIR}/ui/OverlayMessageWidget.hpp
        ${LIB_SOURCE_DIR}/ui/ParamWidgetContextExtender.hpp
        ${LIB_SOURCE_DIR}/ui/ThemedModuleWidget.hpp
        ${LIB_SOURCE_DIR}/ui/ViewportHelper.hpp
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
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)
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

        target_compile_options(${LIB_NAME} PRIVATE -pthread)
        target_link_libraries(${LIB_NAME} PRIVATE -pthread)

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

    set(VCV_NAME "Stoermelder-P1")
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
