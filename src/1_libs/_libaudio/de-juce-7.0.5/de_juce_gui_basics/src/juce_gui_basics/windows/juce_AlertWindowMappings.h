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

#include <juce_gui_basics/juce_gui_basics_config.h>
#include <juce_gui_basics/components/juce_ModalComponentManager.h>

namespace juce
{

namespace AlertWindowMappings
{
    using MapFn = int (*) (int);

    static inline int noMapping (int buttonIndex)    { return buttonIndex; }
    static inline int messageBox (int)               { return 0; }
    static inline int okCancel (int buttonIndex)     { return buttonIndex == 0 ? 1 : 0; }
    static inline int yesNoCancel (int buttonIndex)  { return buttonIndex == 2 ? 0 : buttonIndex + 1; }

    static std::unique_ptr<ModalComponentManager::Callback> getWrappedCallback (ModalComponentManager::Callback* callbackIn,
                                                                                MapFn mapFn)
    {
        jassert (mapFn != nullptr);

        if (callbackIn == nullptr)
            return nullptr;

        auto wrappedCallback = [innerCallback = rawToUniquePtr (callbackIn), mapFn] (int buttonIndex)
        {
            innerCallback->modalStateFinished (mapFn (buttonIndex));
        };

        return rawToUniquePtr (ModalCallbackFunction::create (std::move (wrappedCallback)));
    }

}

} // namespace juce
