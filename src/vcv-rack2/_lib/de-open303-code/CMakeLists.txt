set(LIB_NAME de_open303_code)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/DSPCode/GlobalDefinitions.h
        ${LIB_HEADER_DIR}/DSPCode/GlobalFunctions.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_AcidPattern.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_AcidSequencer.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_AnalogEnvelope.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_BiquadFilter.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_BlendOscillator.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_Complex.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_DecayEnvelope.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_EllipticQuarterBandFilter.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_FourierTransformerRadix2.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_FunctionTemplates.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_LeakyIntegrator.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_MidiNoteEvent.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_MipMappedWaveTable.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_NumberManipulations.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_OnePoleFilter.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_Open303.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_RealFunctions.h
        ${LIB_HEADER_DIR}/DSPCode/rosic_TeeBeeFilter.h
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/fft4g.cpp
        ${LIB_SOURCE_DIR}/GlobalFunctions.cpp
        ${LIB_SOURCE_DIR}/rosic_AcidPattern.cpp
        ${LIB_SOURCE_DIR}/rosic_AcidSequencer.cpp
        ${LIB_SOURCE_DIR}/rosic_AnalogEnvelope.cpp
        ${LIB_SOURCE_DIR}/rosic_BiquadFilter.cpp
        ${LIB_SOURCE_DIR}/rosic_BlendOscillator.cpp
        ${LIB_SOURCE_DIR}/rosic_Complex.cpp
        ${LIB_SOURCE_DIR}/rosic_DecayEnvelope.cpp
        ${LIB_SOURCE_DIR}/rosic_EllipticQuarterBandFilter.cpp
        ${LIB_SOURCE_DIR}/rosic_FourierTransformerRadix2.cpp
        ${LIB_SOURCE_DIR}/rosic_FunctionTemplates.cpp
        ${LIB_SOURCE_DIR}/rosic_LeakyIntegrator.cpp
        ${LIB_SOURCE_DIR}/rosic_MidiNoteEvent.cpp
        ${LIB_SOURCE_DIR}/rosic_MipMappedWaveTable.cpp
        ${LIB_SOURCE_DIR}/rosic_NumberManipulations.cpp
        ${LIB_SOURCE_DIR}/rosic_OnePoleFilter.cpp
        ${LIB_SOURCE_DIR}/rosic_Open303.cpp
        ${LIB_SOURCE_DIR}/rosic_RealFunctions.cpp
        ${LIB_SOURCE_DIR}/rosic_TeeBeeFilter.cpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/DSPCode)

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    # target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
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
