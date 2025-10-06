set(LIB_NAME plugin_63_DrumKit)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/DrumKit.cpp
        ${LIB_SOURCE_DIR}/DrumKit.hpp    
        ${LIB_SOURCE_DIR}/component/button.hpp
        ${LIB_SOURCE_DIR}/component/display.hpp
        ${LIB_SOURCE_DIR}/component/knob.hpp
        ${LIB_SOURCE_DIR}/component/light.hpp
        ${LIB_SOURCE_DIR}/component/pad.hpp
        ${LIB_SOURCE_DIR}/component/port.hpp
        ${LIB_SOURCE_DIR}/component/shadow.hpp
        ${LIB_SOURCE_DIR}/component/switch.hpp
        ${LIB_SOURCE_DIR}/component/tooltips.hpp
        ${LIB_SOURCE_DIR}/controller/Baronial.cpp
        ${LIB_SOURCE_DIR}/controller/Baronial.hpp
        ${LIB_SOURCE_DIR}/controller/BD9.cpp
        ${LIB_SOURCE_DIR}/controller/BD9.hpp
        ${LIB_SOURCE_DIR}/controller/ClosedHH.cpp
        ${LIB_SOURCE_DIR}/controller/ClosedHH.hpp
        ${LIB_SOURCE_DIR}/controller/CR78.cpp
        ${LIB_SOURCE_DIR}/controller/CR78.hpp
        ${LIB_SOURCE_DIR}/controller/DMX.cpp
        ${LIB_SOURCE_DIR}/controller/DMX.hpp
        ${LIB_SOURCE_DIR}/controller/Gnome.cpp
        ${LIB_SOURCE_DIR}/controller/Gnome.hpp
        ${LIB_SOURCE_DIR}/controller/Marionette.cpp
        ${LIB_SOURCE_DIR}/controller/Marionette.hpp
        ${LIB_SOURCE_DIR}/controller/OpenHH.cpp
        ${LIB_SOURCE_DIR}/controller/OpenHH.hpp
        ${LIB_SOURCE_DIR}/controller/SampleController.cpp
        ${LIB_SOURCE_DIR}/controller/SampleController.hpp
        ${LIB_SOURCE_DIR}/controller/SBD.cpp
        ${LIB_SOURCE_DIR}/controller/SBD.hpp
        ${LIB_SOURCE_DIR}/controller/Sequencer.cpp
        ${LIB_SOURCE_DIR}/controller/Sequencer.hpp
        ${LIB_SOURCE_DIR}/controller/Snare.cpp
        ${LIB_SOURCE_DIR}/controller/Snare.hpp
        ${LIB_SOURCE_DIR}/controller/Tomi.cpp
        ${LIB_SOURCE_DIR}/controller/Tomi.hpp
        ${LIB_SOURCE_DIR}/deps/ADSR.cpp
        ${LIB_SOURCE_DIR}/deps/ADSR.h
        ${LIB_SOURCE_DIR}/deps/LFO.cpp
        ${LIB_SOURCE_DIR}/deps/LFO.hpp
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
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/WhiteNoise.hpp
        # ${LIB_SOURCE_DIR}/deps/SynthDevKit/tests
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
        ${LIB_SOURCE_DIR}/model/Envelope.hpp
        ${LIB_SOURCE_DIR}/model/SampleManager.cpp
        ${LIB_SOURCE_DIR}/model/SampleManager.hpp
        ${LIB_SOURCE_DIR}/view/Baronial.cpp
        ${LIB_SOURCE_DIR}/view/BD9.cpp
        ${LIB_SOURCE_DIR}/view/ClosedHH.cpp
        ${LIB_SOURCE_DIR}/view/CR78.cpp
        ${LIB_SOURCE_DIR}/view/DMX.cpp
        ${LIB_SOURCE_DIR}/view/Gnome.cpp
        ${LIB_SOURCE_DIR}/view/Marionette.cpp
        ${LIB_SOURCE_DIR}/view/OpenHH.cpp
        ${LIB_SOURCE_DIR}/view/SBD.cpp
        ${LIB_SOURCE_DIR}/view/Sequencer.cpp
        ${LIB_SOURCE_DIR}/view/Snare.cpp
        ${LIB_SOURCE_DIR}/view/Tomi.cpp
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

    set(VCV_NAME "DrumKit")
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
