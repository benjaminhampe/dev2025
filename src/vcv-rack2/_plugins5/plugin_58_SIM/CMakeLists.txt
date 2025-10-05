set(LIB_NAME plugin_58_SIM)
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
        ${LIB_SOURCE_DIR}/Arr.cpp
        ${LIB_SOURCE_DIR}/Bank.cpp
        ${LIB_SOURCE_DIR}/Blank.cpp
        ${LIB_SOURCE_DIR}/Coerce.cpp
        ${LIB_SOURCE_DIR}/config.hpp
        ${LIB_SOURCE_DIR}/constants.cpp
        ${LIB_SOURCE_DIR}/constants.hpp
        ${LIB_SOURCE_DIR}/Debug.cpp
        ${LIB_SOURCE_DIR}/Debug.hpp
        ${LIB_SOURCE_DIR}/GaitX.cpp
        ${LIB_SOURCE_DIR}/GaitX.hpp
        ${LIB_SOURCE_DIR}/InX.cpp
        ${LIB_SOURCE_DIR}/InX.hpp
        ${LIB_SOURCE_DIR}/ModX.cpp
        ${LIB_SOURCE_DIR}/ModX.hpp
        ${LIB_SOURCE_DIR}/OutX.cpp
        ${LIB_SOURCE_DIR}/OutX.hpp
        ${LIB_SOURCE_DIR}/Phi.cpp
        ${LIB_SOURCE_DIR}/ReX.cpp
        ${LIB_SOURCE_DIR}/ReX.hpp
        ${LIB_SOURCE_DIR}/Shared.cpp
        ${LIB_SOURCE_DIR}/Shared.hpp
        ${LIB_SOURCE_DIR}/Spike.cpp
        ${LIB_SOURCE_DIR}/Tie.cpp
        ${LIB_SOURCE_DIR}/Via.cpp
        ${LIB_SOURCE_DIR}/biexpander/biexpander.hpp
        ${LIB_SOURCE_DIR}/biexpander/CacheState.hpp
        ${LIB_SOURCE_DIR}/biexpander/ConnectionLights.hpp
        ${LIB_SOURCE_DIR}/biexpander/ModuleInstantiationMenu.hpp
        ${LIB_SOURCE_DIR}/biexpander/sigslot/signal.hpp
        ${LIB_SOURCE_DIR}/comp/displays.hpp
        ${LIB_SOURCE_DIR}/comp/knobs.hpp
        ${LIB_SOURCE_DIR}/comp/lights.hpp
        ${LIB_SOURCE_DIR}/comp/ports.hpp
        ${LIB_SOURCE_DIR}/comp/Segment.hpp
        ${LIB_SOURCE_DIR}/comp/switches.hpp
        ${LIB_SOURCE_DIR}/helpers/iters.hpp
        ${LIB_SOURCE_DIR}/helpers/ModuleIterator.hpp
        ${LIB_SOURCE_DIR}/helpers/SliderQuantity.hpp
        ${LIB_SOURCE_DIR}/helpers/wrappers.hpp
        ${LIB_SOURCE_DIR}/sp/ClockTracker.cpp
        ${LIB_SOURCE_DIR}/sp/ClockTracker.hpp
        ${LIB_SOURCE_DIR}/sp/glide.hpp
        ${LIB_SOURCE_DIR}/sp/PhasorAnalyzers.cpp
        ${LIB_SOURCE_DIR}/sp/PhasorAnalyzers.hpp
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

    set(VCV_NAME "SIM")
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
