set(LIB_NAME de_rack2_nanovg)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/nanovg/nanovg.h
        ${LIB_HEADER_DIR}/nanovg/nanovg_gl.h
        ${LIB_HEADER_DIR}/nanovg/nanovg_gl_utils.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/fontstash.h
        ${LIB_SOURCE_DIR}/nanovg.cpp
        ${LIB_SOURCE_DIR}/nanovg_gl.cpp
        ${LIB_SOURCE_DIR}/nanovg_gl_utils.cpp
        ${LIB_SOURCE_DIR}/stb_image.h
        ${LIB_SOURCE_DIR}/stb_truetype.h
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC NANOVG_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/nanovg)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBNANOVG=1)
    target_compile_definitions(${LIB_NAME} PUBLIC NANOVG_GL_IMPLEMENTATION=1)
    target_compile_definitions(${LIB_NAME} PUBLIC NANOVG_GL3_IMPLEMENTATION=1)
    target_compile_definitions(${LIB_NAME} PUBLIC NANOVG_GL_USE_UNIFORMBUFFER=1)
    target_compile_definitions(${LIB_NAME} PUBLIC NANOVG_GL3=1)
    target_compile_definitions(${LIB_NAME} PRIVATE STB_IMAGE_STATIC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE STB_TRUETYPE_IMPLEMENTATION=1)
    target_compile_definitions(${LIB_NAME} PRIVATE FONTSTASH_IMPLEMENTATION=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_rack2_glew)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
	
    # file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
