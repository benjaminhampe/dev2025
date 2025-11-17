set(LIB_NAME plugin_05_vanTies)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Ad.cpp
        ${LIB_SOURCE_DIR}/Ad.h
        ${LIB_SOURCE_DIR}/Adje.cpp
        ${LIB_SOURCE_DIR}/Adje.h
        ${LIB_SOURCE_DIR}/AdjeWidget.cpp
        ${LIB_SOURCE_DIR}/AdWidget.cpp
        ${LIB_SOURCE_DIR}/Bufke.cpp
        ${LIB_SOURCE_DIR}/Bufke.h
        ${LIB_SOURCE_DIR}/BufkeWidget.cpp
        ${LIB_SOURCE_DIR}/Funs.cpp
        ${LIB_SOURCE_DIR}/Funs.h
        ${LIB_SOURCE_DIR}/FunsWidget.cpp
        ${LIB_SOURCE_DIR}/Sjoegele.cpp
        ${LIB_SOURCE_DIR}/Sjoegele.h
        ${LIB_SOURCE_DIR}/SjoegeleWidget.cpp
        ${LIB_SOURCE_DIR}/vanTies.cpp
        ${LIB_SOURCE_DIR}/vanTies.h
        ${LIB_SOURCE_DIR}/dsp/AdditiveOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/AdditiveOscillator.h
        ${LIB_SOURCE_DIR}/dsp/CvBuffer.cpp
        ${LIB_SOURCE_DIR}/dsp/CvBuffer.h
        ${LIB_SOURCE_DIR}/dsp/DoublePendulum.cpp
        ${LIB_SOURCE_DIR}/dsp/DoublePendulum.h
        ${LIB_SOURCE_DIR}/dsp/FollowingCvBuffer.cpp
        ${LIB_SOURCE_DIR}/dsp/FollowingCvBuffer.h
        ${LIB_SOURCE_DIR}/dsp/Oscillator.h
        ${LIB_SOURCE_DIR}/dsp/RatFuncOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/RatFuncOscillator.h
        ${LIB_SOURCE_DIR}/dsp/SineOscillator.h
        ${LIB_SOURCE_DIR}/dsp/Spectrum.cpp
        ${LIB_SOURCE_DIR}/dsp/Spectrum.h        
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

    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

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

    # set_target_properties(${LIB_NAME} PROPERTIES
    #     OUTPUT_NAME "Fundamental-2.6.2-llvm-win-x64"
    #     SUFFIX ".dll"
    # )

    set(VCV_NAME "vanTies")
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

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "$<TARGET_FILE:${LIB_NAME}>"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/myplugin.vcvplugin"
    # )

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myplugin.dll"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcvplugins/myplugin.vcvplugin"
    # )

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)

endif()

# add_subdirectory(test)
