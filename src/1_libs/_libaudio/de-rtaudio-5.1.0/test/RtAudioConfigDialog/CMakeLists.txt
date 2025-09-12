set(APP_NAME RtAudioConfigDialog)
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

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file( GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
   file( GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
   file( GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
   set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_HPP} ${APP_SOURCES_CPP})
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)

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
   target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)     # Audio
   target_link_libraries(${APP_NAME} PRIVATE de_tinyxml2)     # XML
   target_link_libraries(${APP_NAME} PRIVATE DarkImage)
endif()
