set(LIB_NAME plugin_112_PachdeHcOne)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/a_plugin.cpp
        ${LIB_SOURCE_DIR}/a_plugin.hpp
        ${LIB_SOURCE_DIR}/colors.cpp
        ${LIB_SOURCE_DIR}/colors.hpp
        ${LIB_SOURCE_DIR}/common_layout.hpp
        ${LIB_SOURCE_DIR}/debug_log.hpp
        ${LIB_SOURCE_DIR}/em.hpp
        ${LIB_SOURCE_DIR}/em_device.cpp
        ${LIB_SOURCE_DIR}/em_device.hpp
        ${LIB_SOURCE_DIR}/em_midi.cpp
        ${LIB_SOURCE_DIR}/em_midi.hpp
        ${LIB_SOURCE_DIR}/hc_events.hpp
        ${LIB_SOURCE_DIR}/he_group.cpp
        ${LIB_SOURCE_DIR}/he_group.hpp
        ${LIB_SOURCE_DIR}/misc.cpp
        ${LIB_SOURCE_DIR}/misc.hpp
        ${LIB_SOURCE_DIR}/module_broker.cpp
        ${LIB_SOURCE_DIR}/module_broker.hpp
        ${LIB_SOURCE_DIR}/open_file.cpp
        ${LIB_SOURCE_DIR}/open_file.hpp
        ${LIB_SOURCE_DIR}/presets.cpp
        ${LIB_SOURCE_DIR}/presets.hpp
        ${LIB_SOURCE_DIR}/preset_meta.cpp
        ${LIB_SOURCE_DIR}/preset_meta.hpp
        ${LIB_SOURCE_DIR}/text.cpp
        ${LIB_SOURCE_DIR}/text.hpp
        ${LIB_SOURCE_DIR}/Compress/Compress-ui.cpp
        ${LIB_SOURCE_DIR}/Compress/Compress.cpp
        ${LIB_SOURCE_DIR}/Compress/Compress.hpp
        ${LIB_SOURCE_DIR}/em_types/em_compressor.hpp
        ${LIB_SOURCE_DIR}/em_types/em_pedal.cpp
        ${LIB_SOURCE_DIR}/em_types/em_pedal.hpp
        ${LIB_SOURCE_DIR}/em_types/em_recirculator.hpp
        ${LIB_SOURCE_DIR}/em_types/em_rounding.cpp
        ${LIB_SOURCE_DIR}/em_types/em_rounding.hpp
        ${LIB_SOURCE_DIR}/em_types/em_tilteq.hpp
        ${LIB_SOURCE_DIR}/em_types/em_tuning.cpp
        ${LIB_SOURCE_DIR}/em_types/em_tuning.hpp        
        ${LIB_SOURCE_DIR}/Favorites/Favorites-ui.cpp
        ${LIB_SOURCE_DIR}/Favorites/Favorites.cpp
        ${LIB_SOURCE_DIR}/Favorites/Favorites.hpp
        ${LIB_SOURCE_DIR}/Favorites/preset_file_widget.hpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-draw.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-layout.hpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-menu.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-midi-out.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-midi.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-presets.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-process.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1-ui.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1.cpp
        ${LIB_SOURCE_DIR}/HC-1/HC-1.hpp
        ${LIB_SOURCE_DIR}/HC-1/init_phase.cpp
        ${LIB_SOURCE_DIR}/HC-1/init_phase.hpp
        ${LIB_SOURCE_DIR}/HC-1/midi_input_worker.cpp
        ${LIB_SOURCE_DIR}/HC-1/midi_input_worker.hpp
        ${LIB_SOURCE_DIR}/HC-2/cc_map_widget.cpp
        ${LIB_SOURCE_DIR}/HC-2/cc_map_widget.hpp
        ${LIB_SOURCE_DIR}/HC-2/HC-2-layout.hpp
        ${LIB_SOURCE_DIR}/HC-2/HC-2-ui.cpp
        ${LIB_SOURCE_DIR}/HC-2/HC-2.cpp
        ${LIB_SOURCE_DIR}/HC-2/HC-2.hpp
        ${LIB_SOURCE_DIR}/HC-2/header_widget.hpp
        ${LIB_SOURCE_DIR}/HC-4/HC-4-ui.cpp
        ${LIB_SOURCE_DIR}/HC-4/HC-4.cpp
        ${LIB_SOURCE_DIR}/HC-4/HC-4.hpp
        ${LIB_SOURCE_DIR}/Pedals/Pedals-ui.cpp
        ${LIB_SOURCE_DIR}/Pedals/Pedals.cpp
        ${LIB_SOURCE_DIR}/Pedals/Pedals.hpp        
        ${LIB_SOURCE_DIR}/Round/Round-ui.cpp
        ${LIB_SOURCE_DIR}/Round/Round.cpp
        ${LIB_SOURCE_DIR}/Round/Round.hpp
        ${LIB_SOURCE_DIR}/Round/tuning_ui.hpp
        ${LIB_SOURCE_DIR}/Tilt/Tilt-ui.cpp
        ${LIB_SOURCE_DIR}/Tilt/Tilt.cpp
        ${LIB_SOURCE_DIR}/Tilt/Tilt.hpp
        ${LIB_SOURCE_DIR}/widgets/cc_param.hpp
        ${LIB_SOURCE_DIR}/widgets/components.cpp
        ${LIB_SOURCE_DIR}/widgets/components.hpp
        ${LIB_SOURCE_DIR}/widgets/dsp_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/em_picker.hpp
        ${LIB_SOURCE_DIR}/widgets/favorite_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/indicator_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/label_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/partner_picker.hpp
        ${LIB_SOURCE_DIR}/widgets/pedal_param.hpp
        ${LIB_SOURCE_DIR}/widgets/port.cpp
        ${LIB_SOURCE_DIR}/widgets/port.hpp
        ${LIB_SOURCE_DIR}/widgets/preset_widget.cpp
        ${LIB_SOURCE_DIR}/widgets/preset_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/small_push.cpp
        ${LIB_SOURCE_DIR}/widgets/small_push.hpp
        ${LIB_SOURCE_DIR}/widgets/square_button.hpp
        ${LIB_SOURCE_DIR}/widgets/switch_4.hpp
        ${LIB_SOURCE_DIR}/widgets/symbol_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/tab_bar.hpp
        ${LIB_SOURCE_DIR}/widgets/tip_widget.hpp
        ${LIB_SOURCE_DIR}/widgets/vert_slider.cpp
        ${LIB_SOURCE_DIR}/widgets/vert_slider.hpp
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
    # target_link_libraries(${LIB_NAME} PRIVATE de_opensimplexnoise)
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

    set(VCV_NAME "PachdeHcOne")
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
