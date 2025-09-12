#pragma once
#include <de/Image.hpp>

namespace de {

// =======================================================================
struct Brush
// =======================================================================
{
   Brush()
      : color( 0xFFFFFFFF )
      , pattern( nullptr )
   {}
   Brush( uint32_t color_,
          Image* pattern_ = nullptr )
      : color( color_ )
      , pattern( pattern_ )
   {}

   uint32_t color;
   Image* pattern;
};

} // end namespace de.
