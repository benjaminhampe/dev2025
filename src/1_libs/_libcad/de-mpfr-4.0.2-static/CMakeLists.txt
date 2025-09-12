set(LIB_NAME de_mpfr_static_402)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

#	if (BUILD_SHARED)
#		 message("${BoldBlue}[benni] SHARED_LIBRARY: ${LIB_NAME}${ColourReset}")
#		 add_library(${LIB_NAME} SHARED "")
#	else()
    message("${BoldYellow}[benni] INT: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} IMPORTED STATIC)
#   endif()
	
   # add_custom_command( TARGET ${LIB_NAME} COMMAND "echo" "Copy ffprobe.exe" COMMAND ${CMAKE_COMMAND} -E copy
   #       ${CMAKE_CURRENT_LIST_DIR}/bin/ffprobe.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ffprobe.exe)

   # target_sources(${LIB_NAME} PUBLIC ${LIB_MPFR_HEADERS} PRIVATE ${LIB_MPFR_SOURCES})

   target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
   target_link_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib)
   
   # target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

	if (MSVC)
		target_link_libraries(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib/libmpfr.lib)
	else()
		target_link_libraries(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib/libmpfr.a)
		# target_link_libraries(${LIB_NAME} INTERFACE mpfr)		
	endif()

endif()


# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# find_package("Qt5Widgets") # Find the QtWidgets library
# INCLUDE_DIRECTORIES(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})
# ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
# QT5_WRAP_CPP(APP_MOC_FILES_CPP "mainwindow.hpp")
# QT5_WRAP_UI(APP_UI_FILES "ui/mainwindow.ui")
# set(RESOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/fontawesome.qrc")
# qt5_add_resources(LIB_SOURCES ${RESOURCE_FILES} )
# target_link_libraries(${LIB_NAME} PRIVATE "Qt5::Widgets")


# copy fonts:
# file(COPY "${BHD}/Qt5Core.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5Gui.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5Widgets.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release
# file(COPY "${BHD}/Qt5WinExtras.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Release

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
   # COMMAND "echo" "Copying fontawesome.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/fontawesome.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome.ttf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
   # COMMAND "echo" "Copying fontawesome CheatSheet.pdf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/fontawesome.pdf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome.pdf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-regular-400.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-regular-400.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-regular-400.ttf)

# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-brands-400.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-brands-400.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-brands-400.ttf)

         
# add_custom_command( TARGET ${LIB_NAME} PRE_BUILD 
   # COMMAND "echo" "Copying la-solid-900.ttf"
   # COMMAND ${CMAKE_COMMAND} -E copy
         # ${CMAKE_CURRENT_LIST_DIR}/la-solid-900.ttf
         # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/la-solid-900.ttf)
