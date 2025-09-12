#pragma once
#include <cstdint>
#include <sstream>
#include <functional>

namespace de {


// =========================================================================
struct SolidCircle
// =========================================================================
{
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;    // uint32_t getVertexCount() const;
   typedef std::function<void(float,float, uint32_t, float,float)> ADD_VERTEX;    // void addVertex( x, y, colorRGBA );
   typedef std::function<void(uint32_t)> ADD_INDEX;    // void addIndex( index );

   static void
   addIndexedTriangles(
      float w, float h,
      uint32_t tess, uint32_t innerColor, uint32_t outerColor,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex )
   {
      float const rx = 0.5f * w;
      float const ry = 0.5f * h;
      float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
      uint32_t const v = getVertexCount();

      addVertex( 0,0, innerColor, 0.5f, 0.5f );
      for ( uint32_t i = 0; i <= tess; ++i )
      {
         float const phi = angleStep * float( i );
         float const s = ::sinf( phi );
         float const c = ::cosf( phi );
         float x = rx * c; // * float( j ) * outwardX);
         float y = ry * s; // * float( j ) * outwardY);
         float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
         float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
         addVertex( x,y, outerColor, u, v );
      }

      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t M = v;
         uint32_t A = v + 1 + i;
         uint32_t B = v + 1 + i + 1;
         addIndex( M );
         addIndex( A );
         addIndex( B );
      }
   }
};

// =========================================================================
struct LineCircle
// =========================================================================
{
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;    // uint32_t getVertexCount() const;
   typedef std::function<void(float,float, uint32_t, float,float)> ADD_VERTEX;    // void addVertex( x, y, colorRGBA );
   typedef std::function<void(uint32_t)> ADD_INDEX;    // void addIndex( index );

   static void
   addLines(
      float w, float h,
      uint32_t tess, uint32_t innerColor, uint32_t outerColor,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex )
	{
      float const rx = 0.5f * w;
      float const ry = 0.5f * h;
      float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
      uint32_t const v = getVertexCount();

      addVertex( 0,0, innerColor, 0.5f, 0.5f );
      for ( uint32_t i = 0; i <= tess; ++i )
      {
         float const phi = angleStep * float( i );
         float const s = ::sinf( phi );
         float const c = ::cosf( phi );
         float x = rx * c; // * float( j ) * outwardX);
         float y = ry * s; // * float( j ) * outwardY);
         float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
         float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
         addVertex( x,y, outerColor, u, v );
      }

      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t A = v + i;
         uint32_t B = v + i + 1;
         addIndex( A );
         addIndex( B );
      }
	}
};



//==========================================================
/// @brief RoundRect
//==========================================================
/**   
   y0--  /G---H\
        / | / | \
   y1- F--B---C--I
       | /|  /| /|
       |/ | / |/ |    ABC -> BCA ( B in center )
       |  |/  |  |    ACD -> DAC ( D in center )
   y2- E--A---D--J
        \ | / | /
   y3--  \L---K/

       |  |   |  | 
       x0 x1  x2 x3
*/   
// =========================================================================
struct RoundRect
// =========================================================================
{
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;    // uint32_t getVertexCount() const;
   typedef std::function<void(float,float, uint32_t)> ADD_VERTEX;    // void addVertex( x, y, colorRGBA );
   typedef std::function<void(uint32_t)> ADD_INDEX;    // void addIndex( index );

   static void
   addTriangles(
      float w, float h, float rx, float ry,
      uint32_t tess, uint32_t innerColor, uint32_t outerColor,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex )
   {
      uint32_t const vStart = getVertexCount();

      auto addIndexedTriangle = [&] ( uint32_t iA, uint32_t iB, uint32_t iC )
      {
         addIndex( vStart + iA );   // input ABC (cw), output ACB (ccw)
         addIndex( vStart + iC );
         addIndex( vStart + iB );
      };

      auto addIndexedQuad = [&] ( uint32_t iA, uint32_t iB, uint32_t iC, uint32_t iD )
      {
         addIndexedTriangle( iA,iB,iC );   // input ABC (cw)
         addIndexedTriangle( iA,iC,iD );   // input ACD (cw)
      };

      float const x0 = -0.5f * w;
      float const x1 = -0.5f * w + rx;
      float const x2 = 0.5f * w - rx;
      float const x3 = 0.5f * w;
      float const y0 = -0.5f * h;
      float const y1 = -0.5f * h + ry;
      float const y2 = 0.5f * h - ry;
      float const y3 = 0.5f * h;

      // Add 12 vertices for 5 main quads
      addVertex( x1, y2, innerColor ); // A=0
      addVertex( x1, y1, innerColor ); // B=1
      addVertex( x2, y1, innerColor ); // C=2
      addVertex( x2, y2, innerColor ); // D=3
      addVertex( x0, y2, outerColor ); // E=4
      addVertex( x0, y1, outerColor ); // F=5
      addVertex( x1, y0, outerColor ); // G=6
      addVertex( x2, y0, outerColor ); // H=7
      addVertex( x3, y1, outerColor ); // I=8
      addVertex( x3, y2, outerColor ); // J=9
      addVertex( x2, y3, outerColor ); // K=10
      addVertex( x1, y3, outerColor ); // L=11

      // Add 20 indices for 5 main quads
      uint32_t const INDEX_A = 0;
      uint32_t const INDEX_B = 1;
      uint32_t const INDEX_C = 2;
      uint32_t const INDEX_D = 3;
      uint32_t const INDEX_E = 4;
      uint32_t const INDEX_F = 5;
      uint32_t const INDEX_G = 6;
      uint32_t const INDEX_H = 7;
      uint32_t const INDEX_I = 8;
      uint32_t const INDEX_J = 9;
      uint32_t const INDEX_K = 10;
      uint32_t const INDEX_L = 11;
      uint32_t const INDEX_TOPLEFT = 12 + 0*tess;
      uint32_t const INDEX_TOPRIGHT = 12 + 1*tess;
      uint32_t const INDEX_BOTTOMRIGHT = 12 + 2*tess;
      uint32_t const INDEX_BOTTOMLEFT = 12 + 3*tess;
      //uint32_t const INDEX_END = 12 + 4*tess;
      addIndexedQuad( INDEX_B, INDEX_G, INDEX_H, INDEX_C ); // Top BGHC
      addIndexedQuad( INDEX_E, INDEX_F, INDEX_B, INDEX_A ); // Center EFBA
      addIndexedQuad( INDEX_A, INDEX_B, INDEX_C, INDEX_D ); // Center ABCD
      addIndexedQuad( INDEX_D, INDEX_C, INDEX_I, INDEX_J ); // Center DCIJ
      addIndexedQuad( INDEX_L, INDEX_A, INDEX_D, INDEX_K ); // Bottom LADK

      //
      // Add 4*tess corner vertices. They are between the main vertices.
      //
      struct SinCos
      {
         float s,c;
         SinCos() : s(0.f), c(0.f) {}
         SinCos(float _s,float _c) : s(_s), c(_c) {}
      };
      std::vector< SinCos > lookUpTable;
      lookUpTable.reserve( tess );

      float phi_step = float( 0.5 * M_PI ) / float( tess+1 );
      float phi = phi_step; // phi_start > 0° and phi_end < 90°
      for ( uint32_t i = 0; i < tess; ++i )
      {
         lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
         phi += phi_step;
      }

      // 1. topLeft FGB (cw) quarter circle
      for ( uint32_t i = 0; i < tess; ++i )
      {
         float x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
         float y = y1 - ry * lookUpTable[ i ].s;
         addVertex( x, y, outerColor );
      }
      // 2. topRight HIC (cw) quarter circle
      for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
      {
         float x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
         float y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
         addVertex( x, y, outerColor );
      }
      // 3. bottomRight JKD (cw) quarter circle
      for ( uint32_t i = 0; i < tess; ++i )
      {
         float x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
         float y = y2 + ry * lookUpTable[ i ].s;
         addVertex( x, y, outerColor );
      }
      // 4. bottomLeft LEA (cw) quarter circle
      for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
      {
         float x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
         float y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
         addVertex( x, y, outerColor );
      }

      // add 4*tess CornerIndices:

      // 1. topLeft FGB (cw) quarter circle, glm::vec3 B( x1, 0.f, y2 );
      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t F = INDEX_TOPLEFT + i;
         uint32_t G = INDEX_TOPLEFT + i + 1;
         uint32_t B = INDEX_B;   // const (center)
         if ( i == 0 )             { F = INDEX_F; } // StartTriangle
         else if ( i == tess - 1 ) { G = INDEX_G; } // EndTriangle
         else                      {}               // MiddleTriangle
         addIndexedTriangle( F, G, B ); // all given in (cw)
      }

      // 2. topRight HIC (cw) quarter circle, glm::vec3 C( x2, 0.f, y2 );
      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t H = INDEX_TOPRIGHT + i;
         uint32_t I = INDEX_TOPRIGHT + i + 1;
         uint32_t C = INDEX_C;   // const (center)
         if ( i == 0 )             { H = INDEX_H; } // StartTriangle
         else if ( i == tess - 1 ) { I = INDEX_I; } // EndTriangle
         else                      {}               // MiddleTriangle
         addIndexedTriangle( H,I,C ); // all given in (cw)
      }

      // 3. bottomRight JKD (cw) quarter circle, glm::vec3 D( x2, 0.f, y1 );
      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t J = INDEX_BOTTOMRIGHT + i;
         uint32_t K = INDEX_BOTTOMRIGHT + i + 1;
         uint32_t D = INDEX_D;   // const (center)
         if ( i == 0 )             { J = INDEX_J;  } // StartTriangle
         else if ( i == tess - 1 ) { K = INDEX_K;  } // EndTriangle
         else                      {}                // MiddleTriangle
         addIndexedTriangle( J, K, D ); // all given in (cw)
      }

      // 4. bottomLeft LEA (cw) quarter circle, glm::vec3 A( x1, 0.f, y1 );
      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t L = INDEX_BOTTOMLEFT + i;
         uint32_t E = INDEX_BOTTOMLEFT + i + 1;
         uint32_t A = INDEX_A;   // const (center)
         if ( i == 0 )             { L = INDEX_L; } // StartTriangle
         else if ( i == tess - 1 ) { E = INDEX_E; } // EndTriangle
         else                      {}               // MiddleTriangle
         addIndexedTriangle( L, E, A ); // all given in (cw)
      }
   }


};


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
struct LineRoundRect
{
   typedef std::function<uint32_t()> GET_VERTEXCOUNT;    // uint32_t getVertexCount() const;
   typedef std::function<void(float,float, uint32_t)> ADD_VERTEX;    // void addVertex( x, y, colorRGBA );
   typedef std::function<void(uint32_t)> ADD_INDEX;    // void addIndex( index );

   static void
   addLines(
      float w, float h, float rx, float ry, uint32_t tess,
      uint32_t color,
      GET_VERTEXCOUNT const & getVertexCount,
      ADD_VERTEX const & addVertex,
      ADD_INDEX const & addIndex )
   {
      uint32_t const v = getVertexCount();

      //glm::vec2 off( w/2 + 1 + pos.x(), h/2 + pos.y() );
      float const x0 = -0.5f * w;
      float const x1 = -0.5f * w + rx;
      float const x2 = 0.5f * w - rx;
      float const x3 = 0.5f * w;
      float const y0 = -0.5f * h;
      float const y1 = -0.5f * h + ry;
      float const y2 = 0.5f * h - ry;
      float const y3 = 0.5f * h;

   // [SinCosLookUpTable] for a quarter circle
      struct SinCos
      {
         float s,c;
         SinCos() : s(0.f), c(0.f) {}
         SinCos(float _s,float _c) : s(_s), c(_c) {}
      };
      std::vector< SinCos > lookUpTable;
      lookUpTable.reserve( tess );

      float phi_step = float( 0.5 * M_PI ) / float( tess+1 );
      float phi = phi_step; // phi_start > 0° and phi_end < 90°
      for ( uint32_t i = 0; i < tess; ++i )
      {
         lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
         phi += phi_step;
      }
   // [SinCosLookUpTable] End.

      std::vector< glm::vec2 > points;

      auto addPoint = [&] ( float x, float y )
      {
         points.emplace_back( x, y );
      };

      if ( h > 2*ry )
      {
         addPoint( x0, y2 ); // E=0
         addPoint( x0, y1 ); // F=1
      }

      // 1. topLeft FGB (cw) quarter circle
      for ( uint32_t i = 0; i < tess; ++i )
      {
         float x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
         float y = y1 - ry * lookUpTable[ i ].s;
         addPoint( x, y );
      }

      if ( w > 2*rx )
      {
         addPoint( x1, y0 ); // G=2
         addPoint( x2, y0 ); // H=3
      }

      // 2. topRight HIC (cw) quarter circle
      for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
      {
         float x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
         float y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
         addPoint( x, y );
      }

      if ( h > 2*ry )
      {
         addPoint( x3, y1 ); // I=4
         addPoint( x3, y2 ); // J=5
      }


      // 3. bottomRight JKD (cw) quarter circle
      for ( uint32_t i = 0; i < tess; ++i )
      {
         float x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
         float y = y2 + ry * lookUpTable[ i ].s;
         addPoint( x, y );
      }

      if ( w > 2*rx )
      {
         addPoint( x2, y3 ); // K=6
         addPoint( x1, y3 ); // L=7
      }

      // 4. bottomLeft LEA (cw) quarter circle
      for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
      {
         float x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
         float y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
         addPoint( x, y );
      }

      addPoint( x0, y2 ); // E=0

      uint32_t k = 0;
      for ( size_t i = 0; i < points.size() - 1; ++i )
      {
         auto A = points[i];
         auto B = points[i+1];
         addVertex( A.x, A.y, color );
         addVertex( B.x, B.y, color );
         addIndex( v + k );
         addIndex( v + k + 1 );
         k += 2;
      }
      auto A = points[points.size()-1];
      auto B = points[0];
      addVertex( A.x, A.y, color );
      addVertex( B.x, B.y, color );
      addIndex( v + k + 1 );
      addIndex( v );
   }
};



/*
// =========================================================================
glm::vec2
SMeshPolyLine::getOrtho2D( glm::vec2 const & m ) { return glm::vec2( m.y, -m.x ); }
glm::vec2
SMeshPolyLine::getNormal2D( glm::vec2 const & m ) { return glm::normalize( glm::vec2( m.y, -m.x ) ); }

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
bool
SMeshPolyLine::addTriangleStrip( SMeshBuffer & o, std::vector< glm::vec2 > const & points,
     uint32_t color, float lineWidth, LineCap lineCap, LineJoin lineJoin )
{
   auto n = points.size();
   if ( n < 2 ) { return false; } // Not a polyline.
   //DE_DEBUG("PolylineTriangleStrip: lineCap:", int(lineCap), ", lineJoin:", int(lineJoin), ", n:",n )

   o.setPrimitiveType( PrimitiveType::TriangleStrip );

   auto addVertex = [ & ] ( glm::vec2 const & pos, glm::vec2 const & nrm, uint32_t color, float u, float v )
   {
      float x = (nrm.x * 0.5f * lineWidth) + pos.x; // Extrude pos in direction of nrm
      float y = (nrm.y * 0.5f * lineWidth) + pos.y; // Extrude pos in direction of nrm
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, color, u,v ) ); // Write extruded pos and always normal -Z, since its 2D XY plane.
      o.addIndex();
   };

   //
   // PolyLine algo from Benni for Bevel,Miter and RoundJoin,
   // cant use TriangleStrip because of poss. RoundJoin? ( RoundJoin as TriangleStrip can be wasteful )
   //
   float dist = 0.0f;
   glm::vec2 A,B;
   A = points[ 0 ]; // Fetch A
   B = points[ 1 ]; // Fetch B
   glm::vec2 dAB = glm::normalize( B - A );     // 2D direction AB
   glm::vec2 nAB = glm::vec2( dAB.y, -dAB.x );  // 2D normale AB
   if ( lineCap != LineCap::None )
   {
      addVertex( A, -nAB - dAB, color, dist,0 ); // SL
      addVertex( A,  nAB - dAB, color, dist,1 ); // SR
      dist += 0.5f*lineWidth;
   }

   // SegmentAB begin:
   addVertex( A, -nAB, color, dist,0 ); // AL
   addVertex( A,  nAB, color, dist,1 ); // AR
   dist += glm::length( B - A );

   // <optional-loop> May never run ( for only 2 points )
   // The loop is designed as optional and may not break the algo without it ever running.
   // Read the algo twice by ignoring the loop.
   for ( size_t i = 2; i < n; ++i )
   {
      glm::vec2 C = points[ i ]; // Fetch C
      glm::vec2 dBC = glm::normalize( C - B );
      glm::vec2 nBC = glm::vec2( dBC.y, -dBC.x );
      glm::vec2 nABC = ( nAB + nBC )*0.5f;
      // Finalize segment AB and start segment BC using the computed normal nABC
      // This normal is never computed for only 2 points AB and never used elsewhere.
      addVertex( B, -nABC, color, dist,0 ); // LineJoinL
      addVertex( B,  nABC, color, dist,1 ); // LineJoinR
      dist += glm::length( C - B );
      // Prepare next iteration ( simulate behaviour as if loop never ran )
      A = B;      // Simulate behaviour without loop.
      B = C;      // Simulate behaviour without loop.
      dAB = dBC;  // Simulate behaviour without loop.
      nAB = nBC;  // Simulate behaviour without loop.
   }

   // Finalize Segment AB: ( its actually C and nBC if the loop ran atleast once )
   addVertex( B, -nAB, color, dist,0 ); // DL
   addVertex( B,  nAB, color, dist,1 ); // DR

   // End Line Cap:
   if ( lineCap != LineCap::None )
   {
      dist += 0.5f*lineWidth;
      addVertex( B, -nAB + dAB, color, dist,0 ); // EL
      addVertex( B,  nAB + dAB, color, dist,1 ); // ER
   }

   return true;
}

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
void
SMeshPolyLine::addExtrudedLine( SMeshBuffer & o, glm::vec2 const & a, glm::vec2 const & b,
      uint32_t colorA, uint32_t colorB, float lineWidth, LineCap lineCap )
{
   o.setPrimitiveType( PrimitiveType::Triangles );

   auto addQuad = [&] ( glm::vec2 const & a_pos, glm::vec2 const & a_nrm,
                        glm::vec2 const & b_pos, glm::vec2 const & b_nrm,
                        glm::vec2 const & c_pos, glm::vec2 const & c_nrm,
                        glm::vec2 const & d_pos, glm::vec2 const & d_nrm )
   {
      float ax = (a_nrm.x * 0.5f * lineWidth) + a_pos.x; // Extrude pos in direction of nrm
      float ay = (a_nrm.y * 0.5f * lineWidth) + a_pos.y; // Extrude pos in direction of nrm
      float bx = (b_nrm.x * 0.5f * lineWidth) + b_pos.x; // Extrude pos in direction of nrm
      float by = (b_nrm.y * 0.5f * lineWidth) + b_pos.y; // Extrude pos in direction of nrm
      float cx = (c_nrm.x * 0.5f * lineWidth) + c_pos.x; // Extrude pos in direction of nrm
      float cy = (c_nrm.y * 0.5f * lineWidth) + c_pos.y; // Extrude pos in direction of nrm
      float dx = (d_nrm.x * 0.5f * lineWidth) + d_pos.x; // Extrude pos in direction of nrm
      float dy = (d_nrm.y * 0.5f * lineWidth) + d_pos.y; // Extrude pos in direction of nrm
      o.addVertex( S3DVertex( ax,ay,0, 0,0,-1, colorA, 0,0 ) ); // A
      o.addVertex( S3DVertex( bx,by,0, 0,0,-1, colorB, 0,0 ) ); // B
      o.addVertex( S3DVertex( cx,cy,0, 0,0,-1, colorB, 0,0 ) ); // C
      o.addVertex( S3DVertex( dx,dy,0, 0,0,-1, colorA, 0,0 ) ); // D
      o.addIndexedQuad(); // Triangles, not TriangleStrip
   };

   glm::vec2 vAB = b - a;     // 2D direction AB
   //auto distAB = glm::length( vAB );  // 2D positive (abs) length in pixels.
   glm::vec2 dAB = glm::normalize( vAB );     // 2D direction AB
   glm::vec2 nAB = glm::vec2( dAB.y, -dAB.x );  // 2D normale AB

   if ( lineCap == LineCap::None )
   {
      addQuad(
         a, -nAB, // AL
         b, -nAB, // BL
         b,  nAB, // BR
         a,  nAB ); // AR
   }
   else if ( lineCap == LineCap::Square )
   {
      addQuad(
         a, -nAB - dAB, // SL
         b, -nAB + dAB, // EL
         b,  nAB + dAB, // ER
         a,  nAB - dAB ); // SR
   }
   else
   {
      throw std::runtime_error(
         "Invalid LineCap, not implemented in PolyLine::addLineSegment()");
   }
}

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
addRoundRectOutline( SMeshBuffer & o, Recti const & pos, float zIndex,
                     glm::ivec2 const & radius,
                     uint32_t color, TexRef const & ref, int borderWidth, uint32_t tess )
{
   int w = pos.w();
   int h = pos.h();
   if ( w < 4 || w > 1000000 ) { return; }
   if ( h < 4 || h > 1000000 ) { return; }
   o.setBlend( Blend::alphaBlend() );
   if ( ref.m_tex )
   {
      o.setTexture( 0, ref );
   }

   glm::vec2 off( w/2 + 1 + pos.x(), h/2 + pos.y() );
   float const rx = radius.x;
   float const ry = radius.y;
   float const x0 = -0.5f * w;
   float const x1 = -0.5f * w + rx;
   float const x2 = 0.5f * w - rx;
   float const x3 = 0.5f * w;
   float const y0 = -0.5f * h;
   float const y1 = -0.5f * h + ry;
   float const y2 = 0.5f * h - ry;
   float const y3 = 0.5f * h;

// [SinCosLookUpTable] for a quarter circle
   struct SinCos
   {
      float s,c;
      SinCos() : s(0.f), c(0.f) {}
      SinCos(float _s,float _c) : s(_s), c(_c) {}
   };
   std::vector< SinCos > lookUpTable;
   lookUpTable.reserve( tess );

   float phi_step = float( 0.5 * M_PI ) / float( tess+1 );
   float phi = phi_step; // phi_start > 0° and phi_end < 90°
   for ( uint32_t i = 0; i < tess; ++i )
   {
      lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
      phi += phi_step;
   }
// [SinCosLookUpTable] End.

   std::vector< glm::vec2 > points;

   auto addPoint = [&] ( float x, float y )
   {
      points.emplace_back( off.x + x, off.y + y );
   };

   if ( h > 2*radius.y )
   {
      addPoint( x0, y2 ); // E=0
      addPoint( x0, y1 ); // F=1
   }

   // 1. topLeft FGB (cw) quarter circle
   for ( uint32_t i = 0; i < tess; ++i )
   {
      float x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      float y = y1 - ry * lookUpTable[ i ].s;
      addPoint( x, y );
   }

   if ( w > 2*radius.x )
   {
      addPoint( x1, y0 ); // G=2
      addPoint( x2, y0 ); // H=3
   }

   // 2. topRight HIC (cw) quarter circle
   for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
   {
      float x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      float y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addPoint( x, y );
   }

   if ( h > 2*radius.y )
   {
      addPoint( x3, y1 ); // I=4
      addPoint( x3, y2 ); // J=5
   }


   // 3. bottomRight JKD (cw) quarter circle
   for ( uint32_t i = 0; i < tess; ++i )
   {
      float x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      float y = y2 + ry * lookUpTable[ i ].s;
      addPoint( x, y );
   }

   if ( w > 2*radius.x )
   {
      addPoint( x2, y3 ); // K=6
      addPoint( x1, y3 ); // L=7
   }

   // 4. bottomLeft LEA (cw) quarter circle
   for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
   {
      float x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      float y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addPoint( x, y );
   }

   addPoint( x0, y2 ); // E=0

   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::None, LineJoin::Bevel );

   // Set color between black and white to all vertices depending on their u value.
   float uMax = o.Vertices.back().tex.x;
   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      o.Vertices[ i ].pos.z = zIndex;
      float u = o.Vertices[ i ].tex.x;
      uint8_t r = clampByte( dbRound32( 255.0f * u / uMax ) );
      uint32_t color = RGBA( r,r,r );
      o.Vertices[ i ].color = color;
   }
}

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

} // end namespace de.

