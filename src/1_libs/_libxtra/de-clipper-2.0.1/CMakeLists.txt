set(LIB_NAME de_clipper_201)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/clipper2/clipper.h
        ${LIB_HEADER_DIR}/clipper2/clipper.version.h
        ${LIB_HEADER_DIR}/clipper2/clipper.core.h
        ${LIB_HEADER_DIR}/clipper2/clipper.engine.h
        ${LIB_HEADER_DIR}/clipper2/clipper.export.h
        ${LIB_HEADER_DIR}/clipper2/clipper.minkowski.h
        ${LIB_HEADER_DIR}/clipper2/clipper.offset.h
        ${LIB_HEADER_DIR}/clipper2/clipper.rectclip.h
        ${LIB_HEADER_DIR}/clipper2/clipper.triangulation.h        
    )
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/clipper.engine.cpp
        ${LIB_SOURCE_DIR}/clipper.offset.cpp
        ${LIB_SOURCE_DIR}/clipper.rectclip.cpp
        ${LIB_SOURCE_DIR}/clipper.triangulation.cpp        
    )

	# if (BUILD_SHARED_LIBS)
		# message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	# endif()
	
	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    
	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
	
    # target_compile_definitions(${LIB_NAME} PUBLIC USINGZ=0)
    target_compile_definitions(${LIB_NAME} PUBLIC CLIPPER2_HI_PRECISION=0)
    target_compile_definitions(${LIB_NAME} PUBLIC CLIPPER2_MAX_DECIMAL_PRECISION=8)
    # target_compile_definitions(${LIB_NAME} PUBLIC CLIPPER2_HI_PRECISION=1)

    if (MSVC)
        target_compile_options(${LIB_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    
	# file(COPY ${LIB_HEADERS} DESTINATION ${CLIPPER_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)