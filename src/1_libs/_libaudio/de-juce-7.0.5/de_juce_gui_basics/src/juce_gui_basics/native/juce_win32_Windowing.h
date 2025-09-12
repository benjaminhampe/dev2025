#pragma once
#include <juce_gui_basics/juce_gui_basics_config.h>

#if JUCE_WINDOWS
#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>

namespace juce
{

//==============================================================================
class SimpleTimer  : private Timer
{
public:
    SimpleTimer (int intervalMs, std::function<void()> callbackIn)
        : callback (std::move (callbackIn))
    {
        jassert (callback);
        startTimer (intervalMs);
    }

    ~SimpleTimer() override
    {
        stopTimer();
    }

private:
    void timerCallback() override
    {
        callback();
    }

    std::function<void()> callback;
};

} // namespace juce

#endif
