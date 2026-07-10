set(LIB_NAME de_plutovg_131)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    set(PLUTOVG_VERSION_MAJOR 1)
    set(PLUTOVG_VERSION_MINOR 3)
    set(PLUTOVG_VERSION_MICRO 1)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/plutovg.h       
	)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES 
		${LIB_SOURCE_DIR}/plutovg-blend.c
		${LIB_SOURCE_DIR}/plutovg-canvas.c
		${LIB_SOURCE_DIR}/plutovg-font.c
		${LIB_SOURCE_DIR}/plutovg-ft-math.c
		${LIB_SOURCE_DIR}/plutovg-ft-math.h
		${LIB_SOURCE_DIR}/plutovg-ft-raster.c
		${LIB_SOURCE_DIR}/plutovg-ft-raster.h
		${LIB_SOURCE_DIR}/plutovg-ft-stroker.c
		${LIB_SOURCE_DIR}/plutovg-ft-stroker.h
		${LIB_SOURCE_DIR}/plutovg-ft-types.h
		${LIB_SOURCE_DIR}/plutovg-matrix.c
		${LIB_SOURCE_DIR}/plutovg-paint.c
		${LIB_SOURCE_DIR}/plutovg-path.c
		${LIB_SOURCE_DIR}/plutovg-private.h
		${LIB_SOURCE_DIR}/plutovg-rasterize.c
		${LIB_SOURCE_DIR}/plutovg-stb-image-write.h
		${LIB_SOURCE_DIR}/plutovg-stb-image.h
		${LIB_SOURCE_DIR}/plutovg-stb-truetype.h
		${LIB_SOURCE_DIR}/plutovg-surface.c
		${LIB_SOURCE_DIR}/plutovg-utils.h
	)

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC PLUTOVG_BUILD_STATIC=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
		
    target_compile_definitions(${LIB_NAME} PRIVATE PLUTOVG_BUILD=1)
    
    # target_compile_definitions(${LIB_NAME} PRIVATE PLUTOVG_DISABLE_FONT_FACE_CACHE_LOAD=1)
    
    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    # if (MSVC)
    # else()
    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    # endif()
    # target_link_libraries(${LIB_NAME} PUBLIC de_irrlicht)
    # target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    # #target_link_libraries(${LIB_NAME} PUBLIC de_glad)

    target_link_libraries(${LIB_NAME} PUBLIC m)

    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)

endif()

add_subdirectory(test)