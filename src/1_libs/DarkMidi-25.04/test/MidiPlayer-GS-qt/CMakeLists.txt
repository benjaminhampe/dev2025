set(APP_NAME MidiPlayer-GS-qt)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE APP_SOURCES_C "${APP_SOURCE_DIR}/*.c")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")

    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_C} ${APP_SOURCES_CPP} ${APP_SOURCES_HPP})

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
    find_package(Qt6Core)
    find_package(Qt6Widgets)
    #find_package(Qt6Svg)
    find_package(Qt6 COMPONENTS Core Widgets)
    include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    # add_definitions(${Qt6Widgets_DEFINITIONS})
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
    #QT5_WRAP_CPP( APP_MOC_FILES_CPP "mainwindow.hpp" )
    #QT5_WRAP_UI( APP_UI_FILES "ui/mainwindow.ui" )
    #set(RESOURCE_FILES "res/resource.qrc")
    #qt5_add_resources(APP_SOURCES ${RESOURCE_FILES})
    # # # # [QT] END

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")
    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/drivers)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/vendor)

    target_compile_definitions(${APP_NAME} PRIVATE HAVE_CONFIG_H=1)
    
    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows) # no console but win icon
            
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        endif()
        target_link_libraries(${APP_NAME} PRIVATE pthread)
        target_link_libraries(${APP_NAME} PRIVATE m)
    endif()

    target_link_libraries(${APP_NAME} PRIVATE "Qt6::Core")
    target_link_libraries(${APP_NAME} PRIVATE "Qt6::Widgets")

    target_link_libraries(${APP_NAME} PRIVATE DarkMidi_2504)
    target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)

endif()

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
# set(CMAKE_AUTOMOC ON) # Qt6 :: Disable automoc (some files contain qt specific keywords which lead to compile error when processed by the qt pre-compiler)
# find_package(Qt6Widgets CONFIG REQUIRED) # Qt6 :: Find the QtWidgets library
# include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS}) # Qt6 :: Add the include directories for the Qt 5 Widgets module to the compile lines.
# add_definitions(${Qt6Widgets_DEFINITIONS}) # Qt6 :: Use the compile definitions defined in the Qt 5 Widgets module
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}" ) # Qt6 ::
# QT5_WRAP_CPP(MOC_FILES_CPP ${APP_SOURCES}) # Qt6 ::
# ### QT5_WRAP_UI( MAPGYVER_UI "ui/mainwindow.ui" )
# set(APP_SOURCES ${APP_SOURCES} ${MOC_FILES_CPP})

# set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
# ### set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals" ) # Add compiler flags for building executables (-fPIE)

# ##### Add project
# add_executable(${PROJECT_NAME} "")
# set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# target_sources(${PROJECT_NAME} PRIVATE ${APP_SOURCES})

# target_link_libraries(${PROJECT_NAME} PUBLIC "Qt6::Widgets")
# target_link_libraries(${PROJECT_NAME} PUBLIC Irrlicht)
# # target_link_libraries(${PROJECT_NAME} PUBLIC de.hampe.common)

# ### copy resource files
# ### file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) ### copy resource files
# # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

