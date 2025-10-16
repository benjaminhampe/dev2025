set(LIB_NAME plugin_122_AudibleInstruments2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/Blinds.cpp
        ${LIB_SOURCE_DIR}/Braids.cpp
        ${LIB_SOURCE_DIR}/Branches.cpp
        ${LIB_SOURCE_DIR}/Clouds.cpp
        ${LIB_SOURCE_DIR}/Elements.cpp
        ${LIB_SOURCE_DIR}/Frames.cpp
        ${LIB_SOURCE_DIR}/Kinks.cpp
        ${LIB_SOURCE_DIR}/Links.cpp
        ${LIB_SOURCE_DIR}/Marbles.cpp
        ${LIB_SOURCE_DIR}/Plaits.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/Rings.cpp
        ${LIB_SOURCE_DIR}/Ripples.cpp
        ${LIB_SOURCE_DIR}/Shades.cpp
        ${LIB_SOURCE_DIR}/Shelves.cpp
        ${LIB_SOURCE_DIR}/Stages.cpp
        ${LIB_SOURCE_DIR}/Streams.cpp
        ${LIB_SOURCE_DIR}/Tides.cpp
        ${LIB_SOURCE_DIR}/Tides2.cpp
        ${LIB_SOURCE_DIR}/Veils.cpp
        ${LIB_SOURCE_DIR}/Warps.cpp        
        # ${LIB_SOURCE_DIR}/Ripples
        ${LIB_SOURCE_DIR}/Ripples/aafilter.hpp
        ${LIB_SOURCE_DIR}/Ripples/ripples.hpp
        ${LIB_SOURCE_DIR}/Ripples/sos.hpp
        # ${LIB_SOURCE_DIR}/Shelves
        ${LIB_SOURCE_DIR}/Shelves/aafilter.hpp
        ${LIB_SOURCE_DIR}/Shelves/aafilter.py
        ${LIB_SOURCE_DIR}/Shelves/shelves.hpp
        ${LIB_SOURCE_DIR}/Shelves/sos.hpp
        # ${LIB_SOURCE_DIR}/Streams
        ${LIB_SOURCE_DIR}/Streams/aafilter.hpp
        ${LIB_SOURCE_DIR}/Streams/aafilter.py
        ${LIB_SOURCE_DIR}/Streams/adc.hpp
        ${LIB_SOURCE_DIR}/Streams/analog_engine.hpp
        ${LIB_SOURCE_DIR}/Streams/audio_cv_meter.hpp
        ${LIB_SOURCE_DIR}/Streams/cv_scaler.hpp
        ${LIB_SOURCE_DIR}/Streams/digital_engine.hpp
        ${LIB_SOURCE_DIR}/Streams/event_queue.hpp
        ${LIB_SOURCE_DIR}/Streams/leds.hpp
        ${LIB_SOURCE_DIR}/Streams/resampler.hpp
        ${LIB_SOURCE_DIR}/Streams/sos.hpp
        ${LIB_SOURCE_DIR}/Streams/streams.hpp
        ${LIB_SOURCE_DIR}/Streams/switches.hpp
        ${LIB_SOURCE_DIR}/Streams/ui.hpp
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
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack)
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack_elements)
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack_frames)
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack_stages)
    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack_streams)
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

    set(VCV_NAME "AudibleInstruments2")
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
