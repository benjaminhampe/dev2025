set(APP_NAME test_DirectML)
project(${APP_NAME})

if (TARGET ${APP_NAME})
	message("${BoldBlue}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
	set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES ${APP_SOURCE_DIR}/main.cpp)

    if (WIN32)
		enable_language(RC)
		list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
		list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
	endif()

	message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
	add_executable(${APP_NAME} ${APP_SOURCES})
	
	# clang++ dml_test.cpp -o dml_test.exe -ld3d12 -ldxgi -ldxguid -ld3d12sdklayers


	# Name: DirectX-Headers
	# Description: Headers for using D3D12
	# URL: https://github.com/microsoft/DirectX-Headers
	# Version: 1.618.2
	# Cflags: -I${includedir}
	# Libs: -lDirectX-Headers -lDirectX-Guids

	# pacman -S mingw-w64-clang-x86_64-directx-headers
	# 
	# d3d12.h
	# dxgi1_6.h
	# directml.h
	# directml_helpers.h
	# d3dx12.h
	
	# target_link_libraries(${APP_NAME} PRIVATE d3d12 dxgi dxguid d3d12sdklayers)
	
	target_link_libraries(${APP_NAME} PRIVATE de_DirectML)
endif()
