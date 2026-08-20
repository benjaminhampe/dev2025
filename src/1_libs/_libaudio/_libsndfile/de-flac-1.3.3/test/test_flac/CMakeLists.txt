set(APP_NAME test_flac)
project(${APP_NAME})

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${BoldYellow}[benni] APPLICATION: ${APP_NAME}${ColourReset}")

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   set(APP_SOURCES ${APP_SOURCE_DIR}/main_test.cpp)

   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_link_libraries(${APP_NAME} PRIVATE de_flac)

endif()
