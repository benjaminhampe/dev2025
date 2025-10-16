set(LIB_NAME de_gamma_100_docB)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/Gamma/Access.h
        ${LIB_HEADER_DIR}/Gamma/Allocator.h
        ${LIB_HEADER_DIR}/Gamma/Analysis.h
        ${LIB_HEADER_DIR}/Gamma/arr.h
        ${LIB_HEADER_DIR}/Gamma/AudioIO.h
        ${LIB_HEADER_DIR}/Gamma/Config.h
        ${LIB_HEADER_DIR}/Gamma/Constants.h
        ${LIB_HEADER_DIR}/Gamma/Containers.h
        ${LIB_HEADER_DIR}/Gamma/Conversion.h
        ${LIB_HEADER_DIR}/Gamma/Delay.h
        ${LIB_HEADER_DIR}/Gamma/DFT.h
        ${LIB_HEADER_DIR}/Gamma/Domain.h
        ${LIB_HEADER_DIR}/Gamma/Effects.h
        ${LIB_HEADER_DIR}/Gamma/Envelope.h
        ${LIB_HEADER_DIR}/Gamma/FFT.h
        ${LIB_HEADER_DIR}/Gamma/Filter.h
        ${LIB_HEADER_DIR}/Gamma/FormantData.h
        ${LIB_HEADER_DIR}/Gamma/Gamma.h
        ${LIB_HEADER_DIR}/Gamma/gen.h
        ${LIB_HEADER_DIR}/Gamma/HRFilter.h
        ${LIB_HEADER_DIR}/Gamma/ipl.h
        ${LIB_HEADER_DIR}/Gamma/mem.h
        ${LIB_HEADER_DIR}/Gamma/Node.h
        ${LIB_HEADER_DIR}/Gamma/Noise.h
        ${LIB_HEADER_DIR}/Gamma/Oscillator.h
        ${LIB_HEADER_DIR}/Gamma/Print.h
        ${LIB_HEADER_DIR}/Gamma/pstdint.h
        ${LIB_HEADER_DIR}/Gamma/Recorder.h
        ${LIB_HEADER_DIR}/Gamma/rnd.h
        ${LIB_HEADER_DIR}/Gamma/Sample.h
        ${LIB_HEADER_DIR}/Gamma/SamplePlayer.h
        ${LIB_HEADER_DIR}/Gamma/Scheduler.h
        ${LIB_HEADER_DIR}/Gamma/scl.h
        ${LIB_HEADER_DIR}/Gamma/SoundFile.h
        ${LIB_HEADER_DIR}/Gamma/Spatial.h
        ${LIB_HEADER_DIR}/Gamma/std_swap.h
        ${LIB_HEADER_DIR}/Gamma/Strategy.h
        ${LIB_HEADER_DIR}/Gamma/tbl.h
        ${LIB_HEADER_DIR}/Gamma/Thread.h
        ${LIB_HEADER_DIR}/Gamma/Timer.h
        ${LIB_HEADER_DIR}/Gamma/TransferFunc.h
        ${LIB_HEADER_DIR}/Gamma/Types.h
        ${LIB_HEADER_DIR}/Gamma/UnitMaps.h
        ${LIB_HEADER_DIR}/Gamma/Voices.h        
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/arr.cpp
        # ${LIB_SOURCE_DIR}/AudioIO.cpp
        ${LIB_SOURCE_DIR}/Conversion.cpp
        ${LIB_SOURCE_DIR}/DFT.cpp
        ${LIB_SOURCE_DIR}/Domain.cpp
        ${LIB_SOURCE_DIR}/fftpack++.h
        ${LIB_SOURCE_DIR}/fftpack++.inc
        ${LIB_SOURCE_DIR}/fftpack++1.cpp
        ${LIB_SOURCE_DIR}/fftpack++2.cpp
        ${LIB_SOURCE_DIR}/FFT_fftpack.cpp
        ${LIB_SOURCE_DIR}/Print.cpp
        ${LIB_SOURCE_DIR}/Recorder.cpp
        ${LIB_SOURCE_DIR}/Scheduler.cpp
        ${LIB_SOURCE_DIR}/scl.cpp
        ${LIB_SOURCE_DIR}/SoundFile.cpp
        ${LIB_SOURCE_DIR}/SoundFileIO.h
        ${LIB_SOURCE_DIR}/Timer.cpp        
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
    #     message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    #     add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
    #     target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
    #     target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})


    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC NOASM=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC TEST=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    # target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_eurorack)
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

endif()

# add_subdirectory(test)
