set(APP_NAME off2obj)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   
   enable_language(RC)

   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} off2obj.cpp icon.rc)
   
   target_link_libraries(${APP_NAME} PRIVATE m)
   # target_link_libraries(${APP_NAME} PRIVATE de_cork)

   # copy media:
   # file(GLOB_RECURSE APP_MEDIA "${CMAKE_CURRENT_LIST_DIR}/media/*.off")
   # file(COPY ${APP_MEDIA} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/cork)

endif()
