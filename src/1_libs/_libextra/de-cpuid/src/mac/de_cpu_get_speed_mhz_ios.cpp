#include <de_cpu_get_speed.hpp>

#if 0
float
de_cpu_get_speed_Mhz()
{
   float mhz = 0.0f;
   struct clockinfo CpuClock;
   size_t Size = sizeof(clockinfo);

   if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, nullptr, 0))
      return 0;
   else
      return CpuClock.hz;
   return mhz;
}
#endif