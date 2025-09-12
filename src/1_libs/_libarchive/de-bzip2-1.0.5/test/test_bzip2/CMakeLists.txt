set(APP_NAME test_bzip2)

project(${APP_NAME}-dev)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # set(APP_NAME "${APP_NAME}d")
# endif()

# if( BUILD_SHARED EQUAL 1 )
# else()
  # set(APP_NAME "${APP_NAME}s") # static suffix
# endif()

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${Green}[benni] APPLICATION: ${APP_NAME}${ColourReset}")
   enable_language(RC)
   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
   set(APP_SOURCES ${APP_SOURCE_DIR}/main_test.cpp)
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.h)
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.rc)
   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_link_libraries(${APP_NAME} PRIVATE de_bzip2)

endif()
