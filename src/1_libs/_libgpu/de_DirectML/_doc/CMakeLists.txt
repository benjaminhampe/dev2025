set(LIB_NAME de_DirectML)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
    
	add_library(${LIB_NAME}_A SHARED IMPORTED)
	set_target_properties(${LIB_NAME}_A PROPERTIES
		# IMPORTED_LOCATION "C:/libs/libA.dll"
		IMPORTED_IMPLIB   "${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Guids.a"
	)

	# add_library(de_DirectML_B SHARED IMPORTED)
	# set_target_properties(de_DirectML_B PROPERTIES
		# # IMPORTED_LOCATION "C:/libs/libB.dll"
		# IMPORTED_IMPLIB   "${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Headers.a"
	# )

	add_library(${LIB_NAME} SHARED IMPORTED GLOBAL) # INTERFACE
    
	# set_target_properties(${LIB_NAME} PROPERTIES
		# # IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Guids"
		# # IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/vulkan-1.dll"
	    # INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
	# )

	set_target_properties(${LIB_NAME} PROPERTIES
		# IMPORTED_LOCATION  "${CMAKE_SOURCE_DIR}/extern/C.dll"
		IMPORTED_IMPLIB    "${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Headers.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
		INTERFACE_LINK_LIBRARIES "${LIB_NAME}_A"
	)

	# target_link_libraries(${LIB_NAME} 
	# target_compile_definitions(${LIB_NAME} INTERFACE HAVE_OPENCL=1)
    # target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

	# file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/vulkan-1.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

add_subdirectory(test)