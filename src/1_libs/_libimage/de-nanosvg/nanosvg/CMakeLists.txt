set(LIB_NAME de_nanosvg)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS ${LIB_HEADER_DIR}/nanosvg.h)
	set(LIB_SOURCES ${LIB_SOURCE_DIR}/nanosvg.c)

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
	target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBNANOSVG=1)
    target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_IMPLEMENTATION=1) # Must be defined once in exactly one .cpp file to compile the parser.
    target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_ALL_COLOR_KEYWORDS=1) # Enables all CSS color names (140+). Without this, only hex + rgb() are supported.
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_RGB=1) # Override how RGB values are packed. Example: c

    #define NANOSVG_RGB(r,g,b) ((r<<24)|(g<<16)|(b<<8)|255)

    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_ATTRS=1) # # Max XML attributes per element (default 128).
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_NESTING=1) # # Max XML nesting depth (default 64).
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_PATHS=1) # # Max paths per shape (default 1024).
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_SHAPES=1) # # Max shapes in the SVG (default 2048).
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_TEXT=1) # # Max text buffer size (default 512).
    # target_compile_definitions(${LIB_NAME} PUBLIC NANOSVG_MAX_TITLE=1) # # Max title length (default 512).

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
	else()
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
