set(APP_NAME test_speech2text)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})

    # set(APP_SOURCES
    #     ${APP_SOURCE_DIR}/main.cpp
    #     ${APP_SOURCE_DIR}/AudioFile.cpp
    #     ${APP_SOURCE_DIR}/AudioFile.h
    # )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
        # set_target_properties(${APP_NAME} PROPERTIES FOLDER _app)
    else()
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-variable)

        # target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon

    endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    target_link_libraries(${APP_NAME} PRIVATE de_whisper_cpp)
    target_link_libraries(${APP_NAME} PRIVATE de_dr_wav)
    target_link_libraries(${APP_NAME} PRIVATE de_dr_mp3)
    target_link_libraries(${APP_NAME} PRIVATE de_dr_flac)
    target_link_libraries(${APP_NAME} PRIVATE de_speexdsp)
	
endif()
