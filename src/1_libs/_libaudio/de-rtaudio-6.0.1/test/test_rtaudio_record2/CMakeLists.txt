set(APP_NAME test_rtaudio_record2)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
	# add_subdirectory(lib)

	set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	file(GLOB_RECURSE APP_HEADERS "${APP_HEADER_DIR}/*.h")

	set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.cpp")

	if (WIN32)
		enable_language(RC)
		list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
		list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
	endif()

	message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
	add_executable(${APP_NAME})
	target_sources(${APP_NAME} PUBLIC ${APP_HEADERS} PRIVATE ${APP_SOURCES})

	target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR})
	
	if (MSVC)
		set_target_properties(${APP_NAME} PROPERTIES FOLDER _libaudio)
	else()
		# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")	

		if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
			target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
		endif()

		target_link_libraries(${APP_NAME} PUBLIC pthread)    # Threads
		target_link_libraries(${APP_NAME} PUBLIC m)          # Math		
	endif()


	if (WIN32)  # on Windows:
		target_link_libraries(${APP_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
	else()      # on Linux:
		target_link_libraries(${APP_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
	endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
	target_link_libraries(${APP_NAME} PUBLIC de_rtaudio)     # Audio
	
	# COPY MEDIA FILES relative to exe
	# file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
	# DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

	#set(CMAKE_VERBOSE_MAKEFILE ${CMAKE_VERBOSE_MAKEFILE_OLD}) # Restore old state
endif()
