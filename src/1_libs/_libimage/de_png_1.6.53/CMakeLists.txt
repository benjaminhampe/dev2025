set(LIB_NAME de_png)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/png.h
        ${LIB_HEADER_DIR}/pngconf.h
        ${LIB_HEADER_DIR}/pngdebug.h
        ${LIB_HEADER_DIR}/pnginfo.h
        ${LIB_HEADER_DIR}/pnglibconf.h
        ${LIB_HEADER_DIR}/pngpriv.h
        ${LIB_HEADER_DIR}/pngstruct.h
    )
    set(LIB_SOURCES
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

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PRIVATE PNG_BUILD_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libjpeg)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PNG=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBPNG=1)

    # target_compile_definitions(${LIB_NAME} PUBLIC PNG_bKGD_SUPPORTED=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC PNG_READ_BACKGROUND_SUPPORTED=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)

endif()

# add_subdirectory(test)
