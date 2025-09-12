set(LIB_NAME de_poly2tri)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(P2TRI_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/poly2tri)
	set(P2TRI_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

	set(P2TRI_HEADERS 
		${P2TRI_HEADER_DIR}/poly2tri.h
		${P2TRI_HEADER_DIR}/utils.h
		${P2TRI_HEADER_DIR}/shapes.h
		${P2TRI_HEADER_DIR}/advancing_front.h
		${P2TRI_HEADER_DIR}/cdt.h
		${P2TRI_HEADER_DIR}/sweep.h
		${P2TRI_HEADER_DIR}/sweep_context.h)
		
	set(P2TRI_SOURCES
		${P2TRI_SOURCE_DIR}/shapes.cc
		${P2TRI_SOURCE_DIR}/advancing_front.cc
		${P2TRI_SOURCE_DIR}/cdt.cc
		${P2TRI_SOURCE_DIR}/sweep.cc
		${P2TRI_SOURCE_DIR}/sweep_context.cc)

	# if (BUILD_SHARED)
		# message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	# endif()
	
	target_sources(${LIB_NAME} PUBLIC ${P2TRI_HEADERS} PRIVATE ${P2TRI_SOURCES})	
	
	target_include_directories(${LIB_NAME} PUBLIC ${P2TRI_HEADER_DIR}/..) # For users

	# file(COPY ${P2TRI_HEADERS} DESTINATION ${P2TRI_INCLUDE_DIR}) # For the SDK

endif()
