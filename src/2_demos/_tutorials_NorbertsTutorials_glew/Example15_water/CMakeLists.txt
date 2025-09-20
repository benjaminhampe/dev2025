set(APP_NAME tutorial_NorbertsOpenGL_15_water)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.c")

   if (WIN32)
      enable_language(RC)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
   endif()

   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} "")
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
   endif()


   target_link_libraries(${APP_NAME} PUBLIC de_glus)

   # COPY MEDIA FILES to
   #file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
   #      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Irrlicht_fpscube )

endif()
