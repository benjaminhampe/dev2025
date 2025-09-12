set(APP_NAME SoundStretch)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[c,h]pp")

    if (WIN32)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)
    endif()
    
    message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_link_libraries(${APP_NAME} PUBLIC de_soundtouch)

    if (WIN32)
        target_link_libraries(${APP_NAME} PUBLIC winmm)
    endif()
    
    if (MSVC)
        set_target_properties(${APP_NAME} PROPERTIES FOLDER _libaudio)
        set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
	else()
        target_compile_options(${APP_NAME} PUBLIC -Wno-deprecated-declarations)
        target_link_libraries(${APP_NAME} PUBLIC m)
    endif()

endif()
