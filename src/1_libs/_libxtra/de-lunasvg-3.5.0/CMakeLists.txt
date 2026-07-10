set(LIB_NAME de_lunasvg_350)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    set(LUNASVG_VERSION_MAJOR 3)
    set(LUNASVG_VERSION_MINOR 5)
    set(LUNASVG_VERSION_MICRO 0)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/lunasvg.h        
	)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES 
		${LIB_SOURCE_DIR}/graphics.cpp
		${LIB_SOURCE_DIR}/graphics.h
		${LIB_SOURCE_DIR}/lunasvg.cpp
		${LIB_SOURCE_DIR}/svgelement.cpp
		${LIB_SOURCE_DIR}/svgelement.h
		${LIB_SOURCE_DIR}/svggeometryelement.cpp
		${LIB_SOURCE_DIR}/svggeometryelement.h
		${LIB_SOURCE_DIR}/svglayoutstate.cpp
		${LIB_SOURCE_DIR}/svglayoutstate.h
		${LIB_SOURCE_DIR}/svgpaintelement.cpp
		${LIB_SOURCE_DIR}/svgpaintelement.h
		${LIB_SOURCE_DIR}/svgparser.cpp
		${LIB_SOURCE_DIR}/svgparserutils.h
		${LIB_SOURCE_DIR}/svgproperty.cpp
		${LIB_SOURCE_DIR}/svgproperty.h
		${LIB_SOURCE_DIR}/svgrenderstate.cpp
		${LIB_SOURCE_DIR}/svgrenderstate.h
		${LIB_SOURCE_DIR}/svgtextelement.cpp
		${LIB_SOURCE_DIR}/svgtextelement.h
	)

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC LUNASVG_BUILD_STATIC)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})	

    target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_BUILD)
    # target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_DISABLE_LOAD_SYSTEM_FONTS)
    	
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

    target_link_libraries(${LIB_NAME} PUBLIC de_plutovg_131)

endif()

add_subdirectory(test)