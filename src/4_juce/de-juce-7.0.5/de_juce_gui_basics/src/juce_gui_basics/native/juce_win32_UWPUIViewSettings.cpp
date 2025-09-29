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

#include <juce_gui_basics/native/juce_win32_UWPUIViewSettings.h>

#if JUCE_WINDOWS

namespace juce
{

UWPUIViewSettings::UWPUIViewSettings()
{
  ComBaseModule dll (L"api-ms-win-core-winrt-l1-1-0");

  if (dll.h != nullptr)
  {
      roInitialize           = (RoInitializeFuncPtr)           ::GetProcAddress (dll.h, "RoInitialize");
      roGetActivationFactory = (RoGetActivationFactoryFuncPtr) ::GetProcAddress (dll.h, "RoGetActivationFactory");
      createHString          = (WindowsCreateStringFuncPtr)    ::GetProcAddress (dll.h, "WindowsCreateString");
      deleteHString          = (WindowsDeleteStringFuncPtr)    ::GetProcAddress (dll.h, "WindowsDeleteString");

      if (roInitialize == nullptr || roGetActivationFactory == nullptr
           || createHString == nullptr || deleteHString == nullptr)
          return;

      auto status = roInitialize (1);

      if (status != S_OK && status != S_FALSE && (unsigned) status != 0x80010106L)
          return;

      LPCWSTR uwpClassName = L"Windows.UI.ViewManagement.UIViewSettings";
      HSTRING uwpClassId = nullptr;

      if (createHString (uwpClassName, (::UINT32) wcslen (uwpClassName), &uwpClassId) != S_OK
           || uwpClassId == nullptr)
          return;

      JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wlanguage-extension-token")
      status = roGetActivationFactory (uwpClassId, __uuidof (IUIViewSettingsInterop),
                                       (void**) viewSettingsInterop.resetAndGetPointerAddress());
      JUCE_END_IGNORE_WARNINGS_GCC_LIKE
      deleteHString (uwpClassId);

      if (status != S_OK || viewSettingsInterop == nullptr)
          return;

      // move dll into member var
      comBaseDLL = std::move (dll);
  }
}

//UWPUIViewSettings::ComBaseModule::ComBaseModule() = default;
UWPUIViewSettings::ComBaseModule::ComBaseModule (LPCWSTR libraryName)
   : h (::LoadLibraryW(libraryName))
{}

UWPUIViewSettings::ComBaseModule::ComBaseModule (ComBaseModule&& o)
   : h (o.h)
{
   o.h = nullptr;
}

UWPUIViewSettings::ComBaseModule::~ComBaseModule()
{
   release();
}

void
UWPUIViewSettings::ComBaseModule::release()
{
   if (h != nullptr)
   {
      ::FreeLibrary (h);
      h = nullptr;
   }
}

UWPUIViewSettings::ComBaseModule&
UWPUIViewSettings::ComBaseModule::operator= (ComBaseModule&& o)
{
   release();
   h = o.h;
   o.h = nullptr;
   return *this;
}

} // namespace juce

#endif // JUCE_WINDOWS
