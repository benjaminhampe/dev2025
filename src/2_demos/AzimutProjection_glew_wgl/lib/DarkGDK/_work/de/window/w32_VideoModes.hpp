#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

struct VideoMode
{
   int32_t width;
   int32_t height;
   int32_t bpp;
   float freq;
   std::string name;

   std::string str() const
   {
      std::ostringstream o;
      if ( name.size() ) o << name << ", ";
      o << width << ", " << height << ", " << bpp << ", " << freq << "Hz";
      return o.str();
   }
};


std::vector< VideoMode >
getVideoModes();

inline void
printVideoModes()
{
   std::vector< VideoMode > videoModes = getVideoModes();

   printf("VideoModeCount = %d\n", int(videoModes.size()) );
   for ( size_t i = 0; i < videoModes.size(); ++i )
   {
      printf("VideoMode[%d] = %s\n", int(i), videoModes[ i ].str().c_str() );
   }
   fflush( stdout );
}
