set(LIB_NAME plugin_99_SynthKit)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/SynthKit.cpp
        ${LIB_SOURCE_DIR}/SynthKit.hpp    
        ${LIB_SOURCE_DIR}/controller/1x8.cpp
        ${LIB_SOURCE_DIR}/controller/1x8.hpp
        ${LIB_SOURCE_DIR}/controller/1x8CV.cpp
        ${LIB_SOURCE_DIR}/controller/1x8CV.hpp
        ${LIB_SOURCE_DIR}/controller/Addition.cpp
        ${LIB_SOURCE_DIR}/controller/Addition.hpp
        ${LIB_SOURCE_DIR}/controller/And.cpp
        ${LIB_SOURCE_DIR}/controller/And.hpp
        ${LIB_SOURCE_DIR}/controller/ClockDivider.cpp
        ${LIB_SOURCE_DIR}/controller/ClockDivider.hpp
        ${LIB_SOURCE_DIR}/controller/FibonacciClockDivider.cpp
        ${LIB_SOURCE_DIR}/controller/FibonacciClockDivider.hpp
        ${LIB_SOURCE_DIR}/controller/Or.cpp
        ${LIB_SOURCE_DIR}/controller/Or.hpp
        ${LIB_SOURCE_DIR}/controller/PrimeClockDivider.cpp
        ${LIB_SOURCE_DIR}/controller/PrimeClockDivider.hpp
        ${LIB_SOURCE_DIR}/controller/RotatingClockDivider.cpp
        ${LIB_SOURCE_DIR}/controller/RotatingClockDivider.hpp
        ${LIB_SOURCE_DIR}/controller/RotatingClockDivider2.cpp
        ${LIB_SOURCE_DIR}/controller/RotatingClockDivider2.hpp
        ${LIB_SOURCE_DIR}/controller/Seq4.cpp
        ${LIB_SOURCE_DIR}/controller/Seq4.hpp
        ${LIB_SOURCE_DIR}/controller/Seq8.cpp
        ${LIB_SOURCE_DIR}/controller/Seq8.hpp
        ${LIB_SOURCE_DIR}/controller/Subtraction.cpp
        ${LIB_SOURCE_DIR}/controller/Subtraction.hpp
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
        ${LIB_SOURCE_DIR}/deps/SynthDevKit/src/WhiteNoise.hpp
        ${LIB_SOURCE_DIR}/view/1x8.cpp
        ${LIB_SOURCE_DIR}/view/1x8CV.cpp
        ${LIB_SOURCE_DIR}/view/Addition.cpp
        ${LIB_SOURCE_DIR}/view/And.cpp
        ${LIB_SOURCE_DIR}/view/ClockDivider.cpp
        ${LIB_SOURCE_DIR}/view/FibonacciClockDivider.cpp
        ${LIB_SOURCE_DIR}/view/knobs.hpp
        ${LIB_SOURCE_DIR}/view/Or.cpp
        ${LIB_SOURCE_DIR}/view/PrimeClockDivider.cpp
        ${LIB_SOURCE_DIR}/view/RotatingClockDivider.cpp
        ${LIB_SOURCE_DIR}/view/RotatingClockDivider2.cpp
        ${LIB_SOURCE_DIR}/view/Seq4.cpp
        ${LIB_SOURCE_DIR}/view/Seq8.cpp
        ${LIB_SOURCE_DIR}/view/Subtraction.cpp
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

    set(VCV_NAME "SynthKit")
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
