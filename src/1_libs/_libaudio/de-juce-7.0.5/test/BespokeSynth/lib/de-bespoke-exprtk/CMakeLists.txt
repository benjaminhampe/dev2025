set(LIB_NAME de_bespoke_exprtk) # Math Expression Toolkit

project(${LIB_NAME}
	LANGUAGES CXX
	VERSION 0.0.3
	DESCRIPTION  "C++ Mathematical Expression Toolkit Library (ExprTk)"
	HOMEPAGE_URL "https://www.partow.net/programming/exprtk/index.html"
)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	# file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/libxml/*.h")
	# file(GLOB_RECURSE LIB_SOURCES "${LIB_INCLUDE_DIR}/libxml/*.cpp")

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(
		${LIB_NAME} 
		INTERFACE 
		${LIB_INCLUDE_DIR}/exprtk.hpp
	)

	# target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
		
	target_include_directories(
		${LIB_NAME}
		INTERFACE
		$<BUILD_INTERFACE:${LIB_INCLUDE_DIR}>
		# $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBEXPRTK=1)
  
endif()
