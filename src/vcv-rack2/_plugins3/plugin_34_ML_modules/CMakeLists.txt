set(LIB_NAME plugin_34_ML_modules)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Arpeggiator.cpp
        ${LIB_SOURCE_DIR}/BPMdetect.cpp
        ${LIB_SOURCE_DIR}/Cloner.cpp
        ${LIB_SOURCE_DIR}/Constants.cpp
        ${LIB_SOURCE_DIR}/Counter.cpp
        ${LIB_SOURCE_DIR}/FreeVerb.cpp
        ${LIB_SOURCE_DIR}/ML_components.cpp
        ${LIB_SOURCE_DIR}/ML_components.hpp
        ${LIB_SOURCE_DIR}/ML_modules.cpp
        ${LIB_SOURCE_DIR}/ML_modules.hpp
        ${LIB_SOURCE_DIR}/OctaFlop.cpp
        ${LIB_SOURCE_DIR}/OctaPlus.cpp
        ${LIB_SOURCE_DIR}/OctaSwitch.cpp
        ${LIB_SOURCE_DIR}/OctaTimes.cpp
        ${LIB_SOURCE_DIR}/OctaTrig.cpp
        ${LIB_SOURCE_DIR}/PolySplitter.cpp
        ${LIB_SOURCE_DIR}/PulseGenerator_4.hpp
        ${LIB_SOURCE_DIR}/Quant.cpp
        ${LIB_SOURCE_DIR}/Quantum.cpp
        ${LIB_SOURCE_DIR}/SeqSwitch.cpp
        ${LIB_SOURCE_DIR}/SeqSwitch2.cpp
        ${LIB_SOURCE_DIR}/SettingsHandler.cpp
        ${LIB_SOURCE_DIR}/SettingsHandler.hpp
        ${LIB_SOURCE_DIR}/SH8.cpp
        ${LIB_SOURCE_DIR}/ShiftRegister.cpp
        ${LIB_SOURCE_DIR}/ShiftRegister2.cpp
        ${LIB_SOURCE_DIR}/simd_mask.hpp
        ${LIB_SOURCE_DIR}/Sum8-MkII.cpp
        ${LIB_SOURCE_DIR}/Sum8-MkIII.cpp
        ${LIB_SOURCE_DIR}/Sum8.cpp
        ${LIB_SOURCE_DIR}/TrigBuf.cpp
        ${LIB_SOURCE_DIR}/TrigDelay.cpp
        ${LIB_SOURCE_DIR}/TrigSwitch.cpp
        ${LIB_SOURCE_DIR}/TrigSwitch2.cpp
        ${LIB_SOURCE_DIR}/TrigSwitch3.cpp
        ${LIB_SOURCE_DIR}/TrigSwitch3_2.cpp
        ${LIB_SOURCE_DIR}/VoltMeter.cpp
        # This freeverb has a revmodel.init(sampleRate) function that does not exist in my lib?
        ${LIB_SOURCE_DIR}/freeverb/allpass.hpp
        ${LIB_SOURCE_DIR}/freeverb/comb.hpp
        ${LIB_SOURCE_DIR}/freeverb/denormals.h
        # ${LIB_SOURCE_DIR}/freeverb/Makefile
        # ${LIB_SOURCE_DIR}/freeverb/readme.txt
        ${LIB_SOURCE_DIR}/freeverb/revmodel.cpp
        ${LIB_SOURCE_DIR}/freeverb/revmodel.hpp
        ${LIB_SOURCE_DIR}/freeverb/tuning.h

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
    # target_link_libraries(${LIB_NAME} PRIVATE de_freeverb)
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

    set(VCV_NAME "ML_modules")
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
