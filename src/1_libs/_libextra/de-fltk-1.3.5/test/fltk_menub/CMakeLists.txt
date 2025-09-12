set(APP_NAME fltk_menub)
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
   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
   # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   set(APP_SOURCES ${APP_SOURCE_DIR}/fltk_menub.cpp)

	# list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)
   if (WIN32)
      # enable_language(RC)
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
   endif()
       
   message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} "")
   
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   
   target_link_libraries(${APP_NAME} PRIVATE de_fltk)
   
   if(MSVC)
   target_link_options(${APP_NAME} PUBLIC "/MANIFEST:NO")
   endif()

   if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES FOLDER _benni)
    endif()
endif()
