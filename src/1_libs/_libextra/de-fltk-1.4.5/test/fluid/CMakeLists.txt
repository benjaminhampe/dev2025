set(APP_NAME fluid-1.4.5)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(APP_HEADERS
    #     ${APP_HEADER_DIR}/CodeEditor.h
    #     ${APP_HEADER_DIR}/Fd_Snap_Action.h
    #     ${APP_HEADER_DIR}/Fl_Function_Type.h
    #     ${APP_HEADER_DIR}/Fl_Grid_Type.h
    #     ${APP_HEADER_DIR}/Fl_Group_Type.h
    #     ${APP_HEADER_DIR}/Fl_Menu_Type.h
    #     ${APP_HEADER_DIR}/Fl_Type.h
    #     ${APP_HEADER_DIR}/Fl_Widget_Type.h
    #     ${APP_HEADER_DIR}/Fl_Window_Type.h
    #     ${APP_HEADER_DIR}/Fl_Button_Type.h
    #     ${APP_HEADER_DIR}/Fluid_Image.h
    #     ${APP_HEADER_DIR}/StyleParse.h
    #     ${APP_HEADER_DIR}/about_panel.h
    #     ${APP_HEADER_DIR}/align_widget.h
    #     ${APP_HEADER_DIR}/settings_panel.h
    #     ${APP_HEADER_DIR}/autodoc.h
    #     ${APP_HEADER_DIR}/code.h
    #     ${APP_HEADER_DIR}/comments.h
    #     ${APP_HEADER_DIR}/custom_widgets.h
    #     ${APP_HEADER_DIR}/factory.h
    #     ${APP_HEADER_DIR}/file.h
    #     ${APP_HEADER_DIR}/function_panel.h
    #     ${APP_HEADER_DIR}/mergeback.h
    #     ${APP_HEADER_DIR}/print_panel.h
    #     ${APP_HEADER_DIR}/pixmaps.h
    #     ${APP_HEADER_DIR}/shell_command.h
    #     ${APP_HEADER_DIR}/codeview_panel.h
    #     ${APP_HEADER_DIR}/template_panel.h
    #     ${APP_HEADER_DIR}/undo.h
    #     ${APP_HEADER_DIR}/widget_browser.h
    #     ${APP_HEADER_DIR}/widget_panel.h
    #     ${APP_HEADER_DIR}/ExternalCodeEditor_WIN32.h
    # )


    # set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(APP_SOURCES
    #     ${APP_SOURCE_DIR}/CodeEditor.cxx
    #     ${APP_SOURCE_DIR}/StyleParse.cxx
    #     ${APP_SOURCE_DIR}/Fd_Snap_Action.cxx
    #     ${APP_SOURCE_DIR}/Fl_Function_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Grid_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Group_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Menu_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Widget_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Window_Type.cxx
    #     ${APP_SOURCE_DIR}/Fl_Button_Type.cxx
    #     ${APP_SOURCE_DIR}/Fluid_Image.cxx
    #     ${APP_SOURCE_DIR}/about_panel.cxx
    #     ${APP_SOURCE_DIR}/align_widget.cxx
    #     ${APP_SOURCE_DIR}/settings_panel.cxx
    #     ${APP_SOURCE_DIR}/autodoc.cxx
    #     ${APP_SOURCE_DIR}/code.cxx
    #     ${APP_SOURCE_DIR}/custom_widgets.cxx
    #     ${APP_SOURCE_DIR}/factory.cxx
    #     ${APP_SOURCE_DIR}/file.cxx
    #     ${APP_SOURCE_DIR}/fluid_filename.cxx
    #     ${APP_SOURCE_DIR}/function_panel.cxx
    #     ${APP_SOURCE_DIR}/mergeback.cxx
    #     ${APP_SOURCE_DIR}/pixmaps.cxx
    #     ${APP_SOURCE_DIR}/shell_command.cxx
    #     ${APP_SOURCE_DIR}/codeview_panel.cxx
    #     ${APP_SOURCE_DIR}/template_panel.cxx
    #     ${APP_SOURCE_DIR}/undo.cxx
    #     ${APP_SOURCE_DIR}/widget_browser.cxx
    #     ${APP_SOURCE_DIR}/widget_panel.cxx
    #     ${APP_SOURCE_DIR}/ExternalCodeEditor_WIN32.cxx
    # )



    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/about_panel.cxx
        ${APP_SOURCE_DIR}/about_panel.h
        ${APP_SOURCE_DIR}/align_widget.cxx
        ${APP_SOURCE_DIR}/align_widget.h
        ${APP_SOURCE_DIR}/autodoc.cxx
        ${APP_SOURCE_DIR}/autodoc.h
        ${APP_SOURCE_DIR}/code.cxx
        ${APP_SOURCE_DIR}/code.h
        ${APP_SOURCE_DIR}/CodeEditor.cxx
        ${APP_SOURCE_DIR}/CodeEditor.h
        ${APP_SOURCE_DIR}/codeview_panel.cxx
        ${APP_SOURCE_DIR}/codeview_panel.h
        ${APP_SOURCE_DIR}/comments.h
        ${APP_SOURCE_DIR}/custom_widgets.cxx
        ${APP_SOURCE_DIR}/custom_widgets.h
        # ${APP_SOURCE_DIR}/ExternalCodeEditor_UNIX.cxx
        # ${APP_SOURCE_DIR}/ExternalCodeEditor_UNIX.h
        ${APP_SOURCE_DIR}/ExternalCodeEditor_WIN32.cxx
        ${APP_SOURCE_DIR}/ExternalCodeEditor_WIN32.h
        ${APP_SOURCE_DIR}/factory.cxx
        ${APP_SOURCE_DIR}/factory.h
        ${APP_SOURCE_DIR}/Fd_Snap_Action.cxx
        ${APP_SOURCE_DIR}/Fd_Snap_Action.h
        ${APP_SOURCE_DIR}/file.cxx
        ${APP_SOURCE_DIR}/file.h
        ${APP_SOURCE_DIR}/fluid.cxx
        ${APP_SOURCE_DIR}/fluid.h
        ${APP_SOURCE_DIR}/fluid_filename.cxx
        ${APP_SOURCE_DIR}/fluid_filename.h
        ${APP_SOURCE_DIR}/Fluid_Image.cxx
        ${APP_SOURCE_DIR}/Fluid_Image.h
        ${APP_SOURCE_DIR}/Fl_Button_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Button_Type.h
        ${APP_SOURCE_DIR}/Fl_Function_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Function_Type.h
        ${APP_SOURCE_DIR}/Fl_Grid_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Grid_Type.h
        ${APP_SOURCE_DIR}/Fl_Group_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Group_Type.h
        ${APP_SOURCE_DIR}/Fl_Menu_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Menu_Type.h
        ${APP_SOURCE_DIR}/Fl_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Type.h
        ${APP_SOURCE_DIR}/Fl_Widget_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Widget_Type.h
        ${APP_SOURCE_DIR}/Fl_Window_Type.cxx
        ${APP_SOURCE_DIR}/Fl_Window_Type.h
        ${APP_SOURCE_DIR}/function_panel.cxx
        ${APP_SOURCE_DIR}/function_panel.h
        ${APP_SOURCE_DIR}/mergeback.cxx
        ${APP_SOURCE_DIR}/mergeback.h
        ${APP_SOURCE_DIR}/pixmaps.cxx
        ${APP_SOURCE_DIR}/pixmaps.h
        ${APP_SOURCE_DIR}/print_panel.cxx
        ${APP_SOURCE_DIR}/print_panel.h
        ${APP_SOURCE_DIR}/settings_panel.cxx
        ${APP_SOURCE_DIR}/settings_panel.h
        ${APP_SOURCE_DIR}/shell_command.cxx
        ${APP_SOURCE_DIR}/shell_command.h
        ${APP_SOURCE_DIR}/StyleParse.cxx
        ${APP_SOURCE_DIR}/StyleParse.h
        ${APP_SOURCE_DIR}/template_panel.cxx
        ${APP_SOURCE_DIR}/template_panel.h
        ${APP_SOURCE_DIR}/undo.cxx
        ${APP_SOURCE_DIR}/undo.h
        ${APP_SOURCE_DIR}/widget_browser.cxx
        ${APP_SOURCE_DIR}/widget_browser.h
        ${APP_SOURCE_DIR}/widget_panel.cxx
        ${APP_SOURCE_DIR}/widget_panel.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_HEADER_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_link_libraries(${APP_NAME} PUBLIC de_fltk_145)
    # target_link_libraries(${APP_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${APP_NAME} PUBLIC de_jpeg)
    # target_link_libraries(${APP_NAME} PUBLIC de_png)
    target_link_libraries(${APP_NAME} PUBLIC de_zlib)
    # target_link_libraries(${APP_NAME} PUBLIC de_glew)
    # target_link_libraries(${APP_NAME} PUBLIC opengl32 glu32)
    # target_link_libraries(${APP_NAME} PUBLIC de_iconv)
    # target_link_libraries(${APP_NAME} PUBLIC de_iconv_icrt)
    # target_link_libraries(${APP_NAME} PUBLIC de_curl_shared)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
endif()
