#include <juce_gui_basics/mouse/juce_getMouseEventTime.h>
#include <juce_core/time/juce_Time.h>
#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>

namespace juce
{
   int64 getMouseEventTime()
   {
       static int64 eventTimeOffset = 0;
       static LONG lastMessageTime = 0;
       const LONG thisMessageTime = GetMessageTime();

       if (thisMessageTime < lastMessageTime || lastMessageTime == 0)
       {
           lastMessageTime = thisMessageTime;
           eventTimeOffset = Time::currentTimeMillis() - thisMessageTime;
       }

       return eventTimeOffset + thisMessageTime;
   }

} // namespace juce

