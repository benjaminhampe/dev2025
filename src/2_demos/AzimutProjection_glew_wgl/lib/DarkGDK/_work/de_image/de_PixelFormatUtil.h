/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <de_image/de_PixelFormat.h>
#include <sstream>

namespace de {

// ===================================================================
struct PixelFormatUtil
// ===================================================================
{
   static std::string getString( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return "R8G8B8A8";
         case PixelFormat::R8G8B8   : return "R8G8B8";
         case PixelFormat::R5G6B5   : return "R5G6B5";
         case PixelFormat::R5G5B5A1 : return "R5G5B5A1";
         case PixelFormat::R8       : return "R8";
         case PixelFormat::R16      : return "R16";
         case PixelFormat::R24      : return "R24";
         case PixelFormat::R32      : return "R32";
         case PixelFormat::R32F     : return "R32F";
         case PixelFormat::A1R5G5B5 : return "A1R5G5B5";
         case PixelFormat::B8G8R8A8 : return "B8G8R8A8";
         case PixelFormat::B8G8R8   : return "B8G8R8";
         case PixelFormat::D24S8    : return "D24S8";
         case PixelFormat::D32F     : return "D32F";
         case PixelFormat::D32FS8   : return "D32FS8";
         default                    : return "Unknown"; // Auto
      }
   }   

   static uint32_t getBitsPerPixel( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 32;
         case PixelFormat::R8G8B8   : return 24;
         case PixelFormat::R5G6B5   : return 16;
         case PixelFormat::R5G5B5A1 : return 16;
         case PixelFormat::R8       : return 8;
         case PixelFormat::R16      : return 16;
         case PixelFormat::R24      : return 24;
         case PixelFormat::R32      : return 32;
         case PixelFormat::R32F     : return 32;
         case PixelFormat::A1R5G5B5 : return 16;
         case PixelFormat::B8G8R8A8 : return 32;
         case PixelFormat::B8G8R8   : return 24;
         case PixelFormat::D24S8    : return 32;
         case PixelFormat::D32F     : return 32;
         case PixelFormat::D32FS8   : return 40;
         default                    : return 0;
      }
   }   

   static uint32_t getBytesPerPixel( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 4;
         case PixelFormat::R8G8B8   : return 3;
         case PixelFormat::R5G6B5   : return 2;
         case PixelFormat::R5G5B5A1 : return 2;
         case PixelFormat::R8       : return 1;
         case PixelFormat::R16      : return 2;
         case PixelFormat::R24      : return 3;
         case PixelFormat::R32      : return 4;        
         case PixelFormat::R32F     : return 4;
         case PixelFormat::A1R5G5B5 : return 2;
         case PixelFormat::B8G8R8A8 : return 4;
         case PixelFormat::B8G8R8   : return 3;
         case PixelFormat::D24S8    : return 4;
         case PixelFormat::D32F     : return 4;
         case PixelFormat::D32FS8   : return 5;
         default                    : return 0;
      }
   }   

   static uint32_t getChannelCount( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 4;
         case PixelFormat::R8G8B8   : return 3;
         case PixelFormat::R5G6B5   : return 3;
         case PixelFormat::R5G5B5A1 : return 4;
         case PixelFormat::R8       : return 1;
         case PixelFormat::R16      : return 1;
         case PixelFormat::R24      : return 1;
         case PixelFormat::R32      : return 1;        
         case PixelFormat::R32F     : return 1;
         case PixelFormat::A1R5G5B5 : return 4;
         case PixelFormat::B8G8R8A8 : return 4;
         case PixelFormat::B8G8R8   : return 3;
         case PixelFormat::D24S8    : return 2;
         case PixelFormat::D32F     : return 1;
         case PixelFormat::D32FS8   : return 2;
         default                    : return 0;
      }
   }   

   static uint8_t getRedBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 5;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::R8        : return 8;
         case PixelFormat::R16       : return 16;
         case PixelFormat::R24       : return 24;
         case PixelFormat::R32       : return 32;
         case PixelFormat::R32F      : return 32;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;
         default: return 0;
      }
   }

   static uint8_t getGreenBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 6;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;         
         default: return 0;
      }
   }

   static uint8_t getBlueBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 5;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;         
         default: return 0;
      }
   }

   static uint8_t getAlphaBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 8;
         case PixelFormat::R5G5B5A1 : return 1;
         case PixelFormat::A1R5G5B5 : return 1;
         case PixelFormat::B8G8R8A8 : return 8;
                  
         default: return 0;
      }
   }

   static uint8_t getDepthBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::D24S8 : return 24;
         case PixelFormat::D32F : return 32;
         case PixelFormat::D32FS8 : return 32;
         default: return 0;
      }
   }

   static uint8_t getStencilBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::D24S8 : return 8;
         case PixelFormat::D32FS8 : return 8;
         default: return 0;
      }
   }
   
};

} // end namespace de.
