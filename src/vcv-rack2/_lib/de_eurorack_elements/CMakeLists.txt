set(LIB_NAME de_eurorack_elements)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        # ${LIB_HEADER_DIR}/elements/cv_scaler.h
        ${LIB_HEADER_DIR}/elements/meter.h
        ${LIB_HEADER_DIR}/elements/resources.h
        # ${LIB_HEADER_DIR}/elements/ui.h
        # ${LIB_HEADER_DIR}/elements/drivers/codec.h
        # ${LIB_HEADER_DIR}/elements/drivers/cv_adc.h
        # ${LIB_HEADER_DIR}/elements/drivers/debug_pin.h
        # ${LIB_HEADER_DIR}/elements/drivers/debug_port.h
        # ${LIB_HEADER_DIR}/elements/drivers/gate_input.h
        # ${LIB_HEADER_DIR}/elements/drivers/leds.h
        # ${LIB_HEADER_DIR}/elements/drivers/pots_adc.h
        # ${LIB_HEADER_DIR}/elements/drivers/switch.h
        # ${LIB_HEADER_DIR}/elements/drivers/system.h
        ${LIB_HEADER_DIR}/elements/dsp/dsp.h
        ${LIB_HEADER_DIR}/elements/dsp/exciter.h
        ${LIB_HEADER_DIR}/elements/dsp/multistage_envelope.h
        ${LIB_HEADER_DIR}/elements/dsp/ominous_voice.h
        ${LIB_HEADER_DIR}/elements/dsp/part.h
        ${LIB_HEADER_DIR}/elements/dsp/patch.h
        ${LIB_HEADER_DIR}/elements/dsp/resonator.h
        ${LIB_HEADER_DIR}/elements/dsp/string.h
        ${LIB_HEADER_DIR}/elements/dsp/tube.h
        ${LIB_HEADER_DIR}/elements/dsp/voice.h
        ${LIB_HEADER_DIR}/elements/dsp/fx/diffuser.h
        ${LIB_HEADER_DIR}/elements/dsp/fx/fx_engine.h
        ${LIB_HEADER_DIR}/elements/dsp/fx/reverb.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/elements/bootloader/bootloader.cc
        # ${LIB_SOURCE_DIR}/elements/bootloader/makefile
        # ${LIB_SOURCE_DIR}/elements/bootloader/__init__.py
        # ${LIB_SOURCE_DIR}/elements/cv_scaler.cc
        # ${LIB_SOURCE_DIR}/elements/elements.cc
        ${LIB_SOURCE_DIR}/elements/resources.cc
        # ${LIB_SOURCE_DIR}/elements/ui.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/codec.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/cv_adc.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/debug_port.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/gate_input.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/leds.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/pots_adc.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/switch.cc
        # ${LIB_SOURCE_DIR}/elements/drivers/system.cc
        ${LIB_SOURCE_DIR}/elements/dsp/exciter.cc
        ${LIB_SOURCE_DIR}/elements/dsp/multistage_envelope.cc
        ${LIB_SOURCE_DIR}/elements/dsp/ominous_voice.cc
        ${LIB_SOURCE_DIR}/elements/dsp/part.cc
        ${LIB_SOURCE_DIR}/elements/dsp/resonator.cc
        ${LIB_SOURCE_DIR}/elements/dsp/string.cc
        ${LIB_SOURCE_DIR}/elements/dsp/tube.cc
        ${LIB_SOURCE_DIR}/elements/dsp/voice.cc
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC NOASM=1)
    target_compile_definitions(${LIB_NAME} PRIVATE TEST=1)

    target_link_libraries(${LIB_NAME} PRIVATE de_eurorack)

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
