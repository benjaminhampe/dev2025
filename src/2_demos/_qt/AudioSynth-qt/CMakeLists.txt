set(APP_NAME AudioSynth-qt)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

   # add_subdirectory(lib)

# Begin QtSDK5
   set(CMAKE_AUTOMOC ON)
   set(CMAKE_AUTORCC ON)
   #set(CMAKE_VERBOSE_MAKEFILE_OLD ${CMAKE_VERBOSE_MAKEFILE}) # save old state
   #set(CMAKE_VERBOSE_MAKEFILE ON)   # set new state
   # set(CMAKE_AUTOUIC OFF)

   # find_package(Qt6Widgets)
   # find_package(Qt6OpenGL)
   # find_package(Qt6 COMPONENTS Core Widgets OpenGL)
   # include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
   # add_definitions(${Qt6Widgets_DEFINITIONS})
   # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

   find_package(Qt6Widgets)
   find_package(Qt6OpenGL)
   find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets)
   include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
   add_definitions(${Qt6Widgets_DEFINITIONS})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
   # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   # file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   # set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp
   # set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp ${APP_SOURCE_DIR}/main.cpp)
   # QT5_WRAP_CPP( MOC_FILES ${APP_SOURCE_DIR}/MainWindow.cpp ${APP_SOURCE_DIR}/main.cpp)
   # list(APPEND APP_SOURCES ${MOC_FILES})
   # QT5_WRAP_UI( APP_UIF "ui/mainwindow.ui" )
   set(RESOURCE_FILES "res/resource.qrc")
   # list(APPEND APP_SOURCES ${APP_UIF} )
   # copy theme resource files
   # ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}" POST_BUILD COMMAND "echo" "Copying theme.." COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes")
# Ende QtSDK

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_HPP} ${APP_SOURCES_CPP} ${RESOURCE_FILES})

    if (WIN32)
        # enable_language(RC)
        #list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc")
        list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
        list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/widgets)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/groups)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/engine)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/sequencer)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/filePlayer)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/midi)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/mixer)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/master)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/detuneSynth)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/betterSynth)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/decks/vsthost)

	if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)        
        # target_compile_options(${APP_NAME} PRIVATE -Wno-macro-redefined)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-inconsistent-missing-override)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-variable)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-sign-compare)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-function)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-comment)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)        
    endif()

    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    # if (WIN32)  # on Windows:
      # target_link_libraries(${APP_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
    # else()      # on Linux:
      # target_link_libraries(${APP_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
    # endif()

    target_link_libraries(${APP_NAME} PUBLIC Qt6::Widgets)
    target_link_libraries(${APP_NAME} PUBLIC Qt6::OpenGL)
    target_link_libraries(${APP_NAME} PUBLIC Qt6::OpenGLWidgets)

    # target_link_libraries(${APP_NAME} PUBLIC de_gles3)
    # target_link_libraries(${APP_NAME} PUBLIC de_rtaudio)     # Audio
    target_link_libraries(${APP_NAME} PUBLIC de_rtaudio_601)    
    target_link_libraries(${APP_NAME} PUBLIC de_rtmidi)
    target_link_libraries(${APP_NAME} PUBLIC de_fftw3)
    target_link_libraries(${APP_NAME} PUBLIC de_kissfft)
    target_link_libraries(${APP_NAME} PUBLIC de_kissfft_cpp)
    target_link_libraries(${APP_NAME} PUBLIC de_soundtouch)
    target_link_libraries(${APP_NAME} PUBLIC de_lame)
    target_link_libraries(${APP_NAME} PUBLIC de_vst2sdk)
    target_link_libraries(${APP_NAME} PUBLIC de_ffmpeg)
    target_link_libraries(${APP_NAME} PUBLIC DarkImage_2504)
    target_link_libraries(${APP_NAME} PUBLIC DarkCoverArt_2504)

    # include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    
endif()
