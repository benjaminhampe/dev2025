#pragma once
#include <cstdint>
#include <cmath>
#include <functional>
#include <de_glm.hpp>

namespace de {

// =========================================================================
struct Geom3D_Sphere
// =========================================================================
{
    typedef float T;

    constexpr static T T_PI = 3.1415926535897932384626433832795028841971693993751;
    constexpr static T T_TWOPI = 2.0 * T_PI;
    constexpr static T T_RAD2DEG = 180.0 / T_PI;
    constexpr static T T_DEG2RAD = T_PI / 180.0;

    typedef uint32_t C4;
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;

    // [Longitude] Rotation um die y-Achse in degrees, equatorial xz plane is ground plane +Y is north pole
    // [Latitude] Rotation um die x/z-Achse in degrees: meridian plane (yx or yz)
    // in degrees
    static V3
    computeSphereNormal( T lon, T lat )
    {
        if ( lat >= T(89.999) ) { return V3( 0,1,0 ); } // NorthPole
        else if ( lat <= T(-89.999) ) { return V3( 0,-1,0 ); } // SouthPole
        T lon_rad = lon * T_DEG2RAD;
        T lat_rad = lat * T_DEG2RAD;
        T x = ::cosf( lon_rad ) * ::cosf( lat_rad );
        T y =                     ::sinf( lat_rad );
        T z = ::sinf( lon_rad ) * ::cosf( lat_rad );
        return V3( x,y,z );
    }

    static V3
    computeSphereTangent( T lon, T lat )
    {
        return {};
    }

    //typedef std::function< void( V3 pos, V3 normal, V2 texcoords ) > FN_ADD_VERTEX;
    typedef std::function< void( V3, V3, V2,
            V3, V3, V2,
            V3, V3, V2,
            V3, V3, V2 ) > FN_ADD_QUAD;

    // VertexOnly
    static void addTriangles( const FN_ADD_QUAD & addQuad,
                              const V3 & siz = V3(100,100,100),
                              const C4 tessLon = 12,
                              const C4 tessLat = 12,
                              const V3 & offset = V3(0,0,0) )
    {
        if ( tessLat < 3 ) return;
        if ( tessLon < 3 ) return;

        const V3 radius = siz * 0.5f;

        // u-step
        const T lonStep = T(360) / T( tessLon );
        const T lonStart = T(0); // Greenwich // T lonEnd = 360.0f;
        const T lonUV = T(1) / T( tessLon );

        // v-step
        const T latStep = T(-180) / T( tessLat );
        const T latStart = T(90); // T latEnd = T(-90);
        const T latUV = T(1) / T( tessLat );

        for ( size_t j = 0; j < tessLat; ++j ) {
        for ( size_t i = 0; i < tessLon; ++i ) {

            T lonA = lonStart + lonStep * i;
            T latA = latStart + latStep * j;
            V3 An = computeSphereNormal( lonA, latA ); // <- compute actual Sphere
            V3 Ap = An * radius + offset;
            V2 Auv = V2( lonUV * i, latUV * j );

            T lonB = lonStart + lonStep * (i);
            T latB = latStart + latStep * (j+1);
            V3 Bn = computeSphereNormal( lonB, latB ); // <- compute actual Sphere
            V3 Bp = Bn * radius + offset;
            V2 Buv = V2( lonUV * (i), latUV * (j+1) );

            T lonC = lonStart + lonStep * (i+1);
            T latC = latStart + latStep * (j+1);
            V3 Cn = computeSphereNormal( lonC, latC ); // <- compute actual Sphere
            V3 Cp = Cn * radius + offset;
            V2 Cuv = V2( lonUV * (i+1), latUV * (j+1) );

            T lonD = lonStart + lonStep * (i+1);
            T latD = latStart + latStep * (j);
            V3 Dn = computeSphereNormal( lonD, latD ); // <- compute actual Sphere
            V3 Dp = Dn * radius + offset;
            V2 Duv = V2( lonUV * (i+1), latUV * (j) );

            addQuad( Ap,An,Auv, Bp,Bn,Buv, Cp,Cn,Cuv, Dp,Dn,Duv );
        }
        }
    }

    // void addVertex( V3 pos, V3 normal, V2 texcoords )
    // void addIndexedQuad( V3 pos, V3 normal, V2 texcoords )
    typedef std::function< C4(void) >
        FN_GET_VERTEX_COUNT;
    typedef std::function< void( V3 pos, V3 normal, V2 texcoords ) >
        FN_ADD_VERTEX;
    typedef std::function< void( C4 a, C4 b, C4 c ) >
        FN_ADD_INDEX_TRIANGLE;

    // GL_TRIANGLES with IBO
    static void
    addIndexedTriangles( const FN_GET_VERTEX_COUNT & getVertexCount,
                         const FN_ADD_VERTEX & addVertex,
                         const FN_ADD_INDEX_TRIANGLE & addIndexedTriangle,
                         const V3 & siz = V3(100,100,100),
                         const C4 tessLon = 12,
                         const C4 tessLat = 12,
                         const V3 & offset = V3(0,0,0) )
    {
        if ( tessLat < 3 ) return;
        if ( tessLon < 3 ) return;

        const C4 v = getVertexCount();

        const V3 radius = siz * T(0.5);

        // u-step
        const T lonStep = T(360) / T( tessLon );
        const T lonStart = T(0); // Greenwich // T lonEnd = T(360);
        const T lonUV = T(1) / T( tessLon );

        // v-step
        const T latStep = T(-180) / T( tessLat );
        const T latStart = T(90); //T latEnd = T(-90);
        const T latUV = T(1) / T( tessLat );

        // Predict and reserve memory, indexed quads.
        //o.moreVertices( 4 * tessLon * tessLat );
        //o.moreIndices( 6 * tessLon * tessLat );

        for ( size_t j = 0; j < tessLat; ++j )
        {
        for ( size_t i = 0; i < tessLon; ++i )
        {
            const T lon = lonStart + lonStep * i;
            const T lat = latStart + latStep * j;
            const V3 n = computeSphereNormal( lon, lat ); // <- compute actual Sphere
            const V3 p = n * radius + offset;
            const V2 uv = V2( lonUV * i, latUV * j );
            addVertex( p, n, uv );
        }
        }

        auto addQuad = [&] ( int32_t i, int32_t j )
        {
            if ( j == C4(tessLat) - 1 ) return; // iB = v + ix * tessLon;
            if ( i == C4(tessLon) - 1 ) return; // iD = v + j;
            // if ( i == tessLon - 1 || j == tessLat - 1 ) return; // iC = v;
            C4 iA = v + C4(i  ) * tessLon + C4(j  );
            C4 iB = v + C4(i  ) * tessLon + C4(j+1);
            C4 iC = v + C4(i+1) * tessLon + C4(j+1);
            C4 iD = v + C4(i+1) * tessLon + C4(j  );

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
    // T lon1 = lonStart + lonStep * ix;
    // T lon2 = lonStart + lonStep * ( ix + 1 );
    // T lat1 = latStart + latStep * iy;
    // T lat2 = latStart + latStep * ( iy + 1 );
    // V3 na = computeSphereNormal( lon1, lat2 );
    // V3 nb = computeSphereNormal( lon1, lat1 );
    // V3 nc = computeSphereNormal( lon2, lat1 );
    // V3 nd = computeSphereNormal( lon2, lat2 );
    // V3 a = na * radius + offset;
    // V3 b = nb * radius + offset;
    // V3 c = nc * radius + offset;
    // V3 d = nd * radius + offset;
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


