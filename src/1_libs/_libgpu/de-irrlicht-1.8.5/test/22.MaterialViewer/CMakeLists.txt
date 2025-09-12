set(APP_NAME demo_Irrlicht_22_MaterialViewer)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldBlue}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")

   if (WIN32)
      enable_language(RC)
   endif()

   add_executable(${APP_NAME} main.cpp icon.rc)

   target_link_libraries(${APP_NAME} PRIVATE de_irrlicht)
endif()
