set(LIB_NAME de_fltk)
project(${LIB_NAME} CXX C)

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${PROJECT_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/include/FL")
    set(LIB_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/abi-version.h
        ${LIB_HEADER_DIR}/config.h
        ${LIB_HEADER_DIR}/Fl_dirent.h
        ${LIB_HEADER_DIR}/Enumerations.H
        ${LIB_HEADER_DIR}/filename.H
        ${LIB_HEADER_DIR}/Fl_Adjuster.H
        ${LIB_HEADER_DIR}/fl_ask.H
        ${LIB_HEADER_DIR}/Fl_Bitmap.H
        ${LIB_HEADER_DIR}/Fl_BMP_Image.H
        ${LIB_HEADER_DIR}/Fl_Box.H
        ${LIB_HEADER_DIR}/Fl_Browser_.H
        ${LIB_HEADER_DIR}/Fl_Browser.H
        ${LIB_HEADER_DIR}/Fl_Button.H
        ${LIB_HEADER_DIR}/Fl_Cairo.H
        ${LIB_HEADER_DIR}/Fl_Cairo_Window.H
        ${LIB_HEADER_DIR}/Fl_Chart.H
        ${LIB_HEADER_DIR}/Fl_Check_Browser.H
        ${LIB_HEADER_DIR}/Fl_Check_Button.H
        ${LIB_HEADER_DIR}/Fl_Choice.H
        ${LIB_HEADER_DIR}/Fl_Clock.H
        ${LIB_HEADER_DIR}/Fl_Color_Chooser.H
        ${LIB_HEADER_DIR}/Fl_Copy_Surface.H
        ${LIB_HEADER_DIR}/Fl_Counter.H
        ${LIB_HEADER_DIR}/Fl_Device.H
        ${LIB_HEADER_DIR}/Fl_Dial.H
        ${LIB_HEADER_DIR}/Fl_Double_Window.H
        ${LIB_HEADER_DIR}/fl_draw.H
        ${LIB_HEADER_DIR}/Fl_Export.H
        ${LIB_HEADER_DIR}/Fl_File_Browser.H
        ${LIB_HEADER_DIR}/Fl_File_Chooser.H
        ${LIB_HEADER_DIR}/Fl_File_Icon.H
        ${LIB_HEADER_DIR}/Fl_File_Input.H
        ${LIB_HEADER_DIR}/Fl_Fill_Dial.H
        ${LIB_HEADER_DIR}/Fl_Fill_Slider.H
        ${LIB_HEADER_DIR}/Fl_Float_Input.H
        ${LIB_HEADER_DIR}/Fl_FormsBitmap.H
        ${LIB_HEADER_DIR}/Fl_FormsPixmap.H
        ${LIB_HEADER_DIR}/Fl_Free.H
        ${LIB_HEADER_DIR}/Fl_GIF_Image.H
        ${LIB_HEADER_DIR}/Fl_Gl_Window.H
        ${LIB_HEADER_DIR}/Fl_Group.H
        ${LIB_HEADER_DIR}/Fl.H
        ${LIB_HEADER_DIR}/Fl_Help_Dialog.H
        ${LIB_HEADER_DIR}/Fl_Help_View.H
        ${LIB_HEADER_DIR}/Fl_Hold_Browser.H
        ${LIB_HEADER_DIR}/Fl_Hor_Fill_Slider.H
        ${LIB_HEADER_DIR}/Fl_Hor_Nice_Slider.H
        ${LIB_HEADER_DIR}/Fl_Hor_Slider.H
        ${LIB_HEADER_DIR}/Fl_Hor_Value_Slider.H
        ${LIB_HEADER_DIR}/Fl_Image.H
        ${LIB_HEADER_DIR}/Fl_Image_Surface.H
        ${LIB_HEADER_DIR}/Fl_Input_Choice.H
        ${LIB_HEADER_DIR}/Fl_Input_.H
        ${LIB_HEADER_DIR}/Fl_Input.H
        ${LIB_HEADER_DIR}/Fl_Int_Input.H
        ${LIB_HEADER_DIR}/Fl_JPEG_Image.H
        ${LIB_HEADER_DIR}/Fl_Light_Button.H
        ${LIB_HEADER_DIR}/Fl_Line_Dial.H
        ${LIB_HEADER_DIR}/Fl_Menu_Bar.H
        ${LIB_HEADER_DIR}/Fl_Menu_Button.H
        ${LIB_HEADER_DIR}/Fl_Menu_.H
        ${LIB_HEADER_DIR}/Fl_Menu.H
        ${LIB_HEADER_DIR}/Fl_Menu_Item.H
        ${LIB_HEADER_DIR}/Fl_Menu_Window.H
        ${LIB_HEADER_DIR}/fl_message.H
        ${LIB_HEADER_DIR}/Fl_Multi_Browser.H
        ${LIB_HEADER_DIR}/Fl_Multi_Label.H
        ${LIB_HEADER_DIR}/Fl_Multiline_Input.H
        ${LIB_HEADER_DIR}/Fl_Multiline_Output.H
        ${LIB_HEADER_DIR}/Fl_Native_File_Chooser.H
        ${LIB_HEADER_DIR}/Fl_Nice_Slider.H
        ${LIB_HEADER_DIR}/Fl_Object.H
        ${LIB_HEADER_DIR}/Fl_Output.H
        ${LIB_HEADER_DIR}/Fl_Overlay_Window.H
        ${LIB_HEADER_DIR}/Fl_Pack.H
        ${LIB_HEADER_DIR}/Fl_Paged_Device.H
        ${LIB_HEADER_DIR}/Fl_Pixmap.H
        ${LIB_HEADER_DIR}/Fl_Plugin.H
        ${LIB_HEADER_DIR}/Fl_PNG_Image.H
        ${LIB_HEADER_DIR}/Fl_PNM_Image.H
        ${LIB_HEADER_DIR}/Fl_Positioner.H
        ${LIB_HEADER_DIR}/Fl_PostScript.H
        ${LIB_HEADER_DIR}/Fl_Preferences.H
        ${LIB_HEADER_DIR}/Fl_Printer.H
        ${LIB_HEADER_DIR}/Fl_Progress.H
        ${LIB_HEADER_DIR}/Fl_Radio_Button.H
        ${LIB_HEADER_DIR}/Fl_Radio_Light_Button.H
        ${LIB_HEADER_DIR}/Fl_Radio_Round_Button.H
        ${LIB_HEADER_DIR}/Fl_Repeat_Button.H
        ${LIB_HEADER_DIR}/Fl_Return_Button.H
        ${LIB_HEADER_DIR}/Fl_RGB_Image.H
        ${LIB_HEADER_DIR}/Fl_Roller.H
        ${LIB_HEADER_DIR}/Fl_Round_Button.H
        ${LIB_HEADER_DIR}/Fl_Round_Clock.H
        ${LIB_HEADER_DIR}/Fl_Scrollbar.H
        ${LIB_HEADER_DIR}/Fl_Scroll.H
        ${LIB_HEADER_DIR}/Fl_Secret_Input.H
        ${LIB_HEADER_DIR}/Fl_Select_Browser.H
        ${LIB_HEADER_DIR}/Fl_Shared_Image.H
        ${LIB_HEADER_DIR}/fl_show_colormap.H
        ${LIB_HEADER_DIR}/fl_show_input.H
        ${LIB_HEADER_DIR}/Fl_Simple_Counter.H
        ${LIB_HEADER_DIR}/Fl_Single_Window.H
        ${LIB_HEADER_DIR}/Fl_Slider.H
        ${LIB_HEADER_DIR}/Fl_Spinner.H
        ${LIB_HEADER_DIR}/Fl_Sys_Menu_Bar.H
        ${LIB_HEADER_DIR}/Fl_Table.H
        ${LIB_HEADER_DIR}/Fl_Table_Row.H
        ${LIB_HEADER_DIR}/Fl_Tabs.H
        ${LIB_HEADER_DIR}/Fl_Text_Buffer.H
        ${LIB_HEADER_DIR}/Fl_Text_Display.H
        ${LIB_HEADER_DIR}/Fl_Text_Editor.H
        ${LIB_HEADER_DIR}/Fl_Tiled_Image.H
        ${LIB_HEADER_DIR}/Fl_Tile.H
        ${LIB_HEADER_DIR}/Fl_Timer.H
        ${LIB_HEADER_DIR}/Fl_Toggle_Button.H
        ${LIB_HEADER_DIR}/Fl_Toggle_Light_Button.H
        ${LIB_HEADER_DIR}/Fl_Toggle_Round_Button.H
        ${LIB_HEADER_DIR}/Fl_Tooltip.H
        ${LIB_HEADER_DIR}/Fl_Tree.H
        ${LIB_HEADER_DIR}/Fl_Tree_Item_Array.H
        ${LIB_HEADER_DIR}/Fl_Tree_Item.H
        ${LIB_HEADER_DIR}/Fl_Tree_Prefs.H
        ${LIB_HEADER_DIR}/fl_types.h
        ${LIB_HEADER_DIR}/fl_utf8.h
        ${LIB_HEADER_DIR}/Fl_Valuator.H
        ${LIB_HEADER_DIR}/Fl_Value_Input.H
        ${LIB_HEADER_DIR}/Fl_Value_Output.H
        ${LIB_HEADER_DIR}/Fl_Value_Slider.H
        ${LIB_HEADER_DIR}/Fl_Widget.H
        ${LIB_HEADER_DIR}/Fl_Window.H
        ${LIB_HEADER_DIR}/Fl_Wizard.H
        ${LIB_HEADER_DIR}/Fl_XBM_Image.H
        ${LIB_HEADER_DIR}/Fl_XPM_Image.H
        ${LIB_HEADER_DIR}/forms.H
        ${LIB_HEADER_DIR}/gl2opengl.h
        ${LIB_HEADER_DIR}/gl_draw.H
        ${LIB_HEADER_DIR}/gl.h
        ${LIB_HEADER_DIR}/glu.h
        ${LIB_HEADER_DIR}/glut.H
        ${LIB_HEADER_DIR}/mac.H
        ${LIB_HEADER_DIR}/Makefile.in
        ${LIB_HEADER_DIR}/Fl_Math.h
        ${LIB_HEADER_DIR}/names.h
        ${LIB_HEADER_DIR}/platform.H
        ${LIB_HEADER_DIR}/win32.H
        ${LIB_HEADER_DIR}/x.H
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Fl.cxx
        ${LIB_SOURCE_DIR}/Fl_Adjuster.cxx
        ${LIB_SOURCE_DIR}/Fl_Bitmap.cxx
        ${LIB_SOURCE_DIR}/Fl_Browser.cxx
        ${LIB_SOURCE_DIR}/Fl_Browser_.cxx
        ${LIB_SOURCE_DIR}/Fl_Browser_load.cxx
        ${LIB_SOURCE_DIR}/Fl_Box.cxx
        ${LIB_SOURCE_DIR}/Fl_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Chart.cxx
        ${LIB_SOURCE_DIR}/Fl_Check_Browser.cxx
        ${LIB_SOURCE_DIR}/Fl_Check_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Choice.cxx
        ${LIB_SOURCE_DIR}/Fl_Clock.cxx
        ${LIB_SOURCE_DIR}/Fl_Color_Chooser.cxx
        ${LIB_SOURCE_DIR}/Fl_Copy_Surface.cxx
        ${LIB_SOURCE_DIR}/Fl_Counter.cxx
        ${LIB_SOURCE_DIR}/Fl_Device.cxx
        ${LIB_SOURCE_DIR}/Fl_Dial.cxx
        ${LIB_SOURCE_DIR}/Fl_Help_Dialog_Dox.cxx
        ${LIB_SOURCE_DIR}/Fl_Double_Window.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Browser.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Chooser.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Chooser2.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Icon.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Input.cxx
        ${LIB_SOURCE_DIR}/Fl_Group.cxx
        ${LIB_SOURCE_DIR}/Fl_Help_View.cxx
        ${LIB_SOURCE_DIR}/Fl_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_Image_Surface.cxx
        ${LIB_SOURCE_DIR}/Fl_Input.cxx
        ${LIB_SOURCE_DIR}/Fl_Input_.cxx
        ${LIB_SOURCE_DIR}/Fl_Light_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_Bar.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_Window.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_add.cxx
        ${LIB_SOURCE_DIR}/Fl_Menu_global.cxx
        ${LIB_SOURCE_DIR}/Fl_Multi_Label.cxx
        ${LIB_SOURCE_DIR}/Fl_Native_File_Chooser.cxx
        ${LIB_SOURCE_DIR}/Fl_Overlay_Window.cxx
        ${LIB_SOURCE_DIR}/Fl_Pack.cxx
        ${LIB_SOURCE_DIR}/Fl_Paged_Device.cxx
        ${LIB_SOURCE_DIR}/Fl_Pixmap.cxx
        ${LIB_SOURCE_DIR}/Fl_Positioner.cxx
        ${LIB_SOURCE_DIR}/Fl_PostScript.cxx
        ${LIB_SOURCE_DIR}/Fl_Printer.cxx
        ${LIB_SOURCE_DIR}/Fl_Preferences.cxx
        ${LIB_SOURCE_DIR}/Fl_Progress.cxx
        ${LIB_SOURCE_DIR}/Fl_Repeat_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Return_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Roller.cxx
        ${LIB_SOURCE_DIR}/Fl_Round_Button.cxx
        ${LIB_SOURCE_DIR}/Fl_Scroll.cxx
        ${LIB_SOURCE_DIR}/Fl_Scrollbar.cxx
        ${LIB_SOURCE_DIR}/Fl_Shared_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_Single_Window.cxx
        ${LIB_SOURCE_DIR}/Fl_Slider.cxx
        ${LIB_SOURCE_DIR}/Fl_Table.cxx
        ${LIB_SOURCE_DIR}/Fl_Table_Row.cxx
        ${LIB_SOURCE_DIR}/Fl_Tabs.cxx
        ${LIB_SOURCE_DIR}/Fl_Text_Buffer.cxx
        ${LIB_SOURCE_DIR}/Fl_Text_Display.cxx
        ${LIB_SOURCE_DIR}/Fl_Text_Editor.cxx
        ${LIB_SOURCE_DIR}/Fl_Tile.cxx
        ${LIB_SOURCE_DIR}/Fl_Tiled_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_Tooltip.cxx
        ${LIB_SOURCE_DIR}/Fl_Tree.cxx
        ${LIB_SOURCE_DIR}/Fl_Tree_Item_Array.cxx
        ${LIB_SOURCE_DIR}/Fl_Tree_Item.cxx
        ${LIB_SOURCE_DIR}/Fl_Tree_Prefs.cxx
        ${LIB_SOURCE_DIR}/Fl_Valuator.cxx
        ${LIB_SOURCE_DIR}/Fl_Value_Input.cxx
        ${LIB_SOURCE_DIR}/Fl_Value_Output.cxx
        ${LIB_SOURCE_DIR}/Fl_Value_Slider.cxx
        ${LIB_SOURCE_DIR}/Fl_Widget.cxx
        ${LIB_SOURCE_DIR}/Fl_Window.cxx
        ${LIB_SOURCE_DIR}/Fl_Window_fullscreen.cxx
        ${LIB_SOURCE_DIR}/Fl_Window_hotspot.cxx
        ${LIB_SOURCE_DIR}/Fl_Window_iconize.cxx
        ${LIB_SOURCE_DIR}/Fl_Window_shape.cxx
        ${LIB_SOURCE_DIR}/Fl_Wizard.cxx
        ${LIB_SOURCE_DIR}/Fl_XBM_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_XPM_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_abort.cxx
        ${LIB_SOURCE_DIR}/Fl_add_idle.cxx
        ${LIB_SOURCE_DIR}/Fl_arg.cxx
        ${LIB_SOURCE_DIR}/Fl_compose.cxx
        ${LIB_SOURCE_DIR}/Fl_display.cxx
        ${LIB_SOURCE_DIR}/Fl_get_key.cxx
        ${LIB_SOURCE_DIR}/Fl_get_system_colors.cxx
        ${LIB_SOURCE_DIR}/Fl_grab.cxx
        ${LIB_SOURCE_DIR}/Fl_lock.cxx
        ${LIB_SOURCE_DIR}/Fl_own_colormap.cxx
        ${LIB_SOURCE_DIR}/Fl_visual.cxx
        ${LIB_SOURCE_DIR}/Fl_x.cxx
        ${LIB_SOURCE_DIR}/filename_absolute.cxx
        ${LIB_SOURCE_DIR}/filename_expand.cxx
        ${LIB_SOURCE_DIR}/filename_ext.cxx
        ${LIB_SOURCE_DIR}/filename_isdir.cxx
        ${LIB_SOURCE_DIR}/filename_list.cxx
        ${LIB_SOURCE_DIR}/filename_match.cxx
        ${LIB_SOURCE_DIR}/filename_setext.cxx
        ${LIB_SOURCE_DIR}/fl_arc.cxx
        ${LIB_SOURCE_DIR}/fl_arci.cxx
        ${LIB_SOURCE_DIR}/fl_ask.cxx
        ${LIB_SOURCE_DIR}/fl_boxtype.cxx
        ${LIB_SOURCE_DIR}/fl_color.cxx
        ${LIB_SOURCE_DIR}/fl_cursor.cxx
        ${LIB_SOURCE_DIR}/fl_curve.cxx
        ${LIB_SOURCE_DIR}/fl_diamond_box.cxx
        ${LIB_SOURCE_DIR}/fl_dnd.cxx
        ${LIB_SOURCE_DIR}/fl_draw.cxx
        ${LIB_SOURCE_DIR}/fl_draw_image.cxx
        ${LIB_SOURCE_DIR}/fl_draw_pixmap.cxx
        ${LIB_SOURCE_DIR}/fl_engraved_label.cxx
        ${LIB_SOURCE_DIR}/fl_file_dir.cxx
        ${LIB_SOURCE_DIR}/fl_font.cxx
        ${LIB_SOURCE_DIR}/fl_gleam.cxx
        ${LIB_SOURCE_DIR}/fl_gtk.cxx
        ${LIB_SOURCE_DIR}/fl_labeltype.cxx
        ${LIB_SOURCE_DIR}/fl_line_style.cxx
        ${LIB_SOURCE_DIR}/fl_open_uri.cxx
        ${LIB_SOURCE_DIR}/fl_oval_box.cxx
        ${LIB_SOURCE_DIR}/fl_overlay.cxx
        ${LIB_SOURCE_DIR}/fl_overlay_visual.cxx
        ${LIB_SOURCE_DIR}/fl_plastic.cxx
        ${LIB_SOURCE_DIR}/fl_read_image.cxx
        ${LIB_SOURCE_DIR}/fl_rect.cxx
        ${LIB_SOURCE_DIR}/fl_round_box.cxx
        ${LIB_SOURCE_DIR}/fl_rounded_box.cxx
        ${LIB_SOURCE_DIR}/fl_set_font.cxx
        ${LIB_SOURCE_DIR}/fl_set_fonts.cxx
        ${LIB_SOURCE_DIR}/fl_scroll_area.cxx
        ${LIB_SOURCE_DIR}/fl_shadow_box.cxx
        ${LIB_SOURCE_DIR}/fl_shortcut.cxx
        ${LIB_SOURCE_DIR}/fl_show_colormap.cxx
        ${LIB_SOURCE_DIR}/fl_symbols.cxx
        ${LIB_SOURCE_DIR}/fl_vertex.cxx
        ${LIB_SOURCE_DIR}/ps_image.cxx
        ${LIB_SOURCE_DIR}/screen_xywh.cxx
        ${LIB_SOURCE_DIR}/fl_utf8.cxx
        ${LIB_SOURCE_DIR}/fl_encoding_latin1.cxx
        ${LIB_SOURCE_DIR}/fl_encoding_mac_roman.cxx
        ${LIB_SOURCE_DIR}/forms_compatability.cxx
        ${LIB_SOURCE_DIR}/forms_bitmap.cxx
        ${LIB_SOURCE_DIR}/forms_free.cxx
        ${LIB_SOURCE_DIR}/forms_fselect.cxx
        ${LIB_SOURCE_DIR}/forms_pixmap.cxx
        ${LIB_SOURCE_DIR}/forms_timer.cxx
    )

    set(LIB_SOURCES_OPENGL
        ${LIB_SOURCE_DIR}/Fl_Gl_Choice.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Device_Plugin.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Overlay.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Window.cxx
        # ${LIB_SOURCE_DIR}/freeglut_geometry.cxx
        # ${LIB_SOURCE_DIR}/freeglut_stroke_mono_roman.cxx
        # ${LIB_SOURCE_DIR}/freeglut_stroke_roman.cxx
        # ${LIB_SOURCE_DIR}/freeglut_teapot.cxx
        ${LIB_SOURCE_DIR}/gl_draw.cxx
        ${LIB_SOURCE_DIR}/gl_start.cxx
        # ${LIB_SOURCE_DIR}/glut_compatability.cxx
        # ${LIB_SOURCE_DIR}/glut_font.cxx
    )

    set(LIB_SOURCES_FREEGLUT
        ${LIB_SOURCE_DIR}/Fl_Gl_Choice.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Device_Plugin.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Overlay.cxx
        ${LIB_SOURCE_DIR}/Fl_Gl_Window.cxx
        ${LIB_SOURCE_DIR}/freeglut_geometry.cxx
        ${LIB_SOURCE_DIR}/freeglut_stroke_mono_roman.cxx
        ${LIB_SOURCE_DIR}/freeglut_stroke_roman.cxx
        ${LIB_SOURCE_DIR}/freeglut_teapot.cxx
        ${LIB_SOURCE_DIR}/gl_draw.cxx
        ${LIB_SOURCE_DIR}/gl_start.cxx
        ${LIB_SOURCE_DIR}/glut_compatability.cxx
        ${LIB_SOURCE_DIR}/glut_font.cxx
    )

    set(LIB_SOURCES_IMG
        ${LIB_SOURCE_DIR}/fl_images_core.cxx
        ${LIB_SOURCE_DIR}/Fl_BMP_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_File_Icon2.cxx
        ${LIB_SOURCE_DIR}/Fl_GIF_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_Help_Dialog.cxx
        ${LIB_SOURCE_DIR}/Fl_JPEG_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_PNG_Image.cxx
        ${LIB_SOURCE_DIR}/Fl_PNM_Image.cxx
    )

    set(LIB_SOURCES_C
        ${LIB_SOURCE_DIR}/flstring.c
        ${LIB_SOURCE_DIR}/scandir.c
        ${LIB_SOURCE_DIR}/numericsort.c
        ${LIB_SOURCE_DIR}/vsnprintf.c
        ${LIB_SOURCE_DIR}/xutf8/is_right2left.c
        ${LIB_SOURCE_DIR}/xutf8/is_spacing.c
        ${LIB_SOURCE_DIR}/xutf8/case.c
        ${LIB_SOURCE_DIR}/xutf8/utf8Input.c
        ${LIB_SOURCE_DIR}/xutf8/utf8Utils.c
        ${LIB_SOURCE_DIR}/xutf8/utf8Wrap.c
        ${LIB_SOURCE_DIR}/xutf8/keysym2Ucs.c
        ${LIB_SOURCE_DIR}/fl_utf.c
    )

    if (WIN32)
        list(APPEND LIB_SOURCES_C ${LIB_SOURCE_DIR}/fl_call_main.c)
    endif (WIN32)

    if (APPLE AND NOT OPTION_APPLE_X11)
        set(LIB_SOURCES_MAC
        ${LIB_SOURCE_DIR}/Fl_cocoa.mm
        ${LIB_SOURCE_DIR}/Fl_Quartz_Printer.mm
        ${LIB_SOURCE_DIR}/Fl_Native_File_Chooser_MAC.mm
        ${LIB_SOURCE_DIR}/Fl_Sys_Menu_Bar.mm)
    else()
        set(LIB_SOURCES_MAC)
    endif (APPLE AND NOT OPTION_APPLE_X11)

    # list(APPEND LIB_SOURCES ${LIB_SOURCES_MAC})
    list(APPEND LIB_SOURCES ${LIB_SOURCES_IMG})
    # list(APPEND LIB_SOURCES ${LIB_SOURCES_FREEGLUT})
    list(APPEND LIB_SOURCES ${LIB_SOURCES_C})
    list(APPEND LIB_SOURCES ${LIB_SOURCES_OPENGL})
    #######################################################################

    # if (BUILD_SHARED)
    # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    # add_library(${LIB_NAME} SHARED "")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DFL_LIBRARY=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DFL_DLL=1)
    # else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
		target_compile_definitions(${LIB_NAME} PUBLIC FL_LIBRARY=1)
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FLTK=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    target_link_libraries(${LIB_NAME} PUBLIC de_jpeg8d)
    target_link_libraries(${LIB_NAME} PUBLIC de_png)
    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)

    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC comctl32)
    elseif (UNIX)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GLXGETPROCADDRESSARB=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DIRENT_H=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SCANDIR=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SCANDIR_POSIX=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_XINERAMA=1)
        target_compile_definitions(${LIB_NAME} PUBLIC USE_XFT=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_XDBE=1)
        target_compile_definitions(${LIB_NAME} PUBLIC USE_XDBE HAVE_XDBE=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_XFIXES=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_XCURSOR=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_XRENDER=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_X11_XREGION_H=1)
        target_compile_definitions(${LIB_NAME} PUBLIC USE_X11=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DLFCN_H=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DLSYM=1)
        target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ALSA_ASOUNDLIB_H=1)
        target_link_libraries(${LIB_NAME} PUBLIC Xft fontconfig Xext Xrender Xinerama Xfixes Xcursor X11)
        target_link_libraries(${LIB_NAME} PUBLIC dl)
    endif()
    
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libextra)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC pthread)	
    endif()
        
endif()

# add_subdirectory(test)