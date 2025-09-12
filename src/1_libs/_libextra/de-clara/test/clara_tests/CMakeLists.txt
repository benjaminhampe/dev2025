set(APP_NAME clara_tests)
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
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PUBLIC ${APP_HEADERS} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PUBLIC ${APP_HEADER_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if(MSVC)
    else()
        # target_compile_options(${APP_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-maybe-uninitialized)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    # file(COPY ${APP_HEADERS} DESTINATION ${APP_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
