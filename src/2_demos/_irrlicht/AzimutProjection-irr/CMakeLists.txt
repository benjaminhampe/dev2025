set(APP_NAME AzimutProjection)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldBlue}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/C3DFileSystem.h
        ${APP_SOURCE_DIR}/C3DFileSystem.cpp
        ${APP_SOURCE_DIR}/main.cpp
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/icon.rc)
    endif()

    add_executable(${APP_NAME} "")
   
    target_sources(${APP_NAME} PUBLIC ${APP_SOURCES})

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE de_irrlicht)
endif()
