set(APP_NAME demo_freetype2_OutlinePrinter)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
    set(APP_SOURCES ${APP_SOURCE_DIR}/main_OutlinePrinter.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.rc)
    endif()
    
    message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
    
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})    

    target_link_libraries(${APP_NAME} PRIVATE de_freetype2)

endif()
