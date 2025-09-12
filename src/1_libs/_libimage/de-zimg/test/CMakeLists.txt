set(APP_NAME test_zimg)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[c,h]pp")

    add_executable(${APP_NAME} "")
    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
    
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_link_libraries(${APP_NAME} PRIVATE de_zimg)
endif()
