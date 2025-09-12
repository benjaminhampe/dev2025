set(APP_NAME Audi_Q7)
project(${APP_NAME}-dev CXX)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    # ################# Headers ################# #
    set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_HEADERS "${APP_HEADER_DIR}/*.h")

    # ################# Sources ################# #
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.cpp")
    if (WIN32)
    enable_language(RC)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # ################# Executable ################# #
    message("${BoldYellow}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")
    target_sources(${APP_NAME} PRIVATE ${APP_HEADERS} ${APP_SOURCES})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${APP_NAME} PUBLIC -Wno-unused-function)
        target_compile_options(${APP_NAME} PUBLIC -Wno-unused-variable)
        target_compile_options(${APP_NAME} PUBLIC -Wno-unused-but-set-variable)
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
        # target_compile_options(${APP_NAME} PUBLIC -Wl,--demangle)
        # target_link_options(${APP_NAME} PUBLIC -Wl,--demangle)
    endif()

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE de_irrlicht)
    target_link_libraries(${APP_NAME} PRIVATE de_irrExt)
    file(COPY media/Audi_Q7 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
