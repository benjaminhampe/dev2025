set(APP_NAME RtAudioConfigDialog-qt)
project(${APP_NAME}-dev)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(APP_NAME "${APP_NAME}d")
endif()

if( BUILD_SHARED EQUAL 1 )
else()
  set(APP_NAME "${APP_NAME}s") # static suffix
endif()

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${BoldYellow}[benni] APPLICATION: ${APP_NAME}${ColourReset}")

   #add_subdirectory(lib)

   enable_language(RC)

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file( GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
   file( GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
   file( GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
   set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_HPP} ${APP_SOURCES_CPP})
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)

# Begin QtSDK
   set(CMAKE_AUTOMOC ON)
   # set(CMAKE_AUTORCC OFF)
   # set(CMAKE_AUTOUIC OFF)

   find_package(Qt5Widgets)
   find_package(Qt5 COMPONENTS Core Widgets)

   include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})
   add_definitions(${Qt5Widgets_DEFINITIONS})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


   # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

   # file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

   # set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp


#   set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp
#                   ${APP_SOURCE_DIR}/Canvas3D.cpp
#                   ${APP_SOURCE_DIR}/MainWindow.cpp
#                   ${APP_SOURCE_DIR}/LibreCAD.cpp
#                   ${APP_SOURCE_DIR}/main.cpp )

#   QT5_WRAP_CPP( MOC_FILES
#      ${APP_SOURCE_DIR}/MainWindow.cpp
#      ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp
#      ${APP_SOURCE_DIR}/Canvas3D.cpp
#      ${APP_SOURCE_DIR}/main.cpp )
#   list(APPEND APP_SOURCES ${MOC_FILES})

   #QT5_WRAP_UI( APP_UIF "ui/mainwindow.ui" )
   #set(RESOURCE_FILES "res/icons.qrc")
   #qt5_add_resources(APP_SOURCES ${APP_UIF})
   # list(APPEND APP_SOURCES ${APP_UIF} )

   # copy theme resource files
   #ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}"
   #   POST_BUILD
   #   COMMAND "echo" "Copying theme.."
   #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
   #   )
# Ende QtSDK

   add_executable(${APP_NAME} ${APP_SOURCES})
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   # target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_compile_options(${APP_NAME} PUBLIC -Wno-deprecated-declarations)

   if (BUILD_SHARED)
   else()
      target_compile_options(${APP_NAME} PRIVATE -static)
      target_compile_options(${APP_NAME} PRIVATE -static-libstdc++)
      target_compile_options(${APP_NAME} PRIVATE -static-libgcc)
      target_link_options(${APP_NAME} PRIVATE -static)
      target_link_options(${APP_NAME} PRIVATE -static-libstdc++)
      target_link_options(${APP_NAME} PRIVATE -static-libgcc)
   endif()

   # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

   if (WIN32)  # on Windows:
      target_link_libraries(${APP_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
   else()      # on Linux:
      target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
   endif()
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
        target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
    endif()

   target_link_libraries(${APP_NAME} PRIVATE pthread)    # Threads
   target_link_libraries(${APP_NAME} PRIVATE m)          # Math

   target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets)


   ### DarkImage 2D images, filesystem, xml, glm, fontawesome, drawing, text, bresenham, rainbowColor, etc...
   #target_link_libraries(${APP_NAME} PUBLIC DarkImage)
   #target_link_libraries(${APP_NAME} PUBLIC de_glm)         # Math
   #target_link_libraries(${APP_NAME} PUBLIC de_fontawesome) # Font
   #target_link_libraries(${APP_NAME} PUBLIC de_tinyxml2)    # XML

   ### DarkGPU 3D OpenGLES
   target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)     # Audio
   target_link_libraries(${APP_NAME} PRIVATE de_tinyxml2)
   # target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)      # General Midi
   # target_link_libraries(${APP_NAME} PRIVATE de_fftw3)       # Audio Spektrum fft
   # target_link_libraries(${APP_NAME} PRIVATE de_kissfft)     # Audio Spektrum fft
   # target_link_libraries(${APP_NAME} PRIVATE de_soundtouch)  # Audio
   # target_link_libraries(${APP_NAME} PRIVATE de_sndfile)    # Audio
   # target_link_libraries(${APP_NAME} PRIVATE de_lame)      # Audio mp3 encoder
   # target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)    # Audio Steinberg VST SDK
   # target_link_libraries(${APP_NAME} PRIVATE de_ffmpeg)     # Audio + Video

   # target_link_libraries(${APP_NAME} PRIVATE DarkImage)         # 2D
   # target_link_libraries(${APP_NAME} PRIVATE DarkGPU)           # 3D
   # target_link_libraries(${APP_NAME} PRIVATE DarkAudio)   # Audio
   # target_link_libraries(${APP_NAME} PRIVATE DarkGetCoverArt)   # Audio Tags
endif()
