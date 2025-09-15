set(APP_NAME convertImage)
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
   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
   if (WIN32)
      enable_language(RC)
   endif()

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)

   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

   target_compile_options(${APP_NAME} PRIVATE -Wno-unused-value)
   # target_link_libraries(${APP_NAME} PRIVATE DarkVLC)
   #target_link_libraries(${LIB_NAME} PUBLIC de_glm)         # Math
   #target_link_libraries(${LIB_NAME} PUBLIC de_fontawesome) # Font
   #target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)    # XML
   target_link_libraries(${APP_NAME} PRIVATE DarkImage_2504)     # Logger
   # target_link_libraries(${APP_NAME} PRIVATE DarkAudio)     # Logger
   # target_link_libraries(${APP_NAME} PRIVATE de_ffmpeg)     # AudioDecoder + VideoDecoder
   # target_link_libraries(${APP_NAME} PRIVATE de_lame)       # AudioEncoder MP3

endif()
