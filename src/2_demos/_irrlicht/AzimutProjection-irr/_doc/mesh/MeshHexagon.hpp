#pragma once
#include <de/gpu/Mesh.hpp>
#include <de/gpu/MeshTool.hpp>

namespace de {
namespace gpu {

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
   DE_CREATE_LOGGER("MeshHexagon")
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

   static void
   test( float w, float h )
   {
      DE_DEBUG("MeshHexagon Angles for w(",w,"), h(",h,")")
      for ( int i = 0; i < 6; ++i )
      {
         DE_DEBUG("MeshHexagon.CornerAngle[", i,"] = ", computeCornerAngle( i,w,h ) )
      }
      for ( int i = 0; i < 6; ++i )
      {
         DE_DEBUG("MeshHexagon.EdgeAngle[", i,"] = ", computeEdgeAngle( i,w,h ) )
      }
   }
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
      o.Material.setCulling( false );
      o.Material.setBlend( Blend::alphaBlend() );
      return o;
   }
};

} // end namespace gpu.
} // end namespace de.


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
