set(LIB_NAME plugin_60_CharredDesert)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/CharredDesert.cpp
        ${LIB_SOURCE_DIR}/CharredDesert.hpp
        ${LIB_SOURCE_DIR}/LFO.cpp
        ${LIB_SOURCE_DIR}/LFO.hpp
        ${LIB_SOURCE_DIR}/controller/Biquad.cpp
        ${LIB_SOURCE_DIR}/controller/Biquad.h
        ${LIB_SOURCE_DIR}/controller/Carbon.cpp
        ${LIB_SOURCE_DIR}/controller/Carbon.hpp
        ${LIB_SOURCE_DIR}/controller/CV.cpp
        ${LIB_SOURCE_DIR}/controller/CV.hpp
        ${LIB_SOURCE_DIR}/controller/CVSeq.cpp
        ${LIB_SOURCE_DIR}/controller/CVSeq.hpp
        ${LIB_SOURCE_DIR}/controller/DTMF.cpp
        ${LIB_SOURCE_DIR}/controller/DTMF.hpp
        ${LIB_SOURCE_DIR}/controller/Eq.cpp
        ${LIB_SOURCE_DIR}/controller/Eq.hpp
        ${LIB_SOURCE_DIR}/controller/filters.cpp
        ${LIB_SOURCE_DIR}/controller/filters.hpp
        ${LIB_SOURCE_DIR}/controller/K.cpp
        ${LIB_SOURCE_DIR}/controller/K.hpp
        ${LIB_SOURCE_DIR}/controller/M.cpp
        ${LIB_SOURCE_DIR}/controller/M.hpp
        ${LIB_SOURCE_DIR}/controller/Mixer.cpp
        ${LIB_SOURCE_DIR}/controller/Mixer.hpp
        ${LIB_SOURCE_DIR}/controller/MixerCV.cpp
        ${LIB_SOURCE_DIR}/controller/MixerCV.hpp
        ${LIB_SOURCE_DIR}/controller/Noise.cpp
        ${LIB_SOURCE_DIR}/controller/Noise.hpp
        ${LIB_SOURCE_DIR}/controller/Not.cpp
        ${LIB_SOURCE_DIR}/controller/Not.hpp
        ${LIB_SOURCE_DIR}/controller/Oscar2.cpp
        ${LIB_SOURCE_DIR}/controller/Oscar2.hpp
        ${LIB_SOURCE_DIR}/controller/Pan.cpp
        ${LIB_SOURCE_DIR}/controller/Pan.hpp
        ${LIB_SOURCE_DIR}/controller/Shift.cpp
        ${LIB_SOURCE_DIR}/controller/Shift.hpp
        ${LIB_SOURCE_DIR}/controller/Tine.cpp
        ${LIB_SOURCE_DIR}/controller/Tine.hpp
        ${LIB_SOURCE_DIR}/controller/Tsunami.cpp
        ${LIB_SOURCE_DIR}/controller/Tsunami.hpp
        ${LIB_SOURCE_DIR}/controller/X.cpp
        ${LIB_SOURCE_DIR}/controller/X.hpp
        ${LIB_SOURCE_DIR}/model/Compressor.hpp
        ${LIB_SOURCE_DIR}/model/Delay.hpp
        ${LIB_SOURCE_DIR}/model/MessageBus.hpp
        ${LIB_SOURCE_DIR}/view/Carbon.cpp
        ${LIB_SOURCE_DIR}/view/components.hpp
        ${LIB_SOURCE_DIR}/view/CV.cpp
        ${LIB_SOURCE_DIR}/view/CVSeq.cpp
        ${LIB_SOURCE_DIR}/view/DTMF.cpp
        ${LIB_SOURCE_DIR}/view/Eq.cpp
        ${LIB_SOURCE_DIR}/view/K.cpp
        ${LIB_SOURCE_DIR}/view/M.cpp
        ${LIB_SOURCE_DIR}/view/Mixer.cpp
        ${LIB_SOURCE_DIR}/view/MixerCV.cpp
        ${LIB_SOURCE_DIR}/view/Noise.cpp
        ${LIB_SOURCE_DIR}/view/Not.cpp
        ${LIB_SOURCE_DIR}/view/Oscar2.cpp
        ${LIB_SOURCE_DIR}/view/Pan.cpp
        ${LIB_SOURCE_DIR}/view/Shift.cpp
        ${LIB_SOURCE_DIR}/view/Tine.cpp
        ${LIB_SOURCE_DIR}/view/Tsunami.cpp
        ${LIB_SOURCE_DIR}/view/X.cpp
        ${LIB_SOURCE_DIR}/deps/rack-components/display.hpp
        ${LIB_SOURCE_DIR}/deps/rack-components/jacks.hpp
        ${LIB_SOURCE_DIR}/deps/rack-components/knobs.hpp
        ${LIB_SOURCE_DIR}/deps/rack-components/screws.hpp
        ${LIB_SOURCE_DIR}/deps/rack-components/sliders.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/Bjorklund.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/Bjorklund.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/Clock.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/Clock.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/CV.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/CV.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/CVEvent.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/CVEvent.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/DistributedClock.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/DistributedClock.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/DTMF.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/DTMF.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/EventEmitter.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/EventEmitter.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/FibonacciClock.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/FibonacciClock.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/PinkNoise.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/PrimeClock.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/PrimeClock.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/SampleManager.cpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/SampleManager.hpp
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/WhiteNoise.hpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/Bjorklund.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/Clock.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/CV.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/CVEvent.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/DistributedClock.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/EventEmitter.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/FibonacciClock.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/PrimeClock.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/testrunner.cpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests/testrunner.hpp        
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

    set(VCV_NAME "CharredDesert")
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
