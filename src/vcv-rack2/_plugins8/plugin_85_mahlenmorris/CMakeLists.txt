set(LIB_NAME plugin_85_mahlenmorris)
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
        ${LIB_SOURCE_DIR}/AudioFile.h
        ${LIB_SOURCE_DIR}/Basically.cpp
        ${LIB_SOURCE_DIR}/Brainwash.cpp
        ${LIB_SOURCE_DIR}/buffered.cpp
        ${LIB_SOURCE_DIR}/buffered.hpp
        ${LIB_SOURCE_DIR}/code_block.cpp
        ${LIB_SOURCE_DIR}/code_block.h
        ${LIB_SOURCE_DIR}/Depict.cpp
        ${LIB_SOURCE_DIR}/Drifter.cpp
        ${LIB_SOURCE_DIR}/Embellish.cpp
        ${LIB_SOURCE_DIR}/extended_text.cpp
        ${LIB_SOURCE_DIR}/extended_text.h
        ${LIB_SOURCE_DIR}/Fermata.cpp
        ${LIB_SOURCE_DIR}/Fixation.cpp
        ${LIB_SOURCE_DIR}/Fuse.cpp
        ${LIB_SOURCE_DIR}/Memory.cpp
        ${LIB_SOURCE_DIR}/NoLockQueue.h
        ${LIB_SOURCE_DIR}/pcode.cpp
        ${LIB_SOURCE_DIR}/pcode.h
        ${LIB_SOURCE_DIR}/pcode_trans.h
        ${LIB_SOURCE_DIR}/Ruminate.cpp
        ${LIB_SOURCE_DIR}/smoother.h
        ${LIB_SOURCE_DIR}/st_textfield.cpp
        ${LIB_SOURCE_DIR}/st_textfield.hpp
        ${LIB_SOURCE_DIR}/TTY.cpp
        ${LIB_SOURCE_DIR}/Venn.cpp
        ${LIB_SOURCE_DIR}/parser/driver.cc
        ${LIB_SOURCE_DIR}/parser/driver.hh
        ${LIB_SOURCE_DIR}/parser/environment.h
        ${LIB_SOURCE_DIR}/parser/location.hh
        ${LIB_SOURCE_DIR}/parser/parser.cc
#        ${LIB_SOURCE_DIR}/parser/parser.gv
        ${LIB_SOURCE_DIR}/parser/parser.hh
#        ${LIB_SOURCE_DIR}/parser/parser.yy
        ${LIB_SOURCE_DIR}/parser/scanner.cc
        #${LIB_SOURCE_DIR}/parser/scanner.ll
        ${LIB_SOURCE_DIR}/parser/tree.cc
        ${LIB_SOURCE_DIR}/parser/tree.h
        ${LIB_SOURCE_DIR}/parser-venn/driver.cc
        ${LIB_SOURCE_DIR}/parser-venn/driver.h
#        ${LIB_SOURCE_DIR}/parser-venn/location.hh
        ${LIB_SOURCE_DIR}/parser-venn/parser.cc
#        ${LIB_SOURCE_DIR}/parser-venn/parser.gv
        ${LIB_SOURCE_DIR}/parser-venn/parser.hh
#        ${LIB_SOURCE_DIR}/parser-venn/parser.yy
        ${LIB_SOURCE_DIR}/parser-venn/scanner.cc
        #${LIB_SOURCE_DIR}/parser-venn/scanner.ll
        ${LIB_SOURCE_DIR}/parser-venn/tree.cc
        ${LIB_SOURCE_DIR}/parser-venn/tree.h
        ${LIB_SOURCE_DIR}/tipsy_utils.h
        ${LIB_SOURCE_DIR}/tipsy-encoder/include/tipsy/binary-to-float.h
        ${LIB_SOURCE_DIR}/tipsy-encoder/include/tipsy/protocol.h
        ${LIB_SOURCE_DIR}/tipsy-encoder/include/tipsy/tipsy.h
        ${LIB_SOURCE_DIR}/tipsy-encoder/include/tipsy/version.h
        # ${LIB_SOURCE_DIR}/tipsy-encoder/test/binary.cpp
        # ${LIB_SOURCE_DIR}/tipsy-encoder/test/catch2.hpp
        # ${LIB_SOURCE_DIR}/tipsy-encoder/test/main.cpp
        # ${LIB_SOURCE_DIR}/tipsy-encoder/test/protocol.cpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/tipsy-encoder/include)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_oui_blendish)
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

    set(VCV_NAME "mahlenmorris")
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
