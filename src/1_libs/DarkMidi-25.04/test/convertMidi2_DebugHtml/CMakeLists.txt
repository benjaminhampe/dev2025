set(APP_NAME convertMidi2_DebugHtml)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES 
        ${APP_SOURCE_DIR}/main_convertMidi2_DebugHtml.cpp
        ${APP_SOURCE_DIR}/DebugHtml.hpp
    )
    
    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE "${APP_SOURCE_DIR}")

    if (MSVC)
    else()
        target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)

        # target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
    endif()
	
    target_link_libraries(${APP_NAME} PRIVATE DarkMidi_2504)    
    
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)    
endif()
