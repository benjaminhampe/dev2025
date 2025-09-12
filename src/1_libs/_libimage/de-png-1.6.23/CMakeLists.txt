set(LIB_NAME de_png) # We dont like to clash with system libs.
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/png)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/png.h
        ${LIB_HEADER_DIR}/pngconf.h
        ${LIB_HEADER_DIR}/pnglibconf.h
        ${LIB_HEADER_DIR}/pngdebug.h
        ${LIB_HEADER_DIR}/pnginfo.h
        ${LIB_HEADER_DIR}/pngstruct.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/pngpriv.h
        ${LIB_SOURCE_DIR}/png.c
        ${LIB_SOURCE_DIR}/pngerror.c
        ${LIB_SOURCE_DIR}/pngget.c
        ${LIB_SOURCE_DIR}/pngmem.c
        ${LIB_SOURCE_DIR}/pngpread.c
        ${LIB_SOURCE_DIR}/pngread.c
        ${LIB_SOURCE_DIR}/pngrio.c
        ${LIB_SOURCE_DIR}/pngrtran.c
        ${LIB_SOURCE_DIR}/pngrutil.c
        ${LIB_SOURCE_DIR}/pngset.c
        ${LIB_SOURCE_DIR}/pngtrans.c
        ${LIB_SOURCE_DIR}/pngwio.c
        ${LIB_SOURCE_DIR}/pngwrite.c
        ${LIB_SOURCE_DIR}/pngwtran.c
        ${LIB_SOURCE_DIR}/pngwutil.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # For the SDK
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}) # For the sources

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBPNG=1)

    target_compile_definitions(${LIB_NAME} PUBLIC PNG_bKGD_SUPPORTED=1)
    target_compile_definitions(${LIB_NAME} PUBLIC PNG_READ_BACKGROUND_SUPPORTED=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
        target_link_libraries(${LIB_NAME} PRIVATE m)        
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${PNG_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
