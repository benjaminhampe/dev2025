set(APP_NAME convertMidi2_ImageHtml)
project(${APP_NAME}-dev)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # set(APP_NAME "${APP_NAME}_debug")
# endif()

# if( BUILD_SHARED EQUAL 1 )
# else()
  # set(APP_NAME "${APP_NAME}_static")
# endif()

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

   if (WIN32)
      enable_language(RC)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
   endif()

   message("${BoldYellow}[benni] APPLICATION: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE "${APP_SOURCE_DIR}")

   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   
   target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)

   target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon

   # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)   # Filesystem c++17
   # target_link_libraries(${APP_NAME} PRIVATE pthread)    # Threads
   # target_link_libraries(${APP_NAME} PRIVATE m)          # Math

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
   target_link_libraries(${APP_NAME} PRIVATE DarkMidi_midi)
   target_link_libraries(${APP_NAME} PRIVATE DarkCommonDialogs_midi)
   target_link_libraries(${APP_NAME} PRIVATE DarkImage_midi)

endif()
