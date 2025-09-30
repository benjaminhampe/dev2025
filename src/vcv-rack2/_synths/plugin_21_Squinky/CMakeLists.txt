set(LIB_NAME plugin_21_Squinky)
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
    file(GLOB_RECURSE LIB_SOURCES_H ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/composites)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ctrl)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/fft)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/filters)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/generators)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/samp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/falco)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/FLAC)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/private)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/protected)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/share)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/midifile)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/utils)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/grammar)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/kbd)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/controller)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/model)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/view)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/seq)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/clock)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/delay)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/grammar)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/thread)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/util)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/third-party)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    target_compile_definitions(${LIB_NAME} PRIVATE __V1x=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __PLUGIN=1)
    target_compile_definitions(${LIB_NAME} PRIVATE _SEQ=1) # Sequencer
    target_compile_definitions(${LIB_NAME} PRIVATE _EXP=0) # Experimental
    
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FSEEKO=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LROUND=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_STDINT_H=1)

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_kissfft)
    target_link_libraries(${LIB_NAME} PRIVATE de_flac)
    target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

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

    set(VCV_NAME "Squinky")
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
