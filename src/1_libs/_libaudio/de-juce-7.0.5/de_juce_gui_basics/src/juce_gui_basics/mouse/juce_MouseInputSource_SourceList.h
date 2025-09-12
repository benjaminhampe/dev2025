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

#pragma once
#include <juce_gui_basics/mouse/juce_MouseInputSource.h>

namespace juce
{

class MouseInputSource::SourceList : public Timer
{
public:
    SourceList();
    bool addSource();
    bool canUseTouch();
    MouseInputSource* addSource (int index, InputSourceType type);
    MouseInputSource* getMouseSource (int index) noexcept;
    MouseInputSource* getOrCreateMouseInputSource (InputSourceType type, int touchIndex = 0);
    int getNumDraggingMouseSources() const noexcept;
    MouseInputSource* getDraggingMouseSource (int index) noexcept;
    void beginDragAutoRepeat (int interval);
    void timerCallback() override;
    OwnedArray<MouseInputSourceInternal> sources;
    Array<MouseInputSource> sourceArray;
    //OwnedArray<MouseInputSource> sourceArray;
};

} // namespace juce
