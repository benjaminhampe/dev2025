#pragma once
#include <de/Color.h>

namespace de {

// ===========================================================================
// ===   Bresenham Algorithms
// ===========================================================================
typedef std::function< void( s32, s32 ) > SetPixel_t;

// ===========================================================================
struct Bresenham
// ===========================================================================
{
    static void
    traverseEllipse( s32 a, s32 b, const SetPixel_t& setPixel );

    // static void
    // traverseEllipseBorder( s32 x1, s32 y1, s32 x2, s32 y2, const SetPixel_t& setPixel );

    static void
    traverseCircle( s32 r, const SetPixel_t& setPixel );

    static void
    traverseCircleBorder( s32 x, s32 y, s32 radius, const SetPixel_t& setPixel );

    static void
    traverseLineH( s32 x1, s32 x2, s32 y, const SetPixel_t& setPixel );

    static void
    traverseLine( s32 x1, s32 y1, s32 x2, s32 y2, const SetPixel_t& setPixel );

    inline static void
    traverseLine( const Recti& pos, const SetPixel_t& setPixel )
    {
        s32 x1 = pos.x;
        s32 y1 = pos.y;
        s32 x2 = pos.x2();
        s32 y2 = pos.y2();
        traverseLine(x1,y1,x2,y2,setPixel);
    }

    static void
    traverseSupercoverLine( s32 x1, s32 y1, s32 x2, s32 y2, const SetPixel_t& setPixel );

    static void
    traverseRect( s32 x1, s32 y1, s32 x2, s32 y2, const SetPixel_t& setPixel );

    inline static void
    traverseRect( const Recti& pos, const SetPixel_t& setPixel )
    {
        s32 x1 = pos.x;
        s32 y1 = pos.y;
        s32 x2 = pos.x2();
        s32 y2 = pos.y2();
        traverseRect(x1,y1,x2,y2,setPixel);
    }

    static void
    traverseRectBorder( s32 x1, s32 y1, s32 x2, s32 y2, const SetPixel_t& setPixel );

    inline static void
    traverseRectBorder( const Recti& pos, const SetPixel_t& setPixel )
    {
        s32 x1 = pos.x;
        s32 y1 = pos.y;
        s32 x2 = pos.x2();
        s32 y2 = pos.y2();
        traverseRectBorder(x1,y1,x2,y2,setPixel);
    }

    static void
    traverseTriangle( s32 Ax, s32 Ay, s32 Bx, s32 By, s32 Cx, s32 Cy, const SetPixel_t& setPixel );

    static void
    traverseTriangleBorder( s32 Ax, s32 Ay, s32 Bx, s32 By, s32 Cx, s32 Cy, const SetPixel_t& setPixel );

/*
    static void
    traverseLine( glm::ivec2 const & A, glm::ivec2 const & B, const SetPixel_t& setPixel );

    static void
    traverseSupercoverLine( glm::ivec2 const & A, glm::ivec2 const & B, const SetPixel_t& setPixel );

    static void
    drawTriangle( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, const SetPixel_t& setPixel );

    static void
    drawTriangleBorder( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, const SetPixel_t& setPixel );
*/
};

} // end namespace de.
