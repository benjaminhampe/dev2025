#pragma once
#include <de/gpu/Mesh.hpp>

namespace de {
namespace gpu {

// =========================================================================
struct SMeshPolyLine
// =======================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshPolyLine")
   inline static glm::vec2 getOrtho2D( glm::vec2 const & m ) { return glm::vec2( m.y, -m.x ); }
   inline static glm::vec2 getNormal2D( glm::vec2 const & m ) { return glm::normalize( glm::vec2( m.y, -m.x ) ); }

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
        LineCap::ELineCap lineCap = LineCap::Square,
        LineJoin::ELineJoin lineJoin = LineJoin::Bevel )
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
   static void
   addExtrudedLine( SMeshBuffer & o, glm::vec2 const & a, glm::vec2 const & b,
         uint32_t colorA = 0xFFFFFFFF, uint32_t colorB = 0xFFFFFFFF, float lineWidth = 1.0f,
         LineCap::ELineCap lineCap = LineCap::Square )
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
};

} // end namespace gpu.
} // end namespace de.















/*




   inline static float32_t
   getAngleABC( glm::vec2 const & A, glm::vec2 const & B, glm::vec2 const & C )
   {
      auto const ABx = float64_t( B.x ) - float64_t( A.x );
      auto const ABy = float64_t( B.y ) - float64_t( A.y );
      auto const BCx = float64_t( C.x ) - float64_t( B.x );
      auto const BCy = float64_t( C.y ) - float64_t( B.y );
      auto const phiAB = ::atan2( ABy, ABx );
      auto const phiBC = ::atan2( BCy, BCx );
      auto const phiABC = static_cast< float32_t >( phiBC - phiAB );
   #if 0
      std::cout << __FUNCTION__ << " :: phi(" << phiABC << "), ";
      if ( phiABC > 0.0f )
      {
         std::cout << "Left=CCW";
      }
      else
      {
         std::cout << "Right=CW";
      }

      std::cout << ", A(" << A << "), B(" << B << "), C(" << C << "), "
         << "AB(" << B - A << "), phiAB(" << phiAB << ")"
         << "BC(" << C - B << "), phiBC(" << phiBC << ")"
         << "\n";
   #endif
      return phiABC;
   }

   static bool
   addTriangles( SMeshBuffer & o,
        std::vector< glm::vec2 > const & points,
        uint32_t color = 0xFFFFFFFF,
        float lineWidth = 1.0f,
        LineCap::ELineCap lineCap = LineCap::None,
        LineJoin::ELineJoin lineJoin = LineJoin::None )
   {
//      uint32_t ghost = 0x80FFFFFF;
//      uint32_t black = 0xFF000000;
//      uint32_t white = 0xFFFFFFFF;
//      uint32_t red = 0xFFFF0000;
//      uint32_t green = 0xFF00FF00;
//      uint32_t blue = 0xFF0000FF;
//      uint32_t yellow = 0xFFFFFF00;
//      uint32_t orange = 0xFFFF8000;

      auto n = points.size();
      //auto v = getVertexCount();

      DE_DEBUG("[Begin] Polyline caps:", int(lineCap), ", join:", int(lineJoin), ", "
                        "n:",n," )" )
      if ( n < 2 )
      {
         if ( n == 1 )
         {
            DE_ERROR("INFORMATION-LOSS!")
         }
         return false; // nothing todo, empty input
      }


      auto addVertex = [ & ] ( glm::vec2 const & pos, glm::vec2 const & nrm, uint32_t color, float u, float v )
      {
         float x = (nrm.x * lineWidth) + pos.x;
         float y = (nrm.y * lineWidth) + pos.y;
         o.addVertex( S3DVertex( x,y,0, 0,0,-1, color, u,v ) );
      };

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
      auto addLineCapStart = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, uint32_t color, LineCap::ELineCap lineCap )
      {
         DE_DEBUG( "StartLineCap A(",A,"), B(",B,"), color(",color,")")
         auto v = o.getVertexCount();
         auto dAB = glm::normalize( B - A );
         auto nAB = getOrtho2D( dAB );      // Compute normal nAB
         uint32_t colorSL = varyColor( color, 30 );
         uint32_t colorSR = varyColor( color, 30 );
         uint32_t colorAL = color;
         uint32_t colorAR = varyColor( color, 30 );

         addVertex( A, -nAB - dAB, colorSL, 0,0 ); // SL
         addVertex( A,  nAB - dAB, colorSR, 0,0 ); // SR
         addVertex( A, -nAB,       colorAL, 0,0 ); // AL
         addVertex( A,  nAB,       colorAR, 0,0 ); // AR
         o.addIndexedQuad( v, v + 1, v + 2, v + 3 );
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
      auto addLineCapEnd = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, uint32_t color, LineCap::ELineCap lineCap )
      {
         DE_DEBUG( "EndLineCap A(",A,"), B(",B,"), color(",color,")" )
         auto v = o.getVertexCount();
         auto dAB = glm::normalize( B - A );
         auto nAB = getOrtho2D( dAB );      // Compute normal nAB
         uint32_t colorBL = varyColor( color, 30 );
         uint32_t colorBR = varyColor( color, 30 );
         uint32_t colorEL = color;
         uint32_t colorER = varyColor( color, 30 );
         addVertex( B, -nAB,       colorBL, 0,0 ); // BL
         addVertex( B,  nAB,       colorBR, 0,0 ); // BR
         addVertex( B, -nAB + dAB, colorEL, 0,0 ); // EL
         addVertex( B,  nAB + dAB, colorER, 0,0 ); // ER
         o.addIndexedQuad( v, v + 1, v + 2, v + 3 );
      };

      // [SegmentAB] A normal Segment AB, BC, CD, etc.. to p[ N-2 ]p[ N-1 ] for N >= 2
      //
      //    AL-----BL   A,B are stored, and their normal displacements +-nAB.
      //    |     /|
      //    |    / |
      //    A------B
      //    |  /   |
      //    | /    |
      //    AR-----BR
      //
      auto addSegment = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, uint32_t color )
      {
         auto v = o.getVertexCount();
         auto dAB = glm::normalize( B - A );
         auto nAB = getOrtho2D( dAB );      // Compute normal nAB
         DE_DEBUG( "Segment A(",A,"), B(",B,"), dAB(",dAB,"), nAB(",nAB,")" )
         addVertex( A, -nAB, color, 0,0 );                  // AL
         addVertex( A,  nAB, varyColor( color, 30 ), 0,0 ); // AR
         addVertex( B, -nAB, varyColor( color, 30 ), 0,0 ); // BL
         addVertex( B,  nAB, varyColor( color, 30 ), 0,0 ); // BR
         o.addIndexedQuad( v, v + 1, v + 2, v + 3 );
      };

      // Add Line Join between points A,B,C with possible type Bevel, Miter or Round )
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
      // |--------        |    /\
      // |        --------|  /   \
      // AR---------------BR------+
      //
      // turn right ( math negative, cw )
      //
      // AL---------------BL------+    AL,AR,BL + AR,BL,
      // |        --------| \    /
      // |--------        |   \ /    Bevel + Miter
      // A----------CR----B----CL
      // |----      /    /|   /
      // |    ---- / -  / |  /
      // AR-------/----/--BR/
      //         /    /    /        // SR = B + 0.5 ( nAB + nBC );
      //        /    /    /         // BL = B - nBC;
      //       /    /    /          // CL = B - nAB;
      //      /    /    /
      //     DR---C----DL
      auto addLineJoin = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, glm::vec2 const & C, LineJoin::ELineJoin const & lineJoin )
      {
         auto v = o.getVertexCount();
         auto n00 = glm::vec2( 0, 0 );
         auto nAB = getNormal2D( B - A );
         auto nBC = getNormal2D( C - B );
         auto phi = getAngleABC( A, B, C );    // Compute angle between vector AB and BC

         #ifdef USE_POINT_SKIPPING
         // if (parallel) then combine both segments by skipping this one.
         if ( std::abs( phi ) < 0.00001f || std::abs( phi - Math::TWO_PI ) < 0.00001f )
         {
            B = C;                        // Skip current segment
            nAB = getNormal2D( B - A );   // Update normal nAB
            std::cout << __FUNCTION__ << " :: Skip parallel point(" << i << ")\n";
            continue;
         }
         #endif
         // Right Line Join ( Bevel, Miter or Round )
         if ( phi > 0.0f ) // turn left ( math positive, ccw )
         {
            DE_DEBUG("LeftLineJoin phi(",phi,"), join(", int(lineJoin),"), A(",A,"), B(",B,"), C(",C,")" )
            if ( lineJoin == LineJoin::Bevel )
            {
               //addVertex3D( B, n00, color );  // B
               //addVertex3D( B, nAB, color );  // BR = B + nAB
               //addVertex3D( B, nBC, color );  // CR = B + nBC
               //addIndexedTriangle( v, v + 1, v + 2 ); // Bevel Triangle B,BR,CR
               o.addIndexedTriangle( v + 1, v, v - 1 ); // Bevel Triangle CR,CL,BR
            }
         }
         // Left Line Join, CL, BL, CR ( Bevel, Miter or Round )
         else
         {
            DE_DEBUG("RightLineJoin phi(",phi,"), join(",int(lineJoin),"), A(",A,"), B(",B,"), C(",C,")" )
            if ( lineJoin == LineJoin::Bevel )
            {
               //addVertex3D( B,  n00, color ); // B
               //addVertex3D( B, -nBC, color ); // CL
               //addVertex3D( B, -nAB, color ); // BL
               //addIndexedTriangle( v, v + 1, v + 2 ); // Left Bevel Triangle B,CL,BL ccw;
               o.addIndexedTriangle( v, v - 2, v + 1 ); // Bevel Triangle CL,BL,CR
            }
         }
      };

      //
      // PolyLine algo from Benni for Bevel,Miter and RoundJoin,
      // cant use TriangleStrip because of poss. RoundJoin? ( RoundJoin as TriangleStrip can be wasteful )
      //
      //uint32_t baseColor = green;
      glm::vec2 A,B,C; // We also need normals for both line-segments AB and BC
      A = points[ 0 ];    // Fetch A
      B = points[ 1 ];    // Fetch B
      addLineCapStart( A,B, 0xFFAA3388, lineCap );
      addSegment( A,B, color );
      for ( size_t i = 2; i < n; ++i )
      {
         C = points[ i ]; // Fetch C
         // Add Join ABC
         float32_t phi = getAngleABC( A,B,C ); // Compute angle between AB and BC just for debug coloring
         uint32_t color = 0xFF00FFFF;
         if ( phi < 0.0f ) { color = 0xFFFF00FF; }
         addLineJoin( A,B,C, lineJoin );
         // Add Segment BC
         addSegment( B,C, varyColor( color,30 ) );
         // Next point
         A = B;
         B = C;
      }
      addLineCapEnd( A,B, 0xFFAAC388, lineCap );
      return true;
   }

   static void
   add( SMeshBuffer & o, std::vector< glm::vec2 > const & points,
        uint32_t color = 0xFFFFFFFF, int lineWidth = 1 )
   {
      if ( points.size() < 2 ) { return; }

      float radius = (0.5f * lineWidth);

      auto addSegment = [&] ( glm::vec2 const & a, glm::vec2 const & b )
      {
         glm::vec2 m = Math::getNormal2D( glm::vec2( b ) - glm::vec2( a ) );
         glm::vec2 n = glm::vec2( m.x, m.y ) * radius;
         SQuad::add( o, glm::vec3( a - n, 0),
                        glm::vec3( b - n, 0),
                        glm::vec3( b + n, 0),
                        glm::vec3( a + n, 0), color );
      };

      glm::vec3 a = points[ 0 ];



      for ( size_t i = 1; i < points.size(); ++i )
      {
         glm::vec3 b = points[ i ];
         addSegment( a, b );
         a = b;
      }
   }

   static void
   addBilligUndIncorrect( SMeshBuffer & o, std::vector< glm::vec2 > const & points,
        uint32_t color = 0xFFFFFFFF, int lineWidth = 1 )
   {
      if ( points.size() < 2 )
      {
         return;
      }

      float radius = (0.5f * lineWidth);

      auto addSegment = [&] ( glm::vec2 const & a, glm::vec2 const & b )
      {
         glm::vec2 m = Math::getNormal2D( glm::vec2( b ) - glm::vec2( a ) );
         glm::vec2 n = glm::vec2( m.x, m.y ) * radius;
         SQuad::add( o, glm::vec3( a - n, 0),
                        glm::vec3( b - n, 0),
                        glm::vec3( b + n, 0),
                        glm::vec3( a + n, 0), color );
      };

      glm::vec3 a = points[ 0 ];
      for ( size_t i = 1; i < points.size(); ++i )
      {
         glm::vec3 b = points[ i ];
         addSegment( a, b );
         a = b;
      }
   }

   */

