set(LIB_NAME de_DaisySP)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/daisysp.h
        ${LIB_HEADER_DIR}/Control/adenv.h
        ${LIB_HEADER_DIR}/Control/adsr.h
        ${LIB_HEADER_DIR}/Control/line.h
        ${LIB_HEADER_DIR}/Control/phasor.h
        ${LIB_HEADER_DIR}/Drums/analogbassdrum.h
        ${LIB_HEADER_DIR}/Drums/analogsnaredrum.h
        ${LIB_HEADER_DIR}/Drums/hihat.h
        ${LIB_HEADER_DIR}/Drums/synthbassdrum.h
        ${LIB_HEADER_DIR}/Drums/synthsnaredrum.h
        ${LIB_HEADER_DIR}/Dynamics/balance.h
        ${LIB_HEADER_DIR}/Dynamics/compressor.h
        ${LIB_HEADER_DIR}/Dynamics/crossfade.h
        ${LIB_HEADER_DIR}/Dynamics/limiter.h
        ${LIB_HEADER_DIR}/Effects/autowah.h
        ${LIB_HEADER_DIR}/Effects/bitcrush.h
        ${LIB_HEADER_DIR}/Effects/chorus.h
        ${LIB_HEADER_DIR}/Effects/decimator.h
        ${LIB_HEADER_DIR}/Effects/flanger.h
        ${LIB_HEADER_DIR}/Effects/fold.h
        ${LIB_HEADER_DIR}/Effects/overdrive.h
        ${LIB_HEADER_DIR}/Effects/phaser.h
        ${LIB_HEADER_DIR}/Effects/pitchshifter.h
        ${LIB_HEADER_DIR}/Effects/reverbsc.h
        ${LIB_HEADER_DIR}/Effects/sampleratereducer.h
        ${LIB_HEADER_DIR}/Effects/tremolo.h
        ${LIB_HEADER_DIR}/Filters/allpass.h
        ${LIB_HEADER_DIR}/Filters/atone.h
        ${LIB_HEADER_DIR}/Filters/biquad.h
        ${LIB_HEADER_DIR}/Filters/comb.h
        ${LIB_HEADER_DIR}/Filters/fir.h
        ${LIB_HEADER_DIR}/Filters/mode.h
        ${LIB_HEADER_DIR}/Filters/moogladder.h
        ${LIB_HEADER_DIR}/Filters/nlfilt.h
        ${LIB_HEADER_DIR}/Filters/svf.h
        ${LIB_HEADER_DIR}/Filters/tone.h
        ${LIB_HEADER_DIR}/Noise/clockednoise.h
        ${LIB_HEADER_DIR}/Noise/dust.h
        ${LIB_HEADER_DIR}/Noise/fractal_noise.h
        ${LIB_HEADER_DIR}/Noise/grainlet.h
        ${LIB_HEADER_DIR}/Noise/particle.h
        ${LIB_HEADER_DIR}/Noise/whitenoise.h
        ${LIB_HEADER_DIR}/PhysicalModeling/drip.h
        ${LIB_HEADER_DIR}/PhysicalModeling/KarplusString.h
        ${LIB_HEADER_DIR}/PhysicalModeling/modalvoice.h
        ${LIB_HEADER_DIR}/PhysicalModeling/pluck.h
        ${LIB_HEADER_DIR}/PhysicalModeling/PolyPluck.h
        ${LIB_HEADER_DIR}/PhysicalModeling/resonator.h
        ${LIB_HEADER_DIR}/PhysicalModeling/stringvoice.h
        ${LIB_HEADER_DIR}/Synthesis/blosc.h
        ${LIB_HEADER_DIR}/Synthesis/fm2.h
        ${LIB_HEADER_DIR}/Synthesis/formantosc.h
        ${LIB_HEADER_DIR}/Synthesis/harmonic_osc.h
        ${LIB_HEADER_DIR}/Synthesis/oscillator.h
        ${LIB_HEADER_DIR}/Synthesis/oscillatorbank.h
        ${LIB_HEADER_DIR}/Synthesis/variablesawosc.h
        ${LIB_HEADER_DIR}/Synthesis/variableshapeosc.h
        ${LIB_HEADER_DIR}/Synthesis/vosim.h
        ${LIB_HEADER_DIR}/Synthesis/zoscillator.h
        ${LIB_HEADER_DIR}/Utility/dcblock.h
        ${LIB_HEADER_DIR}/Utility/delayline.h
        ${LIB_HEADER_DIR}/Utility/dsp.h
        ${LIB_HEADER_DIR}/Utility/jitter.h
        ${LIB_HEADER_DIR}/Utility/looper.h
        ${LIB_HEADER_DIR}/Utility/maytrig.h
        ${LIB_HEADER_DIR}/Utility/metro.h
        ${LIB_HEADER_DIR}/Utility/port.h
        ${LIB_HEADER_DIR}/Utility/samplehold.h
        ${LIB_HEADER_DIR}/Utility/smooth_random.h
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Control/adenv.cpp
        ${LIB_SOURCE_DIR}/Control/adsr.cpp
        ${LIB_SOURCE_DIR}/Control/line.cpp
        ${LIB_SOURCE_DIR}/Control/phasor.cpp
        ${LIB_SOURCE_DIR}/Drums/analogbassdrum.cpp
        ${LIB_SOURCE_DIR}/Drums/analogsnaredrum.cpp
        ${LIB_SOURCE_DIR}/Drums/hihat.cpp
        ${LIB_SOURCE_DIR}/Drums/synthbassdrum.cpp
        ${LIB_SOURCE_DIR}/Drums/synthsnaredrum.cpp
        ${LIB_SOURCE_DIR}/Dynamics/balance.cpp
        ${LIB_SOURCE_DIR}/Dynamics/compressor.cpp
        ${LIB_SOURCE_DIR}/Dynamics/crossfade.cpp
        ${LIB_SOURCE_DIR}/Dynamics/limiter.cpp
        ${LIB_SOURCE_DIR}/Effects/autowah.cpp
        ${LIB_SOURCE_DIR}/Effects/bitcrush.cpp
        ${LIB_SOURCE_DIR}/Effects/chorus.cpp
        ${LIB_SOURCE_DIR}/Effects/decimator.cpp
        ${LIB_SOURCE_DIR}/Effects/flanger.cpp
        ${LIB_SOURCE_DIR}/Effects/fold.cpp
        ${LIB_SOURCE_DIR}/Effects/overdrive.cpp
        ${LIB_SOURCE_DIR}/Effects/phaser.cpp
        ${LIB_SOURCE_DIR}/Effects/reverbsc.cpp
        ${LIB_SOURCE_DIR}/Effects/sampleratereducer.cpp
        ${LIB_SOURCE_DIR}/Effects/tremolo.cpp
        ${LIB_SOURCE_DIR}/Filters/allpass.cpp
        ${LIB_SOURCE_DIR}/Filters/atone.cpp
        ${LIB_SOURCE_DIR}/Filters/biquad.cpp
        ${LIB_SOURCE_DIR}/Filters/comb.cpp
        ${LIB_SOURCE_DIR}/Filters/mode.cpp
        ${LIB_SOURCE_DIR}/Filters/moogladder.cpp
        ${LIB_SOURCE_DIR}/Filters/nlfilt.cpp
        ${LIB_SOURCE_DIR}/Filters/svf.cpp
        ${LIB_SOURCE_DIR}/Filters/tone.cpp
        ${LIB_SOURCE_DIR}/Noise/clockednoise.cpp
        ${LIB_SOURCE_DIR}/Noise/grainlet.cpp
        ${LIB_SOURCE_DIR}/Noise/particle.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/drip.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/KarplusString.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/modalvoice.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/pluck.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/resonator.cpp
        ${LIB_SOURCE_DIR}/PhysicalModeling/stringvoice.cpp
        ${LIB_SOURCE_DIR}/Synthesis/blosc.cpp
        ${LIB_SOURCE_DIR}/Synthesis/fm2.cpp
        ${LIB_SOURCE_DIR}/Synthesis/formantosc.cpp
        ${LIB_SOURCE_DIR}/Synthesis/oscillator.cpp
        ${LIB_SOURCE_DIR}/Synthesis/oscillatorbank.cpp
        ${LIB_SOURCE_DIR}/Synthesis/variablesawosc.cpp
        ${LIB_SOURCE_DIR}/Synthesis/variableshapeosc.cpp
        ${LIB_SOURCE_DIR}/Synthesis/vosim.cpp
        ${LIB_SOURCE_DIR}/Synthesis/zoscillator.cpp
        ${LIB_SOURCE_DIR}/Utility/dcblock.cpp
        ${LIB_SOURCE_DIR}/Utility/jitter.cpp
        ${LIB_SOURCE_DIR}/Utility/metro.cpp
        ${LIB_SOURCE_DIR}/Utility/port.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Control)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Drums)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Dynamics)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Effects)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Filters)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Noise)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/PhysicalModeling)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Synthesis)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/Utility)

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    # target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_daisysp)

    ### Hide symbol noise
    # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)

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

    # file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
