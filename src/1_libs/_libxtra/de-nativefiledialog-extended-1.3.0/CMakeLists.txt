set(LIB_NAME de_nfd_130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

	set(LIB_HEADERS
		${LIB_HEADER_DIR}/nfd.h
		# ${LIB_HEADER_DIR}/nfd.hpp
		${LIB_HEADER_DIR}/nfd_glfw3.h
		${LIB_HEADER_DIR}/nfd_sdl2.h        
	)
	
	set(LIB_SOURCES 
		# ${LIB_SOURCE_DIR}/nfd_cocoa.m
		# ${LIB_SOURCE_DIR}/nfd_gtk.cpp
		# ${LIB_SOURCE_DIR}/nfd_portal.cpp
		${LIB_SOURCE_DIR}/nfd_win.cpp
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
	
    # target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_IMPLEMENTATION=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_ENABLE_FREETYPE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC IMGUI_ENABLE_FREETYPE_LUNASVG=1)
    
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

    target_link_libraries(${LIB_NAME} PUBLIC de_glfw)
    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)

endif()
