set(APP_NAME Synister)
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

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    # if (BUILD_SHARED)
    #   add_library(${APP_NAME} SHARED "")
    #   message("${BoldBlue}[benni] DLL: ${APP_NAME}${ColourReset}")
    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=1)
    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=1)
    # else()
    # add_library(${APP_NAME} STATIC "")
    # message("${BoldCyan}[benni] LIB: ${APP_NAME}${ColourReset}")
    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=0)
    # endif()

    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=0)

    # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    # target_include_directories(${APP_NAME} PRIVATE ${JUCE_HEADER_DIR})

    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/audio)
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/gui)
    target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/gui/panels)

    # target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/drivers)
    # target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/vendor)

    # target_compile_definitions(${APP_NAME} PUBLIC -DJucePlugin_Name="juce_vst2_plugin_001")

    # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

    # target_compile_definitions(${APP_NAME} PRIVATE -DHAVE_CONFIG_H=1)

    if (MSVC)
    
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
        target_compile_options(${APP_NAME} PUBLIC -Wno-multichar)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_cryptography)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_video)
    target_link_libraries(${APP_NAME} PRIVATE de_Synister_app)

    # COPY MEDIA FILES relative to exe
    # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
    # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # copy theme resource files
    #ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}"
    #   POST_BUILD
    #   COMMAND "echo" "Copying theme.."
    #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
    #   )
endif()
