#pragma once
#include <cstdint>
#include <sstream>
#include <functional>

namespace de {

// =========================================================================
struct Sphere
// =========================================================================
{
   // [Longitude] Rotation um die y-Achse in degrees, equatorial xz plane is ground plane +Y is north pole
   // [Latitude] Rotation um die x/z-Achse in degrees: meridian plane (yx or yz)
   // in degrees
   static glm::vec3
   computeSphereNormal( float lon, float lat )
   {
      if ( lat >= 89.999f ) { return glm::vec3( 0,1,0 ); } // NorthPole
      else if ( lat <= -89.999f ) { return glm::vec3( 0,-1,0 ); } // SouthPole
      float lon_rad = lon * Math::DEG2RAD;
      float lat_rad = lat * Math::DEG2RAD;
      float x = ::cosf( lon_rad ) * ::cosf( lat_rad );
      float y =                     ::sinf( lat_rad );
      float z = ::sinf( lon_rad ) * ::cosf( lat_rad );
      return glm::vec3( x,y,z );
   }


   // void addVertex( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords )
   // void addIndexedQuad( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords )
   //typedef std::function< uint32_t(void) > FN_GET_VERTEX_COUNT;
   //typedef std::function< void( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords ) > FN_ADD_VERTEX;
   typedef std::function< void( glm::vec3, glm::vec3, glm::vec2,
                                glm::vec3, glm::vec3, glm::vec2,
                                glm::vec3, glm::vec3, glm::vec2,
                                glm::vec3, glm::vec3, glm::vec2 ) >
      FN_ADD_QUAD;

   // VertexOnly
   static void
   addTriangles(
         FN_ADD_QUAD const & addQuad,
         glm::vec3 const & siz = glm::vec3(100,100,100),
         uint32_t tessLon = 12,
         uint32_t tessLat = 12,
         glm::vec3 const & offset = glm::vec3() )
   {
      if ( tessLat < 3 ) tessLat = 3;
      if ( tessLon < 3 ) tessLon = 3;

      glm::vec3 const radius = siz * 0.5f;

      // u-step
      float const lonStep = 360.0f / float( tessLon );
      float const lonStart = 0.0f; // Greenwich
      //float lonEnd = 360.0f;
      float const lonUV = 1.0f / float( tessLon );

      // v-step
      float const latStep = -180.0f / float( tessLat );
      float const latStart = 90.0f;
      //float latEnd = -90.0f;
      float const latUV = 1.0f / float( tessLat );

      for ( size_t j = 0; j < tessLat; ++j )
      {
         for ( size_t i = 0; i < tessLon; ++i )
         {
            //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
            float Alon = lonStart + lonStep * i;
            float Alat = latStart + latStep * j;
            glm::vec3 An = computeSphereNormal( Alon, Alat ); // <- compute actual Sphere
            glm::vec3 Ap = An * radius + offset;
            glm::vec2 Auv = glm::vec2( lonUV * i, latUV * j );
            float Blon = lonStart + lonStep * (i);
            float Blat = latStart + latStep * (j+1);
            glm::vec3 Bn = computeSphereNormal( Blon, Blat ); // <- compute actual Sphere
            glm::vec3 Bp = Bn * radius + offset;
            glm::vec2 Buv = glm::vec2( lonUV * (i), latUV * (j+1) );
            float Clon = lonStart + lonStep * (i+1);
            float Clat = latStart + latStep * (j+1);
            glm::vec3 Cn = computeSphereNormal( Clon, Clat ); // <- compute actual Sphere
            glm::vec3 Cp = Cn * radius + offset;
            glm::vec2 Cuv = glm::vec2( lonUV * (i+1), latUV * (j+1) );
            float Dlon = lonStart + lonStep * (i+1);
            float Dlat = latStart + latStep * (j);
            glm::vec3 Dn = computeSphereNormal( Dlon, Dlat ); // <- compute actual Sphere
            glm::vec3 Dp = Dn * radius + offset;
            glm::vec2 Duv = glm::vec2( lonUV * (i+1), latUV * (j) );
            addQuad( Ap,An,Auv, Bp,Bn,Buv, Cp,Cn,Cuv, Dp,Dn,Duv );
         }
      }
   }

   // void addVertex( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords )
   // void addIndexedQuad( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords )
   typedef std::function< uint32_t(void) > FN_GET_VERTEX_COUNT;
   typedef std::function< void( glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoords ) > FN_ADD_VERTEX;
   typedef std::function< void( uint32_t a, uint32_t b, uint32_t c ) > FN_ADD_INDEXED_TRIANGLE;

   // GL_TRIANGLES with IBO
   static void
   addIndexedTriangles(
         FN_GET_VERTEX_COUNT const & getVertexCount,
         FN_ADD_VERTEX const & addVertex,
         FN_ADD_INDEXED_TRIANGLE const & addIndexedTriangle,
         glm::vec3 const & siz = glm::vec3(100,100,100),
         uint32_t tessLon = 12,
         uint32_t tessLat = 12,
         glm::vec3 const & offset = glm::vec3() )
   {
      if ( tessLat < 3 ) tessLat = 3;
      if ( tessLon < 3 ) tessLon = 3;

      uint32_t const v = getVertexCount();

      glm::vec3 const radius = siz * 0.5f;

      // u-step
      float const lonStep = 360.0f / float( tessLon );
      float const lonStart = 0.0f; // Greenwich
      //float lonEnd = 360.0f;
      float const lonUV = 1.0f / float( tessLon );

      // v-step
      float const latStep = -180.0f / float( tessLat );
      float const latStart = 90.0f;
      //float latEnd = -90.0f;
      float const latUV = 1.0f / float( tessLat );

      // Predict and reserve memory, indexed quads.
      //o.moreVertices( 4 * tessLon * tessLat );
      //o.moreIndices( 6 * tessLon * tessLat );

      for ( size_t j = 0; j < tessLat; ++j )
      {
         for ( size_t i = 0; i < tessLon; ++i )
         {
            //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
            float lon = lonStart + lonStep * i;
            float lat = latStart + latStep * j;
            glm::vec3 n = computeSphereNormal( lon, lat ); // <- compute actual Sphere
            glm::vec3 p = n * radius + offset;
            glm::vec2 uv = glm::vec2( lonUV * i, latUV * j );
            addVertex( p, n, uv );
         }
      }

      auto addQuad = [&] ( int32_t i, int32_t j )
      {
         if ( j == int32_t(tessLat) - 1 ) return; // iB = v + ix * tessLon;
         if ( i == int32_t(tessLon) - 1 ) return; // iD = v + j;
         // if ( i == tessLon - 1 || j == tessLat - 1 ) return; // iC = v;
         uint32_t iA = v + uint32_t(i  ) * tessLon + uint32_t(j  );
         uint32_t iB = v + uint32_t(i  ) * tessLon + uint32_t(j+1);
         uint32_t iC = v + uint32_t(i+1) * tessLon + uint32_t(j+1);
         uint32_t iD = v + uint32_t(i+1) * tessLon + uint32_t(j  );

         addIndexedTriangle( iA,iB,iC );
         addIndexedTriangle( iA,iC,iD );
      };

      for ( int32_t j = 0; j < int32_t(tessLat); ++j )
      {
         for ( int32_t i = 0; i < int32_t(tessLon); ++i )
         {
            addQuad( i,j );
         }
      }

      // auto addQuad = [&] ( int32_t ix, int32_t iy )
      // {
         // DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
         // float lon1 = lonStart + lonStep * ix;
         // float lon2 = lonStart + lonStep * ( ix + 1 );
         // float lat1 = latStart + latStep * iy;
         // float lat2 = latStart + latStep * ( iy + 1 );
         // glm::vec3 na = computeSphereNormal( lon1, lat2 );
         // glm::vec3 nb = computeSphereNormal( lon1, lat1 );
         // glm::vec3 nc = computeSphereNormal( lon2, lat1 );
         // glm::vec3 nd = computeSphereNormal( lon2, lat2 );
         // glm::vec3 a = na * radius + offset;
         // glm::vec3 b = nb * radius + offset;
         // glm::vec3 c = nc * radius + offset;
         // glm::vec3 d = nd * radius + offset;
         // uint32_t colorA = color;
         // uint32_t colorB = color;
         // uint32_t colorC = color;
         // uint32_t colorD = color;
         // S3DVertex A( a.x,a.y,a.z, na.x,na.y,na.z, colorA, lonUV * ix, latUV * (iy+1) );
         // S3DVertex B( b.x,b.y,b.z, nb.x,nb.y,nb.z, colorB, lonUV * ix, latUV * (iy) );
         // S3DVertex C( c.x,c.y,c.z, nc.x,nc.y,nc.z, colorC, lonUV * (ix+1), latUV * (iy) );
         // S3DVertex D( d.x,d.y,d.z, nd.x,nd.y,nd.z, colorD, lonUV * (ix+1), latUV * (iy+1) );
         // o.addQuad( A,B,C,D );
      // };

      // for ( size_t j = 0; j < tessLat; ++j )
      // {
         // for ( size_t i = 0; i < tessLon; ++i )
         // {
            // addQuad( i,j );
         // }
      // }
   }
};

} // end namespace de.


