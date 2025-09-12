set(LIB_NAME de_bespoke_catch) # cmake target name
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	# file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/libxml/*.h")
	# file(GLOB_RECURSE LIB_SOURCES "${LIB_INCLUDE_DIR}/libxml/*.cpp")

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/catch/catch.hpp)

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBCATCH=1)
  
endif()
