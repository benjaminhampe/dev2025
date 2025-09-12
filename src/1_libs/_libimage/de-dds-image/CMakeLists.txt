set(LIB_NAME de_dds_image)

project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	set(LIB_HEADERS 
		"${LIB_INCLUDE_DIR}/dds_image/dds.hpp"
		"${LIB_INCLUDE_DIR}/dds_image/dds_formats.hpp"
	)

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBDDSIMAGE=1)
	target_compile_definitions(${LIB_NAME} INTERFACE DDS_CPP_17=1)
    target_compile_definitions(${LIB_NAME} INTERFACE DDS_USE_STD_FILESYSTEM=1)
    
endif()
