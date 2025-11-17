set(LIB_NAME de_SynthDevKit)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SynthDevKit/Bjorklund.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/Clock.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/CV.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/CVEvent.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/DistributedClock.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/DTMF.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/EventEmitter.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/FibonacciClock.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/PinkNoise.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/PrimeClock.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/SampleManager.hpp
        ${LIB_HEADER_DIR}/SynthDevKit/WhiteNoise.hpp 
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Bjorklund.cpp
        ${LIB_SOURCE_DIR}/Clock.cpp
        ${LIB_SOURCE_DIR}/CV.cpp
        ${LIB_SOURCE_DIR}/CVEvent.cpp
        ${LIB_SOURCE_DIR}/DistributedClock.cpp
        ${LIB_SOURCE_DIR}/DTMF.cpp
        ${LIB_SOURCE_DIR}/EventEmitter.cpp
        ${LIB_SOURCE_DIR}/FibonacciClock.cpp
        ${LIB_SOURCE_DIR}/PrimeClock.cpp
        ${LIB_SOURCE_DIR}/SampleManager.cpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/SynthDevKit)

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PUBLIC OSC_ON=1)

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
            # target_link_libraries(${LIB_NAME} PRIVATE ws2_32)
            # target_link_libraries(${LIB_NAME} PRIVATE winmm)
        # endif()
    endif()

endif()

# add_subdirectory(test)
