project(OpenGLAppDemo)

if (TARGET ${PROJECT_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${PROJECT_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/main.c)

    if (WIN32)
        enable_language(RC)
        list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${PROJECT_NAME}${ColourReset}")
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

    if (MSVC)
    else()
        # target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    endif()
 
    target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE=1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE=1)	
    target_link_libraries(${PROJECT_NAME} PRIVATE glu32 opengl32 uuid ole32 oleaut32 ComCtl32 SetupAPI)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm user32 gdi32 kernel32 advapi32 shell32)
        
    # target_link_libraries(${PROJECT_NAME} PRIVATE de_glew)
    # target_link_libraries(${PROJECT_NAME} PRIVATE ComCtl32)
    # target_link_libraries(${PROJECT_NAME} PRIVATE glu32 opengl32) # glmf32

    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-command-line-argument)
    # target_compile_options(${PROJECT_NAME} PRIVATE -static)
    target_compile_options(${PROJECT_NAME} PRIVATE -static-libstdc++)
    target_compile_options(${PROJECT_NAME} PRIVATE -static-libgcc)
    # target_link_options(${PROJECT_NAME} PRIVATE -static)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc)
    message("${PROJECT_NAME} will link statically to GNU runtime.")

    # target_link_libraries(${PROJECT_NAME} PRIVATE de_gui_2504)

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
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Stripping debug symbols"
        )

    endif()

    file(COPY media/OpenGLAppDemo DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
