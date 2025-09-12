set(APP_NAME RtAudioConfigDialog-wx)
project(${APP_NAME}-dev)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(APP_NAME "${APP_NAME}d")
endif()

if(NOT BUILD_SHARED EQUAL 1)
  set(APP_NAME "${APP_NAME}s") # static suffix
endif()

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[c,h]pp")
   file(GLOB_RECURSE APP_SOURCES_XPM "${APP_SOURCE_DIR}/*.xpm")
   list(APPEND APP_SOURCES ${APP_SOURCES_XPM})
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)

   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   # target_link_libraries(${APP_NAME} PUBLIC DarkImage)
   target_link_libraries(${APP_NAME} PUBLIC de_rtaudio)
   target_link_libraries(${APP_NAME} PUBLIC de_tinyxml2)
   target_link_libraries(${APP_NAME} PUBLIC de_wx)
   target_link_libraries(${APP_NAME} PUBLIC de_fontawesome)
   # target_link_libraries(${APP_NAME} PUBLIC de_wx_canvas3d)
   # target_link_libraries(${APP_NAME} PUBLIC de_dvdcss de_dvdnav de_dvdread)

endif()
