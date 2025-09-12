set(APP_NAME sqlite3_shell)
project(${APP_NAME})

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
   set(APP_SOURCES ${APP_SOURCE_DIR}/shell.c)

   if (WIN32)
       enable_language(RC)
       list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.h)
       list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.rc)
   endif()

   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} ${APP_SOURCES})

   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

   target_link_libraries(${APP_NAME} PRIVATE de_sqlite3)

endif()
