set(LIB_NAME de_nlohmann_json) # cmake target name
project(${LIB_NAME}-3.12.0)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	set(LIB_HEADERS 
		"${LIB_INCLUDE_DIR}/nlohmann/json.hpp"
		"${LIB_INCLUDE_DIR}/nlohmann/json_fwd.hpp"
	)

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

	target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBLOHMANNJSON=1)
  
endif()
