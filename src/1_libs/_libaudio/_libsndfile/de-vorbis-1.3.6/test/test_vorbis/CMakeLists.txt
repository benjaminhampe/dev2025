set(APP_NAME test_vorbis)
project(${APP_NAME})

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

	set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(APP_SOURCES ${APP_SOURCE_DIR}/main_test_vorbis.cpp )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
        list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
    endif()

    message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME})
    target_sources(${APP_NAME} PUBLIC ${APP_HEADERS} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR})

    # target_link_options(${APP_NAME} PRIVATE -mwindows) # hide console window

	if (MSVC)
		set_target_properties(${APP_NAME} PROPERTIES FOLDER _libaudio)
	else() 
	endif()
   
    target_link_libraries(${APP_NAME} PUBLIC de_vorbis)
    target_link_libraries(${APP_NAME} PUBLIC de_ogg)
    
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

endif()
