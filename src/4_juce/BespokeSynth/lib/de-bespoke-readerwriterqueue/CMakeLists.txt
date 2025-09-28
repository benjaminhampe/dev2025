set(LIB_NAME de_bespoke_readerwriterqueue)

project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
	
	add_library(${LIB_NAME} INTERFACE)

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBREADERWRITERQUEUE=1)
  
endif()
