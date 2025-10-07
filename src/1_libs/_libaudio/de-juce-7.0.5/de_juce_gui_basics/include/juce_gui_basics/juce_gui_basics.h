/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_SAFT_MODULE_DECLARATION

  ID:                 juce_gui_basics
  vendor:             juce
  version:            7.0.5
  name:               JUCE GUI core classes
  description:        Basic user-interface components and related classes.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_graphics juce_data_structures
  OSXFrameworks:      Cocoa QuartzCore
  WeakOSXFrameworks:  Metal MetalKit
  iOSFrameworks:      CoreServices UIKit
  WeakiOSFrameworks:  Metal MetalKit
  mingwLibs:          dxgi

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_GUI_BASICS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_gui_basics 1
#include <juce_gui_basics/juce_gui_basics_config.h>

#include <juce_gui_basics/application/juce_Application.h>
#include <juce_gui_basics/accessibility/interfaces/juce_AccessibilityCellInterface.h>
#include <juce_gui_basics/accessibility/interfaces/juce_AccessibilityTableInterface.h>
#include <juce_gui_basics/accessibility/interfaces/juce_AccessibilityTextInterface.h>
#include <juce_gui_basics/accessibility/interfaces/juce_AccessibilityValueInterface.h>
#include <juce_gui_basics/accessibility/enums/juce_AccessibilityActions.h>
#include <juce_gui_basics/accessibility/enums/juce_AccessibilityEvent.h>
#include <juce_gui_basics/accessibility/enums/juce_AccessibilityRole.h>
#include <juce_gui_basics/accessibility/juce_AccessibilityState.h>
#include <juce_gui_basics/accessibility/juce_AccessibilityHandler.h>

#include <juce_gui_basics/buttons/juce_ToolbarButton.h>
#include <juce_gui_basics/buttons/juce_Button.h>
#include <juce_gui_basics/buttons/juce_ArrowButton.h>
#include <juce_gui_basics/buttons/juce_DrawableButton.h>
#include <juce_gui_basics/buttons/juce_HyperlinkButton.h>
#include <juce_gui_basics/buttons/juce_ImageButton.h>
#include <juce_gui_basics/buttons/juce_ShapeButton.h>
#include <juce_gui_basics/buttons/juce_TextButton.h>
#include <juce_gui_basics/buttons/juce_ToggleButton.h>

#include <juce_gui_basics/commands/juce_ApplicationCommandID.h>
#include <juce_gui_basics/commands/juce_ApplicationCommandInfo.h>
#include <juce_gui_basics/commands/juce_ApplicationCommandTarget.h>
#include <juce_gui_basics/commands/juce_ApplicationCommandManager.h>
#include <juce_gui_basics/commands/juce_KeyPressMappingSet.h>

#include <juce_gui_basics/components/juce_ComponentTraverser.h>
#include <juce_gui_basics/components/juce_FocusTraverser.h>
#include <juce_gui_basics/components/juce_ModalComponentManager.h>
#include <juce_gui_basics/components/juce_ComponentListener.h>
#include <juce_gui_basics/components/juce_CachedComponentImage.h>
#include <juce_gui_basics/components/juce_Component.h>

#include <juce_gui_basics/desktop/juce_Desktop.h>
#include <juce_gui_basics/desktop/juce_Displays.h>

#include <juce_gui_basics/drawables/juce_Drawable.h>
#include <juce_gui_basics/drawables/juce_DrawableShape.h>
#include <juce_gui_basics/drawables/juce_DrawableComposite.h>
#include <juce_gui_basics/drawables/juce_DrawableImage.h>
#include <juce_gui_basics/drawables/juce_DrawablePath.h>
#include <juce_gui_basics/drawables/juce_DrawableRectangle.h>
#include <juce_gui_basics/drawables/juce_DrawableText.h>

#include <juce_gui_basics/filebrowser/juce_FileBrowserListener.h>
#include <juce_gui_basics/filebrowser/juce_DirectoryContentsList.h>
#include <juce_gui_basics/filebrowser/juce_DirectoryContentsDisplayComponent.h>
#include <juce_gui_basics/filebrowser/juce_FileBrowserComponent.h>
#include <juce_gui_basics/filebrowser/juce_FileChooser.h>
#include <juce_gui_basics/filebrowser/juce_FileChooserDialogBox.h>
#include <juce_gui_basics/filebrowser/juce_FileListComponent.h>
#include <juce_gui_basics/filebrowser/juce_FilenameComponent.h>
#include <juce_gui_basics/filebrowser/juce_FilePreviewComponent.h>
#include <juce_gui_basics/filebrowser/juce_FileSearchPathListComponent.h>
#include <juce_gui_basics/filebrowser/juce_FileTreeComponent.h>
#include <juce_gui_basics/filebrowser/juce_ImagePreviewComponent.h>
#include <juce_gui_basics/filebrowser/juce_ContentSharer.h>

#include <juce_gui_basics/keyboard/juce_CaretComponent.h>
#include <juce_gui_basics/keyboard/juce_KeyboardFocusTraverser.h>
#include <juce_gui_basics/keyboard/juce_KeyPress.h>
#include <juce_gui_basics/keyboard/juce_KeyListener.h>
#include <juce_gui_basics/keyboard/juce_ModifierKeys.h>
#include <juce_gui_basics/keyboard/juce_SystemClipboard.h>
#include <juce_gui_basics/keyboard/juce_TextEditorKeyMapper.h>
#include <juce_gui_basics/keyboard/juce_TextInputTarget.h>

#include <juce_gui_basics/layout/juce_ComponentAnimator.h>
#include <juce_gui_basics/layout/juce_ComponentBoundsConstrainer.h>
#include <juce_gui_basics/layout/juce_FlexItem.h>
#include <juce_gui_basics/layout/juce_FlexBox.h>
#include <juce_gui_basics/layout/juce_GridItem.h>
#include <juce_gui_basics/layout/juce_Grid.h>
#include <juce_gui_basics/layout/juce_AnimatedPosition.h>
#include <juce_gui_basics/layout/juce_AnimatedPositionBehaviours.h>
#include <juce_gui_basics/layout/juce_ComponentBuilder.h>
#include <juce_gui_basics/layout/juce_ComponentMovementWatcher.h>
#include <juce_gui_basics/layout/juce_ConcertinaPanel.h>
#include <juce_gui_basics/layout/juce_GroupComponent.h>
#include <juce_gui_basics/layout/juce_ResizableBorderComponent.h>
#include <juce_gui_basics/layout/juce_ResizableCornerComponent.h>
#include <juce_gui_basics/layout/juce_ResizableEdgeComponent.h>
#include <juce_gui_basics/layout/juce_ScrollBar.h>
#include <juce_gui_basics/layout/juce_StretchableLayoutManager.h>
#include <juce_gui_basics/layout/juce_StretchableLayoutResizerBar.h>
#include <juce_gui_basics/layout/juce_StretchableObjectResizer.h>
#include <juce_gui_basics/layout/juce_TabbedButtonBar.h>
#include <juce_gui_basics/layout/juce_TabbedComponent.h>
#include <juce_gui_basics/layout/juce_Viewport.h>
#include <juce_gui_basics/layout/juce_MultiDocumentPanel.h>
#include <juce_gui_basics/layout/juce_SidePanel.h>

#include <juce_gui_basics/lookandfeel/juce_LookAndFeel.h>
#include <juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.h>
#include <juce_gui_basics/lookandfeel/juce_LookAndFeel_V1.h>
#include <juce_gui_basics/lookandfeel/juce_LookAndFeel_V3.h>
#include <juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.h>

#include <juce_gui_basics/menus/juce_BurgerMenuComponent.h>
#include <juce_gui_basics/menus/juce_PopupMenu.h>
#include <juce_gui_basics/menus/juce_MenuBarModel.h>
#include <juce_gui_basics/menus/juce_MenuBarComponent.h>

#include <juce_gui_basics/misc/juce_DropShadower.h>
#include <juce_gui_basics/misc/juce_FocusOutline.h>
#include <juce_gui_basics/misc/juce_JUCESplashScreen.h>
#include <juce_gui_basics/misc/juce_BubbleComponent.h>

#include <juce_gui_basics/mouse/juce_getMouseEventTime.h>
#include <juce_gui_basics/mouse/juce_LassoComponent.h>
#include <juce_gui_basics/mouse/juce_ComponentDragger.h>
#include <juce_gui_basics/mouse/juce_DragAndDropTarget.h>
#include <juce_gui_basics/mouse/juce_DragAndDropContainer.h>
#include <juce_gui_basics/mouse/juce_FileDragAndDropTarget.h>
#include <juce_gui_basics/mouse/juce_MouseCursor.h>
#include <juce_gui_basics/mouse/juce_MouseListener.h>
#include <juce_gui_basics/mouse/juce_MouseInputSource.h>
#include <juce_gui_basics/mouse/juce_MouseEvent.h>
#include <juce_gui_basics/mouse/juce_MouseInactivityDetector.h>
#include <juce_gui_basics/mouse/juce_SelectedItemSet.h>
#include <juce_gui_basics/mouse/juce_TextDragAndDropTarget.h>
#include <juce_gui_basics/mouse/juce_TooltipClient.h>

#include <juce_gui_basics/native/juce_ScopedDPIAwarenessDisabler.h>

#include <juce_gui_basics/positioning/juce_RelativeCoordinate.h>
#include <juce_gui_basics/positioning/juce_MarkerList.h>
#include <juce_gui_basics/positioning/juce_RelativePoint.h>
#include <juce_gui_basics/positioning/juce_RelativeRectangle.h>
#include <juce_gui_basics/positioning/juce_RelativeCoordinatePositioner.h>
#include <juce_gui_basics/positioning/juce_RelativeParallelogram.h>
#include <juce_gui_basics/positioning/juce_RelativePointPath.h>

#include <juce_gui_basics/properties/juce_PropertyComponent.h>
#include <juce_gui_basics/properties/juce_BooleanPropertyComponent.h>
#include <juce_gui_basics/properties/juce_ButtonPropertyComponent.h>
#include <juce_gui_basics/properties/juce_ChoicePropertyComponent.h>
#include <juce_gui_basics/properties/juce_PropertyPanel.h>
#include <juce_gui_basics/properties/juce_SliderPropertyComponent.h>
#include <juce_gui_basics/properties/juce_TextPropertyComponent.h>
#include <juce_gui_basics/properties/juce_MultiChoicePropertyComponent.h>

#include <juce_gui_basics/widgets/juce_TextEditor.h>
#include <juce_gui_basics/widgets/juce_Label.h>
#include <juce_gui_basics/widgets/juce_ComboBox.h>
#include <juce_gui_basics/widgets/juce_ImageComponent.h>
#include <juce_gui_basics/widgets/juce_ListBox.h>
#include <juce_gui_basics/widgets/juce_ProgressBar.h>
#include <juce_gui_basics/widgets/juce_Slider.h>
#include <juce_gui_basics/widgets/juce_TableHeaderComponent.h>
#include <juce_gui_basics/widgets/juce_TableListBox.h>
#include <juce_gui_basics/widgets/juce_Toolbar.h>
#include <juce_gui_basics/widgets/juce_ToolbarItemComponent.h>
#include <juce_gui_basics/widgets/juce_ToolbarItemFactory.h>
#include <juce_gui_basics/widgets/juce_ToolbarItemPalette.h>
#include <juce_gui_basics/widgets/juce_TreeView.h>

#include <juce_gui_basics/windows/juce_TopLevelWindow.h>
#include <juce_gui_basics/windows/juce_MessageBoxOptions.h>
#include <juce_gui_basics/windows/juce_AlertWindow.h>
#include <juce_gui_basics/windows/juce_CallOutBox.h>
#include <juce_gui_basics/windows/juce_ComponentPeer.h>
#include <juce_gui_basics/windows/juce_ResizableWindow.h>
#include <juce_gui_basics/windows/juce_DocumentWindow.h>
#include <juce_gui_basics/windows/juce_DialogWindow.h>
#include <juce_gui_basics/windows/juce_NativeMessageBox.h>
#include <juce_gui_basics/windows/juce_ThreadWithProgressWindow.h>
#include <juce_gui_basics/windows/juce_TooltipWindow.h>
#include <juce_gui_basics/windows/juce_VBlankAttachement.h>

/*
   #if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
   Image createSnapshotOfNativeWindow (void* nativeWindowHandle);
   #endif

#if JUCE_LINUX || JUCE_BSD
 #if JUCE_GUI_BASICS_INCLUDE_XHEADERS
  // If you're missing these headers, you need to install the libx11-dev package
  #include <X11/Xlib.h>
  #include <X11/Xatom.h>
  #include <X11/Xresource.h>
  #include <X11/Xutil.h>
  #include <X11/Xmd.h>
  #include <X11/keysym.h>
  #include <X11/XKBlib.h>
  #include <X11/cursorfont.h>
  #include <unistd.h>

  #if JUCE_USE_XRANDR
   // If you're missing this header, you need to install the libxrandr-dev package
   #include <X11/extensions/Xrandr.h>
  #endif

  #if JUCE_USE_XINERAMA
   // If you're missing this header, you need to install the libxinerama-dev package
   #include <X11/extensions/Xinerama.h>
  #endif

  #if JUCE_USE_XSHM
   #include <X11/extensions/XShm.h>
   #include <sys/shm.h>
   #include <sys/ipc.h>
  #endif

  #if JUCE_USE_XRENDER
   // If you're missing these headers, you need to install the libxrender-dev and libxcomposite-dev packages
   #include <X11/extensions/Xrender.h>
   #include <X11/extensions/Xcomposite.h>
  #endif

  #if JUCE_USE_XCURSOR
   // If you're missing this header, you need to install the libxcursor-dev package
   #include <X11/Xcursor/Xcursor.h>
  #endif

  #undef SIZEOF
  #undef KeyPress

  #include "native/x11/juce_linux_XWindowSystem.h"
  #include "native/x11/juce_linux_X11_Symbols.h"
 #endif
#endif

#if JUCE_GUI_BASICS_INCLUDE_SCOPED_THREAD_DPI_AWARENESS_SETTER && JUCE_WINDOWS
 #include "native/juce_win32_ScopedThreadDPIAwarenessSetter.h"
#endif


*/
