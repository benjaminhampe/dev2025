set(APP_NAME wxIrrlichtControl2021)
project(${APP_NAME}-dev)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # set(APP_NAME "${APP_NAME}d")
# endif()

# if(NOT BUILD_SHARED EQUAL 1)
  # set(APP_NAME "${APP_NAME}s") # static suffix
# endif()

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   #file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[c,h]pp")
   set(APP_SOURCES
         ${APP_SOURCE_DIR}/wxIrrlichtControl2021.hpp
         ${APP_SOURCE_DIR}/wxIrrlichtControl2021.cpp
         ${APP_SOURCE_DIR}/Window.hpp
         ${APP_SOURCE_DIR}/Window.cpp
         ${APP_SOURCE_DIR}/App.hpp
         ${APP_SOURCE_DIR}/App.cpp
         ${APP_SOURCE_DIR}/main.cpp)

   if (WIN32)
      enable_language(RC)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
   endif()

   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")

   add_executable(${APP_NAME} "")
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

   target_compile_options(${APP_NAME} PUBLIC -Wno-deprecated-declarations)
   target_link_libraries(${APP_NAME} PUBLIC de_wx)
   target_link_libraries(${APP_NAME} PUBLIC de_irrlicht)

   # media/tex.png -> bin/media/wxIrrlicht/tex.png
   file(COPY "${CMAKE_CURRENT_LIST_DIR}/media/tex.png" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/wxIrrlicht) #  dll -> bin

endif()
