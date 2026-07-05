set(APP_NAME Abenton_SineMachine5_vst3)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_HEADERS ${APP_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_DIR}/*.cpp)
    # set(APP_SOURCES ${APP_SOURCE_DIR}/main.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    # message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    # add_executable(${APP_NAME} ${APP_HEADERS} ${APP_SOURCES})

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${APP_NAME} SHARED "")

        # target_compile_options(${LIB_NAME} PRIVATE -g)
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${APP_NAME} PRIVATE ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
        target_compile_options(${APP_NAME} PRIVATE -Wno-pragma-pack)
    endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    target_link_libraries(${APP_NAME} PRIVATE de_vst3sdk_380)
    target_link_libraries(${APP_NAME} PRIVATE de_glew)

    set_target_properties(${APP_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".vst3"
        OUTPUT_NAME "SineMachine5"
        # ARCHIVE_OUTPUT_NAME "Abenton_Spectrum3d_1.0.0_x64_vst3"
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # message(STATUS "Building in Release mode, strip symbols")

        # 1. Stripping Debug Symbols from an Executable
        #  objcopy --strip-debug myprogram
        #   - Removes debugging symbols but keeps function names.
        #
        # 2. Creating a Separate Debug Symbol File
        # If you want to keep debugging symbols in a separate file for later use:
        #  objcopy --only-keep-debug myprogram myprogram.debug
        #  objcopy --strip-debug myprogram
        #  objcopy --add-gnu-debuglink=myprogram.debug myprogram

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${APP_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            COMMENT "Stripping debug symbols"
        )

    endif()

    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
