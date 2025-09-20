set(APP_NAME test_rtaudio)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(APP_NAME "${APP_NAME}_d") # hint exe filename as debug build.
endif()

if( BUILD_SHARED EQUAL 0 )
  set(APP_NAME "${APP_NAME}_s") # hint exe filename as static build.
endif()

project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${Green}[benni] APPLICATION: ${APP_NAME}${ColourReset}")

   enable_language(RC)

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
   # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   set(APP_SOURCES
      ${APP_SOURCE_DIR}/test_rtaudio.cpp
      ${APP_SOURCE_DIR}/icon.rc
   )
   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)
endif()
