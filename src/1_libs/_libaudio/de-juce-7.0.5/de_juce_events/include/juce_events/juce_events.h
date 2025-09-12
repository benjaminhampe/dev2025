/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

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

  ID:                 juce_events
  vendor:             juce
  version:            7.0.5
  name:               JUCE message and event handling classes
  description:        Classes for running an application's main event loop and sending/receiving messages, timers, etc.
  website:            http://www.juce.com/juce
  license:            ISC
  minimumCppStandard: 17

  dependencies:       juce_core

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_EVENTS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_events 1
#include <juce_events/juce_events_config.h>

#include <juce_events/messages/juce_MessageManager.h>
#include <juce_events/messages/juce_Message.h>
#include <juce_events/messages/juce_MessageListener.h>
#include <juce_events/messages/juce_CallbackMessage.h>
#include <juce_events/messages/juce_DeletedAtShutdown.h>
#include <juce_events/messages/juce_NotificationType.h>
#include <juce_events/messages/juce_ApplicationBase.h>
#include <juce_events/messages/juce_Initialisation.h>
#include <juce_events/messages/juce_MountedVolumeListChangeDetector.h>
#include <juce_events/broadcasters/juce_ActionBroadcaster.h>
#include <juce_events/broadcasters/juce_ActionListener.h>
#include <juce_events/broadcasters/juce_AsyncUpdater.h>
#include <juce_events/broadcasters/juce_ChangeListener.h>
#include <juce_events/broadcasters/juce_ChangeBroadcaster.h>
#include <juce_events/timers/juce_Timer.h>
#include <juce_events/timers/juce_MultiTimer.h>
#include <juce_events/interprocess/juce_InterprocessConnection.h>
#include <juce_events/interprocess/juce_InterprocessConnectionServer.h>
#include <juce_events/interprocess/juce_ConnectedChildProcess.h>
#include <juce_events/interprocess/juce_NetworkServiceDiscovery.h>
#include <juce_events/native/juce_ScopedLowPowerModeDisabler.h>

/*
#if JUCE_LINUX || JUCE_BSD
 #include <juce_events/native/juce_linux_EventLoop.h>
#endif

#if JUCE_WINDOWS
 #if JUCE_EVENTS_INCLUDE_WINRT_WRAPPER
  #include <juce_events/native/juce_win32_WinRTWrapper.h>
 #endif
#endif
*/