#include <de/gpu/renderer/ScreenRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de/gpu/smesh/SMeshLibrary.h>
#include <de_opengl.h>

namespace de {
namespace gpu {
namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

ScreenRenderer::ScreenRenderer()
   : m_driver( nullptr )
   , m_zIndex( CONST_Z_INIT )
   , m_isDirty( true )
{
}

int ScreenRenderer::getScreenWidth() const { return m_driver->getScreenWidth(); }
int ScreenRenderer::getScreenHeight() const { return m_driver->getScreenHeight(); }

void ScreenRenderer::draw2D( const SMeshBuffer& m )
{
    setMaterial( m.material );

    m.draw();

    unsetMaterial( m.material );
}

void ScreenRenderer::draw2DHexagon( const Recti& pos, const uint32_t color, const TexRef& tex )
{
    const float x0 = pos.x;
    const float x1 = pos.x + pos.w/2;
    const float x2 = pos.x + pos.w;

    const float y0 = pos.y;
    const float y1 = pos.y + pos.h/4;
    const float y2 = pos.y + 3*pos.h/4;
    const float y3 = pos.y + pos.h;

    const S3DVertex A( x1,y3,-1, 0,0,-1, color, 0.5f,  1.0f );
    const S3DVertex B( x0,y2,-1, 0,0,-1, color, 0.0f, 0.75f );
    const S3DVertex C( x0,y1,-1, 0,0,-1, color, 0.0f, 0.25f );
    const S3DVertex D( x1,y0,-1, 0,0,-1, color, 0.5f,  0.0f );
    const S3DVertex E( x2,y1,-1, 0,0,-1, color, 1.0f, 0.25f );
    const S3DVertex F( x2,y2,-1, 0,0,-1, color, 1.0f, 0.75f );

    SMeshBuffer m( PrimitiveType::Triangles );
    m.addTriangle( C,D,E ); // Top
    m.addQuad( B,C,E,F ); // Body
    m.addTriangle( A,B,F ); // Bottom
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );

    draw2D( m );

    m.destroy();
}

void ScreenRenderer::draw2DCircle( const Recti& pos, const uint32_t color, const TexRef& tex, int tess )
{
    SMeshBuffer m( PrimitiveType::Triangles );

    // Add center point
    const float rx = 0.5f * pos.w;
    const float ry = 0.5f * pos.h;
    const float cx = float( pos.x ) + 0.5f*pos.w;
    const float cy = float( pos.y ) + 0.5f*pos.h;
    const S3DVertex M( cx,cy,0, 0,0,-1, color, .5f, .5f );
    m.addVertex( M );

    const float angleStep = float( ( 2.0 * M_PI ) / double( tess ) );

    for ( uint32_t i = 0; i <= tess; ++i )
    {
        const float phi = angleStep * float( i );
        const float s = ::sinf( phi );
        const float c = ::cosf( phi );
        const float x = cx + (rx * c);
        const float y = cy + (ry * s);
        const float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
        const float v = std::clamp( 0.5f + 0.5f * s, 0.0001f, 0.9999f );
        const S3DVertex P( x,y,0, 0,0,-1, color, u, v );
        m.addVertex( P );
    }

    for ( uint32_t i = 0; i < tess; ++i )
    {
        const uint32_t c = 0;
        const uint32_t a = 1 + i;
        const uint32_t b = 1 + i + 1;
        m.addIndexedTriangle( c, a, b );
    }

    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );

    draw2D( m );

    m.destroy();
}

void
ScreenRenderer::draw2DLine( int x1, int y1, int x2, int y2,
                            uint32_t c1, uint32_t c2 )
{
    SMeshBuffer m( PrimitiveType::Lines );
    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::disabled() );
    if (c1 == c2)
    {
        const auto a1 = dbRGBA_A(c1) < 255;
        if (a1) { m.setBlend( Blend::alphaBlend() ); }
    }
    else
    {
        const auto a1 = dbRGBA_A(c1) < 255;
        const auto a2 = dbRGBA_A(c2) < 255;
        if (a1 || a2) { m.setBlend( Blend::alphaBlend() ); }
    }

    m.addVertex( S3DVertex(x1,y1,0, 0,0,1, c1, 0,0) );
    m.addVertex( S3DVertex(x2,y2,0, 0,0,1, c2, 1,0) );
    m.upload();

    draw2D( m );

    m.destroy();
}

void
ScreenRenderer::draw2DRect( const Recti& pos, const uint32_t color, const TexRef& tex )
{
    SMeshBuffer m( PrimitiveType::Triangles );
    const int x1 = pos.x1();
    const int x2 = pos.x2();
    const int y1 = pos.y1();
    const int y2 = pos.y2();

    const S3DVertex A( x1, y1, 0, 0,0,1, color, 0, 0 );
    const S3DVertex B( x2, y1, 0, 0,0,1, color, 1, 0 );
    const S3DVertex C( x2, y2, 0, 0,0,1, color, 1, 1 );
    const S3DVertex D( x1, y2, 0, 0,0,1, color, 0, 1 );

    m.addVertex( A );
    m.addVertex( B );
    m.addVertex( C );
    m.addVertex( A );
    m.addVertex( C );
    m.addVertex( D );
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );

    draw2D( m );

    m.destroy();
}

void
ScreenRenderer::draw2DRectLine( const Recti& pos, const uint32_t color, const TexRef& tex )
{
    SMeshBuffer m( PrimitiveType::Lines );
    const int x1 = pos.x1();
    const int x2 = pos.x2();
    const int y1 = pos.y1();
    const int y2 = pos.y2();

    const S3DVertex A( x1, y1, 0, 0,0,1, color, 0, 0 );
    const S3DVertex B( x2, y1, 0, 0,0,1, color, 1, 0 );
    const S3DVertex C( x2, y2, 0, 0,0,1, color, 1, 1 );
    const S3DVertex D( x1, y2, 0, 0,0,1, color, 0, 1 );

    m.addVertex( A ); m.addVertex( B ); // Line AB
    m.addVertex( B ); m.addVertex( C ); // Line BC
    m.addVertex( C ); m.addVertex( D ); // Line CD
    m.addVertex( D ); m.addVertex( A ); // Line DA
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );

    draw2D( m );

    m.destroy();
}

void
ScreenRenderer::draw2DRoundRect( const Recti& pos, const glm::ivec2& r, const uint32_t color, const TexRef& tex, int tess )
{
    SMeshBuffer m( PrimitiveType::Triangles );
    SMeshRoundRect::addXY( m,
                           glm::vec3(pos.x + 0.5f*pos.w, pos.y + 0.5f*pos.h, 0),
                           pos.w,
                           pos.h,
                           r.x,
                           r.y,
                           tess,
                           color, color);
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );

    draw2D( m );

    m.destroy();
}

void
ScreenRenderer::draw2DRoundRectLine( const Recti& pos, const glm::ivec2& r, const uint32_t color, const TexRef& tex, int tess )
{
    draw2DRectLine(pos,color,tex);
    /*
    SMeshBuffer m( PrimitiveType::Triangles );
    SMeshRoundRect::addXY( m, glm::vec3(pos.x(), pos.y(), 0), pos.w(), pos.h(), r.x, r.y, 23, color, color);

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );


    draw2D( m );

    m.destroy();
    */
}

bool
ScreenRenderer::setMaterial( SMaterial const & mtl, const Recti& pos )
{
    if ( !m_driver ) { DE_ERROR("No driver") return false; }

    const std::string name = createShaderName( mtl );

    auto shader = m_driver->getShader( name );
    if (!shader)
    {
        shader = m_driver->createShader( name, createVS( mtl ), createFS( mtl ) );
        if (!shader)
        {
            DE_ERROR("No shader created name(", name, ")")
            return false;
        }
    }

    if ( !m_driver->useShader( shader ) )
    {
        DE_ERROR("No bound shader ",name )
        return false;
    }

    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    shader->setVec2f( "u_screenSize", glm::vec2( w, h ) );

    const TexRef& diffuseMap = mtl.getDiffuseMap();
    if (diffuseMap.tex)
    {
        m_driver->useTexture(diffuseMap.tex,0);
        shader->setInt( "u_tex", 0 );
        shader->setVec4f( "u_texTransform", diffuseMap.coords );
    }

    State state = mtl.state;
    state.depth = Depth::disabled();
    state.culling = Culling::disabled();
    m_driver->setState( state );

    return true;
}

void
ScreenRenderer::unsetMaterial( SMaterial const & mtl )
{
    const TexRef& diffuseMap = mtl.getDiffuseMap();
    if (diffuseMap.tex)
    {
        m_driver->useTexture(0,0);
    }
}

std::string
ScreenRenderer::createShaderName( SMaterial const & material )
{
   std::stringstream s; s << "s2d";
   if ( material.hasDiffuseMap() ) s << "+Td";
   if ( material.CloudTransparent ) s << "+cloud";
   return s.str();
}

std::string
ScreenRenderer::createVS( SMaterial const & material )
{
    std::stringstream vs; vs <<
    R"(
        precision highp float;
      //precision mediump float;

        layout(location = 0) in vec3 a_pos;
        layout(location = 1) in vec3 a_normal;
        layout(location = 2) in lowp vec4 a_color;
        layout(location = 3) in vec2 a_tex;

        uniform vec2 u_screenSize;

      //uniform vec2 u_pixelOffset;
      //uniform vec2 u_pixelScale;

      //out vec3 v_pos;
      //out vec3 v_normal;
        out vec4 v_color;
        out vec2 v_tex;

        void main()
        {
          //vec2 dim = u_pixelScale * a_pos.xy;
          //vec2 pos = u_pixelOffset + dim;
            vec2 ndc = (2.0 * vec2( a_pos.x, u_screenSize.y - 1.0 - a_pos.y ) / u_screenSize ) - 1.0;
          //v_pos = pos;
          //v_normal = a_normal;
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_tex = a_tex;
          //vec2 ndc = (2.0 * vec2( pos.x, u_screenSize.y - 1.0 - pos.y ) / u_screenSize ) - 1.0;
            gl_Position = vec4( ndc, 0.0, 1.0 );
        }
    )";
    return vs.str();
}

std::string
ScreenRenderer::createFS( SMaterial const & material )
{
    std::stringstream fs;
    fs << R"(
        precision highp float;
      //precision mediump float;

        layout(location = 0) out vec4 fragColor;
    )";

    if ( material.hasDiffuseMap() )
    {
        fs << R"(
        uniform sampler2D u_tex;
        uniform vec4 u_texTransform;
        )";
    }

    fs << R"(
      //in vec3 v_pos;
      //in vec3 v_normal;
        in vec4 v_color;
        in vec2 v_tex;

        void main()
        {
    )";

    if ( material.hasDiffuseMap() )
    {
        fs << R"(
            vec2 atlasPos = u_texTransform.xy;
            vec2 atlasDim = u_texTransform.zw;
            vec2 atlasUV = atlasDim * fract( v_tex ) + atlasPos;
            vec4 Td = texture( u_tex, atlasUV );
        )";

        if ( material.CloudTransparent )
        {
            fs << R"(
            fragColor = v_color * vec4(1,1,1,Td.r);
            )";
        }
        else
        {
            fs << R"(
            fragColor = v_color * Td;
            )";
        }
    }
    else
    {
        fs << R"(
            fragColor = v_color;
        )";
    }

    fs << R"(
            // Final clamp
            fragColor = clamp( fragColor, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    return fs.str();
}

} // end namespace gpu.
} // end namespace de.

