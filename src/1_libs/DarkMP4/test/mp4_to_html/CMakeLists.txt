set(APP_NAME mp4_to_html)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES 
        ${APP_SOURCE_DIR}/main_mp4_to_html.cpp
        ${APP_SOURCE_DIR}/DebugHtml.h
    )
    
    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE "${APP_SOURCE_DIR}")

    # if (MSVC)
    # else()
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)

        # # target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
    # endif()
	
    target_link_libraries(${APP_NAME} PRIVATE DarkMP4)
    
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)    
endif()
