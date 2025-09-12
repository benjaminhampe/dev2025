set(LIB_NAME de_utf8cpp) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(UTF8CPP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/utf8cpp)
	set(UTF8CPP_INCLUDE_DIR ${UTF8CPP_SOURCE_DIR}/..)
	set(UTF8CPP_HEADERS
		${UTF8CPP_SOURCE_DIR}/core.h
		${UTF8CPP_SOURCE_DIR}/checked.h
		${UTF8CPP_SOURCE_DIR}/unchecked.h
		${UTF8CPP_SOURCE_DIR}/utf8.h
    )

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} INTERFACE)
    
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_UTF8CPP=1) # Now we can check in C++ code if library is available
	target_include_directories(${LIB_NAME} INTERFACE ${UTF8CPP_INCLUDE_DIR})

	# file(COPY ${UTF8CPP_HEADERS} DESTINATION ${UTF8CPP_INCLUDE_DIR}) # For the SDK
endif()
