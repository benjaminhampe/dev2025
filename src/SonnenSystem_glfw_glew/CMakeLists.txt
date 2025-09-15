set(APP_NAME SonnenSystem_glfw_glew)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE APP_HEADER_H ${APP_HEADER_DIR}/*.h)
    file(GLOB_RECURSE APP_HEADER_HPP ${APP_HEADER_DIR}/*.hpp)
    set(APP_HEADERS ${APP_HEADER_H} ${APP_HEADER_HPP})
	
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H ${APP_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE APP_SOURCES_CPP ${APP_SOURCE_DIR}/*.cpp)
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
          # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_HEADER_DIR})
	
    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()

    target_link_libraries(${APP_NAME} PRIVATE de_glm)
    target_link_libraries(${APP_NAME} PRIVATE de_glew)
    target_link_libraries(${APP_NAME} PRIVATE de_glfw_glew)
    target_link_libraries(${APP_NAME} PRIVATE de_imgui)
    target_link_libraries(${APP_NAME} PRIVATE DarkImage_2504)
    # target_link_libraries(${APP_NAME} PRIVATE rayTracingCore)
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # # message(STATUS "Building in Release mode, strip symbols")
        # # --strip-debug : Removes only debugging symbols.
        # # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
            # TARGET ${APP_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )

    # endif()

    # file(COPY media/Raytracing DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
