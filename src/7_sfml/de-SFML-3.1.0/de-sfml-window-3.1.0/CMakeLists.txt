set(LIB_NAME de_sfml_window_310)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    # set(LUNASVG_VERSION_MAJOR 3)
    # set(LUNASVG_VERSION_MINOR 1)
    # set(LUNASVG_VERSION_MICRO 0)

    set(SFML_OS_WINDOWS 1)
    # set(SFML_OS_LINUX 1)
    # set(SFML_OS_ANDROID 1)
    # set(SFML_OS_MACOS 1)
    # set(SFML_OS_IOS 1)
    # set(SFML_OS_FREEBSD 1)
    # set(SFML_OS_OPENBSD 1)
    # set(SFML_OS_NETBSD 1)

    # set(SFML_OPENGL_ES 1)
    # set(SFML_USE_DRM 1)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SFML/Window.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Clipboard.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Context.hpp
        ${LIB_HEADER_DIR}/SFML/Window/ContextSettings.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Cursor.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Event.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Event.inl
        ${LIB_HEADER_DIR}/SFML/Window/Export.hpp
        ${LIB_HEADER_DIR}/SFML/Window/GlResource.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Joystick.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Keyboard.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Mouse.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Sensor.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Touch.hpp
        ${LIB_HEADER_DIR}/SFML/Window/VideoMode.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Vulkan.hpp
        ${LIB_HEADER_DIR}/SFML/Window/Window.hpp
        ${LIB_HEADER_DIR}/SFML/Window/WindowBase.hpp
        ${LIB_HEADER_DIR}/SFML/Window/WindowBase.inl
        ${LIB_HEADER_DIR}/SFML/Window/WindowEnums.hpp
        ${LIB_HEADER_DIR}/SFML/Window/WindowHandle.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_SOURCES_ANDROID
        ${LIB_SOURCE_DIR}/SFML/Window/Android/ClipboardImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/CursorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/InputImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JniHelper.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JniHelper.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JniHelper.inl
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JoystickButton.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/SensorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/VideoModeImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/WindowImplAndroid.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Android/WindowImplAndroid.hpp
    )

    set(LIB_SOURCES_DRM
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/ClipboardImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/CursorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/DRMContext.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/DRMContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/InputImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/VideoModeImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/WindowImplDRM.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/DRM/WindowImplDRM.hpp
    )

    set(LIB_SOURCES_IOS
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/ClipboardImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/CursorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/EaglContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/EaglContext.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/InputImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/JoystickImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/ObjCType.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SensorImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFAppDelegate.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFAppDelegate.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFMain.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFMain.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFView.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFView.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFViewController.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/SFViewController.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/VideoModeImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/WindowImplUIKit.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/iOS/WindowImplUIKit.mm
    )

    set(LIB_SOURCES_MACOS
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/AutoreleasePoolWrapper.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/AutoreleasePoolWrapper.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/cg_sf_conversion.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/cg_sf_conversion.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/ClipboardImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/CursorImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/HIDInputManager.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/HIDInputManager.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/HIDJoystickManager.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/HIDJoystickManager.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/InputImpl.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/NSImage+raw.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/NSImage+raw.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/Scaling.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SensorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFApplication.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFApplication.m
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFApplicationDelegate.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFApplicationDelegate.m
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFContext.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFKeyboardModifiersHelper.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFKeyboardModifiersHelper.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView+keyboard.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView+keyboard_priv.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView+mouse.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView+mouse_priv.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFOpenGLView.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFSilentResponder.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFSilentResponder.m
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFViewController.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFViewController.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFWindow.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFWindow.m
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFWindowController.h
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/SFWindowController.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/VideoModeImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/WindowImplCocoa.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/WindowImplCocoa.mm
        ${LIB_SOURCE_DIR}/SFML/Window/macOS/WindowImplDelegateProtocol.h
    )

    set(LIB_SOURCES_FREEBSD
        ${LIB_SOURCE_DIR}/SFML/Window/FreeBSD/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/FreeBSD/JoystickImpl.hpp
    )

    set(LIB_SOURCES_NETBSD
        ${LIB_SOURCE_DIR}/SFML/Window/NetBSD/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/NetBSD/JoystickImpl.hpp
    )

    set(LIB_SOURCES_OPENBSD
        ${LIB_SOURCE_DIR}/SFML/Window/OpenBSD/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/OpenBSD/JoystickImpl.hpp
    )

    set(LIB_SOURCES_UNIX
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/ClipboardImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/CursorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/Display.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/Display.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/GlxContext.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/GlxContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/InputImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeyboardImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeyboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeySymToKeyMapping.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeySymToKeyMapping.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeySymToUnicodeMapping.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/KeySymToUnicodeMapping.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/SensorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/Utils.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/VideoModeImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/VulkanImplX11.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/WindowImplX11.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Unix/WindowImplX11.hpp
    )

    set(LIB_SOURCES_WIN32
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/ClipboardImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/ClipboardImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/CursorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/CursorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/InputImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/JoystickImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/SensorImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/Utils.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/VideoModeImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/VulkanImplWin32.cpp
        # ${LIB_SOURCE_DIR}/Win32/WglContext.cpp
        # ${LIB_SOURCE_DIR}/Win32/WglContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/WindowImplWin32.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Win32/WindowImplWin32.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Window.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/WindowBase.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/WindowImpl.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/WindowImpl.hpp
    )

    if (SFML_OPENGL_ES)
        list(APPEND LIB_SOURCES_WIN32
            ${LIB_SOURCE_DIR}/SFML/Window/EGLCheck.cpp
            ${LIB_SOURCE_DIR}/SFML/Window/EGLCheck.hpp
            ${LIB_SOURCE_DIR}/SFML/Window/EglContext.cpp
            ${LIB_SOURCE_DIR}/SFML/Window/EglContext.hpp
        )
    else()
        list(APPEND LIB_SOURCES_WIN32
            ${LIB_SOURCE_DIR}/SFML/Window/Win32/WglContext.cpp
            ${LIB_SOURCE_DIR}/SFML/Window/Win32/WglContext.hpp
        )
    endif()

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/SFML/Window/Clipboard.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/ClipboardImpl.hpp
        # ${LIB_SOURCE_DIR}/CMakeLists.txt
        ${LIB_SOURCE_DIR}/SFML/Window/Context.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/Cursor.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/CursorImpl.hpp
        # ${LIB_SOURCE_DIR}/Dependencies.cmake.in

        # ${LIB_SOURCE_DIR}/EGLCheck.cpp
        # ${LIB_SOURCE_DIR}/EGLCheck.hpp
        # ${LIB_SOURCE_DIR}/EglContext.cpp
        # ${LIB_SOURCE_DIR}/EglContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/GlContext.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/GlContext.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/GlResource.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/InputImpl.hpp

        ${LIB_SOURCE_DIR}/SFML/Window/Joystick.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/JoystickImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/JoystickManager.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/JoystickManager.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Keyboard.cpp

        ${LIB_SOURCE_DIR}/SFML/Window/Mouse.cpp

        ${LIB_SOURCE_DIR}/SFML/Window/Sensor.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/SensorImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/SensorManager.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/SensorManager.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Touch.cpp

        ${LIB_SOURCE_DIR}/SFML/Window/VideoMode.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/VideoModeImpl.hpp
        ${LIB_SOURCE_DIR}/SFML/Window/Vulkan.cpp
        ${LIB_SOURCE_DIR}/SFML/Window/VulkanImpl.hpp
    )

    if (WIN32)
        list(APPEND LIB_SOURCES ${LIB_SOURCES_WIN32})
    endif()

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SFML_STATIC=1)
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SFML_WINDOW_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/extlibs/headers/vulkan)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/extlibs/headers/glad-4.6/include)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # set(LIB_EXTLIB_DIR ${PROJECT_SOURCE_DIR}/extlibs/headers)
    # target_include_directories(${LIB_NAME} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/vulkan")

    # target_compile_definitions(${LIB_NAME} PRIVATE SFML_USE_DRM)


    # target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_DISABLE_LOAD_SYSTEM_FONTS)

    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    # if (MSVC)
    # else()
    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    # endif()
    target_link_libraries(${LIB_NAME} PUBLIC opengl32 gdi32)
    # target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    # #target_link_libraries(${LIB_NAME} PUBLIC de_glad)

    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_system_310)
    # target_link_libraries(${LIB_NAME} PUBLIC de_vulkan_loader_shared)

endif()

# add_subdirectory(test)
