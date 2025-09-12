set(LIB_NAME de_iconv_charset)

project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	# if (WIN32)
	# enable_language(RC)
	# # list(APPEND LIB_SOURCES 
	# # ${LIB_SOURCE_DIR}/common/win32thread.c
	# # ${LIB_SOURCE_DIR}/common/win32thread.h )
	# endif ()

	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/libcharset/export.h
		${LIB_HEADER_DIR}/libcharset/libcharset.h
		${LIB_HEADER_DIR}/libcharset/localcharset.h
	)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/config.h
		${LIB_SOURCE_DIR}/localcharset.c
		# ${LIB_SOURCE_DIR}/relocatable-stub.c
	)

	# if (BUILD_SHARED)
		# message("${BoldCyan}SHARED_LIBRARY: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
	message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} STATIC "")
	# endif()

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBCHARSET=1)
	target_compile_definitions(${LIB_NAME} PUBLIC BUILDING_LIBCHARSET=1)
	target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
		
	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libcharset)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	# target_link_libraries(${LIB_NAME} PUBLIC m)

endif()
