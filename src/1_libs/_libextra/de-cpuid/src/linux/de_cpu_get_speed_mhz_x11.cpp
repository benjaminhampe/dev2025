#include <de_cpu_get_speed.hpp>
#include <cstdio>
#include <sstream>

float
de_cpu_get_speed_Mhz()
{
   float mhz = 0.0f;
   // int status = system( "/proc/cpuinfo | grep processor | wc -1 > /tmp/TempGetCPUInfoText" );
   // could probably be read from "/proc/cpuinfo" or "/proc/cpufreq"
   FILE* f = ::fopen( "/proc/cpuinfo", "r");
   if ( f )
   {
      char buffer[1024];
      fread( buffer, 1, 1024, f );
      buffer[ 1023 ] = 0;
      std::string s( buffer );
      int32_t pos = s.find("cpu MHz");
      if (pos != -1)
      {
         pos = s.find(':', pos);
         if (pos != -1)
         {
            mhz = atof( s.c_str() + pos + 1 );
         }
      }
      ::fclose( f );
   }
   return mhz;
}
