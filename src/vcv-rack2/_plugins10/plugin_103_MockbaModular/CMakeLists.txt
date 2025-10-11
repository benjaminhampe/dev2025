set(LIB_NAME plugin_103_MockbaModular)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/a_plugin.cpp
        ${LIB_SOURCE_DIR}/a_plugin.hpp
        ${LIB_SOURCE_DIR}/Blank.cpp
        ${LIB_SOURCE_DIR}/Comparator.cpp
        ${LIB_SOURCE_DIR}/Countah.cpp
        ${LIB_SOURCE_DIR}/CZDblSine.cpp
        ${LIB_SOURCE_DIR}/CZOsc.cpp
        ${LIB_SOURCE_DIR}/CZPulse.cpp
        ${LIB_SOURCE_DIR}/CZReso1.cpp
        ${LIB_SOURCE_DIR}/CZReso2.cpp
        ${LIB_SOURCE_DIR}/CZReso3.cpp
        ${LIB_SOURCE_DIR}/CZSaw.cpp
        ${LIB_SOURCE_DIR}/CZSawPulse.cpp
        ${LIB_SOURCE_DIR}/CZSquare.cpp
        ${LIB_SOURCE_DIR}/Dividah.cpp
        ${LIB_SOURCE_DIR}/DualAND.cpp
        ${LIB_SOURCE_DIR}/DualBUFFER.cpp
        ${LIB_SOURCE_DIR}/DualNAND.cpp
        ${LIB_SOURCE_DIR}/DualNOR.cpp
        ${LIB_SOURCE_DIR}/DualNOT.cpp
        ${LIB_SOURCE_DIR}/DualOR.cpp
        ${LIB_SOURCE_DIR}/DualXNOR.cpp
        ${LIB_SOURCE_DIR}/DualXOR.cpp
        ${LIB_SOURCE_DIR}/Feidah.cpp
        ${LIB_SOURCE_DIR}/FeidahS.cpp
        ${LIB_SOURCE_DIR}/Filtah.cpp
        ${LIB_SOURCE_DIR}/Holdah.cpp
        ${LIB_SOURCE_DIR}/MaugOsc.cpp
        ${LIB_SOURCE_DIR}/MaugSaw.cpp
        ${LIB_SOURCE_DIR}/MaugSaw2.cpp
        ${LIB_SOURCE_DIR}/MaugShark.cpp
        ${LIB_SOURCE_DIR}/MaugSquare.cpp
        ${LIB_SOURCE_DIR}/MaugSquare2.cpp
        ${LIB_SOURCE_DIR}/MaugSquare3.cpp
        ${LIB_SOURCE_DIR}/MaugTriangle.cpp
        ${LIB_SOURCE_DIR}/Mixah.cpp
        ${LIB_SOURCE_DIR}/Mixah3.cpp
        ${LIB_SOURCE_DIR}/MockbaModular.cpp
        ${LIB_SOURCE_DIR}/MockbaModular.hpp
        ${LIB_SOURCE_DIR}/Pannah.cpp
        ${LIB_SOURCE_DIR}/PSelectah.cpp
        ${LIB_SOURCE_DIR}/ReVoltah.cpp
        ${LIB_SOURCE_DIR}/Selectah.cpp
        ${LIB_SOURCE_DIR}/Shapah.cpp
        ${LIB_SOURCE_DIR}/UDPClockMaster.cpp
        ${LIB_SOURCE_DIR}/UDPClockSlave.cpp

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
    # target_link_libraries(${LIB_NAME} PRIVATE de_opensimplexnoise)
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

        if (WIN32)
            target_link_libraries(${LIB_NAME} PRIVATE ws2_32)
            target_link_libraries(${LIB_NAME} PRIVATE winmm)
        endif()
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

    set(VCV_NAME "MockbaModular")
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
