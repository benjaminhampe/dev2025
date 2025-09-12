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
#include <juce_gui_basics/components/juce_Component.h>
#include <juce_gui_basics/windows/juce_ComponentPeer.h>
#include <juce_gui_basics/accessibility/juce_AccessibilityTextHelpers.h>

#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>
#include <juce_gui_basics/native/juce_win32_WindowsUIAWrapper.h>
#include <juce_gui_basics/native/juce_win32_UIAHelpers.h>
#include <juce_gui_basics/native/juce_win32_AccessibilityElement.h>

namespace juce
{

void sendAccessibilityAutomationEvent (const AccessibilityHandler&, EVENTID);
void sendAccessibilityPropertyChangedEvent (const AccessibilityHandler&, PROPERTYID, VARIANT);

template <typename MemberFn>
static const AccessibilityHandler*
getEnclosingHandlerWithInterface (const AccessibilityHandler* handler, MemberFn fn)
{
  if (handler == nullptr)
      return nullptr;

  if ((handler->*fn)() != nullptr)
      return handler;

  return getEnclosingHandlerWithInterface (handler->getParent(), fn);
}

//==============================================================================
class UIAProviderBase
{
public:
    explicit UIAProviderBase (AccessibilityNativeHandle* nativeHandleIn)
        : nativeHandle (nativeHandleIn)
    {
    }

    bool isElementValid() const
    {
        if (nativeHandle != nullptr)
            return nativeHandle->isElementValid();

        return false;
    }

    const AccessibilityHandler& getHandler() const
    {
        return nativeHandle->getHandler();
    }

private:
    ComSmartPtr<AccessibilityNativeHandle> nativeHandle;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAProviderBase)
};


//==============================================================================
class UIAExpandCollapseProvider  : public UIAProviderBase,
                                   public ComBaseClassHelper<ComTypes::IExpandCollapseProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT Expand() override
    {
        return invokeShowMenu();
    }

    JUCE_COMRESULT Collapse() override
    {
        return invokeShowMenu();
    }

    JUCE_COMRESULT get_ExpandCollapseState (ComTypes::ExpandCollapseState* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = getHandler().getCurrentState().isExpanded()
                           ? ComTypes::ExpandCollapseState_Expanded
                           : ComTypes::ExpandCollapseState_Collapsed;

            return S_OK;
        });
    }

private:
    JUCE_COMRESULT invokeShowMenu()
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();

        if (handler.getActions().invoke (AccessibilityActionType::showMenu))
        {
            using namespace ComTypes::Constants;

            sendAccessibilityAutomationEvent (handler, handler.getCurrentState().isExpanded()
                                                           ? UIA_MenuOpenedEventId
                                                           : UIA_MenuClosedEventId);

            return S_OK;
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAExpandCollapseProvider)
};


//==============================================================================
class UIAGridItemProvider  : public UIAProviderBase,
                             public ComBaseClassHelper<ComTypes::IGridItemProvider, ComTypes::ITableItemProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT get_Row (int* pRetVal) override
    {
        return withTableSpan (pRetVal,
                              &AccessibilityTableInterface::getRowSpan,
                              &AccessibilityTableInterface::Span::begin);
    }

    JUCE_COMRESULT get_Column (int* pRetVal) override
    {
        return withTableSpan (pRetVal,
                              &AccessibilityTableInterface::getColumnSpan,
                              &AccessibilityTableInterface::Span::begin);
    }

    JUCE_COMRESULT get_RowSpan (int* pRetVal) override
    {
        return withTableSpan (pRetVal,
                              &AccessibilityTableInterface::getRowSpan,
                              &AccessibilityTableInterface::Span::num);
    }

    JUCE_COMRESULT get_ColumnSpan (int* pRetVal) override
    {
        return withTableSpan (pRetVal,
                              &AccessibilityTableInterface::getColumnSpan,
                              &AccessibilityTableInterface::Span::num);
    }

    JUCE_COMRESULT get_ContainingGrid (IRawElementProviderSimple** pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityHandler& tableHandler)
        {
            JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")
            tableHandler.getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
            JUCE_END_IGNORE_WARNINGS_GCC_LIKE

            return true;
        });
    }

    JUCE_COMRESULT GetRowHeaderItems (SAFEARRAY**) override
    {
        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT GetColumnHeaderItems (SAFEARRAY** pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityHandler& tableHandler)
        {
            if (auto* tableInterface = tableHandler.getTableInterface())
            {
                if (const auto column = tableInterface->getColumnSpan (getHandler()))
                {
                    if (auto* header = tableInterface->getHeaderHandler())
                    {
                        const auto children = header->getChildren();

                        if (isPositiveAndBelow (column->begin, children.size()))
                        {
                            IRawElementProviderSimple* provider = nullptr;

                            if (auto* child = children[(size_t) column->begin])
                            {
                                JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")
                                if (child->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (&provider)) == S_OK && provider != nullptr)
                                {
                                    *pRetVal = SafeArrayCreateVector (VT_UNKNOWN, 0, 1);
                                    LONG index = 0;
                                    const auto hr = SafeArrayPutElement (*pRetVal, &index, provider);

                                    return ! FAILED (hr);
                                }
                                JUCE_END_IGNORE_WARNINGS_GCC_LIKE
                            }
                        }
                    }
                }
            }

            return false;
        });
    }
private:
    template <typename Value, typename Callback>
    JUCE_COMRESULT withTableInterface (Value* pRetVal, Callback&& callback) const
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* handler = getEnclosingHandlerWithInterface (&getHandler(), &AccessibilityHandler::getTableInterface))
                if (handler->getTableInterface() != nullptr && callback (*handler))
                    return S_OK;

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT withTableSpan (int* pRetVal,
                                  Optional<AccessibilityTableInterface::Span> (AccessibilityTableInterface::* getSpan) (const AccessibilityHandler&) const,
                                  int AccessibilityTableInterface::Span::* spanMember) const
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityHandler& handler)
        {
            if (const auto span = ((handler.getTableInterface())->*getSpan) (getHandler()))
            {
                *pRetVal = (*span).*spanMember;
                return true;
            }

            return false;
        });
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAGridItemProvider)
};


//==============================================================================
class UIAGridProvider  : public UIAProviderBase,
                         public ComBaseClassHelper<ComTypes::IGridProvider, ComTypes::ITableProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT GetItem (int row, int column, IRawElementProviderSimple** pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityTableInterface& tableInterface)
        {
            if (! isPositiveAndBelow (row, tableInterface.getNumRows())
                || ! isPositiveAndBelow (column, tableInterface.getNumColumns()))
                return E_INVALIDARG;

            JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")

            if (auto* cellHandler = tableInterface.getCellHandler (row, column))
            {
                cellHandler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
                return S_OK;
            }

            if (auto* rowHandler = tableInterface.getRowHandler (row))
            {
                rowHandler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
                return S_OK;
            }

            JUCE_END_IGNORE_WARNINGS_GCC_LIKE

            return E_FAIL;
        });
    }

    JUCE_COMRESULT get_RowCount (int* pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityTableInterface& tableInterface)
        {
            *pRetVal = tableInterface.getNumRows();
            return S_OK;
        });
    }

    JUCE_COMRESULT get_ColumnCount (int* pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityTableInterface& tableInterface)
        {
            *pRetVal = tableInterface.getNumColumns();
            return S_OK;
        });
    }

    JUCE_COMRESULT GetRowHeaders (SAFEARRAY**) override
    {
        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT GetColumnHeaders (SAFEARRAY** pRetVal) override
    {
        return withTableInterface (pRetVal, [&] (const AccessibilityTableInterface& tableInterface)
        {
            if (auto* header = tableInterface.getHeaderHandler())
            {
                const auto children = header->getChildren();

                *pRetVal = SafeArrayCreateVector (VT_UNKNOWN, 0, (ULONG) children.size());

                LONG index = 0;

                for (const auto& child : children)
                {
                    IRawElementProviderSimple* provider = nullptr;

                    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")
                    if (child != nullptr)
                        child->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (&provider));
                    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

                    if (provider == nullptr)
                        return E_FAIL;

                    const auto hr = SafeArrayPutElement (*pRetVal, &index, provider);

                    if (FAILED (hr))
                        return E_FAIL;

                    ++index;
                }

                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_RowOrColumnMajor (ComTypes::RowOrColumnMajor* pRetVal) override
    {
        *pRetVal = ComTypes::RowOrColumnMajor_RowMajor;
        return S_OK;
    }

private:
    template <typename Value, typename Callback>
    JUCE_COMRESULT withTableInterface (Value* pRetVal, Callback&& callback) const
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* tableHandler = getEnclosingHandlerWithInterface (&getHandler(), &AccessibilityHandler::getTableInterface))
                if (auto* tableInterface = tableHandler->getTableInterface())
                    return callback (*tableInterface);

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAGridProvider)
};


//==============================================================================
class UIAInvokeProvider  : public UIAProviderBase,
                           public ComBaseClassHelper<ComTypes::IInvokeProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT Invoke() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();

        if (handler.getActions().invoke (AccessibilityActionType::press))
        {
            using namespace ComTypes::Constants;

            if (isElementValid())
                sendAccessibilityAutomationEvent (handler, UIA_Invoke_InvokedEventId);

            return S_OK;
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAInvokeProvider)
};


//==============================================================================
class UIARangeValueProvider  : public UIAProviderBase,
                               public ComBaseClassHelper<ComTypes::IRangeValueProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT SetValue (double val) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();

        if (auto* valueInterface = handler.getValueInterface())
        {
            auto range = valueInterface->getRange();

            if (range.isValid())
            {
                if (val < range.getMinimumValue() || val > range.getMaximumValue())
                    return E_INVALIDARG;

                if (! valueInterface->isReadOnly())
                {
                    valueInterface->setValue (val);

                    VARIANT newValue;
                    VariantHelpers::setDouble (valueInterface->getCurrentValue(), &newValue);
                    sendAccessibilityPropertyChangedEvent (handler, UIA_RangeValueValuePropertyId, newValue);

                    return S_OK;
                }
            }
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_Value (double* pRetVal) override
    {
        return withValueInterface (pRetVal, [] (const AccessibilityValueInterface& valueInterface)
        {
            return valueInterface.getCurrentValue();
        });
    }

    JUCE_COMRESULT get_IsReadOnly (BOOL* pRetVal) override
    {
        return withValueInterface (pRetVal, [] (const AccessibilityValueInterface& valueInterface)
        {
            return valueInterface.isReadOnly();
        });
    }

    JUCE_COMRESULT get_Maximum (double* pRetVal) override
    {
        return withValueInterface (pRetVal, [] (const AccessibilityValueInterface& valueInterface)
        {
            return valueInterface.getRange().getMaximumValue();
        });
    }

    JUCE_COMRESULT get_Minimum (double* pRetVal) override
    {
        return withValueInterface (pRetVal, [] (const AccessibilityValueInterface& valueInterface)
        {
            return valueInterface.getRange().getMinimumValue();
        });
    }

    JUCE_COMRESULT get_LargeChange (double* pRetVal) override
    {
        return get_SmallChange (pRetVal);
    }

    JUCE_COMRESULT get_SmallChange (double* pRetVal) override
    {
        return withValueInterface (pRetVal, [] (const AccessibilityValueInterface& valueInterface)
        {
            return valueInterface.getRange().getInterval();
        });
    }

private:
    template <typename Value, typename Callback>
    JUCE_COMRESULT withValueInterface (Value* pRetVal, Callback&& callback) const
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* valueInterface = getHandler().getValueInterface())
            {
                if (valueInterface->getRange().isValid())
                {
                    *pRetVal = callback (*valueInterface);
                    return S_OK;
                }
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIARangeValueProvider)
};


JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")

//==============================================================================
class UIASelectionItemProvider  : public UIAProviderBase,
                                  public ComBaseClassHelper<ComTypes::ISelectionItemProvider>
{
public:
    explicit UIASelectionItemProvider (AccessibilityNativeHandle* handle)
        : UIAProviderBase (handle),
          isRadioButton (getHandler().getRole() == AccessibilityRole::radioButton)
    {
    }

    //==============================================================================
    JUCE_COMRESULT AddToSelection() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();

        if (isRadioButton)
        {
            using namespace ComTypes::Constants;

            handler.getActions().invoke (AccessibilityActionType::press);
            sendAccessibilityAutomationEvent (handler, UIA_SelectionItem_ElementSelectedEventId);

            return S_OK;
        }

        handler.getActions().invoke (AccessibilityActionType::toggle);
        handler.getActions().invoke (AccessibilityActionType::press);

        return S_OK;
    }

    JUCE_COMRESULT get_IsSelected (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            const auto state = getHandler().getCurrentState();
            *pRetVal = isRadioButton ? state.isChecked() : state.isSelected();
            return S_OK;
        });
    }

    JUCE_COMRESULT get_SelectionContainer (IRawElementProviderSimple** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            if (! isRadioButton)
                if (auto* parent = getHandler().getParent())
                    parent->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

            return S_OK;
        });
    }

    JUCE_COMRESULT RemoveFromSelection() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        if (! isRadioButton)
        {
            const auto& handler = getHandler();

            if (handler.getCurrentState().isSelected())
                getHandler().getActions().invoke (AccessibilityActionType::toggle);
        }

        return S_OK;
    }

    JUCE_COMRESULT Select() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        AddToSelection();

        if (isElementValid() && ! isRadioButton)
        {
            const auto& handler = getHandler();

            if (auto* parent = handler.getParent())
                for (auto* child : parent->getChildren())
                    if (child != &handler && child->getCurrentState().isSelected())
                        child->getActions().invoke (AccessibilityActionType::toggle);
        }

        return S_OK;
    }

private:
    const bool isRadioButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIASelectionItemProvider)
};

//==============================================================================
class UIASelectionProvider  : public UIAProviderBase,
                              public ComBaseClassHelper<ComTypes::ISelectionProvider2>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT QueryInterface (REFIID iid, void** result) override
    {
        if (iid == __uuidof (IUnknown) || iid == __uuidof (ComTypes::ISelectionProvider))
            return castToType<ComTypes::ISelectionProvider> (result);

        if (iid == __uuidof (ComTypes::ISelectionProvider2))
            return castToType<ComTypes::ISelectionProvider2> (result);

        *result = nullptr;
        return E_NOINTERFACE;
    }

    //==============================================================================
    JUCE_COMRESULT get_CanSelectMultiple (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = isMultiSelectable();
            return S_OK;
        });
    }

    JUCE_COMRESULT get_IsSelectionRequired (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = getSelectedChildren().size() > 0 && ! isMultiSelectable();
            return S_OK;
        });
    }

    JUCE_COMRESULT GetSelection (SAFEARRAY** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            return addHandlersToArray (getSelectedChildren(), pRetVal);
        });
    }

    //==============================================================================
    JUCE_COMRESULT get_FirstSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            auto selectedChildren = getSelectedChildren();

            if (! selectedChildren.empty())
                selectedChildren.front()->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

            return S_OK;
        });
    }

    JUCE_COMRESULT get_LastSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            auto selectedChildren = getSelectedChildren();

            if (! selectedChildren.empty())
                selectedChildren.back()->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

            return S_OK;
        });
    }

    JUCE_COMRESULT get_CurrentSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            get_FirstSelectedItem (pRetVal);
            return S_OK;
        });
    }

    JUCE_COMRESULT get_ItemCount (int* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = (int) getSelectedChildren().size();
            return S_OK;
        });
    }

private:
    bool isMultiSelectable() const noexcept
    {
         return getHandler().getCurrentState().isMultiSelectable();
    }

    std::vector<const AccessibilityHandler*> getSelectedChildren() const
    {
        std::vector<const AccessibilityHandler*> selectedHandlers;

        for (auto* child : getHandler().getComponent().getChildren())
            if (auto* handler = child->getAccessibilityHandler())
                if (handler->getCurrentState().isSelected())
                    selectedHandlers.push_back (handler);

        return selectedHandlers;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIASelectionProvider)
};

JUCE_END_IGNORE_WARNINGS_GCC_LIKE


//==============================================================================
class UIATextProvider  : public UIAProviderBase,
                         public ComBaseClassHelper<ComTypes::ITextProvider2>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT QueryInterface (REFIID iid, void** result) override
    {
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")

        if (iid == __uuidof (IUnknown) || iid == __uuidof (ComTypes::ITextProvider))
            return castToType<ComTypes::ITextProvider> (result);

        if (iid == __uuidof (ComTypes::ITextProvider2))
            return castToType<ComTypes::ITextProvider2> (result);

        *result = nullptr;
        return E_NOINTERFACE;

        JUCE_END_IGNORE_WARNINGS_GCC_LIKE
    }

    //=============================================================================
    JUCE_COMRESULT get_DocumentRange (ComTypes::ITextRangeProvider** pRetVal) override
    {
        return withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
        {
            *pRetVal = new UIATextRangeProvider (*this, { 0, textInterface.getTotalNumCharacters() });
            return S_OK;
        });
    }

    JUCE_COMRESULT get_SupportedTextSelection (ComTypes::SupportedTextSelection* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = ComTypes::SupportedTextSelection_Single;
            return S_OK;
        });
    }

    JUCE_COMRESULT GetSelection (SAFEARRAY** pRetVal) override
    {
        return withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
        {
            *pRetVal = SafeArrayCreateVector (VT_UNKNOWN, 0, 1);

            if (pRetVal != nullptr)
            {
                auto selection = textInterface.getSelection();
                auto hasSelection = ! selection.isEmpty();
                auto cursorPos = textInterface.getTextInsertionOffset();

                auto* rangeProvider = new UIATextRangeProvider (*this,
                                                                { hasSelection ? selection.getStart() : cursorPos,
                                                                  hasSelection ? selection.getEnd()   : cursorPos });

                LONG pos = 0;
                auto hr = SafeArrayPutElement (*pRetVal, &pos, static_cast<IUnknown*> (rangeProvider));

                if (FAILED (hr))
                    return E_FAIL;

                rangeProvider->Release();
            }

            return S_OK;
        });
    }

    JUCE_COMRESULT GetVisibleRanges (SAFEARRAY** pRetVal) override
    {
        return withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
        {
            *pRetVal = SafeArrayCreateVector (VT_UNKNOWN, 0, 1);

            if (pRetVal != nullptr)
            {
                auto* rangeProvider = new UIATextRangeProvider (*this, { 0, textInterface.getTotalNumCharacters() });

                LONG pos = 0;
                auto hr = SafeArrayPutElement (*pRetVal, &pos, static_cast<IUnknown*> (rangeProvider));

                if (FAILED (hr))
                    return E_FAIL;

                rangeProvider->Release();
            }

            return S_OK;
        });
    }

    JUCE_COMRESULT RangeFromChild (IRawElementProviderSimple*, ComTypes::ITextRangeProvider** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, []
        {
            return S_OK;
        });
    }

    JUCE_COMRESULT RangeFromPoint (ComTypes::UiaPoint point, ComTypes::ITextRangeProvider** pRetVal) override
    {
        return withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
        {
            auto offset = textInterface.getOffsetAtPoint ({ roundToInt (point.x), roundToInt (point.y) });

            if (offset > 0)
                *pRetVal = new UIATextRangeProvider (*this, { offset, offset });

            return S_OK;
        });
    }

    //==============================================================================
    JUCE_COMRESULT GetCaretRange (BOOL* isActive, ComTypes::ITextRangeProvider** pRetVal) override
    {
        return withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
        {
            *isActive = getHandler().hasFocus (false);

            auto cursorPos = textInterface.getTextInsertionOffset();
            *pRetVal = new UIATextRangeProvider (*this, { cursorPos, cursorPos });

            return S_OK;
        });
    }

    JUCE_COMRESULT RangeFromAnnotation (IRawElementProviderSimple*, ComTypes::ITextRangeProvider** pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, []
        {
            return S_OK;
        });
    }

private:
    //==============================================================================
    template <typename Value, typename Callback>
    JUCE_COMRESULT withTextInterface (Value* pRetVal, Callback&& callback) const
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* textInterface = getHandler().getTextInterface())
                return callback (*textInterface);

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    //==============================================================================
    class UIATextRangeProvider  : public UIAProviderBase,
                                  public ComBaseClassHelper<ComTypes::ITextRangeProvider>
    {
    public:
        UIATextRangeProvider (UIATextProvider& textProvider, Range<int> range)
            : UIAProviderBase (textProvider.getHandler().getNativeImplementation()),
              owner (&textProvider),
              selectionRange (range)
        {
        }

        //==============================================================================
        Range<int> getSelectionRange() const noexcept  { return selectionRange; }

        //==============================================================================
        JUCE_COMRESULT AddToSelection() override
        {
            return Select();
        }

        JUCE_COMRESULT Clone (ComTypes::ITextRangeProvider** pRetVal) override
        {
            return withCheckedComArgs (pRetVal, *this, [&]
            {
                *pRetVal = new UIATextRangeProvider (*owner, selectionRange);
                return S_OK;
            });
        }

        JUCE_COMRESULT Compare (ComTypes::ITextRangeProvider* range, BOOL* pRetVal) override
        {
            return withCheckedComArgs (pRetVal, *this, [&]
            {
                *pRetVal = (selectionRange == static_cast<UIATextRangeProvider*> (range)->getSelectionRange());
                return S_OK;
            });
        }

        JUCE_COMRESULT CompareEndpoints (ComTypes::TextPatternRangeEndpoint endpoint,
                                         ComTypes::ITextRangeProvider* targetRange,
                                         ComTypes::TextPatternRangeEndpoint targetEndpoint,
                                         int* pRetVal) override
        {
            if (targetRange == nullptr)
                return E_INVALIDARG;

            return withCheckedComArgs (pRetVal, *this, [&]
            {
                auto offset = (endpoint == ComTypes::TextPatternRangeEndpoint_Start ? selectionRange.getStart()
                                                                                    : selectionRange.getEnd());

                auto otherRange = static_cast<UIATextRangeProvider*> (targetRange)->getSelectionRange();
                auto otherOffset = (targetEndpoint == ComTypes::TextPatternRangeEndpoint_Start ? otherRange.getStart()
                                                                                               : otherRange.getEnd());

                *pRetVal = offset - otherOffset;
                return S_OK;
            });
        }

        JUCE_COMRESULT ExpandToEnclosingUnit (ComTypes::TextUnit unit) override
        {
            if (! isElementValid())
                return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

            if (auto* textInterface = owner->getHandler().getTextInterface())
            {
                using ATH = AccessibilityTextHelpers;

                const auto boundaryType = getBoundaryType (unit);
                const auto start = ATH::findTextBoundary (*textInterface,
                                                          selectionRange.getStart(),
                                                          boundaryType,
                                                          ATH::Direction::backwards,
                                                          ATH::IncludeThisBoundary::yes,
                                                          ATH::IncludeWhitespaceAfterWords::no);

                const auto end = ATH::findTextBoundary (*textInterface,
                                                        start,
                                                        boundaryType,
                                                        ATH::Direction::forwards,
                                                        ATH::IncludeThisBoundary::no,
                                                        ATH::IncludeWhitespaceAfterWords::yes);

                selectionRange = Range<int> (start, end);

                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        }

        JUCE_COMRESULT FindAttribute (TEXTATTRIBUTEID, VARIANT, BOOL, ComTypes::ITextRangeProvider** pRetVal) override
        {
            return withCheckedComArgs (pRetVal, *this, []
            {
                return S_OK;
            });
        }

        JUCE_COMRESULT FindText (BSTR text, BOOL backward, BOOL ignoreCase,
                                 ComTypes::ITextRangeProvider** pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                auto selectionText = textInterface.getText (selectionRange);
                String textToSearchFor (text);

                auto offset = (backward ? (ignoreCase ? selectionText.lastIndexOfIgnoreCase (textToSearchFor) : selectionText.lastIndexOf (textToSearchFor))
                                        : (ignoreCase ? selectionText.indexOfIgnoreCase (textToSearchFor)     : selectionText.indexOf (textToSearchFor)));

                if (offset != -1)
                    *pRetVal = new UIATextRangeProvider (*owner, { offset, offset + textToSearchFor.length() });

                return S_OK;
            });
        }

        JUCE_COMRESULT GetAttributeValue (TEXTATTRIBUTEID attributeId, VARIANT* pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                VariantHelpers::clear (pRetVal);

                using namespace ComTypes::Constants;

                switch (attributeId)
                {
                    case UIA_IsReadOnlyAttributeId:
                    {
                        VariantHelpers::setBool (textInterface.isReadOnly(), pRetVal);
                        break;
                    }

                    case UIA_CaretPositionAttributeId:
                    {
                        auto cursorPos = textInterface.getTextInsertionOffset();

                        auto caretPos = [&]
                        {
                            if (cursorPos == 0)
                                return ComTypes::CaretPosition_BeginningOfLine;

                            if (cursorPos == textInterface.getTotalNumCharacters())
                                return ComTypes::CaretPosition_EndOfLine;

                            return ComTypes::CaretPosition_Unknown;
                        }();

                        VariantHelpers::setInt (caretPos, pRetVal);
                        break;
                    }
                }

                return S_OK;
            });
        }

        JUCE_COMRESULT GetBoundingRectangles (SAFEARRAY** pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                auto rectangleList = textInterface.getTextBounds (selectionRange);
                auto numRectangles = rectangleList.getNumRectangles();

                *pRetVal = SafeArrayCreateVector (VT_R8, 0, 4 * (ULONG) numRectangles);

                if (*pRetVal == nullptr)
                    return E_FAIL;

                if (numRectangles > 0)
                {
                    double* doubleArr = nullptr;

                    if (FAILED (SafeArrayAccessData (*pRetVal, reinterpret_cast<void**> (&doubleArr))))
                    {
                        SafeArrayDestroy (*pRetVal);
                        return E_FAIL;
                    }

                    for (int i = 0; i < numRectangles; ++i)
                    {
                        auto r = Desktop::getInstance().getDisplays().logicalToPhysical (rectangleList.getRectangle (i));

                        doubleArr[i * 4]     = r.getX();
                        doubleArr[i * 4 + 1] = r.getY();
                        doubleArr[i * 4 + 2] = r.getWidth();
                        doubleArr[i * 4 + 3] = r.getHeight();
                    }

                    if (FAILED (SafeArrayUnaccessData (*pRetVal)))
                    {
                        SafeArrayDestroy (*pRetVal);
                        return E_FAIL;
                    }
                }

                return S_OK;
            });
        }

        JUCE_COMRESULT GetChildren (SAFEARRAY** pRetVal) override
        {
            return withCheckedComArgs (pRetVal, *this, [&]
            {
                *pRetVal = SafeArrayCreateVector (VT_UNKNOWN, 0, 0);
                return S_OK;
            });
        }

        JUCE_COMRESULT GetEnclosingElement (IRawElementProviderSimple** pRetVal) override
        {
            JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")

            return withCheckedComArgs (pRetVal, *this, [&]
            {
                getHandler().getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
                return S_OK;
            });

            JUCE_END_IGNORE_WARNINGS_GCC_LIKE
        }

        JUCE_COMRESULT GetText (int maxLength, BSTR* pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                auto text = textInterface.getText (selectionRange);

                if (maxLength >= 0 && text.length() > maxLength)
                    text = text.substring (0, maxLength);

                *pRetVal = SysAllocString ((const OLECHAR*) text.toWideCharPointer());
                return S_OK;
            });
        }

        JUCE_COMRESULT Move (ComTypes::TextUnit unit, int count, int* pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                using ATH = AccessibilityTextHelpers;

                const auto boundaryType = getBoundaryType (unit);
                const auto previousUnitBoundary = ATH::findTextBoundary (textInterface,
                                                                         selectionRange.getStart(),
                                                                         boundaryType,
                                                                         ATH::Direction::backwards,
                                                                         ATH::IncludeThisBoundary::yes,
                                                                         ATH::IncludeWhitespaceAfterWords::no);

                auto numMoved = 0;
                auto movedEndpoint = previousUnitBoundary;

                for (; numMoved < std::abs (count); ++numMoved)
                {
                    const auto nextEndpoint = ATH::findTextBoundary (textInterface,
                                                                     movedEndpoint,
                                                                     boundaryType,
                                                                     count > 0 ? ATH::Direction::forwards : ATH::Direction::backwards,
                                                                     ATH::IncludeThisBoundary::no,
                                                                     count > 0 ? ATH::IncludeWhitespaceAfterWords::yes : ATH::IncludeWhitespaceAfterWords::no);

                    if (nextEndpoint == movedEndpoint)
                        break;

                    movedEndpoint = nextEndpoint;
                }

                *pRetVal = numMoved;

                ExpandToEnclosingUnit (unit);
                return S_OK;
            });
        }

        JUCE_COMRESULT MoveEndpointByRange (ComTypes::TextPatternRangeEndpoint endpoint,
                                            ComTypes::ITextRangeProvider* targetRange,
                                            ComTypes::TextPatternRangeEndpoint targetEndpoint) override
        {
            if (targetRange == nullptr)
                return E_INVALIDARG;

            if (! isElementValid())
                return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

            if (owner->getHandler().getTextInterface() != nullptr)
            {
                auto otherRange = static_cast<UIATextRangeProvider*> (targetRange)->getSelectionRange();
                auto targetPoint = (targetEndpoint == ComTypes::TextPatternRangeEndpoint_Start ? otherRange.getStart()
                                                                                               : otherRange.getEnd());

                setEndpointChecked (endpoint, targetPoint);
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        }

        JUCE_COMRESULT MoveEndpointByUnit (ComTypes::TextPatternRangeEndpoint endpoint,
                                           ComTypes::TextUnit unit,
                                           int count,
                                           int* pRetVal) override
        {
            return owner->withTextInterface (pRetVal, [&] (const AccessibilityTextInterface& textInterface)
            {
                if (count == 0 || textInterface.getTotalNumCharacters() == 0)
                    return S_OK;

                const auto endpointToMove = (endpoint == ComTypes::TextPatternRangeEndpoint_Start ? selectionRange.getStart()
                                                                                                  : selectionRange.getEnd());

                using ATH = AccessibilityTextHelpers;

                const auto direction = (count > 0 ? ATH::Direction::forwards
                                                  : ATH::Direction::backwards);

                const auto boundaryType = getBoundaryType (unit);
                auto movedEndpoint = endpointToMove;

                int numMoved = 0;
                for (; numMoved < std::abs (count); ++numMoved)
                {
                    auto nextEndpoint = ATH::findTextBoundary (textInterface,
                                                               movedEndpoint,
                                                               boundaryType,
                                                               direction,
                                                               ATH::IncludeThisBoundary::no,
                                                               direction == ATH::Direction::forwards ? ATH::IncludeWhitespaceAfterWords::yes
                                                                                                     : ATH::IncludeWhitespaceAfterWords::no);

                    if (nextEndpoint == movedEndpoint)
                        break;

                    movedEndpoint = nextEndpoint;
                }

                *pRetVal = numMoved;

                setEndpointChecked (endpoint, movedEndpoint);

                return S_OK;
            });
        }

        JUCE_COMRESULT RemoveFromSelection() override
        {
            if (! isElementValid())
                return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

            if (auto* textInterface = owner->getHandler().getTextInterface())
            {
                textInterface->setSelection ({});
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        }

        JUCE_COMRESULT ScrollIntoView (BOOL) override
        {
            if (! isElementValid())
                return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

            return (HRESULT) UIA_E_NOTSUPPORTED;
        }

        JUCE_COMRESULT Select() override
        {
            if (! isElementValid())
                return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

            if (auto* textInterface = owner->getHandler().getTextInterface())
            {
                textInterface->setSelection ({});
                textInterface->setSelection (selectionRange);

                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        }

    private:
        static AccessibilityTextHelpers::BoundaryType getBoundaryType (ComTypes::TextUnit unit)
        {
            switch (unit)
            {
                case ComTypes::TextUnit_Character:
                    return AccessibilityTextHelpers::BoundaryType::character;

                case ComTypes::TextUnit_Format:
                case ComTypes::TextUnit_Word:
                    return AccessibilityTextHelpers::BoundaryType::word;

                case ComTypes::TextUnit_Line:
                    return AccessibilityTextHelpers::BoundaryType::line;

                case ComTypes::TextUnit_Paragraph:
                case ComTypes::TextUnit_Page:
                case ComTypes::TextUnit_Document:
                    return AccessibilityTextHelpers::BoundaryType::document;
            };

            jassertfalse;
            return AccessibilityTextHelpers::BoundaryType::character;
        }

        void setEndpointChecked (ComTypes::TextPatternRangeEndpoint endpoint, int newEndpoint)
        {
            if (endpoint == ComTypes::TextPatternRangeEndpoint_Start)
            {
                if (selectionRange.getEnd() < newEndpoint)
                    selectionRange.setEnd (newEndpoint);

                selectionRange.setStart (newEndpoint);
            }
            else
            {
                if (selectionRange.getStart() > newEndpoint)
                    selectionRange.setStart (newEndpoint);

                selectionRange.setEnd (newEndpoint);
            }
        }

        ComSmartPtr<UIATextProvider> owner;
        Range<int> selectionRange;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIATextRangeProvider)
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIATextProvider)
};


//==============================================================================
class UIAToggleProvider  : public UIAProviderBase,
                           public ComBaseClassHelper<ComTypes::IToggleProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT Toggle() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();

        if (handler.getActions().invoke (AccessibilityActionType::toggle)
            || handler.getActions().invoke (AccessibilityActionType::press))
        {
            VARIANT newValue;
            VariantHelpers::setInt (getCurrentToggleState(), &newValue);

            sendAccessibilityPropertyChangedEvent (handler, UIA_ToggleToggleStatePropertyId, newValue);

            return S_OK;
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_ToggleState (ComTypes::ToggleState* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = getCurrentToggleState();
            return S_OK;
        });
    }

private:
    ComTypes::ToggleState getCurrentToggleState() const
    {
        return getHandler().getCurrentState().isChecked() ? ComTypes::ToggleState_On
                                                          : ComTypes::ToggleState_Off;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAToggleProvider)
};


//==============================================================================
class UIATransformProvider  : public UIAProviderBase,
                              public ComBaseClassHelper<ComTypes::ITransformProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT Move (double x, double y) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = getPeer())
        {
            RECT rect;
            GetWindowRect ((HWND) peer->getNativeHandle(), &rect);

            rect.left = roundToInt (x);
            rect.top  = roundToInt (y);

            auto bounds = Rectangle<int>::leftTopRightBottom (rect.left, rect.top, rect.right, rect.bottom);

            peer->setBounds (Desktop::getInstance().getDisplays().physicalToLogical (bounds),
                             peer->isFullScreen());
        }

        return S_OK;
    }

    JUCE_COMRESULT Resize (double width, double height) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = getPeer())
        {
            auto scale = peer->getPlatformScaleFactor();

            peer->getComponent().setSize (roundToInt (width  / scale),
                                          roundToInt (height / scale));
        }

        return S_OK;
    }

    JUCE_COMRESULT Rotate (double) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_CanMove (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = true;
            return S_OK;
        });
    }

    JUCE_COMRESULT get_CanResize (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            if (auto* peer = getPeer())
                *pRetVal = ((peer->getStyleFlags() & ComponentPeer::windowIsResizable) != 0);

            return S_OK;
        });
    }

    JUCE_COMRESULT get_CanRotate (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = false;
            return S_OK;
        });
    }

private:
    ComponentPeer* getPeer() const
    {
        return getHandler().getComponent().getPeer();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIATransformProvider)
};


//==============================================================================
class UIAValueProvider  : public UIAProviderBase,
                          public ComBaseClassHelper<ComTypes::IValueProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT SetValue (LPCWSTR val) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        const auto& handler = getHandler();
        auto& valueInterface = *handler.getValueInterface();

        if (valueInterface.isReadOnly())
            return (HRESULT) UIA_E_NOTSUPPORTED;

        valueInterface.setValueAsString (String (val));

        VARIANT newValue;
        VariantHelpers::setString (valueInterface.getCurrentValueAsString(), &newValue);

        sendAccessibilityPropertyChangedEvent (handler, UIA_ValueValuePropertyId, newValue);

        return S_OK;
    }

    JUCE_COMRESULT get_Value (BSTR* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            auto currentValueString = getHandler().getValueInterface()->getCurrentValueAsString();

            *pRetVal = SysAllocString ((const OLECHAR*) currentValueString.toWideCharPointer());
            return S_OK;
        });
    }

    JUCE_COMRESULT get_IsReadOnly (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]
        {
            *pRetVal = getHandler().getValueInterface()->isReadOnly();
            return S_OK;
        });
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAValueProvider)
};


//==============================================================================
class UIAWindowProvider : public UIAProviderBase,
                          public ComBaseClassHelper<ComTypes::IWindowProvider>
{
public:
    using UIAProviderBase::UIAProviderBase;

    //==============================================================================
    JUCE_COMRESULT SetVisualState (ComTypes::WindowVisualState state) override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = getPeer())
        {
            switch (state)
            {
                case ComTypes::WindowVisualState_Maximized:
                    peer->setFullScreen (true);
                    break;

                case ComTypes::WindowVisualState_Minimized:
                    peer->setMinimised (true);
                    break;

                case ComTypes::WindowVisualState_Normal:
                    peer->setFullScreen (false);
                    peer->setMinimised (false);
                    break;

                default:
                    break;
            }

            return S_OK;
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT Close() override
    {
        if (! isElementValid())
            return (HRESULT) UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = getPeer())
        {
            peer->handleUserClosingWindow();
            return S_OK;
        }

        return (HRESULT) UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT WaitForInputIdle (int, BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, []
        {
            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_CanMaximize (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                *pRetVal = (peer->getStyleFlags() & ComponentPeer::windowHasMaximiseButton) != 0;
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_CanMinimize (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                *pRetVal = (peer->getStyleFlags() & ComponentPeer::windowHasMinimiseButton) != 0;
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_IsModal (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                *pRetVal = peer->getComponent().isCurrentlyModal();
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_WindowVisualState (ComTypes::WindowVisualState* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                if (peer->isFullScreen())
                    *pRetVal = ComTypes::WindowVisualState_Maximized;
                else if (peer->isMinimised())
                    *pRetVal = ComTypes::WindowVisualState_Minimized;
                else
                    *pRetVal = ComTypes::WindowVisualState_Normal;

                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_WindowInteractionState (ComTypes::WindowInteractionState* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                *pRetVal = peer->getComponent().isCurrentlyBlockedByAnotherModalComponent()
                    ? ComTypes::WindowInteractionState::WindowInteractionState_BlockedByModalWindow
                    : ComTypes::WindowInteractionState::WindowInteractionState_Running;

                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

    JUCE_COMRESULT get_IsTopmost (BOOL* pRetVal) override
    {
        return withCheckedComArgs (pRetVal, *this, [&]() -> HRESULT
        {
            if (auto* peer = getPeer())
            {
                *pRetVal = peer->isFocused();
                return S_OK;
            }

            return (HRESULT) UIA_E_NOTSUPPORTED;
        });
    }

private:
    ComponentPeer* getPeer() const
    {
        return getHandler().getComponent().getPeer();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAWindowProvider)
};


} // namespace juce
