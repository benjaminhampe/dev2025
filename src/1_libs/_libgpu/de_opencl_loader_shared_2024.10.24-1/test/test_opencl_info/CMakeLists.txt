set(APP_NAME test_opencl_info)
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

	target_link_libraries(${APP_NAME} PRIVATE de_opencl_loader_shared)
endif()
