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

#define JUCE_NATIVE_ACCESSIBILITY_INCLUDED 1

#include <juce_gui_basics/native/juce_win32_WindowsUIAWrapper.h>
#include <juce_gui_basics/native/juce_win32_UIAHelpers.h>
#include <juce_gui_basics/native/juce_win32_AccessibilityElement.h>

namespace juce
{

bool isStartingUpOrShuttingDown();

bool isHandlerValid (const AccessibilityHandler& handler);

bool areAnyAccessibilityClientsActive();

template <typename Callback>
void getProviderWithCheckedWrapper (const AccessibilityHandler& handler, Callback&& callback)
{
    if (! areAnyAccessibilityClientsActive() || isStartingUpOrShuttingDown() || ! isHandlerValid (handler))
        return;

    if (auto* uiaWrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
    {
        ComSmartPtr<IRawElementProviderSimple> provider;
        handler.getNativeImplementation()->QueryInterface (IID_PPV_ARGS (provider.resetAndGetPointerAddress()));

        callback (uiaWrapper, provider);
    }
}

void sendAccessibilityAutomationEvent (const AccessibilityHandler& handler, EVENTID event);

void sendAccessibilityPropertyChangedEvent (const AccessibilityHandler& handler, PROPERTYID property, VARIANT newValue);

void notifyAccessibilityEventInternal (const AccessibilityHandler& handler, InternalAccessibilityEvent eventType);

//==============================================================================
namespace WindowsAccessibility
{
   long getUiaRootObjectId();

   bool handleWmGetObject (AccessibilityHandler* handler, WPARAM wParam, LPARAM lParam, LRESULT* res);

   void revokeUIAMapEntriesForWindow (HWND hwnd);

} // WindowsAccessibility


//==============================================================================
class AccessibilityHandler::AccessibilityNativeImpl
{
public:
    explicit AccessibilityNativeImpl (AccessibilityHandler& owner)
        : accessibilityElement (new AccessibilityNativeHandle (owner))
    {
        ++providerCount;
    }

    ~AccessibilityNativeImpl()
    {
        ComSmartPtr<IRawElementProviderSimple> provider;
        accessibilityElement->QueryInterface (IID_PPV_ARGS (provider.resetAndGetPointerAddress()));

        accessibilityElement->invalidateElement();
        --providerCount;

        if (auto* uiaWrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
        {
            uiaWrapper->disconnectProvider (provider);

            if (providerCount == 0 && JUCEApplicationBase::isStandaloneApp())
                uiaWrapper->disconnectAllProviders();
        }
    }

    //==============================================================================
    ComSmartPtr<AccessibilityNativeHandle> accessibilityElement;
    static int providerCount;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeImpl)
};

} // namespace juce
