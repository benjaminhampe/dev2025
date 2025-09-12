set(LIB_NAME de_clipper) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(CLIPPER_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(CLIPPER_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(CLIPPER_HEADERS ${CLIPPER_HEADER_DIR}/ClipperLib/clipper.hpp)
	set(CLIPPER_SOURCES ${CLIPPER_SOURCE_DIR}/clipper.cpp)

	# if (BUILD_SHARED)
		# message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	# endif()
	
	target_sources(${LIB_NAME} PUBLIC ${CLIPPER_HEADERS} PRIVATE ${CLIPPER_SOURCES})
	target_include_directories(${LIB_NAME} PUBLIC ${CLIPPER_HEADER_DIR})
	
	# file(COPY ${CLIPPER_HEADERS} DESTINATION ${CLIPPER_INCLUDE_DIR}) # For the SDK
endif()
