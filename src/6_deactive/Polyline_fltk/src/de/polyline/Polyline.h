//////////////////////////////////////////////////////////////////////
/// @file   Polyline.hpp
/// @brief  SVG Miter PolyLine (miter line joins, fastest polyline)
/// @author Copyright (c) Benjamin Hampe <benjaminhampe@gmx.de>
///
///	The author grants permission of free use, free distribution and
/// free usage for commercial applications even for modified sources.
///
///	What is not allowed is to remove the original authors name or claim
/// any intellectual rights. This original code was written in my spare time
/// and is distributed publicly as free software to anyone over GitLab and GitHub.
///
//////////////////////////////////////////////////////////////////////

#pragma once
#include <DarkImage.h>

//#ifndef USE_POLYLINE_DEBUG_LOGGING
//#define USE_POLYLINE_DEBUG_LOGGING
//#endif

namespace de {
namespace polyline {

/// @brief LineCaps for Start- and EndSegment of a polyline by SVG standard
struct LineCap
{
    enum ELineCap
    {
        None = 0,       // SVG lineCap = "butt"
        Round,          // SVG lineCap = "round"
        Square,         // SVG lineCap = "square"
        Count,
        Default = Round,
        Butt = None
    };
};

/// @brief LineJoins between two segments of a polyline by SVG standard
struct LineJoin
{
    enum ELineJoin
    {
        None = 0,   // No visible line-join
        Bevel,      // Add one or two triangles == AUTO_DOUBLE_BEVEL
        Round,      // For now a circular, not elliptic shape, because lineWidth is const for all segments ( for now )
        Miter,      // Not used, produces too large and ugly triangles. TODO: Miter should reuse already computed intersection point
        Count,	// Indicates invalid state, also counts the enum-items before
        Default = Bevel
    };
};

// =======================================================
class Polyline
// =======================================================
{
public:
    typedef glm::vec2 V2;

    // Produces 2*(N-1) triangles = 6*(N-1) vertices
    // polyline: vector of 2D points (x,y)
    // lineWidth: full width of the strip
    static inline void buildQuadStrip2D(
        const std::vector<V2>& polyline,
        float lineWidth,
        std::vector<V2>& outVerts)
    {
        outVerts.clear();
        if (polyline.size() < 2)
            return;

        const float w = lineWidth * 0.5f;

        for (size_t i = 0; i + 1 < polyline.size(); ++i)
        {
            const V2& P0 = polyline[i];
            const V2& P1 = polyline[i + 1];

            V2 t = glm::normalize(P1 - P0);
            V2 n = V2(-t.y, t.x); // 2D perpendicular

            V2 L0 = P0 + n * w;
            V2 R0 = P0 - n * w;
            V2 L1 = P1 + n * w;
            V2 R1 = P1 - n * w;

            // Triangle 1
            outVerts.push_back(L0);
            outVerts.push_back(R0);
            outVerts.push_back(R1);

            // Triangle 2
            outVerts.push_back(L0);
            outVerts.push_back(R1);
            outVerts.push_back(L1);
        }
    }


    inline static V2
    getOrtho2D( V2 const & m ) { return V2( m.y, -m.x ); }

    inline static V2
    getNormal2D( V2 const & m ) { return glm::normalize( getOrtho2D( m ) ); }

    inline static float
    getAngleABC( V2 const & A, V2 const & B, V2 const & C )
    {
        auto const ABx = f64( B.x ) - f64( A.x );
        auto const ABy = f64( B.y ) - f64( A.y );
        auto const BCx = f64( C.x ) - f64( B.x );
        auto const BCy = f64( C.y ) - f64( B.y );
        auto const phiAB = ::atan2( ABy, ABx );
        auto const phiBC = ::atan2( BCy, BCx );
        auto const phiABC = static_cast< float >( phiBC - phiAB );
        #if 1
        DE_BENNI("phi(",phiABC,"), ",( phiABC > 0.0f ) ? "Left=CCW" : "Right=CW",", ",
                "A(",A,"), B(",B,"), C(",C,"), ",
                "AB(",B - A,"), phiAB(",phiAB,")",
                "BC(",C - B,"), phiBC(",phiBC,")")
        #endif
        return phiABC;
    }

    inline static void
    testMath()
    {
        V2 m( 1, 0 );
        DE_DEBUG("m1(",m,"), o1(",getOrtho2D( m ),"), n1(",getNormal2D( m ),")")
        m = V2( 0, 1 );
        DE_DEBUG("m2(",m,"), o2(",getOrtho2D( m ),"), n2(",getNormal2D( m ),")")
        m = V2( 1, 1 );
        DE_DEBUG("m3(",m,"), o3(",getOrtho2D( m ),"), n3(",getNormal2D( m ),")")
    }

    // @param void.
    // @return Num points.
    typedef std::function<uint32_t()>
        FN_getPointCount;

    // @param Point index.
    // @return Point position XY.
    typedef std::function<V2(uint32_t)>
        FN_getPoint;

    // @param void.
    // @return Num vertices is needed to compute correct indices
    //         if mesh already vertices. Called once at begin.
    typedef std::function<uint32_t()>
        FN_getVertexCount;

    // @param V2 posXY.
    // @param V2 normalXY.
    // @param uint32_t colorRGBA - e.g. 0xAABBGGRR in little-endian
    // @param V2 texUV.
    // @return void.
    typedef std::function<void(V2,V2,uint32_t,V2)>
        FN_addVertex;

    // uint32_t indexA.
    // uint32_t indexB.
    // uint32_t indexC.
    // @return void.
    typedef std::function<void(uint32_t,uint32_t,uint32_t)>
        FN_addIndexedTriangle;

    static bool
    asTriangles(
        const FN_getPointCount&      getPointCount,     // in
        const FN_getPoint&           getPoint,          // in
        const FN_getVertexCount&     getVertexCount,    // in
        const FN_addVertex&          addVertex,         // out
        const FN_addIndexedTriangle& addIndexedTriangle,// out
        const LineCap::ELineCap&     lineCap,           // in
        const LineJoin::ELineJoin&   lineJoin)          // in
    {
    uint32_t ghost = 0x80FFFFFF;
    uint32_t black = 0xFF000000;
    uint32_t white = 0xFFFFFFFF;
    uint32_t red = 0xFFFF0000;
    uint32_t green = 0xFF00FF00;
    uint32_t blue = 0xFF0000FF;
    uint32_t yellow = 0xFFFFFF00;
    uint32_t orange = 0xFFFF8000;

    auto n = getPointCount();
    //auto v = getVertexCount();

    DE_DEBUG("[Begin] Polyline( cap:",lineCap,", join:",lineJoin,", n:",n," ) as Scalable Triangles:")
    if ( n < 2 )
    {
        if ( n == 1 )
        {
            DE_WARN("INFORMATION-LOSS!")
        }
        return false; // nothing todo, empty input
    }

    // [LineCapStart]
    //
    //    SL-----AL
    //    |     /|
    //    |    / |    --> dAB
    //    S------A------------B
    //    |  /   |      |
    //    | /    |      v
    //    SR-----AR     +nAB
    //
    auto addLineCapStart = [ & ] ( V2 const & A, V2 const & B, LineCap::ELineCap lineCap )
    {
        uint32_t color = 0xFFAA3388;
        auto v = getVertexCount();
        auto dAB = glm::normalize(B - A);
        auto nAB = getOrtho2D( dAB );      // Compute normal nAB
        DE_DEBUG("addLineCapStart( cap:",lineCap,", A:",A,", B:",B,", color:",color," )")

        addVertex( A, -nAB - dAB, color                 , V2(0,0) ); // SL
        addVertex( A,  nAB - dAB, varyColor( color, 30 ), V2(0,1) ); // SR
        addVertex( A, -nAB,       color                 , V2(1,0) ); // AL
        addVertex( A,  nAB,       varyColor( color, 30 ), V2(1,1) ); // AR
        addIndexedTriangle( v, v + 1, v + 2 );     // Triangle SL,SR,AL ccw 012
        addIndexedTriangle( v + 1, v + 3, v + 2 ); // Triangle SR,AR,AL ccw 132
    };

    // [LineCapEnd]
    //                 BL-----EL
    //                 |     /|
    //      --> dAB    |    / |
    //    A------------B------E
    //      |          |  /   |
    //      v          | /    |
    //      nAB        BR-----ER
    //
    auto addLineCapEnd = [ & ] ( V2 const & A, V2 const & B, LineCap::ELineCap lineCap )
    {
        uint32_t color = 0xFFAAC388;
        auto v = getVertexCount();
        auto dAB = glm::normalize(B - A);
        auto nAB = getOrtho2D( dAB );      // Compute normal nAB
        DE_DEBUG("addLineCapEnd( cap:",lineCap,", A:",A,", B:",B,", color:",color," )")

        addVertex( B, -nAB,       color                 , V2(0,0) ); // BL
        addVertex( B,  nAB,       varyColor( color, 30 ), V2(0,1) ); // BR
        addVertex( B, -nAB + dAB, color                 , V2(1,0) ); // EL
        addVertex( B,  nAB + dAB, varyColor( color, 30 ), V2(1,1) ); // ER
        addIndexedTriangle( v,     v + 1, v + 2 ); // Triangle BL,BR,EL ccw 012
        addIndexedTriangle( v + 1, v + 3, v + 2 ); // Triangle BR,ER,EL ccw 132
    };
    // [SegmentAB]
    //
    //    AL-----BL   A,B are stored, and their normal displacements +-nAB.
    //    |     /|
    //    |    / |
    //    A------B
    //    |  /   |
    //    | /    |
    //    AR-----BR
    //
    auto addLineSegment = [ & ] ( V2 const & A, V2 const & B, uint32_t color )
    {
        auto v = getVertexCount();
        auto dAB = glm::normalize( V2( B ) - V2( A ) );
        auto nAB = getOrtho2D( dAB );      // Compute normal nAB
        DE_DEBUG("addLineSegment( A:",A,", B:",B,", dAB:",dAB,", nAB:",nAB," )")

        addVertex( A, -nAB, color                 , V2(0,0) ); // AL
        addVertex( A,  nAB, varyColor( color, 30 ), V2(0,1) ); // AR
        addVertex( B, -nAB, varyColor( color, 30 ), V2(1,0) ); // BL
        addVertex( B,  nAB, varyColor( color, 30 ), V2(1,1) ); // BR
        addIndexedTriangle( v,     v + 1, v + 2 ); // Triangle AL,AR,BL ccw 012
        addIndexedTriangle( v + 1, v + 3, v + 2 ); // Triangle AR,BR,BL ccw 132
    };

    // Add Line Join between points A,B,C with possible type Bevel, Miter or Round )
    /**
    // turn left ( math positive, ccw )
    //
    //     DL---C----DR
    //      \    \    \        // SL = B - 0.5 ( nAB + nBC );
    //       \    \    \       // CR = B + nBC;
    //        \    \    \      // BR = B + nAB;
    //         \    \    \
    // AL-------SL   \    \
    // |    ----  -   \    \
    // |----        -  \    \
    // A----------------B----CR
    // |--------        |    /
    // |        --------|  /
    // AR---------------BR/
    //
    // turn right ( math negative, cw )
    //
    // AL---------------BL\       AL,AR,BL + AR,BL,
    // |        --------|  \
    // |--------        |    \
    // A----------CR----B----CL
    // |----      /    /|   /
    // |    ---- / -  / |  /
    // AR-------/----/--BR/
    //         /    /    /        // SR = B + 0.5 ( nAB + nBC );
    //        /    /    /         // BL = B - nBC;
    //       /    /    /          // CL = B - nAB;
    //      /    /    /
    //     DR---C----DL
    */
    auto addLineJoin = [ & ] ( V2 const & a, V2 const & b, V2 const & c, LineJoin::ELineJoin const & lineJoin )
    {
        auto v = getVertexCount();
        auto n00 = V2( 0, 0 );
        auto nAB = getNormal2D( b - a );
        auto nBC = getNormal2D( c - b );
        auto phi = getAngleABC( a, b, c );    // Compute angle between vector AB and BC

        #ifdef USE_POINT_SKIPPING
        // if (parallel) then combine both segments by skipping this one.
        if ( std::abs( phi ) < 0.00001f || std::abs( phi - Math::TWO_PI ) < 0.00001f )
        {
            B = C;                        // Skip current segment
            nAB = getNormal2D( B - A );   // Update normal nAB
            std::cout,__FUNCTION__," :: Skip parallel point(",i,")\n";
            continue;
        }
        #endif

        // Right Line Join ( Bevel, Miter or Round )
        if ( phi > 0.0f ) // turn left ( math positive, ccw )
        {
            DE_DEBUG("addLineJoin( Left, phi:",phi," join:",lineJoin," ) A:",a,", B:",a,", C:",c)
            uint32_t color = red;
            //std::cout,"Polyline turns left, adds line join right, BR(",BR,"), CR(",CR,")\n";
            if ( lineJoin == LineJoin::Bevel )
            {
                //addVertex3D( B, n00, color );  // B
                //addVertex3D( B, nAB, color );  // BR = B + nAB
                //addVertex3D( B, nBC, color );  // CR = B + nBC
                //addIndexedTriangle( v, v + 1, v + 2 ); // Bevel Triangle B,BR,CR
                addIndexedTriangle( v + 1, v, v - 1 ); // Bevel Triangle CR,CL,BR
            }
        }
        // Left Line Join, CL, BL, CR ( Bevel, Miter or Round )
        else
        {
            DE_DEBUG("addLineJoin( Right, phi:",phi," join:",lineJoin," ) A:",a,", B:",b,", C:",c)
            uint32_t color = blue;
            //std::cout,"Polyline turns right, adds line join left, BL(",BL,"), CL(",CL,")\n";
            if ( lineJoin == LineJoin::Bevel )
            {
                //               addVertex3D( B,  n00, color ); // B
                //               addVertex3D( B, -nBC, color ); // CL
                //               addVertex3D( B, -nAB, color ); // BL
                //               addIndexedTriangle( v, v + 1, v + 2 ); // Left Bevel Triangle B,CL,BL ccw;
                addIndexedTriangle( v, v - 2, v + 1 ); // Bevel Triangle CL,BL,CR
            }
        }
    };

    //
    // Local variables:
    //
    uint32_t baseColor = green;
    V2 A,B,C; // We also need normals for both line-segments AB and BC
    A = getPoint( 0 );    // Fetch A
    B = getPoint( 1 );    // Fetch B
    addLineCapStart( A, B, lineCap );
    addLineSegment( A, B, baseColor );
    for ( size_t i = 2; i < n; ++i )
    {
        C = getPoint( i ); // Fetch C
        // Add Join ABC
        float phi = getAngleABC( A, B, C );    // Compute angle between vector AB and BC just for debug coloring
        uint32_t color = 0xFF00FFFF;
        if ( phi < 0.0f )
        {
            color = 0xFFFF00FF;
        }
        addLineJoin( A, B, C, lineJoin );
        // Add Segment BC
        addLineSegment( B, C, varyColor( baseColor, 30 ) );
        // Next point
        A = B;
        B = C;
    }
    addLineCapEnd( A, B, lineCap );
    return true;
}

};

} // end namespace polyline
} // end namespace de
