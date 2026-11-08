#pragma once
#include <de/smesh/SMesh.h>
//#include <de/gpu/smesh/SMeshHillplane.h>

namespace de {
namespace smesh {

// =========================================================================
/// @brief The Hexagon ( ver_2018 ):
// =========================================================================
///
///               M| x=0 | y=0 | z=0 | u=.5| v=.5|
///       D       -|-----|-----|-----|-----|-----|
///      / \      A|   0 |  0  | -.5 | .5  | 0   |
///   C /   \ E   B| -.5 |  0  |-.25 |  0  | 0.25|
///    |-----|    C| -.5 |  0  | .25 |  0  | 0.75|
///    |  M  |    D| 0.0 |  0  |  .5 |  .5 | 1   |
///    |-----|    E| 0.5 |  0  | .25 |  1  | 0.75|
///   B \   / F   F| 0.5 |  0  |-.25 |  1  | 0.25|
///      \ /
///       A       triangles: ABF, BCE, BEF, CDE
///

struct SMeshHexagon
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
    typedef glm::dvec3 V3d;
    typedef glm::dvec4 V4d;

    static void
    add(int i_min, int i_max,
        int j_min, int j_max,
        SMeshBuffer & mb,
        f32 w = 100.0f, f32 h = 100.0f,
        bool forceIndices = false,
        glm::vec3 const & pos = glm::vec3( 0.0f ) );

    static void
    add(int i,
        int j,
        SMeshBuffer & mb,
        f32 w = 100.0f, f32 h = 100.0f,
        u32 color = 0xFFFFFFFF,
        bool forceIndices = false,
        glm::vec3 const & pos = glm::vec3( 0.0f ) );

    static void
    addM4(SMeshBuffer & mb,
          f32 w = 100.0f, f32 h = 100.0f,
          u32 color = 0xFFFFFFFF,
          bool forceIndices = false,
          glm::dmat4 const & trs = glm::dmat4( 1.0 ) );

    static void
    add(SMeshBuffer & mb,
        f32 w, f32 h,
        u32 color,
        bool forceIndices = false,
        glm::vec3 const & pos = glm::vec3( 0.0f ) );

    static glm::ivec2
    getNeighborTileIndex( int corner, int i, int j );

    static glm::vec2
    getNeighborTilePos( int corner, f32 w, f32 h, int i, int j );

    static f32
    computeBoardPosX( f32 w, f32 h, int i, int j );

    static f32
    computeBoardPosY( f32 w, f32 h, int i, int j );

    static glm::ivec2
    computeTileIndex( f32 x, f32 y, f32 w, f32 h );


    static glm::vec3 cornerA3( f32 w, f32 h );
    static glm::vec3 cornerB3( f32 w, f32 h );
    static glm::vec3 cornerC3( f32 w, f32 h );
    static glm::vec3 cornerD3( f32 w, f32 h );
    static glm::vec3 cornerE3( f32 w, f32 h );
    static glm::vec3 cornerF3( f32 w, f32 h );

    static glm::vec2 lineCenter( glm::vec2 const & a, glm::vec2 const & b );

    static glm::vec2 cornerA( f32 w, f32 h );
    static glm::vec2 cornerB( f32 w, f32 h );
    static glm::vec2 cornerC( f32 w, f32 h );
    static glm::vec2 cornerD( f32 w, f32 h );
    static glm::vec2 cornerE( f32 w, f32 h );
    static glm::vec2 cornerF( f32 w, f32 h );

    static glm::vec2 edgeAB( f32 w, f32 h );
    static glm::vec2 edgeBC( f32 w, f32 h );
    static glm::vec2 edgeCD( f32 w, f32 h );
    static glm::vec2 edgeDE( f32 w, f32 h );
    static glm::vec2 edgeEF( f32 w, f32 h );
    static glm::vec2 edgeFA( f32 w, f32 h );

    static glm::vec2
    getCorner( int i, f32 w, f32 h );

    static std::vector< glm::vec2 >
    getCorners( f32 w, f32 h );

    static std::vector< glm::vec3 >
    getCornersXZ( f32 w, f32 h );

    static glm::vec2
    getEdge( int i, f32 w, f32 h );

    static f32
    computeAngle( glm::vec2 const & v );

    static f32
    computeCornerAngle( int i, f32 w, f32 h );

    static f32
    computeEdgeAngle( int i, f32 w, f32 h );

    /*
    static f32
    computeEdgeAngle( int i, f32 w, f32 h );

    static void
    test( f32 w, f32 h );

    static SMeshBuffer*
    create( f32 w, f32 h, u32 color );

    static void
    add( SMeshBuffer & out, int i, int j, f32 w, f32 h, u32 color );
    */
};


// =========================================================================
/// @brief SMeshBox = UnitCube * scale(x,y,z)
// =========================================================================
//
//        F-------G                 tex atlas infos         +----+----+---------+
//       /|      /|                                         |    |    |         |
//      / |  +Y / |                                         |    | +Z |         |
//     /  |    /  |                                         |    |    |         |
//    /   E---/---H   lookat = -X for plane PX              +----+----+----+----+
//   /-X /   / X /    <-------P  EyePos is +X for plane PX  |    |    |    |    |
//  /   /   / + /                                           | -X | -Y | +X | +Y |
// B-------C   /     +z                                     |    |    |    |    |
// |  /    |  /      |  +y                                  +----+----+----+----+
// | / -Y  | /       | /                                    |    |    |         |
// |/      |/        |/                                     |    | -Z |         |
// A-------D         0------> +x                            |    |    |         |
//
// =========================================================================
struct SMeshBox
// =========================================================================
{
    static void
    add(SMeshBuffer & o,
        glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
        glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h, u32 color = 0xFFFFFFFF);

    static void
    add(SMeshBuffer & o,
        glm::vec3 cubeSize, glm::vec3 pos, u32 color = 0xFFFFFFFF);

    static void
    addLines(SMeshBuffer & o,
             glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
             glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h, u32 color = 0xFFFFFFFF);

    static void
    addLines(SMeshBuffer & o,
             glm::vec3 cubeSize, glm::vec3 pos, u32 color = 0xFFFFFFFF);
};

// @brief Benni book example 2021
// =========================================================================
struct Generic3DCone
// =========================================================================
{
   // u32 getVertexCount();
   typedef std::function<u32()> FN_GET_VERTEXCOUNT;
   // void addVertex( f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, u32 color, f32 u, f32 v) ;
   typedef std::function<void(f32, f32, f32, f32, f32, f32, u32, f32, f32 )> FN_ADD_VERTEX;
   // void addIndex( u32 index );
   typedef std::function<void(u32)> FN_ADD_INDEX;

   static void
   addTrianglesXZ(
      f32 rx, f32 h, f32 rz,
      u32 innerColor,
      u32 outerColor,
      int32_t tessCircle,
      int32_t tessHeight,
      FN_GET_VERTEXCOUNT const & getVertexCount,
      FN_ADD_VERTEX const & addVertex,
      FN_ADD_INDEX const & addIndex );

};

// =========================================================================
struct SMeshCone
// =========================================================================
{
   static void
   addXZ( SMeshBuffer & o, glm::vec3 const & siz,
         u32 iColor = 0xAFFFFFFF, u32 oColor = 0xFFFFFFFF,
         int32_t tessCircle = 12,
         int32_t tessHeight = 1 );
};

// =========================================================================
struct SMeshPolyLine
// =======================================================================
{
   static glm::vec2 getOrtho2D( glm::vec2 const & m );
   static glm::vec2 getNormal2D( glm::vec2 const & m );

   // if N = 1 then it produces (1) StartLineCap and (1) EndLineCap (if any), or nothing if LineCap::None
   // if N = 2 then it produces (1) StartLineCap and (1) EndLineCap (if any) and (1) LineSegment
   // if N > 2 then it produces (1) StartLineCap and (1) EndLineCap (if any) and (N-1) LineSegments and (N-2) LineJoins
   // Does only produce Bevel Join for speed,
   // Does not compute angle between ABC, just add normals and divides by 2 to get nABC = 1/2 * (nAB + n_BC)
   // Does not compute intersection points for speed reasons.
   // Produces LineJoins exactly the same way as segments for symmetry reasons, always AL,AR,BL,BR
   // So always Left first, then right then the next segment or linejoin from left to right.

   // So 3 points ABC produce TriangleStrip [SL,SR,]AL,AR,BL,BR,CL,CR,DL,DR[,EL,ER] depending on LineCaps,
   // LineJoin is always produced just by symmetric adding points. But its always beveled join.
   // Should be very fast because of benni symmetry.

   // SL = AL - dAB,  AL = A - nAB,  BL = B - nAB,  CL = B - nBC,  DL = C - nBC,  EL = DL + dBC
   // SR = AR - dAB,  AR = A + nAB,  BR = B + nAB,  CR = B + nBC,  DR = C + nBC,  ER = DR + dBC
   //
   //     SL-------AL------------BL----------CL------------DL--------EL
   //     |      / |           / |         / |           / |       / |
   //     |  Start |   Segment   |    Join   |   Segment   |   End   |
   //     S    /   A      /      B     /     B      /      C    /    E
   //     |   Cap  |   i of N-1  |  i of N-2 |  i+1 of N-1 |   Cap   |
   //     | /      | /           | /         | /           | /       |
   //     SR-------AR------------BR----------CR------------DR--------ER
   //
   static bool
   addTriangleStrip( SMeshBuffer & o,
        std::vector< glm::vec2 > const & points,
        u32 color = 0xFFFFFFFF,
        f32 lineWidth = 1.0f,
        LineCap lineCap = LineCap::Square,
        LineJoin lineJoin = LineJoin::Bevel );

   // LineCap:None, Quad: AL,BL,BR,AR = Triangle AL,BL,BR + Triangle BR,AR,AL
   //
   //     AL------------BL
   //     |--           |        AL = A-nAB, BL = B-nAB
   //     |  ---        |        AR = A+nAB, BR = B+nAB
   // ----A-------------B-----
   //     |        ---  |
   //     |           --|
   //     AR------------BR
   //
   // LineCap:Square, Quad SE ( extruded AB ) Quad: SL,EL,ER,SR
   //
   //     SL-------------------------------EL
   //     |------            TriSL,EL,ER   |    SL = AL-dAB, EL = BL+dAB
   //     |      ------                    |    SR = AR-dAB, ER = BR+dAB
   //     S        A    ------     B       E
   //     |                    ------      |
   //     |  TriER,SR,SL             ------|
   //     SR-------------------------------ER
   //
   static void
   addExtrudedLine( SMeshBuffer & o, glm::vec2 const & a, glm::vec2 const & b,
         u32 colorA = 0xFFFFFFFF, u32 colorB = 0xFFFFFFFF, f32 lineWidth = 1.0f,
         LineCap lineCap = LineCap::Square );
};


struct SMeshRect
{
   typedef glm::vec3 V3;

   static void
   addLine2D( SMeshBuffer & o,
              int ax, int ay, int bx, int by, u32 colorA, u32 colorB );

   static void
   add( SMeshBuffer & o,
        V3 const & a, V3 const & b, V3 const & c, V3 const & d, u32 color = 0xFFFFFFFF,
        f32 u1 = 0.0f, f32 v1 = 0.0f, f32 u2 = 1.0f, f32 v2 = 1.0f );

   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   static void
   add( SMeshBuffer & o, Recti const & pos, u32 colorA, u32 colorB,
        u32 colorC, u32 colorD, gpu::TexRef const & ref );

   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   //
   static void
   addOutline( SMeshBuffer & o, Recti const & pos,
               u32 color,
               gpu::TexRef const & ref, int borderWidth );

   static void
   addOutline( SMeshBuffer & o, Recti const & pos,
              u32 colorA, u32 colorB, u32 colorC, u32 colorD,
              gpu::TexRef const & ref, int borderWidth );

};

//
// Vertices:   12 + 4*tess
//
//       x0  x1  x2  x3
// ry    |   |   |   |
//       |
// y0 ---+   G---H   tess=1   (CW) TopQuad:     BGHC(1,6,7,2)
//           |  /|  /         (CW) BottomQuad  LADK(11,0,3,10)
//         ry| / | /
//           |/  |/       A=0, B=1, C=2, D=3, E=4, F=5, G=6, H=7, I=8, J=9, K=10, L=11
// y1 -  F---B---C---I
//       |  /|  /|  /|
//       | / | M | / |    Round Corners as triangles:
//       |/  |/  |/  |
// y2 -  E---A---D---J        (CW) top-left = FGB
//        rx |  /|            (CW) top-right = HIC
//           | / |            (CW) bottom-right = JKD
//           |/  |            (CW) bottom-left = LEA
// y3 ---+   L---K
//
// =========================================================================
struct Generic2DTriangleRoundRect
// =========================================================================
{
   typedef std::function<u32()> GET_VERTEXCOUNT;
   // void addVertex( f32 x, f32 y, u32 color )
   typedef std::function<void(f32,f32, u32)> ADD_VERTEX;
   // void addVertex( f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, u32 color, f32 u, f32 v )
   //typedef std::function<void(f32,f32,f32, f32,f32,f32, u32, f32,f32 )> ADD_VERTEX;
   typedef std::function<void(u32)> ADD_INDEX;

   static void
   addTriangles(
      f32 w, f32 h, f32 rx, f32 ry,
      u32 tess, u32 innerColor, u32 outerColor,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex );
};

// =========================================================================
struct SMeshRoundRect
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o,
         glm::vec3 const & pos,
         f32 w, f32 h, f32 rx, f32 ry,
         int32_t tess = 13,
         u32 innerColor = 0xFFFFFFFF,
         u32 outerColor = 0xFFFFFFFF );

   static void
   addXZ( SMeshBuffer & o,
         glm::vec3 const & pos,
         f32 w, f32 h, f32 rx, f32 ry,
         int32_t tess = 13,
         u32 innerColor = 0xFFFFFFFF,
         u32 outerColor = 0xFFFFFFFF );
};

void
addRoundRect( SMeshBuffer & o,
    Recti const & pos,
    f32 zIndex = 0.0f,
    glm::ivec2 const & radius = glm::ivec2(8,8),
    u32 outerColor = 0xffffffff,
    u32 innerColor = 0xffffffff,
    gpu::TexRef const & ref = gpu::TexRef(),
    int tess = 13 );

// Vertices = 8 + 4*(tess)
// Indices = 2 * Primitives
// Primitives = 4 + 4*(tess+1) lines
//
//       x0  x1  x2  x3
// ry    |   |   |   |
//       |
// y0 ---+   G---H
//        (         )     A,B,C,D are not used for the outline
//    ry (           )
//       (           )    E=0, F=1, G=2, H=3, I=4, J=5, K=6, L=7, n = 8
// y1 -  F   B   C   I
//       |           |     Lines: EF, GH, IJ, KL
//       |     M     |            + lineFG[tess] + lineHI[tess] + lineJK[tess] + lineLE[tess]
//       |           |
// y2 -  E   A   D   J        (CW) top-left = FGB
//       (           )        (CW) top-right = HIC
//        (         )         (CW) bottom-right = JKD
//         (       )          (CW) bottom-left = LEA
// y3 ---+   L---K
//
void
addRoundRectOutline( SMeshBuffer & o,
    Recti const & pos, f32 zIndex = 0.0f,
    glm::ivec2 const & radius = glm::ivec2(8,8),
    u32 color = 0xffffffff,
    gpu::TexRef const & ref = gpu::TexRef(),
    int borderWidth = 1,
    u32 tess = 13 );

// =========================================================================
// Circle
// =========================================================================

void
addCircle( SMeshBuffer & o, Recti const & pos, u32 outerColor,
           u32 innerColor, gpu::TexRef const & ref, u32 tess = 12 );

void
addCircleOutline( SMeshBuffer & o, Recti const & pos, u32 color,
                  gpu::TexRef const & ref, int borderWidth, u32 tess = 12 );

// =========================================================================
struct SMeshCircle
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o, glm::vec2 const & siz,
          u32 color, u32 tessRadial = 33 );

   static void
   addXY( SMeshBuffer & o, glm::vec2 const & siz,
          u32 innerColor, u32 outerColor, u32 tessRadial );

   static void
   addXZ( SMeshBuffer & o, glm::vec2 const & siz,
          u32 color, u32 tessRadial = 33 );

   static void
   addXZ( SMeshBuffer & o, glm::vec2 const & siz,
          u32 innerColor, u32 outerColor, u32 tessRadial );
};



// =========================================================================
struct Generic2DTriangleRing
// =========================================================================
{
   // u32 getVertexCount();
   typedef std::function<u32()> GET_VERTEXCOUNT;
   // void addVertex( f32 x, f32 y, u32 color );
   typedef std::function<void(f32, f32, u32)> ADD_VERTEX;
   // void addIndex( u32 index );
   typedef std::function<void(u32)> ADD_INDEX;

   static void
   addIndexedTriangles(
      f32 outer_x, f32 outer_y,
      f32 inner_x, f32 inner_y,
      int32_t tess, u32 innerColor, u32 outerColor,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex );
};

// =========================================================================
struct SMeshRing
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o,
         f32 ow,
         f32 oh,
         f32 iw,
         f32 ih,
         u32 innerColor = 0xFFFFFFFF,
         u32 outerColor = 0xFFFFFFFF,
         int32_t tess = 13,
         glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addXZ(
      SMeshBuffer & o,
      f32 ow,
      f32 oh,
      f32 iw,
      f32 ih,
      u32 innerColor = 0xFFFFFFFF,
      u32 outerColor = 0xFFFFFFFF,
      int32_t tess = 13,
      glm::vec3 const & pos = glm::vec3(0,0,0) );
};


// =========================================================================
/// @brief PolyHull
// =========================================================================
struct SMeshPolyHull
{
   static void
   addLeftStripXY( SMeshBuffer & o, std::vector< glm::vec3 > const & points, f32 height,
                   u32 color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addLeftStripXZ( SMeshBuffer & o, std::vector< glm::vec3 > const & points, f32 height,
                   u32 color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addRightStrip( SMeshBuffer & o, std::vector< glm::vec3 > const & points, f32 height,
                  u32 color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );
};

// =========================================================================
struct SMeshCylinderHull
// =========================================================================
{
   typedef glm::vec3 V3;

   static void
   addXY( SMeshBuffer & o,
        f32 dx, f32 dy, f32 h,
        u32 color = 0xFFFFFFFF, int tessCirc = 33,
        V3 const & pos = V3(0,0,0) );

   static void
   addXZ( SMeshBuffer & o,
        f32 dx, f32 h, f32 dz,
        u32 color = 0xFFFFFFFF, int tessCirc = 33,
        V3 const & pos = V3(0,0,0) );
};

// =========================================================================
struct SMeshCylinder
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o, f32 dx, f32 dy, f32 h,
          u32 color = 0xFFFFFFFF, u32 tessCirc = 33 );

   static void
   addXZ( SMeshBuffer & o, f32 dx, f32 h, f32 dz,
          u32 color = 0xFFFFFFFF, u32 tessCirc = 33 );

   static void
   addXZ( SMeshBuffer & o, f32 bottom_dx, f32 bottom_dz,
          f32 top_dx, f32 top_dz, f32 cylinder_h,
          u32 color = 0xFFFFFFFF, u32 tessCirc = 33, u32 tessHull = 11,
          glm::vec3 const & offset = glm::vec3() );

};

// =========================================================================
struct SMeshTube
// =========================================================================
{
   static void
   add( SMeshBuffer & o,
        // Describes outer cylinder
        f32 dx, // (outer) cylinder width
        f32 dz, // (outer) cylinder depth
        f32 hy, // (outer) cylinder height
        // Describes inner cylinder, rx,ry define the extrusion relative inside to dx,dy.
        // They define the inside border width and are not absolute values like dx,dy.
        f32 rx, // inside going border-width ( inside cylinder width cx = dx - rx )
        f32 rz, // inside going border-depth ( inside cylinder depth cz = dz - rz )
        u32 color = 0xFFFFFFFF, // one color to rule them all (vertices).
        int32_t tessCircle = 13, // tesselation of top and bottom ring = num ring segments
        int32_t tessHeight = 1 ); // tesselation of cylinder hull height ( y-direction )
};

// =========================================================================
struct SMeshSphere
// =========================================================================
{
   // [Longitude] Rotation um die y-Achse in degrees, equatorial xz plane is ground plane +Y is north pole
   // [Latitude] Rotation um die x/z-Achse in degrees: meridian plane (yx or yz)
   //
   static glm::vec3
   computeNormal( f32 lon_degrees, f32 lat_degrees );

   static void
   add( SMeshBuffer & o,
         glm::vec3 const & siz = glm::vec3(100,100,100),
         u32 color = 0xFFFFFFFF,
         u32 tessLon = 9,
         u32 tessLat = 9,
         glm::vec3 const & offset = glm::vec3() );
};

void
addSphere( SMeshBuffer & o,
      glm::vec3 const & siz = glm::vec3(100,100,100),
      glm::vec3 const & offset = glm::vec3(),
      u32 tessLon = 3, u32 tessLat = 3, u32 color = 0xFFFFFFFF );

// =========================================================================
struct SMeshUpperHalfSphere
// =========================================================================
{
   typedef glm::vec3 V3;
   typedef u32 u32;

   // [Longitude] Rotation um die y-Achse in degrees, equatorial xz plane is ground plane +Y is north pole
   // [Latitude] Rotation um die x/z-Achse in degrees: meridian plane (yx or yz)
   // static V3
   // computeSphereNormal( f32 lon, f32 lat ); // in degrees

   static void
   add( SMeshBuffer & o, V3 const & siz = V3(100,100,100), u32 color = 0xFFFFFFFF,
         u32 tessLon = 3, u32 tessLat = 3, V3 const & offset = V3() );
};

// =========================================================================
struct SMeshHexagonHull
// =========================================================================
{
    static SMeshBuffer
    create( f32 w, f32 h, f32 height, u32 color );
};

/*
// =========================================================================
//struct MeshHexagonOutline
// =========================================================================
//{
//   typedef std::vector< glm::vec3 > PointList;
//};

static SMeshBuffer
create( f32 w, f32 h, f32 lineWidth, u32 color )
{
   auto addPolyLine = [&] ( SMeshBuffer & o, PointList const & points, f32 lineWidth, u32 color )
   {
      if ( points.size() < 2 )
      {
         return;
      }
      auto addSegment = [&] ( glm::vec3 p, glm::vec3 q )
      {
         glm::vec2 m = Math::getNormal2D( glm::vec2( q ) - glm::vec2( p ) );
         glm::vec3 n( 0.5f * lineWidth * m.x, 0.5f * lineWidth * m.y, 0.0f );
         S3DVertex A( a-n, 0,1,0, color, 0,0 );
         S3DVertex A( a-n, 0,1,0, color, 0,0 );
         Quad::add( o, a - n, b - n, b + n, a + n, color  );
      };

      glm::vec3 a = points[ 0 ];
      for ( size_t i = 1; i < points.size(); ++i )
      {
         glm::vec3 b = points[ i ];
         addSegment( a, b );
         a = b;
      }
   }
   SMeshBuffer o( PrimitiveType::Triangles );
   auto points = MeshHexagon::getCorners(w,h);
   PolyLine::add( o, points, lineWidth, color );
   return o;
}

void
addPioneerCity( SMesh & out, f32 dx, f32 dy, f32 dz, u32 color )
{
   out.setName( "PioCity" );

   SMeshBuffer ground;
   Cube::add( ground, dx, 0.5f * dy, dz, color );
   SMeshBufferTool::translateVertices( ground, 0, 0.25f*dy, 0 );
   out.addMeshBuffer( ground );

   SMesh house;
   PioneerHome::add( house, 0.5f * dx, 0.5f * dy, dz, color );
   //house->rotateVertices( 0, 20, 0 );
   SMeshTool::translateVertices( house, 0.25f * dx, 0.5f*dy, 0 );
   out.addMesh( house );
}

   static f32
   computeAngle( glm::vec2 const & v )
   {
      glm::vec2 const y(0,1);
      return acos( glm::dot(v,y) / glm::length(v) ) * Math::RAD2DEG;
   }

   static std::vector< Triangle3f >
   getTriangles( f32 w, f32 h, glm::vec3 const & offset = glm::vec3() )
   {
      glm::vec3 A = offset + cornerA( w,h );
      glm::vec3 B = offset + cornerB( w,h );
      glm::vec3 C = offset + cornerC( w,h );
      glm::vec3 D = offset + cornerD( w,h );
      glm::vec3 E = offset + cornerE( w,h );
      glm::vec3 F = offset + cornerF( w,h );
      return { {B,A,F}, {B,F,E}, {B,E,C}, {C,E,D} };
   }

   static SMeshBuffer*
   create( f32 w, f32 h, u32 color = 0xFFFFFFFF );

   // Connect Corners.Tiles
   static glm::ivec2
   getCornerA_NeighborTileIndex( int neighbor012, int i, int j )
   {
      if ( neighbor012 == 0 )
      {
      return {   i, j-1 };
      }
      else if ( neighbor012 == 1 )
      {
      return {   i, j-1 };
      }
      else if ( neighbor012 == 2 )
      {
      return {   i, j-1 };
      }
      else
      {
      throw std::runtime_error("Invalid neighbor012()");
      }
   }

   SMeshBuffer*
   MeshHexagon::create( f32 w, f32 h, u32 color )
   {
     SMeshBuffer* o = SMeshBufferTool::createTriangles();
     S3DVertex const A( 0.0f,    -.5f*h, 0.0f, 0,0,1, color, 0.5f, 1.0f  );
     S3DVertex const B( -0.5f*w,-.25f*h, 0.0f, 0,0,1, color, 0.0f, 0.75f );
     S3DVertex const C( -0.5f*w, .25f*h, 0.0f, 0,0,1, color, 0.0f, 0.25f );
     S3DVertex const D( 0.0f,     .5f*h, 0.0f, 0,0,1, color, 0.5f, 0.0f  );
     S3DVertex const E( 0.5f*w,  .25f*h, 0.0f, 0,0,1, color, 1.0f, 0.25f );
     S3DVertex const F( 0.5f*w, -.25f*h, 0.0f, 0,0,1, color, 1.0f, 0.75f );
     Triangle::addFront( *o, B, A, F );
     Triangle::addFront( *o, B, F, E );
     Triangle::addFront( *o, B, E, C );
     Triangle::addFront( *o, C, E, D );
     o.recalculateBoundingBox();
     return o;
   }

   static f32
   computeEdgeAngle( int i, f32 w, f32 h )
   {
      glm::vec3 p, q;

      switch ( i )
      {
         case 0: p = cornerA( w,h ); q = cornerB( w,h ); break;
         case 1: p = cornerB( w,h ); q = cornerC( w,h ); break;
         case 2: p = cornerC( w,h ); q = cornerD( w,h ); break;
         case 3: p = cornerD( w,h ); q = cornerE( w,h ); break;
         case 4: p = cornerE( w,h ); q = cornerF( w,h ); break;
         case 5: p = cornerF( w,h ); q = cornerA( w,h ); break;
         default: throw std::runtime_error("Invalid i in computeEdgeAngle()");
      }

      f32 phi = computeAngle( q - p );
      DE_DEBUG("i=",i,", w=",w,", h=",h,", p=",p,", q=",q, ", phi=",phi)
   }

   static f32
   getEdgeAngle( int i, f32 w, f32 h )
   {
      // i = i % 6;
      switch ( i )
      {
         case 0: return -30.0f;
         case 1: return 90.0f;
         case 2: return 30.0f;
         case 3: return -30.0f;
         case 4: return 90.0f;
         case 5: return 30.0f;
         default: throw std::runtime_error("Invalid i in getEdgeAngle()");
      }
   }
*/

} // end namespace gpu.
} // end namespace de.


