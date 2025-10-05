set(LIB_NAME de_pcgcpp) # cmake target name
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/pcg_extras.hpp
        ${LIB_HEADER_DIR}/pcg_random.hpp
        ${LIB_HEADER_DIR}/pcg_uint128.hpp
    )

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBPCGCPP=1)
  
endif()
