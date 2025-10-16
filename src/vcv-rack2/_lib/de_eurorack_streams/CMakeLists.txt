set(LIB_NAME de_eurorack_streams)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/streams/compressor.h
        ${LIB_HEADER_DIR}/streams/envelope.h
        ${LIB_HEADER_DIR}/streams/filter_controller.h
        ${LIB_HEADER_DIR}/streams/follower.h
        ${LIB_HEADER_DIR}/streams/gain.h
        ${LIB_HEADER_DIR}/streams/lorenz_generator.h
        ${LIB_HEADER_DIR}/streams/meta_parameters.h
        ${LIB_HEADER_DIR}/streams/processor.h
        ${LIB_HEADER_DIR}/streams/resources.h
        ${LIB_HEADER_DIR}/streams/svf.h
        ${LIB_HEADER_DIR}/streams/vactrol.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/streams/compressor.cc
        ${LIB_SOURCE_DIR}/streams/envelope.cc
        ${LIB_SOURCE_DIR}/streams/follower.cc
        ${LIB_SOURCE_DIR}/streams/lorenz_generator.cc
        ${LIB_SOURCE_DIR}/streams/processor.cc
        ${LIB_SOURCE_DIR}/streams/resources.cc
        ${LIB_SOURCE_DIR}/streams/svf.cc
        ${LIB_SOURCE_DIR}/streams/vactrol.cc        
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
