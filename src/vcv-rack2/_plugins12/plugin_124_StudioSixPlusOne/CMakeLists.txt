set(LIB_NAME plugin_124_StudioSixPlusOne)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/widgets.h    
        ${LIB_SOURCE_DIR}/composites/Amburgh.h
        ${LIB_SOURCE_DIR}/composites/Bascom.h
        ${LIB_SOURCE_DIR}/composites/BascomParamEnum.h
        ${LIB_SOURCE_DIR}/composites/Bose.h
        ${LIB_SOURCE_DIR}/composites/CombFilter.h
        ${LIB_SOURCE_DIR}/composites/Duffy.h
        ${LIB_SOURCE_DIR}/composites/EmptyComposite.h
        ${LIB_SOURCE_DIR}/composites/Eva.h
        ${LIB_SOURCE_DIR}/composites/Farini.h
        ${LIB_SOURCE_DIR}/composites/Hula.h
        ${LIB_SOURCE_DIR}/composites/Iverson.h
        ${LIB_SOURCE_DIR}/composites/KSDelay.h
        ${LIB_SOURCE_DIR}/composites/LaLa.h
        ${LIB_SOURCE_DIR}/composites/LalaStereo.h
        ${LIB_SOURCE_DIR}/composites/Maccomo.h
        ${LIB_SOURCE_DIR}/composites/Mix.h
        ${LIB_SOURCE_DIR}/composites/PatchNotes.h
        ${LIB_SOURCE_DIR}/composites/PolyShiftRegister.h
        ${LIB_SOURCE_DIR}/composites/Thru.h
        ${LIB_SOURCE_DIR}/composites/Zazel.h
        ${LIB_SOURCE_DIR}/composites/framework/IComposite.h
        ${LIB_SOURCE_DIR}/composites/framework/SqPort.h
        ${LIB_SOURCE_DIR}/composites/framework/TestComposite.h
        ${LIB_SOURCE_DIR}/composites/framework/WidgetComposite.h        
        ${LIB_SOURCE_DIR}/ctrl/SqHelper.h
        ${LIB_SOURCE_DIR}/ctrl/SqMenuItem.h
        ${LIB_SOURCE_DIR}/ctrl/SqUI.h
        ${LIB_SOURCE_DIR}/dsp/Adsr.h
        ${LIB_SOURCE_DIR}/dsp/AudioMath.h
        ${LIB_SOURCE_DIR}/dsp/CircularBuffer.h
        ${LIB_SOURCE_DIR}/dsp/HardLimiter.h
        ${LIB_SOURCE_DIR}/dsp/LookupTable.h
        ${LIB_SOURCE_DIR}/dsp/SampleAndHold.h
        ${LIB_SOURCE_DIR}/dsp/SchmittTrigger_4.h
        ${LIB_SOURCE_DIR}/dsp/SynthFilter.h
        ${LIB_SOURCE_DIR}/dsp/SynthFilterII.h
        ${LIB_SOURCE_DIR}/dsp/TriggerSequencer.h
        ${LIB_SOURCE_DIR}/dsp/UtilityFilters.h
        ${LIB_SOURCE_DIR}/dsp/WaveShaper.h
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/DiodeFeedbackOneAndOneShape.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/DiodeFeedbackOneAndThreeShape.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/DiodeFeedbackOneAndTwoShape.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/DiodeFeedbackRedGreenShape.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Grrr.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Grrr100.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Grrr50.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/k25AttenuvertMinus100.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/k25AttenuvertMinus50.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/k25AttenuvertPlud100.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/k25AttenuvertPlud50.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Mix100100.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Mix50100.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Mix5050.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Mult.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/ThroughShape.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Vca1.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Vca2.cpp
        ${LIB_SOURCE_DIR}/dsp/WaveShapes/Vca3.cpp
        ${LIB_SOURCE_DIR}/modules/Amburgh.cpp
        ${LIB_SOURCE_DIR}/modules/Bascom.cpp
        ${LIB_SOURCE_DIR}/modules/BascomExpander.cpp
        ${LIB_SOURCE_DIR}/modules/Bose.cpp
        ${LIB_SOURCE_DIR}/modules/CombFilter.cpp
        ${LIB_SOURCE_DIR}/modules/Duffy.cpp
        ${LIB_SOURCE_DIR}/modules/Eva.cpp
        ${LIB_SOURCE_DIR}/modules/Farini.cpp
        ${LIB_SOURCE_DIR}/modules/Hula.cpp
        ${LIB_SOURCE_DIR}/modules/Iverson.cpp
        ${LIB_SOURCE_DIR}/modules/KSDelay.cpp
        ${LIB_SOURCE_DIR}/modules/LaLa.cpp
        ${LIB_SOURCE_DIR}/modules/LalaStereo.cpp
        ${LIB_SOURCE_DIR}/modules/Maccomo.cpp
        ${LIB_SOURCE_DIR}/modules/Mix.cpp
        ${LIB_SOURCE_DIR}/modules/PatchNotes.cpp
        ${LIB_SOURCE_DIR}/modules/PolyShiftRegister.cpp
        ${LIB_SOURCE_DIR}/modules/Te.cpp
        ${LIB_SOURCE_DIR}/modules/Thru.cpp
        ${LIB_SOURCE_DIR}/modules/Zazel.cpp
        ${LIB_SOURCE_DIR}/modules/Zilah.cpp        
        # ${LIB_SOURCE_DIR}/templates/templateClass.h
        # ${LIB_SOURCE_DIR}/templates/templateClassTest.cpp
        # ${LIB_SOURCE_DIR}/templates/templateComposite.h
        # ${LIB_SOURCE_DIR}/templates/templateModule.cpp
        # ${LIB_SOURCE_DIR}/templates/templateTest.cpp
        ${LIB_SOURCE_DIR}/third-party/sqsrc/util/asserts.h
        ${LIB_SOURCE_DIR}/third-party/sqsrc/util/easing.h
        ${LIB_SOURCE_DIR}/third-party/sqsrc/util/SqTime.h
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/composites)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/composites/framework)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ctrl)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/WaveShapes)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/modules)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/third-party/sqsrc/util)

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

    set(VCV_NAME "StudioSixPlusOne")
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
