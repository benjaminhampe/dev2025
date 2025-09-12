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
#include <juce_gui_basics/accessibility/juce_AccessibilityHandler.h>

namespace juce
{

template <typename RowComponentType>
static AccessibilityActions getListRowAccessibilityActions (RowComponentType& rowComponent)
{
    auto onFocus = [&rowComponent]
    {
        rowComponent.owner.scrollToEnsureRowIsOnscreen (rowComponent.row);
        rowComponent.owner.selectRow (rowComponent.row);
    };

    auto onPress = [&rowComponent, onFocus]
    {
        onFocus();
        rowComponent.owner.keyPressed (KeyPress (KeyPress::returnKey));
    };

    auto onToggle = [&rowComponent]
    {
        rowComponent.owner.flipRowSelection (rowComponent.row);
    };

    return AccessibilityActions().addAction (AccessibilityActionType::focus,  std::move (onFocus))
                                 .addAction (AccessibilityActionType::press,  std::move (onPress))
                                 .addAction (AccessibilityActionType::toggle, std::move (onToggle));
}

} // namespace juce
