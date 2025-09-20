set(APP_NAME midi-keyboard-GS-qt)
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
   # add_subdirectory(lib)

   set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
   file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
   file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
   file(GLOB_RECURSE APP_SOURCES_C "${APP_SOURCE_DIR}/*.c")
   file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")

   set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_C} ${APP_SOURCES_CPP} ${APP_SOURCES_HPP})


   if (WIN32)
      enable_language(RC)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
      list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
   endif()

# # # # [QT] BEGIN
   set(CMAKE_AUTOMOC ON)
   set(CMAKE_AUTORCC ON)
   #set(CMAKE_AUTOUIC ON)
   find_package(Qt5Core)
   find_package(Qt5Widgets)
   #find_package(Qt5Svg)
   find_package(Qt5 COMPONENTS Core Widgets)
   include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})
   # add_definitions(${Qt5Widgets_DEFINITIONS})
   # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
   #QT5_WRAP_CPP( APP_MOC_FILES_CPP "mainwindow.hpp" )
   #QT5_WRAP_UI( APP_UI_FILES "ui/mainwindow.ui" )
   set(RESOURCE_FILES "res/resource.qrc")
   qt5_add_resources(APP_SOURCES ${RESOURCE_FILES})
# # # # [QT] END

   message("${BoldRed}[benni] APPLICATION: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME} "")
   target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/drivers)
   target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/vendor)

	if (MSVC)
		set_target_properties(${APP_NAME} PROPERTIES FOLDER _libaudio)
	else()
		# target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon

		target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

		# target_compile_definitions(${APP_NAME} PRIVATE -DHAVE_CONFIG_H=1)

		target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
		target_link_libraries(${APP_NAME} PRIVATE pthread)
		target_link_libraries(${APP_NAME} PRIVATE m)
	endif()
	

   target_link_libraries(${APP_NAME} PRIVATE "Qt5::Core")
   target_link_libraries(${APP_NAME} PRIVATE "Qt5::Widgets")

   target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)
   #target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)
   # target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)
   # target_link_libraries(${APP_NAME} PRIVATE de_glm)
   # target_link_libraries(${APP_NAME} PRIVATE de_fftw3)

   # COPY MEDIA FILES relative to exe
   #file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
   #      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

   # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

   # copy theme resource files
   #ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}"
   #   POST_BUILD
   #   COMMAND "echo" "Copying theme.."
   #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
   #   )
endif()

# cmake_minimum_required(VERSION 3.0)

# project(test.irrlicht.qt CXX)

# set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
# set(APP_INCLUDE_DIR ${BENNI_SDK_INCLUDE_DIR}/irrExt)

# set(APP_SOURCES
   # ${APP_SOURCE_DIR}/irrExt/QIrrlichtWidget2019.cpp
   # ${APP_SOURCE_DIR}/irrExt/QIrrlichtWidget2019.hpp
   # ${APP_SOURCE_DIR}/MainWindow.cpp
   # ${APP_SOURCE_DIR}/MainWindow.hpp
   # ${APP_SOURCE_DIR}/main.cpp)

# ### find  package
# set(CMAKE_AUTOMOC ON) # Qt5 :: Disable automoc (some files contain qt specific keywords which lead to compile error when processed by the qt pre-compiler)
# find_package(Qt5Widgets CONFIG REQUIRED) # Qt5 :: Find the QtWidgets library
# include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS}) # Qt5 :: Add the include directories for the Qt 5 Widgets module to the compile lines.
# add_definitions(${Qt5Widgets_DEFINITIONS}) # Qt5 :: Use the compile definitions defined in the Qt 5 Widgets module
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" ) # Qt5 ::
# QT5_WRAP_CPP(MOC_FILES_CPP ${APP_SOURCES}) # Qt5 ::
# ### QT5_WRAP_UI( MAPGYVER_UI "ui/mainwindow.ui" )
# set(APP_SOURCES ${APP_SOURCES} ${MOC_FILES_CPP})

# set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
# ### set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals" ) # Add compiler flags for building executables (-fPIE)

# ##### Add project
# add_executable(${PROJECT_NAME} "")
# set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# target_sources(${PROJECT_NAME} PRIVATE ${APP_SOURCES})

# target_link_libraries(${PROJECT_NAME} PUBLIC "Qt5::Widgets")
# target_link_libraries(${PROJECT_NAME} PUBLIC Irrlicht)
# # target_link_libraries(${PROJECT_NAME} PUBLIC de.hampe.common)

# ### copy resource files
# ### file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) ### copy resource files
# # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

