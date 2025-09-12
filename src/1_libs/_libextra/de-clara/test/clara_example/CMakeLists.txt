set(APP_NAME clara_example)
project(${APP_NAME} CXX)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(APP_SOURCES ${APP_SOURCE_DIR}/main_clara_example.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PUBLIC ${APP_HEADER_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    
    if (MSVC)
    else()
        # target_compile_options(${APP_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_link_options(${APP_NAME} PRIVATE -mwindows)        
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)    
    target_link_libraries(${APP_NAME} PRIVATE de_clara)
    
    # file(COPY ${APP_HEADERS} DESTINATION ${APP_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
