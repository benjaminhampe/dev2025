set(LIB_NAME de_juce_gui_basics)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_gui_basics)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_gui_basics)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_gui_basics.h
      ${LIB_HEADER_DIR}/juce_gui_basics_config.h
      ${LIB_HEADER_DIR}/accessibility/enums/juce_AccessibilityActions.h
      ${LIB_HEADER_DIR}/accessibility/enums/juce_AccessibilityEvent.h
      ${LIB_HEADER_DIR}/accessibility/enums/juce_AccessibilityRole.h
      ${LIB_HEADER_DIR}/accessibility/interfaces/juce_AccessibilityCellInterface.h
      ${LIB_HEADER_DIR}/accessibility/interfaces/juce_AccessibilityTableInterface.h
      ${LIB_HEADER_DIR}/accessibility/interfaces/juce_AccessibilityTextInterface.h
      ${LIB_HEADER_DIR}/accessibility/interfaces/juce_AccessibilityValueInterface.h
      ${LIB_HEADER_DIR}/accessibility/juce_AccessibilityHandler.h
      ${LIB_HEADER_DIR}/accessibility/juce_AccessibilityState.h
      ${LIB_HEADER_DIR}/application/juce_Application.h
      ${LIB_HEADER_DIR}/buttons/juce_ArrowButton.h
      ${LIB_HEADER_DIR}/buttons/juce_Button.h
      ${LIB_HEADER_DIR}/buttons/juce_DrawableButton.h
      ${LIB_HEADER_DIR}/buttons/juce_HyperlinkButton.h
      ${LIB_HEADER_DIR}/buttons/juce_ImageButton.h
      ${LIB_HEADER_DIR}/buttons/juce_ShapeButton.h
      ${LIB_HEADER_DIR}/buttons/juce_TextButton.h
      ${LIB_HEADER_DIR}/buttons/juce_ToggleButton.h
      ${LIB_HEADER_DIR}/buttons/juce_ToolbarButton.h
      ${LIB_HEADER_DIR}/commands/juce_ApplicationCommandID.h
      ${LIB_HEADER_DIR}/commands/juce_ApplicationCommandInfo.h
      ${LIB_HEADER_DIR}/commands/juce_ApplicationCommandManager.h
      ${LIB_HEADER_DIR}/commands/juce_ApplicationCommandTarget.h
      ${LIB_HEADER_DIR}/commands/juce_KeyPressMappingSet.h
      ${LIB_HEADER_DIR}/components/juce_CachedComponentImage.h
      ${LIB_HEADER_DIR}/components/juce_Component.h
      ${LIB_HEADER_DIR}/components/juce_ComponentListener.h
      ${LIB_HEADER_DIR}/components/juce_ComponentTraverser.h
      ${LIB_HEADER_DIR}/components/juce_FocusTraverser.h
      ${LIB_HEADER_DIR}/components/juce_ModalComponentManager.h
      ${LIB_HEADER_DIR}/components/juce_ScalingHelpers.h
      ${LIB_HEADER_DIR}/desktop/juce_Desktop.h
      ${LIB_HEADER_DIR}/desktop/juce_Displays.h
      ${LIB_HEADER_DIR}/drawables/juce_Drawable.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawableComposite.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawableImage.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawablePath.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawableRectangle.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawableShape.h
      ${LIB_HEADER_DIR}/drawables/juce_DrawableText.h
      ${LIB_HEADER_DIR}/filebrowser/juce_createIconForFile.h
      ${LIB_HEADER_DIR}/filebrowser/juce_ContentSharer.h
      ${LIB_HEADER_DIR}/filebrowser/juce_DirectoryContentsDisplayComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_DirectoryContentsList.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileBrowserComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileBrowserListener.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileChooser.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileChooserDialogBox.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileListComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FilenameComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FilePreviewComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileSearchPathListComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_FileTreeComponent.h
      ${LIB_HEADER_DIR}/filebrowser/juce_ImagePreviewComponent.h
      ${LIB_HEADER_DIR}/keyboard/juce_CaretComponent.h
      ${LIB_HEADER_DIR}/keyboard/juce_KeyboardFocusTraverser.h
      ${LIB_HEADER_DIR}/keyboard/juce_KeyListener.h
      ${LIB_HEADER_DIR}/keyboard/juce_KeyPress.h
      ${LIB_HEADER_DIR}/keyboard/juce_ModifierKeys.h
      ${LIB_HEADER_DIR}/keyboard/juce_SystemClipboard.h
      ${LIB_HEADER_DIR}/keyboard/juce_TextEditorKeyMapper.h
      ${LIB_HEADER_DIR}/keyboard/juce_TextInputTarget.h
      ${LIB_HEADER_DIR}/layout/juce_AnimatedPosition.h
      ${LIB_HEADER_DIR}/layout/juce_AnimatedPositionBehaviours.h
      ${LIB_HEADER_DIR}/layout/juce_ComponentAnimator.h
      ${LIB_HEADER_DIR}/layout/juce_ComponentBoundsConstrainer.h
      ${LIB_HEADER_DIR}/layout/juce_ComponentBuilder.h
      ${LIB_HEADER_DIR}/layout/juce_ComponentMovementWatcher.h
      ${LIB_HEADER_DIR}/layout/juce_ConcertinaPanel.h
      ${LIB_HEADER_DIR}/layout/juce_FlexBox.h
      ${LIB_HEADER_DIR}/layout/juce_FlexItem.h
      ${LIB_HEADER_DIR}/layout/juce_Grid.h
      ${LIB_HEADER_DIR}/layout/juce_GridItem.h
      ${LIB_HEADER_DIR}/layout/juce_GroupComponent.h
      ${LIB_HEADER_DIR}/layout/juce_MultiDocumentPanel.h
      ${LIB_HEADER_DIR}/layout/juce_ResizableBorderComponent.h
      ${LIB_HEADER_DIR}/layout/juce_ResizableCornerComponent.h
      ${LIB_HEADER_DIR}/layout/juce_ResizableEdgeComponent.h
      ${LIB_HEADER_DIR}/layout/juce_ScrollBar.h
      ${LIB_HEADER_DIR}/layout/juce_SidePanel.h
      ${LIB_HEADER_DIR}/layout/juce_StretchableLayoutManager.h
      ${LIB_HEADER_DIR}/layout/juce_StretchableLayoutResizerBar.h
      ${LIB_HEADER_DIR}/layout/juce_StretchableObjectResizer.h
      ${LIB_HEADER_DIR}/layout/juce_TabbedButtonBar.h
      ${LIB_HEADER_DIR}/layout/juce_TabbedComponent.h
      ${LIB_HEADER_DIR}/layout/juce_Viewport.h
      ${LIB_HEADER_DIR}/lookandfeel/juce_LookAndFeel.h
      ${LIB_HEADER_DIR}/lookandfeel/juce_LookAndFeel_V1.h
      ${LIB_HEADER_DIR}/lookandfeel/juce_LookAndFeel_V2.h
      ${LIB_HEADER_DIR}/lookandfeel/juce_LookAndFeel_V3.h
      ${LIB_HEADER_DIR}/lookandfeel/juce_LookAndFeel_V4.h
      ${LIB_HEADER_DIR}/menus/juce_BurgerMenuComponent.h
      ${LIB_HEADER_DIR}/menus/juce_MenuBarComponent.h
      ${LIB_HEADER_DIR}/menus/juce_MenuBarModel.h
      ${LIB_HEADER_DIR}/menus/juce_PopupMenu.h
      ${LIB_HEADER_DIR}/misc/juce_BubbleComponent.h
      ${LIB_HEADER_DIR}/misc/juce_DropShadower.h
      ${LIB_HEADER_DIR}/misc/juce_FocusOutline.h
      ${LIB_HEADER_DIR}/misc/juce_JUCESplashScreen.h
      ${LIB_HEADER_DIR}/mouse/juce_ComponentDragger.h
      ${LIB_HEADER_DIR}/mouse/juce_CustomMouseCursorInfo.h
      ${LIB_HEADER_DIR}/mouse/juce_DragAndDropContainer.h
      ${LIB_HEADER_DIR}/mouse/juce_DragAndDropTarget.h
      ${LIB_HEADER_DIR}/mouse/juce_FileDragAndDropTarget.h
      ${LIB_HEADER_DIR}/mouse/juce_LassoComponent.h
      ${LIB_HEADER_DIR}/mouse/juce_MouseCursor.h
      ${LIB_HEADER_DIR}/mouse/juce_MouseEvent.h
      ${LIB_HEADER_DIR}/mouse/juce_MouseInactivityDetector.h
      ${LIB_HEADER_DIR}/mouse/juce_MouseInputSource.h
      ${LIB_HEADER_DIR}/mouse/juce_MouseListener.h
      ${LIB_HEADER_DIR}/mouse/juce_PointerState.h
      ${LIB_HEADER_DIR}/mouse/juce_SelectedItemSet.h
      ${LIB_HEADER_DIR}/mouse/juce_TextDragAndDropTarget.h
      ${LIB_HEADER_DIR}/mouse/juce_TooltipClient.h
      ${LIB_HEADER_DIR}/mouse/juce_getMouseEventTime.h
      # ${LIB_HEADER_DIR}/native/juce_mac_CGMetalLayerRenderer.h
      # ${LIB_HEADER_DIR}/native/juce_mac_PerScreenDisplayLinks.h
      ${LIB_HEADER_DIR}/native/juce_MultiTouchMapper.h
      ${LIB_HEADER_DIR}/native/juce_ScopedDPIAwarenessDisabler.h
      ${LIB_HEADER_DIR}/native/juce_win32_ScopedThreadDPIAwarenessSetter.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_AccessibilityTextHelpers.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_AccessibilityElement.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_ComInterfaces.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAExpandCollapseProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAGridItemProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAGridProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAHelpers.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAInvokeProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAProviderBase.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAProviders.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIARangeValueProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIASelectionProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIATextProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAToggleProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIATransformProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAValueProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_UIAWindowProvider.h
      # ${LIB_HEADER_DIR}/native/accessibility/juce_win32_WindowsUIAWrapper.h
      # ${LIB_HEADER_DIR}/native/x11/juce_linux_ScopedWindowAssociation.h
      # ${LIB_HEADER_DIR}/native/x11/juce_linux_X11_Symbols.h
      # ${LIB_HEADER_DIR}/native/x11/juce_linux_XWindowSystem.h
      ${LIB_HEADER_DIR}/positioning/juce_MarkerList.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativeCoordinate.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativeCoordinatePositioner.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativeParallelogram.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativePoint.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativePointPath.h
      ${LIB_HEADER_DIR}/positioning/juce_RelativeRectangle.h
      ${LIB_HEADER_DIR}/properties/juce_BooleanPropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_ButtonPropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_ChoicePropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_MultiChoicePropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_PropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_PropertyPanel.h
      ${LIB_HEADER_DIR}/properties/juce_SliderPropertyComponent.h
      ${LIB_HEADER_DIR}/properties/juce_TextPropertyComponent.h
      ${LIB_HEADER_DIR}/widgets/juce_ComboBox.h
      ${LIB_HEADER_DIR}/widgets/juce_ImageComponent.h
      ${LIB_HEADER_DIR}/widgets/juce_Label.h
      ${LIB_HEADER_DIR}/widgets/juce_ListBox.h
      ${LIB_HEADER_DIR}/widgets/juce_ProgressBar.h
      ${LIB_HEADER_DIR}/widgets/juce_Slider.h
      ${LIB_HEADER_DIR}/widgets/juce_TableHeaderComponent.h
      ${LIB_HEADER_DIR}/widgets/juce_TableListBox.h
      ${LIB_HEADER_DIR}/widgets/juce_TextEditor.h
      ${LIB_HEADER_DIR}/widgets/juce_Toolbar.h
      ${LIB_HEADER_DIR}/widgets/juce_ToolbarItemComponent.h
      ${LIB_HEADER_DIR}/widgets/juce_ToolbarItemFactory.h
      ${LIB_HEADER_DIR}/widgets/juce_ToolbarItemPalette.h
      ${LIB_HEADER_DIR}/widgets/juce_TreeView.h
      ${LIB_HEADER_DIR}/windows/juce_AlertWindow.h
      ${LIB_HEADER_DIR}/windows/juce_CallOutBox.h
      ${LIB_HEADER_DIR}/windows/juce_ComponentPeer.h
      ${LIB_HEADER_DIR}/windows/juce_DialogWindow.h
      ${LIB_HEADER_DIR}/windows/juce_DocumentWindow.h
      ${LIB_HEADER_DIR}/windows/juce_MessageBoxOptions.h
      ${LIB_HEADER_DIR}/windows/juce_NativeMessageBox.h
      ${LIB_HEADER_DIR}/windows/juce_ResizableWindow.h
      ${LIB_HEADER_DIR}/windows/juce_ThreadWithProgressWindow.h
      ${LIB_HEADER_DIR}/windows/juce_TooltipWindow.h
      ${LIB_HEADER_DIR}/windows/juce_TopLevelWindow.h
      ${LIB_HEADER_DIR}/windows/juce_VBlankAttachement.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_gui_basics.cpp
      ${LIB_SOURCE_DIR}/accessibility/juce_AccessibilityHandler.cpp
      ${LIB_SOURCE_DIR}/accessibility/juce_AccessibilityTextHelpers.h
      ${LIB_SOURCE_DIR}/accessibility/juce_AccessibilityTextHelpers_test.cpp
      ${LIB_SOURCE_DIR}/application/juce_Application.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_ArrowButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_Button.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_DrawableButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_HyperlinkButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_ImageButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_ShapeButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_TextButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_ToggleButton.cpp
      ${LIB_SOURCE_DIR}/buttons/juce_ToolbarButton.cpp
      ${LIB_SOURCE_DIR}/commands/juce_ApplicationCommandInfo.cpp
      ${LIB_SOURCE_DIR}/commands/juce_ApplicationCommandManager.cpp
      ${LIB_SOURCE_DIR}/commands/juce_ApplicationCommandTarget.cpp
      ${LIB_SOURCE_DIR}/commands/juce_KeyPressMappingSet.cpp
      ${LIB_SOURCE_DIR}/components/juce_Component.cpp
      ${LIB_SOURCE_DIR}/components/juce_ComponentHelpers.h
      ${LIB_SOURCE_DIR}/components/juce_ComponentListener.cpp
      ${LIB_SOURCE_DIR}/components/juce_FocusTraverser.cpp
      ${LIB_SOURCE_DIR}/components/juce_FocusHelpers.h
      ${LIB_SOURCE_DIR}/components/juce_FocusRestorer.h
      ${LIB_SOURCE_DIR}/components/juce_ModalComponentManager.cpp
      ${LIB_SOURCE_DIR}/desktop/juce_Desktop.cpp
      ${LIB_SOURCE_DIR}/desktop/juce_Displays.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_Drawable.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawableComposite.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawableImage.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawablePath.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawableRectangle.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawableShape.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_DrawableText.cpp
      ${LIB_SOURCE_DIR}/drawables/juce_SVGParser.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_ContentSharer.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_DirectoryContentsDisplayComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_DirectoryContentsList.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileBrowserComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileChooser.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileChooserDialogBox.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileListComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FilenameComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileSearchPathListComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_FileTreeComponent.cpp
      ${LIB_SOURCE_DIR}/filebrowser/juce_ImagePreviewComponent.cpp
      ${LIB_SOURCE_DIR}/keyboard/juce_CaretComponent.cpp
      ${LIB_SOURCE_DIR}/keyboard/juce_KeyboardFocusTraverser.cpp
      ${LIB_SOURCE_DIR}/keyboard/juce_KeyListener.cpp
      ${LIB_SOURCE_DIR}/keyboard/juce_KeyPress.cpp
      ${LIB_SOURCE_DIR}/keyboard/juce_ModifierKeys.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ComponentAnimator.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ComponentBoundsConstrainer.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ComponentBuilder.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ComponentMovementWatcher.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ConcertinaPanel.cpp
      ${LIB_SOURCE_DIR}/layout/juce_FlexBox.cpp
      ${LIB_SOURCE_DIR}/layout/juce_Grid.cpp
      ${LIB_SOURCE_DIR}/layout/juce_GridItem.cpp
      ${LIB_SOURCE_DIR}/layout/juce_GroupComponent.cpp
      ${LIB_SOURCE_DIR}/layout/juce_MultiDocumentPanel.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ResizableBorderComponent.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ResizableCornerComponent.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ResizableEdgeComponent.cpp
      ${LIB_SOURCE_DIR}/layout/juce_ScrollBar.cpp
      ${LIB_SOURCE_DIR}/layout/juce_SidePanel.cpp
      ${LIB_SOURCE_DIR}/layout/juce_StretchableLayoutManager.cpp
      ${LIB_SOURCE_DIR}/layout/juce_StretchableLayoutResizerBar.cpp
      ${LIB_SOURCE_DIR}/layout/juce_StretchableObjectResizer.cpp
      ${LIB_SOURCE_DIR}/layout/juce_TabbedButtonBar.cpp
      ${LIB_SOURCE_DIR}/layout/juce_TabbedComponent.cpp
      ${LIB_SOURCE_DIR}/layout/juce_Viewport.cpp
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeel.cpp
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeelHelpers.h
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeel_V1.cpp
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeel_V2.cpp
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeel_V3.cpp
      ${LIB_SOURCE_DIR}/lookandfeel/juce_LookAndFeel_V4.cpp
      ${LIB_SOURCE_DIR}/menus/juce_BurgerMenuComponent.cpp
      ${LIB_SOURCE_DIR}/menus/juce_MenuBarComponent.cpp
      ${LIB_SOURCE_DIR}/menus/juce_MenuBarModel.cpp
      ${LIB_SOURCE_DIR}/menus/juce_PopupMenu.cpp
      ${LIB_SOURCE_DIR}/misc/juce_BubbleComponent.cpp
      ${LIB_SOURCE_DIR}/misc/juce_DropShadower.cpp
      ${LIB_SOURCE_DIR}/misc/juce_FocusOutline.cpp
      ${LIB_SOURCE_DIR}/misc/juce_JUCESplashScreen.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_ComponentDragger.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_DragAndDropContainer.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseCursor.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseEvent.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseInactivityDetector.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseInputSource.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseInputSourceInternal.h
      ${LIB_SOURCE_DIR}/mouse/juce_MouseListener.cpp
      ${LIB_SOURCE_DIR}/mouse/juce_MouseInputSource_SourceList.h
      ${LIB_SOURCE_DIR}/mouse/juce_MouseInputSource_SourceList.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_ContentSharer.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_FileChooser.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_Windowing.cpp
      # ${LIB_SOURCE_DIR}/native/juce_ios_ContentSharer.cpp
      # ${LIB_SOURCE_DIR}/native/juce_ios_FileChooser.mm
      # ${LIB_SOURCE_DIR}/native/juce_ios_UIViewComponentPeer.mm
      # ${LIB_SOURCE_DIR}/native/juce_ios_Windowing.mm
      # ${LIB_SOURCE_DIR}/native/juce_linux_FileChooser.cpp
      # ${LIB_SOURCE_DIR}/native/juce_linux_Windowing.cpp
      # ${LIB_SOURCE_DIR}/native/juce_mac_FileChooser.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_MainMenu.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_MouseCursor.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_NSViewComponentPeer.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_Windowing.mm
      ${LIB_SOURCE_DIR}/native/juce_win32_WindowsUIAWrapper.h
      ${LIB_SOURCE_DIR}/native/juce_win32_ComInterfaces.h
      ${LIB_SOURCE_DIR}/native/juce_win32_UIAProviders.h
      ${LIB_SOURCE_DIR}/native/juce_win32_UIAHelpers.h
      ${LIB_SOURCE_DIR}/native/juce_win32_AccessibilityElement.h
      ${LIB_SOURCE_DIR}/native/juce_win32_AccessibilityElement.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Accessibility.h
      ${LIB_SOURCE_DIR}/native/juce_win32_Accessibility.cpp
      ${LIB_SOURCE_DIR}/native/juce_isWindowOnCurrentVirtualDesktop.h
      ${LIB_SOURCE_DIR}/native/juce_isWindowOnCurrentVirtualDesktop.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_MouseCursor.h
      ${LIB_SOURCE_DIR}/native/juce_win32_MouseCursor.cpp
      ${LIB_SOURCE_DIR}/native/juce_createSnapshotOfNativeWindow.cpp
      ${LIB_SOURCE_DIR}/native/juce_createSnapshotOfNativeWindow.h
      ${LIB_SOURCE_DIR}/native/juce_isWindowOnCurrentVirtualDesktop.cpp
      ${LIB_SOURCE_DIR}/native/juce_isWindowOnCurrentVirtualDesktop.h
      ${LIB_SOURCE_DIR}/native/juce_win32_BitmapImage.h
      ${LIB_SOURCE_DIR}/native/juce_win32_BitmapImage.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_ComponentPeer.h
      ${LIB_SOURCE_DIR}/native/juce_win32_ComponentPeer.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Desktop.h
      ${LIB_SOURCE_DIR}/native/juce_win32_Desktop.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_DPIAwareness.h
      ${LIB_SOURCE_DIR}/native/juce_win32_DPIAwareness.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_KeyPress.h
      ${LIB_SOURCE_DIR}/native/juce_win32_KeyPress.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Touch.h
      ${LIB_SOURCE_DIR}/native/juce_win32_Touch.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_DragAndDrop.h
      ${LIB_SOURCE_DIR}/native/juce_win32_DragAndDrop.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_FileChooser.h
      ${LIB_SOURCE_DIR}/native/juce_win32_FileChooser.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_UWPUIViewSettings.h
      ${LIB_SOURCE_DIR}/native/juce_win32_UWPUIViewSettings.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Windowing.h
      ${LIB_SOURCE_DIR}/native/juce_win32_Windowing.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_getMouseEventTime.cpp
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_AccessibilityTextHelpers_test.cpp
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_android_Accessibility.cpp
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_ios_Accessibility.mm
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_mac_Accessibility.mm
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_mac_AccessibilitySharedCode.mm
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_win32_Accessibility.cpp  # included in accessibility/juce_AccessibilityHandler.cpp
      # ${LIB_SOURCE_DIR}/native/accessibility/juce_win32_AccessibilityElement.cpp # included in accessibility/juce_AccessibilityHandler.cpp
      # ${LIB_SOURCE_DIR}/native/java/app/com/rmsl/juce/ComponentPeerView.java
      # ${LIB_SOURCE_DIR}/native/java/app/com/rmsl/juce/JuceContentProviderCursor.java
      # ${LIB_SOURCE_DIR}/native/java/app/com/rmsl/juce/JuceContentProviderFileObserver.java
      # ${LIB_SOURCE_DIR}/native/javaopt/app/com/rmsl/juce/JuceActivity.java
      # ${LIB_SOURCE_DIR}/native/javaopt/app/com/rmsl/juce/JuceSharingContentProvider.java
      # ${LIB_SOURCE_DIR}/native/x11/juce_linux_X11_DragAndDrop.cpp
      # ${LIB_SOURCE_DIR}/native/x11/juce_linux_X11_Symbols.cpp
      # ${LIB_SOURCE_DIR}/native/x11/juce_linux_XWindowSystem.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_MarkerList.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativeCoordinate.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativeCoordinatePositioner.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativeParallelogram.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativePoint.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativePointPath.cpp
      ${LIB_SOURCE_DIR}/positioning/juce_RelativeRectangle.cpp
      ${LIB_SOURCE_DIR}/properties/juce_BooleanPropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_ButtonPropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_ChoicePropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_MultiChoicePropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_PropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_PropertyPanel.cpp
      ${LIB_SOURCE_DIR}/properties/juce_SliderPropertyComponent.cpp
      ${LIB_SOURCE_DIR}/properties/juce_TextPropertyComponent.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_getListRowAccessibilityActions.h
      ${LIB_SOURCE_DIR}/widgets/juce_areThereAnyAlwaysOnTopWindows.h
      ${LIB_SOURCE_DIR}/widgets/juce_ComboBox.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_ImageComponent.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Label.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_ListBox.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_ProgressBar.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Slider.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_TableHeaderComponent.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_TableListBox.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_TextEditor.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Toolbar.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_ToolbarItemComponent.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_ToolbarItemComponent_ItemDragAndDropOverlayComponent.h
      ${LIB_SOURCE_DIR}/widgets/juce_ToolbarItemPalette.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_TreeView.cpp
      ${LIB_SOURCE_DIR}/windows/juce_AlertWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_AlertWindowMappings.h
      ${LIB_SOURCE_DIR}/windows/juce_CallOutBox.cpp
      ${LIB_SOURCE_DIR}/windows/juce_ComponentPeer.cpp
      ${LIB_SOURCE_DIR}/windows/juce_DialogWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_DocumentWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_ResizableWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_ThreadWithProgressWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_TooltipWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_TopLevelWindow.cpp
      ${LIB_SOURCE_DIR}/windows/juce_VBlankAttachement.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

	if (MSVC)
	else()
		target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
	endif()

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_graphics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_data_structures)

endif()

# add_subdirectory(test)

