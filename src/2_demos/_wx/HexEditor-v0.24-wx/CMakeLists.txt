set(APP_NAME HexEditor-0.24-wx)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/windrv.h
        ${APP_SOURCE_DIR}/windrv.cpp
        ${APP_SOURCE_DIR}/HexPanels.h
        ${APP_SOURCE_DIR}/HexPanels.cpp
        ${APP_SOURCE_DIR}/HexEditorGui.h
        ${APP_SOURCE_DIR}/HexEditorGui.cpp
        ${APP_SOURCE_DIR}/HexEditorFrame.h
        ${APP_SOURCE_DIR}/HexEditorFrame.cpp
        ${APP_SOURCE_DIR}/HexEditorApp.h
        ${APP_SOURCE_DIR}/HexEditorApp.cpp
        ${APP_SOURCE_DIR}/HexEditor.h
        ${APP_SOURCE_DIR}/HexEditor.cpp
        ${APP_SOURCE_DIR}/HexDialogs.h
        ${APP_SOURCE_DIR}/HexDialogs.cpp
        ${APP_SOURCE_DIR}/FAL.h
        ${APP_SOURCE_DIR}/FAL.cpp
        # 1:
        # ${APP_SOURCE_DIR}/HexEditorCtrl/app.cpp
        # ${APP_SOURCE_DIR}/HexEditorCtrl/app.h
        # ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrl.cbp
        ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrl.cpp
        # ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrl.fbp
        ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrl.h
        ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrlGui.cpp
        ${APP_SOURCE_DIR}/HexEditorCtrl/HexEditorCtrlGui.h
        # ${APP_SOURCE_DIR}/HexEditorCtrl/main.cpp
        # ${APP_SOURCE_DIR}/HexEditorCtrl/main.h
        # 2:
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/app.cpp
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/app.h
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/main.cpp
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/main.h
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/Tag.cpp
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/Tag.h
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/TagDialog.fbp
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/TagDialogGui.h
        # ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cbp
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp
        ${APP_SOURCE_DIR}/HexEditorCtrl/wxHexCtrl/wxHexCtrl.h
    )

    if (WIN32)
        enable_language(RC)
        # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")
    
    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${APP_NAME} PUBLIC -Wno-deprecated-declarations)

        target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
    endif()
    
    target_link_libraries(${APP_NAME} PRIVATE de_wx)
    target_link_libraries(${APP_NAME} PRIVATE de_mhash)
    target_link_libraries(${APP_NAME} PRIVATE de_udis86)

    # media/tex.png -> bin/media/wxIrrlicht/tex.png
    #file(COPY "${CMAKE_CURRENT_LIST_DIR}/media/tex.png" 
    #   DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/wxIrrlichtControl2021)

endif()
