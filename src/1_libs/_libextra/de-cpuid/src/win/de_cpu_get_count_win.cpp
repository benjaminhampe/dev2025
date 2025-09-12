#include <de_cpu_get_count.hpp>
#include "de_win32api.hpp"

int
de_cpu_get_count()
{
   uint32_t cpuCount = 0;
   SYSTEM_INFO info;
   GetSystemInfo(&info);
   cpuCount = (uint32_t)info.dwNumberOfProcessors;
   return cpuCount;
}
