set(LIB_NAME de_vstgui4_standalone)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/vstgui/standalone/appinit.h
        ${LIB_HEADER_DIR}/vstgui/standalone/doc.h
        ${LIB_HEADER_DIR}/vstgui/standalone/fwd.h
        ${LIB_HEADER_DIR}/vstgui/standalone/ialertbox.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iappdelegate.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iapplication.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iasync.h
        ${LIB_HEADER_DIR}/vstgui/standalone/icommand.h
        ${LIB_HEADER_DIR}/vstgui/standalone/icommondirectories.h
        ${LIB_HEADER_DIR}/vstgui/standalone/imenubuilder.h
        ${LIB_HEADER_DIR}/vstgui/standalone/interface.h
        ${LIB_HEADER_DIR}/vstgui/standalone/ipreference.h
        ${LIB_HEADER_DIR}/vstgui/standalone/ishareduiresources.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iuidescwindow.h
        ${LIB_HEADER_DIR}/vstgui/standalone/ivalue.h
        ${LIB_HEADER_DIR}/vstgui/standalone/ivaluelistener.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iwindow.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iwindowcontroller.h
        ${LIB_HEADER_DIR}/vstgui/standalone/iwindowlistener.h

        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/appdelegate.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/async.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/menubuilder.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/preferences.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/value.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/valuelistener.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/windowcontroller.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/windowlistener.h

        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/uidesc/customization.h
        ${LIB_HEADER_DIR}/vstgui/standalone/helpers/uidesc/modelbinding.h
    )	
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/vstgui/standalone/application.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/application.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/genericalertbox.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/genericalertbox.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/shareduiresources.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/shareduiresources.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/uidescriptionwindowcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/window.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/window.h

        ${LIB_SOURCE_DIR}/vstgui/standalone/helpers/value.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/iplatformwindow.h

        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkapplication.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkapplication.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkasync.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkcommondirectories.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkcommondirectories.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkpreference.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkpreference.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkrunloop.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkrunloop.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkwindow.cpp
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/gdk/gdkwindow.h

        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macapplication.mm
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macasync.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macasync.mm
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/maccommondirectories.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/maccommondirectories.mm
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macpreference.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macpreference.mm
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macutilities.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macwindow.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/macwindow.mm
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/VSTGUICommand.h
        # ${LIB_SOURCE_DIR}/vstgui/standalone/platform/mac/VSTGUICommand.mm

        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32application.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32async.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32async.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32commondirectories.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32commondirectories.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32menu.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32menu.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32preference.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32preference.h
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32window.cpp
        ${LIB_SOURCE_DIR}/vstgui/standalone/platform/win32/win32window.h   
    )
	
	# if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	# "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
	# "$<$<CONFIG:Release>:-DRELEASE=1>"
	# "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
	else()
		target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
	endif()
	
	target_link_libraries(${LIB_NAME} PUBLIC de_vstgui4_lib)
	# target_link_libraries(${LIB_NAME} PUBLIC vstgui4_uidescription)

endif()


