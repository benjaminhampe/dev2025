set(APP_NAME Qt6IrrlichtWidget2019)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[c,h]pp")
   
   list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc")
   if (WIN32)
      enable_language(RC)
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
   endif()

# # # # [QT] BEGIN
   set(CMAKE_AUTOMOC ON)
   set(CMAKE_AUTORCC ON)
   #set(CMAKE_AUTOUIC ON)
   find_package(Qt6Widgets)
   find_package(Qt6Svg)
   find_package(Qt6 COMPONENTS Core Widgets)
   include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
   # add_definitions(${Qt6Widgets_DEFINITIONS})
   # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
   #QT5_WRAP_CPP( APP_MOC_FILES_CPP "mainwindow.hpp" )
   #QT5_WRAP_UI( APP_UI_FILES "ui/mainwindow.ui" )
   #set(RESOURCE_FILES "res/resource.qrc")
   #qt6_add_resources(APP_SOURCES ${RESOURCE_FILES})
# # # # [QT] END

   message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} "")
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   
   if (MSVC)
   else()
	target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
	target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
   endif()
   
   target_link_libraries( ${APP_NAME} PRIVATE "Qt6::Widgets")
   target_link_libraries(${APP_NAME} PRIVATE de_irrlicht)

   # COPY MEDIA FILES relative to exe
   file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png 
         DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

endif()
