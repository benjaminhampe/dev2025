#pragma once
#include "SMesh.hpp"
#include "SMeshTool.hpp"
#include "SMeshHillplane.hpp"
namespace de {
namespace gpu {

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
   add( SMeshBuffer & o,
         glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
         glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h,
         uint32_t color = 0xFFFFFFFF );

   static void
   add( SMeshBuffer & o, glm::vec3 cubeSize, glm::vec3 pos, uint32_t color = 0xFFFFFFFF );
};


// @brief Benni book example 2021
// =========================================================================
struct Generic3DCone
// =========================================================================
{
   // uint32_t getVertexCount();
   typedef std::function<uint32_t()> FN_GET_VERTEXCOUNT;
   // void addVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t color, float u, float v) ;
   typedef std::function<void(float, float, float, float, float, float, uint32_t, float, float )> FN_ADD_VERTEX;
   // void addIndex( uint32_t index );
   typedef std::function<void(uint32_t)> FN_ADD_INDEX;

   static void
   addTrianglesXZ(
      float rx, float h, float rz,
      uint32_t innerColor,
      uint32_t outerColor,
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
         uint32_t iColor = 0xAFFFFFFF, uint32_t oColor = 0xFFFFFFFF,
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
        uint32_t color = 0xFFFFFFFF,
        float lineWidth = 1.0f,
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
         uint32_t colorA = 0xFFFFFFFF, uint32_t colorB = 0xFFFFFFFF, float lineWidth = 1.0f,
         LineCap lineCap = LineCap::Square );
};


struct SMeshRect
{
   typedef glm::vec3 V3;

   static void
   addLine2D( SMeshBuffer & o,
              int ax, int ay, int bx, int by, uint32_t colorA, uint32_t colorB );

   static void
   add( SMeshBuffer & o,
        V3 const & a, V3 const & b, V3 const & c, V3 const & d, uint32_t color = 0xFFFFFFFF,
        float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f );

   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   static void
   add( SMeshBuffer & o, Recti const & pos, uint32_t colorA, uint32_t colorB,
        uint32_t colorC, uint32_t colorD, TexRef const & ref );

   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   //
   static void
   addOutline( SMeshBuffer & o, Recti const & pos, uint32_t color,
               TexRef const & ref, int borderWidth );

   static void
   addOutline( SMeshBuffer & o, Recti const & pos, uint32_t colorA, uint32_t colorB,
               uint32_t colorC, uint32_t colorD, TexRef const & ref, int borderWidth );

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
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;
   // void addVertex( float x, float y, uint32_t color )
   typedef std::function<void(float,float, uint32_t)> ADD_VERTEX;
   // void addVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t color, float u, float v )
   //typedef std::function<void(float,float,float, float,float,float, uint32_t, float,float )> ADD_VERTEX;
   typedef std::function<void(uint32_t)> ADD_INDEX;

   static void
   addTriangles(
      float w, float h, float rx, float ry,
      uint32_t tess, uint32_t innerColor, uint32_t outerColor,
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
         float w, float h, float rx, float ry,
         int32_t tess = 13,
         uint32_t innerColor = 0xFFFFFFFF,
         uint32_t outerColor = 0xFFFFFFFF );

   static void
   addXZ( SMeshBuffer & o,
         glm::vec3 const & pos,
         float w, float h, float rx, float ry,
         int32_t tess = 13,
         uint32_t innerColor = 0xFFFFFFFF,
         uint32_t outerColor = 0xFFFFFFFF );
};

void
addRoundRect( SMeshBuffer & o,
      Recti const & pos,
      float zIndex = 0.0f,
      glm::ivec2 const & radius = glm::ivec2(8,8),
      uint32_t outerColor = 0xffffffff,
      uint32_t innerColor = 0xffffffff,
      TexRef const & ref = TexRef(), int tess = 13 );

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
      Recti const & pos, float zIndex = 0.0f,
      glm::ivec2 const & radius = glm::ivec2(8,8),
      uint32_t color = 0xffffffff,
      TexRef const & ref = TexRef(),
      int borderWidth = 1,
      uint32_t tess = 13 );

// =========================================================================
// Circle
// =========================================================================

void
addCircle( SMeshBuffer & o, Recti const & pos, uint32_t outerColor,
           uint32_t innerColor, TexRef const & ref, uint32_t tess = 12 );

void
addCircleOutline( SMeshBuffer & o, Recti const & pos, uint32_t color,
                  TexRef const & ref, int borderWidth, uint32_t tess = 12 );

// =========================================================================
struct SMeshCircle
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o, glm::vec2 const & siz,
          uint32_t color, uint32_t tessRadial = 33 );

   static void
   addXY( SMeshBuffer & o, glm::vec2 const & siz,
          uint32_t innerColor, uint32_t outerColor, uint32_t tessRadial );

   static void
   addXZ( SMeshBuffer & o, glm::vec2 const & siz,
          uint32_t color, uint32_t tessRadial = 33 );

   static void
   addXZ( SMeshBuffer & o, glm::vec2 const & siz,
          uint32_t innerColor, uint32_t outerColor, uint32_t tessRadial );
};

// =========================================================================
struct Generic2DTriangleRing
// =========================================================================
{
   // uint32_t getVertexCount();
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;
   // void addVertex( float x, float y, uint32_t color );
   typedef std::function<void(float, float, uint32_t)> ADD_VERTEX;
   // void addIndex( uint32_t index );
   typedef std::function<void(uint32_t)> ADD_INDEX;

   static void
   addIndexedTriangles(
      float outer_x, float outer_y,
      float inner_x, float inner_y,
      int32_t tess, uint32_t innerColor, uint32_t outerColor,
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
         float ow,
         float oh,
         float iw,
         float ih,
         uint32_t innerColor = 0xFFFFFFFF,
         uint32_t outerColor = 0xFFFFFFFF,
         int32_t tess = 13,
         glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addXZ(
      SMeshBuffer & o,
      float ow,
      float oh,
      float iw,
      float ih,
      uint32_t innerColor = 0xFFFFFFFF,
      uint32_t outerColor = 0xFFFFFFFF,
      int32_t tess = 13,
      glm::vec3 const & pos = glm::vec3(0,0,0) );
};


// =========================================================================
/// @brief PolyHull
// =========================================================================
struct SMeshPolyHull
{
   static void
   addLeftStripXY( SMeshBuffer & o, std::vector< glm::vec3 > const & points, float height,
                   uint32_t color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addLeftStripXZ( SMeshBuffer & o, std::vector< glm::vec3 > const & points, float height,
                   uint32_t color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );

   static void
   addRightStrip( SMeshBuffer & o, std::vector< glm::vec3 > const & points, float height,
                  uint32_t color = 0xFFFFFFFF, glm::vec3 const & pos = glm::vec3(0,0,0) );
};

// =========================================================================
struct SMeshCylinderHull
// =========================================================================
{
   typedef glm::vec3 V3;

   static void
   addXY( SMeshBuffer & o,
        float dx, float dy, float h,
        uint32_t color = 0xFFFFFFFF, int tessCirc = 33,
        V3 const & pos = V3(0,0,0) );

   static void
   addXZ( SMeshBuffer & o,
        float dx, float h, float dz,
        uint32_t color = 0xFFFFFFFF, int tessCirc = 33,
        V3 const & pos = V3(0,0,0) );
};

// =========================================================================
struct SMeshCylinder
// =========================================================================
{
   static void
   addXY( SMeshBuffer & o, float dx, float dy, float h,
          uint32_t color = 0xFFFFFFFF, uint32_t tessCirc = 33 );

   static void
   addXZ( SMeshBuffer & o, float dx, float h, float dz,
          uint32_t color = 0xFFFFFFFF, uint32_t tessCirc = 33 );

   static void
   addXZ( SMeshBuffer & o, float bottom_dx, float bottom_dz,
          float top_dx, float top_dz, float cylinder_h,
          uint32_t color = 0xFFFFFFFF, uint32_t tessCirc = 33, uint32_t tessHull = 11,
          glm::vec3 const & offset = glm::vec3() );

};

// =========================================================================
struct SMeshTube
// =========================================================================
{
   static void
   add( SMeshBuffer & o,
        // Describes outer cylinder
        float dx, // (outer) cylinder width
        float dz, // (outer) cylinder depth
        float hy, // (outer) cylinder height
        // Describes inner cylinder, rx,ry define the extrusion relative inside to dx,dy.
        // They define the inside border width and are not absolute values like dx,dy.
        float rx, // inside going border-width ( inside cylinder width cx = dx - rx )
        float rz, // inside going border-depth ( inside cylinder depth cz = dz - rz )
        uint32_t color = 0xFFFFFFFF, // one color to rule them all (vertices).
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
   computeSphereNormal( float lon, float lat ); // in degrees

   static void
   add( SMeshBuffer & o,
         glm::vec3 const & siz = glm::vec3(100,100,100),
         uint32_t color = 0xFFFFFFFF,
         uint32_t tessLon = 3,
         uint32_t tessLat = 3,
         glm::vec3 const & offset = glm::vec3() );
};


void
addSphere( SMeshBuffer & o,
      glm::vec3 const & siz = glm::vec3(100,100,100),
      glm::vec3 const & offset = glm::vec3(),
      uint32_t tessLon = 3, uint32_t tessLat = 3, uint32_t color = 0xFFFFFFFF );

// =========================================================================
struct SMeshUpperHalfSphere
// =========================================================================
{
   typedef glm::vec3 V3;
   typedef uint32_t u32;

   // [Longitude] Rotation um die y-Achse in degrees, equatorial xz plane is ground plane +Y is north pole
   // [Latitude] Rotation um die x/z-Achse in degrees: meridian plane (yx or yz)
   static V3
   computeSphereNormal( float lon, float lat ); // in degrees

   static void
   add( SMeshBuffer & o, V3 const & siz = V3(100,100,100), uint32_t color = 0xFFFFFFFF,
         uint32_t tessLon = 3, uint32_t tessLat = 3, V3 const & offset = V3() );
};






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

struct MeshHexagon
{
   typedef glm::vec2 V2;
   typedef glm::vec3 V3;
   typedef glm::dvec3 V3d;
   typedef glm::dvec4 V4d;

   static void
   add(  int i_min, int i_max,
         int j_min, int j_max,
         SMeshBuffer & mb,
         float w = 100.0f, float h = 100.0f,
         bool forceIndices = false,
         glm::vec3 const & pos = glm::vec3() )
   {
      for ( int j = j_min; j < j_max; ++j )
      {
         for ( int i = i_min; i < i_max; ++i )
         {
            //uint32_t color = randomColor();
            uint32_t color = randomColorInRange( 200, 255, 255 );
            add( i,j, mb, w, h, color, forceIndices, pos );
         }
      }
   }

   static void
   add(  int i,
         int j,
         SMeshBuffer & mb,
         float w = 100.0f, float h = 100.0f,
         uint32_t color = 0xFFFFFFFF,
         bool forceIndices = false,
         glm::vec3 const & pos = glm::vec3() )
   {
      float x = computeBoardPosX(w,h,i,j);
      float z = computeBoardPosY(w,h,i,j);
      add( mb, w, h, color, forceIndices, pos + V3(x,0,z) );
   }

   static void
   addM4( SMeshBuffer & mb,
         float w = 100.0f, float h = 100.0f,
         uint32_t color = 0xFFFFFFFF,
         bool forceIndices = false,
         glm::dmat4 const & trs = glm::dmat4() )
   {
      bool useIndices = forceIndices || mb.getIndexCount() > 0;
      V3 const n(0,1,0);
      V3 vA = V3d(trs*V4d(V3d(     0,0,-.5f*h ), 1 ));
      V3 vB = V3d(trs*V4d(V3d(-.5f*w,0,-.25f*h), 1));
      V3 vC = V3d(trs*V4d(V3d(-.5f*w,0, .25f*h), 1));
      V3 vD = V3d(trs*V4d(V3d(     0,0, .5f*h ), 1));
      V3 vE = V3d(trs*V4d(V3d(0.5f*w,0, .25f*h), 1));
      V3 vF = V3d(trs*V4d(V3d(0.5f*w,0,-.25f*h), 1));
      S3DVertex const A( vA, n, color, V2(.5f, 1 ) );
      S3DVertex const B( vB, n, color, V2(0, .75f ));
      S3DVertex const C( vC, n, color, V2(0, .25f ));
      S3DVertex const D( vD, n, color, V2(.5f, 0 ) );
      S3DVertex const E( vE, n, color, V2(1, .25f ));
      S3DVertex const F( vF, n, color, V2(1, .75f ));
      if ( useIndices )
      {
         size_t v = mb.getVertexCount();
         mb.addVertex( A ); mb.addVertex( B ); mb.addVertex( C );
         mb.addVertex( D ); mb.addVertex( E ); mb.addVertex( F );
         //mb.addIndexedTriangle( B, A, F );
         mb.addIndexedTriangle( v+1, v+5, v+0 );
         //mb.addIndexedTriangle( B, F, E );
         //mb.addIndex( v+1 ); mb.addIndex( v+5 ); mb.addIndex( v+4 );
         mb.addIndexedTriangle( v+1, v+4, v+5 );
         //mb.addIndexedTriangle( B, E, C );
         //mb.addIndex( v+1 ); mb.addIndex( v+4 ); mb.addIndex( v+2 );
         mb.addIndexedTriangle( v+1, v+2, v+4 );
         //mb.addIndexedTriangle( C, E, D );
         //mb.addIndex( v+2 ); mb.addIndex( v+4 ); mb.addIndex( v+3 );
         mb.addIndexedTriangle( v+2, v+3, v+4 );

      }
      else
      {
         //mb.addVertexTriangle( B, A, F );
         mb.addVertex( B ); mb.addVertex( F ); mb.addVertex( A );
         // mb.addVertexTriangle( B, F, E );
         mb.addVertex( B ); mb.addVertex( E ); mb.addVertex( F );
         // mb.addVertexTriangle( B, E, C );
         mb.addVertex( B ); mb.addVertex( C ); mb.addVertex( E );
         // mb.addVertexTriangle( C, E, D );
         mb.addVertex( C ); mb.addVertex( D ); mb.addVertex( E );
      }

     //mb.recalculateBoundingBox();
   }

   static void
   add( SMeshBuffer & mb,
         float w, float h,
         uint32_t color,
         bool forceIndices = false,
         glm::vec3 const & pos = glm::vec3()
         // glm::dmat4 const* mTransform = nullptr
          )
   {
      bool useIndices = forceIndices || mb.getIndexCount() > 0;
      V3 const n(0,1,0);
      S3DVertex const A( pos+V3(     0,0,-.5f*h ), n, color, V2(.5f, 1 ) );
      S3DVertex const B( pos+V3(-.5f*w,0,-.25f*h), n, color, V2(0, .75f ));
      S3DVertex const C( pos+V3(-.5f*w,0, .25f*h), n, color, V2(0, .25f ));
      S3DVertex const D( pos+V3(     0,0, .5f*h ), n, color, V2(.5f, 0 ) );
      S3DVertex const E( pos+V3(0.5f*w,0, .25f*h), n, color, V2(1, .25f ));
      S3DVertex const F( pos+V3(0.5f*w,0,-.25f*h), n, color, V2(1, .75f ));
      if ( useIndices )
      {
         size_t v = mb.getVertexCount();
         mb.addVertex( A ); mb.addVertex( B ); mb.addVertex( C );
         mb.addVertex( D ); mb.addVertex( E ); mb.addVertex( F );
         //mb.addIndexedTriangle( B, A, F );
         mb.addIndexedTriangle( v+1, v+5, v+0 );
         //mb.addIndexedTriangle( B, F, E );
         //mb.addIndex( v+1 ); mb.addIndex( v+5 ); mb.addIndex( v+4 );
         mb.addIndexedTriangle( v+1, v+4, v+5 );
         //mb.addIndexedTriangle( B, E, C );
         //mb.addIndex( v+1 ); mb.addIndex( v+4 ); mb.addIndex( v+2 );
         mb.addIndexedTriangle( v+1, v+2, v+4 );
         //mb.addIndexedTriangle( C, E, D );
         //mb.addIndex( v+2 ); mb.addIndex( v+4 ); mb.addIndex( v+3 );
         mb.addIndexedTriangle( v+2, v+3, v+4 );

      }
      else
      {
         //mb.addVertexTriangle( B, A, F );
         mb.addVertex( B ); mb.addVertex( F ); mb.addVertex( A );
         // mb.addVertexTriangle( B, F, E );
         mb.addVertex( B ); mb.addVertex( E ); mb.addVertex( F );
         // mb.addVertexTriangle( B, E, C );
         mb.addVertex( B ); mb.addVertex( C ); mb.addVertex( E );
         // mb.addVertexTriangle( C, E, D );
         mb.addVertex( C ); mb.addVertex( D ); mb.addVertex( E );
      }

     //mb.recalculateBoundingBox();
   }

   /*
   SMeshBuffer*
   MeshHexagon::create( float w, float h, uint32_t color )
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

   static add( SMeshBuffer & out, int i, int j, float w, float h, uint32_t color )
   {
      SMeshBuffer o( PrimitiveType::Triangles );

      auto addTriangle = [&] ( glm::vec3 p, glm::vec3 q )
      {
         glm::vec3 a = glm::vec3( q.x, q.y - 0.5f * height, q.z );
         glm::vec3 b = glm::vec3( q.x, q.y + 0.5f * height, q.z );
         glm::vec3 c = glm::vec3( p.x, p.y + 0.5f * height, p.z );
         glm::vec3 d = glm::vec3( p.x, p.y - 0.5f * height, p.z );
         glm::vec3 n = Math::getNormal3D( a,b,c );
         S3DVertex A( a.x, a.y, a.z, n.x,n.y,n.z, color, 0,1 );
         S3DVertex B( b.x, b.y, b.z, n.x,n.y,n.z, color, 0,0 );
         S3DVertex C( c.x, c.y, c.z, n.x,n.y,n.z, color, 1,0 );
         S3DVertex D( d.x, d.y, d.z, n.x,n.y,n.z, color, 1,1 );
         o.addQuad( A,B,C,D );
      };

         glm::vec3 a = points[ 0 ];
         for ( size_t i = 1; i < points.size(); ++i )
         {
            glm::vec3 b = points[ i ];
            addHullSegment( a, b );
            a = b;
         }

         addHullSegment( points[ points.size() - 1 ], points[ 0 ] );
      };

      auto corners = MeshHexagon::getCorners(w,h);
      std::vector< glm::vec3 > points;
      points.reserve( corners.size() );
      for ( auto const & p : corners )
      {
         points.emplace_back( p.x, 0.0f, p.y );
      }

      addLeftStripXZ( points, height, color );
      o.Material.setCulling( false );
      o.Material.setBlend( Blend::alphaBlend() );
      return o;
   }
*/
   static glm::ivec2
   getNeighborTileIndex( int corner, int i, int j )
   {
      if ( safeMod(j,2) == 1 )
      {
         switch ( corner )
         {
            case 0: return {   i, j-1 };
            case 1: return { i-1, j   };
            case 2: return {   i, j+1 };
            case 3: return { i+1, j+1 };
            case 4: return { i+1, j   };
            case 5: return { i+1, j-1 };
            default: throw std::runtime_error("Invalid getNeighborIndex()");
         }
      }
      else
      {
         switch ( corner )
         {
            case 0: return { i-1, j-1 };
            case 1: return { i-1, j   };
            case 2: return { i-1, j+1 };
            case 3: return {   i, j+1 };
            case 4: return { i+1, j   };
            case 5: return { i+1, j-1 };
            default: throw std::runtime_error("Invalid getNeighborIndex()");
         }
      }
   }

   static glm::vec2
   getNeighborTilePos( int corner, float w, float h, int i, int j )
   {
      glm::ivec2 n = getNeighborTileIndex( corner, i, j );
      return glm::vec2( computeBoardPosX( w,h,n.x,n.y),
                        computeBoardPosY( w,h,n.x,n.y) );
   }

   static int32_t
   safeMod( int32_t a, int32_t b )
   {
      if ( a == 0 || b == 0 ) { return 0; }
      return std::abs( a ) % std::abs( b );
   }

   static float
   computeBoardPosX( float w, float h, int i, int j )
   {
      //float x = 0.5f * float( safeMod( j, 2 ) == 1 ) + float( i );
      //return x * w;
      //return (0.5f * j + float( i )) * w;
      float x = 0.5f * float( safeMod( j, 2 ) == 1 ) + float( i );
      return x * w;
   }

   static float
   computeBoardPosY( float w, float h, int i, int j )
   {
      //float y = 3.0f/4.0f * float( j );
      //return y * h;
      return h * 3.0f/4.0f * float( j );
   }

   static glm::ivec2
   computeTileIndex( float x, float y, float w, float h )
   {
      int j = int( y / h );
      int i = int( x/w - (0.5f * j) );
      return glm::ivec2(i,j);
   }

   static glm::vec2 lineCenter( glm::vec2 const & a, glm::vec2 const & b ) { return a + ( b - a ) * 0.5f; }

   static glm::vec2 cornerA( float w, float h ) { return glm::vec2( 0.0f,    -.5f*h ); }
   static glm::vec2 cornerB( float w, float h ) { return glm::vec2( -0.5f*w, -.25f*h ); }
   static glm::vec2 cornerC( float w, float h ) { return glm::vec2( -0.5f*w, .25f*h ); }
   static glm::vec2 cornerD( float w, float h ) { return glm::vec2( 0.0f,     .5f*h ); }
   static glm::vec2 cornerE( float w, float h ) { return glm::vec2( 0.5f*w,  .25f*h ); }
   static glm::vec2 cornerF( float w, float h ) { return glm::vec2( 0.5f*w, -.25f*h ); }

   static glm::vec2 edgeAB( float w, float h ) { return lineCenter( cornerA(w,h), cornerB(w,h)); }
   static glm::vec2 edgeBC( float w, float h ) { return lineCenter( cornerB(w,h), cornerC(w,h)); }
   static glm::vec2 edgeCD( float w, float h ) { return lineCenter( cornerC(w,h), cornerD(w,h)); }
   static glm::vec2 edgeDE( float w, float h ) { return lineCenter( cornerD(w,h), cornerE(w,h)); }
   static glm::vec2 edgeEF( float w, float h ) { return lineCenter( cornerE(w,h), cornerF(w,h)); }
   static glm::vec2 edgeFA( float w, float h ) { return lineCenter( cornerF(w,h), cornerA(w,h)); }

   static glm::vec2
   getCorner( int i, float w, float h )
   {
      switch ( i )
      {
         case 0: return cornerA( w,h );
         case 1: return cornerB( w,h );
         case 2: return cornerC( w,h );
         case 3: return cornerD( w,h );
         case 4: return cornerE( w,h );
         case 5: return cornerF( w,h );
         default: throw std::runtime_error("Invalid getCorner()");
      }
   }

   static std::vector< glm::vec2 >
   getCorners( float w, float h )
   {
      return std::vector< glm::vec2 > {
         cornerA( w,h ), cornerB( w,h ), cornerC( w,h ),
         cornerD( w,h ), cornerE( w,h ), cornerF( w,h ) };
   }

   static std::vector< glm::vec3 >
   getCornersXZ( float w, float h )
   {
      auto a = cornerA( w,h );
      auto b = cornerB( w,h );
      auto c = cornerC( w,h );
      auto d = cornerD( w,h );
      auto e = cornerE( w,h );
      auto f = cornerF( w,h );

      return std::vector< glm::vec3 > {
         glm::vec3( a.x, 0, a.y ),
         glm::vec3( b.x, 0, b.y ),
         glm::vec3( c.x, 0, c.y ),
         glm::vec3( d.x, 0, d.y ),
         glm::vec3( e.x, 0, e.y ),
         glm::vec3( f.x, 0, f.y ) };
   }

   static glm::vec2
   getEdge( int i, float w, float h )
   {
      switch ( i )
      {
         case 0: return edgeAB( w,h );
         case 1: return edgeBC( w,h );
         case 2: return edgeCD( w,h );
         case 3: return edgeDE( w,h );
         case 4: return edgeEF( w,h );
         case 5: return edgeFA( w,h );
         default: throw std::runtime_error("Invalid i in getEdge()");
      }
   }

   static float
   computeAngle( glm::vec2 const & v )
   {
      float phi = atan2( v.y, v.x ) * Math::RAD2DEG;
      //DE_DEBUG("atan2( ",v," ) = ", phi)
      if ( phi < 0.0f ) phi += 360.0f;
      return phi;
   }

   static float
   computeCornerAngle( int i, float w, float h )
   {
      switch ( i )
      {
         case 0: return 90.0f;
         case 1: return 10.0f;
         case 2: return 20.0f;
         case 3: return 30.0f;
         case 4: return 40.0f;
         case 5: return 90.0f;
         default: throw std::runtime_error("Invalid i in computeCornerAngle()");
      }
   }

   static float
   computeEdgeAngle( int i, float w, float h )
   {
      switch ( i )
      {
         case 0: return 90-153.435f;
         case 1: return 0.0f;
         case 2: return 90-26.5651f;
         case 3: return 90-153.435f;
         case 4: return 0.0f;
         case 5: return 90-26.565f;
         default: throw std::runtime_error("Invalid i in computeEdgeAngle()");
      }
   }

   // static void
   // test( float w, float h )
   // {
      // DE_DEBUG("MeshHexagon Angles for w(",w,"), h(",h,")")
      // for ( int i = 0; i < 6; ++i )
      // {
         // DE_DEBUG("MeshHexagon.CornerAngle[", i,"] = ", computeCornerAngle( i,w,h ) )
      // }
      // for ( int i = 0; i < 6; ++i )
      // {
         // DE_DEBUG("MeshHexagon.EdgeAngle[", i,"] = ", computeEdgeAngle( i,w,h ) )
      // }
   // }
};

// =========================================================================
struct MeshHexagonHull
// =========================================================================
{
   static SMeshBuffer
   create( float w, float h, float height, uint32_t color )
   {
      SMeshBuffer o( PrimitiveType::Triangles );

      auto addLeftStripXZ = [&] ( std::vector< glm::vec3 > const & points, float height, uint32_t color )
      {
         if ( points.size() < 2 )
         {
            return;
         }

         auto addHullSegment = [&] ( glm::vec3 p, glm::vec3 q )
         {
            glm::vec3 a = glm::vec3( q.x, q.y - 0.5f * height, q.z );
            glm::vec3 b = glm::vec3( q.x, q.y + 0.5f * height, q.z );
            glm::vec3 c = glm::vec3( p.x, p.y + 0.5f * height, p.z );
            glm::vec3 d = glm::vec3( p.x, p.y - 0.5f * height, p.z );
            glm::vec3 n = Math::getNormal3D( a,b,c );
            S3DVertex A( a.x, a.y, a.z, n.x,n.y,n.z, color, 0,1 );
            S3DVertex B( b.x, b.y, b.z, n.x,n.y,n.z, color, 0,0 );
            S3DVertex C( c.x, c.y, c.z, n.x,n.y,n.z, color, 1,0 );
            S3DVertex D( d.x, d.y, d.z, n.x,n.y,n.z, color, 1,1 );
            o.addQuad( A,B,C,D );
         };

         glm::vec3 a = points[ 0 ];
         for ( size_t i = 1; i < points.size(); ++i )
         {
            glm::vec3 b = points[ i ];
            addHullSegment( a, b );
            a = b;
         }

         addHullSegment( points[ points.size() - 1 ], points[ 0 ] );
      };

      auto corners = MeshHexagon::getCorners(w,h);
      std::vector< glm::vec3 > points;
      points.reserve( corners.size() );
      for ( auto const & p : corners )
      {
         points.emplace_back( p.x, 0.0f, p.y );
      }

      addLeftStripXZ( points, height, color );
      o.getMaterial().setCulling( false );
      o.getMaterial().setBlend( Blend::alphaBlend() );
      return o;
   }
};

/*
// =========================================================================
//struct MeshHexagonOutline
// =========================================================================
//{
//   typedef std::vector< glm::vec3 > PointList;
//};

static SMeshBuffer
create( float w, float h, float lineWidth, uint32_t color )
{
   auto addPolyLine = [&] ( SMeshBuffer & o, PointList const & points, float lineWidth, uint32_t color )
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
addPioneerCity( SMesh & out, float dx, float dy, float dz, uint32_t color )
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

   static float
   computeAngle( glm::vec2 const & v )
   {
      glm::vec2 const y(0,1);
      return acos( glm::dot(v,y) / glm::length(v) ) * Math::RAD2DEG;
   }

   static std::vector< Triangle3f >
   getTriangles( float w, float h, glm::vec3 const & offset = glm::vec3() )
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
   create( float w, float h, uint32_t color = 0xFFFFFFFF );

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
   MeshHexagon::create( float w, float h, uint32_t color )
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

   static float
   computeEdgeAngle( int i, float w, float h )
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

      float phi = computeAngle( q - p );
      DE_DEBUG("i=",i,", w=",w,", h=",h,", p=",p,", q=",q, ", phi=",phi)
   }

   static float
   getEdgeAngle( int i, float w, float h )
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


