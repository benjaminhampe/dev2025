#pragma once
#include <de_image/de_Recti.h>
#include <de_image/de_Rectf.h>
#include <de_image/de_PixelFormatUtil.h>
#include <de_image/de_ColorRGBA.h>
#include <vector>
#include <de_core/de_Logger.h>
#include <memory>
//#include <de_image/de_Align.h>
//#include <de_core/de_FileSystem.h>

namespace de {

} // end namespace de.

inline std::ostream&
// ===========================================================================
operator<< ( std::ostream & o, de::Image const & img )
// ===========================================================================
{
   o << img.toString();
   return o;
}

/*
// ===========================================================================
// implementation of the VIP font atlas getter defined in <de/Font.hpp>
// ===========================================================================
inline std::string
ANSIConsole_setColor( int r, int g, int b )
{
   std::ostringstream s;
   s << "\033[48;2;" << r << ";" << g << ";" << b << "m";
   return s.str();
}

inline std::string
ANSIConsole_drawImage( de::Image const & img )
{
   uint32_t w = img.w();
   uint32_t h = img.h();
   uint32_t bpp = img.getBytesPerPixel();
   uint8_t const* src = img.data(); // rgb(a) tupels

   std::ostringstream s;

   if ( bpp == 3 )
   {
      for ( size_t y = 0; y < h; ++y )
      {
         for ( size_t x = 0; x < w; ++x )
         {
            int r = *src++;
            int g = *src++;
            int b = *src++;
            s << "\033[48;2;" << r << ";" << g << ";" << b << "m ";
         }
         s << "\033[0m\n";
      }
   }
   else if ( bpp == 4 )
   {
      for ( size_t y = 0; y < h; ++y )
      {
         for ( size_t x = 0; x < w; ++x )
         {
            int r = *src++;
            int g = *src++;
            int b = *src++;
            int a = *src++;
            s << "\033[48;2;" << r << ";" << g << ";" << b << "m ";
         }
         s << "\033[0m\n";
      }
   }
   else
   {
      s << ANSIConsole_setColor(255,0,0) << " ERROR " << "\033[0m\n";
   }
   return s.str();
}
*/
