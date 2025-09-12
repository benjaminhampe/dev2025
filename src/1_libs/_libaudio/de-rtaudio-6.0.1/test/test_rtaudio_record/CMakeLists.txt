set(APP_NAME test_rtaudio_record)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
   message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
	# add_subdirectory(lib)

# Begin QtSDK
   #set(CMAKE_AUTOMOC ON)
   #set(CMAKE_AUTORCC ON)
   #set(CMAKE_VERBOSE_MAKEFILE_OLD ${CMAKE_VERBOSE_MAKEFILE}) # save old state
   #set(CMAKE_VERBOSE_MAKEFILE ON)   # set new state
   #set(CMAKE_AUTOUIC OFF)
   # find_package(Qt5Core)
   #find_package(Qt5Widgets)
   #find_package(Qt5OpenGL)
   #find_package(Qt5 COMPONENTS Core Widgets OpenGL)
   #include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})
   #add_definitions(${Qt5Widgets_DEFINITIONS})
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
   # file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   # file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")
   # set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp
   # set(APP_SOURCES ${APP_SOURCE_DIR}/Canvas3DToolPanel.cpp ${APP_SOURCE_DIR}/main.cpp)
   # QT5_WRAP_CPP( MOC_FILES ${APP_SOURCE_DIR}/MainWindow.cpp ${APP_SOURCE_DIR}/main.cpp)
   # list(APPEND APP_SOURCES ${MOC_FILES})
   # QT5_WRAP_UI( APP_UIF "ui/mainwindow.ui" )
   # list(APPEND APP_SOURCES ${APP_UIF} )
   # copy theme resource files
   # ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}" POST_BUILD COMMAND "echo" "Copying theme.." COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes")
# Ende QtSDK

	set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	file(GLOB_RECURSE APP_HEADERS "${APP_HEADER_DIR}/*.h")
	# set(APP_HEADERS
	#	${APP_HEADER_DIR}/RtAudioStream_qt.h
	# )
	
	set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	# file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
	# file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
	# file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
	
	set(APP_SOURCES
		${APP_SOURCE_DIR}/test_rtaudio_record.cpp
		# # ${APP_SOURCE_DIR}/AudioEngineConfig.cpp
		# ${APP_SOURCE_DIR}/AudioEngineConfig_qt.cpp
		# ${APP_SOURCE_DIR}/AudioEngine.cpp
		# ${APP_SOURCE_DIR}/AudioEngine_qt.cpp
		# ${APP_SOURCE_DIR}/DarthVaderVoice_qt.cpp		
		# ${APP_SOURCE_DIR}/DspSoundTouch.cpp
		# ${APP_SOURCE_DIR}/RtAudioStream.cpp
		# ${APP_SOURCE_DIR}/RtAudioStream_qt.cpp
	)

	# list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)
   if (WIN32)
      enable_language(RC)
      #list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc")
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.h")
      list(APPEND APP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/resource.rc")
   endif()

   message("${Green}[benni] APP: ${APP_NAME}${ColourReset}")
   add_executable(${APP_NAME})
   target_sources(${APP_NAME} PUBLIC ${APP_HEADERS} PRIVATE ${APP_SOURCES})
   
   target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR})
   # target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})
   # target_compile_options(${APP_NAME} PUBLIC -Wno-deprecated-declarations)
   # target_compile_options(${APP_NAME} PUBLIC -Wno-unused-variable)
   # target_compile_options(${APP_NAME} PUBLIC -Wno-unused-but-set-variable)
   # target_compile_options(${APP_NAME} PUBLIC -Wno-sign-compare)
   # target_compile_options(${APP_NAME} PUBLIC -Wno-unused-function)
   # target_compile_options(${APP_NAME} PUBLIC -Wno-comment)

   # target_link_options(${APP_NAME} PRIVATE -mwindows) # hide console window

	if (MSVC)
		set_target_properties(${APP_NAME} PROPERTIES FOLDER _libaudio)
	else() 
		# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")	

		if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
			target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
		endif()

		target_link_libraries(${APP_NAME} PUBLIC pthread)    # Threads
		target_link_libraries(${APP_NAME} PUBLIC m)          # Math
	endif()
   

	# if (WIN32)  # on Windows:
		# target_link_libraries(${APP_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
	# else()      # on Linux:
		# target_link_libraries(${APP_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
	# endif()
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
	target_link_libraries(${APP_NAME} PUBLIC de_rtaudio)     # Audio

	# COPY MEDIA FILES relative to exe
	# file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
         # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

    # message("${Green}[benni] CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}{ColourReset}")
    # message("${Green}[benni] OUT_DIR = ${OUT_DIR}{ColourReset}")

	#set(CMAKE_VERBOSE_MAKEFILE ${CMAKE_VERBOSE_MAKEFILE_OLD}) # Restore old state
endif()
