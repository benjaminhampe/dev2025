set(APP_NAME off2obj)
project(${APP_NAME}-dev)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(APP_NAME "${APP_NAME}d")
endif()

if( BUILD_SHARED EQUAL 0 )
  set(APP_NAME "${APP_NAME}s") # static suffix
endif()

if (TARGET ${APP_NAME})
   message("${BoldBlue}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${BoldBlue}[benni] APPLICATION: ${APP_NAME}${ColourReset}")
   
   enable_language(RC)

   add_executable(${APP_NAME} off2obj.cpp icon.rc)
   
   target_link_libraries(${APP_NAME} PRIVATE m)
   # target_link_libraries(${APP_NAME} PRIVATE de_cork)

   # copy media:
   # file(GLOB_RECURSE APP_MEDIA "${CMAKE_CURRENT_LIST_DIR}/media/*.off")
   # file(COPY ${APP_MEDIA} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/cork)

endif()
