set(LIB_NAME plugin_106_NotThings)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_include/not-things.hpp

        ${LIB_SOURCE_DIR}/_include/components/dualbefacoswitch.hpp
        ${LIB_SOURCE_DIR}/_include/components/leddisplay.hpp
        ${LIB_SOURCE_DIR}/_include/components/lights.hpp
        ${LIB_SOURCE_DIR}/_include/components/notedisplay.hpp
        ${LIB_SOURCE_DIR}/_include/components/ntknob.hpp
        ${LIB_SOURCE_DIR}/_include/components/ntpanel.hpp
        ${LIB_SOURCE_DIR}/_include/components/ntport.hpp
        ${LIB_SOURCE_DIR}/_include/components/ntscrew.hpp
        ${LIB_SOURCE_DIR}/_include/components/timeseq-display.hpp

        ${LIB_SOURCE_DIR}/_include/core/solim-core.hpp
        ${LIB_SOURCE_DIR}/_include/core/timeseq-core.hpp
        ${LIB_SOURCE_DIR}/_include/core/timeseq-json.hpp
        ${LIB_SOURCE_DIR}/_include/core/timeseq-processor.hpp
        ${LIB_SOURCE_DIR}/_include/core/timeseq-script.hpp
        ${LIB_SOURCE_DIR}/_include/core/timeseq-validation.hpp

        ${LIB_SOURCE_DIR}/_include/modules/pipo-input.hpp
        ${LIB_SOURCE_DIR}/_include/modules/pipo-output.hpp
        ${LIB_SOURCE_DIR}/_include/modules/poly-same-diff.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim-input-octaver.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim-input.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim-output-octaver.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim-output.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim-random.hpp
        ${LIB_SOURCE_DIR}/_include/modules/solim.hpp
        ${LIB_SOURCE_DIR}/_include/modules/timeseq.hpp

        ${LIB_SOURCE_DIR}/_include/util/avgduration.hpp
        ${LIB_SOURCE_DIR}/_include/util/notes.hpp
        
        ${LIB_SOURCE_DIR}/not-things.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        
        ${LIB_SOURCE_DIR}/components/dualbefacoswitch.cpp
        ${LIB_SOURCE_DIR}/components/leddisplay.cpp
        ${LIB_SOURCE_DIR}/components/notedisplay.cpp
        ${LIB_SOURCE_DIR}/components/ntknob.cpp
        ${LIB_SOURCE_DIR}/components/ntpanel.cpp
        ${LIB_SOURCE_DIR}/components/ntport.cpp
        ${LIB_SOURCE_DIR}/components/ntscrew.cpp
        ${LIB_SOURCE_DIR}/components/timeseq-display.cpp

        ${LIB_SOURCE_DIR}/core/solim-core.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-core.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-json-script.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-json.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-processor-script.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-processor.cpp
        ${LIB_SOURCE_DIR}/core/timeseq-validation.cpp

        ${LIB_SOURCE_DIR}/modules/pipo-input.cpp
        ${LIB_SOURCE_DIR}/modules/pipo-output.cpp
        ${LIB_SOURCE_DIR}/modules/poly-same-diff.cpp
        ${LIB_SOURCE_DIR}/modules/solim-input-octaver.cpp
        ${LIB_SOURCE_DIR}/modules/solim-input.cpp
        ${LIB_SOURCE_DIR}/modules/solim-output-octaver.cpp
        ${LIB_SOURCE_DIR}/modules/solim-output.cpp
        ${LIB_SOURCE_DIR}/modules/solim-random.cpp
        ${LIB_SOURCE_DIR}/modules/solim.cpp
        ${LIB_SOURCE_DIR}/modules/timeseq.cpp

        ${LIB_SOURCE_DIR}/util/notes.cpp
        
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_include)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_nlohmann_json)
    target_link_libraries(${LIB_NAME} PRIVATE de_nlohmann_json_schema_validator)
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

    set(VCV_NAME "NotThings")
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
