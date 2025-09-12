set(LIB_NAME BasicOsc_vst2)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(de_juce_audio_plugin_client_vst2)

    # set(JUCE_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/modules)
    # set(JUCE_HEADERS
    # ${JUCE_HEADER_DIR}/juce_core/juce_core.h
    # )

    # set(JUCE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode/modules)
    # set(JUCE_SOURCES
    # ${JUCE_SOURCE_DIR}/juce_core/juce_core.cpp
    # )

    # set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/JuceLibraryCode)
    # set(APP_HEADERS
    # ${APP_HEADER_DIR}/AppConfig.h
    # ${APP_HEADER_DIR}/JuceHeader.h
    # ${APP_HEADER_DIR}/JuceSources.cpp
    # )

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE APP_SOURCES_C "${APP_SOURCE_DIR}/*.c")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_C} ${APP_SOURCES_CPP} ${APP_SOURCES_HPP})

    #   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
    #   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
    #   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

    #   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
    #   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
    #   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    #set(CMAKE_AUTOMOC ON)
    #set(CMAKE_AUTORCC ON)
    #set(CMAKE_AUTOUIC ON)
    #find_package(Qt6Core)
    #find_package(Qt6Widgets)
    #set(RESOURCE_FILES "res/resource.qrc")
    #qt5_add_resources(APP_SOURCES ${RESOURCE_FILES})

    #set(CMAKE_AUTOUIC ON)
    # add_definitions(${Qt6Widgets_DEFINITIONS})
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
    #QT5_WRAP_CPP( APP_MOC_FILES_CPP "mainwindow.hpp" )
    #QT5_WRAP_UI( APP_UI_FILES "ui/mainwindow.ui" )

    #message("${BoldGreen}[benni] APP: ${LIB_NAME}${ColourReset}")
    #add_executable(${LIB_NAME} "")

    # if (BUILD_SHARED)
    add_library(${LIB_NAME} SHARED "")
    message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=1)
    # else()
    # add_library(${LIB_NAME} STATIC "")
    # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=0)
    # endif()

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=0)

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

    target_sources(${LIB_NAME} PRIVATE ${APP_SOURCES})

    # target_include_directories(${LIB_NAME} PRIVATE ${JUCE_HEADER_DIR})

    target_include_directories(${LIB_NAME} PUBLIC ${APP_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${APP_SOURCE_DIR}/audio)
    target_include_directories(${LIB_NAME} PUBLIC ${APP_SOURCE_DIR}/gui)
    target_include_directories(${LIB_NAME} PUBLIC ${APP_SOURCE_DIR}/gui/panels)

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-multichar)
    endif()
    
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_cryptography)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_video)
    target_link_libraries(${LIB_NAME} PRIVATE de_BasicOsc_vst2)

    # COPY MEDIA FILES relative to exe
    # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
    # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # copy theme resource files
    #ADD_CUSTOM_COMMAND( TARGET "${LIB_NAME}"
    #   POST_BUILD
    #   COMMAND "echo" "Copying theme.."
    #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
    #   )
endif()
