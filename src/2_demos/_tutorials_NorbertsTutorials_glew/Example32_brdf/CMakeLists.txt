set(APP_NAME tutorial_NorbertsOpenGL_32_brdf)
project(${APP_NAME}-dev)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#  set(APP_NAME "${APP_NAME}d")
#endif()

#if(NOT BUILD_SHARED EQUAL 1)
#  set(APP_NAME "${APP_NAME}s") # static suffix
#endif()

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   # add_subdirectory(lib)

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
   target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
   #set_target_properties(${APP_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
   #target_link_libraries(${APP_NAME} PRIVATE DarkImage)

   # target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon

   target_link_libraries(${APP_NAME} PUBLIC de_glus)

   # COPY MEDIA FILES to
   #file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
   #      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Irrlicht_fpscube )

endif()
