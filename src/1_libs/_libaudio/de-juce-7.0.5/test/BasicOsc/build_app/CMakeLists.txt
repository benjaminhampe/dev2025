set(APP_NAME BasicOsc)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE APP_SOURCES_C "${APP_SOURCE_DIR}/*.c")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_C} ${APP_SOURCES_CPP} ${APP_SOURCES_HPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()


    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/audio)
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/gui)
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/gui/panels)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
	
    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
        target_compile_options(${APP_NAME} PUBLIC -Wno-multichar)
    endif()
    
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_cryptography)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_video)
    target_link_libraries(${APP_NAME} PRIVATE de_BasicOsc_app)

    # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
         # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

endif()
