set(LIB_NAME de_DirectML)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
    
	set(LIB_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/de_DirectML.h")
	set(LIB_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/de_DirectML.c")

	add_library(${LIB_NAME} STATIC "")

	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
	
	target_link_libraries(${LIB_NAME} PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Guids.a"
		"${CMAKE_CURRENT_LIST_DIR}/lib/libDirectX-Headers.a"
	)
	
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DIRECTML=1)

	target_compile_definitions(${LIB_NAME} PUBLIC WINVER=0x0A00)
    target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0A00)
    target_compile_definitions(${LIB_NAME} PUBLIC NTDDI_VERSION=0x0A000000)
    
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/DirectML")
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/DirectML/composition")
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/DirectML/directx")
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/DirectML/dxguids")
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/DirectML/wsl")

	# file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/vulkan-1.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

add_subdirectory(test)