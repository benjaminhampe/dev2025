#include <de/gpu/smesh/SMeshLibrary.h>

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

// static
void
SMeshHexagon::add( int i_min, int i_max, int j_min, int j_max,
    SMeshBuffer & mb, float w, float h, bool forceIndices, glm::vec3 const & pos )
{
    for ( int j = j_min; j < j_max; ++j )
    {
        for ( int i = i_min; i < i_max; ++i )
        {
            add( i,j, mb, w, h, 0xFFFFFFFF, forceIndices, pos );
        }
    }
}

// static
void
SMeshHexagon::add( int i, int j, SMeshBuffer & mb, float w, float h, uint32_t color,
    bool forceIndices, glm::vec3 const & pos )
{
    float x = computeBoardPosX(w,h,i,j);
    float z = computeBoardPosY(w,h,i,j);
    add( mb, w, h, color, forceIndices, pos + V3(x,0,z) );
}

// static
void
SMeshHexagon::addM4( SMeshBuffer & mb, float w, float h, uint32_t color,
                    bool forceIndices, glm::dmat4 const & trs )
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

// static
void
SMeshHexagon::add( SMeshBuffer & mb, float w, float h, uint32_t color,
                    bool forceIndices, glm::vec3 const & pos )
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

// static
glm::ivec2
SMeshHexagon::getNeighborTileIndex( int corner, int i, int j )
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

// static
glm::vec2
SMeshHexagon::getNeighborTilePos( int corner, float w, float h, int i, int j )
{
    glm::ivec2 n = getNeighborTileIndex( corner, i, j );
    return glm::vec2( computeBoardPosX( w,h,n.x,n.y),
                     computeBoardPosY( w,h,n.x,n.y) );
}

// static
int32_t
SMeshHexagon::safeMod( int32_t a, int32_t b )
{
    if ( a == 0 || b == 0 ) { return 0; }
    return std::abs( a ) % std::abs( b );
}

// static
float
SMeshHexagon::computeBoardPosX( float w, float h, int i, int j )
{
    //float x = 0.5f * float( safeMod( j, 2 ) == 1 ) + float( i );
    //return x * w;
    //return (0.5f * j + float( i )) * w;
    float x = 0.5f * float( safeMod( j, 2 ) == 1 ) + float( i );
    return x * w;
}

// static
float
SMeshHexagon::computeBoardPosY( float w, float h, int i, int j )
{
    //float y = 3.0f/4.0f * float( j );
    //return y * h;
    return h * 3.0f/4.0f * float( j );
}

// static
glm::ivec2
SMeshHexagon::computeTileIndex( float x, float y, float w, float h )
{
    int j = int( y / h );
    int i = int( x/w - (0.5f * j) );
    return glm::ivec2(i,j);
}

// static
glm::vec2
SMeshHexagon::lineCenter( glm::vec2 const & a, glm::vec2 const & b ) { return a + ( b - a ) * 0.5f; }

// static
glm::vec2 SMeshHexagon::cornerA( float w, float h ) { return glm::vec2( 0.0f,    -.5f*h ); }
// static
glm::vec2 SMeshHexagon::cornerB( float w, float h ) { return glm::vec2( -0.5f*w, -.25f*h ); }
// static
glm::vec2 SMeshHexagon::cornerC( float w, float h ) { return glm::vec2( -0.5f*w, .25f*h ); }
// static
glm::vec2 SMeshHexagon::cornerD( float w, float h ) { return glm::vec2( 0.0f,     .5f*h ); }
// static
glm::vec2 SMeshHexagon::cornerE( float w, float h ) { return glm::vec2( 0.5f*w,  .25f*h ); }
// static
glm::vec2 SMeshHexagon::cornerF( float w, float h ) { return glm::vec2( 0.5f*w, -.25f*h ); }

// static
glm::vec2 SMeshHexagon::edgeAB( float w, float h ) { return lineCenter( cornerA(w,h), cornerB(w,h)); }
// static
glm::vec2 SMeshHexagon::edgeBC( float w, float h ) { return lineCenter( cornerB(w,h), cornerC(w,h)); }
// static
glm::vec2 SMeshHexagon::edgeCD( float w, float h ) { return lineCenter( cornerC(w,h), cornerD(w,h)); }
// static
glm::vec2 SMeshHexagon::edgeDE( float w, float h ) { return lineCenter( cornerD(w,h), cornerE(w,h)); }
// static
glm::vec2 SMeshHexagon::edgeEF( float w, float h ) { return lineCenter( cornerE(w,h), cornerF(w,h)); }
// static
glm::vec2 SMeshHexagon::edgeFA( float w, float h ) { return lineCenter( cornerF(w,h), cornerA(w,h)); }

// static
glm::vec2
SMeshHexagon::getCorner( int i, float w, float h )
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

// static
std::vector< glm::vec2 >
SMeshHexagon::getCorners( float w, float h )
{
    return std::vector< glm::vec2 > {
                                  cornerA( w,h ), cornerB( w,h ), cornerC( w,h ),
                                  cornerD( w,h ), cornerE( w,h ), cornerF( w,h ) };
}

// static
std::vector< glm::vec3 >
SMeshHexagon::getCornersXZ( float w, float h )
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

// static
glm::vec2
SMeshHexagon::getEdge( int i, float w, float h )
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

// static
float
SMeshHexagon::computeAngle( glm::vec2 const & v )
{
    float phi = atan2( v.y, v.x ) * Math::RAD2DEG;
    //DE_DEBUG("atan2( ",v," ) = ", phi)
    if ( phi < 0.0f ) phi += 360.0f;
    return phi;
}

// static
float
SMeshHexagon::computeCornerAngle( int i, float w, float h )
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

// static
float
SMeshHexagon::computeEdgeAngle( int i, float w, float h )
{
    switch ( i ) // -153.435f;
    {
    case 0: return 26.5651f;
    case 1: return 90.0f;
    case 2: return 180.0f - 26.5651f;
    case 3: return 26.5651f;
    case 4: return 90.0f;
    case 5: return 180.0f - 26.5651f;
    default: throw std::runtime_error("Invalid i in computeEdgeAngle()");
    }
}

/*
// static
float
SMeshHexagon::computeEdgeAngle( int i, float w, float h )
{
    switch ( i )
    {
    case 0: return 90.0f - 26.5651f;
    case 1: return 90.0f;
    case 2: return 90.0f - 153.435f;
    case 3: return 90.0f - 26.565f;
    case 4: return 90.0f;
    case 5: return 90.0f - 153.435f;
    default: throw std::runtime_error("Invalid i in computeEdgeAngle()");
    }
}

// static
float
SMeshHexagon::computeEdgeAngle( int i, float w, float h )
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

// static
void
SMeshHexagon::test( float w, float h )
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

// static
SMeshBuffer*
SMeshHexagon::create( float w, float h, uint32_t color )
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

// static
void
SMeshHexagon::add( SMeshBuffer & out, int i, int j, float w, float h, uint32_t color )
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
// =========================================================================
void SMeshBox::add( SMeshBuffer & o,
                   glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                   glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h,
                   uint32_t color )
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
/*
    { // Front quad ABCD + Back quad HGFE
        S3DVertex A( a.x, a.y, a.z, 0,0,-1, color, 0.0,1.0 );
        S3DVertex B( b.x, b.y, b.z, 0,0,-1, color, 0.0,0.0 );
        S3DVertex C( c.x, c.y, c.z, 0,0,-1, color, 1.0,0.0 );
        S3DVertex D( d.x, d.y, d.z, 0,0,-1, color, 1.0,1.0 );
        o.addQuad( A,B,C,D );
        S3DVertex E( e.x, e.y, e.z, 0,0,1, color, 0.0,1.0 );
        S3DVertex F( f.x, f.y, f.z, 0,0,1, color, 0.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,0,1, color, 1.0,0.0 );
        S3DVertex H( h.x, h.y, h.z, 0,0,1, color, 1.0,1.0 );
        o.addQuad( H,G,F,E );
    }

    { // Wall R+L quad DCGH + EFBA
        S3DVertex D( d.x, d.y, d.z, 0,0,1, color, 0.0,1.0 );
        S3DVertex C( c.x, c.y, c.z, 0,0,1, color, 0.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,0,1, color, 1.0,0.0 );
        S3DVertex H( h.x, h.y, h.z, 0,0,1, color, 1.0,1.0 );
        o.addQuad( D,C,G,H );
        S3DVertex E( e.x, e.y, e.z, 0,0,-1, color, 0.0,1.0 );
        S3DVertex F( f.x, f.y, f.z, 0,0,-1, color, 0.0,0.0 );
        S3DVertex B( b.x, b.y, b.z, 0,0,-1, color, 1.0,0.0 );
        S3DVertex A( a.x, a.y, a.z, 0,0,-1, color, 1.0,1.0 );
        o.addQuad( E,F,B,A );
    }

    { // Top quad BFGC + Bottom quad AEHD
        S3DVertex B( b.x, b.y, b.z, 0,1,0, color, 0.0,1.0 );
        S3DVertex F( f.x, f.y, f.z, 0,1,0, color, 0.0,0.0 );
        S3DVertex G( g.x, g.y, g.z, 0,1,0, color, 1.0,0.0 );
        S3DVertex C( c.x, c.y, c.z, 0,1,0, color, 1.0,1.0 );
        o.addQuad( B,F,G,C );
        S3DVertex A( a.x, a.y, a.z, 0,-1,0, color, 0.0,1.0 );
        S3DVertex E( e.x, e.y, e.z, 0,-1,0, color, 0.0,0.0 );
        S3DVertex H( h.x, h.y, h.z, 0,-1,0, color, 1.0,0.0 );
        S3DVertex D( d.x, d.y, d.z, 0,-1,0, color, 1.0,1.0 );
        o.addQuad( A,E,H,D );
    }
*/
}

void
SMeshBox::add( SMeshBuffer & o, glm::vec3 cubeSize, glm::vec3 pos, uint32_t color )
{
    float dx = cubeSize.x * 0.5f;
    float dy = cubeSize.y * 0.5f;
    float dz = cubeSize.z * 0.5f;
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
      uint32_t color )
{
    o.vertices.push_back(S3DVertex( a.x, a.y, a.z, 0,0,1, color, 0,1 )); // = 0
    o.vertices.push_back(S3DVertex( b.x, b.y, b.z, 0,0,1, color, 0,0 )); // = 1
    o.vertices.push_back(S3DVertex( c.x, c.y, c.z, 0,0,1, color, 1,0 )); // = 2
    o.vertices.push_back(S3DVertex( d.x, d.y, d.z, 0,0,1, color, 1,1 )); // = 3
    o.vertices.push_back(S3DVertex( e.x, e.y, e.z, 0,0,1, color, 0,1 )); // = 4
    o.vertices.push_back(S3DVertex( f.x, f.y, f.z, 0,0,1, color, 0,0 )); // = 5
    o.vertices.push_back(S3DVertex( g.x, g.y, g.z, 0,0,1, color, 1,0 )); // = 6
    o.vertices.push_back(S3DVertex( h.x, h.y, h.z, 0,0,1, color, 1,1 )); // = 7

    o.indices.push_back( 0 );
    o.indices.push_back( 1 );
    o.indices.push_back( 1 );
    o.indices.push_back( 2 );
    o.indices.push_back( 2 );
    o.indices.push_back( 3 );
    o.indices.push_back( 3 );
    o.indices.push_back( 0 );

    o.indices.push_back( 4 );
    o.indices.push_back( 5 );
    o.indices.push_back( 5 );
    o.indices.push_back( 6 );
    o.indices.push_back( 6 );
    o.indices.push_back( 7 );
    o.indices.push_back( 7 );
    o.indices.push_back( 4 );

    o.indices.push_back( 0 );
    o.indices.push_back( 4 );
    o.indices.push_back( 1 );
    o.indices.push_back( 5 );
    o.indices.push_back( 2 );
    o.indices.push_back( 6 );
    o.indices.push_back( 3 );
    o.indices.push_back( 7 );
}

void
SMeshBox::addLines( SMeshBuffer & o, glm::vec3 cubeSize, glm::vec3 pos, uint32_t color )
{
   float dx = cubeSize.x * 0.5f;
   float dy = cubeSize.y * 0.5f;
   float dz = cubeSize.z * 0.5f;
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
   float rx, float h, float rz,
   uint32_t innerColor,
   uint32_t outerColor,
   int32_t tessCircle,
   int32_t tessHeight,
   FN_GET_VERTEXCOUNT const & getVertexCount,
   FN_ADD_VERTEX const & addVertex,
   FN_ADD_INDEX const & addIndex
)
{
   std::vector< glm::vec3 > positions;
   positions.reserve( tessCircle + 1 );
   positions.emplace_back( 0.0f, h, 0.0f );

   std::vector< glm::vec2 > texcoords;
   texcoords.reserve( tessCircle + 1 );
   texcoords.emplace_back( 0.5f, 0.5f );

   float const angleStep = float( ( 2.0 * M_PI ) / double( tessCircle ) );
   for ( int32_t i = 0; i < tessCircle; ++i )
   {
      float const phi = angleStep * float( i );
      float const c = ::cosf( phi );
      float const s = ::sinf( phi );
      positions.emplace_back( rx*c, 0.0f, rz*s );
      texcoords.emplace_back( 0.5f*c+0.5f, 0.5f*s+0.5f );
   }

   auto addTriangle = [&] ( uint32_t iA, uint32_t iB )
   {
      const uint32_t v = getVertexCount();
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
         uint32_t A = 1 + i;
         uint32_t B = 1;
         addTriangle( A, B );
      }
      else
      {
         uint32_t A = 1 + i;
         uint32_t B = 2 + i;
         addTriangle( A, B );
      }
   }

}

// =========================================================================
void SMeshCone::addXZ( SMeshBuffer & o, glm::vec3 const & siz,
      uint32_t iColor, uint32_t oColor, int32_t tessCircle, int32_t tessHeight )
{
   float rx = siz.x * 0.5f;
   float h = siz.y * 0.5f;
   float rz = siz.z * 0.5f;
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addIndex = [&] ( uint32_t index ) { o.addIndex( index ); };
   auto addVertex = [&] ( float px, float py, float pz,
                          float nx, float ny, float nz,
                          uint32_t color, float u, float v )
   {
      o.addVertex( S3DVertex( px,py,pz, nx,ny,nz, color, u,v ) );
   };
   Generic3DCone::addTrianglesXZ( rx,h,rz, iColor,oColor,
      tessCircle, tessHeight, getVertexCount, addVertex, addIndex );
}


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

void
SMeshRect::addLine2D( SMeshBuffer & o, int ax, int ay, int bx, int by,
                      uint32_t colorA, uint32_t colorB )
{
   o.setPrimitiveType( PrimitiveType::Lines );
   o.vertices.reserve( 2 );
   o.vertices.push_back( S3DVertex( ax, ay, -1, 0,0,0, colorA, 0,0 ) ); // A
   o.vertices.push_back( S3DVertex( bx, by, -1, 0,0,0, colorB, 0,0 ) ); // B
}

void
SMeshRect::add( SMeshBuffer & o, V3 const & a, V3 const & b, V3 const & c, V3 const & d,
                uint32_t color, float u1, float v1, float u2, float v2 )
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
SMeshRect::add( SMeshBuffer & o, Recti const & pos,
     uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref )
{
    const int w = pos.w; // pos.w() > 0 ? pos.w() : ref.w();
    const int h = pos.h; // pos.h() > 0 ? pos.h() : ref.h();
    if ( w < 1 || w > 10000 ) { return; }
    if ( h < 1 || h > 10000 ) { return; }

    o.setPrimitiveType( PrimitiveType::Triangles );
    o.setBlend( Blend::alphaBlend() );
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
SMeshRect::addOutline( SMeshBuffer & o, Recti const & pos, uint32_t color,
                        TexRef const & ref, int borderWidth )
{
    int w = pos.w; //pos.w() > 0 ? pos.w() : ref.w();
    int h = pos.h; //pos.h() > 0 ? pos.h() : ref.h();
    if ( w < 1 || w > 10000 ) { return; }
    if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::TriangleStrip );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( Blend::alphaBlend() );

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
SMeshRect::addOutline( SMeshBuffer & o, Recti const & pos, uint32_t colorA, uint32_t colorB,
            uint32_t colorC, uint32_t colorD, TexRef const & ref, int borderWidth )
{
    int w = pos.w; //pos.w() > 0 ? pos.w() : ref.w();
   int h = pos.h; //pos.h() > 0 ? pos.h() : ref.h();
   if ( w < 1 || w > 10000 ) { return; }
   if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::Lines );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( Blend::alphaBlend() );

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
void
Generic2DTriangleRoundRect::addTriangles(
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
   addVertex( x2, y3, outerColor ); // K=A
   addVertex( x1, y3, outerColor ); // L=B

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


// =========================================================================
void
SMeshRoundRect::addXY( SMeshBuffer & o,
      glm::vec3 const & pos,
      float w, float h, float rx, float ry,
      int32_t tess, uint32_t innerColor, uint32_t outerColor )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addVertex = [&] ( float x, float y, uint32_t color )
   {
      float px = pos.x + x;
      float py = pos.y + y;
      float pz = pos.z;
      float u = 0.5f + x / w;
      float v = 0.5f + y / h;
      o.addVertex( S3DVertex( px,py,pz, 0,0,-1, color, u,v ) );
   };

   auto addIndex = [&] ( uint32_t index ) { o.addIndex( index ); };

   Generic2DTriangleRoundRect::addTriangles( w, h, rx, ry, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );

   o.recalculateBoundingBox();
}

void
SMeshRoundRect::addXZ( SMeshBuffer & o,
      glm::vec3 const & pos,
      float w, float h, float rx, float ry,
      int32_t tess, uint32_t innerColor, uint32_t outerColor )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addVertex = [&] ( float x, float y, uint32_t color )
   {
      float px = pos.x + x;
      float py = pos.y;
      float pz = pos.z + y;
      float u = 0.5f + x / w;
      float v = 0.5f - y / h;
      o.addVertex( S3DVertex( px,py,pz, 0,0,-1, color, u,v ) );
   };

   auto addIndex = [&] ( uint32_t index ) { o.addIndex( index ); };

   Generic2DTriangleRoundRect::addTriangles( w, h, rx, ry, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );

   o.recalculateBoundingBox();
}

void
addRoundRect( SMeshBuffer & o,
      Recti const & pos,
      float zIndex,
      glm::ivec2 const & radius,
      uint32_t outerColor,
      uint32_t innerColor,
      TexRef const & ref, int tess )
{
   int w = pos.w;
   int h = pos.h;
   if ( w < 3 || w > 10000 ) { return; }
   if ( h < 3 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setCulling( false );
   o.setBlend( Blend::alphaBlend() );
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
addRoundRectOutline( SMeshBuffer & o, Recti const & pos, float zIndex,
                     glm::ivec2 const & radius,
                     uint32_t color, TexRef const & ref, int borderWidth, uint32_t tess )
{
   int w = pos.w;
   int h = pos.h;
   if ( w < 4 || w > 1000000 ) { return; }
   if ( h < 4 || h > 1000000 ) { return; }
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   glm::vec2 off( w/2 + 1 + pos.x, h/2 + pos.y );
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
   float uMax = o.vertices.back().tex.x;
   for ( size_t i = 0; i < o.vertices.size(); ++i )
   {
      o.vertices[ i ].pos.z = zIndex;
      float u = o.vertices[ i ].tex.x;
      uint8_t r = static_cast<uint8_t>( std::clamp( int(std::lround( 255.0f * u / uMax )), 0, 255 ) );
      uint32_t color = dbRGBA( r,r,r );
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
      uint32_t color, TexRef const & ref, int borderWidth, int tess )
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

   auto addVertex = [&] ( float x, float y )
   {
      float uCoord = 0.5f + x / w;
      float vCoord = 0.5f + y / h;
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

   uint32_t const INDEX_E = 0;
   uint32_t const INDEX_F = 1;
   uint32_t const INDEX_G = 2;
   uint32_t const INDEX_H = 3;
   uint32_t const INDEX_I = 4;
   uint32_t const INDEX_J = 5;
   uint32_t const INDEX_K = 6;
   uint32_t const INDEX_L = 7;
   uint32_t const INDEX_TOPLEFT = 8 + 0*tess;
   uint32_t const INDEX_TOPRIGHT = 8 + 1*tess;
   uint32_t const INDEX_BOTTOMRIGHT = 8 + 2*tess;
   uint32_t const INDEX_BOTTOMLEFT = 8 + 3*tess;
   o.addIndexedLine( INDEX_E, INDEX_F );
   o.addIndexedLine( INDEX_G, INDEX_H );
   o.addIndexedLine( INDEX_I, INDEX_J );
   o.addIndexedLine( INDEX_K, INDEX_L );
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
      addVertex( x, y );
   }
   // 2. topRight HIC (cw) quarter circle
   for ( int32_t i = tess - 1; i >= 0; --i )
   {
      float x = x2 + rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      float y = y1 - ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y );
   }
   // 3. bottomRight JKD (cw) quarter circle
   for ( uint32_t i = 0; i < tess; ++i )
   {
      float x = x2 + rx * lookUpTable[ i ].c;   // Vertices already in correct index order.
      float y = y2 + ry * lookUpTable[ i ].s;
      addVertex( x, y );
   }
   // 4. bottomLeft LEA (cw) quarter circle
   for ( int32_t i = tess - 1; i >= 0; --i )
   {
      float x = x1 - rx * lookUpTable[ i ].c;   // Add vertices in reverse (--i) order
      float y = y2 + ry * lookUpTable[ i ].s;   // to build indices in correct order.
      addVertex( x, y );
   }

   // add 4*tess CornerIndices:

   // 1. topLeft FGB (cw) quarter circle, glm::vec3 B( x1, 0.f, y2 );
   for ( uint32_t i = 0; i < tess; ++i )
   {
      uint32_t iF = INDEX_TOPLEFT + i;
      uint32_t iG = INDEX_TOPLEFT + i + 1;
      if ( i == 0 )             { iF = INDEX_F; } // StartTriangle
      else if ( i == tess - 1 ) { iG = INDEX_G; } // EndTriangle
      else                      {}                // MiddleTriangle
      o.addIndexedLine( iF, iG ); // all given in (cw)
   }

   // 2. topRight HIC (cw) quarter circle, glm::vec3 C( x2, 0.f, y2 );
   for ( uint32_t i = 0; i < tess; ++i )
   {
      uint32_t iH = INDEX_TOPRIGHT + i;
      uint32_t iI = INDEX_TOPRIGHT + i + 1;
      if ( i == 0 )             { iH = INDEX_H; } // StartTriangle
      else if ( i == tess - 1 ) { iI = INDEX_I; } // EndTriangle
      else                      {}                // MiddleTriangle
      o.addIndexedLine( iH,iI ); // all given in (cw)
   }

   // 3. bottomRight JKD (cw) quarter circle, glm::vec3 D( x2, 0.f, y1 );
   for ( uint32_t i = 0; i < tess; ++i )
   {
      uint32_t iJ = INDEX_BOTTOMRIGHT + i;
      uint32_t iK = INDEX_BOTTOMRIGHT + i + 1;
      if ( i == 0 )             { iJ = INDEX_J;  } // StartTriangle
      else if ( i == tess - 1 ) { iK = INDEX_K;  } // EndTriangle
      else                      {}                 // MiddleTriangle
      o.addIndexedLine( iJ, iK ); // all given in (cw)
   }

   // 4. bottomLeft LEA (cw) quarter circle, glm::vec3 A( x1, 0.f, y1 );
   for ( uint32_t i = 0; i < tess; ++i )
   {
      uint32_t iL = INDEX_BOTTOMLEFT + i;
      uint32_t iE = INDEX_BOTTOMLEFT + i + 1;
      if ( i == 0 )             { iL = INDEX_L; } // StartTriangle
      else if ( i == tess - 1 ) { iE = INDEX_E; } // EndTriangle
      else                      {}               // MiddleTriangle
      o.addIndexedLine( iL, iE ); // all given in (cw)
   }
}
*/




// =========================================================================
// Circle
// =========================================================================
/*
void
addCircle( SMeshBuffer & o, Recti const & pos,
      uint32_t outerColor, uint32_t innerColor, TexRef const & ref, uint32_t tess )
{
   float rx = 0.5f * pos.w;
   float ry = 0.5f * pos.h;

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   // Add center point
   uint32_t v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

   float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

   for ( uint32_t i = 0; i <= tess; ++i )
   {
      float const phi = angleStep * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );
//   for ( uint32_t j = 0; j <= tessOutwards; ++j )
//   {
      float x = rx * c; // * float( j ) * outwardX);
      float y = ry * s; // * float( j ) * outwardY);
      float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
   }

   for ( uint32_t i = 0; i < tess; ++i )
   {
//   for ( uint32_t j = 0; j < tessOutwards; ++j )
//   {
      uint32_t M = v;
      uint32_t A = v + 1 + i;
      uint32_t B = v + 2 + i;
      o.addIndexedTriangle( M, A, B );
//   }
   }
}

void addCircleOutline( SMeshBuffer & o, Recti const & pos,
      uint32_t color, TexRef const & ref, int borderWidth, uint32_t tess )
{
   float rx = 0.5f * pos.w;
   float ry = 0.5f * pos.h;

   std::vector< glm::vec2 > points;
   float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
   for ( uint32_t i = 0; i <= tess; ++i )
   {
      float const phi = angleStep * float( i );
      float const x = rx * ::cosf( phi );
      float const y = ry * ::sinf( phi );
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
*/

// =========================================================================
void
SMeshCircle::addXY( SMeshBuffer & o, glm::vec2 const & siz, uint32_t color, uint32_t tessRadial )
{
   addXY( o, siz, color, color, tessRadial );
}

void
SMeshCircle::addXY(
      SMeshBuffer & o,
      glm::vec2 const & siz,
      uint32_t innerColor,
      uint32_t outerColor,
      uint32_t tessRadial ) // bool forceIndices = false
{
   glm::vec2 radius = siz * 0.5f;

   //bool useIndices = o.getIndexCount() > 0 || forceIndices;

   // Add center point
   uint32_t v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

   float const angleStep = float( ( 2.0 * M_PI ) / double( tessRadial ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

   for ( uint32_t i = 0; i <= tessRadial; ++i )
   {
      float const phi = angleStep * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );

//         for ( uint32_t j = 0; j <= tessOutwards; ++j )
//         {
      float x = (radius.x * c); // * float( j ) * outwardX);
      float y = (radius.y * s); // * float( j ) * outwardY);
      float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
   }

   for ( uint32_t i = 0; i < tessRadial; ++i )
   {
//         for ( uint32_t j = 0; j < tessOutwards; ++j )
//         {
         uint32_t M = v;
         uint32_t A = v + 1 + i;
         uint32_t B = v + 1 + i + 1;
         o.addIndexedTriangle( M, A, B );
//         }
   }
}


void
SMeshCircle::addXZ( SMeshBuffer & o, glm::vec2 const & siz, uint32_t color, uint32_t tessRadial )
{
   addXZ( o, siz, color, color, tessRadial );
}

void
SMeshCircle::addXZ( SMeshBuffer & o, glm::vec2 const & siz, uint32_t innerColor, uint32_t outerColor, uint32_t tessRadial )
{
   glm::vec2 radius = siz * 0.5f;

   //bool useIndices = o.getIndexCount() > 0 || forceIndices;

   // Add center point
   uint32_t v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,1,0, innerColor, 0.5f, 0.5f ) );

   float const angleStep = float( ( 2.0 * M_PI ) / double( tessRadial ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

   for ( uint32_t i = 0; i <= tessRadial; ++i )
   {
      float const phi = angleStep * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );

//         for ( uint32_t j = 0; j <= tessOutwards; ++j )
//         {
      float x = (radius.x * c); // * float( j ) * outwardX);
      float z = (radius.y * s); // * float( j ) * outwardY);
      float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,0,z, 0,1,0, outerColor, u, v ) );
//         }
   }

   for ( uint32_t i = 0; i < tessRadial; ++i )
   {
//         for ( uint32_t j = 0; j < tessOutwards; ++j )
//         {
         uint32_t M = v;
         uint32_t A = v + 1 + i;
         uint32_t B = v + 1 + i + 1;
         o.addIndexedTriangle( M, B, A );
//         }
   }
}

// =========================================================================
void
Generic2DTriangleRing::addIndexedTriangles(
   float outer_x, float outer_y,
   float inner_x, float inner_y,
   int32_t tess, uint32_t innerColor, uint32_t outerColor,
   GET_VERTEXCOUNT const & getVertexCount,
   ADD_VERTEX const & addVertex,
   ADD_INDEX const & addIndex )
{
   uint32_t const vStart = getVertexCount();

   auto addIndexedQuad = [&] ( uint32_t a, uint32_t b, uint32_t c, uint32_t d )
   {
      addIndex( a ); addIndex( c ); addIndex( b ); // ABC (cw) = ACB (ccw)
      addIndex( a ); addIndex( d ); addIndex( c ); // ACD (cw) = ADC (ccw)
   };

   float const ix = 0.5f * inner_x;
   float const iy = 0.5f * inner_y;
   float const ox = 0.5f * outer_x; // a,b are radia, not sizes.
   float const oy = 0.5f * outer_y;
   float const phi_step = float( ( 2.0 * M_PI ) / double( tess ) ); // 0 to 2pi

   // inner ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      float const phi = phi_step * float( i );
      float const s = -::sinf( phi );
      float const c =  ::cosf( phi );
      addVertex( ix * s, iy * c, innerColor );
   }

   // outer ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      float const phi = phi_step * float( i );
      float const s = -::sinf( phi );
      float const c =  ::cosf( phi );
      addVertex( ox * s, oy * c, outerColor );
   }

   // ring quad indices
   for ( int32_t i = 0; i < tess; ++i )
   {
      uint32_t A = vStart + i;
      uint32_t B = vStart + i + tess + 1;
      uint32_t C = vStart + i + tess + 2;
      uint32_t D = vStart + i + 1;
      addIndexedQuad( B, A, D, C );
   }
}

// =========================================================================
void
SMeshRing::addXY( SMeshBuffer & o, float ow, float oh, float iw, float ih,
                  uint32_t innerColor, uint32_t outerColor, int32_t tess,
                  glm::vec3 const & pos )
{
   auto getVertexCount = [&] () { return o.getVertexCount(); };
   auto addIndex = [&] ( uint32_t index ) { o.addIndex( index ); };
   auto addVertex = [&] ( float px, float py, uint32_t color )
   {
      float x = pos.x + px;
      float y = pos.y + py;
      float z = pos.z;
      float u = 0.5f + px / ow;
      float v = 0.5f - py / oh;
      o.addVertex( S3DVertex( x,y,z, 0,0,1, color, u,v ) );
   };

   Generic2DTriangleRing::addIndexedTriangles( ow, oh, iw, ih, tess, innerColor, outerColor,
      getVertexCount, addVertex, addIndex );
}

void
SMeshRing::addXZ( SMeshBuffer & o, float ow, float oh, float iw, float ih,
                  uint32_t innerColor, uint32_t outerColor, int32_t tess,
                  glm::vec3 const & pos )
{
   iw *= 0.5f;
   ow *= 0.5f;
   ih *= 0.5f;
   oh *= 0.5f;

   uint32_t const vStart = o.getVertexCount();

   float const phi_step = float( ( 2.0 * M_PI ) / double( tess ) ); // 0 to 2pi

   // Inner ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      float const phi = phi_step * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );
      float const x = pos.x + iw * c;
      float const y = pos.y;
      float const z = pos.z + ih * s;
      float const u = 0.5f - 0.5f * c;
      float const v = 0.5f - 0.5f * s;
      o.addVertex( S3DVertex( x, y, z, 0,1,0, innerColor, u,v ) );
   }

   // Outer ring vertices
   for ( int32_t i = 0; i <= tess; ++i )
   {
      float const phi = phi_step * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );
      float const x = pos.x + ow * c;
      float const y = pos.y;
      float const z = pos.z + oh * s;
      float const u = 0.5f - 0.5f * c;
      float const v = 0.5f - 0.5f * s;
      o.addVertex( S3DVertex( x, y, z, 0,1,0, outerColor, u,v ) );
   }

   // Build quads
   for ( int32_t i = 0; i < tess; ++i )
   {
      uint32_t A = vStart + i;
      uint32_t B = A + tess + 1;
      uint32_t C = A + tess + 2;
      uint32_t D = A + 1;
      o.addIndexedQuad( B, A, D, C );
   }

}




// =========================================================================
/// @brief PolyHull
// =========================================================================
void
SMeshPolyHull::addLeftStripXY( SMeshBuffer & o,
                std::vector< glm::vec3 > const & points, float height,
                uint32_t color, glm::vec3 const & pos )
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
                std::vector< glm::vec3 > const & points, float height,
                uint32_t color, glm::vec3 const & pos )
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
               std::vector< glm::vec3 > const & points, float height,
               uint32_t color, glm::vec3 const & pos )
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
     float dx, float dy, float h,
     uint32_t color, int tessCirc, V3 const & pos )
{
   std::vector< V3 > points;
   points.reserve( tessCirc + 1 );
   float angleStart = 0.0f;     // from 0 degrees
   float angleEnd = float( 2.0 * M_PI ); // to 90 degrees
   float angleStep = ( angleEnd - angleStart) / float( tessCirc );
   float a = 0.5f * dx;
   float b = 0.5f * dy;

   for ( int i = 0; i <= tessCirc; ++i )
   {
      float phi = angleStart + angleStep * float( i );
      float x = a * ::cosf( phi );
      float y = b * ::sinf( phi );
      points.emplace_back( x, y, 0.0f );
   }

   SMeshPolyHull::addLeftStripXY( o, points, h, color, pos );
}

void
SMeshCylinderHull::addXZ( SMeshBuffer & o,
     float dx, float h, float dz,
     uint32_t color, int tessCirc, V3 const & pos )
{
   std::vector< V3 > points;
   points.reserve( tessCirc + 1 );
   float angleStart = 0.0f;     // from 0 degrees
   float angleEnd = float( 2.0 * M_PI ); // to 90 degrees
   float angleStep = ( angleEnd - angleStart) / float( tessCirc );
   float a = 0.5f * dx;
   float b = 0.5f * dz;

   for ( int i = 0; i <= tessCirc; ++i )
   {
      float phi = angleStart + angleStep * float( i );
      float x = a * ::cosf( phi );
      float z = b * ::sinf( phi );
      points.emplace_back( x, 0.0f, z );
   }

   SMeshPolyHull::addLeftStripXZ( o, points, h, color, pos );
}

// =========================================================================
void
SMeshCylinder::addXY( SMeshBuffer & o,
       float dx, float dy, float h,
       uint32_t color, uint32_t tessCirc )
{
   glm::vec3 circleOffset( 0, 0, 0.5f * h );
   glm::vec2 circleSize( dx, dy );
   uint32_t vStart = o.getVertexCount();
   SMeshCircle::addXY( o, circleSize, color, tessCirc );
   SMeshBufferTool::translateVertices( o, circleOffset, vStart );
   //vStart = o.getVertexCount();
   SMeshCylinderHull::addXY( o, dx,dy,h, color, tessCirc );
}

void
SMeshCylinder::addXZ( SMeshBuffer & o,
       float dx, float h, float dz,
       uint32_t color, uint32_t tessCirc )
{
   glm::vec3 circleOffset( 0, 0.5f * h, 0 );
   glm::vec2 circleSize( dx, dz );
   uint32_t vStart = o.getVertexCount();
   SMeshCircle::addXZ( o, circleSize, color, tessCirc );
   SMeshBufferTool::translateVertices( o, circleOffset, vStart );
   //vStart = o.getVertexCount();
   SMeshCylinderHull::addXZ( o, dx,h,dz, color, tessCirc );
}

float de_fma( float a, float b, float c ) // a * b + c
{
   return (a * b) + c;
}

float de_lerp( float a, float b, float t = 0.5f )
{
   return de_fma( b-a, t, a ); //return a + (b-a) * t;
}

float randomFloatInRange( float range_min = 0.0f, float range_max = 1.0f )
{
   return ((range_max - range_min) * (float(rand()%65536)/float(65536.0f))) + range_min;
}

void
SMeshCylinder::addXZ( SMeshBuffer & o, float bottom_dx, float bottom_dz,
       float top_dx, float top_dz, float cylinder_h,
       uint32_t color, uint32_t tessCircle, uint32_t tessHull, glm::vec3 const & offset )
{
   // BEGIN: Create lookup table for a unit-circle
   std::vector< glm::vec2 > unitCircle; //x,y,u,v for full radius
   unitCircle.reserve( tessCircle + 1 );
   float angleStart = 0.0f;     // from 0 degrees
   float angleEnd = float( 2.0 * M_PI ); // to 90 degrees
   float angleStep = ( angleEnd - angleStart) / float( tessCircle - 1 );
   for ( size_t i = 0; i < tessCircle; ++i )
   {
      float phi = angleStart + angleStep * float( i );
      float cx = ::cosf( phi );
      float sy = ::sinf( phi );
      unitCircle.emplace_back( cx, sy );
   }
   unitCircle.emplace_back( unitCircle[ 0 ] );

   // END: Create lookup table for a unit-circle

   // BEGIN: Vertex positions, to be reused.
   float top_a = 0.5f * top_dx;
   float top_b = 0.5f * top_dz;
   float bot_a = 0.5f * bottom_dx;
   float bot_b = 0.5f * bottom_dz;
   std::vector< glm::vec3 > a_pos; // reusable vertex pos
   a_pos.reserve( (tessHull + 1) * (tessCircle + 1) );
   for ( size_t j = 0; j < tessHull+1; ++j )
   {
      float t = float(j)/float(tessHull-1);
      for ( size_t i = 0; i < tessCircle; ++i )
      {
         glm::vec2 c = unitCircle[ i ];
         float r = randomFloatInRange(0.98,1.02);
         //float g = randomFloatInRange(0.98,1.02);
         float x = offset.x + (c.x * de_lerp( bot_a, top_a, t ) * r);
         float y = offset.y + (t * cylinder_h);
         float z = offset.z + (c.y * de_lerp( bot_b, top_b, t ) * r);
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
      float dx, // (outer) cylinder width
      float dz, // (outer) cylinder depth
      float hy, // (outer) cylinder height
      // Describes inner cylinder, rx,ry define the extrusion relative inside to dx,dy.
      // They define the inside border width and are not absolute values like dx,dy.
      float rx, // inside going border-width ( inside cylinder width cx = dx - rx )
      float rz, // inside going border-depth ( inside cylinder depth cz = dz - rz )
      uint32_t color, // one color to rule them all (vertices).
      int32_t tessCircle,// tesselation of top and bottom ring = num ring segments
      int32_t tessHeight )// tesselation of cylinder hull height ( y-direction )
{
   SMeshRing::addXZ( o, dx, dz, rx, rz, color, color, tessCircle );

}


// =========================================================================
// Sphere
// =========================================================================

glm::vec3
SMeshSphere::computeSphereNormal( float lon, float lat ) // in degrees
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

void
SMeshSphere::add( SMeshBuffer & o, glm::vec3 const & siz, uint32_t color,
                  uint32_t tessLon, uint32_t tessLat, glm::vec3 const & offset )
{
   o.setPrimitiveType( PrimitiveType::Triangles );
   if ( tessLat < 3 ) tessLat = 3;
   if ( tessLon < 3 ) tessLon = 3;

   glm::vec3 radius = siz * 0.5f;

   // u-step
   float lonStep = 360.0f / float( tessLon );
   float lonStart = 0.0f; // Greenwich
   //float lonEnd = 360.0f;
   float lonUV = 1.0f / float( tessLon );

   // v-step
   float latStep = -180.0f / float( tessLat );
   float latStart = 90.0f;
   //float latEnd = -90.0f;
   float latUV = 1.0f / float( tessLat );

   // Predict and reserve memory, indexed quads.
   o.moreVertices( 4 * tessLon * tessLat );
   o.moreIndices( 6 * tessLon * tessLat );

   auto addQuad = [&] ( int32_t ix, int32_t iy )
   {
      //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
      float lon1 = lonStart + lonStep * ix;
      float lon2 = lonStart + lonStep * ( ix + 1 );
      float lat1 = latStart + latStep * iy;
      float lat2 = latStart + latStep * ( iy + 1 );
      glm::vec3 na = computeSphereNormal( lon1, lat2 );
      glm::vec3 nb = computeSphereNormal( lon1, lat1 );
      glm::vec3 nc = computeSphereNormal( lon2, lat1 );
      glm::vec3 nd = computeSphereNormal( lon2, lat2 );
      glm::vec3 a = na * radius + offset;
      glm::vec3 b = nb * radius + offset;
      glm::vec3 c = nc * radius + offset;
      glm::vec3 d = nd * radius + offset;
      uint32_t colorA = color;
      uint32_t colorB = color;
      uint32_t colorC = color;
      uint32_t colorD = color;
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
addSphere( SMeshBuffer & o, glm::vec3 const & siz, glm::vec3 const & offset,
           uint32_t tessLon, uint32_t tessLat, uint32_t color )
{
   SMeshSphere::add( o, siz, color, tessLon, tessLat, offset );
}

// =========================================================================
// UpperHalfSphere
// =========================================================================

SMeshUpperHalfSphere::V3
SMeshUpperHalfSphere::computeSphereNormal( float lon, float lat ) // in degrees
{
   if ( lat >= 89.999f ) { return V3( 0,1,0 ); } // NorthPole
   else if ( lat <= -89.999f ) { return V3( 0,-1,0 ); } // SouthPole
   float lon_rad = lon * Math::DEG2RAD;
   float lat_rad = lat * Math::DEG2RAD;
   float x = ::cosf( lon_rad ) * ::cosf( lat_rad );
   float y =                     ::sinf( lat_rad );
   float z = ::sinf( lon_rad ) * ::cosf( lat_rad );
   return glm::vec3( x,y,z );
}

void
SMeshUpperHalfSphere::add( SMeshBuffer & o, V3 const & siz, uint32_t color,
                           uint32_t tessLon, uint32_t tessLat, V3 const & offset )
{
   o.setPrimitiveType( PrimitiveType::Triangles );
   if ( tessLat < 3 ) tessLat = 3;
   if ( tessLon < 3 ) tessLon = 3;

   // x-step
   float lonStep = 360.0f / float( tessLon );
   float lonStart = 0.0f; // Greenwich
   //float lonEnd = 360.0f;

   // y-step
   float latStep = -180.0f / float( tessLat );
   float latStart = 90.0f;
   //float latEnd = -90.0f;

   // tex-coord-steps
   float du = 1.0f / float( tessLon );
   float dv = 1.0f / float( tessLat );

   // Predict and reserve memory, indexed quads.
   o.moreVertices( 4 * tessLon * tessLat );
   o.moreIndices( 6 * tessLon * tessLat );

   auto addQuad = [&] ( int32_t ix, int32_t iy )
   {
      //DE_DEBUG("addQuad( i=",ix,", j=", iy, " )")
      glm::vec3 radius = siz * 0.5f;
      float lon1 = lonStart + lonStep * ix;
      float lon2 = lonStart + lonStep * ( ix + 1 );
      float lat1 = latStart + latStep * iy;
      float lat2 = latStart + latStep * ( iy + 1 );
      glm::vec3 na = computeSphereNormal( lon1, lat2 );
      glm::vec3 nb = computeSphereNormal( lon1, lat1 );
      glm::vec3 nc = computeSphereNormal( lon2, lat1 );
      glm::vec3 nd = computeSphereNormal( lon2, lat2 );
      glm::vec3 a = na * radius + offset;
      glm::vec3 b = nb * radius + offset;
      glm::vec3 c = nc * radius + offset;
      glm::vec3 d = nd * radius + offset;
      uint32_t colorA = color;
      uint32_t colorB = color;
      uint32_t colorC = color;
      uint32_t colorD = color;
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

/*
// =========================================================================
struct SMeshUpperHalfSphere
// =========================================================================
{
   // @brief Benni book example 2021
   typedef glm::vec3 V3;
   typedef uint32_t u32;

   static void
   add( SMeshBuffer & o, V3 siz, V3 pos, u32 tessCircle = 15,  u32 tessRows = 15, u32 color = 0xAFFFFFFF )
   {
      u32 v = o.getVertexCount();

      // TopCenter vertex
      o.addVertex( S3DVertex( 0,ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[v+1 to v+tessCircle*tessRows ] = other vertices
      float const sxz = 1.f / (float)tessCircle;
      float const sy = 1.f / (float)tessRows;

      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, float(0.0), float( 0.5*M_PI ) );

      for (uint32_t j=0; j<tessRows; ++j)
      {
         for (uint32_t i=0; i<tessCircle; ++i)
         {
            float const sinA = sincosA[i].s;
            float const cosA = sincosA[i].c;
            float const sinB = sincosB[j].s;
            float const cosB = sincosB[j].c;
            float const nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            float const ny =  sinB;					// equals the normal at point radius*(x,y,z)
            float const nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            float const u = 0.5f-0.5f*cosB * sinA; // not finished
            float const v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( uint32_t j = 0; j < tessRows; ++j )
      {
         for ( uint32_t i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               uint32_t i0 = v + 1 + (j)*tessCircle + (i);    // A
               uint32_t i1 = v + 1 + (j)*tessCircle + (i+1);  // B
               uint32_t i2 = v;    // C - top center
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
               uint32_t i0 = v + 1 + (j)*tessCircle + (i);	// A
               uint32_t i1 = v + 1 + (j+1)*tessCircle + (i);	// B
               uint32_t i2 = v + 1 + (j+1)*tessCircle + (i+1);// C
               uint32_t i3 = v + 1 + (j)*tessCircle + (i+1);	// D
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
      uint32_t tessCircle = 15,
      uint32_t tessRows = 15,
      uint32_t color = 0xAFFFFFFF )
   {
      uint32_t v = o.getVertexCount();

      // TopCenter vertex
      o.addVertex( S3DVertex( 0,ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[v+1 to v+tessCircle*tessRows ] = other vertices
      float const sxz = 1.f / (float)tessCircle;
      float const sy = 1.f / (float)tessRows;

      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, float(0.0), float( 0.5*M_PI ) );

      for (uint32_t j=0; j<tessRows; ++j)
      {
         for (uint32_t i=0; i<tessCircle; ++i)
         {
            float const sinA = sincosA[i].s;
            float const cosA = sincosA[i].c;
            float const sinB = sincosB[j].s;
            float const cosB = sincosB[j].c;
            float const nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            float const ny =  sinB;					// equals the normal at point radius*(x,y,z)
            float const nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            float const u = 0.5f-0.5f*cosB * sinA; // not finished
            float const v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( uint32_t j = 0; j < tessRows; ++j )
      {
         for ( uint32_t i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               uint32_t i0 = v + 1 + (j)*tessCircle + (i);    // A
               uint32_t i1 = v + 1 + (j)*tessCircle + (i+1);  // B
               uint32_t i2 = v;    // C - top center
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
               uint32_t i0 = v + 1 + (j)*tessCircle + (i);	// A
               uint32_t i1 = v + 1 + (j+1)*tessCircle + (i);	// B
               uint32_t i2 = v + 1 + (j+1)*tessCircle + (i+1);// C
               uint32_t i3 = v + 1 + (j)*tessCircle + (i+1);	// D
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
         float rx = 50, float ry = 50, float rz = 50,
         uint32_t tessCircle = 15, uint32_t tessRows = 15,
         float x = 0.f, float y = 0.f, float z = 0.f,
         uint32_t color = 0xAFFFFFFF )
   {
      uint32_t const vStart = o.getVertexCount();

      // Vertices[vStart] = top center vertex
      o.addVertex( S3DVertex( 0,-ry,0, 0,1,0, color, 0.5f,0.5f ) );

      // Vertices[vStart+1 to vStart+tessCircle*tessRows ] = other vertices
      SinCosTablef sincosA(tessCircle);
      SinCosTablef sincosB(tessRows, float(0.0), float( 0.5*M_PI ) );

      for (uint32_t j=0; j<tessRows; ++j)
      {
         for (uint32_t i=0; i<tessCircle; ++i)
         {
            float const sinA = sincosA[i].s;
            float const cosA = sincosA[i].c;
            float const sinB = sincosB[j].s;
            float const cosB = sincosB[j].c;
            float const nx = -sinA * cosB;				// equals the normal at point radius*(x,y,z)
            float const ny = -sinB;					// equals the normal at point radius*(x,y,z)
            float const nz =  cosA * cosB;				// equals the normal at point radius*(x,y,z)
            float const u = 0.5f-0.5f*cosB * sinA; // not finished
            float const v = 0.5f-0.5f*cosB * cosA; // not finished
            o.addVertex( S3DVertex( rx*nx,ry*ny,rz*nz, nx,ny,nz, color, u,v ) );
         }
      }

      // push indices
      for ( uint32_t j = 0; j < tessRows; ++j )
      {
         for ( uint32_t i = 0; i < tessCircle; ++i )
         {
            // top row contains just triangles, no quads
            if ( j == tessRows - 1 )
            {
               uint32_t i0 = vStart + 1 + (j)*tessCircle + (i);    // A
               uint32_t i1 = vStart + 1 + (j)*tessCircle + (i+1);  // B
               uint32_t i2 = vStart;    // C - top center
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
               uint32_t i0 = vStart + 1 + (j)*tessCircle + (i);	// A
               uint32_t i1 = vStart + 1 + (j+1)*tessCircle + (i);	// B
               uint32_t i2 = vStart + 1 + (j+1)*tessCircle + (i+1);// C
               uint32_t i3 = vStart + 1 + (j)*tessCircle + (i+1);	// D
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
*/

} // end namespace gpu.
} // end namespace de.








/*




   static float32_t
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

