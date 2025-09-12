set(LIB_NAME de_lzma)
project(${LIB_NAME}-dev C ASM)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/lzma/lzmadec.h
        ${LIB_HEADER_DIR}/lzma/config.h
        ${LIB_HEADER_DIR}/lzma/lzma_version.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/lzmadec_private.h
        ${LIB_SOURCE_DIR}/lzmadec_buffer.c
        ${LIB_SOURCE_DIR}/lzmadec_io.c
        ${LIB_SOURCE_DIR}/lzmadec_main.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    # list(APPEND LIB_SOURCES
    # ${LIB_SOURCE_DIR}/lzma/Asm/x86/7zCrcOpt.asm
    # ${LIB_SOURCE_DIR}/lzma/Asm/x86/AesOpt.asm
    # ${LIB_SOURCE_DIR}/lzma/Asm/x86/LzmaDecOpt.asm
    # ${LIB_SOURCE_DIR}/lzma/Asm/x86/XzCrc64Opt.asm
    # )

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/lzma)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC WITH_LZMA=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBLZMA=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZMA=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZMA_H=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZMA_STREAM_ENCODER_MT=1)
    target_compile_definitions(${LIB_NAME} PUBLIC _LZMA_OUT_READ=1)
    target_compile_definitions(${LIB_NAME} PUBLIC _LZMA_IN_CB=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()

endif()

add_subdirectory(test)
