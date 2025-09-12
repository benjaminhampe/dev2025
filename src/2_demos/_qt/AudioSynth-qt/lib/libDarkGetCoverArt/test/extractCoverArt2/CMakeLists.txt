set(APP_NAME extractCoverArt-id3v2)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   message("${BoldYellow}[benni] APPLICATION: ${APP_NAME}${ColourReset}")
   enable_language(RC)

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)

   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

   # target_compile_definitions(${APP_NAME} PRIVATE -DUNICODE=1)
   # target_compile_definitions(${APP_NAME} PRIVATE -D_UNICODE=1)
#   target_compile_options(${APP_NAME} PRIVATE -DWIN32=1)
   # target_compile_options(${APP_NAME} PRIVATE -municode)
   #target_link_libraries(${APP_NAME} PRIVATE DarkImage_as)  # Logger, ImageIO
   target_link_libraries(${APP_NAME} PRIVATE de_id3v2)   # 

endif()
