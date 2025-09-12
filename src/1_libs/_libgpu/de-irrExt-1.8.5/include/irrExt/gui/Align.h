#pragma once
#include <cstdint>
#include <sstream>
#include <irrlicht.h>

// =======================================================================
namespace irrExt {

struct Align
{
   enum EAlign : uint8_t
   {
      None     = 0,
      Left     = 1,
      Center   = 1 << 1,
      Right    = 1 << 2,
      Top      = 1 << 3,
      Middle   = 1 << 4,
      Bottom   = 1 << 5,

      TopLeft = Left | Top,
      TopCenter = Center | Top,
      TopRight = Right | Top,

      LeftMiddle = Left | Middle,
      Centered = Center | Middle,
      RightMiddle = Right | Middle,

      BottomLeft = Left | Bottom,
      BottomCenter = Center | Bottom,
      BottomRight = Right | Bottom,
   };
};

inline irr::core::recti
mkRect( int32_t x, int32_t y, irr::core::dimension2du const & size )
{
   return irr::core::recti( irr::core::position2di( x, y ), size );
}

inline irr::core::recti
mkRect( int32_t x, int32_t y, int32_t w, int32_t h )
{
   w = irr::core::clamp( w, 0, 100000 );
   h = irr::core::clamp( h, 0, 100000 );
   return irr::core::recti(
                  irr::core::position2di( x, y ),
                  irr::core::dimension2du( uint32_t( w ), uint32_t( h ) ) );
}

} // end namespace irrExt