#pragma once
#include <de/Color.h>

namespace de {

// ===========================================================================
// ===   Bresenham Algorithms
// ===========================================================================
typedef std::function< void( int32_t, int32_t ) > SetPixel_t;

// ===========================================================================
struct Bresenham
// ===========================================================================
{
   DE_CREATE_LOGGER("de.image.Bresenham")

//   typedef std::vector< glm::ivec2 > PointList;

//   static PointList
//   traverseCircle( int32_t x, int32_t y, int32_t radius );

   // static PointList
   // traverseCircle2( glm::ivec2 const & center, int32_t radius );

   // static void
   // drawPixel( int32_t x, int32_t y, SetPixel_t const & setPixel );

   static void
   traverseCircle( int32_t r, SetPixel_t const & setPixel );

   static void
   traverseEllipse( int32_t a, int32_t b, SetPixel_t const & setPixel );

   // static void
   // drawLineEllipse( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel );

   static void
   drawLineH( int32_t x1, int32_t x2, int32_t y, SetPixel_t const & setPixel );

   static void
   drawLine( glm::ivec2 const & A, glm::ivec2 const & B, SetPixel_t const & setPixel );

   inline static void
   drawLine( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel )
   {
      drawLine( glm::ivec2( x1, y1 ), glm::ivec2( x2, y2 ), setPixel );
   }

   static void
   drawSuperCoverLine( glm::ivec2 const & A, glm::ivec2 const & B, SetPixel_t const & setPixel );

   inline static void
   drawSuperCoverLine( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel )
   {
      drawSuperCoverLine( glm::ivec2( x1, y1 ), glm::ivec2( x2, y2 ), setPixel );
   }

   static void
   drawRect( Recti pos, SetPixel_t const & setPixel );

   static void
   drawRectBorder( Recti pos, SetPixel_t const & setPixel );

//   static void
//   drawRect( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel );

//   static void
//   drawRectBorder( glm::ivec2 const & A, glm::ivec2 const & B, SetPixel_t const & setPixel );

//   inline static void
//   drawRectBorder( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel )
//   {
//      drawRectBorder( glm::ivec2( x1, y1 ), glm::ivec2( x2, y2 ), setPixel );
//   }

   static void
   drawTriangle( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, SetPixel_t const & setPixel );

   static void
   drawTriangleBorder( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, SetPixel_t const & setPixel );

   inline static void
   drawTriangleBorder( int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, SetPixel_t const & setPixel )
   {
      drawTriangleBorder( glm::ivec2( x1, y1 ), glm::ivec2( x2, y2 ), glm::ivec2( x3, y3 ), setPixel );
   }

//   void
//   floodFill( glm::ivec2 const & p,
//            std::function< void( int32_t, int32_t ) > const & getPixel,
//            std::function< void( int32_t, int32_t ) > const & setPixel,
//            SetPixel_t const & setPixel );


//   static void
//   drawFlatTopTriangle( int32_t x1, int32_t x2, int32_t y12, int32_t x3, int32_t y3,
//                        SetPixel_t const & setPixel );

//   static void
//   drawFlatBottomTriangle(  int32_t x1, int32_t y1, int32_t x2, int32_t x3, int32_t y23,
//                            SetPixel_t const & setPixel);
   static void
   drawCircleBorder( int32_t x, int32_t y, int32_t radius, SetPixel_t const & setPixel );

};

} // end namespace de.
