
#pragma once
#include <juce_audio_processors/processors/juce_PluginDescription.h>

namespace juce
{

inline bool arrayContainsPlugin (const OwnedArray<PluginDescription>& list,
                                 const PluginDescription& desc)
{
    for (auto* p : list)
        if (p->isDuplicateOf (desc))
            return true;

    return false;
}

template <typename Callback>
void callOnMessageThread (Callback&& callback)
{
    if (MessageManager::getInstance()->existsAndIsLockedByCurrentThread())
    {
        callback();
        return;
    }

    WaitableEvent completionEvent;

    MessageManager::callAsync ([&callback, &completionEvent]
                               {
                                   callback();
                                   completionEvent.signal();
                               });

    completionEvent.wait();
}

} // namespace juce

