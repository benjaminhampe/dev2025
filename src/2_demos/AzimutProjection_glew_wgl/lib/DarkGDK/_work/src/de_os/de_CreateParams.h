#pragma once
#include <de_os/de_EventReceiver.h>

namespace de {

struct CreateParams
{
   int width = 1024;
   int height = 768;
   int r = 8;    // color buffer red bits
   int g = 8;    // color buffer green
   int b = 8;    // color buffer b
   int a = 8;    // color buffer alpha
   int d = 24;   // depth/zbuffer
   int s = 0;    // stencil
   int aa = 0;   // MSAA samples
   int vsync = 0; // Vertical sync, =swap interval
   bool isResizable = false;
   bool isFullscreen = false;
   bool isDoubleBuffered = false;
   bool handleSRGB = false;
   uint64_t parentWindow = 0;

   EventReceiver* eventReceiver = nullptr;
   //bool isStereoBuffered = false;

   int
   getColorBits() const { return r+g+b+a; }

   std::string
   toString() const
   {
      std::ostringstream o;
      o << width << "," << height;
      if ( r > 0 ) o << ",R" << r;
      if ( g > 0 ) o << ",G" << g;
      if ( b > 0 ) o << ",B" << b;
      if ( a > 0 ) o << ",A" << a;
      if ( d > 0 ) o << ",D" << d;
      if ( s > 0 ) o << ",S" << s;
      o << ",AA" << aa;
      o << ",VSync" << vsync;
      o << ",Fullscreen" << isFullscreen;
      return o.str();
   }

};

}// end namespace de
