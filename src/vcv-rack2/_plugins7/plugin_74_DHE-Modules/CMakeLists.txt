set(LIB_NAME plugin_74_DHE-Modules)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/plugin.cpp    
        ${LIB_SOURCE_DIR}/components/cxmath.h
        ${LIB_SOURCE_DIR}/components/latch.h
        ${LIB_SOURCE_DIR}/components/meta.h
        ${LIB_SOURCE_DIR}/components/phase-rotor.h
        ${LIB_SOURCE_DIR}/components/phase-timer.h
        ${LIB_SOURCE_DIR}/components/range.h
        ${LIB_SOURCE_DIR}/components/sigmoid.h
        ${LIB_SOURCE_DIR}/controls/buttons.h
        ${LIB_SOURCE_DIR}/controls/duration-controls.h
        ${LIB_SOURCE_DIR}/controls/knobs.h
        ${LIB_SOURCE_DIR}/controls/ports.h
        ${LIB_SOURCE_DIR}/controls/shape-controls.h
        ${LIB_SOURCE_DIR}/controls/step-selection-controls.h
        ${LIB_SOURCE_DIR}/controls/switches.h
        ${LIB_SOURCE_DIR}/controls/voltage-controls.h
        ${LIB_SOURCE_DIR}/modules/blossom/bounce-ratio.h
        ${LIB_SOURCE_DIR}/modules/blossom/control-ids.h
        ${LIB_SOURCE_DIR}/modules/blossom/controls.h
        ${LIB_SOURCE_DIR}/modules/blossom/init.cpp
        ${LIB_SOURCE_DIR}/modules/blossom/init.h
        ${LIB_SOURCE_DIR}/modules/blossom/module.h
        ${LIB_SOURCE_DIR}/modules/blossom/panel.h
        ${LIB_SOURCE_DIR}/modules/blossom/spin-speed.h
        ${LIB_SOURCE_DIR}/modules/buttons/control-ids.h
        ${LIB_SOURCE_DIR}/modules/buttons/init.cpp
        ${LIB_SOURCE_DIR}/modules/buttons/init.h
        ${LIB_SOURCE_DIR}/modules/buttons/module.h
        ${LIB_SOURCE_DIR}/modules/buttons/panel.h
        ${LIB_SOURCE_DIR}/modules/cubic/coefficient.h
        ${LIB_SOURCE_DIR}/modules/cubic/control-ids.h
        ${LIB_SOURCE_DIR}/modules/cubic/init.cpp
        ${LIB_SOURCE_DIR}/modules/cubic/init.h
        ${LIB_SOURCE_DIR}/modules/cubic/module.h
        ${LIB_SOURCE_DIR}/modules/cubic/panel.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/advance-mode.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/control-ids.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/engine.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/generate-mode.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/init.cpp
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/init.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/module.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/panel.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/signals.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/step-controller.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/step-event.h
        ${LIB_SOURCE_DIR}/modules/curve-sequencer/step-selector.h
        ${LIB_SOURCE_DIR}/modules/envelope/init.cpp
        ${LIB_SOURCE_DIR}/modules/envelope/init.h
        ${LIB_SOURCE_DIR}/modules/envelope/booster-stage/control-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/booster-stage/module.h
        ${LIB_SOURCE_DIR}/modules/envelope/booster-stage/panel.h
        ${LIB_SOURCE_DIR}/modules/envelope/booster-stage/signals.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/control-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/controls.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/engine.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/mode-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/module.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/panel.h
        ${LIB_SOURCE_DIR}/modules/envelope/hostage/signals.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/defer-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/events.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/generate-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/hold-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/idle-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/input-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/level-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/mode-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/mode/sustain-mode.h
        ${LIB_SOURCE_DIR}/modules/envelope/stage/control-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/stage/engine.h
        ${LIB_SOURCE_DIR}/modules/envelope/stage/module.h
        ${LIB_SOURCE_DIR}/modules/envelope/stage/panel.h
        ${LIB_SOURCE_DIR}/modules/envelope/stage/signals.h
        ${LIB_SOURCE_DIR}/modules/envelope/upstage/control-ids.h
        ${LIB_SOURCE_DIR}/modules/envelope/upstage/engine.h
        ${LIB_SOURCE_DIR}/modules/envelope/upstage/module.h
        ${LIB_SOURCE_DIR}/modules/envelope/upstage/panel.h
        ${LIB_SOURCE_DIR}/modules/envelope/upstage/signals.h
        ${LIB_SOURCE_DIR}/modules/func/control-ids.h
        ${LIB_SOURCE_DIR}/modules/func/controls.h
        ${LIB_SOURCE_DIR}/modules/func/engine.h
        ${LIB_SOURCE_DIR}/modules/func/func1-panel.h
        ${LIB_SOURCE_DIR}/modules/func/func6-panel.h
        ${LIB_SOURCE_DIR}/modules/func/init.cpp
        ${LIB_SOURCE_DIR}/modules/func/init.h
        ${LIB_SOURCE_DIR}/modules/func/module.h
        ${LIB_SOURCE_DIR}/modules/func/multiplier.h
        ${LIB_SOURCE_DIR}/modules/func/offset.h
        ${LIB_SOURCE_DIR}/modules/func/operation.h
        ${LIB_SOURCE_DIR}/modules/func/signals.h
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/control-ids.h
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/engines.h
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/init.cpp
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/init.h
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/module.h
        ${LIB_SOURCE_DIR}/modules/fuzzy-logic/panel.h
        ${LIB_SOURCE_DIR}/modules/gator/control-ids.h
        ${LIB_SOURCE_DIR}/modules/gator/init.cpp
        ${LIB_SOURCE_DIR}/modules/gator/init.h
        ${LIB_SOURCE_DIR}/modules/gator/module.h
        ${LIB_SOURCE_DIR}/modules/gator/panel.h
        ${LIB_SOURCE_DIR}/modules/ranger/control-ids.h
        ${LIB_SOURCE_DIR}/modules/ranger/init.cpp
        ${LIB_SOURCE_DIR}/modules/ranger/init.h
        ${LIB_SOURCE_DIR}/modules/ranger/module.h
        ${LIB_SOURCE_DIR}/modules/ranger/panel.h
        ${LIB_SOURCE_DIR}/modules/scannibal/anchor.h
        ${LIB_SOURCE_DIR}/modules/scannibal/control-ids.h
        ${LIB_SOURCE_DIR}/modules/scannibal/controller.h
        ${LIB_SOURCE_DIR}/modules/scannibal/generator.h
        ${LIB_SOURCE_DIR}/modules/scannibal/init.cpp
        ${LIB_SOURCE_DIR}/modules/scannibal/init.h
        ${LIB_SOURCE_DIR}/modules/scannibal/module.h
        ${LIB_SOURCE_DIR}/modules/scannibal/panel.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/anchor.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/control-ids.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/generator.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/init.cpp
        ${LIB_SOURCE_DIR}/modules/sequencizer/init.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/interrupter.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/module.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/panel.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/sequence-controller.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/signals.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/status.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/step-controller.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/step-selector.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/sustainer.h
        ${LIB_SOURCE_DIR}/modules/sequencizer/trigger-mode.h
        ${LIB_SOURCE_DIR}/modules/swave/control-ids.h
        ${LIB_SOURCE_DIR}/modules/swave/init.cpp
        ${LIB_SOURCE_DIR}/modules/swave/init.h
        ${LIB_SOURCE_DIR}/modules/swave/module.h
        ${LIB_SOURCE_DIR}/modules/swave/panel.h
        ${LIB_SOURCE_DIR}/modules/tapers/control-ids.h
        ${LIB_SOURCE_DIR}/modules/tapers/init.cpp
        ${LIB_SOURCE_DIR}/modules/tapers/init.h
        ${LIB_SOURCE_DIR}/modules/tapers/module.h
        ${LIB_SOURCE_DIR}/modules/tapers/panel.h
        ${LIB_SOURCE_DIR}/modules/truth/control-ids.h
        ${LIB_SOURCE_DIR}/modules/truth/gate-mode.h
        ${LIB_SOURCE_DIR}/modules/truth/init.cpp
        ${LIB_SOURCE_DIR}/modules/truth/init.h
        ${LIB_SOURCE_DIR}/modules/truth/input-selector.h
        ${LIB_SOURCE_DIR}/modules/truth/module.h
        ${LIB_SOURCE_DIR}/modules/truth/outcome.h
        ${LIB_SOURCE_DIR}/modules/truth/panel.h
        ${LIB_SOURCE_DIR}/modules/truth/upgrader.h
        ${LIB_SOURCE_DIR}/modules/xycloid/control-ids.h
        ${LIB_SOURCE_DIR}/modules/xycloid/controls.h
        ${LIB_SOURCE_DIR}/modules/xycloid/init.cpp
        ${LIB_SOURCE_DIR}/modules/xycloid/init.h
        ${LIB_SOURCE_DIR}/modules/xycloid/module.h
        ${LIB_SOURCE_DIR}/modules/xycloid/panel.h
        ${LIB_SOURCE_DIR}/modules/xycloid/throb-speed.h
        ${LIB_SOURCE_DIR}/modules/xycloid/wobble-ratio.h
        ${LIB_SOURCE_DIR}/panels/assets.h
        ${LIB_SOURCE_DIR}/panels/dimensions.h
        ${LIB_SOURCE_DIR}/panels/panel-widget.h
        ${LIB_SOURCE_DIR}/params/presets.h
        ${LIB_SOURCE_DIR}/signals/basic.h
        ${LIB_SOURCE_DIR}/signals/duration-signals.h
        ${LIB_SOURCE_DIR}/signals/linear-signals.h
        ${LIB_SOURCE_DIR}/signals/shape-signals.h
        ${LIB_SOURCE_DIR}/signals/voltage-signals.h
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

    set(VCV_NAME "DHE-Modules")
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
