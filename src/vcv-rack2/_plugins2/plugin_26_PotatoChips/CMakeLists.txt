set(LIB_NAME plugin_26_PotatoChips)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	# set(LIB_SOURCES 
        # ${LIB_SOURCE_DIR}/Gemini.cpp
        # ${LIB_SOURCE_DIR}/plugin.cpp
        # ${LIB_SOURCE_DIR}/plugin.hpp
    # )
    file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Blanks.cpp
        ${LIB_SOURCE_DIR}/Blocks.cpp
        ${LIB_SOURCE_DIR}/BossFight.cpp
        ${LIB_SOURCE_DIR}/InfiniteStairs.cpp
        ${LIB_SOURCE_DIR}/Jairasullator.cpp
        ${LIB_SOURCE_DIR}/MegaTone.cpp
        ${LIB_SOURCE_DIR}/MiniBoss.cpp
        ${LIB_SOURCE_DIR}/NameCorpOctalWaveGenerator.cpp
        ${LIB_SOURCE_DIR}/PalletTownWavesSystem.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/PotKeys.cpp
        ${LIB_SOURCE_DIR}/Pulses.cpp
        ${LIB_SOURCE_DIR}/StepSaw.cpp
        ${LIB_SOURCE_DIR}/SuperADSR.cpp
        ${LIB_SOURCE_DIR}/SuperEcho.cpp
        ${LIB_SOURCE_DIR}/SuperSampler.cpp
        ${LIB_SOURCE_DIR}/SuperSynth.cpp
        ${LIB_SOURCE_DIR}/SuperVCA.cpp
        ${LIB_SOURCE_DIR}/dsp/atari_pokey.hpp
        ${LIB_SOURCE_DIR}/dsp/blip_buffer.hpp
        ${LIB_SOURCE_DIR}/dsp/exceptions.hpp
        ${LIB_SOURCE_DIR}/dsp/general_instrument_ay_3_8910.hpp
        ${LIB_SOURCE_DIR}/dsp/konami_scc.hpp
        ${LIB_SOURCE_DIR}/dsp/konami_vrc6.hpp
        ${LIB_SOURCE_DIR}/dsp/math.hpp
        ${LIB_SOURCE_DIR}/dsp/mi_edges.hpp
        ${LIB_SOURCE_DIR}/dsp/namco_163.hpp
        ${LIB_SOURCE_DIR}/dsp/nec_turbo_grafx_16.hpp
        ${LIB_SOURCE_DIR}/dsp/nintendo_gameboy.hpp
        ${LIB_SOURCE_DIR}/dsp/pcm.hpp
        ${LIB_SOURCE_DIR}/dsp/ricoh_2a03.hpp
        ${LIB_SOURCE_DIR}/dsp/sunsoft_fme7.hpp
        ${LIB_SOURCE_DIR}/dsp/texas_instruments_sn76489.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger.hpp
        ${LIB_SOURCE_DIR}/dsp/wavetable4bit.hpp
        ${LIB_SOURCE_DIR}/dsp/math/constants.hpp
        ${LIB_SOURCE_DIR}/dsp/math/eurorack.hpp
        ${LIB_SOURCE_DIR}/dsp/math/functions.hpp
        ${LIB_SOURCE_DIR}/dsp/mi_edges/lfsr.hpp
        ${LIB_SOURCE_DIR}/dsp/mi_edges/sample_hold.hpp
        ${LIB_SOURCE_DIR}/dsp/mi_edges/wavetable.hpp
        ${LIB_SOURCE_DIR}/dsp/mi_edges/wavetables.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/adsr.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/brr_sample_player.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/common.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/echo.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/gaussian_interpolation_filter.hpp
        ${LIB_SOURCE_DIR}/dsp/sony_s_dsp/processor.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger/boolean.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger/divider.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger/hold.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger/threshold.hpp
        ${LIB_SOURCE_DIR}/dsp/trigger/zero.hpp
        ${LIB_SOURCE_DIR}/dsp/yamaha_ym2612/feedback_operator.hpp
        ${LIB_SOURCE_DIR}/dsp/yamaha_ym2612/operator.hpp
        ${LIB_SOURCE_DIR}/dsp/yamaha_ym2612/tables.hpp
        ${LIB_SOURCE_DIR}/dsp/yamaha_ym2612/voice4op.hpp
        ${LIB_SOURCE_DIR}/engine/chip_module.hpp
        ${LIB_SOURCE_DIR}/engine/yamaha_ym2612_params.hpp
        ${LIB_SOURCE_DIR}/kautenja_rack/helpers.hpp
        ${LIB_SOURCE_DIR}/kautenja_rack/param_quantity.hpp
        ${LIB_SOURCE_DIR}/widget/indexed_frame_display.hpp
        ${LIB_SOURCE_DIR}/widget/wavetable_editor.hpp
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

    set(VCV_NAME "PotatoChips")
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
