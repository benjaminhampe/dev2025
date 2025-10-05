set(LIB_NAME de_resid)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_HEADERS_H ${LIB_HEADER_DIR}/*.h)
    # set(LIB_HEADERS ${LIB_HEADERS_H})    
    # file(GLOB_RECURSE LIB_SOURCES_CC ${LIB_SOURCE_DIR}/*.cc)
    # set(LIB_SOURCES ${LIB_SOURCES_CC})
    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/dac.h
        ${LIB_HEADER_DIR}/envelope.h
        ${LIB_HEADER_DIR}/extfilt.h
        ${LIB_HEADER_DIR}/filter.h
        ${LIB_HEADER_DIR}/filter8580new.h
        ${LIB_HEADER_DIR}/pot.h
        ${LIB_HEADER_DIR}/resid-config.h
        ${LIB_HEADER_DIR}/sid.h
        ${LIB_HEADER_DIR}/siddefs.h
        ${LIB_HEADER_DIR}/spline.h
        ${LIB_HEADER_DIR}/voice.h
        ${LIB_HEADER_DIR}/wave.h
        ${LIB_HEADER_DIR}/wave6581_PST.h
        ${LIB_HEADER_DIR}/wave6581_PS_.h
        ${LIB_HEADER_DIR}/wave6581_P_T.h
        ${LIB_HEADER_DIR}/wave6581__ST.h
        ${LIB_HEADER_DIR}/wave8580_PST.h
        ${LIB_HEADER_DIR}/wave8580_PS_.h
        ${LIB_HEADER_DIR}/wave8580_P_T.h
        ${LIB_HEADER_DIR}/wave8580__ST.h
    )
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/dac.cc
        ${LIB_SOURCE_DIR}/envelope.cc
        ${LIB_SOURCE_DIR}/extfilt.cc
        ${LIB_SOURCE_DIR}/filter.cc
        ${LIB_SOURCE_DIR}/filter8580new.cc
        ${LIB_SOURCE_DIR}/pot.cc
        ${LIB_SOURCE_DIR}/sid.cc
        ${LIB_SOURCE_DIR}/version.cc
        ${LIB_SOURCE_DIR}/voice.cc
        ${LIB_SOURCE_DIR}/wave.cc
        # ${LIB_SOURCE_DIR}/wave6581_PST.dat
        # ${LIB_SOURCE_DIR}/wave6581_PS_.dat
        # ${LIB_SOURCE_DIR}/wave6581_P_T.dat
        # ${LIB_SOURCE_DIR}/wave6581__ST.dat
        # ${LIB_SOURCE_DIR}/wave8580_PST.dat
        # ${LIB_SOURCE_DIR}/wave8580_PS_.dat
        # ${LIB_SOURCE_DIR}/wave8580_P_T.dat
        # ${LIB_SOURCE_DIR}/wave8580__ST.dat
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    # target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
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
        # add_custom_command(
            # TARGET ${LIB_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            # COMMENT "Stripping debug symbols"
        # )

    # endif()

    # set(VCV_NAME "CaptVolt")
    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # # Create working directory "z_benni/Gemini"
        # COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}"
        # # Move libGemini.dll to working directory Gemini and rename to plugin.dll
        # COMMAND ${CMAKE_COMMAND} -E rename
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${LIB_NAME}.dll"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}/plugin.dll"
        # # Compress directory Gemini to Gemini.tar.zst
        # COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni"
                # tar -cf "${VCV_NAME}.tar" "${VCV_NAME}"
        # COMMAND zstd -o
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar"
        # # Move Gemini.tar.zst to final destination and rename to .vcvplugin
        # COMMAND ${CMAKE_COMMAND} -E make_directory
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins"
        # COMMAND ${CMAKE_COMMAND} -E rename
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/${VCV_NAME}.vcvplugin"
    # )

    # file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
