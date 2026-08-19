set(APP_NAME screenshot)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldBlue}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES ${APP_SOURCE_DIR}/main.cpp)
    
    if (WIN32)
       enable_language(RC)
       list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
       list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # #target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/window)

    target_compile_definitions(${APP_NAME} PRIVATE UNICODE=1)
    target_compile_definitions(${APP_NAME} PRIVATE _UNICODE=1)
    # target_compile_definitions(${APP_NAME} PRIVATE OEMRESOURCE=1) # needed by winuser.h ODC
    # target_compile_definitions(${APP_NAME} PRIVATE GL_EXT_PROTOTYPES=1)
    # target_compile_definitions(${APP_NAME} PRIVATE GL_GLEXT_PROTOTYPES=1)
    # target_compile_definitions(${APP_NAME} PRIVATE WGL_WGLEXT_PROTOTYPES=1)

    # target_compile_definitions(${APP_NAME} PUBLIC QT_FATAL_WARNINGS=1)
    
    if (MSVC)
        # set_target_properties(${APP_NAME} PROPERTIES
            # LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
            # LINK_FLAGS_RELEASE "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
            # LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
            # LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
        # )

        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        #               LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")

        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        #               LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
        #set_target_properties(${APP_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
        #set_target_properties(${APP_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
        #set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")

    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
        
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-value)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-old-style-cast)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-variable)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-function)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
    
        # target_compile_options(${APP_NAME} PRIVATE -Werror=maybe-uninitialized)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        target_link_libraries(${APP_NAME} PRIVATE pthread)
        target_link_libraries(${APP_NAME} PRIVATE m)
        target_link_libraries(${APP_NAME} PRIVATE comctl32)
        
        if (BUILD_STATIC)
            target_compile_options(${APP_NAME} PRIVATE -static)
            target_compile_options(${APP_NAME} PRIVATE -static-libstdc++)
            target_compile_options(${APP_NAME} PRIVATE -static-libgcc)
            target_link_options(${APP_NAME} PRIVATE -static)
            target_link_options(${APP_NAME} PRIVATE -static-libstdc++)
            target_link_options(${APP_NAME} PRIVATE -static-libgcc)
        endif()
    endif()

    # target_link_libraries(${APP_NAME} PRIVATE Qt5::Widgets)

endif()