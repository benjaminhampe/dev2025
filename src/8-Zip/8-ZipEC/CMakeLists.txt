set(LIB_NAME 8z_dll)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    # set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(LIB_HEADERS
    #     ${LIB_HEADER_DIR}/8-ZipClassFactory.h
    #     ${LIB_HEADER_DIR}/8-ZipShellExtension.h
    #     ${LIB_HEADER_DIR}/8-ZipShellExtension.def
    # )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(LIB_SOURCES
    #     ${LIB_SOURCE_DIR}/8-ZipClassFactory.cpp
    #     ${LIB_SOURCE_DIR}/8-ZipShellExtension.cpp
    #     ${LIB_SOURCE_DIR}/main_8-ZipShellExtension.cpp
    # )

    file(GLOB_RECURSE LIB_HEADERS ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES ${LIB_SOURCE_DIR}/*.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    # else()
      # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
      # add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_link_libraries(${LIB_NAME} PRIVATE libFileInfo)

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE ole32)
        target_link_libraries(${LIB_NAME} PRIVATE uuid)
        target_link_libraries(${LIB_NAME} PRIVATE shlwapi)
      # target_link_libraries(${LIB_NAME} PRIVATE shcore)
      # target_link_libraries(${LIB_NAME} PRIVATE dbghelp)
    endif()

    if (MSVC)
        message("Eff your compiler")
        # DEF-Datei für korrekte COM-Exports
        # set_target_properties(${LIB_NAME} PROPERTIES
        #     LINK_FLAGS "/DEF:${LIB_SOURCE_DIR}/8-ZipShellExtension.def"
        # )
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-dll-attribute-on-redeclaration )

        # Entfernt stdcall-Dekorationen (mingw-llvm wichtig!)
        target_link_options(${LIB_NAME} PRIVATE "-Wl,--kill-at")
      # target_link_options(${LIB_NAME} PRIVATE "-Wl,--add-stdcall-alias")

        # target_link_options(${LIB_NAME} PRIVATE
        #     "-Wl,${LIB_SOURCE_DIR}/8-ZipShellExtension.def"
        # )

        # set_target_properties(${LIB_NAME} PROPERTIES
        #     LINK_FLAGS "${LINK_FLAGS} -Wl,--kill-at -Wl,--add-stdcall-alias"
        #     LINK_DEPENDS ${LIB_SOURCE_DIR}/8-ZipShellExtension.def
        # )
    endif()

    # Remove 'lib' prefix
    set_target_properties(${LIB_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        OUTPUT_NAME "8z"
        # ARCHIVE_OUTPUT_NAME "8zEC_dll"
    )

    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Building in Release mode, strip symbols")
        add_custom_command(TARGET ${LIB_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            COMMENT "Stripping debug symbols"
        )
    endif()


    # file(COPY media/8-Zip DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
