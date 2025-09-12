set(APP_NAME brotli)

project(${APP_NAME}-1.1.1-rc0)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    # set(LIB_HEADERS ${LIB_HEADER_DIR}/brotli/config.h )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/brotli.c )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_sources(${APP_NAME} PRIVATE ${LIB_SOURCES})

    # target_compile_definitions(${APP_NAME} PUBLIC HAVE_BROTLICOMMON=1)
	
    # target_compile_options(${APP_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
		    
	target_link_libraries(${APP_NAME} PRIVATE de_brotlicommon)
	target_link_libraries(${APP_NAME} PRIVATE de_brotlidec)
	target_link_libraries(${APP_NAME} PRIVATE de_brotlienc)

endif()

# add_subdirectory(test)
