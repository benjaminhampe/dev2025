set(LIB_NAME plugin_100_docB)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/RackAssets.cpp
        ${LIB_SOURCE_DIR}/RackAssets.h
        ${LIB_SOURCE_DIR}/AddSynth.cpp
        ${LIB_SOURCE_DIR}/AP.cpp
        ${LIB_SOURCE_DIR}/AX.cpp
        ${LIB_SOURCE_DIR}/buffer.hpp
        ${LIB_SOURCE_DIR}/BWF.cpp
        ${LIB_SOURCE_DIR}/CHBY.cpp
        ${LIB_SOURCE_DIR}/clipper.hpp
        ${LIB_SOURCE_DIR}/CLP.cpp
        ${LIB_SOURCE_DIR}/Computer.hpp
        ${LIB_SOURCE_DIR}/CSOSC.cpp
        ${LIB_SOURCE_DIR}/CVS.cpp
        ${LIB_SOURCE_DIR}/CWS.cpp
        ${LIB_SOURCE_DIR}/dcb.cpp
        ${LIB_SOURCE_DIR}/dcb.h
        ${LIB_SOURCE_DIR}/DCBlock.cpp
        ${LIB_SOURCE_DIR}/DRM.cpp
        ${LIB_SOURCE_DIR}/Drums.cpp
        ${LIB_SOURCE_DIR}/DrumSampler.hpp
        ${LIB_SOURCE_DIR}/DTrig.cpp
        ${LIB_SOURCE_DIR}/EVA.cpp
        ${LIB_SOURCE_DIR}/Faders.cpp
        ${LIB_SOURCE_DIR}/FDProcessor.hpp
        ${LIB_SOURCE_DIR}/filter.hpp
        ${LIB_SOURCE_DIR}/FLA.cpp
        ${LIB_SOURCE_DIR}/Frac.cpp
        ${LIB_SOURCE_DIR}/FS6.cpp
        ${LIB_SOURCE_DIR}/Gendy.cpp
        ${LIB_SOURCE_DIR}/GeneticTerrain.cpp
        ${LIB_SOURCE_DIR}/GenScale.cpp
        ${LIB_SOURCE_DIR}/hexfield.h
        ${LIB_SOURCE_DIR}/HexSeq.cpp
        ${LIB_SOURCE_DIR}/HexSeq.hpp
        ${LIB_SOURCE_DIR}/HexSeqExp.cpp
        ${LIB_SOURCE_DIR}/HexSeqP.cpp
        ${LIB_SOURCE_DIR}/hexutil.h
        ${LIB_SOURCE_DIR}/Hopa.cpp
        ${LIB_SOURCE_DIR}/Interface.cpp
        ${LIB_SOURCE_DIR}/JTKeys.cpp
        ${LIB_SOURCE_DIR}/JTScaler.cpp
        ${LIB_SOURCE_DIR}/L4P.cpp
        ${LIB_SOURCE_DIR}/LWF.cpp
        ${LIB_SOURCE_DIR}/MPad2.cpp
        ${LIB_SOURCE_DIR}/MVerb.cpp
        ${LIB_SOURCE_DIR}/OFS.cpp
        ${LIB_SOURCE_DIR}/OFS.hpp
        ${LIB_SOURCE_DIR}/OFS3.cpp
        ${LIB_SOURCE_DIR}/Osc1.cpp
        ${LIB_SOURCE_DIR}/Osc2.cpp
        ${LIB_SOURCE_DIR}/Osc3.cpp
        ${LIB_SOURCE_DIR}/Osc4.cpp
        ${LIB_SOURCE_DIR}/Osc5.cpp
        ${LIB_SOURCE_DIR}/Osc6.cpp
        ${LIB_SOURCE_DIR}/Osc7.cpp
        ${LIB_SOURCE_DIR}/Osc8.cpp
        ${LIB_SOURCE_DIR}/Osc9.cpp
        ${LIB_SOURCE_DIR}/OscA1.cpp
        ${LIB_SOURCE_DIR}/OscP.cpp
        ${LIB_SOURCE_DIR}/OscS.cpp
        ${LIB_SOURCE_DIR}/Pad.cpp
        ${LIB_SOURCE_DIR}/Pad2.cpp
        ${LIB_SOURCE_DIR}/Pad2.hpp
        ${LIB_SOURCE_DIR}/PhO.cpp
        ${LIB_SOURCE_DIR}/PhS.cpp
        ${LIB_SOURCE_DIR}/PHSR.cpp
        ${LIB_SOURCE_DIR}/PHSR2.cpp
        ${LIB_SOURCE_DIR}/PLC.cpp
        ${LIB_SOURCE_DIR}/Plotter.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/PPD.cpp
        ${LIB_SOURCE_DIR}/PRB.cpp
        ${LIB_SOURCE_DIR}/PShift.cpp
        ${LIB_SOURCE_DIR}/Pulsar.cpp
        ${LIB_SOURCE_DIR}/Ratio.cpp
        ${LIB_SOURCE_DIR}/rnd.cpp
        ${LIB_SOURCE_DIR}/rnd.h
        ${LIB_SOURCE_DIR}/RndC.cpp
        ${LIB_SOURCE_DIR}/RndG.cpp
        ${LIB_SOURCE_DIR}/RndH.cpp
        ${LIB_SOURCE_DIR}/RndH2.cpp
        ${LIB_SOURCE_DIR}/RndHvs3.cpp
        ${LIB_SOURCE_DIR}/RSC.cpp
        ${LIB_SOURCE_DIR}/RTrig.cpp
        ${LIB_SOURCE_DIR}/Shaper.cpp
        ${LIB_SOURCE_DIR}/Shaper.hpp
        ${LIB_SOURCE_DIR}/SIMDUtilities.hpp
        ${LIB_SOURCE_DIR}/SKF.cpp
        ${LIB_SOURCE_DIR}/SPF.cpp
        ${LIB_SOURCE_DIR}/SPL.cpp
        ${LIB_SOURCE_DIR}/STrig.cpp
        ${LIB_SOURCE_DIR}/SuperLFO.cpp
        ${LIB_SOURCE_DIR}/SWF.cpp
        # ${LIB_SOURCE_DIR}/test
        # ${LIB_SOURCE_DIR}/test/ComputerTest.cpp
        ${LIB_SOURCE_DIR}/textfield.cpp
        ${LIB_SOURCE_DIR}/textfield.hpp
        ${LIB_SOURCE_DIR}/USVF.cpp
        ${LIB_SOURCE_DIR}/Waveshaping.hpp
        ${LIB_SOURCE_DIR}/WS.cpp
        ${LIB_SOURCE_DIR}/X16.cpp
        ${LIB_SOURCE_DIR}/X4.cpp
        ${LIB_SOURCE_DIR}/X8.cpp
        ${LIB_SOURCE_DIR}/YC.cpp
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
    # target_compile_definitions(${LIB_NAME} PRIVATE NOASM=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE TEST=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_gamma_100_docB)

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

    set(VCV_NAME "docB")
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
