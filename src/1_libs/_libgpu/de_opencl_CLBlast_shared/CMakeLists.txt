set(LIB_NAME de_opencl_CLBlast_shared)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
    
	add_library(${LIB_NAME} SHARED IMPORTED GLOBAL) # INTERFACE
    
	set_target_properties(${LIB_NAME} PROPERTIES
		IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/libclblast.dll.a"
		IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/libclblast.dll"
	    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
	)

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_CLBLAST=1)
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_OPENCL_BLAST=1)
    # target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

	file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/libclblast.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
