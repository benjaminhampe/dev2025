#include <de/smesh/SMeshLibrary.h>

namespace de {
namespace smesh {

f32 de_fma( f32 a, f32 b, f32 c ) // a * b + c
{
   return (a * b) + c;
}

f32 de_lerp( f32 a, f32 b, f32 t = 0.5f )
{
   return de_fma( b-a, t, a ); //return a + (b-a) * t;
}

f32 randomf32InRange( f32 range_min = 0.0f, f32 range_max = 1.0f )
{
   return ((range_max - range_min) * (f32(rand()%65536)/f32(65536.0f))) + range_min;
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

void SMeshHexagon::add( int i_min, int i_max, int j_min, int j_max,
    SMeshBuffer & mb, f32 w, f32 h, bool forceIndices, const glm::vec3 & pos )
{
    for ( int j = j_min; j < j_max; ++j )
    {
        for ( int i = i_min; i < i_max; ++i )
        {
            add( i,j, mb, w, h, 0xFFFFFFFF, forceIndices, pos );
        }
    }
}

void SMeshHexagon::add( int i, int j, SMeshBuffer & mb, f32 w, f32 h, u32 color,
    bool forceIndices, const glm::vec3 & pos )
{
    f32 x = computeBoardPosX(w,h,i,j);
    f32 z = computeBoardPosY(w,h,i,j);
    add( mb, w, h, color, forceIndices, pos + V3(x,0,z) );
}

// static
void
SMeshHexagon::addM4( SMeshBuffer & mb, f32 w, f32 h, u32 color,
                    bool forceIndices, const glm::dmat4 & trs )
{
    bool useIndices = forceIndices || mb.getIndexCount() > 0;
    const V3 n(0,1,0);
    V3 vA = V3d(trs*V4d(V3d(     0,0,-.5f*h ), 1 ));
    V3 vB = V3d(trs*V4d(V3d(-.5f*w,0,-.25f*h), 1));
    V3 vC = V3d(trs*V4d(V3d(-.5f*w,0, .25f*h), 1));
    V3 vD = V3d(trs*V4d(V3d(     0,0, .5f*h ), 1));
    V3 vE = V3d(trs*V4d(V3d(0.5f*w,0, .25f*h), 1));
    V3 vF = V3d(trs*V4d(V3d(0.5f*w,0,-.25f*h), 1));
    const S3DVertex A( vA, n, color, V2(.5f, 1 ) );
    const S3DVertex B( vB, n, color, V2(0, .75f ));
    const S3DVertex C( vC, n, color, V2(0, .25f ));
    const S3DVertex D( vD, n, color, V2(.5f, 0 ) );
    const S3DVertex E( vE, n, color, V2(1, .25f ));
    const S3DVertex F( vF, n, color, V2(1, .75f ));
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

// static
void
SMeshHexagon::add( SMeshBuffer & mb, f32 w, f32 h, u32 color,
                    bool forceIndices, const glm::vec3 & pos )
{
    bool useIndices = forceIndices || mb.getIndexCount() > 0;
    const V3 n(0,1,0);
    const S3DVertex A( pos+V3(     0,0,-.5f*h ), n, color, V2(.5f, 1 ) );
    const S3DVertex B( pos+V3(-.5f*w,0,-.25f*h), n, color, V2(0, .75f ));
    const S3DVertex C( pos+V3(-.5f*w,0, .25f*h), n, color, V2(0, .25f ));
    const S3DVertex D( pos+V3(     0,0, .5f*h ), n, color, V2(.5f, 0 ) );
    const S3DVertex E( pos+V3(0.5f*w,0, .25f*h), n, color, V2(1, .25f ));
    const S3DVertex F( pos+V3(0.5f*w,0,-.25f*h), n, color, V2(1, .75f ));

    bool bFlip = false;
    if ( useIndices )
    {
        size_t v = mb.getVertexCount();
        mb.addVertex( A );
        mb.addVertex( B );
        mb.addVertex( C );
        mb.addVertex( D );
        mb.addVertex( E );
        mb.addVertex( F );

        mb.addIndexedTriangle( v+0, v+1, v+5, bFlip ); // ( A, B, F );
        mb.addIndexedTriangle( v+1, v+4, v+5, bFlip ); // ( B, E, F );
        mb.addIndexedTriangle( v+1, v+2, v+4, bFlip ); // ( B, C, E );
        mb.addIndexedTriangle( v+2, v+3, v+4, bFlip ); // ( C, D, E );

    }
    else
    {
        mb.addVertexTriangle( A, B, F, bFlip );
        mb.addVertexTriangle( B, E, F, bFlip );
        mb.addVertexTriangle( B, C, E, bFlip );
        mb.addVertexTriangle( C, D, E, bFlip );
    }

    mb.recalculateBoundingBox();
}

// static
glm::ivec2
SMeshHexagon::getNeighborTileIndex( int corner, int i, int j )
{
    if ( Math::safeMod(j,2) == 1 )
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

// static
glm::vec2
SMeshHexagon::getNeighborTilePos( int corner, f32 w, f32 h, int i, int j )
{
    glm::ivec2 n = getNeighborTileIndex( corner, i, j );
    return glm::vec2( computeBoardPosX( w,h,n.x,n.y),
                     computeBoardPosY( w,h,n.x,n.y) );
}

// static
f32
SMeshHexagon::computeBoardPosX( f32 w, f32 h, int i, int j )
{
    //f32 x = 0.5f * f32( Math::safeMod( j, 2 ) == 1 ) + f32( i );
    //return x * w;
    //return (0.5f * j + f32( i )) * w;
    f32 x = 0.5f * f32( Math::safeMod( j, 2 ) == 1 ) + f32( i );
    return x * w;
}

// static
f32
SMeshHexagon::computeBoardPosY( f32 w, f32 h, int i, int j )
{
    //f32 y = 3.0f/4.0f * f32( j );
    //return y * h;
    return h * 3.0f/4.0f * f32( j );
}

// static
glm::ivec2
SMeshHexagon::computeTileIndex( f32 x, f32 y, f32 w, f32 h )
{
    int j = int( y / h );
    int i = int( x/w - (0.5f * j) );
    return glm::ivec2(i,j);
}

glm::vec3 SMeshHexagon::cornerA3( f32 w, f32 h ) { return glm::vec3( 0.0f,   0.0f,  -.5f*h ); }
glm::vec3 SMeshHexagon::cornerB3( f32 w, f32 h ) { return glm::vec3( -.5f*w, 0.0f, -.25f*h ); }
glm::vec3 SMeshHexagon::cornerC3( f32 w, f32 h ) { return glm::vec3( -.5f*w, 0.0f,  .25f*h ); }
glm::vec3 SMeshHexagon::cornerD3( f32 w, f32 h ) { return glm::vec3( 0.0f,   0.0f,   .5f*h ); }
glm::vec3 SMeshHexagon::cornerE3( f32 w, f32 h ) { return glm::vec3( 0.5f*w, 0.0f,  .25f*h ); }
glm::vec3 SMeshHexagon::cornerF3( f32 w, f32 h ) { return glm::vec3( 0.5f*w, 0.0f, -.25f*h ); }

glm::vec2 SMeshHexagon::lineCenter( glm::vec2 const & a, glm::vec2 const & b ) { return a + ( b - a ) * 0.5f; }

glm::vec2 SMeshHexagon::cornerA( f32 w, f32 h ) { return glm::vec2( 0.0f,    -.5f*h ); }
glm::vec2 SMeshHexagon::cornerB( f32 w, f32 h ) { return glm::vec2( -0.5f*w, -.25f*h ); }
glm::vec2 SMeshHexagon::cornerC( f32 w, f32 h ) { return glm::vec2( -0.5f*w, .25f*h ); }
glm::vec2 SMeshHexagon::cornerD( f32 w, f32 h ) { return glm::vec2( 0.0f,     .5f*h ); }
glm::vec2 SMeshHexagon::cornerE( f32 w, f32 h ) { return glm::vec2( 0.5f*w,  .25f*h ); }
glm::vec2 SMeshHexagon::cornerF( f32 w, f32 h ) { return glm::vec2( 0.5f*w, -.25f*h ); }

glm::vec2 SMeshHexagon::edgeAB( f32 w, f32 h ) { return lineCenter( cornerA(w,h), cornerB(w,h)); }
glm::vec2 SMeshHexagon::edgeBC( f32 w, f32 h ) { return lineCenter( cornerB(w,h), cornerC(w,h)); }
glm::vec2 SMeshHexagon::edgeCD( f32 w, f32 h ) { return lineCenter( cornerC(w,h), cornerD(w,h)); }
glm::vec2 SMeshHexagon::edgeDE( f32 w, f32 h ) { return lineCenter( cornerD(w,h), cornerE(w,h)); }
glm::vec2 SMeshHexagon::edgeEF( f32 w, f32 h ) { return lineCenter( cornerE(w,h), cornerF(w,h)); }
glm::vec2 SMeshHexagon::edgeFA( f32 w, f32 h ) { return lineCenter( cornerF(w,h), cornerA(w,h)); }

glm::vec2 SMeshHexagon::getCorner( int i, f32 w, f32 h )
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

std::vector< glm::vec2 >
SMeshHexagon::getCorners( f32 w, f32 h )
{
    return std::vector< glm::vec2 >
    {
        cornerA( w,h ), cornerB( w,h ), cornerC( w,h ),
        cornerD( w,h ), cornerE( w,h ), cornerF( w,h )
    };
}

std::vector< glm::vec3 >
SMeshHexagon::getCornersXZ( f32 w, f32 h )
{
    auto a = cornerA( w,h );
    auto b = cornerB( w,h );
    auto c = cornerC( w,h );
    auto d = cornerD( w,h );
    auto e = cornerE( w,h );
    auto f = cornerF( w,h );

    return std::vector< glm::vec3 >
    {
        glm::vec3( a.x, 0, a.y ),
        glm::vec3( b.x, 0, b.y ),
        glm::vec3( c.x, 0, c.y ),
        glm::vec3( d.x, 0, d.y ),
        glm::vec3( e.x, 0, e.y ),
        glm::vec3( f.x, 0, f.y )
    };
}

glm::vec2 SMeshHexagon::getEdge( int i, f32 w, f32 h )
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

f32 SMeshHexagon::computeAngle( glm::vec2 const & v )
{
    f32 phi = atan2( v.y, v.x ) * Math::RAD2DEG;
    //DE_DEBUG("atan2( ",v," ) = ", phi)
    if ( phi < 0.0f ) phi += 360.0f;
    return phi;
}

f32 SMeshHexagon::computeCornerAngle( int i, f32 w, f32 h )
{
    switch ( i )
    {
    case 0: return 90.0f;
    case 1: return 10.0f;
    case 2: return 20.0f;
    case 3: return 30.0f;
    case 4: return 40.0f;
    case 5: return 90.0f;
    default:
        throw std::runtime_error("Invalid i in computeCornerAngle()");
    }
}

f32 SMeshHexagon::computeEdgeAngle( int i, f32 w, f32 h )
{
    switch ( i ) // -153.435f;
    {
    case 0: return 26.5651f;
    case 1: return 90.0f;
    case 2: return 180.0f - 26.5651f;
    case 3: return 26.5651f;
    case 4: return 90.0f;
    case 5: return 180.0f - 26.5651f;
    default:
        throw std::runtime_error("Invalid i in computeEdgeAngle()");
    }
}

// =========================================================================
void SMeshBox::add( SMeshBuffer & o,
                   glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                   glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h,
                   u32 color )
{
    { // Front quad ABCD + Back quad HGFE
        S3DVertex A( a.x, a.y, a.z, 0,0,-1, color, 0.0,0.0 );
        S3DVertex B( b.x, b.y, b.z, 0,0,-1, color, 1.0,0.0 );
        S3DVertex C( c.x, c.y, c.z, 0,0,-1, color, 1.0,1.0 );
        S3DVertex D( d.x, d.y, d.z, 0,0,-1, color, 0.0,1.0 );
        o.addQuad( A,B,C,D );
        S3DVertex E( e.x, e.y, e.z, 0,0,1, color, 0.0,0.0 );
        S3DVertex F( f.x, f.y, f.z, 0,0,1, color, 1.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,0,1, color, 1.0,1.0 );
        S3DVertex H( h.x, h.y, h.z, 0,0,1, color, 0.0,1.0 );
        o.addQuad( H,G,F,E );
    }

    { // Wall R+L quad DCGH + EFBA
        S3DVertex D( d.x, d.y, d.z, 0,0,1, color, 0.0,0.0 );
        S3DVertex C( c.x, c.y, c.z, 0,0,1, color, 1.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,0,1, color, 1.0,1.0 );
        S3DVertex H( h.x, h.y, h.z, 0,0,1, color, 0.0,1.0 );
        o.addQuad( D,C,G,H );
        S3DVertex E( e.x, e.y, e.z, 0,0,-1, color, 0.0,0.0 );
        S3DVertex F( f.x, f.y, f.z, 0,0,-1, color, 1.0,0.0 );
        S3DVertex B( b.x, b.y, b.z, 0,0,-1, color, 1.0,1.0 );
        S3DVertex A( a.x, a.y, a.z, 0,0,-1, color, 0.0,1.0 );
        o.addQuad( E,F,B,A );
    }

    { // Top quad BFGC + Bottom quad AEHD
        S3DVertex B( b.x, b.y, b.z, 0,1,0, color, 0.0,0.0 );
        S3DVertex F( f.x, f.y, f.z, 0,1,0, color, 1.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,1,0, color, 1.0,1.0 );
        S3DVertex C( c.x, c.y, c.z, 0,1,0, color, 0.0,1.0 );
        o.addQuad( B,F,G,C );
        S3DVertex A( a.x, a.y, a.z, 0,-1,0, color, 0.0,0.0 );
        S3DVertex E( e.x, e.y, e.z, 0,-1,0, color, 1.0,0.0 );
        S3DVertex H( h.x, h.y, h.z, 0,-1,0, color, 1.0,1.0 );
        S3DVertex D( d.x, d.y, d.z, 0,-1,0, color, 0.0,1.0 );
        o.addQuad( A,E,H,D );
    }
}

void
SMeshBox::add( SMeshBuffer & o, glm::vec3 cubeSize, glm::vec3 pos, u32 color )
{
    f32 dx = cubeSize.x * 0.5f;
    f32 dy = cubeSize.y * 0.5f;
    f32 dz = cubeSize.z * 0.5f;
    auto a = pos + glm::vec3( -dx, -dy, -dz );
    auto b = pos + glm::vec3( -dx,  dy, -dz );
    auto c = pos + glm::vec3(  dx,  dy, -dz );
    auto d = pos + glm::vec3(  dx, -dy, -dz );
    auto e = pos + glm::vec3( -dx, -dy,  dz );
    auto f = pos + glm::vec3( -dx,  dy,  dz );
    auto g = pos + glm::vec3(  dx,  dy,  dz );
    auto h = pos + glm::vec3(  dx, -dy,  dz );
    add( o, a,b,c,d, e,f,g,h, color );
}

// =========================================================================
void SMeshBox::addLines( SMeshBuffer & o,
      glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
      glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h,
      u32 color )
{
    o.vertices.push_back(S3DVertex( a.x, a.y, a.z, 0,0,1, color, 0,1 )); // = 0
    o.vertices.push_back(S3DVertex( b.x, b.y, b.z, 0,0,1, color, 0,0 )); // = 1
    o.vertices.push_back(S3DVertex( c.x, c.y, c.z, 0,0,1, color, 1,0 )); // = 2
    o.vertices.push_back(S3DVertex( d.x, d.y, d.z, 0,0,1, color, 1,1 )); // = 3
    o.vertices.push_back(S3DVertex( e.x, e.y, e.z, 0,0,1, color, 0,1 )); // = 4
    o.vertices.push_back(S3DVertex( f.x, f.y, f.z, 0,0,1, color, 0,0 )); // = 5
    o.vertices.push_back(S3DVertex( g.x, g.y, g.z, 0,0,1, color, 1,0 )); // = 6
    o.vertices.push_back(S3DVertex( h.x, h.y, h.z, 0,0,1, color, 1,1 )); // = 7

    o.indices.push_back( 0 ); o.indices.push_back( 1 );
    o.indices.push_back( 1 ); o.indices.push_back( 2 );
    o.indices.push_back( 2 ); o.indices.push_back( 3 );
    o.indices.push_back( 3 ); o.indices.push_back( 0 );

    o.indices.push_back( 4 ); o.indices.push_back( 5 );
    o.indices.push_back( 5 ); o.indices.push_back( 6 );
    o.indices.push_back( 6 ); o.indices.push_back( 7 );
    o.indices.push_back( 7 ); o.indices.push_back( 4 );

    o.indices.push_back( 0 ); o.indices.push_back( 4 );
    o.indices.push_back( 1 ); o.indices.push_back( 5 );
    o.indices.push_back( 2 ); o.indices.push_back( 6 );
    o.indices.push_back( 3 ); o.indices.push_back( 7 );
}

void
SMeshBox::addLines( SMeshBuffer & o, glm::vec3 cubeSize, glm::vec3 pos, u32 color )
{
   f32 dx = cubeSize.x * 0.5f;
   f32 dy = cubeSize.y * 0.5f;
   f32 dz = cubeSize.z * 0.5f;
   auto a = pos + glm::vec3( -dx, -dy, -dz );
   auto b = pos + glm::vec3( -dx,  dy, -dz );
   auto c = pos + glm::vec3(  dx,  dy, -dz );
   auto d = pos + glm::vec3(  dx, -dy, -dz );
   auto e = pos + glm::vec3( -dx, -dy,  dz );
   auto f = pos + glm::vec3( -dx,  dy,  dz );
   auto g = pos + glm::vec3(  dx,  dy,  dz );
   auto h = pos + glm::vec3(  dx, -dy,  dz );
   addLines( o, a,b,c,d, e,f,g,h, color );
}

// =========================================================================
void Generic3DCone::addTrianglesXZ(
   f32 rx, f32 h, f32 rz,
   u32 innerColor,
   u32 outerColor,
   int32_t tessCircle,
   int32_t tessHeight,
   FN_GET_VERTEXCOUNT const & getVertexCount,
   FN_ADD_VERTEX const & addVertex,
   FN_ADD_INDEX const & addIndex)
{
    std::vector< glm::vec3 > positions;
    positions.reserve( tessCircle + 1 );
    positions.emplace_back( 0.0f, h, 0.0f );

    std::vector< glm::vec2 > texcoords;
    texcoords.reserve( tessCircle + 1 );
    texcoords.emplace_back( 0.5f, 0.5f );

    const f32 angleStep = f32( ( 2.0 * M_PI ) / double( tessCircle ) );
    for ( int32_t i = 0; i < tessCircle; ++i )
    {
        const f32 phi = angleStep * f32( i );
        const f32 c = ::cosf( phi );
        const f32 s = ::sinf( phi );
        positions.emplace_back( rx*c, 0.0f, rz*s );
        texcoords.emplace_back( 0.5f*c+0.5f, 0.5f*s+0.5f );
    }

    auto addTriangle = [&] ( u32 iA, u32 iB )
    {
        const u32 v = getVertexCount();
        const glm::vec3& m = positions[ 0 ];
        const glm::vec3& a = positions[ iA ];
        const glm::vec3& b = positions[ iB ];
        const glm::vec3 n = Math::getNormal3D( m, a, b );
        addVertex( m.x,m.y,m.z, n.x,n.y,n.z, innerColor, 0.5f, 0.5f );
        addVertex( a.x,a.y,a.z, n.x,n.y,n.z, outerColor, texcoords[iA].x, texcoords[iA].y );
        addVertex( b.x,b.y,b.z, n.x,n.y,n.z, outerColor, texcoords[iB].x, texcoords[iB].y );
        addIndex( v + 0 );   // ABC (cw) = ACB (ccw)
        addIndex( v + 2 );
        addIndex( v + 1 );
    };

    for ( int32_t i = 0; i < tessCircle; ++i )
    {
        if ( i == tessCircle - 1 )
        {
            u32 A = 1 + i;
            u32 B = 1;
            addTriangle( A, B );
        }
        else
        {
            u32 A = 1 + i;
            u32 B = 2 + i;
            addTriangle( A, B );
        }
    }
}

// =========================================================================
void SMeshCone::addXZ( SMeshBuffer & o, const glm::vec3 & siz,
      u32 iColor, u32 oColor, int32_t tessCircle, int32_t tessHeight )
{
    f32 rx = siz.x * 0.5f;
    f32 h = siz.y * 0.5f;
    f32 rz = siz.z * 0.5f;
    auto getVertexCount = [&] () { return o.getVertexCount(); };
    auto addIndex = [&] ( u32 index ) { o.addIndex( index ); };
    auto addVertex = [&] (  f32 px, f32 py, f32 pz,
                            f32 nx, f32 ny, f32 nz,
                            u32 color, f32 u, f32 v )
    {
        o.addVertex( S3DVertex( px,py,pz, nx,ny,nz, color, u,v ) );
    };
    Generic3DCone::addTrianglesXZ( rx,h,rz, iColor,oColor,
    tessCircle, tessHeight, getVertexCount, addVertex, addIndex );
}

// =========================================================================
glm::vec2
SMeshPolyLine::getOrtho2D( glm::vec2 const & m )
{
    return glm::vec2( m.y, -m.x );
}
glm::vec2
SMeshPolyLine::getNormal2D( glm::vec2 const & m )
{
    return glm::normalize( glm::vec2( m.y, -m.x ) );
}

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
     u32 color, f32 lineWidth, LineCap lineCap, LineJoin lineJoin )
{
   auto n = points.size();
   if ( n < 2 ) { return false; } // Not a polyline.
   //DE_DEBUG("PolylineTriangleStrip: lineCap:", int(lineCap), ", lineJoin:", int(lineJoin), ", n:",n )

   o.setPrimitiveType( gpu::PrimitiveType::TriangleStrip );

   auto addVertex = [ & ] ( glm::vec2 const & pos, glm::vec2 const & nrm, u32 color, f32 u, f32 v )
   {
      f32 x = (nrm.x * 0.5f * lineWidth) + pos.x; // Extrude pos in direction of nrm
      f32 y = (nrm.y * 0.5f * lineWidth) + pos.y; // Extrude pos in direction of nrm
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, color, u,v ) ); // Write extruded pos and always normal -Z, since its 2D XY plane.
      o.addIndex();
   };

   //
   // PolyLine algo from Benni for Bevel,Miter and RoundJoin,
   // cant use TriangleStrip because of poss. RoundJoin? ( RoundJoin as TriangleStrip can be wasteful )
   //
   f32 dist = 0.0f;
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
      u32 colorA, u32 colorB, f32 lineWidth, LineCap lineCap )
{
   o.setPrimitiveType( gpu::PrimitiveType::Triangles );

   auto addQuad = [&] ( glm::vec2 const & a_pos, glm::vec2 const & a_nrm,
                        glm::vec2 const & b_pos, glm::vec2 const & b_nrm,
                        glm::vec2 const & c_pos, glm::vec2 const & c_nrm,
                        glm::vec2 const & d_pos, glm::vec2 const & d_nrm )
   {
      f32 ax = (a_nrm.x * 0.5f * lineWidth) + a_pos.x; // Extrude pos in direction of nrm
      f32 ay = (a_nrm.y * 0.5f * lineWidth) + a_pos.y; // Extrude pos in direction of nrm
      f32 bx = (b_nrm.x * 0.5f * lineWidth) + b_pos.x; // Extrude pos in direction of nrm
      f32 by = (b_nrm.y * 0.5f * lineWidth) + b_pos.y; // Extrude pos in direction of nrm
      f32 cx = (c_nrm.x * 0.5f * lineWidth) + c_pos.x; // Extrude pos in direction of nrm
      f32 cy = (c_nrm.y * 0.5f * lineWidth) + c_pos.y; // Extrude pos in direction of nrm
      f32 dx = (d_nrm.x * 0.5f * lineWidth) + d_pos.x; // Extrude pos in direction of nrm
      f32 dy = (d_nrm.y * 0.5f * lineWidth) + d_pos.y; // Extrude pos in direction of nrm
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

void
SMeshRect::addLine2D( SMeshBuffer & o, int ax, int ay, int bx, int by,
                      u32 colorA, u32 colorB )
{
   o.setPrimitiveType( gpu::PrimitiveType::Lines );
   o.vertices.reserve( 2 );
   o.vertices.push_back( S3DVertex( ax, ay, -1, 0,0,0, colorA, 0,0 ) ); // A
   o.vertices.push_back( S3DVertex( bx, by, -1, 0,0,0, colorB, 0,0 ) ); // B
}

void
SMeshRect::add( SMeshBuffer & o, const V3 & a, const V3 & b, const V3 & c, const V3 & d,
                u32 color, f32 u1, f32 v1, f32 u2, f32 v2 )
{
   auto n = Math::getNormal3D( a,b,c );
   o.addQuad(  S3DVertex( a.x, a.y, a.z, n.x,n.y,n.z, color, u1,v2 ),
               S3DVertex( b.x, b.y, b.z, n.x,n.y,n.z, color, u1,v1 ),
               S3DVertex( c.x, c.y, c.z, n.x,n.y,n.z, color, u2,v1 ),
               S3DVertex( d.x, d.y, d.z, n.x,n.y,n.z, color, u2,v2 ) );
}

//
//  B +-----+ C --> +x, +u
//    |   / |
//    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
//  A +-----+ D
//    |           Normal -z shows towards viewer
//   +v = -y      Pos.z is always -1, so its at near plane.
void
SMeshRect::add( SMeshBuffer & o, Recti const & pos, u32 colorA,
                u32 colorB, u32 colorC, u32 colorD, gpu::TexRef const & ref )
{
    const int w = pos.w; // pos.w() > 0 ? pos.w() : ref.w();
    const int h = pos.h; // pos.h() > 0 ? pos.h() : ref.h();
    if ( w < 1 || w > 10000 ) { return; }
    if ( h < 1 || h > 10000 ) { return; }

    o.setPrimitiveType( gpu::PrimitiveType::Triangles );
    o.setBlend( gpu::Blend::alphaBlend() );
    if ( ref.tex )
    {
        o.setTexture( 0, ref );
    }

    const int x = pos.x;
    const int y = pos.y;
    const auto& coords = ref.coords; // Is identity(0,0,1,1) if ref.tex is empty/nullptr.
    const auto u1 = ref.coords.x;
    const auto v1 = ref.coords.y;
    const auto u2 = ref.coords.x + ref.coords.z;
    const auto v2 = ref.coords.y + ref.coords.w;
    const S3DVertex A( x,   y-1,   1.f, 0,0,0, colorA, u1,v1 );
    const S3DVertex B( x,   y-1+h, 1.f, 0,0,0, colorB, u1,v2 );
    const S3DVertex C( x+w, y-1+h, 1.f, 0,0,0, colorC, u2,v2 );
    const S3DVertex D( x+w, y-1,   1.f, 0,0,0, colorD, u2,v1 );
    o.addTriangle( A, B, C );
    o.addTriangle( A, C, D );
}

//
//  B +-----+ C --> +x, +u
//    |   / |
//    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
//  A +-----+ D
//    |           Normal -z shows towards viewer
//   +v = -y      Pos.z is always -1, so its at near plane.
//
void
SMeshRect::addOutline( SMeshBuffer & o, Recti const & pos, u32 color,
                        gpu::TexRef const & ref, int borderWidth )
{
    int w = pos.w; //pos.w() > 0 ? pos.w() : ref.w();
    int h = pos.h; //pos.h() > 0 ? pos.h() : ref.h();
    if ( w < 1 || w > 10000 ) { return; }
    if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( gpu::PrimitiveType::TriangleStrip );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( gpu::Blend::alphaBlend() );

   int x = pos.x;
   int y = pos.y;
//   Rectf const & coords = ref.coords;
//   auto const & u1 = coords.u1();
//   auto const & v1 = coords.v1();
//   auto const & u2 = coords.u2();
//   auto const & v2 = coords.v2();

   std::vector< glm::vec2 > points;
   points.reserve( 5 );
   points.emplace_back( x, y );
   points.emplace_back( x, y+h );
   points.emplace_back( x+w, y+h );
   points.emplace_back( x+w, y );
   points.emplace_back( x, y );
   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::None, LineJoin::Bevel );
}

void
SMeshRect::addOutline( SMeshBuffer & o, Recti const & pos, u32 colorA, u32 colorB,
    u32 colorC, u32 colorD, gpu::TexRef const & ref, int borderWidth )
{
    int w = pos.w; //pos.w() > 0 ? pos.w() : ref.w();
   int h = pos.h; //pos.h() > 0 ? pos.h() : ref.h();
   if ( w < 1 || w > 10000 ) { return; }
   if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( gpu::PrimitiveType::Lines );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( gpu::Blend::alphaBlend() );

   const int x = pos.x;
   const int y = pos.y;
   //const auto& coords = ref.coords; // Is identity(0,0,1,1) if ref.tex is empty/nullptr.
   const auto u1 = ref.coords.x;
   const auto v1 = ref.coords.y;
   const auto u2 = ref.coords.x + ref.coords.z;
   const auto v2 = ref.coords.y + ref.coords.w;
   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   //
//      int x1 = x + 1;
//      int y1 = y - 1;
//      int x2 = x + w;
//      int y2 = y + h;

   S3DVertex A1( x+1, y-1+h,  -1, 0,0,-1, colorA, u1,v2 );
   S3DVertex B1( x+1, y-1,    -1, 0,0,-1, colorB, u1,v1 );
   o.addLine( A1, B1 );

   S3DVertex B2( x,   y-1,    -1, 0,0,-1, colorB, u1,v1 );
   S3DVertex C2( x+w, y-1,    -1, 0,0,-1, colorC, u2,v1 );
   o.addLine( B2, C2 );

   S3DVertex C3( x+w, y-1,    -1, 0,0,-1, colorC, u2,v1 );
   S3DVertex D3( x+w, y-1+h,  -1, 0,0,-1, colorD, u2,v2 );
   o.addLine( C3, D3 );

   S3DVertex D4( x+w, y-2+h,  -1, 0,0,-1, colorD, u2,v2 );
   S3DVertex A4( x,   y-2+h,  -1, 0,0,-1, colorA, u1,v2 );
   o.addLine( D4, A4 );
}

// =========================================================================
void Generic2DTriangleRoundRect::addTriangles(
   f32 w, f32 h, f32 rx, f32 ry,
   u32 tess, u32 innerColor, u32 outerColor,
   GET_VERTEXCOUNT const & getVertexCount,
   ADD_VERTEX const & addVertex,
   ADD_INDEX const & addIndex )
{
   const u32 vStart = getVertexCount();

   auto addIndexedTriangle = [&] ( u32 iA, u32 iB, u32 iC )
   {
      addIndex( vStart + iA );   // input ABC (cw), output ACB (ccw)
      addIndex( vStart + iC );
      addIndex( vStart + iB );
   };

   auto addIndexedQuad = [&] ( u32 iA, u32 iB, u32 iC, u32 iD )
   {
      addIndexedTriangle( iA,iB,iC );   // input ABC (cw)
      addIndexedTriangle( iA,iC,iD );   // input ACD (cw)
   };

   const f32 x0 = -0.5f * w;
   const f32 x1 = -0.5f * w + rx;
   const f32 x2 = 0.5f * w - rx;
   const f32 x3 = 0.5f * w;
   const f32 y0 = -0.5f * h;
   const f32 y1 = -0.5f * h + ry;
   const f32 y2 = 0.5f * h - ry;
   const f32 y3 = 0.5f * h;

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
   addVertex( x2, y3, outerColor ); // K=A
   addVertex( x1, y3, outerColor ); // L=B

   // Add 20 indices for 5 main quads
   const u32 INDEX_A = 0;
   const u32 INDEX_B = 1;
   const u32 INDEX_C = 2;
   const u32 INDEX_D = 3;
   const u32 INDEX_E = 4;
   const u32 INDEX_F = 5;
   const u32 INDEX_G = 6;
   const u32 INDEX_H = 7;
   const u32 INDEX_I = 8;
   const u32 INDEX_J = 9;
   const u32 INDEX_K = 10;
   const u32 INDEX_L = 11;
   const u32 INDEX_TOPLEFT = 12 + 0*tess;
   const u32 INDEX_TOPRIGHT = 12 + 1*tess;
   const u32 INDEX_BOTTOMRIGHT = 12 + 2*tess;
   const u32 INDEX_BOTTOMLEFT = 12 + 3*tess;
   //const u32 INDEX_END = 12 + 4*tess;
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
      f32 s,c;
      SinCos() : s(0.f), c(0.f) {}
      SinCos(f32 _s,f32 _c) : s(_s), c(_c) {}
   };
   std::vector< SinCos > lookUpTable;
   lookUpTable.reserve( tess );

   f32 phi_step = f32( 0.5 * M_PI ) / f32( tess+1 );
   f32 phi = phi_step; // phi_start > 0° and phi_end < 90°
   for ( u32 i = 0; i < tess; ++i )
   {
      lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
      phi += phi_step;
   }

   // 1. topLeft FGB (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y1 - ry * lookUpTable[ i ].s;
      addVertex( x, y, outerColor );
   }
   // 2. topRight HIC (cw) quarter circle
   for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
   {
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y, outerColor );
   }
   // 3. bottomRight JKD (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y2 + ry * lookUpTable[ i ].s;
      addVertex( x, y, outerColor );
   }
   // 4. bottomLeft LEA (cw) quarter circle
   for ( int32_t i = int32_t(tess) - 1; i >= 0; --i )
   {
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y, outerColor );
   }

   // add 4*tess CornerIndices:

   // 1. topLeft FGB (cw) quarter circle, glm::vec3 B( x1, 0.f, y2 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 F = INDEX_TOPLEFT + i;
      u32 G = INDEX_TOPLEFT + i + 1;
      u32 B = INDEX_B;   // const (center)
      if ( i == 0 )             { F = INDEX_F; } // StartTriangle
      else if ( i == tess - 1 ) { G = INDEX_G; } // EndTriangle
      else                      {}               // MiddleTriangle
      addIndexedTriangle( F, G, B ); // all given in (cw)
   }

   // 2. topRight HIC (cw) quarter circle, glm::vec3 C( x2, 0.f, y2 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 H = INDEX_TOPRIGHT + i;
      u32 I = INDEX_TOPRIGHT + i + 1;
      u32 C = INDEX_C;   // const (center)
      if ( i == 0 )             { H = INDEX_H; } // StartTriangle
      else if ( i == tess - 1 ) { I = INDEX_I; } // EndTriangle
      else                      {}               // MiddleTriangle
      addIndexedTriangle( H,I,C ); // all given in (cw)
   }

   // 3. bottomRight JKD (cw) quarter circle, glm::vec3 D( x2, 0.f, y1 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 J = INDEX_BOTTOMRIGHT + i;
      u32 K = INDEX_BOTTOMRIGHT + i + 1;
      u32 D = INDEX_D;   // const (center)
      if ( i == 0 )             { J = INDEX_J;  } // StartTriangle
      else if ( i == tess - 1 ) { K = INDEX_K;  } // EndTriangle
      else                      {}                // MiddleTriangle
      addIndexedTriangle( J, K, D ); // all given in (cw)
   }

   // 4. bottomLeft LEA (cw) quarter circle, glm::vec3 A( x1, 0.f, y1 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 L = INDEX_BOTTOMLEFT + i;
      u32 E = INDEX_BOTTOMLEFT + i + 1;
      u32 A = INDEX_A;   // const (center)
      if ( i == 0 )             { L = INDEX_L; } // StartTriangle
      else if ( i == tess - 1 ) { E = INDEX_E; } // EndTriangle
      else                      {}               // MiddleTriangle
      addIndexedTriangle( L, E, A ); // all given in (cw)
   }
}


// =========================================================================
void
SMeshRoundRect::addXY( SMeshBuffer & o,
      const glm::vec3 & pos,
      f32 w, f32 h, f32 rx, f32 ry,
      int32_t tess, u32 innerColor, u32 outerColor )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addVertex = [&] ( f32 x, f32 y, u32 color )
   {
      f32 px = pos.x + x;
      f32 py = pos.y + y;
      f32 pz = pos.z;
      f32 u = 0.5f + x / w;
      f32 v = 0.5f + y / h;
      o.addVertex( S3DVertex( px,py,pz, 0,0,-1, color, u,v ) );
   };

   auto addIndex = [&] ( u32 index ) { o.addIndex( index ); };

   Generic2DTriangleRoundRect::addTriangles( w, h, rx, ry, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );

   o.recalculateBoundingBox();
}

void
SMeshRoundRect::addXZ( SMeshBuffer & o,
      const glm::vec3 & pos,
      f32 w, f32 h, f32 rx, f32 ry,
      int32_t tess, u32 innerColor, u32 outerColor )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addVertex = [&] ( f32 x, f32 y, u32 color )
   {
      f32 px = pos.x + x;
      f32 py = pos.y;
      f32 pz = pos.z + y;
      f32 u = 0.5f + x / w;
      f32 v = 0.5f - y / h;
      o.addVertex( S3DVertex( px,py,pz, 0,0,-1, color, u,v ) );
   };

   auto addIndex = [&] ( u32 index ) { o.addIndex( index ); };

   Generic2DTriangleRoundRect::addTriangles( w, h, rx, ry, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );

   o.recalculateBoundingBox();
}

void
addRoundRect( SMeshBuffer & o,
      Recti const & pos,
      f32 zIndex,
      glm::ivec2 const & radius,
      u32 outerColor,
      u32 innerColor,
      gpu::TexRef const & ref, int tess )
{
   int w = pos.w;
   int h = pos.h;
   if ( w < 3 || w > 10000 ) { return; }
   if ( h < 3 || h > 10000 ) { return; }

   o.setPrimitiveType( gpu::PrimitiveType::Triangles );
   o.setCulling( false );
   o.setBlend( gpu::Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   glm::vec3 off( w/2 + 1 + pos.x, h/2 + pos.y, zIndex );
   SMeshRoundRect::addXY( o, off, w, h, radius.x, radius.y, tess, innerColor, outerColor );
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
addRoundRectOutline( SMeshBuffer & o, Recti const & pos, f32 zIndex,
                     glm::ivec2 const & radius,
                     u32 color, gpu::TexRef const & ref, int borderWidth, u32 tess )
{
   int w = pos.w;
   int h = pos.h;
   if ( w < 4 || w > 1000000 ) { return; }
   if ( h < 4 || h > 1000000 ) { return; }
   o.setBlend( gpu::Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   glm::vec2 off( w/2 + 1 + pos.x, h/2 + pos.y );
   const f32 rx = radius.x;
   const f32 ry = radius.y;
   const f32 x0 = -0.5f * w;
   const f32 x1 = -0.5f * w + rx;
   const f32 x2 = 0.5f * w - rx;
   const f32 x3 = 0.5f * w;
   const f32 y0 = -0.5f * h;
   const f32 y1 = -0.5f * h + ry;
   const f32 y2 = 0.5f * h - ry;
   const f32 y3 = 0.5f * h;

// [SinCosLookUpTable] for a quarter circle
   struct SinCos
   {
      f32 s,c;
      SinCos() : s(0.f), c(0.f) {}
      SinCos(f32 _s,f32 _c) : s(_s), c(_c) {}
   };
   std::vector< SinCos > lookUpTable;
   lookUpTable.reserve( tess );

   f32 phi_step = f32( 0.5 * M_PI ) / f32( tess+1 );
   f32 phi = phi_step; // phi_start > 0° and phi_end < 90°
   for ( u32 i = 0; i < tess; ++i )
   {
      lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
      phi += phi_step;
   }
// [SinCosLookUpTable] End.

   std::vector< glm::vec2 > points;

   auto addPoint = [&] ( f32 x, f32 y )
   {
      points.emplace_back( off.x + x, off.y + y );
   };

   if ( h > 2*radius.y )
   {
      addPoint( x0, y2 ); // E=0
      addPoint( x0, y1 ); // F=1
   }

   // 1. topLeft FGB (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y1 - ry * lookUpTable[ i ].s;
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
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addPoint( x, y );
   }

   if ( h > 2*radius.y )
   {
      addPoint( x3, y1 ); // I=4
      addPoint( x3, y2 ); // J=5
   }


   // 3. bottomRight JKD (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y2 + ry * lookUpTable[ i ].s;
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
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addPoint( x, y );
   }

   addPoint( x0, y2 ); // E=0

   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::None, LineJoin::Bevel );

   // Set color between black and white to all vertices depending on their u value.
   f32 uMax = o.vertices.back().tex.x;
   for ( size_t i = 0; i < o.vertices.size(); ++i )
   {
      o.vertices[ i ].pos.z = zIndex;
      f32 u = o.vertices[ i ].tex.x;
      uint8_t r = static_cast<uint8_t>( std::clamp( int(std::lround( 255.0f * u / uMax )), 0, 255 ) );
      u32 color = dbRGB( r,r,r );
      o.vertices[ i ].color = color;
   }
}

/*
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
S2DRoundRectOutline::add( SMeshBuffer & o, Recti const & pos, glm::ivec2 const & radius,
      u32 color, TexRef const & ref, int borderWidth, int tess )
{
   S2DPolyLine::
   int w = pos.getWidth();
   int h = pos.getHeight();
   if ( w < 4 || w > 1000000 ) { DE_DEBUG( "Invalid w ",w ) return; }
   if ( h < 4 || h > 1000000 ) { DE_DEBUG( "Invalid h ",h ) return; }
   o.setPrimitiveType( PrimitiveType::Lines );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   glm::vec3 off( w/2 + 1 + pos.x, h/2 + pos.y, 0.0f );
   //o.moreVertices( 8 + 4*tess );
   //o.moreIndices( 8 + 8*(tess+1) );
   const f32 rx = radius.x;
   const f32 ry = radius.y;
   const f32 x0 = -0.5f * w;
   const f32 x1 = -0.5f * w + rx;
   const f32 x2 = 0.5f * w - rx;
   const f32 x3 = 0.5f * w;
   const f32 y0 = -0.5f * h;
   const f32 y1 = -0.5f * h + ry;
   const f32 y2 = 0.5f * h - ry;
   const f32 y3 = 0.5f * h;

   auto addVertex = [&] ( f32 x, f32 y )
   {
      f32 uCoord = 0.5f + x / w;
      f32 vCoord = 0.5f + y / h;
      o.addVertex( S3DVertex( off.x + x, off.y + y,-1, 0,0,-1, color, uCoord,vCoord ) );   // input ABC (cw), output ACB (ccw)
   };

   addVertex( x0, y2 ); // E=0
   addVertex( x0, y1 ); // F=1
   addVertex( x1, y0 ); // G=2
   addVertex( x2, y0 ); // H=3
   addVertex( x3, y1 ); // I=4
   addVertex( x3, y2 ); // J=5
   addVertex( x2, y3 ); // K=6
   addVertex( x1, y3 ); // L=7

   const u32 INDEX_E = 0;
   const u32 INDEX_F = 1;
   const u32 INDEX_G = 2;
   const u32 INDEX_H = 3;
   const u32 INDEX_I = 4;
   const u32 INDEX_J = 5;
   const u32 INDEX_K = 6;
   const u32 INDEX_L = 7;
   const u32 INDEX_TOPLEFT = 8 + 0*tess;
   const u32 INDEX_TOPRIGHT = 8 + 1*tess;
   const u32 INDEX_BOTTOMRIGHT = 8 + 2*tess;
   const u32 INDEX_BOTTOMLEFT = 8 + 3*tess;
   o.addIndexedLine( INDEX_E, INDEX_F );
   o.addIndexedLine( INDEX_G, INDEX_H );
   o.addIndexedLine( INDEX_I, INDEX_J );
   o.addIndexedLine( INDEX_K, INDEX_L );
   //
   // Add 4*tess corner vertices. They are between the main vertices.
   //
   struct SinCos
   {
      f32 s,c;
      SinCos() : s(0.f), c(0.f) {}
      SinCos(f32 _s,f32 _c) : s(_s), c(_c) {}
   };
   std::vector< SinCos > lookUpTable;
   lookUpTable.reserve( tess );

   f32 phi_step = f32( 0.5 * M_PI ) / f32( tess+1 );
   f32 phi = phi_step; // phi_start > 0° and phi_end < 90°
   for ( u32 i = 0; i < tess; ++i )
   {
      lookUpTable.emplace_back( ::sinf( phi ), ::cosf( phi ) );
      phi += phi_step;
   }

   // 1. topLeft FGB (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y1 - ry * lookUpTable[ i ].s;
      addVertex( x, y );
   }
   // 2. topRight HIC (cw) quarter circle
   for ( int32_t i = tess - 1; i >= 0; --i )
   {
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y );
   }
   // 3. bottomRight JKD (cw) quarter circle
   for ( u32 i = 0; i < tess; ++i )
   {
      f32 x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      f32 y = y2 + ry * lookUpTable[ i ].s;
      addVertex( x, y );
   }
   // 4. bottomLeft LEA (cw) quarter circle
   for ( int32_t i = tess - 1; i >= 0; --i )
   {
      f32 x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      f32 y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y );
   }

   // add 4*tess CornerIndices:

   // 1. topLeft FGB (cw) quarter circle, glm::vec3 B( x1, 0.f, y2 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 iF = INDEX_TOPLEFT + i;
      u32 iG = INDEX_TOPLEFT + i + 1;
      if ( i == 0 )             { iF = INDEX_F; } // StartTriangle
      else if ( i == tess - 1 ) { iG = INDEX_G; } // EndTriangle
      else                      {}                // MiddleTriangle
      o.addIndexedLine( iF, iG ); // all given in (cw)
   }

   // 2. topRight HIC (cw) quarter circle, glm::vec3 C( x2, 0.f, y2 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 iH = INDEX_TOPRIGHT + i;
      u32 iI = INDEX_TOPRIGHT + i + 1;
      if ( i == 0 )             { iH = INDEX_H; } // StartTriangle
      else if ( i == tess - 1 ) { iI = INDEX_I; } // EndTriangle
      else                      {}                // MiddleTriangle
      o.addIndexedLine( iH,iI ); // all given in (cw)
   }

   // 3. bottomRight JKD (cw) quarter circle, glm::vec3 D( x2, 0.f, y1 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 iJ = INDEX_BOTTOMRIGHT + i;
      u32 iK = INDEX_BOTTOMRIGHT + i + 1;
      if ( i == 0 )             { iJ = INDEX_J;  } // StartTriangle
      else if ( i == tess - 1 ) { iK = INDEX_K;  } // EndTriangle
      else                      {}                 // MiddleTriangle
      o.addIndexedLine( iJ, iK ); // all given in (cw)
   }

   // 4. bottomLeft LEA (cw) quarter circle, glm::vec3 A( x1, 0.f, y1 );
   for ( u32 i = 0; i < tess; ++i )
   {
      u32 iL = INDEX_BOTTOMLEFT + i;
      u32 iE = INDEX_BOTTOMLEFT + i + 1;
      if ( i == 0 )             { iL = INDEX_L; } // StartTriangle
      else if ( i == tess - 1 ) { iE = INDEX_E; } // EndTriangle
      else                      {}               // MiddleTriangle
      o.addIndexedLine( iL, iE ); // all given in (cw)
   }
}
*/


// =========================================================================
void
SMeshCircle::addXY( SMeshBuffer & o, glm::vec2 const & siz, u32 color, u32 tessRadial )
{
   addXY( o, siz, color, color, tessRadial );
}

void
SMeshCircle::addXY(
      SMeshBuffer & o,
      glm::vec2 const & siz,
      u32 innerColor,
      u32 outerColor,
      u32 tessRadial ) // bool forceIndices = false
{
   glm::vec2 radius = siz * 0.5f;

   //bool useIndices = o.getIndexCount() > 0 || forceIndices;

   // Add center point
   u32 v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

   const f32 angleStep = f32( ( 2.0 * M_PI ) / double( tessRadial ) );
//      const f32 outwardX = f32( radius.x ) / f32( tessOutwards );
//      const f32 outwardY = f32( radius.y ) / f32( tessOutwards );

   for ( u32 i = 0; i <= tessRadial; ++i )
   {
      const f32 phi = angleStep * f32( i );
      const f32 s = ::sinf( phi );
      const f32 c = ::cosf( phi );

//         for ( u32 j = 0; j <= tessOutwards; ++j )
//         {
      f32 x = (radius.x * c); // * f32( j ) * outwardX);
      f32 y = (radius.y * s); // * f32( j ) * outwardY);
      f32 u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      f32 v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
   }

   for ( u32 i = 0; i < tessRadial; ++i )
   {
//         for ( u32 j = 0; j < tessOutwards; ++j )
//         {
         u32 M = v;
         u32 A = v + 1 + i;
         u32 B = v + 1 + i + 1;
         o.addIndexedTriangle( M, A, B );
//         }
   }
}


void
SMeshCircle::addXZ( SMeshBuffer & o, glm::vec2 const & siz, u32 color, u32 tessRadial )
{
   addXZ( o, siz, color, color, tessRadial );
}

void
SMeshCircle::addXZ( SMeshBuffer & o, glm::vec2 const & siz, u32 innerColor, u32 outerColor, u32 tessRadial )
{
   glm::vec2 radius = siz * 0.5f;

   //bool useIndices = o.getIndexCount() > 0 || forceIndices;

   // Add center point
   u32 v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,1,0, innerColor, 0.5f, 0.5f ) );

   const f32 angleStep = f32( ( 2.0 * M_PI ) / double( tessRadial ) );
//      const f32 outwardX = f32( radius.x ) / f32( tessOutwards );
//      const f32 outwardY = f32( radius.y ) / f32( tessOutwards );

   for ( u32 i = 0; i <= tessRadial; ++i )
   {
      const f32 phi = angleStep * f32( i );
      const f32 s = ::sinf( phi );
      const f32 c = ::cosf( phi );

//         for ( u32 j = 0; j <= tessOutwards; ++j )
//         {
      f32 x = (radius.x * c); // * f32( j ) * outwardX);
      f32 z = (radius.y * s); // * f32( j ) * outwardY);
      f32 u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      f32 v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,0,z, 0,1,0, outerColor, u, v ) );
//         }
   }

   for ( u32 i = 0; i < tessRadial; ++i )
   {
//         for ( u32 j = 0; j < tessOutwards; ++j )
//         {
         u32 M = v;
         u32 A = v + 1 + i;
         u32 B = v + 1 + i + 1;
         o.addIndexedTriangle( M, B, A );
//         }
   }
}

// =========================================================================
void
Generic2DTriangleRing::addIndexedTriangles(
   f32 outer_x, f32 outer_y,
   f32 inner_x, f32 inner_y,
   int32_t tess, u32 innerColor, u32 outerColor,
   GET_VERTEXCOUNT const & getVertexCount,
   ADD_VERTEX const & addVertex,
   ADD_INDEX const & addIndex )
{
   const u32 vStart = getVertexCount();

   auto addIndexedQuad = [&] ( u32 a, u32 b, u32 c, u32 d )
   {
      addIndex( a ); addIndex( c ); addIndex( b ); // ABC (cw) = ACB (ccw)
      addIndex( a ); addIndex( d ); addIndex( c ); // ACD (cw) = ADC (ccw)
   };

   const f32 ix = 0.5f * inner_x;
   const f32 iy = 0.5f * inner_y;
   const f32 ox = 0.5f * outer_x; // a,b are radia, not sizes.
   const f32 oy = 0.5f * outer_y;
   const f32 phi_step = f32( ( 2.0 * M_PI ) / double( tess ) ); // 0 to 2pi

   // inner ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      const f32 phi = phi_step * f32( i );
      const f32 s = -::sinf( phi );
      const f32 c =  ::cosf( phi );
      addVertex( ix * s, iy * c, innerColor );
   }

   // outer ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      const f32 phi = phi_step * f32( i );
      const f32 s = -::sinf( phi );
      const f32 c =  ::cosf( phi );
      addVertex( ox * s, oy * c, outerColor );
   }

   // ring quad indices
   for ( int32_t i = 0; i < tess; ++i )
   {
      u32 A = vStart + i;
      u32 B = vStart + i + tess + 1;
      u32 C = vStart + i + tess + 2;
      u32 D = vStart + i + 1;
      addIndexedQuad( B, A, D, C );
   }
}

// =========================================================================
void
SMeshRing::addXY( SMeshBuffer & o, f32 ow, f32 oh, f32 iw, f32 ih,
                  u32 innerColor, u32 outerColor, int32_t tess,
                  const glm::vec3 & pos )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addIndex = [&] ( u32 index ) { o.addIndex( index ); };
   auto addVertex = [&] ( f32 px, f32 py, u32 color )
   {
      f32 x = pos.x + px;
      f32 y = pos.y + py;
      f32 z = pos.z;
      f32 u = 0.5f + px / ow;
      f32 v = 0.5f - py / oh;
      o.addVertex( S3DVertex( x,y,z, 0,0,1, color, u,v ) );
   };

   Generic2DTriangleRing::addIndexedTriangles( ow, oh, iw, ih, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );
}

void
SMeshRing::addXZ( SMeshBuffer & o, f32 ow, f32 oh, f32 iw, f32 ih,
                  u32 innerColor, u32 outerColor, int32_t tess,
                  const glm::vec3 & pos )
{
   iw *= 0.5f;
   ow *= 0.5f;
   ih *= 0.5f;
   oh *= 0.5f;

   const u32 vStart = o.getVertexCount();

   const f32 phi_step = f32( ( 2.0 * M_PI ) / double( tess ) ); // 0 to 2pi

   // Inner ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      const f32 phi = phi_step * f32( i );
      const f32 s = ::sinf( phi );
      const f32 c = ::cosf( phi );
      const f32 x = pos.x + iw * c;
      const f32 y = pos.y;
      const f32 z = pos.z + ih * s;
      const f32 u = 0.5f - 0.5f * c;
      const f32 v = 0.5f - 0.5f * s;
      o.addVertex( S3DVertex( x, y, z, 0,1,0, innerColor, u,v ) );
   }

   // Outer ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      const f32 phi = phi_step * f32( i );
      const f32 s = ::sinf( phi );
      const f32 c = ::cosf( phi );
      const f32 x = pos.x + ow * c;
      const f32 y = pos.y;
      const f32 z = pos.z + oh * s;
      const f32 u = 0.5f - 0.5f * c;
      const f32 v = 0.5f - 0.5f * s;
      o.addVertex( S3DVertex( x, y, z, 0,1,0, outerColor, u,v ) );
   }

   // Build quads
   for ( int32_t i = 0; i < tess; ++i )
   {
      u32 A = vStart + i;
      u32 B = A + tess + 1;
      u32 C = A + tess + 2;
      u32 D = A + 1;
      o.addIndexedQuad( B, A, D, C );
   }
}

// =========================================================================
/// @brief PolyHull
// =========================================================================
void
SMeshPolyHull::addLeftStripXY( SMeshBuffer & o,
                std::vector< glm::vec3 > const & points, f32 height,
                u32 color, const glm::vec3 & pos )
{
   auto addHullSegment = [&] ( glm::vec3 a, glm::vec3 b )
   {
      glm::vec3 A = pos + glm::vec3( b.x, b.y, b.z - 0.5f * height );
      glm::vec3 B = pos + glm::vec3( b.x, b.y, b.z + 0.5f * height );
      glm::vec3 C = pos + glm::vec3( a.x, a.y, a.z + 0.5f * height );
      glm::vec3 D = pos + glm::vec3( a.x, a.y, a.z - 0.5f * height );
      SMeshRect::add( o, A,B,C,D, color );
   };

   // Logic

   if ( points.size() < 2 )
   {
      return;
   }

   glm::vec3 a = points[ 0 ];
   for ( size_t i = 1; i < points.size(); ++i )
   {
      glm::vec3 b = points[ i ];
      addHullSegment( a, b );
      a = b;
   }

   addHullSegment( points[ points.size() - 1 ], points[ 0 ] );
}

void
SMeshPolyHull::addLeftStripXZ( SMeshBuffer & o,
                std::vector< glm::vec3 > const & points, f32 height,
                u32 color, const glm::vec3 & pos )
{
   auto addHullSegment = [&] ( glm::vec3 a, glm::vec3 b )
   {
      glm::vec3 A = pos + glm::vec3( a.x, a.y - 0.5f * height, a.z );
      glm::vec3 B = pos + glm::vec3( a.x, a.y + 0.5f * height, a.z );
      glm::vec3 C = pos + glm::vec3( b.x, b.y + 0.5f * height, b.z );
      glm::vec3 D = pos + glm::vec3( b.x, b.y - 0.5f * height, b.z );
      SMeshRect::add( o, A,B,C,D, color );
   };

   // Logic

   if ( points.size() < 2 )
   {
      return;
   }

   glm::vec3 a = points[ 0 ];
   for ( size_t i = 1; i < points.size(); ++i )
   {
      glm::vec3 b = points[ i ];
      addHullSegment( a, b );
      a = b;
   }

   addHullSegment( points[ points.size() - 1 ], points[ 0 ] );
}

void
SMeshPolyHull::addRightStrip( SMeshBuffer & o,
               std::vector< glm::vec3 > const & points, f32 height,
               u32 color, const glm::vec3 & pos )
{

   auto addHullSegment = [&] ( glm::vec3 a, glm::vec3 b )
   {
      glm::vec3 A = pos + glm::vec3( a.x, a.y, a.z - 0.5f * height );
      glm::vec3 B = pos + glm::vec3( a.x, a.y, a.z + 0.5f * height );
      glm::vec3 C = pos + glm::vec3( b.x, b.y, b.z + 0.5f * height );
      glm::vec3 D = pos + glm::vec3( b.x, b.y, b.z - 0.5f * height );
      SMeshRect::add( o, A,B,C,D, color );
   };

   // Logic

   if ( points.size() < 2 )
   {
      return;
   }

   glm::vec3 a = points[ 0 ];
   for ( size_t i = 1; i < points.size(); ++i )
   {
      glm::vec3 b = points[ i ];
      addHullSegment( a, b );
      a = b;
   }

   addHullSegment( points[ points.size() - 1 ], points[ 0 ] );
}

// =========================================================================
void
SMeshCylinderHull::addXY( SMeshBuffer & o,
     f32 dx, f32 dy, f32 h,
     u32 color, int tessCirc, const V3 & pos )
{
   std::vector< V3 > points;
   points.reserve( tessCirc + 1 );
   f32 angleStart = 0.0f;     // from 0 degrees
   f32 angleEnd = f32( 2.0 * M_PI ); // to 90 degrees
   f32 angleStep = ( angleEnd - angleStart) / f32( tessCirc );
   f32 a = 0.5f * dx;
   f32 b = 0.5f * dy;

   for ( int i = 0; i <= tessCirc; ++i )
   {
      f32 phi = angleStart + angleStep * f32( i );
      f32 x = a * ::cosf( phi );
      f32 y = b * ::sinf( phi );
      points.emplace_back( x, y, 0.0f );
   }

   SMeshPolyHull::addLeftStripXY( o, points, h, color, pos );
}

void
SMeshCylinderHull::addXZ( SMeshBuffer & o,
     f32 dx, f32 h, f32 dz,
     u32 color, int tessCirc, const V3 & pos )
{
   std::vector< V3 > points;
   points.reserve( tessCirc + 1 );
   f32 angleStart = 0.0f;     // from 0 degrees
   f32 angleEnd = f32( 2.0 * M_PI ); // to 90 degrees
   f32 angleStep = ( angleEnd - angleStart) / f32( tessCirc );
   f32 a = 0.5f * dx;
   f32 b = 0.5f * dz;

   for ( int i = 0; i <= tessCirc; ++i )
   {
      f32 phi = angleStart + angleStep * f32( i );
      f32 x = a * ::cosf( phi );
      f32 z = b * ::sinf( phi );
      points.emplace_back( x, 0.0f, z );
   }

   SMeshPolyHull::addLeftStripXZ( o, points, h, color, pos );
}

// =========================================================================
void
SMeshCylinder::addXY( SMeshBuffer & o, f32 dx, f32 dy, f32 h,
                      u32 color, u32 tessCirc )
{
    glm::vec3 circleOffset( 0, 0, 0.5f * h );
    glm::vec2 circleSize( dx, dy );
    u32 vStart = o.getVertexCount();
    SMeshCircle::addXY( o, circleSize, color, tessCirc );
    SMeshBufferTool::translateVertices( o, circleOffset, vStart );
    SMeshCylinderHull::addXY( o, dx,dy,h, color, tessCirc );
}

void
SMeshCylinder::addXZ( SMeshBuffer & o, f32 dx, f32 h, f32 dz,
                      u32 color, u32 tessCirc )
{
    glm::vec3 circleOffset( 0, 0.5f * h, 0 );
    glm::vec2 circleSize( dx, dz );
    u32 vStart = o.getVertexCount();
    SMeshCircle::addXZ( o, circleSize, color, tessCirc );
    SMeshBufferTool::translateVertices( o, circleOffset, vStart );
    SMeshCylinderHull::addXZ( o, dx,h,dz, color, tessCirc );
}

void
SMeshCylinder::addXZ( SMeshBuffer & o, f32 bottom_dx, f32 bottom_dz,
       f32 top_dx, f32 top_dz, f32 cylinder_h,
       u32 color, u32 tessCircle, u32 tessHull, const glm::vec3 & offset )
{
   // BEGIN: Create lookup table for a unit-circle
   std::vector< glm::vec2 > unitCircle; //x,y,u,v for full radius
   unitCircle.reserve( tessCircle + 1 );
   f32 angleStart = 0.0f;     // from 0 degrees
   f32 angleEnd = f32( 2.0 * M_PI ); // to 90 degrees
   f32 angleStep = ( angleEnd - angleStart) / f32( tessCircle - 1 );
   for ( size_t i = 0; i < tessCircle; ++i )
   {
      f32 phi = angleStart + angleStep * f32( i );
      f32 cx = ::cosf( phi );
      f32 sy = ::sinf( phi );
      unitCircle.emplace_back( cx, sy );
   }
   unitCircle.emplace_back( unitCircle[ 0 ] );

   // END: Create lookup table for a unit-circle

   // BEGIN: Vertex positions, to be reused.
   f32 top_a = 0.5f * top_dx;
   f32 top_b = 0.5f * top_dz;
   f32 bot_a = 0.5f * bottom_dx;
   f32 bot_b = 0.5f * bottom_dz;
   std::vector< glm::vec3 > a_pos; // reusable vertex pos
   a_pos.reserve( (tessHull + 1) * (tessCircle + 1) );
   for ( size_t j = 0; j < tessHull+1; ++j )
   {
      f32 t = f32(j)/f32(tessHull-1);
      for ( size_t i = 0; i < tessCircle; ++i )
      {
         glm::vec2 c = unitCircle[ i ];
         f32 r = randomf32InRange(0.98,1.02);
         //f32 g = randomf32InRange(0.98,1.02);
         f32 x = offset.x + (c.x * de_lerp( bot_a, top_a, t ) * r);
         f32 y = offset.y + (t * cylinder_h);
         f32 z = offset.z + (c.y * de_lerp( bot_b, top_b, t ) * r);
         a_pos.emplace_back( x,y,z );
      }
      a_pos.emplace_back( a_pos[ j*(tessCircle+1) ] );
   }

   // END: Create lookup table for a unit-circle

   // BEGIN: Raw vertex quads, not indexed quads
   for ( size_t j = 0; j < tessHull; ++j )
   {
      for ( size_t i = 0; i < tessCircle; ++i )
      {
         size_t a = i + j * (tessCircle+1);
         size_t b = i + (j+1) * (tessCircle+1);
         size_t c = (i+1) + (j+1) * (tessCircle+1);
         size_t d = (i+1) + j * (tessCircle+1);

         if ( a >= a_pos.size() )
         {
            DE_RUNTIME_ERROR( dbStr("a = ",a,", n = ",a_pos.size()),"Got a >= a_pos.size()" )
         }
         if ( b >= a_pos.size() )
         {
            DE_RUNTIME_ERROR( dbStr("b = ",b,", n = ",a_pos.size()),"Got b >= a_pos.size()" )
         }
         if ( c >= a_pos.size() )
         {
            DE_RUNTIME_ERROR( dbStr("c = ",c,", n = ",a_pos.size()),"Got c >= a_pos.size()" )
         }
         if ( d >= a_pos.size() )
         {
            DE_RUNTIME_ERROR( dbStr("d = ",d,", n = ",a_pos.size()),"Got d >= a_pos.size()" )
         }

         glm::vec3 n(0,1,0);
         S3DVertex A( a_pos[ a ], n, color, glm::vec2(0,1) );
         S3DVertex B( a_pos[ b ], n, color, glm::vec2(0,0) );
         S3DVertex C( a_pos[ c ], n, color, glm::vec2(1,0) );
         S3DVertex D( a_pos[ d ], n, color, glm::vec2(1,1) );
         o.addQuad( A, B, C, D );
      }
   }
}

// =========================================================================
// Tube
// =========================================================================

void
SMeshTube::add( SMeshBuffer & o,
    // Describes outer cylinder
    f32 dx, // (outer) cylinder width
    f32 dz, // (outer) cylinder depth
    f32 hy, // (outer) cylinder height
    // Describes inner cylinder, rx,ry define the extrusion relative inside to dx,dy.
    // They define the inside border width and are not absolute values like dx,dy.
    f32 rx, // inside going border-width ( inside cylinder width cx = dx - rx )
    f32 rz, // inside going border-depth ( inside cylinder depth cz = dz - rz )
    u32 color, // one color to rule them all (vertices).
    int32_t tessCircle,// tesselation of top and bottom ring = num ring segments
    int32_t tessHeight )// tesselation of cylinder hull height ( y-direction )
{
    SMeshRing::addXZ( o, dx, dz, rx, rz, color, color, tessCircle );
}

// =========================================================================
// Sphere
// =========================================================================

glm::vec3
SMeshSphere::computeNormal( f32 lon_degrees, f32 lat_degrees ) // in degrees
{
    if ( lat_degrees >= 89.999f ) { return glm::vec3( 0,1,0 ); } // NorthPole
    else if ( lat_degrees <= -89.999f ) { return glm::vec3( 0,-1,0 ); } // SouthPole
    const f32 lon = lon_degrees * Math::DEG2RAD;
    const f32 lat = lat_degrees * Math::DEG2RAD;
    const f32 c_lat = ::cosf( lat );
    const f32 s_lat = ::sinf( lat );
    const f32 c_lon = ::cosf( lon );
    const f32 s_lon = ::sinf( lon );
    const f32 x = c_lat * c_lon;
    const f32 y = s_lat;
    const f32 z = c_lat * s_lon;
    return glm::vec3( x,y,z );
}

void
SMeshSphere::add( SMeshBuffer & o, const glm::vec3 & siz, u32 color,
                  u32 tessLon, u32 tessLat, const glm::vec3 & offset )
{
    o.setPrimitiveType( gpu::PrimitiveType::Triangles );
    if ( tessLat < 3 ) tessLat = 3;
    if ( tessLon < 3 ) tessLon = 3;

    glm::vec3 radius = siz * 0.5f;

    // u-step
    f32 lonStep = 360.0f / f32( tessLon );
    f32 lonStart = 0.0f; // Greenwich
    //f32 lonEnd = 360.0f;
    f32 lonUV = 1.0f / f32( tessLon );

    // v-step
    f32 latStep = -180.0f / f32( tessLat );
    f32 latStart = 90.0f;
    //f32 latEnd = -90.0f;
    f32 latUV = 1.0f / f32( tessLat );

    // Predict and reserve memory, indexed quads.
    o.moreVertices( 4 * tessLon * tessLat );
    o.moreIndices( 6 * tessLon * tessLat );

    auto addQuad = [&] ( int32_t ix, int32_t iy )
    {
        //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
        f32 lon1 = lonStart + lonStep * ix;
        f32 lon2 = lonStart + lonStep * ( ix + 1 );
        f32 lat1 = latStart + latStep * iy;
        f32 lat2 = latStart + latStep * ( iy + 1 );
        glm::vec3 na = computeNormal( lon1, lat2 );
        glm::vec3 nb = computeNormal( lon1, lat1 );
        glm::vec3 nc = computeNormal( lon2, lat1 );
        glm::vec3 nd = computeNormal( lon2, lat2 );
        glm::vec3 a = na * radius + offset;
        glm::vec3 b = nb * radius + offset;
        glm::vec3 c = nc * radius + offset;
        glm::vec3 d = nd * radius + offset;
        u32 colorA = color;
        u32 colorB = color;
        u32 colorC = color;
        u32 colorD = color;
        S3DVertex A( a.x,a.y,a.z, na.x,na.y,na.z, colorA, lonUV * ix, latUV * (iy+1) );
        S3DVertex B( b.x,b.y,b.z, nb.x,nb.y,nb.z, colorB, lonUV * ix, latUV * (iy) );
        S3DVertex C( c.x,c.y,c.z, nc.x,nc.y,nc.z, colorC, lonUV * (ix+1), latUV * (iy) );
        S3DVertex D( d.x,d.y,d.z, nd.x,nd.y,nd.z, colorD, lonUV * (ix+1), latUV * (iy+1) );
        o.addQuad( A,B,C,D );
    };

    for ( size_t j = 0; j < tessLat; ++j )
    {
        for ( size_t i = 0; i < tessLon; ++i )
        {
            addQuad( i,j );
        }
    }
}

void
addSphere( SMeshBuffer & o, const glm::vec3 & siz, const glm::vec3 & offset,
           u32 tessLon, u32 tessLat, u32 color )
{
    SMeshSphere::add( o, siz, color, tessLon, tessLat, offset );
}

// =========================================================================
// UpperHalfSphere
// =========================================================================

void SMeshUpperHalfSphere::add( SMeshBuffer & o, const V3 & siz,
        u32 color, u32 tessLon, u32 tessLat, const V3 & offset )
{
    o.setPrimitiveType( gpu::PrimitiveType::Triangles );
    if ( tessLat < 3 ) tessLat = 3;
    if ( tessLon < 3 ) tessLon = 3;

    // x-step
    f32 lonStep = 360.0f / f32( tessLon );
    f32 lonStart = 0.0f; // Greenwich
    //f32 lonEnd = 360.0f;

    // y-step
    f32 latStep = -180.0f / f32( tessLat );
    f32 latStart = 90.0f;
    //f32 latEnd = -90.0f;

    // tex-coord-steps
    f32 du = 1.0f / f32( tessLon );
    f32 dv = 1.0f / f32( tessLat );

    // Predict and reserve memory, indexed quads.
    o.moreVertices( 4 * tessLon * tessLat );
    o.moreIndices( 6 * tessLon * tessLat );

    auto addQuad = [&] ( int32_t ix, int32_t iy )
    {
        //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
        glm::vec3 radius = siz * 0.5f;
        f32 lon1 = lonStart + lonStep * ix;
        f32 lon2 = lonStart + lonStep * ( ix + 1 );
        f32 lat1 = latStart + latStep * iy;
        f32 lat2 = latStart + latStep * ( iy + 1 );
        glm::vec3 na = SMeshSphere::computeNormal( lon1, lat2 );
        glm::vec3 nb = SMeshSphere::computeNormal( lon1, lat1 );
        glm::vec3 nc = SMeshSphere::computeNormal( lon2, lat1 );
        glm::vec3 nd = SMeshSphere::computeNormal( lon2, lat2 );
        glm::vec3 a = na * radius + offset;
        glm::vec3 b = nb * radius + offset;
        glm::vec3 c = nc * radius + offset;
        glm::vec3 d = nd * radius + offset;
        u32 colorA = color;
        u32 colorB = color;
        u32 colorC = color;
        u32 colorD = color;
        S3DVertex A( a.x,a.y,a.z, na.x,na.y,na.z, colorA, du * ix, dv * (iy+1) );
        S3DVertex B( b.x,b.y,b.z, nb.x,nb.y,nb.z, colorB, du * ix, dv * (iy) );
        S3DVertex C( c.x,c.y,c.z, nc.x,nc.y,nc.z, colorC, du * (ix+1), dv * (iy) );
        S3DVertex D( d.x,d.y,d.z, nd.x,nd.y,nd.z, colorD, du * (ix+1), dv * (iy+1) );
        o.addQuad( B,C,D,A );
    };

    for ( size_t j = 0; j < tessLat/2+1; ++j )
    {
        for ( size_t i = 0; i < tessLon; ++i )
        {
            addQuad( i,j );
        }
    }
}

// =========================================================================
SMeshBuffer
SMeshHexagonHull::create( f32 w, f32 h, f32 height, u32 color )
{
    SMeshBuffer o( gpu::PrimitiveType::Triangles );

    auto addLeftStripXZ = [&] ( std::vector< glm::vec3 > const & points, f32 height, u32 color )
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

    auto corners = SMeshHexagon::getCorners(w,h);
    std::vector< glm::vec3 > points;
    points.reserve( corners.size() );
    for ( auto const & p : corners )
    {
        points.emplace_back( p.x, 0.0f, p.y );
    }

    addLeftStripXZ( points, height, color );
    o.getMaterial().setCulling( false );
    o.getMaterial().setBlend( gpu::Blend::alphaBlend() );
    return o;
}

} // end namespace gpu.
} // end namespace de.


// =========================================================================
// Circle
// =========================================================================
/*
void
addCircle( SMeshBuffer & o, Recti const & pos,
      u32 outerColor, u32 innerColor, TexRef const & ref, u32 tess )
{
   f32 rx = 0.5f * pos.w;
   f32 ry = 0.5f * pos.h;

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   // Add center point
   u32 v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

   const f32 angleStep = f32( ( 2.0 * M_PI ) / double( tess ) );
//      const f32 outwardX = f32( radius.x ) / f32( tessOutwards );
//      const f32 outwardY = f32( radius.y ) / f32( tessOutwards );

   for ( u32 i = 0; i <= tess; ++i )
   {
      const f32 phi = angleStep * f32( i );
      const f32 s = ::sinf( phi );
      const f32 c = ::cosf( phi );
//   for ( u32 j = 0; j <= tessOutwards; ++j )
//   {
      f32 x = rx * c; // * f32( j ) * outwardX);
      f32 y = ry * s; // * f32( j ) * outwardY);
      f32 u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      f32 v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
   }

   for ( u32 i = 0; i < tess; ++i )
   {
//   for ( u32 j = 0; j < tessOutwards; ++j )
//   {
      u32 M = v;
      u32 A = v + 1 + i;
      u32 B = v + 2 + i;
      o.addIndexedTriangle( M, A, B );
//   }
   }
}

void addCircleOutline( SMeshBuffer & o, Recti const & pos,
      u32 color, TexRef const & ref, int borderWidth, u32 tess )
{
   f32 rx = 0.5f * pos.w;
   f32 ry = 0.5f * pos.h;

   std::vector< glm::vec2 > points;
   const f32 angleStep = f32( ( 2.0 * M_PI ) / double( tess ) );
   for ( u32 i = 0; i <= tess; ++i )
   {
      const f32 phi = angleStep * f32( i );
      const f32 x = rx * ::cosf( phi );
      const f32 y = ry * ::sinf( phi );
      points.emplace_back( x, y );
   }
   points.emplace_back( points.front() );

   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::Square, LineJoin::Bevel );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

}

// =========================================================================
struct SMeshUpperHalfSphere
// =========================================================================
{
   // @brief Benni book example 2021
   typedef glm::vec3 V3;
   typedef u32 u32;

   static void
   add( SMeshBuffer & o, V3 siz, V3 pos, u32 tessCircle = 15,  u32 tessRows = 15, u32 color = 0xAFFFFFFF )
   {
      u32 v = o.getVertexCount();

      // TopCenter vertex
      o.addVertex( S3DVertex( 0,ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[v+1 to v+tessCircle*tessRows ] = other vertices
      const f32 sxz = 1.f / (f32)tessCircle;
      const f32 sy = 1.f / (f32)tessRows;

      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, f32(0.0), f32( 0.5*M_PI ) );

      for (u32 j=0; j<tessRows; ++j)
      {
         for (u32 i=0; i<tessCircle; ++i)
         {
            const f32 sinA = sincosA[i].s;
            const f32 cosA = sincosA[i].c;
            const f32 sinB = sincosB[j].s;
            const f32 cosB = sincosB[j].c;
            const f32 nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 ny =  sinB;					// equals the normal at point radius*(x,y,z)
            const f32 nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 u = 0.5f-0.5f*cosB * sinA; // not finished
            const f32 v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( u32 j = 0; j < tessRows; ++j )
      {
         for ( u32 i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               u32 i0 = v + 1 + (j)*tessCircle + (i);    // A
               u32 i1 = v + 1 + (j)*tessCircle + (i+1);  // B
               u32 i2 = v;    // C - top center
               if ( i == tessCircle - 1 )
               {
                  i0 = v + 1 + (j)*tessCircle + (i);	// A
                  i1 = v + 1 + (j)*tessCircle + (0);	// B
               }
               o.addIndexedTriangle( i0, i1, i2 ); // ABC
            }

            // bottom to top-1 rows consist of quad segments (each of 2 triangles)
            else
            {
               u32 i0 = v + 1 + (j)*tessCircle + (i);	// A
               u32 i1 = v + 1 + (j+1)*tessCircle + (i);	// B
               u32 i2 = v + 1 + (j+1)*tessCircle + (i+1);// C
               u32 i3 = v + 1 + (j)*tessCircle + (i+1);	// D
               if ( i == tessCircle - 1 )
               {
                  i0 = v + 1 + (j)*tessCircle + (i);	// A
                  i1 = v + 1 + (j+1)*tessCircle + (i);	// B
                  i2 = v + 1 + (j+1)*tessCircle + (0);	// C
                  i3 = v + 1 + (j)*tessCircle + (0);	// D
               }
               o.addIndexedQuad( i0, i1, i2, i3 );
            }
         }
      }
   }
};

// =========================================================================
struct SUpperHalfSphere
// =========================================================================
{
   // @brief Benni book example 2021

   static void
   add(
      SMeshBuffer & o,
      glm::vec3 siz,
      glm::vec3 pos,
      u32 tessCircle = 15,
      u32 tessRows = 15,
      u32 color = 0xAFFFFFFF )
   {
      u32 v = o.getVertexCount();

      // TopCenter vertex
      o.addVertex( S3DVertex( 0,ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[v+1 to v+tessCircle*tessRows ] = other vertices
      const f32 sxz = 1.f / (f32)tessCircle;
      const f32 sy = 1.f / (f32)tessRows;

      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, f32(0.0), f32( 0.5*M_PI ) );

      for (u32 j=0; j<tessRows; ++j)
      {
         for (u32 i=0; i<tessCircle; ++i)
         {
            const f32 sinA = sincosA[i].s;
            const f32 cosA = sincosA[i].c;
            const f32 sinB = sincosB[j].s;
            const f32 cosB = sincosB[j].c;
            const f32 nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 ny =  sinB;					// equals the normal at point radius*(x,y,z)
            const f32 nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 u = 0.5f-0.5f*cosB * sinA; // not finished
            const f32 v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( u32 j = 0; j < tessRows; ++j )
      {
         for ( u32 i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               u32 i0 = v + 1 + (j)*tessCircle + (i);    // A
               u32 i1 = v + 1 + (j)*tessCircle + (i+1);  // B
               u32 i2 = v;    // C - top center
               if ( i == tessCircle - 1 )
               {
                  i0 = v + 1 + (j)*tessCircle + (i);	// A
                  i1 = v + 1 + (j)*tessCircle + (0);	// B
               }
               o.addIndexedTriangle( i0, i1, i2 ); // ABC
            }

            // bottom to top-1 rows consist of quad segments (each of 2 triangles)
            else
            {
               u32 i0 = v + 1 + (j)*tessCircle + (i);	// A
               u32 i1 = v + 1 + (j+1)*tessCircle + (i);	// B
               u32 i2 = v + 1 + (j+1)*tessCircle + (i+1);// C
               u32 i3 = v + 1 + (j)*tessCircle + (i+1);	// D
               if ( i == tessCircle - 1 )
               {
                  i0 = v + 1 + (j)*tessCircle + (i);	// A
                  i1 = v + 1 + (j+1)*tessCircle + (i);	// B
                  i2 = v + 1 + (j+1)*tessCircle + (0);	// C
                  i3 = v + 1 + (j)*tessCircle + (0);	// D
               }
               o.addIndexedQuad( i0, i1, i2, i3 );
            }
         }
      }
   }
};

// =========================================================================
struct SLowerHalfSphere
// =========================================================================
{
   // @brief Benni book example 2021
   static void
   add( SMeshBuffer & o,
         f32 rx = 50, f32 ry = 50, f32 rz = 50,
         u32 tessCircle = 15, u32 tessRows = 15,
         f32 x = 0.f, f32 y = 0.f, f32 z = 0.f,
         u32 color = 0xAFFFFFFF )
   {
      const u32 vStart = o.getVertexCount();

      // Vertices[vStart] = top center vertex
      o.addVertex( S3DVertex( 0,-ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[vStart+1 to vStart+tessCircle*tessRows ] = other vertices
      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, f32(0.0), f32( 0.5*M_PI ) );

      for (u32 j=0; j<tessRows; ++j)
      {
         for (u32 i=0; i<tessCircle; ++i)
         {
            const f32 sinA = sincosA[i].s;
            const f32 cosA = sincosA[i].c;
            const f32 sinB = sincosB[j].s;
            const f32 cosB = sincosB[j].c;
            const f32 nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 ny = -sinB;					// equals the normal at point radius*(x,y,z)
            const f32 nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            const f32 u = 0.5f-0.5f*cosB * sinA; // not finished
            const f32 v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( u32 j = 0; j < tessRows; ++j )
      {
         for ( u32 i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               u32 i0 = vStart + 1 + (j)*tessCircle + (i);    // A
               u32 i1 = vStart + 1 + (j)*tessCircle + (i+1);  // B
               u32 i2 = vStart;    // C - top center
               if ( i == tessCircle - 1 )
               {
                  i0 = vStart + 1 + (j)*tessCircle + (i);	// A
                  i1 = vStart + 1 + (j)*tessCircle + (0);	// B
               }
               o.addIndexedTriangle( i0, i1, i2 );
            }

            // bottom to top-1 rows consist of quad segments (each of 2 triangles)
            else
            {
               u32 i0 = vStart + 1 + (j)*tessCircle + (i);	// A
               u32 i1 = vStart + 1 + (j+1)*tessCircle + (i);	// B
               u32 i2 = vStart + 1 + (j+1)*tessCircle + (i+1);// C
               u32 i3 = vStart + 1 + (j)*tessCircle + (i+1);	// D
               if ( i == tessCircle - 1 )
               {
                  i0 = vStart + 1 + (j)*tessCircle + (i);	// A
                  i1 = vStart + 1 + (j+1)*tessCircle + (i);	// B
                  i2 = vStart + 1 + (j+1)*tessCircle + (0);	// C
                  i3 = vStart + 1 + (j)*tessCircle + (0);	// D
               }
               o.addIndexedQuad( i0, i1, i2, i3 );
            }
         }
      }
   }
};


static f3232_t
getAngleABC( glm::vec2 const & A, glm::vec2 const & B, glm::vec2 const & C )
{
auto const ABx = f3264_t( B.x ) - f3264_t( A.x );
auto const ABy = f3264_t( B.y ) - f3264_t( A.y );
auto const BCx = f3264_t( C.x ) - f3264_t( B.x );
auto const BCy = f3264_t( C.y ) - f3264_t( B.y );
auto const phiAB = ::atan2( ABy, ABx );
auto const phiBC = ::atan2( BCy, BCx );
auto const phiABC = static_cast< f3232_t >( phiBC - phiAB );
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
u32 color = 0xFFFFFFFF,
f32 lineWidth = 1.0f,
LineCap::ELineCap lineCap = LineCap::None,
LineJoin::ELineJoin lineJoin = LineJoin::None )
{
//      u32 ghost = 0x80FFFFFF;
//      u32 black = 0xFF000000;
//      u32 white = 0xFFFFFFFF;
//      u32 red = 0xFFFF0000;
//      u32 green = 0xFF00FF00;
//      u32 blue = 0xFF0000FF;
//      u32 yellow = 0xFFFFFF00;
//      u32 orange = 0xFFFF8000;

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


auto addVertex = [ & ] ( glm::vec2 const & pos, glm::vec2 const & nrm, u32 color, f32 u, f32 v )
{
f32 x = (nrm.x * lineWidth) + pos.x;
f32 y = (nrm.y * lineWidth) + pos.y;
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
auto addLineCapStart = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, u32 color, LineCap::ELineCap lineCap )
{
DE_DEBUG( "StartLineCap A(",A,"), B(",B,"), color(",color,")")
auto v = o.getVertexCount();
auto dAB = glm::normalize( B - A );
auto nAB = getOrtho2D( dAB );      // Compute normal nAB
u32 colorSL = varyColor( color, 30 );
u32 colorSR = varyColor( color, 30 );
u32 colorAL = color;
u32 colorAR = varyColor( color, 30 );

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
auto addLineCapEnd = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, u32 color, LineCap::ELineCap lineCap )
{
DE_DEBUG( "EndLineCap A(",A,"), B(",B,"), color(",color,")" )
auto v = o.getVertexCount();
auto dAB = glm::normalize( B - A );
auto nAB = getOrtho2D( dAB );      // Compute normal nAB
u32 colorBL = varyColor( color, 30 );
u32 colorBR = varyColor( color, 30 );
u32 colorEL = color;
u32 colorER = varyColor( color, 30 );
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
auto addSegment = [ & ] ( glm::vec2 const & A, glm::vec2 const & B, u32 color )
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
//u32 baseColor = green;
glm::vec2 A,B,C; // We also need normals for both line-segments AB and BC
A = points[ 0 ];    // Fetch A
B = points[ 1 ];    // Fetch B
addLineCapStart( A,B, 0xFFAA3388, lineCap );
addSegment( A,B, color );
for ( size_t i = 2; i < n; ++i )
{
C = points[ i ]; // Fetch C
// Add Join ABC
f3232_t phi = getAngleABC( A,B,C ); // Compute angle between AB and BC just for debug coloring
u32 color = 0xFF00FFFF;
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
u32 color = 0xFFFFFFFF, int lineWidth = 1 )
{
if ( points.size() < 2 ) { return; }

f32 radius = (0.5f * lineWidth);

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
u32 color = 0xFFFFFFFF, int lineWidth = 1 )
{
if ( points.size() < 2 )
{
return;
}

f32 radius = (0.5f * lineWidth);

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

