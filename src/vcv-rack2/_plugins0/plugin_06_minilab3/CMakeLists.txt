set(LIB_NAME plugin_06_minilab3)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/BaseModule.cpp
        ${LIB_SOURCE_DIR}/BaseModule.hpp
        ${LIB_SOURCE_DIR}/MiniLab.cpp
        ${LIB_SOURCE_DIR}/MiniLab.hpp
        ${LIB_SOURCE_DIR}/MiniLog.cpp
        ${LIB_SOURCE_DIR}/MiniLog.hpp
        ${LIB_SOURCE_DIR}/MiniPad.cpp
        ${LIB_SOURCE_DIR}/MiniPad.hpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp

        ${LIB_SOURCE_DIR}/consts/midi.hpp

        ${LIB_SOURCE_DIR}/logging/MidiMessageFilter.cpp
        ${LIB_SOURCE_DIR}/logging/MidiMessageFilter.hpp
        ${LIB_SOURCE_DIR}/logging/MidiMessageRenderer.cpp
        ${LIB_SOURCE_DIR}/logging/MidiMessageRenderer.hpp

        ${LIB_SOURCE_DIR}/menu/AbsoluteParamMenu.cpp
        ${LIB_SOURCE_DIR}/menu/AbsoluteParamMenu.hpp
        ${LIB_SOURCE_DIR}/menu/MenuSlider.cpp
        ${LIB_SOURCE_DIR}/menu/MenuSlider.hpp
        ${LIB_SOURCE_DIR}/menu/ModularContextMenuItem.cpp
        ${LIB_SOURCE_DIR}/menu/ModularContextMenuItem.hpp
        ${LIB_SOURCE_DIR}/menu/RelativeParamMenu.cpp
        ${LIB_SOURCE_DIR}/menu/RelativeParamMenu.hpp
        ${LIB_SOURCE_DIR}/menu/ResolutionSelector.cpp
        ${LIB_SOURCE_DIR}/menu/ResolutionSelector.hpp
        ${LIB_SOURCE_DIR}/menu/StrengthSelector.cpp
        ${LIB_SOURCE_DIR}/menu/StrengthSelector.hpp
        ${LIB_SOURCE_DIR}/menu/VoltageModeSelector.cpp
        ${LIB_SOURCE_DIR}/menu/VoltageModeSelector.hpp

        ${LIB_SOURCE_DIR}/params/AbsoluteParam.hpp
        ${LIB_SOURCE_DIR}/params/BaseParam.cpp
        ${LIB_SOURCE_DIR}/params/BaseParam.hpp
        ${LIB_SOURCE_DIR}/params/RelativeParam.cpp
        ${LIB_SOURCE_DIR}/params/RelativeParam.hpp

        ${LIB_SOURCE_DIR}/props/CallbackQuantity.cpp
        ${LIB_SOURCE_DIR}/props/CallbackQuantity.hpp
        ${LIB_SOURCE_DIR}/props/Choice.hpp
        ${LIB_SOURCE_DIR}/props/ResolutionChoice.cpp
        ${LIB_SOURCE_DIR}/props/ResolutionChoice.hpp
        ${LIB_SOURCE_DIR}/props/StrengthChoice.cpp
        ${LIB_SOURCE_DIR}/props/StrengthChoice.hpp
        ${LIB_SOURCE_DIR}/props/VoltageModeChoice.cpp
        ${LIB_SOURCE_DIR}/props/VoltageModeChoice.hpp

        ${LIB_SOURCE_DIR}/ui/FancyPanelBorder.cpp
        ${LIB_SOURCE_DIR}/ui/FancyPanelBorder.hpp
        ${LIB_SOURCE_DIR}/ui/LedText.cpp
        ${LIB_SOURCE_DIR}/ui/LedText.hpp
        ${LIB_SOURCE_DIR}/ui/OutputPort.cpp
        ${LIB_SOURCE_DIR}/ui/OutputPort.hpp
        ${LIB_SOURCE_DIR}/ui/SvgHelper.hpp
        ${LIB_SOURCE_DIR}/ui/TextLogWidget.cpp
        ${LIB_SOURCE_DIR}/ui/TextLogWidget.hpp

        ${LIB_SOURCE_DIR}/utils/Clamp.cpp
        ${LIB_SOURCE_DIR}/utils/Clamp.hpp
        ${LIB_SOURCE_DIR}/utils/getBrightness.hpp
        ${LIB_SOURCE_DIR}/utils/padNotes.cpp
        ${LIB_SOURCE_DIR}/utils/padNotes.hpp
        ${LIB_SOURCE_DIR}/utils/paintToColor.cpp
        ${LIB_SOURCE_DIR}/utils/paintToColor.hpp
        ${LIB_SOURCE_DIR}/utils/Pile.cpp
        ${LIB_SOURCE_DIR}/utils/Pile.hpp
        ${LIB_SOURCE_DIR}/utils/Resolution.hpp
        ${LIB_SOURCE_DIR}/utils/Slew.cpp
        ${LIB_SOURCE_DIR}/utils/Slew.hpp
        ${LIB_SOURCE_DIR}/utils/Strength.hpp
        ${LIB_SOURCE_DIR}/utils/units.hpp
        ${LIB_SOURCE_DIR}/utils/VoltageMode.hpp
        ${LIB_SOURCE_DIR}/utils/VoltageRescaler.cpp
        ${LIB_SOURCE_DIR}/utils/VoltageRescaler.hpp

        ${LIB_SOURCE_DIR}/widgets/BaseWidget.hpp
        ${LIB_SOURCE_DIR}/widgets/MiniLabWidget.cpp
        ${LIB_SOURCE_DIR}/widgets/MiniLabWidget.hpp
        ${LIB_SOURCE_DIR}/widgets/MiniLogWidget.cpp
        ${LIB_SOURCE_DIR}/widgets/MiniLogWidget.hpp
        ${LIB_SOURCE_DIR}/widgets/MiniPadWidget.cpp
        ${LIB_SOURCE_DIR}/widgets/MiniPadWidget.hpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/consts)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/logging)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/menu)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/params)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/props)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ui)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/utils)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/widgets)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

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

    # set_target_properties(${LIB_NAME} PROPERTIES
    #     OUTPUT_NAME "Fundamental-2.6.2-llvm-win-x64"
    #     SUFFIX ".dll"
    # )

    set(VCV_NAME "minilab3")
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

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "$<TARGET_FILE:${LIB_NAME}>"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/myplugin.vcvplugin"
    # )

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myplugin.dll"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcvplugins/myplugin.vcvplugin"
    # )

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)

endif()

# add_subdirectory(test)
