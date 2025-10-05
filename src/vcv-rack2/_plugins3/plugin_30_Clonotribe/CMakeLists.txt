set(LIB_NAME plugin_30_Clonotribe)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/clonotribe.cpp
        ${LIB_SOURCE_DIR}/clonotribe.hpp
        ${LIB_SOURCE_DIR}/constants.hpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp        
      # ${LIB_SOURCE_DIR}/dsp
        ${LIB_SOURCE_DIR}/dsp/dc_blocker.hpp
        ${LIB_SOURCE_DIR}/dsp/delay.hpp
        ${LIB_SOURCE_DIR}/dsp/distortion.hpp
        ${LIB_SOURCE_DIR}/dsp/dsp.hpp
        ${LIB_SOURCE_DIR}/dsp/envelope.hpp
        ${LIB_SOURCE_DIR}/dsp/fastmath.hpp
        ${LIB_SOURCE_DIR}/dsp/filter_processor.hpp
        ${LIB_SOURCE_DIR}/dsp/lfo.hpp
        ${LIB_SOURCE_DIR}/dsp/noise.hpp
        ${LIB_SOURCE_DIR}/dsp/parameter_cache.hpp
        ${LIB_SOURCE_DIR}/dsp/process.cpp
        ${LIB_SOURCE_DIR}/dsp/ribbon.hpp       
        ${LIB_SOURCE_DIR}/dsp/signal_processor.cpp
        ${LIB_SOURCE_DIR}/dsp/vco.hpp        
      # ${LIB_SOURCE_DIR}/dsp/drumkits/base
        ${LIB_SOURCE_DIR}/dsp/drumkits/base/base.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/base/drum_processor.hpp
      # ${LIB_SOURCE_DIR}/dsp/drumkits/latin
        ${LIB_SOURCE_DIR}/dsp/drumkits/latin/hihat.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/latin/kickdrum.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/latin/snaredrum.hpp
      # ${LIB_SOURCE_DIR}/dsp/drumkits/original
        ${LIB_SOURCE_DIR}/dsp/drumkits/original/hihat.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/original/kickdrum.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/original/snaredrum.hpp
      # ${LIB_SOURCE_DIR}/dsp/drumkits/tr808
        ${LIB_SOURCE_DIR}/dsp/drumkits/tr808/hihat.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/tr808/kickdrum.hpp
        ${LIB_SOURCE_DIR}/dsp/drumkits/tr808/snaredrum.hpp
      # ${LIB_SOURCE_DIR}/dsp/sequencer
        ${LIB_SOURCE_DIR}/dsp/sequencer/sequencer.hpp
        ${LIB_SOURCE_DIR}/dsp/sequencer/sequencer_processor.cpp
      # ${LIB_SOURCE_DIR}/dsp/vcf
        ${LIB_SOURCE_DIR}/dsp/vcf/filter_type.hpp
        ${LIB_SOURCE_DIR}/dsp/vcf/ladder.hpp
        ${LIB_SOURCE_DIR}/dsp/vcf/moog.hpp
        ${LIB_SOURCE_DIR}/dsp/vcf/ms20.hpp
      # ${LIB_SOURCE_DIR}/ui
        ${LIB_SOURCE_DIR}/ui/button.hpp
        ${LIB_SOURCE_DIR}/ui/mainpanel.cpp
        ${LIB_SOURCE_DIR}/ui/octaveknob.hpp
        ${LIB_SOURCE_DIR}/ui/ribboncontroller.cpp
        ${LIB_SOURCE_DIR}/ui/ribboncontroller.hpp
        ${LIB_SOURCE_DIR}/ui/ui.hpp
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

    set(VCV_NAME "Clonotribe")
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
