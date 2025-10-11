set(LIB_NAME plugin_107_OkaySynthesizer)
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
        ${LIB_SOURCE_DIR}/RackUtils.cpp
        ${LIB_SOURCE_DIR}/RackUtils.h
    
        ${LIB_SOURCE_DIR}/discomfort/discomfort.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Clipper.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Clipper.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Crush.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Crush.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/DiscomfortInput.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/DiscomfortInternal.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/DiscomfortInternal.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/DryWet.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/DryWet.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/FilterBank.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/FilterBank.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Folder.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Folder.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Follower.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/Follower.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/SubOctave.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/SubOctave.h
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/util.cpp
        ${LIB_SOURCE_DIR}/discomfort/DiscomfortInternal/util.h

        ${LIB_SOURCE_DIR}/lowstepper/DebugLowstepperV5.cpp
        ${LIB_SOURCE_DIR}/lowstepper/DebugLowstepperV7.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_module.cpp
        ${LIB_SOURCE_DIR}/lowstepper/TestLowstepper.cpp
        
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/AverageBuffer.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/AverageBuffer.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/LowStepperLfo.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/LowStepperLfo.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/SyncManager.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/SyncManager.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/util.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper/util.h

        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/LowstepperChannel.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/LowstepperChannel.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/LowstepperRack.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/LowstepperRack.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/Names.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/Names.h
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/RackIORef.cpp
        ${LIB_SOURCE_DIR}/lowstepper/lowstepper_rack/RackIORef.h

        
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
    target_link_libraries(${LIB_NAME} PRIVATE de_DaisySP)
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

    set(VCV_NAME "OkaySynthesizer")
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
