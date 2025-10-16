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

  ID:                     juce_gui_extra
  vendor:                 juce
  version:                7.0.5
  name:                   JUCE extended GUI classes
  description:            Miscellaneous GUI classes for specialised tasks.
  website:                http://www.juce.com/juce
  license:                GPL/Commercial
  minimumCppStandard:     17

  dependencies:           juce_gui_basics
  OSXFrameworks:          WebKit
  iOSFrameworks:          WebKit
  WeakiOSFrameworks:      UserNotifications
  WeakMacOSFrameworks:    UserNotifications

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_GUI_EXTRA_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_gui_extra 1

#include <juce_gui_extra/juce_gui_extra_config.h>

//==============================================================================
#include <juce_gui_extra/documents/juce_FileBasedDocument.h>
#include <juce_gui_extra/code_editor/juce_CodeDocument.h>
#include <juce_gui_extra/code_editor/juce_CodeEditorComponent.h>
#include <juce_gui_extra/code_editor/juce_CodeTokeniser.h>
#include <juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniser.h>
#include <juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniserFunctions.h>
#include <juce_gui_extra/code_editor/juce_XMLCodeTokeniser.h>
#include <juce_gui_extra/code_editor/juce_LuaCodeTokeniser.h>
#include <juce_gui_extra/embedding/juce_ActiveXControlComponent.h>
//#include <juce_gui_extra/embedding/juce_AndroidViewComponent.h>
//#include <juce_gui_extra/embedding/juce_NSViewComponent.h>
#include <juce_gui_extra/embedding/juce_UIViewComponent.h>
//#include <juce_gui_extra/embedding/juce_XEmbedComponent.h>
#include <juce_gui_extra/embedding/juce_HWNDComponent.h>
//#include <juce_gui_extra/misc/juce_AppleRemote.h>
#include <juce_gui_extra/misc/juce_BubbleMessageComponent.h>
#include <juce_gui_extra/misc/juce_ColourSelector.h>
#include <juce_gui_extra/misc/juce_KeyMappingEditorComponent.h>
#include <juce_gui_extra/misc/juce_PreferencesPanel.h>
#include <juce_gui_extra/misc/juce_PushNotifications.h>
#include <juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h>
#include <juce_gui_extra/misc/juce_SplashScreen.h>
#include <juce_gui_extra/misc/juce_SystemTrayIconComponent.h>
#include <juce_gui_extra/misc/juce_WebBrowserComponent.h>
#include <juce_gui_extra/misc/juce_LiveConstantEditor.h>
#include <juce_gui_extra/misc/juce_AnimatedAppComponent.h>
