set(LIB_NAME de_bzip3)
project(${LIB_NAME}
        VERSION 1.5.4
        DESCRIPTION "A better and stronger spiritual successor to BZip2"
        HOMEPAGE_URL "https://github.com/iczelia/bzip3"
        LANGUAGES C)

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # Finde die plattformunabhängige Thread-Bibliothek
    find_package(Threads REQUIRED)
    
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/bzip3/libbz3.h
        ${LIB_HEADER_DIR}/bzip3/libsais.h
        ${LIB_HEADER_DIR}/bzip3/libsais_common.h
        ${LIB_HEADER_DIR}/bzip3/yarg.h
        # help
        ${LIB_HEADER_DIR}/bzip3/bzip3_compress_help.h
        ${LIB_HEADER_DIR}/bzip3/bzip3_compress_speed.h
        ${LIB_HEADER_DIR}/bzip3/bzip3_decompress_help.h
        ${LIB_HEADER_DIR}/bzip3/bzip3_decompress_speed.h
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/libbz3.c
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
        # if (WIN32)
            # target_compile_definitions(${LIB_NAME} PRIVATE BZIP3_DLL_EXPORT=1)
            # target_compile_definitions(${LIB_NAME} INTERFACE BZIP3_DLL_IMPORT=1)
        # endif()        
    # else()
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
      add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC VERSION="${PROJECT_VERSION}")

    target_compile_definitions(${LIB_NAME} PUBLIC PTHREAD)
       
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2604)

    if (WIN32)
        # target_link_libraries(${LIB_NAME} PRIVATE ole32)
        # target_link_libraries(${LIB_NAME} PRIVATE uuid)
        # target_link_libraries(${LIB_NAME} PRIVATE shlwapi)
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
        target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads)
    
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-dll-attribute-on-redeclaration )

        # Entfernt stdcall-Dekorationen (mingw-llvm wichtig!)
        # target_link_options(${LIB_NAME} PRIVATE "-Wl,--kill-at")
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
    # set_target_properties(${LIB_NAME} PROPERTIES
        # PREFIX ""
        # SUFFIX ".dll"
        # OUTPUT_NAME "8-Zip"
        # # ARCHIVE_OUTPUT_NAME "8-Zip_dll"
    # )

    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
    #     message(STATUS "Building in Release mode, strip symbols")
    #     add_custom_command(TARGET ${LIB_NAME} POST_BUILD
    #         COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
    #         COMMENT "Stripping debug symbols"
    #     )
    # endif()


    # file(COPY media/8-Zip DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()

add_subdirectory(test)